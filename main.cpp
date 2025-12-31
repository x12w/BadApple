#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<fstream>
using namespace std;

int main(){

    //读取视频
    cv::VideoCapture cap("video.mp4");
    if (!cap.isOpened()){
        cout << "not found video" << endl;
        return -1;
    }

    //获取帧率并计算帧间时间
    double fps = cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 24;
    int delay_ms = 1000 / fps;

    //创建输出文件流
    ofstream writer("data.txt");
    if (!writer.is_open()) {
        cout << "can't open file" << endl;
        return -1;
    }

    //图片数据
    cv::Mat frame, gray, binary, small;

    //播放尺寸
    cv::Size displaySize(300, 100);


    while(cap.read(frame)){
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        cv::resize(binary, small, displaySize);

        

        for (int y = 0; y < small.rows; ++y) {
            for (int x = 0; x < small.cols; ++x) {
                uchar pixel = small.at<uchar>(y, x);

                writer << (pixel == 0) ? '1' : '0';
            }
            writer << "\n";
        }
        writer << '@' << "\n";
    }

    //创建输入文件流
    ifstream reader("data.txt");
    char chr;


    //隐藏光标
    cout << "\033[?25l";

    string line, frameBuffer;

    //播放
    while (true) {
        auto startTime = chrono::steady_clock::now();

        // 按行读取，直到遇到分隔符 @
        frameBuffer = "";
        bool hasMore = false;
        while (getline(reader, line)) {
            hasMore = true;
            if (line == "@") break;
            frameBuffer += line + "\n";
        }

        if (!hasMore) break; // 文件读完

        // 打印整帧
        cout << "\033[H" << frameBuffer << flush;

        // 控速逻辑
        auto endTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        if (elapsed < delay_ms) {
            this_thread::sleep_for(chrono::milliseconds(delay_ms - elapsed));
        }
    }
    cout << "\033[2J" << endl;

    cap.release();
    return 0;
}