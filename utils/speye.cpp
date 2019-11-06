#include "speye.h"

void speye(
	const int & numEle,
	Eigen::SparseMatrix<double> & M)
{
	std::vector<Eigen::Triplet<double>> IJV;
	IJV.reserve(numEle);
	for(int ii = 0; ii < numEle; ii++)
		IJV.push_back(Eigen::Triplet<double>(ii, ii, 1.0));
		
	M.resize(numEle,numEle);
	M.setFromTriplets(IJV.begin(),IJV.end());
}