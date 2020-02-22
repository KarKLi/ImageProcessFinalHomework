#pragma once
#include "ImageIO.hpp"
class FT {
private:
	Mat Img;
	Mat planes[2];
	Mat padded;
	Mat complexI;
	Mat magnitudeImg;
	//Mat phase;
	int m, n;//the size of enlarge
public:
	FT(const IO* obj)
	{
		Img = (obj->GetImage());
		cvtColor(Img, Img, CV_BGR2GRAY);
	}

	void dft();

	void showAmplitude()const
	{
		imshow("·ù¶ÈÍ¼",magnitudeImg);
	}
	void showCentralAmplitude()const;

	void showPhase()const
	{
		imshow("ÏàÎ»Æ×Í¼", cv::Mat((planes[1] / planes[0])));
	}
};