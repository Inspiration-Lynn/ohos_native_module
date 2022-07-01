#include "yolo.h"
// debug 
// #include <filesystem>
// namespace fs = std::filesystem;

using namespace cv;
using namespace dnn;
using namespace std;

// camera
using namespace OHOS;
using namespace OHOS::HITCamera;
using namespace chrono;


//yolo
int endsWith(string s, string sub) {
    return s.rfind(sub) == (s.length() - sub.length()) ? 1 : 0;
}

YOLO::YOLO(Net_config config) {
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->objThreshold = config.objThreshold;

    HILOG_INFO("[HIT] [yolo] YOLO1");

    this->net = readNet(config.modelpath);

    HILOG_INFO("[HIT] [yolo] YOLO2");

    // ifstream ifs("/yolo/class.names");
    ifstream ifs("/data/storage/el2/base/haps/yolo/class.names");
    
    HILOG_INFO("[HIT] [yolo] YOLO3");

    string line;
    while (getline(ifs, line)) this->class_names.push_back(line);
    this->num_class = class_names.size();

    HILOG_INFO("[HIT] [yolo] YOLO4");

    if (endsWith(config.modelpath, "6.onnx")) {
        anchors = (float *) anchors_1280;
        this->num_stride = 4;
        this->inpHeight = 1280;
        this->inpWidth = 1280;
    } else {
        anchors = (float *) anchors_640;
        this->num_stride = 3;
        this->inpHeight = 640;
        this->inpWidth = 640;
    }
}

Mat YOLO::resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left) {
    int srch = srcimg.rows, srcw = srcimg.cols;
    *newh = this->inpHeight;
    *neww = this->inpWidth;
    Mat dstimg;
    if (this->keep_ratio && srch != srcw) {
        float hw_scale = (float) srch / srcw;
        if (hw_scale > 1) {
            *newh = this->inpHeight;
            *neww = int(this->inpWidth / hw_scale);
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *left = int((this->inpWidth - *neww) * 0.5);
            copyMakeBorder(dstimg, dstimg, 0, 0, *left, this->inpWidth - *neww - *left, BORDER_CONSTANT, 114);
        } else {
            *newh = (int) this->inpHeight * hw_scale;
            *neww = this->inpWidth;
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *top = (int) (this->inpHeight - *newh) * 0.5;
            copyMakeBorder(dstimg, dstimg, *top, this->inpHeight - *newh - *top, 0, 0, BORDER_CONSTANT, 114);
        }
    } else {
        resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
    }
    return dstimg;
}

void YOLO::drawPred(float conf, int left, int top, int right, int bottom, Mat &frame,
                    int classid)   // Draw the predicted bounding box
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 2);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    label = this->class_names[classid] + ":" + label;
    HILOG_INFO("[HIT] The image to be classified is  %{public}s", label.c_str());

    HILOG_INFO("[HIT] [yolo] 6");    // 6
    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    //rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1);

    HILOG_INFO("[HIT] [yolo] 7");    // 7
}

