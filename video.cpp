#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
#include<video.hpp>
#include<utility>
using namespace std;



namespace VIDEO{
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




}