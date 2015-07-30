#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class SwimmingPool
{
public:
	SwimmingPool();
	~SwimmingPool();

	//getters
	cv::Size2i getPoolSizePx();
	cv::Size2f getPoolSizeMeters();
	cv::Scalar getLowerColor();
	cv::Scalar getUpperColor();
	int getContourMin();
	cv::Point2f getPoolCenter();

	//setters
	void setPoolSizePx(cv::Size2i size_px);
	void setPoolRectangle(cv::Point p1[4], cv::Point p2[4]);
	void setPoolCenter(cv::Point2f center);

	//other functions
	cv::Size2f TransformPxToMeters(float scale);

	//public variable
	cv::Point p1_[4];		//points for the contour
	cv::Point p2_[4];

private:
	//Variable
	cv::Scalar lower_color_;		//minimum color to detect for the swimming pool
	cv::Scalar upper_color_;		//max color
	int contour_min_;		//min contour in px of the pool
	int width_px_;
	int length_px_;
	int width_meters_;
	int length_meters_;
	cv::Point2f center_point_;
};

