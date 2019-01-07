#ifndef IMGREADER_H
#define IMGREADER_H

#ifdef WITH_EIGEN

#include <string>
#include <Eigen/Sparse>
#include <Eigen/Dense>

std::string parser_filename(std::string filename);

Eigen::VectorXd pixel_matrix(const std::string file_name);

#endif

#endif
