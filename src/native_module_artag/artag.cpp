#include "artag.h"

using namespace std;
using namespace cv;

// udp
using namespace OHOS;
using namespace OHOS::HITCamera;
using namespace chrono;

Mat background;

string intToString(int number)
{
	stringstream ss;
	ss << number;
	return ss.str();
}

int getBackground()
{

	HILOG_INFO("[HIT] [artag] Get background from udp start");
    auto manager = CameraManager::getInstance();
    if (manager == nullptr) {
        // printf("Failed to get CameraManager instance\n");
        HILOG_INFO("[HIT] [artag] Failed to get CameraManager instance");
    }
    if (manager->BindRemote(5678) != ERR_OK) {
		HILOG_INFO("[HIT] [artag] Failed to bind remote");
    }
    
	// auto t0 = system_clock::now();
	auto res = manager->GetRemote();
	if (auto handle = std::get_if<PictureHandle>(&res)) {
		// auto t1 = system_clock::now();
		// auto duration = duration_cast<microseconds>(t1 - t0);
		// auto dd = double(duration.count()) * microseconds::period::num / microseconds::period::den;
		// printf("Picture cost %lf\n", dd);
		HILOG_INFO("[HIT] [artag] recieve a picture");
        int nSize = handle->size;                  // Size of buffer  
        void* pcBuffer = (void*) handle->buffer;   // Raw buffer data
        // Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
        Mat rawData( 1, nSize, CV_8UC1, pcBuffer );
        background = imdecode( rawData, IMREAD_UNCHANGED);
        if ( background.data == NULL )   
        {
            // Error reading raw image data
            HILOG_INFO("[HIT] [artag] Decode image is Null");
        }
		manager->Release(*handle);
	} else {
		int err = std::get<int>(res);
        HILOG_INFO("[HIT] [artag] Failed to capture picture, error code: %{public}d", err);
		return -1;
	}
    
    manager->UnbindRemote();

    HILOG_INFO("[HIT] [artag] Get background from udp end");

	return 0;
}


int getBackgroundFromFile() {
	background = imread("/data/storage/el2/base/haps/artag/background.jpg");
	if (background.empty()) {
		HILOG_INFO("[HIT] [artag] Fail to read background.jpg from /data/storage/el2/base/haps/artag/");
		return -1;
	}

	return 0;
}


