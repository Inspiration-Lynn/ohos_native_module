#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "utils/log.h" // hilog调试用
 

// 检测目标最小面积；需要根据待检测的物体 & 分辨率进行调整
#define OBJECT_MIN_AREA 800
// 裁切后图⽚宽度
#define DEST_RESOLUTION_X 1500.0
// 裁切后图⽚⾼度
#define DEST_RESOLUTION_Y 1100.0
// 是否写出检测后图片
#define IS_WRITE_IMG true


std::string locating();

void getSrcPoint(const cv::Mat& inputImage, std::vector<cv::Point2f>& marker_coordinates);

void setPic(const std::vector<cv::Point2f>& srcPoint, const std::vector<cv::Point2f>& dstPoint, const cv::Point2f& desResolution, cv::Mat& src, cv::Mat& dst);

std::string MoveDetect(cv::Mat& background, cv::Mat& img, const int minArea);

std::string intToString(int number);
