#include <windows.h>
#undef max
#undef min
#include "../../lib/muil_display.hpp"
#include "../../lib/muil_touchscreen.hpp"
#include "../../lib/muil_utils.hpp"

HWND hWnd = 0;
const wchar_t *szTitle = L"title";
const wchar_t *szWindowClass = L"muil_class";
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	// Initialize global strings

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	RECT desired_rect = { 0, 0, 240, 320 };

	AdjustWindowRect(&desired_rect, WS_BORDER | WS_CAPTION, false);

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, desired_rect.right - desired_rect.left, desired_rect.bottom - desired_rect.top, NULL, NULL, hInstance, NULL);

	if (!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int mouse_x = -1;
int mouse_y = -1;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		break;

	case WM_LBUTTONUP:
		mouse_x = -1;
		mouse_y = -1;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int offset_x = 0;
int offset_y = 0;

muil::Size muil::display_get_size()
{
	return muil::Size(240, 320);
}

uint16_t muil::display_get_dpi()
{
	return 96;
}

void muil::display_set_point(int x, int y, const muil::Color &color)
{
	HDC hdc = GetDC(hWnd);
	SetPixel(hdc, x + offset_x, y + offset_y, RGB(color.r, color.g, color.b));
	ReleaseDC(hWnd, hdc);
}

void muil::display_fill_rect(const muil::Rect &rect, const muil::Color &color)
{
	HDC hdc = GetDC(hWnd);
	HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	SelectObject(hdc, brush);
	
	RECT r;
	r.left = rect.x1 + offset_x;
	r.top = rect.y1 + offset_y;
	r.right = rect.x2 + 1 + offset_x;
	r.bottom = rect.y2 + 1 + offset_y;
	FillRect(hdc, &r, brush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);
}

void muil::display_paint_character(int x0, int y0, const uint8_t *data, uint8_t width, uint8_t height, const muil::Color &color)
{
	default_display_paint_character(x0, y0, data, width, height, color);
}

void muil::display_set_offset(int x, int y)
{
	offset_x = x;
	offset_y = y;
}

bool muil::touchscreen_is_pressed()
{
	return true;
}

muil::Point muil::touchscreen_get_pos()
{
	return muil::Point(mouse_x, mouse_y);
}

void muil::delay_ms(uint16_t milliseconds)
{
	MSG msg;

	int count = milliseconds / 10;
	for (int i = 0; i < count; i++)
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(10);
	}
}

void init_for_win32()
{
	MyRegisterClass(GetModuleHandle(NULL));
	InitInstance(GetModuleHandle(NULL), SW_SHOW);
}