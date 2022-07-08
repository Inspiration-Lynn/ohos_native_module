#include "artag.h"

using namespace std;
using namespace cv;

string intToString(int number)
{
	stringstream ss;
	ss << number;
	return ss.str();
}

Mat MoveDetect(Mat& background, Mat& img, const int minArea)
{
	HILOG_INFO("[HIT] [artag] MoveDetect() start"); 
	Mat result = img.clone();
	Mat gray1, gray2;
	auto start = std::chrono::system_clock::now();
	cvtColor(background, gray1, CV_BGR2GRAY);
	cvtColor(img, gray2, CV_BGR2GRAY);
	Mat diff;
	absdiff(gray1, gray2, diff);
	threshold(diff, diff, 50, 255, CV_THRESH_BINARY);
	medianBlur(diff, diff, 5);
	Mat stats, centroids;
	int num = connectedComponentsWithStats(diff, diff, stats, centroids, 8, CV_16U);
	RNG rng(10086);
	vector<Vec3b> colors;
	for (int i = 0; i < num; i++)
	{
		Vec3b vec3 = Vec3b(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		colors.push_back(vec3);
	}
	for (int i = 1; i < num; i++)
	{
		int center_x = centroids.at<double>(i, 0);
		int center_y = centroids.at<double>(i, 1);
		int x = stats.at<int>(i, CC_STAT_LEFT);
		int y = stats.at<int>(i, CC_STAT_TOP);
		int w = stats.at<int>(i, CC_STAT_WIDTH);
		int h = stats.at<int>(i, CC_STAT_HEIGHT);
		int area = stats.at<int>(i, CC_STAT_AREA);
		if (area < minArea)
		{
			continue;
		}
		circle(result, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
		Rect rect(x, y, w, h);
		rectangle(result, rect, colors[i], 1, 8, 0);
		putText(result, "(" + intToString(i) + "," + intToString(center_x) + "," + intToString(center_y) + ")", Point(center_x, center_y),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
		cout << "number: " << i << ",area: " << area << endl;
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time is :" << elapsed.count() << "s" << endl;

	HILOG_INFO("[HIT] [artag] MoveDetect() end"); 
	return result;
}

void setPic(const vector<Point2f>& srcPoint, const vector<Point2f>& dstPoint, const Point2f& desResolution, Mat& src, Mat& dst)
{
	Mat matrix = getPerspectiveTransform(srcPoint, dstPoint);
	warpPerspective(src, dst, matrix, Point(desResolution.x, desResolution.y));
}


void getSrcPoint(const Mat& inputImage, vector<Point2f>& marker_coordinates)
{
	HILOG_INFO("[HIT] [artag] getSrcPoint() start"); 
	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners, rejectedCandidates;
	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
	for (int i = 0; i < 4; ++i)
	{
		// each marker's 4 corners' coordinates
		// std::cout << markerCorners[i][0] << markerCorners[i][1] << markerCorners[i][2] << markerCorners[i][3] << std::endl;

		float center_x, center_y;
		center_x = (markerCorners[i][0].x + markerCorners[i][1].x + markerCorners[i][2].x + markerCorners[i][3].x) / 4;
		center_y = (markerCorners[i][0].y + markerCorners[i][1].y + markerCorners[i][2].y + markerCorners[i][3].y) / 4;
		// std::cout << "center: " << center_x << " " << center_y << std::endl;
		// HILOG_INFO("[HIT] [artag] center: %{public}s %{public}s", to_string(center_x).c_str(), to_string(center_y).c_str()); 
		
		marker_coordinates.push_back(Point2f(center_x, center_y));
	}
	HILOG_INFO("[HIT] [artag] getSrcPoint() end");
}

void locating()
{
	// HILOG_INFO("[HIT] In NAPI: obj is %{public}s", obj.c_str());
    HILOG_INFO("[HIT] [artag] locating() start");   
	
	// Mat background = imread("t1.jpg");
	// Mat frame = imread("t2.jpg");
	Mat background = imread("/data/storage/el2/base/haps/artag/t1.jpg");
	Mat frame = imread("/data/storage/el2/base/haps/artag/t2.jpg");

    // the position to set the name of input file
	vector<Point2f> src;
	getSrcPoint(background, src);
	Point2f dstResolution{ 430.0,687.0 };
	vector<Point2f> dest{ {0.0,0.0} ,{dstResolution.x,0.0},{0.0,dstResolution.y}, {dstResolution.x,dstResolution.y} };
	setPic(src, dest, dstResolution, background, background);
	setPic(src, dest, dstResolution, frame, frame);

	Mat result = MoveDetect(background, frame, 1000);
	// imwrite("t.jpg", result);
	imwrite("/data/storage/el2/base/haps/artag/output.jpg", result);

	HILOG_INFO("[HIT] [artag] locating() end"); 
}

