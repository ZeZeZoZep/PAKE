#ifndef BABAI_H
#define BABAI_H
#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include "../include/common.h"
using namespace std;
using namespace Eigen;
pair<int, VectorXi> babai_nearest_plane(const VectorXi& b);

#endif // BABAI_H
