//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************

using namespace std;
// Include class files
#include "Image.h"
#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>

#define PI 3.14159265

#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage, outImage;				// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text
int lineNum;
double anglePerSec;
int pin1;
vector<pair<int, int>> trackFPS60;
vector<pair<int, int>> trackFPS;
HWND hWnd1;
HWND hWnd2;
double angleOne=0;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void 				ColorPixel(char* imgBuf, int w, int h, int x, int y);
void				DrawLine(char* imgBuf, int w, int h, int x1, int y1, int x2, int y2, vector<pair<int, int>>& track);

struct line {
	pair<int, int> point1;
	pair<int, int> point2;
};

struct rgb {
	char b;
	char g;
	char r;
};


line lineCoordinates(double angle) {
	double size = 511 / 2.0;
	pair<double, double> p1;
	pair<double, double> p2;

	if (angle < 0 || angle >= 360) {
		angle = angle - trunc(angle / 360.0)*360;
		if (angle < 0) {
			angle+= 360;
		}
	}

	if (angle == 0) {
		p1.first = 0.5;
		p1.second = 0.5;
		p2.first = size;
		p2.second = 0.5;
	} 
	else if (0 < angle && angle < 45) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = 0.5;
		p1.second = 0.5;
		p2.first = size;
		p2.second = floor(size* tanV)+0.5;
	} 
	else if (angle == 45) {
		p1.first = 0.5;
		p1.second = 0.5;
		p2.first = size;
		p2.second = size;
	}
	else if (45 < angle && angle < 90) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = 0.5;
		p1.second = 0.5;
		p2.first = floor(size/ tanV)+0.5;
		p2.second = size;
	}
	else if (angle == 90) {
		p1.first = -0.5;
		p1.second = 0.5;
		p2.first = -0.5;
		p2.second = size;
	}
	else if (90 < angle && angle < 135) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = -0.5;
		p1.second = 0.5;
		p2.first = floor(size / tanV) + 0.5;
		p2.second = size;
	}
	else if (angle == 135) {
		p1.first = -0.5;
		p1.second = 0.5;
		p2.first = -size;
		p2.second = size;
	}
	else if (135 < angle && angle < 180) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = -0.5;
		p1.second = 0.5;
		p2.first = -size;
		p2.second = floor(-size * tanV) + 0.5;
	}
	else if (angle == 180) {
		p1.first = -0.5;
		p1.second = -0.5;
		p2.first = -size;
		p2.second = -0.5;
	}
	else if (180 < angle && angle < 225) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = -0.5;
		p1.second = -0.5;
		p2.first = -size;
		p2.second= floor(-size * tanV) + 0.5;
	}
	else if (angle == 225) {
		p1.first = -0.5;
		p1.second = -0.5;
		p2.first = -size;
		p2.second = -size;
	}
	else if (225 < angle && angle < 270) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = -0.5;
		p1.second = -0.5;
		p2.first = floor(-size/tanV)+0.5;
		p2.second = -size;
	}
	else if (angle == 270) {
		p1.first = 0.5;
		p1.second = -0.5;
		p2.first = 0.5;
		p2.second = -size;
	}
	else if (270 < angle && angle < 315) {
		double tanV = tan(angle * PI / 180.0);
		p1.first = 0.5;
		p1.second = -0.5;
		p2.first = floor(-size/tanV)+0.5;
		p2.second = -size;
	}
	else if (angle == 315) {
		p1.first = 0.5;
		p1.second = -0.5;
		p2.first = size;
		p2.second = -size;
	}
	else {
		double tanV = tan(angle * PI / 180.0);
		p1.first = 0.5;
		p1.second = -0.5;
		p2.first = size;
		p2.second = floor(size*tanV)+0.5;
	}
	return {make_pair(size+p1.first, size-p1.second), make_pair(size+p2.first, size-p2.second)};
}


rgb findRGB(int x, int y, int size, const char* buf) {
	return {
		buf[(3 * y * size) + 3 * x],
		buf[(3 * y * size) + 3 * x + 1],
		buf[(3 * y * size) + 3 * x + 2]
	};
}

