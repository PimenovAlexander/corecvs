
#ifndef IMGREADER_H
#define IMGREADER_H

#include <string>
#include "eigen/Eigen/Sparse"
#include "eigen/Eigen/Dense"

std::string parser_filename(std::string filename);

Eigen::VectorXd pixel_matrix(const std::string file_name);

#endif
