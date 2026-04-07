#include <QApplication>
#include <qnamespace.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <video.hpp>

using namespace VIDEO;

namespace {
    std::string askLine(const std::string &prompt, const std::string &defaultValue) {
        std::cout << prompt;
        if (!defaultValue.empty()) {
            std::cout << " [" << defaultValue << "]";
        }
        std::cout << ": ";

        std::string value;
        std::getline(std::cin, value);
        return value.empty() ? defaultValue : value;
    }

    int askInt(const std::string &prompt, int defaultValue) {
        while (true) {
            const std::string value = askLine(prompt, std::to_string(defaultValue));
            try {
                return std::stoi(value);
            } catch (...) {
                std::cout << "请输入有效整数" << std::endl;
            }
        }
    }

    video::playMode askPlayMode() {
        while (true) {
            const std::string value = askLine("播放模式 (terminal/windows)", "windows");
            if (value == "terminal" || value == "t") {
                return video::TERMINAL;
            }
            if (value == "windows" || value == "w") {
                return video::WINDOWS;
            }
            std::cout << "请输入 terminal 或 windows" << std::endl;
        }
    }

    std::string defaultCachePath(const std::string &videoPath, video::playMode mode) {
        const std::filesystem::path input(videoPath);
        const std::string suffix = (mode == video::WINDOWS) ? ".bin" : ".txt";
        return (input.parent_path() / (input.stem().string() + "_cache" + suffix)).string();
    }
}

int main(int argc, char *argv[]) {
    const std::string videoPath = askLine("视频路径", "build/video.mp4");
    const video::playMode playMode = askPlayMode();

    const int defaultWidth = (playMode == video::WINDOWS) ? 300 : 210;
    const int defaultHeight = (playMode == video::WINDOWS) ? 100 : 70;

    const int width = askInt("缩放宽度", defaultWidth);
    const int height = askInt("缩放高度", defaultHeight);
    const std::string cachePath = askLine("缓存输出路径", defaultCachePath(videoPath, playMode));

    if (playMode == video::WINDOWS) {
#ifdef __linux__
        qputenv("QT_QPA_PLATFORM", "xcb");
#endif
        QApplication app(argc, argv);

        video player(videoPath, width, height);
        player.writeToFile(cachePath, video::BINARY);
        player.play(video::WINDOWS);
        return app.exec();
    }

    video player(videoPath, width, height);
    player.writeToFile(cachePath, video::STRING);
    player.play(video::TERMINAL);
    return 0;
}
