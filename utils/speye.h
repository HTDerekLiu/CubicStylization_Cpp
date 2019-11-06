#ifndef SPEYE_H
#define SPEYE_H

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Sparse>

void speye(
	const int & numEle,
	Eigen::SparseMatrix<double> & M);
#endif