// String MoveDetect(Mat& background, Mat& img, const int minArea, bool isWriteResultImg=false)
String MoveDetect(Mat& img, const int minArea, bool isWriteResultImg=false)
{
	HILOG_INFO("[HIT] [artag] MoveDetect() start"); 
	string detectString;
	Mat result = img.clone();
	Mat gray1, gray2;
	// auto start = std::chrono::system_clock::now();
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
		if (area < minArea) {
			// HILOG_INFO("[HIT] [artag] area is smaller than OBJECT_MIN_AREA"); 
			continue;
		}
		circle(result, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
		Rect rect(x, y, w, h);
		rectangle(result, rect, colors[i], 1, 8, 0);
		putText(result, "(" + intToString(i) + "," + intToString(center_x) + "," + intToString(center_y) + ")", Point(center_x, center_y),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
		detectString += "(" + to_string(center_x) + "," + to_string(center_y) + ")";
		HILOG_INFO("[HIT] [artag] number: %{public}d", num); 
		HILOG_INFO("[HIT] [artag] area: %{public}d", area); 
		
	}
	// auto end = std::chrono::system_clock::now();
	// std::chrono::duration<double> elapsed = end - start;
	// cout << "time is :" << elapsed.count() << "s" << endl;

	// ?????????????????????????????????
	if (isWriteResultImg) {
		imwrite("/data/storage/el2/base/haps/artag/out.png", result);
	}

	HILOG_INFO("[HIT] [artag] MoveDetect() end"); 
	return detectString;
}

// apply perspective transform to source image
void setPic(const vector<Point2f>& srcPoint, const vector<Point2f>& dstPoint, const Point2f& desResolution, Mat& src, Mat& dst)
{
	// calculates the 3*3 matrix of a perspective transform
	Mat matrix = getPerspectiveTransform(srcPoint, dstPoint);
	// Applies a perspective transformation to an image
	warpPerspective(src, dst, matrix, Point(desResolution.x, desResolution.y));
}


void getSrcPoint(const Mat& inputImage, vector<Point2f>& marker_coordinates)
{
	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners, rejectedCandidates;
	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
	for (int i = 0; i < 4; ++i)
	{
		// each marker's 4 corners' coordinates
		// std::cout << markerCorners[i][0] << markerCorners[i][1] << markerCorners[i][2] << markerCorners[i][3] << std::endl;

		// 4 artag markers's centers as new corners
		// float center_x, center_y;
		// std::cout << "id: " << markerIds[i] << endl;
		// center_x = (markerCorners[i][0].x + markerCorners[i][1].x + markerCorners[i][2].x + markerCorners[i][3].x) / 4;
		// center_y = (markerCorners[i][0].y + markerCorners[i][1].y + markerCorners[i][2].y + markerCorners[i][3].y) / 4;
		// std::cout << "center: " << center_x << " " << center_y << std::endl;
		// marker_coordinates.push_back(Point2f(center_x, center_y));

		// 4 artag markers's corners as new corners
		switch (i)
        {
            case 2:
                marker_coordinates.push_back(Point2f(markerCorners[i][0].x, markerCorners[i][0].y));
                break;
            case 3:
                marker_coordinates.push_back(Point2f(markerCorners[i][1].x, markerCorners[i][1].y));
                break;
            case 0:
                marker_coordinates.push_back(Point2f(markerCorners[i][3].x, markerCorners[i][3].y));
                break;
            case 1:
                marker_coordinates.push_back(Point2f(markerCorners[i][2].x, markerCorners[i][2].y));
                break;
        }
	}
}

string locating()
{
	HILOG_INFO("[HIT] [artag] locating() start");   
	string detectRes;

	// 1: Get background & frame image from file
	// Mat background = imread("/data/storage/el2/base/haps/artag/background.png");
	// if (background.empty()) {
	// 	HILOG_INFO("[HIT] [artag] background image is empty");   
	// 	return detectRes;
	// }
	// Mat frame = imread("/data/storage/el2/base/haps/artag/frame.png");
	// if (frame.empty()) {
	// 	HILOG_INFO("[HIT] [artag] frame image is empty");   
	// 	return detectRes;
	// }
	

	// 2: Get image from udp
    Mat frame;
    HILOG_INFO("[HIT] [artag] Get image from udp start");
    auto manager = CameraManager::getInstance();
    if (manager == nullptr) {
        // printf("Failed to get CameraManager instance\n");
        HILOG_INFO("[HIT] [artag] Failed to get CameraManager instance");
    }
    if (manager->BindRemote(5678) != ERR_OK) {
		HILOG_INFO("[HIT] [artag] Failed to bind remote");
    }
    
	// auto t0 = system_clock::now();
	auto res = manager->GetRemote();
	if (auto handle = std::get_if<PictureHandle>(&res)) {
		// auto t1 = system_clock::now();
		// auto duration = duration_cast<microseconds>(t1 - t0);
		// auto dd = double(duration.count()) * microseconds::period::num / microseconds::period::den;
		// printf("Picture cost %lf\n", dd);
		HILOG_INFO("[HIT] [artag] recieve a picture");
        int nSize = handle->size;                  // Size of buffer  
        void* pcBuffer = (void*) handle->buffer;   // Raw buffer data
        // Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
        Mat rawData( 1, nSize, CV_8UC1, pcBuffer );
        frame = imdecode( rawData, IMREAD_UNCHANGED);
        if ( frame.data == NULL )   
        {
            // Error reading raw image data
            HILOG_INFO("[HIT] [artag] Decode image is Null");
        }
		manager->Release(*handle);
	} else {
		int err = std::get<int>(res);
        HILOG_INFO("[HIT] [artag] Failed to capture picture, error code: %{public}d", err);
	}
    manager->UnbindRemote();
    HILOG_INFO("[HIT] [artag] Get image from udp end");

	// image undistortion
	Mat frameUndistorted; 
    Mat cameraMatrix, distCoeffs;
    FileStorage fs("/data/storage/el2/base/haps/artag/camera_undistortion.yaml", FileStorage::READ);  // read camera parameters from file
	fs["K"] >> cameraMatrix;
    fs["D"] >> distCoeffs;
	undistort(frame, frameUndistorted, cameraMatrix, distCoeffs);

    // the position to set the name of input file
	vector<Point2f> src;
	getSrcPoint(background, src);
	// set resolution
	Point2f dstResolution{ DEST_RESOLUTION_X, DEST_RESOLUTION_Y };
	vector<Point2f> dest{ {0.0, dstResolution.y}, {dstResolution.x, dstResolution.y}, {0.0, 0.0}, {dstResolution.x, 0.0}  };
	setPic(src, dest, dstResolution, background, background);
	setPic(src, dest, dstResolution, frameUndistorted, frameUndistorted);

	// detectRes = MoveDetect(background, frame, OBJECT_MIN_AREA, IS_WRITE_IMG);
	detectRes = MoveDetect(frameUndistorted, OBJECT_MIN_AREA, IS_WRITE_IMG);
	HILOG_INFO("[HIT] [artag] location: %{public}s", detectRes.c_str());

	HILOG_INFO("[HIT] [artag] locating() end");

	return detectRes;
}



