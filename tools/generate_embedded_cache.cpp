#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <video.hpp>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "usage: generate_embedded_cache <input_video> <output_header> <width> <height>\n";
        return 1;
    }

    const std::string inputVideo = argv[1];
    const std::string outputHeader = argv[2];
    const int width = std::stoi(argv[3]);
    const int height = std::stoi(argv[4]);

    cv::VideoCapture capture(inputVideo);
    if (!capture.isOpened()) {
        std::cerr << "failed to open input video: " << inputVideo << '\n';
        return 1;
    }

    double fps = capture.get(cv::CAP_PROP_FPS);
    if (fps <= 0) {
        fps = 24.0;
    }
    capture.release();

    const auto tempCachePath = std::filesystem::temp_directory_path() / "badapple_embedded_cache.bin";

    VIDEO::video generator(inputVideo, width, height);
    generator.writeToFile(tempCachePath.string(), VIDEO::video::BINARY);

    std::ifstream reader(tempCachePath, std::ios::binary);
    if (!reader.is_open()) {
        std::cerr << "failed to open generated cache: " << tempCachePath << '\n';
        return 1;
    }

    const std::vector<unsigned char> bytes{
        std::istreambuf_iterator<char>(reader),
        std::istreambuf_iterator<char>()
    };
    reader.close();
    std::filesystem::remove(tempCachePath);

    std::filesystem::create_directories(std::filesystem::path(outputHeader).parent_path());
    std::ofstream writer(outputHeader, std::ios::trunc);
    if (!writer.is_open()) {
        std::cerr << "failed to write output header: " << outputHeader << '\n';
        return 1;
    }

    writer << "#pragma once\n\n";
    writer << "#include <cstddef>\n\n";
    writer << "namespace BAD_APPLE_EMBEDDED {\n";
    writer << "inline constexpr double kVideoFps = " << std::fixed << std::setprecision(6) << fps << ";\n";
    writer << "inline constexpr unsigned char kVideoCache[] = {\n";

    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (i % 12 == 0) {
            writer << "    ";
        }

        writer << static_cast<unsigned int>(bytes[i]);
        if (i + 1 != bytes.size()) {
            writer << ", ";
        }

        if (i % 12 == 11 || i + 1 == bytes.size()) {
            writer << '\n';
        }
    }

    writer << "};\n";
    writer << "inline constexpr std::size_t kVideoCacheSize = sizeof(kVideoCache);\n";
    writer << "} // namespace BAD_APPLE_EMBEDDED\n";

    return 0;
}
