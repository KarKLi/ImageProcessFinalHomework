#include "ImageIO.hpp"
using namespace Gdiplus;
OPENFILENAME IO::ofn = { 0 };

IO::IO(HWND ParentHwnd, std::string name)
{
	err_code = 0;
	ParentHwnd = ParentHwnd;
	try
	{
		Image = imread(name);
		if (Image.data == NULL)
			throw "Open Image file failed!";
	}
	catch (char *e)
	{
		_STL_ASSERT(NULL, e);
	}

}

IO::IO(const IO& obj)
{
	this->Image = obj.Image;
	this->Imgname = obj.Imgname;
	this->ofn = obj.ofn;
	this->err_code = obj.err_code;
	this->ParentHwnd = obj.ParentHwnd;
	wcscpy(this->wImgname, obj.wImgname);
	this->coor = obj.coor;
}

//This four functions provide the way of wchar_t * and char *'s transform, which adapt the Windows API and Opencv API
void IO::wideCharToMultiByte(std::string &str,const wchar_t * pWCStrKey)
{
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	str = pCStrKey;
	delete pCStrKey;
}

void IO::multiByteToWideChar(std::string &cchar,wchar_t *pWCStr)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar.c_str(), cchar.length(), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar.c_str(), cchar.length(), m_wchar, len);
	m_wchar[len] = '\0';
	wcscpy(pWCStr, m_wchar);
	delete m_wchar;
}

char *IO::wideCharToMultiByte(const wchar_t *wchar)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

wchar_t *IO::multiByteToWideChar(const std::string cchar)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar.c_str(), cchar.length(), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar.c_str(),cchar.length(), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

void IO::OpenImage()
{
	wchar_t fpath[100] = { 0 };
//Initalize the struct ofn
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ParentHwnd;
	ofn.lpstrFilter = L"所有文件(All Files)\0*.*\0\0";
	ofn.lpstrInitialDir = L"./";
	ofn.lpstrFile = fpath;
	ofn.nMaxFile = sizeof(fpath) / sizeof(*fpath);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (!GetOpenFileName(&ofn))
	{
		err_code = 1;//Didn't choose file
	}
	else
	{
		wcscpy(wImgname, fpath);
		wideCharToMultiByte(Imgname, fpath);
		Image = imread(Imgname);
		auto iter = std::find(Imgname.begin(), Imgname.end(), '.');
		std::string extension;//fill the extension, for instance: .bmp
		for (; iter != Imgname.end(); ++iter)
			extension += *iter;
		tempname = "tmp1" + extension;
		imwrite(tempname, Image);
		try
		{
			if (Image.data == NULL)
				throw "Open Image file failed!";
		}
		catch (char *e)
		{
			_STL_ASSERT(NULL, e);
			err_code = 2;//No such file or directory or image broken
		}
		
		Gdiplus::Image img(wImgname);
		if (img.GetLastStatus() != Gdiplus::Status::Ok)
		{
			MessageBox(ParentHwnd, L"加载图片失败!", L"提示", MB_OK);
			err_code = 2;//No such file or directory or image broken
		}
		this->coor.width = img.GetWidth();
		this->coor.height = img.GetHeight();
	}
//loading end
}

void IO::OpenImage(const wchar_t *filename)
{
	size_t converted = 0;
	char *buff = new char[50];
	wcstombs_s(&converted, buff,50, filename,_TRUNCATE);
	this->Image=imread(buff);
	this->Imgname = buff;
	wsprintf(this->wImgname, filename);
	this->coor.width = Image.cols;
	this->coor.height = Image.rows;
	delete[] buff;
}

void IO::OpenImage(const char *filename)
{
	size_t converted = 0;
	wchar_t *buff = new wchar_t[50];
	mbstowcs_s(&converted, buff, 50, filename, _TRUNCATE);
	this->Image = imread(filename);
	this->Imgname = filename;
	wsprintf(this->wImgname,buff);
	this->coor.width = Image.cols;
	this->coor.height = Image.rows;
	delete[] buff;
}
void IO::ShowImage(const wchar_t *filename,HDC hdc)
{
	RECT rect;
	GetWindowRect(ParentHwnd, &rect);
	Gdiplus::Image img(filename);
	if (img.GetLastStatus() != Gdiplus::Status::Ok)
	{
		MessageBox(ParentHwnd, L"加载图片失败!", L"提示", MB_OK);
		err_code = 2;//No such file or directory or image broken
	}
	Gdiplus::Graphics graphics(hdc);
	graphics.DrawImage(&img, 0, 0, this->coor.width, this->coor.height);
	drawed = true;
}

void IO::LoadTempSize()
{
	Mat tmp = imread(this->GetTmpName());
	this->coor.height = tmp.rows;
	this->coor.width = tmp.cols;
}

Coor IO::GetCoor()const
{
	return coor;
}

void IO::SaveImage()
{
	if (lstrlenW(wImgname) == 0)
	{
		MessageBox(ParentHwnd, L"请先打开文件！", L"警告！", MB_OK | MB_ICONWARNING);
		return;
	}
	//save file struct
	OPENFILENAME sfn;
	wchar_t *fname = new wchar_t[200];
	wchar_t *extension = new wchar_t[30];
	wchar_t *currentPath = new wchar_t[MAX_PATH];
	auto iter = std::find(Imgname.begin(), Imgname.end(), '.');
	std::string ext;
	for (; iter != Imgname.end(); ++iter)
		ext += *iter;
	multiByteToWideChar(ext, extension);
	std::wstring filter;
	for (int i = 1; i < wcslen(extension); i++)
		filter += extension[i];
	filter = filter + L" File(*." + filter + L")";
	filter += L'\0';
	filter += L"*";
	for (int i = 0; i < wcslen(extension); i++)
		filter += extension[i];
	filter += L'\0';
	memset(fname, 0, sizeof(wchar_t)*200);
	memset(currentPath, 0, sizeof(wchar_t)*MAX_PATH);
	memset(&sfn, 0, sizeof(OPENFILENAME));
	sfn.lStructSize = sizeof(sfn);
	sfn.hwndOwner = ParentHwnd;
	sfn.lpstrFilter = filter.c_str();
	sfn.lpstrInitialDir = L"./";
	sfn.lpstrFile = fname;
	sfn.nMaxFile = MAX_PATH;
	sfn.nFilterIndex = 0;
	sfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT; //标志如果是多选要加上OFN_ALLOWMULTISELECT
	if (!GetSaveFileName(&sfn))
		return;
	//TODO：解决多重扩展名的问题（在选择一个文件之后，不执行wcsncat操作）
	//Final：已解决
	auto location=wcsstr(fname, extension);
	if(location==NULL)
		wcsncat(fname, extension, wcslen(extension));
	wcsncat(currentPath, L"tmp1", wcslen(L"tmp1"));
	wcsncat(currentPath, extension, wcslen(extension));
	CopyFile(currentPath, fname, FALSE);
	delete[] fname;
	delete[] currentPath;
	delete[] extension;
	MessageBox(this->ParentHwnd, L"保存成功！",L"提示", MB_OK | MB_ICONINFORMATION);
}