#pragma once
#include "ImageIO.hpp"
enum FilterType { AVERAGE, MEDIUM,GAUSSIAN,SDERIVATIVE };
class Filter
{
private:
	Mat source;
	Mat dst;
public:
	Filter(const IO *obj)
	{
		source = obj->GetImage();
	}
	void filter2D(FilterType type, int ksize = 3);
	void showAfterFilt()const
	{
		imshow("ÂË²¨Ö®ºó", dst);
	}
};