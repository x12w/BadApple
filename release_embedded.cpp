#include <QApplication>
#include <QByteArray>
#include <QPaintEvent>
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <QWidget>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "generated/embedded_video_cache.hpp"

namespace {
    constexpr int kCellWidth = 8;
    constexpr int kCellHeight = 10;

    using Rectangle = std::pair<std::pair<int, int>, std::pair<int, int>>;
    using Frame = std::vector<Rectangle>;

    std::int32_t readInt32LE(const unsigned char *data, std::size_t size, std::size_t &offset) {
        if (offset + 4 > size) {
            throw std::runtime_error("unexpected end of embedded cache data");
        }

        const std::uint32_t value =
            static_cast<std::uint32_t>(data[offset]) |
            (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
            (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
            (static_cast<std::uint32_t>(data[offset + 3]) << 24);

        offset += 4;
        return static_cast<std::int32_t>(value);
    }

    std::vector<Frame> parseFrames() {
        std::vector<Frame> frames;
        std::size_t offset = 0;

        while (offset < BAD_APPLE_EMBEDDED::kVideoCacheSize) {
            const int rectangleCount = readInt32LE(
                BAD_APPLE_EMBEDDED::kVideoCache,
                BAD_APPLE_EMBEDDED::kVideoCacheSize,
                offset
            );

            if (rectangleCount < 0) {
                throw std::runtime_error("embedded cache contains a negative rectangle count");
            }

            Frame frame;
            frame.reserve(static_cast<std::size_t>(rectangleCount));

            for (int i = 0; i < rectangleCount; ++i) {
                const int row = readInt32LE(BAD_APPLE_EMBEDDED::kVideoCache, BAD_APPLE_EMBEDDED::kVideoCacheSize, offset);
                const int col = readInt32LE(BAD_APPLE_EMBEDDED::kVideoCache, BAD_APPLE_EMBEDDED::kVideoCacheSize, offset);
                const int height = readInt32LE(BAD_APPLE_EMBEDDED::kVideoCache, BAD_APPLE_EMBEDDED::kVideoCacheSize, offset);
                const int width = readInt32LE(BAD_APPLE_EMBEDDED::kVideoCache, BAD_APPLE_EMBEDDED::kVideoCacheSize, offset);

                frame.push_back({{row, col}, {height, width}});
            }

            frames.push_back(std::move(frame));
        }

        return frames;
    }

    class EmbeddedPlayerWidget : public QWidget {
    public:
        explicit EmbeddedPlayerWidget(std::vector<Frame> frames, QWidget *parent = nullptr)
            : QWidget(parent),
              frames_(std::move(frames)),
              frameDelayMs_(std::max(1, static_cast<int>(1000.0 / std::max(BAD_APPLE_EMBEDDED::kVideoFps, 1.0)))) {
            setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
            setAttribute(Qt::WA_TranslucentBackground);
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_OpaquePaintEvent, false);

            contentSize_ = computeCanvasSize();

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
            timer_.start(frameDelayMs_);
            update();
        }

    protected:
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
        QSize computeCanvasSize() const {
            int maxRow = 0;
            int maxCol = 0;

            for (const auto &frame : frames_) {
                for (const auto &rectangle : frame) {
                    maxRow = std::max(maxRow, rectangle.first.first + rectangle.second.first);
                    maxCol = std::max(maxCol, rectangle.first.second + rectangle.second.second);
                }
            }

            return QSize(
                std::max(maxCol * kCellWidth, 1),
                std::max(maxRow * kCellHeight, 1)
            );
        }

        std::vector<Frame> frames_;
        QTimer timer_;
        std::size_t currentFrameIndex_ = 0;
        int frameDelayMs_;
        QSize contentSize_;
    };
}

int main(int argc, char *argv[]) {
#ifdef __linux__
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    QApplication app(argc, argv);

    try {
        auto *player = new EmbeddedPlayerWidget(parseFrames());
        player->setAttribute(Qt::WA_DeleteOnClose);
        player->start();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return app.exec();
}
