#include "get_bounding_box.h"

void get_bounding_box(
	const Eigen::MatrixXd & V,
    Eigen::MatrixXd & V_box,
    Eigen::MatrixXi & E_box)
{
    using namespace Eigen;
    Vector3d m = V.colwise().minCoeff();
    Vector3d M = V.colwise().maxCoeff();
    Vector3d meanV = V.colwise().mean();
    double r = (M-m).maxCoeff() * 0.5;

    // V_box.setZero(8,3);
    // V_box <<
    // m(0), m(1), m(2),
    // M(0), m(1), m(2),
    // M(0), M(1), m(2),
    // m(0), M(1), m(2),
    // m(0), m(1), M(2),
    // M(0), m(1), M(2),
    // M(0), M(1), M(2),
    // m(0), M(1), M(2);

    V_box.setZero(8,3);
    V_box <<
    meanV(0)-r, meanV(1)-r, meanV(2)-r,
    meanV(0)+r, meanV(1)-r, meanV(2)-r,
    meanV(0)+r, meanV(1)+r, meanV(2)-r,
    meanV(0)-r, meanV(1)+r, meanV(2)-r,
    meanV(0)-r, meanV(1)-r, meanV(2)+r,
    meanV(0)+r, meanV(1)-r, meanV(2)+r,
    meanV(0)+r, meanV(1)+r, meanV(2)+r,
    meanV(0)-r, meanV(1)+r, meanV(2)+r;


    E_box.setZero(12,2);
    E_box <<
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    7 ,3;
}