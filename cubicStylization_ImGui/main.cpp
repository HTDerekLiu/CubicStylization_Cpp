#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/snap_points.h>

#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>

#include <normalize_unitbox.h>
#include <cube_style_data.h>
#include <cube_style_precomputation.h>
#include "cube_style_single_iteration.h"
#include <get_bounding_box.h>

#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>

#ifndef MESH_PATH
#define MESH_PATH "../../meshes/"
#endif

#ifndef OUTPUT_PATH
#define OUTPUT_PATH "../../results/"
#endif

// state of the mode
struct Mode
{
    Eigen::MatrixXd CV; // point constraint
    bool place_constraints = true;
    int numCV = 0;
    Eigen::MatrixXi CV_row_col; 
} state;

// to run the code, type "./cubeStyle_bin [meshName]"
int main(int argc, char *argv[])
{
	using namespace Eigen;
	using namespace std;

    // load mesh
	MatrixXd V, U, VO;
	MatrixXi F;
	{   
        string meshName;
        if (argc == 1)
            meshName = "spot.obj"; // default mesh
        else
            meshName = argv[1];
		string file = MESH_PATH + meshName;
		igl::readOBJ(file, V, F);
		normalize_unitbox(V);
		V = V.array() - 0.5;
        U = V;
        VO = V;
	}

    // prepare data for cube stylization
    cube_style_data data;
    data.lambda = 2e-1;

    // initialize viewer
    igl::opengl::glfw::Viewer viewer;
    igl::opengl::glfw::imgui::ImGuiMenu menu;
	viewer.plugins.push_back(&menu);

    // rotation parameters
    double theta_x = 10.0; 
	double theta_y = 10.0; 
	double theta_z = 10.0; 
	Matrix3d Rx, iRx, Ry, iRy, Rz, iRz;
    {
        Rx << 
            1., 0., 0.,
            0., cos(theta_x/180.*3.14), -sin(theta_x/180.*3.14),
            0., sin(theta_x/180.*3.14), cos(theta_x/180.*3.14);
        Ry <<
            cos(theta_y/180.*3.14), 0., sin(theta_y/180.*3.14),
            0., 1., 0.,
            -sin(theta_y/180.*3.14), 0, cos(theta_y/180.*3.14);
        Rz <<
            cos(theta_z/180.*3.14), -sin(theta_z/180.*3.14), 0.,
            sin(theta_z/180.*3.14), cos(theta_z/180.*3.14), 0.,
            0., 0., 1;
    }

    // draw additional windows
    // string outputName = "output.obj";
    char outputName[128] = "output";
	menu.callback_draw_viewer_window = [](){};
	menu.callback_draw_custom_window = [&]()
	{
		// Define next window position + size
		{
			ImGui::SetNextWindowPos(ImVec2(0.f * menu.menu_scaling(), 0), ImGuiSetCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(220, 400), ImGuiSetCond_FirstUseEver);
			ImGui::Begin(
				"Cubic Stylization", nullptr,
				ImGuiWindowFlags_NoSavedSettings
			);
		}
        // Expose the same variable directly
        {
            // How to use
            ImGui::Text("Instructions");
            ImGui::BulletText("[click]  place constrained points");
            ImGui::BulletText("[space]  change mode");
            ImGui::BulletText("R          reset ");
            ImGui::BulletText("</>      decrease/increase lambda");
            ImGui::BulletText("Q/W    rotate x-axis");
            ImGui::BulletText("A/S      rotate y-axis");
            ImGui::BulletText("Z/X      rotate z-axis");
            ImGui::BulletText("L      show edges");
            ImGui::Text(" ");
        }
        {
            ImGui::PushItemWidth(-80);
            ImGui::DragScalar("lambda", ImGuiDataType_Double, &data.lambda, 2e-1, 0, 0, "%.1e");
            ImGui::PopItemWidth();
        }
        {
            // output file name
            ImGui::InputText(".obj", outputName, IM_ARRAYSIZE(outputName));
        }
        if (ImGui::Button("save output mesh", ImVec2(-1,0)))
		{
			string file = OUTPUT_PATH;
            file.append(outputName);
            file.append(".obj");
			igl::writeOBJ(file, U, F);
		}
        ImGui::End();
    };

    // recompute state.CV function
    int maxCV = 100;
    state.CV_row_col.resize(maxCV,2); // assume nor more than 100 constraints
    const auto & resetCV = [&]()
    {
        for (int ii=0; ii<state.numCV; ii++)
        {
            int fid = state.CV_row_col(ii,0);
            int c   = state.CV_row_col(ii,1);
            RowVector3d new_c = V.row(F(fid,c));
            state.CV.row(ii) = new_c;
        }
    };

    // draw function
    const auto & draw = [&]()
    {
        const Eigen::RowVector3d blue(149.0/255, 217.0/255, 244.0/255);
        const Eigen::RowVector3d red(250.0/255, 114.0/255, 104.0/255);
        const Eigen::RowVector3d gray(200.0/255, 200.0/255, 200.0/255);

        if(state.place_constraints)
        {
            viewer.data().clear();
            viewer.data().face_based = true;
            viewer.data().set_mesh(V,F);
            viewer.data().set_colors(gray);
            viewer.data().set_points(state.CV, red);

            // draw bounding box
            MatrixXd V_box;
            MatrixXi E_box;
            get_bounding_box(V, V_box, E_box);
            viewer.data().add_points(V_box, red);
            for (unsigned i=0;i<E_box.rows(); ++i)
                viewer.data().add_edges(V_box.row(E_box(i,0)),V_box.row(E_box(i,1)),red);
        }
        else
        {
            cube_style_single_iteration(V,U,data);
            viewer.data().clear();
            viewer.data().face_based = true;
            viewer.data().set_mesh(U,F);
            viewer.data().set_colors(blue);
            viewer.data().set_points(state.CV, red);

            // draw bounding box
            MatrixXd V_box;
            MatrixXi E_box;
            get_bounding_box(U, V_box, E_box);
            viewer.data().add_points(V_box, red);
            for (unsigned i=0;i<E_box.rows(); ++i)
                viewer.data().add_edges(V_box.row(E_box(i,0)),V_box.row(E_box(i,1)),red);
        }
    };

    // when key pressed do 
    viewer.callback_key_pressed = [&](igl::opengl::glfw::Viewer &, unsigned int key, int mod)
    {
        switch(key)
        {
            case '>':
            case '.':
            {
                data.lambda += 0.02;
                break;
            }
            case '<':
            case ',':
            {
                data.lambda -= 0.02;
                break;
            }
            case 'R':
            case 'r':
            {
                if (state.place_constraints)
                {
                    state.CV = MatrixXd();
                    state.CV_row_col = MatrixXi();
                    state.CV_row_col.resize(maxCV,2);
                    state.numCV = 0;
                    V = VO;
                }
                break;
            }
            case 'Q':
            case 'q':
            {
                U = (Rx * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rx * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case 'W':
            case 'w':
            {
                U = (Rx.transpose() * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rx.transpose() * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case 'A':
            case 'a':
            {
                U = (Ry * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rz * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case 'S':
            case 's':
            {
                U = (Ry.transpose() * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rx.transpose() * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case 'Z':
            case 'z':
            {
                U = (Rz * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rz * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case 'X':
            case 'x':
            {
                U = (Rz.transpose() * U.transpose()).transpose();
                if (state.place_constraints)
                {
                    V = (Rz.transpose() * V.transpose()).transpose();
                    resetCV();
                    draw();
                }
                break;
            }
            case ' ':
            {
                state.place_constraints = !state.place_constraints; // switch mode
                if (state.place_constraints)
                {
                    resetCV();
                    draw();
                }
                if (!state.place_constraints && state.CV.rows()>0)
                {
                    // set constrained points
                    igl::snap_points(state.CV, V, data.b);
                    data.bc.setZero(data.b.size(), 3);
                    for (int ii=0; ii<data.b.size(); ii++)
                        data.bc.row(ii) = V.row(data.b(ii));

                    // pre computation
                    U = V;
                    cube_style_precomputation(V,F,data);
                }
                else if(!state.place_constraints && state.CV.rows()==0) 
                {
                    // if not constraint points, then set the F(0,0) to be the contrained point
                    state.CV = MatrixXd();
                    state.CV.resize(1,3);
                    RowVector3d new_c = V.row(F(0,0));
                    state.CV.row(0) << new_c;
                    state.CV_row_col = MatrixXi();
                    state.CV_row_col.resize(maxCV,2);
                    state.CV_row_col.row(0) << 0, 0;
                    state.numCV = 1;

                    // set constrained points
                    igl::snap_points(state.CV, V, data.b);
                    data.bc.setZero(data.b.size(), 3);
                    for (int ii=0; ii<data.b.size(); ii++)
                        data.bc.row(ii) = V.row(data.b(ii));

                    // pre computation
                    U = V;
                    cube_style_precomputation(V,F,data);
                }
                break;
            }
            default:
                return false;
        }
        draw();
        return true;
    };

    // when click mouse
    viewer.callback_mouse_down = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        if(state.place_constraints)
        {
            // Find closest point on mesh to mouse position
            double x = viewer.current_mouse_x;
            double y = viewer.core().viewport(3) - viewer.current_mouse_y;
            int fid;
            Vector3f bary;
            if(igl::unproject_onto_mesh(Vector2f(x,y), viewer.core().view, viewer.core().proj, viewer.core().viewport, V, F, fid, bary))
            {
                long c;
                bary.maxCoeff(&c);
                
                state.CV_row_col.row(state.numCV) << fid, c;
                state.numCV++;

                RowVector3d new_c = V.row(F(fid,c));
                if(state.CV.size()==0 || (state.CV.rowwise()-new_c).rowwise().norm().minCoeff() > 0)
                {
                    state.CV.conservativeResize(state.CV.rows()+1,3);
                    state.CV.row(state.CV.rows()-1) = new_c;
                    draw();
                    return true;
                }
            }
        }
        return false;
    };

    // default mode: keep drawing the current mesh
    viewer.callback_pre_draw = [&](igl::opengl::glfw::Viewer &)->bool
    {
        if(viewer.core().is_animating &&!state.place_constraints)
            draw();
        return false;
    };

    // initialize the scene
    {
        viewer.data().set_mesh(V,F);
        viewer.data().show_lines = false;
        viewer.core().is_animating = true;
        viewer.data().face_based = true;
        Vector4f backColor;
        backColor << 208/255., 237/255., 227/255., 1.;
        viewer.core().background_color = backColor;
        draw();
        viewer.launch();
    }

}