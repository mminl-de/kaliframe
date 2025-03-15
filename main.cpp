#include <opencv2/opencv.hpp>
#include <deque>
#include <iostream>

using namespace cv;
using namespace std;

// RTMP Stream URLs
const string RTMP_INPUT_1 = "rtmp://source_stream_1";
const string RTMP_INPUT_2 = "rtmp://source_stream_2";
const string RTMP_OUTPUT_1 = "rtmp://output_stream_1";
const string RTMP_OUTPUT_2 = "rtmp://output_stream_2";

// Buffer Settings
deque<Mat> frame_buffer_1; // Frame queue for stream 1
deque<Mat> frame_buffer_2; // Frame queue for stream 2

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::printf("Usage: %s <input_source_1> <input_source_2>\n", argv[0]);
        return -1;
    }

    int delay_frames_1 = strtol(argv[1], NULL, 10);
    int delay_frames_2 = strtol(argv[2], NULL, 10);
                            
    // Open the input streams
    VideoCapture cap1(argv[3]);
    VideoCapture cap2(argv[4]);
    if (!cap1.isOpened() || !cap2.isOpened()) {
        cerr << "Error: Could not open RTMP input streams!" << endl;
        return -1;
    }

    // Get video properties
    int width1 = cap1.get(CAP_PROP_FRAME_WIDTH);
    int height1 = cap1.get(CAP_PROP_FRAME_HEIGHT);
    int width2 = cap2.get(CAP_PROP_FRAME_WIDTH);
    int height2 = cap2.get(CAP_PROP_FRAME_HEIGHT);

    double fps1 = cap1.get(CAP_PROP_FPS);
    double fps2 = cap2.get(CAP_PROP_FPS);
    if (fps1 <= 0 || fps2 <= 0) {
        cerr << "Error: Could not determine FPS of input streams!" << endl;
        return -1;
    }
    cout << "Stream 1 FPS: " << fps1 << endl;
    cout << "Stream 2 FPS: " << fps2 << endl;

    // Define RTMP outputs with FFmpeg
    string pipeline1 = "appsrc ! videoconvert ! v4l2sink device=/dev/video0";
    string pipeline2 = "appsrc ! videoconvert ! v4l2sink device=/dev/video1";
    VideoWriter writer1(pipeline1, VideoWriter::fourcc('X', '2', '6', '4'), 30, Size(width1, height1));
    VideoWriter writer2(pipeline2, VideoWriter::fourcc('X', '2', '6', '4'), 30, Size(width2, height2));

    if (!writer1.isOpened() || !writer2.isOpened()) {
        cerr << "Error: Could not open RTMP output streams!" << endl;
        return -1;
    }

    cout << "Streaming with delays of " << delay_frames_1 << " and " << delay_frames_2 << " frames..." << endl;

    int i=0;
    while (true) {
        Mat frame1, frame2;
        if (!cap1.read(frame1) || !cap2.read(frame2)) {
            cerr << "Error: Cannot read frame from stream!" << endl;
            break;
        }

        // Add frames to buffers
        frame_buffer_1.push_back(frame1.clone());
        frame_buffer_2.push_back(frame2.clone());

        // Output delayed frames if buffers are ready
        if (frame_buffer_1.size() > delay_frames_1) {
            writer1.write(frame_buffer_1.front());
            frame_buffer_1.pop_front();
        }

        if (frame_buffer_2.size() > delay_frames_2) {
            writer2.write(frame_buffer_2.front());
            frame_buffer_2.pop_front();
        }
        printf("successfully ran %5d. time\n", i);
        i++;
    }

    cap1.release();
    cap2.release();
    writer1.release();
    writer2.release();
    return 0;
}
