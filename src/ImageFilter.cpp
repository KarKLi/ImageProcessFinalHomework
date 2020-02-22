#include "ImageFilter.hpp"
void Filter::filter2D(FilterType type,int ksize)
{
	switch (type)
	{
	case FilterType::AVERAGE:
		blur(source, dst, Size_<int>(ksize, ksize));
		break;
	case FilterType::MEDIUM:
		medianBlur(source, dst, ksize);
		break;
	case FilterType::GAUSSIAN:
		GaussianBlur(source, dst, Size_<int>(ksize, ksize), 0, 0);
		break;
	case FilterType::SDERIVATIVE:
		Mat ker = (Mat_<char>(3, 3) << -1, -1, -1,
			-1, 8, -1,
			-1, -1, -1);
		cvtColor(source, dst, CV_BGR2GRAY);
		cv::filter2D(dst, dst, -1, ker);
		break;
	}
}