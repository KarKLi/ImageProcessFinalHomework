#pragma once
#include "ImageIO.hpp"
class ImgSize {
private:
	Mat img;
	Mat tmp;
	std::string tmpext;
	inline void Write2Tmp()const;
	IO *obj;
public:
	ImgSize(const IO *obj);
	void Resize(int rows, int cols);
	void GetCutImageRect(int x1, int y1, int x2, int y2);
	void showAfterCutImage()const;
};