/*
处理01矩阵的方法
*/
#include<vector>
#include<utility>
#include<queue>
#include<algorithm>
#include<iostream>
#include<matrix.hpp>
using namespace std;


extern vector<pair<pair<int, int>, pair<int, int>>> MATRIX::simplyMatrix(vector<vector<int>> & matrix) {
    
    
    //设置最小矩阵面积
    int minLim = 4;

    //构造结果数组，结构说明：pair.first(矩阵左上角坐标,横坐标+纵坐标),pair.second(矩阵尺寸，横向*竖向)
    vector<pair<pair<int, int>, pair<int, int>>> res{};



    //自定义比较方法，比较矩阵面积
    auto comp = [](pair<pair<int, int>, pair<int, int>> &p1, pair<pair<int, int>, pair<int, int>> &p2) -> bool {
        return (p1.second.first * p1.second.second < p2.second.first * p2.second.second);
    };

    //中间优先队列，用来获得最大矩阵
    priority_queue<pair<pair<int, int>, pair<int, int>>, vector<pair<pair<int, int>, pair<int, int>>>, decltype(comp)> heap(comp);
    

    //遍历源数组
    for (int row = 0; row < matrix.size(); ++row){
        for (int col = 0; col < matrix[row].size(); ++col){
            if (matrix[row][col] == 0){
                continue;
            }



        }
    }
    
}


extern void MATRIX::horizontalCompression(vector<vector<int>> &matrix){
    for(int row = 0; row < matrix.size(); ++row){
        for(int col = 0; col < matrix[0].size(); ++col){
            if (matrix[row][col] == 0 || col == 0){
                continue;
            }

            matrix[row][col] = matrix[row][col - 1] + 1;

            
        }
    }
}


extern void MATRIX::printMatrix(const vector<vector<int>> &matrix){
    for(const auto &row: matrix){
        for(const auto &element: row){
            cout << element << "\t";
        }
        cout << endl;
    }
}


// 返回: vector<pair<起始坐标(row,col), 尺寸(height,width)>>
extern vector<pair<pair<int,int>, pair<int,int>>> 
MATRIX::minRectangleCover(const vector<vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return {};
    
    int m = matrix.size(), n = matrix[0].size();
    vector<vector<bool>> covered(m, vector<bool>(n, false));
    vector<pair<pair<int,int>, pair<int,int>>> result;
    
    while (true) {
        int bestArea = 0, br = 0, bc = 0, bh = 0, bw = 0;
        
        // 遍历每个未覆盖的1，找最大矩形
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (matrix[r][c] != 1 || covered[r][c]) continue;
                
                int maxCol = n;
                for (int i = r; i < m && matrix[i][c] == 1 && !covered[i][c]; i++) {
                    // 找当前行能延伸到哪
                    int j = c;
                    while (j < maxCol && matrix[i][j] == 1 && !covered[i][j]) j++;
                    maxCol = j;
                    
                    int h = i - r + 1, w = maxCol - c;
                    if (h * w > bestArea) {
                        bestArea = h * w;
                        br = r; bc = c; bh = h; bw = w;
                    }
                }
            }
        }
        
        if (bestArea == 0) break;
        
        // 记录结果并标记覆盖
        result.push_back({{br, bc}, {bh, bw}});
        for (int i = br; i < br + bh; i++)
            for (int j = bc; j < bc + bw; j++)
                covered[i][j] = true;
    }
    
    return result;
}