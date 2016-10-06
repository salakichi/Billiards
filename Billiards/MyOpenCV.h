#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <opencv2/opencv.hpp>

#if _DEBUG
#pragma comment(lib, "opencv_calib3d310d.lib")
#pragma comment(lib, "opencv_core310d.lib")
#pragma comment(lib, "opencv_features2d310d.lib")
#pragma comment(lib, "opencv_flann310d.lib")
#pragma comment(lib, "opencv_highgui310d.lib")
#pragma comment(lib, "opencv_imgcodecs310d.lib")
#pragma comment(lib, "opencv_imgproc310d.lib")
#pragma comment(lib, "opencv_ml310d.lib")
#pragma comment(lib, "opencv_objdetect310d.lib")
#pragma comment(lib, "opencv_photo310d.lib")
#pragma comment(lib, "opencv_shape310d.lib")
#pragma comment(lib, "opencv_stitching310d.lib")
#pragma comment(lib, "opencv_superres310d.lib")
#pragma comment(lib, "opencv_ts310d.lib")
#pragma comment(lib, "opencv_video310d.lib")
#pragma comment(lib, "opencv_videoio310d.lib")
#pragma comment(lib, "opencv_videostab310d.lib")
#else
#pragma comment(lib, "opencv_calib3d310.lib")
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_features2d310.lib")
#pragma comment(lib, "opencv_flann310.lib")
#pragma comment(lib, "opencv_highgui310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
#pragma comment(lib, "opencv_imgproc310.lib")
#pragma comment(lib, "opencv_ml310.lib")
#pragma comment(lib, "opencv_objdetect310.lib")
#pragma comment(lib, "opencv_photo310.lib")
#pragma comment(lib, "opencv_shape310.lib")
#pragma comment(lib, "opencv_stitching310.lib")
#pragma comment(lib, "opencv_superres310.lib")
#pragma comment(lib, "opencv_ts310.lib")
#pragma comment(lib, "opencv_video310.lib")
#pragma comment(lib, "opencv_videoio310.lib")
#pragma comment(lib, "opencv_videostab310.lib")
#endif