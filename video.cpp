#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include<opencv2/opencv.hpp>
#include<chrono>
#include<thread>
#include<iostream>
#include<fstream>
#include<video.hpp>
#include<utility>
using namespace std;



namespace VIDEO{

    /*
    
    video类
    
    */



    bool video::isOpen(){
        return cap.isOpened();
    }

    void video::open(const string &videoName){
        if(this -> isOpen()){
            cout << "already opened a video" << endl;
            return;
        }
        cap.open(videoName);
    }

    void video::close(){
        if(!this -> isOpen()){
            cout << "no video is opened" << endl;
            return;
        }

        buffered = false;
        cap.release();
    }

    void video::setSize(int width, int height){
        size.first = width;
        size.second = height;
    }


    //将视频二值化处理后写入文件，分两种模式，文本模式和二进制模式
    void video::writeToFile(string fileName, writeMode mode){
        
        if(!this -> isOpen()){
            cout << "no video is opened" << endl;
            return;
        }

        //写入进度可视化
        double totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
        double frameCount = 0;

        //打开文件输出流
        ofstream writer(fileName);

        //创建需要的图片对象
        cv::Mat frame, gray, binary, temp;
        cv::Size newSize(size.first, size.second);

        switch(mode){
            case STRING:{

                cout << "\033[?25l";
            while(cap.read(frame)) {
                //尺寸缩放
                cv::resize(frame, temp, newSize);
                frame = temp;

                //转灰度图
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                //二值化
                cv::threshold(gray, binary, 127, 255, cv::THRESH_OTSU);

                string buffer{};

                for(int _row = 0; _row < binary.rows; ++_row){
                    for(int _col = 0; _col < binary.cols; ++_col){
                        buffer += ((binary.at<uchar>(_row, _col) == 0) ? '1' : '0');
                    }
                    buffer += "\n";
                }
                //写完一帧后用@做分隔符
                buffer += "@\n";
                writer << buffer;

                frameCount++;
                cout << "写入进度:%" << 100 * frameCount / totalFrames << "\r";
            }
            cout << "\033[?25h";
            bufferMode = false;
            break;
        }

            case BINARY:{
                cout << "\033[?2l";
                while(cap.read(frame)){
                //尺寸缩放
                cv::resize(frame, temp, newSize);
                frame = temp;


                //转灰度图
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                //二值化
                cv::threshold(gray, binary, 127, 255, cv::THRESH_OTSU);

                for(int _row = 0; _row < binary.rows; ++_row){
                    for(int _col = 0; _col < binary.cols; ++_col){
                        writer << binary.at<uchar>(_row, _col);
                    }
                }

                //写完一帧后写入2作为分隔符
                writer << static_cast<uchar>(2);

                frameCount++;
                cout << "写入进度:%" << 100 * frameCount / totalFrames << "\r";

            }
            cout << "\033[?25h";
            bufferMode = true;
            break;
            }
            default:
            cout << "the mode is not correct, please check" << endl;
        }
        buffered = true;
        bufferedName = fileName;
        writer.close();
    }


    void video::play(playMode mode){
        if(!this -> isOpen()){
            cout << "no video is opened" << endl;
            return;
        }

        if(!buffered){
            cout << "video is not buffered" << endl;
            this -> writeToFile("data.dat", static_cast<writeMode>(static_cast<int>(mode)));
        }

        //检查缓存格式
        if(bufferMode){
            if(mode != WINDOWS){
                cout << "error buffer mode" << endl;
                return;
            }
        }
        else{
            if(mode != TERMINAL){
                cout << "error buffer mode" << endl;
                return;
            }
        }


        //获取帧率并计算帧间时间
        double fps = cap.get(cv::CAP_PROP_FPS);
        if (fps <= 0) fps = 24;
        int delay_ms = 1000 / fps;

        //创建输入文件流
        ifstream reader(bufferedName);
        if(!reader.is_open()){
            cout << "can't open the buffer file,please check" << endl;
            return;
        }

        switch(mode){
            case TERMINAL:{
                //清屏并复位
                cout << "\033[?25l" << "\033[2J";

                string frame,line;


                while(getline(reader, line)){

                    frame.clear();

                    //记录帧起始时间
                    auto start = chrono::high_resolution_clock::now();

                    while(line != "@"){
                        frame += line;
                        frame += "\n";
                        getline(reader, line);
                    }
                    auto end = chrono::high_resolution_clock::now();

                    auto time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                    cout << "\033[H" << frame << flush;

                    // 计算剩余时间并进行类型转换
                    auto sleep_time = chrono::milliseconds(delay_ms - time);

                    this_thread::sleep_for(sleep_time);

                }

                //恢复光标
                cout << "\033[?25h";

                break;
            }
            case WINDOWS:{

            }
        }



    }

    video::video():buffered(false),cap(),size({0, 0}){
    }

    video::video(string videoName):buffered(false),cap(videoName),size({0, 0}){
    }

    video::video(string videoName, int width, int height):buffered(false),cap(videoName),size({width, height}){
    }

    video::~video(){
        if(cap.isOpened()){
            cap.release();
        }
    }


    /*
    
    binaryFrame类
    
    */


    void binaryFrame::readFromFile(const string fileName){

        //创建输入流
        ifstream reader(fileName);

        if(!reader.is_open()){
            cout << "file not exist" << endl;
            return;
        }

        int _numberOfRectangle;
        pair<pair<int, int>, pair<int, int>> rectangle;

        while(reader >> _numberOfRectangle){
            for(int i = 0; i < _numberOfRectangle; ++i){
                reader >> rectangle.first.first >> rectangle.first.second >> rectangle.second.first >> rectangle.second.second;
                rectangleList.push_back(rectangle);
            }
        }
        reader.close();
    }

    void binaryFrame::writeToFile(string fileName){

        //创建输出流,以追加模式打开，文件创建和清理工作由video类管理
        ofstream writer(fileName, ofstream::app | ofstream::binary);

        writer << numberOfRectangle;
        for(auto rectangle:rectangleList){
            writer << rectangle.first.first << rectangle.first.second << rectangle.second.first << rectangle.second.second;
        }
        writer.close();
    }

    void binaryFrame::printData(){
        for(auto rectangle:rectangleList){
            cout << "坐标：" << rectangle.first.first << "," << rectangle.first.second << "\t长x宽" << rectangle.second.first << "x" << rectangle.second.second << endl;
        }
    }

}