#ifndef CUBE_STYLE_SINGLE_ITERATION_H
#define CUBE_STYLE_SINGLE_ITERATION_H

#include <iostream>
#include <ctime>
#include <vector>
#include <Eigen/Core>

// include libigl functions
#include <igl/columnize.h>
#include <igl/slice.h>
#include <igl/min_quad_with_fixed.h>

// include cube flow functions
#include <fit_rotations_l1.h>
#include <cube_style_data.h>

void cube_style_single_iteration(
	const Eigen::MatrixXd & V,
    Eigen::MatrixXd & U,
    cube_style_data & data);
#endif