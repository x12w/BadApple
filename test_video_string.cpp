#include<video.hpp>
using namespace VIDEO;
using namespace std;


int main() {

    video v1("video.mp4", 210, 70);
    v1.writeToFile("video_string", video::STRING);

    v1.play(video::TERMINAL);
}