#ifndef VIDEO_HPP   
#define VIDEO_HPP

#include<opencv2/opencv.hpp>
#include<string>
#include<vector>
#include<utility>
#include<utility>

namespace VIDEO{

    class binaryFrame{
        private:
        int numberOfRectangle;
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rectangleList;


        public:
        binaryFrame() = default;

        void writeToFile(const std::string fileName);
        void readFromFile(const std::string fileNmae);
        void printData();
    };




















    class video{
        private:
        bool buffered;
        std::string bufferedName;
        bool bufferMode;
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
        void writeToFile(const std::string fileName, writeMode mode);
        void play(playMode mode);


    };

}


#endif