#ifndef DRAW_BOX_H
#define DRAW_BOX_H

#include <iostream>
#include <Eigen/Core>

void get_bounding_box(
	const Eigen::MatrixXd & V,
    Eigen::MatrixXd & V_box,
    Eigen::MatrixXi & E_box);
#endif