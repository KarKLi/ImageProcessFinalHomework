#include "ImageSize.hpp"
ImgSize::ImgSize(const IO *obj)
{
	img = obj->GetImage();
	//Get extension name
	std::string temp = obj->GetTmpName();
	std::string ext;
	auto iter = std::find(temp.begin(), temp.end(), '.');
	for (; iter != temp.end(); ++iter)
		ext += *iter;
	this->tmpext = ext;
	this->obj = const_cast<IO *>(obj);
}

void ImgSize::Write2Tmp()const
{
	imwrite("tmp1" + this->tmpext, this->img);
}

void ImgSize::Resize(int rows,int cols)
{
	resize(this->img, this->img, Size_<int>(rows, cols));
	Write2Tmp();
}

void ImgSize::GetCutImageRect(int x1, int y1, int x2, int y2)
{
	Rect rect(x1, y1,x2-x1,y2-y1);
	tmp = img(rect);
}

void ImgSize::showAfterCutImage()const
{
	imshow("裁剪后的效果", tmp);
	int res = MessageBox(NULL, L"是否保存？", L"保存吗？", MB_OKCANCEL);
	if (res == IDOK)
	{
		OPENFILENAME sfn;
		wchar_t *fname = new wchar_t[200];
		wchar_t *extension = new wchar_t[30];
		wchar_t *currentPath = new wchar_t[MAX_PATH];
		size_t converted = 0;
		mbstowcs_s(&converted, extension, this->tmpext.length() + 1,this->tmpext.c_str(), _TRUNCATE);
		std::wstring filter;
		for (int i = 1; i < wcslen(extension); i++)
			filter += extension[i];
		filter = filter + L" File(*." + filter + L")";
		filter += L'\0';
		filter += L"*";
		for (int i = 0; i < wcslen(extension); i++)
			filter += extension[i];
		filter += L'\0';
		memset(fname, 0, sizeof(wchar_t) * 200);
		memset(currentPath, 0, sizeof(wchar_t)*MAX_PATH);
		memset(&sfn, 0, sizeof(OPENFILENAME));
		sfn.lStructSize = sizeof(sfn);
		sfn.hwndOwner = NULL;
		sfn.lpstrFilter = filter.c_str();
		sfn.lpstrInitialDir = L"./";
		sfn.lpstrFile = fname;
		sfn.nMaxFile = MAX_PATH;
		sfn.nFilterIndex = 0;
		sfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT; //标志如果是多选要加上OFN_ALLOWMULTISELECT
		if (!GetSaveFileName(&sfn))
			return;
		char *filename = new char[100];
		memset(filename, 0, sizeof(char) * 100);
		wcstombs_s(&converted, filename,wcslen(fname)+1, fname, _TRUNCATE);
		for (int i = wcslen(fname) - 1; i > wcslen(fname) - 1 - wcslen(extension); i--)//check the last extension is the same as the original file extension
			if (fname[i] != extension[wcslen(extension) - 1-(wcslen(fname)-1-i)])
			{
				imwrite(filename + this->tmpext, this->tmp);
				MessageBox(NULL, L"保存成功", L"成功", MB_OK | MB_ICONINFORMATION);
				break;
			}
			else if (i == wcslen(fname) - wcslen(extension))
			{
				imwrite(filename, this->tmp);
				MessageBox(NULL, L"保存成功", L"成功", MB_OK | MB_ICONINFORMATION);
				break;
			}
		
		delete[] filename;
		delete[] fname;
		delete[] currentPath;
		delete[] extension;
	}
	else return;
}