void YOLO::detect(Mat &frame) {

    HILOG_INFO("[HIT] [yolo] 3");    // 3

    int newh = 0, neww = 0, padh = 0, padw = 0;
    Mat dstimg = this->resize_image(frame, &newh, &neww, &padh, &padw);
    Mat blob = blobFromImage(dstimg, 1 / 255.0, Size(this->inpWidth, this->inpHeight), Scalar(0, 0, 0), true, false);
    this->net.setInput(blob);
    vector<Mat> outs;
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());

    int num_proposal = outs[0].size[1];
    int nout = outs[0].size[2];
    if (outs[0].dims > 2) {
        outs[0] = outs[0].reshape(0, num_proposal);
    }

    HILOG_INFO("[HIT] [yolo] 4");    // 4

    /////generate proposals
    vector<float> confidences;
    vector<Rect> boxes;
    vector<int> classIds;
    float ratioh = (float) frame.rows / newh, ratiow = (float) frame.cols / neww;
    int n = 0, q = 0, i = 0, j = 0, row_ind = 0;
    float *pdata = (float *) outs[0].data;
    for (n = 0; n < this->num_stride; n++) {
        const float stride = pow(2, n + 3);
        int num_grid_x = (int) ceil((this->inpWidth / stride));
        int num_grid_y = (int) ceil((this->inpHeight / stride));
        for (q = 0; q < 3; q++)    ///anchor
        {
            const float anchor_w = this->anchors[n * 6 + q * 2];
            const float anchor_h = this->anchors[n * 6 + q * 2 + 1];
            for (i = 0; i < num_grid_y; i++) {
                for (j = 0; j < num_grid_x; j++) {
                    float box_score = pdata[4];
                    if (box_score > this->objThreshold) {
                        Mat scores = outs[0].row(row_ind).colRange(5, nout);
                        Point classIdPoint;
                        double max_class_socre;
                        // Get the value and location of the maximum score
                        minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
                        max_class_socre *= box_score;
                        if (max_class_socre > this->confThreshold) {
                            const int class_idx = classIdPoint.x;
                            float cx = (pdata[0] * 2.f - 0.5f + j) * stride;  ///cx
                            float cy = (pdata[1] * 2.f - 0.5f + i) * stride;   ///cy
                            float w = powf(pdata[2] * 2.f, 2.f) * anchor_w;   ///w
                            float h = powf(pdata[3] * 2.f, 2.f) * anchor_h;  ///h

                            int left = int((cx - padw - 0.5 * w) * ratiow);
                            int top = int((cy - padh - 0.5 * h) * ratioh);

                            confidences.push_back((float) max_class_socre);
                            boxes.push_back(Rect(left, top, (int) (w * ratiow), (int) (h * ratioh)));
                            classIds.push_back(class_idx);
                        }
                    }
                    row_ind++;
                    pdata += nout;
                }
            }
        }
    }

    HILOG_INFO("[HIT] [yolo] 5");    // 5

    vector<int> indices;
    dnn::NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        Rect box = boxes[idx];
        this->drawPred(confidences[idx], box.x, box.y,
                       box.x + box.width, box.y + box.height, frame, classIds[idx]);
    }
}


Mat detect(string imgpath) {
    
    HILOG_INFO("[HIT] [yolo] 2");    // 2

    HILOG_INFO("[HIT] [yolo] image path is %{public}s", imgpath.c_str());

    // 1: Get image from file
    // HILOG_INFO("[HIT] [yolo] Get image from file ");
    // Mat data = imread(imgpath);
    // // Mat data = imread("/data/storage/el2/base/haps/yolo/images/bus.jpg");
    // if(data.empty()) {
    //      HILOG_INFO("[HIT] [yolo] 222Could not read the image ");
    //  } 
    //  else {
    //      HILOG_INFO("[HIT] [yolo] 222read image successfully");
    //  }

    // 2: Get image from camera
    Mat decodedImage;
    HILOG_INFO("[HIT] [yolo] Get image from camera ");
    auto manager = CameraManager::getInstance();
    if (manager == nullptr) {
        // printf("Failed to get CameraManager instance\n");
        HILOG_INFO("[HIT] [yolo] Failed to get CameraManager instance");
    }
    auto res = manager->Capture(320, 240);
    if (auto handle = std::get_if<PictureHandle>(&res)) {
        int nSize = handle->size;                              // Size of buffer  
        void* pcBuffer = (void*) handle->buffer;   // Raw buffer data
        // Create a Size(1, nSize) Mat object of 8-bit, single-byte elements
        Mat rawData( 1, nSize, CV_8UC1, pcBuffer );
        decodedImage = imdecode( rawData, IMREAD_UNCHANGED);
        if ( decodedImage.data == NULL )   
        {
            // Error reading raw image data
            HILOG_INFO("[HIT] [yolo] Decode image is Null");
        }
        manager->Release(*handle);
    } else {
        int err = std::get<int>(res);
        // printf("Failed to capture picture, error code: %d\n", err);
        HILOG_INFO("[HIT] [yolo] Failed to capture picture, error code: %{public}d", err);
    }


    Net_config yolo_nets = {0.3, 0.5, 0.3, "/data/storage/el2/base/haps/yolo/weights/yolov5s.onnx"};
    HILOG_INFO("[HIT] [yolo] here1");
    YOLO yolo_model(yolo_nets);
    HILOG_INFO("[HIT] [yolo] here2");
    yolo_model.detect(decodedImage);

    return decodedImage;
}



