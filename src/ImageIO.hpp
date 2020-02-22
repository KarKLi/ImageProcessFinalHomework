#pragma once
#pragma warning(disable:4267)
#include <iostream>
#include <string>
#include <iterator>
#include <Windows.h>
#include <commdlg.h>
#include <comdef.h>
#include <gdiplus.h>
#include <process.h>
#pragma comment(lib,"gdiplus.lib")
#include "highgui.hpp"
#include "opencv.hpp"

using namespace std;
using namespace cv;

typedef unsigned int uint;

struct Coor
{
	uint width;
	uint height;
};
class IO
{
private:
	Mat Image;
	string Imgname;
	string tempname;
	wchar_t wImgname[100] = { 0 };
	static OPENFILENAME ofn;
	uint err_code;
	HWND ParentHwnd;
	Coor coor;

public:
	bool drawed;
	IO(HWND hwnd) :Image(),Imgname(""),err_code(0),ParentHwnd(hwnd) {}
	IO(HWND hwnd, std::string name);
	IO(const IO& obj);

	//The convertion of wchar_t pointer and char pointer
	void wideCharToMultiByte(std::string &str, const wchar_t * pWCStrKey);
	char *wideCharToMultiByte(const wchar_t *wchar);
	void multiByteToWideChar(std::string &cchar, wchar_t *pWCStr);
	wchar_t *multiByteToWideChar(const std::string cchar);

	uint GetErrorCode()const { return err_code; }
	Mat GetImage()const { return this->Image; }
	const char *GetImageName()const { return Imgname.c_str(); }
	void OpenImage();
	void OpenImage(const wchar_t *);
	void OpenImage(const char *);
	void LoadTempSize();
	void ShowImage(const wchar_t *, HDC);
	string GetTmpName()const { return tempname; }
	Coor GetCoor()const;
	void SaveImage();
};
//ImageIO.hpp end