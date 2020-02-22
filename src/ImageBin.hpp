#pragma once
#include "ImageIO.hpp"
class BinMorphology {
private:
	Mat src;
	Mat dst;
public:
	BinMorphology(const IO* obj)
	{
		src = obj->GetImage();
	}
	void Erode(int ksize = 3);
	void Dilate(int ksize = 3);
	void Open(int ksize = 3);
	void Close(int ksize = 3);
	void showMorphologyImg()const;
};
//ImageBin.hpp end