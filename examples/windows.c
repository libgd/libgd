/*
Sample usage of GD on windows. This little program opens a window, fetch its DIB 
and assigns to a GD truecolor image.

Thanks to Mateusz Loskot (http://mateusz.loskot.net) for the AttachBuffer function!
*/
#include <gd.h>
#include <windows.h>
#include <gdfontg.h>
#include <gdfontl.h>
#include <windef.h>

const char g_szClassName[] = "myWindowClass";

gdImagePtr gdImageTrueColorAttachBuffer(int* buffer, int sx, int sy, int stride)
{
	int i;
	int height;
	int* rowptr;
	gdImagePtr im;
	im = (gdImage *) malloc (sizeof (gdImage));
	if (!im) {
		return 0;
	}
	memset (im, 0, sizeof (gdImage));
#if 0
	if (overflow2(sizeof (int *), sy)) {
		return 0;
	}
#endif

	im->tpixels = (int **) malloc (sizeof (int *) * sy);
	if (!im->tpixels) {
		free(im);
		return 0;
	}

	im->polyInts = 0;
	im->polyAllocated = 0;
	im->brush = 0;
	im->tile = 0;
	im->style = 0;

	height = sy;
	rowptr = buffer;
	if (stride < 0) {
		 int startoff = (height - 1) * stride;
		 rowptr = buffer - startoff;
	}

	i = 0;
	while (height--) {
		 im->tpixels[i] = rowptr;
		 rowptr += stride;
		 i++;
	}

	im->sx = sx;
	im->sy = sy;
	im->transparent = (-1);
	im->interlace = 0;
	im->trueColor = 1;
	im->saveAlphaFlag = 0;
	im->alphaBlendingFlag = 1;
	im->thick = 1;
	im->AA = 0;
	im->cx1 = 0;
	im->cy1 = 0;
	im->cx2 = im->sx - 1;
	im->cy2 = im->sy - 1;
	return im;
}


void gdWin32Paint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC dc;
	RECT rc;
	int width, height;
	BITMAPINFO bmp_info;
	HDC mem_dc;
	void* bits;
	HBITMAP bmp, temp;
	int white, black, blue, red;
	int stride;
	char *s = "Hello world!";
	gdImagePtr im;
	gdFontPtr lfont, gfont;

	memset(&ps, 0, sizeof(ps));
	dc = BeginPaint(hwnd, &ps);

	GetClientRect(hwnd, &rc);

	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	// Configure bitmap properties

	ZeroMemory(&bmp_info, sizeof(BITMAPINFO));
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = width;
	bmp_info.bmiHeader.biHeight = height;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = 0;
	bmp_info.bmiHeader.biXPelsPerMeter = 0;
	bmp_info.bmiHeader.biYPelsPerMeter = 0;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;

	// Create memory device context
	mem_dc = CreateCompatibleDC(dc);
	if (!mem_dc) {
		MessageBox(NULL, "Can't create a compatible DC!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	// bits points to a shared buffer of pixels
	bits = NULL;
	bmp = CreateDIBSection(mem_dc, &bmp_info, DIB_RGB_COLORS, (void**)&bits, 0, 0);

	// Selecting the object before doing anything allows you to use libgd
	// together with native Windows GDI.
	temp = (HBITMAP)SelectObject(mem_dc, bmp);


	// Start of GD drawing
	stride = ((width * 1 + 3) >> 2) << 2;

	im = NULL;

	// Attach shared buffer of pixels to GD image
	// Negative stride places 0,0 in upper-left corner
	im = gdImageTrueColorAttachBuffer((int*)bits, width, height, -stride);
	if (!im) {
		MessageBox(NULL, "GD image creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);
	blue = gdImageColorAllocate(im, 0, 0, 255);
	// Allocate the color red, 50% transparent.
	red = gdImageColorAllocateAlpha(im, 255, 0, 0, 64);

	// Erase background with white color
	gdImageFilledRectangle(im, 0, 0, width, height, 0xFF0000);

	lfont = gdFontGetLarge();
	gfont = gdFontGetGiant();

	// Draw a dashed line from the upper left corner to the lower right corner.
	gdImageFilledRectangle(im, 25, 25, 100, 100, blue);
	gdImageChar(im, gfont, 35, 35, 'Q', white);
	gdImageFilledRectangle(im, 50, 50, 75, 175, red);
	gdImageLine(im, 0, 0, 150, 150, black);

	gdImageString(im, gdFontGetLarge(),
	im->sx / 2 - (strlen(s) * lfont->w / 2),
	im->sy / 2 - lfont->h / 2,
	(unsigned char*)s, black);


	// Copy drawing from memory context (shared bitmap buffer) to screen DC.
	BitBlt(dc, rc.left, rc.top, width, height, mem_dc, 0, 0, SRCCOPY);

	// Free
	//gdImageDetachBuffer(im);
	SelectObject(mem_dc, temp);
	DeleteObject(bmp);
	DeleteObject(mem_dc);
	EndPaint(hwnd, &ps);
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_PAINT:
			gdWin32Paint(hwnd);
				break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registering the Window Class
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Step 3: The Message Loop
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}