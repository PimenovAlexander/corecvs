#ifndef MODEL_H
#define MODEL_H

#ifdef WITH_EIGEN
#include <Eigen/Sparse>
#include <Eigen/Dense>

Eigen::VectorXd count(Eigen::VectorXd a);

#endif

#endif
