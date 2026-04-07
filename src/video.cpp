#include<QWidget>
#include<QCoreApplication>
#include<QApplication>
#include<QPainter>
#include<QKeyEvent>
#include<QScreen>
#include<QTimer>
#include<qnamespace.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include<opencv2/opencv.hpp>
#include<chrono>
#include<thread>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<video.hpp>
#include<utility>
#include<matrix.hpp>
using namespace std;

namespace {
    constexpr int kCellWidth = 8;
    constexpr int kCellHeight = 10;

    class FramePlayerWidget : public QWidget {
    public:
        FramePlayerWidget(
            std::vector<std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>> frames,
            QSize logicalFrameSize,
            int frameDelayMs,
            QWidget *parent = nullptr
        ) : QWidget(parent),
            frames_(std::move(frames)),
            logicalFrameSize_(logicalFrameSize),
            frameDelayMs_(std::max(frameDelayMs, 1)) {
            setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
            setAttribute(Qt::WA_TranslucentBackground);
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_OpaquePaintEvent, false);
            setFocusPolicy(Qt::StrongFocus);

            contentSize_ = computeCanvasSize(logicalFrameSize_);

            connect(&timer_, &QTimer::timeout, this, [this]() {
                if (frames_.empty()) {
                    timer_.stop();
                    close();
                    return;
                }

                currentFrameIndex_ = (currentFrameIndex_ + 1) % frames_.size();
                update();
            });
        }

        void start() {
            if (frames_.empty()) {
                close();
                return;
            }

            if (auto *screen = QGuiApplication::primaryScreen()) {
                setGeometry(screen->geometry());
            } else {
                resize(contentSize_);
            }

            showFullScreen();
            raise();
            activateWindow();
            setFocus();
            timer_.start(frameDelayMs_);
            update();
        }

    protected:
        void keyPressEvent(QKeyEvent *event) override {
            if (event->key() == Qt::Key_Escape) {
                close();
                return;
            }

            QWidget::keyPressEvent(event);
        }

        void paintEvent(QPaintEvent *) override {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.fillRect(rect(), Qt::transparent);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::black);

            const double scaleX = static_cast<double>(width()) / std::max(contentSize_.width(), 1);
            const double scaleY = static_cast<double>(height()) / std::max(contentSize_.height(), 1);

