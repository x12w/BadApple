#include<vector>
#include<iostream>
#include<matrix.hpp>
using namespace std;


int main(){
    vector<vector<int>> matrix{
        {1, 1, 0, 0, 1, 1},
        {1, 1, 0, 0, 1, 1},
        {0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 0, 0} 
    };
    MATRIX::printMatrix(matrix);
    auto res = MATRIX::minRectangleCover(matrix);
    MATRIX::printMatrix(matrix);
    for (auto ele: res){
        cout << ele.first.first << "," << ele.first.second << "\t" << ele.second.first << "," << ele.second.second << endl;
    }

}