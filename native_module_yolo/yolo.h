#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "vector"

#include "utils/log.h" // hilog调试用

// camera
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "hitcamera_manager.h"

struct Net_config {
    float confThreshold;
    float nmsThreshold;
    float objThreshold;
    std::string modelpath;
};

const float anchors_640[3][6] = {{10.0,  13.0, 16.0,  30.0,  33.0,  23.0},
                                 {30.0,  61.0, 62.0,  45.0,  59.0,  119.0},
                                 {116.0, 90.0, 156.0, 198.0, 373.0, 326.0}};

const float anchors_1280[4][6] = {{19,  27,  44,  40,  38,  94},
                                  {96,  68,  86,  152, 180, 137},
                                  {140, 301, 303, 264, 238, 542},
                                  {436, 615, 739, 380, 925, 792}};
                                
class YOLO {
public:
    YOLO(Net_config config);

    std::string detect(cv::Mat &frame, double percentage);

private:
    float *anchors;
    int num_stride;
    int inpWidth;
    int inpHeight;
    std::vector<std::string> class_names;
    int num_class;

    float confThreshold;
    float nmsThreshold;
    float objThreshold;
    const bool keep_ratio = true;
    // Net net;
    cv::dnn::Net net;

    void drawPred(float conf, int left, int top, int right, int bottom, cv::Mat &frame, int classid);

    cv::Mat resize_image(cv::Mat srcimg, int *newh, int *neww, int *top, int *left);
};

std::string detect(double percentage);