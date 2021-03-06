// 图像处理大作业.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "图像处理大作业.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
IO *obj=nullptr;
ColorTrans *ctrans=nullptr;
FT *ft = nullptr;
Filter *fil = nullptr;
BinMorphology *bm = nullptr;
ImgSize *imsize = nullptr;
extern bool drawed;
ULONG_PTR m_gdiplusToken;
Coor screenshot[2] = { {0,0},{0,0} };
static int status = -1;//screenshot status,0 is first coor, 1 is second
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Threshold(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Zoom(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
	// 初始化GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }



    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   obj = new IO(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
HWND CreateStatusBar(HWND hParentWnd)
{
#define PANEL_NUM 2
	static int array[PANEL_NUM] = {350,-1 };
	HINSTANCE hInst = GetModuleHandle(NULL);
	//创建Statusbar控件
	HWND hWndStatus = CreateWindowEx(0, STATUSCLASSNAME, TEXT(""), WS_CHILD | WS_BORDER | WS_VISIBLE, 0, 0, 0, 0, hParentWnd, (HMENU)IDC_STATUSBAR, hInst, NULL);
	if (hWndStatus)
	{
		SendMessage(hWndStatus, SB_SETPARTS, (WPARAM)PANEL_NUM, (LPARAM)array); //设置面板个数
		SendMessage(hWndStatus, SB_SETTEXT, (LPARAM)1, (WPARAM)TEXT("当前未加载任何图片")); //设置第二个面板内容
	}
#undef PANEL_NUM

	return hWndStatus;
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hStatusbar;
    switch (message)
    {
	case WM_CREATE:
	{
		hStatusbar = CreateStatusBar(hWnd);
		LPWIN32_FIND_DATA fd = new _WIN32_FIND_DATAW;
		memset(fd, 0, sizeof(LPWIN32_FIND_DATA));
		if (FindFirstFile(L"miao.bmp", fd) == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, L"组件缺失！封面图无法加载，程序即将退出", L"错误", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
			case IDM_OPEN:
				if (ctrans != nullptr)
				{
					delete ctrans;
					ctrans = nullptr;
				}
				if (ft != nullptr)
				{
					delete ft;
					ft = nullptr;
				}
				if (fil != nullptr)
				{
					delete fil;
					fil = nullptr;
				}
				if (bm != nullptr)
				{
					delete bm;
					bm = nullptr;
				}
				if (imsize != nullptr)
				{
					delete imsize;
					imsize = nullptr;
				}
				obj->OpenImage();
				if (obj->GetErrorCode() == 1)
					break;
				ctrans = new ColorTrans(hWnd, obj);
				ft = new FT(obj);
				fil = new Filter(obj);
				bm = new BinMorphology(obj);
				imsize = new ImgSize(obj);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case IDM_SAVE:
				obj->SaveImage();
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_RGB2GRAY:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->RGB2Gray();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_RGB2HSV:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->RGB2HSV();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_HSV2RGB:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->HSV2RGB();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_GRAY2RGB:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->Gray2RGB();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_DPHIS:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->DisplayHistogram(ctrans->CalcHistogram());
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_HISBALANCE:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->HistBalance();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_AMP:
				if (strlen(obj->GetImageName()) > 0)
				{
					ft->dft();
					ft->showAmplitude();
				}
				break;
			case ID_CENTRAL:
				if (strlen(obj->GetImageName()) > 0)
				{
					ft->dft();
					ft->showCentralAmplitude();
				}
				break;
			case ID_PHASE:
				if (strlen(obj->GetImageName()) > 0)
				{
					ft->dft();
					ft->showPhase();
				}
				break;
			case ID_AVERAGE:
				if (strlen(obj->GetImageName()) > 0)
				{
					fil->filter2D(AVERAGE,7);
					fil->showAfterFilt();
				}
				break;
			case ID_MEDIUM:
				if (strlen(obj->GetImageName()) > 0)
				{
					fil->filter2D(MEDIUM,7);
					fil->showAfterFilt();
				}
				break;
			case ID_GAUSSIAN:
				if (strlen(obj->GetImageName()) > 0)
				{
					fil->filter2D(GAUSSIAN,7);
					fil->showAfterFilt();
				}
				break;
			case ID_SDERIVATIVE:
				if (strlen(obj->GetImageName()) > 0)
				{
					fil->filter2D(SDERIVATIVE, 3);
					fil->showAfterFilt();
				}
				break;
			case ID_CANNY:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->ShowEdge(1);
				}
				break;
			case ID_LAPLACIAN:
				if (strlen(obj->GetImageName()) > 0)
				{
					ctrans->ShowEdge(2);
				}
				break;
			case IDM_THRESHOLD:
				if (strlen(obj->GetImageName()) > 0)
				{
					if (ctrans->GetColorSpace() != GRAY)
					{
						MessageBox(hWnd, L"请先灰度化", L"提示", MB_OK | MB_ICONINFORMATION);
						break;
					}
					DialogBox(hInst, MAKEINTRESOURCE(IDD_THRESHOLD), hWnd, Threshold);
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_BIN_3_ERODE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Erode(3);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_5_ERODE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Erode(5);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_7_ERODE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Erode(7);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_3_DILATE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Dilate(3);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_5_DILATE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Dilate(5);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_7_DILATE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Dilate(7);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_3_OPEN:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Open(3);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_5_OPEN:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Open(5);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_7_OPEN:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Open(7);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_3_CLOSE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Close(3);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_5_CLOSE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Close(5);
					bm->showMorphologyImg();
				}
				break;
			case ID_BIN_7_CLOSE:
				if (strlen(obj->GetImageName()) > 0)
				{
					bm->Close(7);
					bm->showMorphologyImg();
				}
				break;
			case ID_ZOOM:
				if (strlen(obj->GetImageName()) > 0)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ZOOM), hWnd, Zoom);

					wchar_t *tmp = new wchar_t[100];
					tmp=obj->multiByteToWideChar(obj->GetTmpName());
					obj->LoadTempSize();
					obj->ShowImage(tmp, GetDC(hWnd));
					obj->OpenImage(obj->GetTmpName().c_str());
					delete tmp;
					delete ctrans;
					delete ft;
					delete fil;
					delete bm;
					delete imsize;
					//New initalize for the new picture
					ctrans = new ColorTrans(hWnd, obj);
					ft = new FT(obj);
					fil = new Filter(obj);
					bm = new BinMorphology(obj);
					imsize = new ImgSize(obj);
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_CUT:
				if (strlen(obj->GetImageName()) > 0)
				{
					status = 0;
					int res=MessageBox(hWnd, L"在按下确定之后，移动鼠标到您要开始截图的点，按下左键。", L"截图开始", MB_OKCANCEL);
					if (res == IDCANCEL)
						status = -1;
				}
				break;
			case ID_GETSIZE:
				if (strlen(obj->GetImageName()) > 0)
				{
					auto coo = obj->GetCoor();
					wchar_t buff[30];
					wsprintf(buff, L"该图片的大小为%d * %d", coo.width, coo.height);
					MessageBox(hWnd, buff, L"尺寸", MB_OK | MB_ICONINFORMATION);
				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_SIZE:
	{
		SendMessage(hStatusbar, WM_SIZE, 0, 0);
		break;
	}
	break;
	case WM_LBUTTONDOWN:
		if (status == 0)
		{
			screenshot[0] = { LOWORD(lParam), HIWORD(lParam) };
			status++;
		}
		else if (status == 1)
		{
			screenshot[1] = { LOWORD(lParam),HIWORD(lParam) };
			status++;
			MessageBox(hWnd, L"截图结束", L"截图结束", MB_OK);
			wchar_t tmp[100];
			wsprintf(tmp, L"本次截图已完成，截得图片大小为：%d * %d", (screenshot[1].width - screenshot[0].width), (screenshot[1].height - screenshot[0].height));
			MessageBox(NULL, tmp, L"截图报告", MB_OK | MB_ICONINFORMATION);
			imsize->GetCutImageRect(screenshot[0].width, screenshot[0].height, screenshot[1].width, screenshot[1].height);
			imsize->showAfterCutImage();
		}
		break;
	case WM_MOUSEMOVE:
	{
		TCHAR szBuf[MAX_PATH];
		_stprintf(szBuf, TEXT("Mouse(%d,%d)"), LOWORD(lParam), HIWORD(lParam));
		SendMessage(hStatusbar, SB_SETTEXT, 0, (LPARAM)(LPSTR)szBuf);
	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			if (strlen(obj->GetImageName()) > 0/*&&obj->drawed==false*/)
			{
				Coor coor = obj->GetCoor();
				RECT rect;
				GetWindowRect(hWnd, &rect);
				if (!MoveWindow(hWnd, rect.left, rect.top, coor.width,coor.height + 83, TRUE))
					break;
				std::string name = obj->GetTmpName();
				obj->LoadTempSize();
				obj->ShowImage(obj->multiByteToWideChar(name),hdc);
				//InvalidateRect(hWnd, NULL, TRUE);
				int array[2] = { obj->GetCoor().width / 2,-1 };
				SendMessage(hStatusbar, SB_SETPARTS, (WPARAM)2, (LPARAM)array); //设置面板个数
				TCHAR szBuf[MAX_PATH];
				switch (ctrans->GetColorSpace())
				{
				case ColorSpace::GRAY:
					_stprintf(szBuf, TEXT("当前图片色彩空间为：%s"),L"GRAY");
					break;
				case ColorSpace::RGB:
					_stprintf(szBuf, TEXT("当前图片色彩空间为：%s"), L"RGB");
					break;
				case ColorSpace::HSV:
					_stprintf(szBuf, TEXT("当前图片色彩空间为：%s"), L"HSV");
					break;
				default:
					_stprintf(szBuf, TEXT("当前图片色彩空间为：%s"), L"未知");
					break;
				}
				SendMessage(hStatusbar, SB_SETTEXT, 1, (LPARAM)(LPSTR)szBuf);
			}
			else//load default picture
			{

				Gdiplus::Image img(L"miao.bmp");
				auto status = img.GetLastStatus();
				if (status != Gdiplus::Status::Ok)
				{
					MessageBox(hWnd, L"加载图片失败!", L"提示", MB_OK);
				}
				RECT rect;
				GetWindowRect(hWnd, &rect);
				if (!MoveWindow(hWnd, rect.left, rect.top, img.GetWidth(), img.GetHeight()+83, TRUE))
					break;
				Gdiplus::Graphics graphics(hdc);
				graphics.DrawImage(&img,0,0,img.GetWidth(),img.GetHeight());
			}
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_DESTROY:
		remove(obj->GetTmpName().c_str());
		if (obj == nullptr&&ctrans == nullptr&&ft == nullptr&&fil == nullptr&&imsize == nullptr)
			PostQuitMessage(0);
		delete obj;
		obj = nullptr;
		delete ctrans;
		ctrans = nullptr;
		delete ft;
		ft = nullptr;
		delete fil;
		fil = nullptr;
		delete imsize;
		imsize = nullptr;
		// 释放GDI+资源
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// “二值化框”框的消息处理程序。
INT_PTR CALLBACK Threshold(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND editWnd;
	wchar_t buff[100];
	int threshold = -1;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			editWnd = GetDlgItem(hDlg, IDC_EDIT1);
			GetWindowText(editWnd, buff, 100);
			threshold = _wtof(buff);
			if (threshold < 0 || threshold>255)
			{
				MessageBox(hDlg, L"请输入正确的阈值！(0~255)", L"警告", MB_OK | MB_ICONWARNING);
				return (INT_PTR)FALSE;
			}
			else ctrans->Threshold(threshold);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)FALSE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//“缩放”框的消息处理程序
INT_PTR CALLBACK Zoom(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND editWnd;
	int width, height;
	wchar_t buff[100];
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			editWnd = GetDlgItem(hDlg, IDC_WIDTH);
			GetWindowText(editWnd, buff, 100);
			width = _wtof(buff);
			if (width<=0)
			{
				MessageBox(hDlg, L"请输入正确的宽度！(大于0)", L"警告", MB_OK | MB_ICONWARNING);
				return (INT_PTR)FALSE;
			}
			editWnd = GetDlgItem(hDlg, IDC_HEIGHT);
			GetWindowText(editWnd, buff, 100);
			height = _wtof(buff);
			if (height <= 0)
			{
				MessageBox(hDlg, L"请输入正确的高度！(大于0)", L"警告", MB_OK | MB_ICONWARNING);
				return (INT_PTR)FALSE;
			}
			EndDialog(hDlg, LOWORD(wParam));
			imsize->Resize(width, height);
			return (INT_PTR)TRUE;
		}

		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)FALSE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
