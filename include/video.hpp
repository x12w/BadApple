#ifndef VIDEO_HPP   
#define VIDEO_HPP

#include<opencv2/opencv.hpp>
#include<string>
#include<utility>

namespace VIDEO{
    class video{
        private:
        bool buffered;
        std::string bufferedName;
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