void setRGB(rgb color, int x, int y, int size, char* buf) {
	buf[(3 * y * size) + 3 * x] = color.b;
	buf[(3 * y * size) + 3 * x + 1] = color.g;
	buf[(3 * y * size) + 3 * x + 2] = color.r;
}


void timer_start(function<void(void)> func, unsigned int interval)
{
	thread([func, interval]() {
		while (true)
		{
			func();
			this_thread::sleep_for(chrono::milliseconds(interval));
		}
		}).detach();
}


void drawGraph(double angle, int lineNum, char* buf, vector<pair<int,int>>& track) {
	for (int lineIdx = 0; lineIdx < lineNum; ++lineIdx) {
		line coor = lineCoordinates(angle + (360.0 / lineNum) * lineIdx);
		DrawLine(buf, 512, 512, coor.point1.first, coor.point1.second, coor.point2.first, coor.point2.second, track);
	}
}

void clear(char* target, vector<pair<int, int>>& track) {
	rgb col = { 255,255,255 };
	for (pair<int, int> each : track) {
		setRGB(col, each.first, each.second, 512, target);
	}
	track.clear();
}


void FPS60()
{
	clear(inImage.getImageData(), trackFPS60);

	angleOne = pin1 * 0.001* anglePerSec;
	drawGraph(angleOne, lineNum, inImage.getImageData(), trackFPS60);
	RECT rt;
	GetClientRect(hWnd1, &rt);
	InvalidateRect(hWnd1, &rt, false);
	pin1++;
}


void FPS()
{
	clear(outImage.getImageData(), trackFPS);
	drawGraph(angleOne, lineNum, outImage.getImageData(), trackFPS);
	RECT rt;
	GetClientRect(hWnd2, &rt);
	InvalidateRect(hWnd2, &rt, false);
}

// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Read in a parameter from the command line
	stringstream ss(lpCmdLine);
	int option;
	int firstParam;
	double secondParam;
	double thirdParam;
	ss >> option >> firstParam >> secondParam >> thirdParam;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Set up the images
	int w = 512;
	int h = w;
	inImage.setWidth(w);
	inImage.setHeight(h);

	char* tempImagePath = new char[1];
	tempImagePath[0] = 'a';
	inImage.setImagePath(tempImagePath);

	char* imgData = new char[w * h * 3];

	for (int i = 0; i < w * h; ++i)
	{
		imgData[3*i] = 255;
		imgData[3*i+1] = 255;
		imgData[3*i+2] = 255;
	}


	if (option == 1) {
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				if (j == 0 || j == h - 1 || i == 0 || i == w - 1) ColorPixel(imgData, w, h, i, j);
			}
		}
		cout << "Part 1" << endl << "line#: " << firstParam << endl << "scale: " << secondParam << endl << "anti aliasing: " << thirdParam;
		drawGraph(0, firstParam, imgData, trackFPS60);
		inImage.setImageData(imgData);

		int ow = round(secondParam*512);
		int oh =ow;
		outImage.setWidth(ow);
		outImage.setHeight(oh);
		char* outimgData = new char[ow * oh * 3];

		if (thirdParam == 0) {
			for (int i = 0; i < oh; ++i) {
				for (int j = 0; j < ow; ++j) {
					int newI = round((i / ((double)(oh - 1))) * 511);
					int newJ = round((j / ((double)(ow - 1))) * 511);
					rgb cor = findRGB(newJ, newI, 512, imgData);
					setRGB(cor, j, i, ow, outimgData);
				}
			}
		}
		else if (thirdParam == 1) {
			for (int i = 0; i < oh; ++i) {
				for (int j = 0; j < ow; ++j) {
					int newI = round((i / ((double)(oh - 1))) * 511);
					int newJ = round((j / ((double)(ow - 1))) * 511);

					int totalR = 0;
					int totalG = 0;
					int totalB = 0;
					int cnt = 0;

					for (int p = -1; p < 2; ++p) {
						for (int q = -1; q < 2; ++q) {
							int exploreI = newI + p;
							int exploreJ = newJ + q;

							if (0 <= exploreI && exploreI < 512 && 0 <= exploreJ && exploreJ < 512) {
								rgb cor = findRGB(exploreJ, exploreI, 512, imgData);
								totalR += (int)(unsigned char)cor.r;
								totalG += (int)(unsigned char)cor.g;
								totalB += (int)(unsigned char)cor.b;
								cnt++;
							}
						}
					}
					rgb cor = {
						(char)round(totalB / (double)cnt),
						(char)round(totalG / (double)cnt),
						(char)round(totalR / (double)cnt)
					};
					setRGB(cor, j, i, ow, outimgData);
				}
			}
		}
		outImage.setImageData(outimgData);
	
		// Initialize global strings
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);
	}

	if (option == 2) {
		inImage.setImageData(imgData);
		cout << "Part 2" << endl << "line#: " << firstParam << endl << "rev: " << secondParam << endl << "fps: " << thirdParam;
		lineNum = firstParam;
		anglePerSec = secondParam * 360;

		outImage.setHeight(512);
		outImage.setWidth(512);

		char* outData = new char[512*512*3];

		for (int i = 0; i < 512 * 512; ++i)
		{
			outData[3 * i] = 255;
			outData[3 * i + 1] = 255;
			outData[3 * i + 2] = 255;
		}
		outImage.setImageData(outData);


		// Initialize global strings
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);


		pin1 = 0;
		timer_start(FPS60, 1);
		timer_start(FPS, round(1.0 / thirdParam * 1000));
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

