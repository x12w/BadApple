#ifndef VIDEO_HPP   
#define VIDEO_HPP

#include <iostream>
#include<QWidget>
#include<opencv2/opencv.hpp>
#include<string>
#include<vector>
#include<fstream>
#include<utility>

namespace VIDEO{

    class binaryFrame{
        private:
        int numberOfRectangle = 0;
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rectangleList;


        public:
        binaryFrame() = default;
        binaryFrame(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> &_rectangleList);

        void getValue(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> &_rectangleList);
        void writeToFile(const std::string fileName);
        void writeToFile(std::ofstream &writer);
        void readFromFile(const std::string fileNmae);
        std::ifstream& readFromFile(std::ifstream &reader);
        void printData() const;
        const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> &rectangles() const;
    };







    class video{
        private:
        bool buffered;
        std::string bufferedName;
        bool bufferMode;
        double playbackFps = 24.0;
        cv::VideoCapture cap;
        std::pair<int, int> size{};
        

        public:
        
        enum writeMode{STRING, BINARY};
        enum playMode{TERMINAL, WINDOWS};
        video();
        video(std::string);
        video(std::string videoName, int width, int height);
        ~video();

        bool isOpen();
        void open(const std::string &videoName);
        void close();
        void setSize(int width, int height);
        void setBuffer(const std::string &bufferName, writeMode mode);
        void setPlaybackFps(double fps);
        void writeToFile(const std::string fileName, writeMode mode);
        void play(playMode mode);


    };

}


#endif
