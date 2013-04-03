/*
Sample usage of GD on windows. This little program opens a window, fetch its DIB
and assigns to a GD truecolor image.

Thanks to Mateusz Loskot (http://mateusz.loskot.net) for the AttachBuffer function!
$Id$
*/
#include <windows.h>
#include <gd.h>
#include <gdfontg.h>
#include <gdfontl.h>


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

void gdImageDetachBuffer(gdImagePtr im)
{
	free(im->tpixels);
	free(im);
}


BITMAPINFO gdCreateBmp(int width, int height)
{
	BITMAPINFO bmp_info;

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
	return bmp_info;
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Bezier") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass)) {
		// UNICODE-Compilierung ist die einzige realistische Fehlermöglichkeit
		MessageBox (NULL, TEXT ("Programm arbeitet mit Unicode und setzt Windows NT voraus!"),
		            szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Bezierkurven"),
	                     WS_OVERLAPPEDWINDOW,
	                     CW_USEDEFAULT, CW_USEDEFAULT,
	                     CW_USEDEFAULT, CW_USEDEFAULT,
	                     NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0)) {
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

void DrawBezier (HDC hdc, POINT apt[])
{
	PolyBezier (hdc, apt, 4) ;

	MoveToEx (hdc, apt[0].x, apt[0].y, NULL) ;
	LineTo   (hdc, apt[1].x, apt[1].y) ;

	MoveToEx (hdc, apt[2].x, apt[2].y, NULL) ;
	LineTo   (hdc, apt[3].x, apt[3].y) ;
}


void gdDrawImage(HDC hdc, RECT *rc)
{
	HDC  mem_dc;
	BITMAPINFO bmp_info;
	void* bits;
	HBITMAP bmp, temp;
	gdImagePtr im;
	int width, height, stride;
	int white, black, blue, red;
	char *s = "Hello world!";
	gdFontPtr lfont, gfont;

	width = rc->right - rc->left;
	height = rc->bottom - rc->top;

	bmp_info = gdCreateBmp(width, height);

	// Create memory device context
	mem_dc = CreateCompatibleDC(hdc);
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

	/*stride = ((width * 1 + 3) >> 2) << 2;*/
	// always uses 32bit in BMPINFO
	stride = width;
	im = NULL;

	// Attach shared buffer of pixels to GD image
	// Negative stride places 0,0 in upper-left corner
	im = gdImageTrueColorAttachBuffer((int*)bits, width, height, -stride);
	if (!im) {
		MessageBox(NULL, "GD image creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	// Start of GD drawing
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
	BitBlt(hdc, rc->left, rc->top, width, height, mem_dc, 0, 0, SRCCOPY);

	// Free
	gdImageDetachBuffer(im);
	SelectObject(mem_dc, temp);
	DeleteObject(bmp);
	DeleteObject(mem_dc);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT apt[4] ;
	HDC          hdc ;
	int          cxClient, cyClient ;
	PAINTSTRUCT  ps ;
	RECT rc;

	GetClientRect(hwnd, &rc);

	switch (message) {
	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;

		apt[0].x = cxClient / 4 ;
		apt[0].y = cyClient / 2 ;

		apt[1].x = cxClient / 2 ;
		apt[1].y = cyClient / 4 ;

		apt[2].x =     cxClient / 2 ;
		apt[2].y = 3 * cyClient / 4 ;

		apt[3].x = 3 * cxClient / 4 ;
		apt[3].y =     cyClient / 2 ;
		return 0 ;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON || wParam & MK_RBUTTON) {
			hdc = GetDC (hwnd) ;

			// alte Kurve löschen (mit Weiß übermalen)
			SelectObject (hdc, GetStockObject (WHITE_PEN)) ;
			DrawBezier (hdc, apt) ;

			if (wParam & MK_LBUTTON) {
				apt[1].x = LOWORD (lParam) ;
				apt[1].y = HIWORD (lParam) ;
			}

			if (wParam & MK_RBUTTON) {
				apt[2].x = LOWORD (lParam) ;
				apt[2].y = HIWORD (lParam) ;
			}

			// neue Kurve (mit Schwarz) zeichnen
			SelectObject (hdc, GetStockObject (BLACK_PEN)) ;
			gdDrawImage(hdc, &rc);
			DrawBezier (hdc, apt) ;
			ReleaseDC (hwnd, hdc) ;
		}
		return 0 ;


	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		GetClientRect(hwnd, &rc);
		gdDrawImage(hdc, &rc);
		DrawBezier (hdc, apt) ;

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
