#include "ImageTrans.hpp"

_stdcall ColorTrans::ColorTrans(HWND hwnd,const IO *obj)
{

	this->obj = obj;
	this->ParenthWnd = hwnd;
	std::string temp = obj->GetTmpName();
	std::string ext;
	auto iter = std::find(temp.begin(), temp.end(), '.');
	for (; iter != temp.end(); ++iter)
		ext += *iter;
	this->Imgextension = ext;
	this->temp = imread("tmp1" + ext);
	if (this->temp.channels() < 3)
		this->Color = GRAY;
	else
		this->Color = RGB;
	try
	{
		if (this->temp.data == NULL)
			throw "Temp File has broken!";
	}
	catch(const char *e)
	{
		MessageBoxA(hwnd, e, "Error!", MB_OK | MB_ICONERROR);
	}
}

ColorTrans::ColorTrans(HWND hwnd, const IO obj)
{
	ColorTrans(hwnd, &obj);
}

inline void ColorTrans::Write2Tmp()const
{
	imwrite("tmp1" + this->Imgextension, this->temp);
	//error
	//if (GetFileAttributesA(("tmp1" + this->Imgextension).c_str()) != FILE_ATTRIBUTE_HIDDEN)//Judge if hidden attribute
	//	SetFileAttributesA(("tmp1" + this->Imgextension).c_str(), FILE_ATTRIBUTE_HIDDEN);
}

void ColorTrans::RGB2Gray()
{
	if (this->Color != RGB)
		return;
	cv::cvtColor(temp, temp, CV_RGB2GRAY);
	Write2Tmp();
	this->Color = GRAY;
}

void ColorTrans::RGB2HSV()	
{
	if (this->Color != RGB)
		return;
	cv::cvtColor(temp, temp, CV_RGB2HSV);
	Write2Tmp();
	this->Color = HSV;
}

void ColorTrans::HSV2RGB()
{
	if (this->Color != HSV)
		return;
	cv::cvtColor(temp, temp, CV_HSV2RGB);
	Write2Tmp();
	this->Color = RGB;
}

void ColorTrans::Gray2RGB()
{
	if (this->Color != GRAY)
		return;
	cv::cvtColor(temp, temp, CV_GRAY2RGB);
	Write2Tmp();
	this->Color = RGB;
}

MatND ColorTrans::CalcHistogram()const
{
	MatND hist;
	if (this->Color != GRAY)
	{
		MessageBox(this->ParenthWnd, L"请先把图片转为灰度空间", L"提示", MB_OK | MB_ICONINFORMATION);
		hist.data = NULL;
		return hist;
	}
	const int channels[] = { 0 };
	const int histSize[] = { 256 };
	float hranges[2] = { 0.0,256.0 };
	const float* ranges[] = { hranges };
	calcHist(&temp, 1, channels, Mat(), hist,1, histSize, ranges);
	return hist;
}

void ColorTrans::DisplayHistogram(MatND hist)const
{
	if (hist.data == NULL)
		return;
	double maxVal = 0;
	double minVal = 0;

	//找到直方图中的最大值和最小值
	minMaxLoc(hist, &minVal, &maxVal, 0, 0);
	int histSize = hist.rows;
	Mat histImg(histSize, histSize, CV_8U, Scalar(255));
	// 设置最大峰值为图像高度的90%
	int hpt = static_cast<int>(0.9*histSize);

	for (int h = 0; h < histSize; h++)
	{
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		line(histImg, Point(h, histSize), Point(h, histSize - intensity), Scalar::all(0));
	}
	imshow("Histogram", histImg);
	waitKey(0);
	destroyWindow("Histogram");
}

void ColorTrans::HistBalance()
{
	if (this->Color != GRAY)
	{
		MessageBox(this->ParenthWnd, L"请先把图片转为灰度空间", L"提示", MB_OK | MB_ICONINFORMATION);
		return;
	}
	cv::equalizeHist(this->temp, this->temp);
	this->Write2Tmp();
}

void ColorTrans::ShowEdge(int code)const
{
	Mat tedge;//temp edge matrix
	switch (code)
	{
		//1 and 2 is equal to Canny algorithm and Laplacian algorithm
	case 1:
		tedge = obj->GetImage();
		if (tedge.channels() == 3)
			cvtColor(tedge, tedge, CV_BGR2GRAY);
		Canny(tedge, tedge, 200, 300);
		imshow("边缘", tedge);
		break;
	case 2:
		tedge = obj->GetImage();
		if (tedge.channels() == 3)
			cvtColor(tedge, tedge, CV_BGR2GRAY);
		Laplacian(tedge, tedge, CV_8U, 5);
		imshow("边缘", tedge);
		break;
	}
}

void ColorTrans::Threshold(int thval)
{
	if (this->Color != GRAY)
		MessageBox(ParenthWnd, L"请先灰度化", L"提示", MB_OK | MB_ICONINFORMATION);
	else
	{
		threshold(temp, temp, thval, 255, THRESH_BINARY);
		Write2Tmp();
	}
}