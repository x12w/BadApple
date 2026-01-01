#include<vector>
#include<utility>




#ifndef MATRIX_HPP
#define MATRIX_HPP

extern std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> simplyMatrix(std::vector<std::vector<int>> &matrix);
extern void horizontalCompression(std::vector<std::vector<int>> &matrix);
extern std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> minRectangleCover(const std::vector<std::vector<int>> &matrix);
extern void printMatrix(const std::vector<std::vector<int>> &matrix);

#endif