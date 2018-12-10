#include "Bresenham.h"
#include <math.h>

void makePixel(int x, int y, float* PixelBuffer, int mode);
inline void max(int& a, int& b) {
	if (a > b) {
		int c = a;
		a = b;
		b = c;
	}
}


void Pixel(int x, int y, float* pixelBuffer, int mode) {
	if (mode == 0) {
		makePixel(x, y, pixelBuffer, 0);
	}
	else if (mode == 1) {
		makePixel(x, y, pixelBuffer, 1);
	}
}

void Bresenham(int x1, int x2, int y1, int y2, float* PixelBuffer, int windowSizeX, int mode) {
	if (x1 == x2) {
		max(y1, y2);
		for (int i = y1; i < y2; i++) {
			Pixel(x1, i, PixelBuffer, mode);
		}
	}
	else if (y1 == y2) {
		max(x1, x2);
		for (int i = x1; i < x2; i++) {
			Pixel(i, y1,  PixelBuffer, mode);
		}
	}
	else {
		int dx = fabs(x2 - x1), dy = fabs(y2 - y1);
		int x, y;

		float m = (float(y2 - y1) / float(x2 - x1));
		if (m >= 1) {
			if (y1 > y2) {
				x = x2;
				y = y2;
				y2 = y1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dx2 = 2 * dx, Dx2y = 2 * (dx - dy);
			int p = 2 * dx - dy;
			Pixel(x, y, PixelBuffer, mode);
			while (y < y2) {
				y++;
				if (p < 0)
					p += Dx2;
				else {
					x++;
					p += Dx2y;
				}
				Pixel(x, y, PixelBuffer, mode);
			}
		}
		else if (m > 0 && m < 1) {
			if (x1 > x2) {
				x = x2;
				y = y2;
				x2 = x1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dy2 = 2 * dy, Dy2x = 2 * (dy - dx);
			int p = 2 * dy - dx;
			Pixel(x, y, PixelBuffer, mode);
			while (x < x2) {
				x++;
				if (p < 0)
					p += Dy2;
				else {
					y++;
					p += Dy2x;
				}
				Pixel(x, y, PixelBuffer, mode);
			}
		}
		else if (m <= -1) {
			if (y1 > y2) {
				x = x2;
				y = y2;
				y2 = y1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dx2 = 2 * dx, Dx2y = 2 * (dx - dy);
			int p = 2 * dx - dy;
			Pixel(x, y, PixelBuffer, mode);
			while (y < y2) {
				y++;
				if (p < 0)
					p += Dx2;
				else {
					x--;
					p += Dx2y;
				}
				Pixel(x, y, PixelBuffer, mode);
			}
		}
		else if (m < 0 && m > -1) {
			if (x1 > x2) {
				x = x2;
				y = y2;
				x2 = x1;
			}
			else {
				x = x1;
				y = y1;
			}
			int Dy2 = 2 * dy, Dy2x = 2 * (dy - dx);
			int p = 2 * dy - dx;
			Pixel(x, y, PixelBuffer, mode);
			while (x < x2) {
				x++;
				if (p < 0)
					p += Dy2;
				else {
					y--;
					p += Dy2x;
				}
				Pixel(x, y, PixelBuffer, mode);
			}
		}
	}
}