            for (const auto &rectangle : frames_[currentFrameIndex_]) {
                const int row = rectangle.first.first;
                const int col = rectangle.first.second;
                const int height = rectangle.second.first;
                const int width = rectangle.second.second;

                painter.drawRect(
                    static_cast<int>(col * kCellWidth * scaleX),
                    static_cast<int>(row * kCellHeight * scaleY),
                    std::max(1, static_cast<int>(width * kCellWidth * scaleX)),
                    std::max(1, static_cast<int>(height * kCellHeight * scaleY))
                );
            }
        }

    private:
        static QSize computeCanvasSize(const QSize &logicalFrameSize) {
            return QSize(
                std::max(logicalFrameSize.width() * kCellWidth, 1),
                std::max(logicalFrameSize.height() * kCellHeight, 1)
            );
        }

        std::vector<std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>> frames_;
        QSize logicalFrameSize_;
        QTimer timer_;
        std::size_t currentFrameIndex_ = 0;
        int frameDelayMs_;
        QSize contentSize_;
    };
}


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
        const double fps = cap.get(cv::CAP_PROP_FPS);
        if (fps > 0) {
            playbackFps = fps;
        }
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

    void video::setPlaybackFps(double fps) {
        if (fps > 0) {
            playbackFps = fps;
        }
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
                cout << "写入进度:%" << 100 * frameCount / totalFrames << '\r';
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

                //对二值化后的图片进行矩阵处理并写入
                vector<vector<int>> mat(binary.rows, vector<int>(binary.cols));

                for(int _row = 0; _row < binary.rows; ++_row){
                    for(int _col = 0; _col < binary.cols; ++_col){
                        mat[_row][_col] = (binary.at<uchar>(_row, _col) == 0) ? 0 : 1;
                    }
                }

                binaryFrame tempFrame(MATRIX::minRectangleCover(mat));
                tempFrame.writeToFile(writer);

                frameCount++;
                cout << "写入进度:%" << 100 * frameCount / totalFrames << '\r';

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
        if(!this -> isOpen() && !buffered){
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
        double fps = playbackFps;
        if (cap.isOpened()) {
            const double captureFps = cap.get(cv::CAP_PROP_FPS);
            if (captureFps > 0) {
                playbackFps = captureFps;
                fps = captureFps;
            }
        }
        if (fps <= 0) fps = 24;
        int delay_ms = 1000 / fps;

        ifstream reader;
        

        switch(mode){
            case TERMINAL:{
            //创建输入文件流
                reader.open(bufferedName);
                if(!reader.is_open()){
                cout << "can't open the buffer file,please check" << endl;
                return;
                }
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
                    if (time < delay_ms) {
                        this_thread::sleep_for(chrono::milliseconds(delay_ms - time));
                    }

                }

                //恢复光标
                cout << "\033[?25h";

                break;
            }
            case WINDOWS:{
                reader.open(bufferedName, std::ios::binary);
                if(!reader.is_open()){
                cout << "can't open the buffer file,please check" << endl;
                return;
                }

                //重新定位输入流并恢复流状态
                reader.clear();
                reader.seekg(0);

                binaryFrame tempFrame;
                reader.seekg(0);
                reader.clear();

                vector<vector<pair<pair<int, int>, pair<int, int>>>> frames;
                while(tempFrame.readFromFile(reader)) {
                    if (!reader) {
                        break;
                    }
                    frames.push_back(tempFrame.rectangles());
                }

                if (frames.empty()) {
                    cout << "no frame data available" << endl;
                    return;
                }

                const QSize logicalFrameSize(
                    std::max(size.first, 1),
                    std::max(size.second, 1)
                );

                auto *player = new FramePlayerWidget(std::move(frames), logicalFrameSize, delay_ms);
                player->setAttribute(Qt::WA_DeleteOnClose);
                player->start();
                break;

            }
        }



    }

    video::video():buffered(false),playbackFps(24.0),cap(),size({0, 0}){
    }

    video::video(string videoName):buffered(false),playbackFps(24.0),cap(videoName),size({0, 0}){
        const double fps = cap.get(cv::CAP_PROP_FPS);
        if (fps > 0) {
            playbackFps = fps;
        }
    }

    video::video(string videoName, int width, int height):buffered(false),playbackFps(24.0),cap(videoName),size({width, height}){
        const double fps = cap.get(cv::CAP_PROP_FPS);
        if (fps > 0) {
            playbackFps = fps;
        }
    }

    video::~video(){
        if(cap.isOpened()){
            cap.release();
        }
    }

    void video::setBuffer(const string &bufferName, video::writeMode mode) {

        bufferedName = bufferName;
        if(mode == video::STRING){
            bufferMode = false;
        }
        else{
            bufferMode = true;
        }

        buffered = true;
    }


    /*
    
    binaryFrame类
    
    */

    binaryFrame::binaryFrame(const vector<pair<pair<int, int>, pair<int, int>>> &_rectangleList):rectangleList(_rectangleList) {
        numberOfRectangle = static_cast<int>(rectangleList.size());
    }


    void binaryFrame::getValue(const vector<pair<pair<int, int>, pair<int, int>>> &_rectangleList){
        rectangleList = _rectangleList;
        numberOfRectangle = static_cast<int>(rectangleList.size());
    }


    void binaryFrame::readFromFile(const string fileName){

        //创建输入流
        ifstream reader(fileName);

        if(!reader.is_open()){
            cout << "file not exist" << endl;
            return;
        }

        int _numberOfRectangle;
        pair<pair<int, int>, pair<int, int>> rectangle;

        while(reader.read(reinterpret_cast<char *>(&_numberOfRectangle), sizeof(_numberOfRectangle))){
            for(int i = 0; i < _numberOfRectangle; ++i){
                reader.read(reinterpret_cast<char *>(&rectangle), sizeof(rectangle));
                rectangleList.push_back(rectangle);
            }
        }
        reader.close();
    }

    void binaryFrame::writeToFile(string fileName){

        //创建输出流,以追加模式打开，文件创建和清理工作由video类管理
        ofstream writer(fileName, ofstream::app | ofstream::binary);
        numberOfRectangle = static_cast<int>(rectangleList.size());

        writer.write(reinterpret_cast<char *>(&numberOfRectangle), sizeof(numberOfRectangle));
        for(const auto &rectangle:rectangleList){
            writer.write(reinterpret_cast<const char *>(&rectangle), sizeof(rectangle));
        }
        writer.close();
    }

    void binaryFrame::printData() const{
        for(auto rectangle:rectangleList){
            cout << "坐标：" << rectangle.first.first << "," 
            << rectangle.first.second 
            << "\t长x宽," 
            << rectangle.second.first 
            << "x" 
            << rectangle.second.second << endl;
        }
    }

    ifstream& binaryFrame::readFromFile(ifstream &reader){
        
        if(!reader){
            return reader;
        }

        rectangleList.clear();

        int _numberOfRectangle;
        pair<pair<int, int>, pair<int, int>> rectangle;

        if(!reader.read(reinterpret_cast<char *>(&_numberOfRectangle), sizeof(_numberOfRectangle))){
            return reader;
        }

        numberOfRectangle = _numberOfRectangle;
        for(int i = 0; i < _numberOfRectangle; ++i){
            if(!reader.read(reinterpret_cast<char *>(&rectangle), sizeof(rectangle))){
                break;
            }
            rectangleList.push_back(rectangle);
        }
        return reader;
    }

    void binaryFrame::writeToFile(ofstream &writer){
        numberOfRectangle = static_cast<int>(rectangleList.size());
        writer.write(reinterpret_cast<char *>(&numberOfRectangle), sizeof(numberOfRectangle));

        for(const auto &rectangle:rectangleList){
            writer.write(reinterpret_cast<const char *>(&rectangle), sizeof(rectangle));
        }
    }

    const vector<pair<pair<int, int>, pair<int, int>>> &binaryFrame::rectangles() const {
        return rectangleList;
    }
}
