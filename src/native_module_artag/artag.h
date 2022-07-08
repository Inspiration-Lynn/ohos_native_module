#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "utils/log.h" // hilog调试用
 
void locating();

void getSrcPoint(const cv::Mat& inputImage, std::vector<cv::Point2f>& marker_coordinates);

void setPic(const std::vector<cv::Point2f>& srcPoint, const std::vector<cv::Point2f>& dstPoint, const cv::Point2f& desResolution, cv::Mat& src, cv::Mat& dst);

cv::Mat MoveDetect(cv::Mat& background, cv::Mat& img, const int minArea);

std::string intToString(int number);
