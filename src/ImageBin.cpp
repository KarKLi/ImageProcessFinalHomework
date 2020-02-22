#include "ImageBin.hpp"

//ksize is the size of structure element
void BinMorphology::Erode(int ksize)
{
	auto element = getStructuringElement(MORPH_RECT, cv::Size_<int>(ksize,ksize));
	erode(this->src, this->dst, element);
}

void BinMorphology::Dilate(int ksize)
{
	auto element = getStructuringElement(MORPH_RECT, cv::Size_<int>(ksize, ksize));
	dilate(this->src, this->dst, element);
}

void BinMorphology::Open(int ksize)
{
	auto element = getStructuringElement(MORPH_RECT, cv::Size_<int>(ksize, ksize));
	morphologyEx(this->src, this->dst, MORPH_OPEN, element);
}

void BinMorphology::Close(int ksize)
{
	auto element = getStructuringElement(MORPH_RECT, cv::Size_<int>(ksize, ksize));
	morphologyEx(this->src, this->dst, MORPH_CLOSE, element);
}

void BinMorphology::showMorphologyImg()const
{
	imshow("二值形态学处理后", this->dst);
}