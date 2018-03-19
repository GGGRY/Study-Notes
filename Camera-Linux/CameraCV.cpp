#include "CameraCV.h"
#include <cstring>

vector<CameraInfo> CameraCV::getCameraList(){
    
    unsigned int cNum = 10; // default camera

    vector<CameraInfo> ret;
    
    for (unsigned int i=0; i <= cNum; i++) {
        CameraInfo info;
        cv::VideoCapture camera;

        camera.open(i);

        if (!camera.isOpened())
            continue;

        //info.vendor = std::string(cam->vendor ? cam->vendor : "");
        info.vendor = "OPENCV";
        info.model = "video" + to_string(i);
        info.busID = (unsigned int)i;
        
        camera.release();
        ret.push_back(info);
    }
       
    return ret;
}

CameraCV::CameraCV(unsigned int camNum, CameraTriggerMode triggerMode) : Camera(triggerMode) {
    camera = new cv::VideoCapture(camNum);
    
    if(!camera->isOpened()){
        cerr << "OPENCV: failed to open selected camera!\n";
        return;
    }

    _width = static_cast<size_t>(camera->get(CV_CAP_PROP_FRAME_WIDTH));
    _height = static_cast<size_t>(camera->get(CV_CAP_PROP_FRAME_HEIGHT));

    // Set reasonable default settings
    CameraSettings settings;

    settings.shutter = 0.0;
    settings.gain = 0.0;
    this->setCameraSettings(settings);

    // Print camera features and settings
    fflush(stdout);

    return;
}
    

CameraSettings CameraCV::getCameraSettings(){

    // Get settings:
    CameraSettings settings;
    settings.gain = camera->get(CV_CAP_PROP_GAIN);
    // settings.shutter = getSetting(cam, DC1394_FEATURE_SHUTTER);

    return settings;
}

void CameraCV::setCameraSettings(CameraSettings settings){

    // Set settings:
    // setSetting(cam, DC1394_FEATURE_GAIN, settings.gain);
    // setSetting(cam, DC1394_FEATURE_SHUTTER, settings.shutter/1000.0); // [ms]
}


void CameraCV::startCapture(){
   capturing = true;
}

void CameraCV::stopCapture(){

    capturing = false;
}

CameraFrame CameraCV::getFrame(){

    CameraFrame frame;

    if (!capturing) {
        cerr << "ERROR: Not capturing on camera. Call startCapture() before lockFrame()." << endl;
        return frame;
    }

    cv::Mat colorFrame;
    camera->read(colorFrame);

    cv::cvtColor(colorFrame, cameraFrame, CV_BGR2GRAY);

    // Copy frame address and properties
    frame.memory = cameraFrame.data;
    frame.width = cameraFrame.cols;
    frame.height = cameraFrame.rows;
    frame.type = cameraFrame.type();
    frame.sizeBytes = cameraFrame.total() * cameraFrame.elemSize();

    return frame;
}

size_t CameraCV::getFrameSizeBytes(){
    // if (!capturing) {
    //     cerr << "ERROR: Cannot get frame size before capturing. Call startCapture() before getFrameSizeBytes()." << endl;
    //     return 0;
    // }

    return 0;
}

size_t CameraCV::getFrameWidth(){
    return _width;
}


size_t CameraCV::getFrameHeight(){
    return _height;
}


CameraCV::~CameraCV(){
    // Stop camera transmission
    if(capturing)
        stopCapture();

    // Gracefulle destruct the camera
    if (camera != NULL)
    {
        camera->release();
        delete camera;
        camera = NULL;
    }
}



