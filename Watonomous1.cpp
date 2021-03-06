#include "stdafx.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

static Mat gaussianBlur(Mat src, Size kernel) {
	Mat blurred;
	double sigmaX = 9;
	double sigmaY = 9;

	GaussianBlur(src, blurred, kernel, sigmaX, sigmaY, BORDER_DEFAULT);

	imshow("Blurred", blurred);

	return blurred;
}

static Mat houghLines(Mat src) {

	Mat hough_img;
	cvtColor(src, hough_img, CV_GRAY2BGR);
	
	vector<Vec4i> lines;
	HoughLinesP(src, lines, 1, CV_PI / 180, 60, 10, 80);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(hough_img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}

	return hough_img;
}

static Mat subtractBackgroundMOG(Mat src) {

	Mat foreground;
	Rect rect(0, 0, src.cols -1, src.rows-1);
	Mat bgModel, fgModel;
	grabCut(src, foreground, rect, bgModel, fgModel, 3, GC_INIT_WITH_RECT);

	//get pixels that are foreground:
	compare(foreground, GC_PR_FGD, foreground, CMP_EQ);
	
	//Generate output image of only the foreground
	Mat foreground_img(src.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	src.copyTo(foreground_img, foreground);

	imshow("Foreground", foreground_img);
	waitKey(0);

	return foreground_img;
}

static Mat getYellowAndWhite(Mat src, Mat src_gray) {

	Mat imgHSV;
	cvtColor(src, imgHSV, CV_BGR2HSV);

	Mat yllw;
	Mat white;
	Mat yllw_wte;
	Mat result;

	inRange(imgHSV, Scalar(20, 100, 100), Scalar(30, 255, 255), yllw);
	inRange(src_gray, 200, 255, white);

	yllw_wte = yllw | white;
	result = src_gray & yllw_wte;

	imshow("YellowWhite", result);

	return result;
}

int main(int argc, char** argv)
{
	Mat src, src_gray, gray_blurred;
	Mat dst, detected_edges;
	int edgeThresh = 1;
	int lowThreshold = 80;
	int const max_lowThreshold = 100;
	//The optimal ratio between low and high thresholds is 3 according to documentation
	int ratio = 3;
	int kernel_size = 3;

	//src = imread(argv[1], IMREAD_COLOR);
	src = imread("C:/Users/Josie/WATANOMOUS/CarND-LaneLines-P1-master/test_images/whiteCarLaneSwitch.jpg", IMREAD_COLOR); // Load an image
	if (src.empty())
	{
		return -1;
	}
	dst.create(src.size(), src.type());
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	Mat yellow_white = getYellowAndWhite(src, src_gray);

	Mat hough_img = houghLines(yellow_white);
	imshow("Hough", hough_img);
	waitKey(0);

	return 0;
}