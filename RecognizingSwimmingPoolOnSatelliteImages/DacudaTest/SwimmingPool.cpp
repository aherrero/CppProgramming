#include "SwimmingPool.h"


SwimmingPool::SwimmingPool()
{
	//Default params.
	lower_color_ = cv::Scalar(170, 138, 37);
	upper_color_ = cv::Scalar(235, 230, 150);
	contour_min_ = 70;

	//initialization
	width_px_ = 0;
	length_px_ = 0;
	width_meters_ = 0;
	length_meters_ = 0;
	center_point_ = cv::Point2f(0, 0);
}


SwimmingPool::~SwimmingPool()
{

}

cv::Size2i SwimmingPool::getPoolSizePx()
{
	return cv::Size(width_px_, length_px_);
}
cv::Size2f SwimmingPool::getPoolSizeMeters()
{
	return cv::Size(width_meters_, length_meters_);
}
cv::Scalar SwimmingPool::getLowerColor()
{
	return lower_color_;
}
cv::Scalar SwimmingPool::getUpperColor()
{
	return upper_color_;
}
int SwimmingPool::getContourMin()
{
	return contour_min_;
}

void SwimmingPool::setPoolSizePx(cv::Size2i size_px)
{
	width_px_ = size_px.width;
	length_px_ = size_px.height;
}

void SwimmingPool::setPoolRectangle(cv::Point p1[4], cv::Point p2[4])
{
	for (int i = 0; i < 4; i++)
	{
		p1_[i] = p1[i];
		p2_[i] = p2[i];
	}

}

cv::Size2f SwimmingPool::TransformPxToMeters(float scale)
{
	width_meters_ = width_px_ * 100.0 / scale;
	length_meters_ = length_px_ * 100.0 / scale;
	return cv::Size(width_meters_, length_meters_);
}

void SwimmingPool::setPoolCenter(cv::Point2f center)
{
	center_point_ = center;
}
cv::Point2f SwimmingPool::getPoolCenter()
{
	return center_point_;
}