// Colors a pixel at the given (x, y) position black.
void ColorPixel(char* imgBuf, int w, int h, int x, int y)
{
	imgBuf[(3 * y * w) +  3 * x] = 0;
	imgBuf[(3 * y * w) +  3 * x + 1] = 0;
	imgBuf[(3 * y * w) +  3 * x + 2] = 0;
}

// Draws a line using Bresenham's algorithm.
void DrawLine(char* imgBuf, int w, int h, int x1, int y1, int x2, int y2, vector<pair<int,int>> & track)
{
	const bool steep = (abs(y2 - y1) > abs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			ColorPixel(imgBuf, w, h, y, x);
			track.push_back({y,x});
		}
		else
		{
			ColorPixel(imgBuf, w, h, x, y);
			track.push_back({ x,y });
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd1 = CreateWindow(szWindowClass, "Input", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);


   hWnd2 = CreateWindow(szWindowClass, "Output", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd1)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd1, nCmdShow);
   UpdateWindow(hWnd1);

   ShowWindow(hWnd2, nCmdShow);
   UpdateWindow(hWnd2);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   outImage.Modify();
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
			if (hWnd == hWnd1) {
				hdc = BeginPaint(hWnd, &ps);
				char text[1000];
				strcpy(text, "Original image\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi, 0, sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth() * inImage.getHeight();

				SetDIBitsToDevice(hdc,
					20, 100, inImage.getWidth(), inImage.getHeight(),
					0, 0, 0, inImage.getHeight(),
					inImage.getImageData(), &bmi, DIB_RGB_COLORS);
				EndPaint(hWnd, &ps);
			} 

			if (hWnd == hWnd2) {
				hdc = BeginPaint(hWnd, &ps);
				char text[1000];
				strcpy(text, "Image after modification\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nFor Part One, some scale factor works, some are not. Not sure if it is win32 rendering bugs (e.g. ok scale factor: 0.5, 0.86, 0.64, 0.21 and etc)");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);


				BITMAPINFO bmi2;
				CBitmap bitmap2;
				memset(&bmi2, 0, sizeof(bmi2));
				bmi2.bmiHeader.biSize = sizeof(bmi2.bmiHeader);
				bmi2.bmiHeader.biWidth = outImage.getWidth();
				bmi2.bmiHeader.biHeight = -outImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi2.bmiHeader.biPlanes = 1;
				bmi2.bmiHeader.biBitCount = 24;
				bmi2.bmiHeader.biCompression = BI_RGB;
				bmi2.bmiHeader.biSizeImage = outImage.getWidth() * outImage.getHeight();


				SetDIBitsToDevice(hdc,
					20, 100, outImage.getWidth(), outImage.getHeight(),
					0, 0, 0, outImage.getHeight(),
					outImage.getImageData(), &bmi2, DIB_RGB_COLORS);
				EndPaint(hWnd, &ps);
			}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}