// main.cpp
// Created on: 30/07/2015
// Author: Alejandro Herrero
//
// RECOGNIZING A SWIMMING POOL ON SATELLITE IMAGES
// This algorithm was developed for the test of knowledge of the company Dacuda.
// The objective is to seek outdoor pools of some satellite images of Google Maps.
//
// For that, It is possible obtain images in Google Maps in the view “Earth”,
// without Labels(Menu Earth->Labels off), from a distance of 100 meters, in a full screen (my computer size is 1366x768)
// and take a screen capture(Print screen button).
//
// The application works with an input image, like “image.png”, and it will recognize the swimming pools and the measures.
// For that, the algorithms also recognize the Google Maps scale(At bottom right of the image)
// and apply the pixel to meters transformation.
// The algorithm is thinking for big swimming pools(public) so maybe it will not work with private swimming pools.


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SwimmingPool.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//Load the images, in the argument or by console
	Mat src, image;

	if (argc == 2)
	{
		image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
	}
	else
	{
		char filename[100];
		cout << "Enter filename. For example: 'image1.png'" << endl;
		cin >> filename;
		src = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

	}
	
	if (!src.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		system("pause");
		return -1;
	}

	//cut the top (google search bar)
	image = Mat(src, Rect(0, 47, src.size().width, src.size().height - 47));

	//Typical Swimming Pool
	SwimmingPool swimming_pool_common;

	//MASK WITH THE COLOR
	//Mask with a the typical color of the swimming pools (measure with some pools)
	//This is better than split for channels because the swimming pools aren't all blue (or completaly blue)
	Mat mask;
	//specify the range of colours in BGR, from low to high
	cv::Scalar lowerb = swimming_pool_common.getLowerColor();
	cv::Scalar upperb = swimming_pool_common.getUpperColor();

	//Apply mask
	cv::inRange(image, lowerb, upperb, mask);
	blur(mask, mask, Size(3, 3));

	//"cut" the minimap button
	cv::rectangle(mask, cv::Rect(30, 616, 80, 80), cv::Scalar(0, 0, 0), -1);

	//THRESHOLDING
	Mat threshold_output;
	int threshold_value = 63;
	int max_BINARY_value = 255;
	int threshold_type = 0;	//Threshold Binary
	threshold(mask, threshold_output, threshold_value, max_BINARY_value, threshold_type);

	//RECOGNIZE SWIMMING POOL
	vector <SwimmingPool> swimming_pools;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	int thresh = 250;
	/// Detect edges using canny
	Canny(threshold_output, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	///// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
		//printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, 
		//	contourArea(contours[i]), arcLength(contours[i], true));
	}

	//Filter by perimeter if it is very small
	vector<vector<Point> > filtercontours;
	for (int i = 0; i < contours.size(); i++)
	{
		if (arcLength(contours[i], true) > swimming_pool_common.getContourMin())
			filtercontours.push_back(contours[i]);
	}

	/// Find the rotated rectangles for each contour
	vector<RotatedRect> minRect(filtercontours.size());
	//vector<RotatedRect> minRect_filter;
	for (int i = 0; i < filtercontours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(filtercontours[i]));
	}
	
	//if there is more than one swimming pool, check if is the same
	if (minRect.size()>1)
	{
		int n = minRect.size();
		for (int c = 0; c < (n - 1); c++)
		{
			for (int d = 0; d < n - c - 1; d++)
			{
				if (abs(minRect[d].center.x - minRect[d + 1].center.x)<2)		//If the centers are similar (error 2px), erase one
				{
					minRect.erase(minRect.begin() + d);
					n--;	//Finish before the cycle because i am erasing one element
				}
			}
		}
	}

	//Push swimming pools
	for (int i = 0; i < minRect.size(); i++)
	{
		// rotated rectangle
		Point2f rect_points[4];
		minRect[i].points(rect_points);

		Point p1[4], p2[4];
		for (int j = 0; j < 4; j++)
		{
			p1[j] = rect_points[j];
			p2[j] = rect_points[(j + 1) % 4];

		}

		SwimmingPool swimming_pool_found;

		int widthpool, heightpool;
		if (minRect[i].size.height >  minRect[i].size.width)
		{
			heightpool = minRect[i].size.height;
			widthpool = minRect[i].size.width;
		}
		else
		{
			heightpool = minRect[i].size.width;
			widthpool = minRect[i].size.height;
		}

		swimming_pool_found.setPoolSizePx(Size(widthpool, heightpool));
		swimming_pool_found.setPoolRectangle(p1, p2);	//Set the points for draw the rectangle
		swimming_pool_found.setPoolCenter(minRect[i].center);
		swimming_pools.push_back(swimming_pool_found);
	}

	//Mat image2print = image.data;
	Mat image2print = image.clone();
	//Draw swimming pools
	for (int i = 0; i < swimming_pools.size(); i++)
	{
		//Draw the pool borders in red
		Scalar color = Scalar(0, 0, 255);

		// contour
		//drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());

		for (int j = 0; j < 4; j++)
		{
			Point p1, p2;
			p1 = swimming_pools[i].p1_[j];
			p2 = swimming_pools[i].p2_[j];

			line(image2print, p1, p2, color, 2, 8);
		}
	}

	//SCALE
	//We are interested in the bottom-right place in the image
	Mat googleScale = Mat(src, Rect(1190, 757, src.size().width - 1190, src.size().height - 757));
	blur(googleScale, googleScale, Size(3, 3));

	//The line is gray-white always
	Mat rangeoutput;
	cv::inRange(googleScale, Scalar(80, 80, 80), Scalar(255, 255, 255), rangeoutput);

	//Read the number of the scale. Initially, I supussed 100 meters
	//Measure the pixels of the scale
	Mat googleScaleProcess;
	int valuecanny = 200;
	Canny(googleScale, googleScaleProcess, 150, valuecanny * 3, 3);

	vector<Vec4i> lines;
	HoughLinesP(googleScaleProcess, lines, 1, CV_PI / 180, 100, 20, 200);

	//It's almost one line. Transform in one (average)
	Vec4i measureVec(0, 0, 0, 0);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		//line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
		measureVec = measureVec + l;
	}
	for (int i = 0; i < 4; i++)
	{
		measureVec[i] = measureVec[i] / lines.size();
	}

	float measure_scale_px;
	measure_scale_px = sqrt((measureVec[3] - measureVec[1])*(measureVec[3] - measureVec[1]) 
		+ (measureVec[2] - measureVec[0])*(measureVec[2] - measureVec[0]));
	
	//Convert swimming pool size from pixel to meters.
	//If measure_scale_px (pixel) are 100 meters, the measure pixel pool are...

	for (int i = 0; i < swimming_pools.size(); i++)
	{
		//Calculate the real size (in meters)
		Size real_size = swimming_pools[i].TransformPxToMeters(measure_scale_px);

		//Print in console
		cout << "Swimming pool number "<<i+1<<" has " << real_size.height
			<< " meters of large and " << real_size.width << " meters of width." << endl;

		//Print in the image
		std::string poolmeasure = "LxW: " + std::to_string(real_size.height) + "x" + std::to_string(real_size.width);
		int scaletext = 2;
		int thicktext = 3;
		int linetypetext = 8;
		Point2f center;
		center = swimming_pools[i].getPoolCenter();
		
		//if the text is so much at right... correct it
		if (center.x > 900)
			center.x = 900;
		//if there are two pools and the centers are so close, separate.
		if (swimming_pools.size() > 1)
		{
			if ((swimming_pools[1].getPoolCenter().y - swimming_pools[0].getPoolCenter().y) < 50)
				if (i == 1)	//in the second iteration
					center.y -= 100;
		}
		putText(image2print, poolmeasure, center, CV_FONT_HERSHEY_COMPLEX, scaletext,
			Scalar(0, 0, 255), scaletext, linetypetext);
	}

	/// Show in a window
	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Original", 0, 0);
	resize(image, image, Size(image.cols / 2, image.rows / 2));
	imshow("Original", image);

	namedWindow("Result", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Result", 680, 0);	//half of my screen
	resize(image2print, image2print, Size(image2print.cols / 2, image2print.rows / 2));
	imshow("Result", image2print);


	waitKey(0);                                          // Wait for a keystroke in the window
	return 0;
}
