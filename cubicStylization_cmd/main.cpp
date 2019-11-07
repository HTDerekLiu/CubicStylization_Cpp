#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/snap_points.h>

#include <cube_style_data.h>
#include <cube_style_precomputation.h>
#include <cube_style_single_iteration.h>
#include <normalize_unitbox.h>

#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>

#ifndef MESH_PATH
#define MESH_PATH "../../meshes/"
#endif

#ifndef OUTPUT_PATH
#define OUTPUT_PATH "../"
#endif

// to run the code, type "./cubeStyle_bin [meshName] [lambda]"
int main(int argc, char *argv[])
{
	using namespace Eigen;
	using namespace std;

    // load mesh and lambda
	MatrixXd V, U;
	MatrixXi F;
    cube_style_data data;
    string meshName;
	{   
        if (argc == 1)
        {
            meshName = "spot.obj"; // default mesh
            data.lambda = 0.2; // default lambda
        }
        else if (argc == 2)
        {
            meshName = argv[1];
            data.lambda = 0.2; // default lambda
        }
        else
        {
            meshName = argv[1];
            data.lambda = stod(argv[2]);
        }
		string file = MESH_PATH + meshName;
		igl::readOBJ(file, V, F);
        normalize_unitbox(V);
        RowVector3d meanV = V.colwise().mean();
        V = V.rowwise() - meanV;
        U = V;
	}

    // set a constrained point F(0,0)
    {
        data.bc.resize(1,3);
        data.bc << V.row(F(0,0));

        data.b.resize(1);
        data.b << F(0,0);
    }

    // precomputation ARAP and initialize ADMM parameters
    cube_style_precomputation(V,F,data);

    // cubic stylization 
    int maxIter = 1000;
    double stopReldV = 1e-3; // stopping criteria for relative displacement
    for (int iter=0; iter<maxIter; iter++)
    {
        cout << "iteration: " << iter << endl;
        cube_style_single_iteration(V,U,data);
        if (data.reldV < stopReldV) break;
    }
    
    // write output mesh
    {
        string outputName = "cubic_";
        outputName.append(meshName);
        string outputFile = OUTPUT_PATH + outputName;
        igl::writeOBJ(outputFile,U,F);

        string inputName = "input_";
        inputName.append(meshName);
        string inputFile = OUTPUT_PATH + inputName;
        igl::writeOBJ(inputFile,V,F);
    }

    return 0;
}