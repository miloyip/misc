#include "svpng.inc"
#include <math.h>

#define S 512

unsigned char img[S * S * 3];
float px = 0.0f, py = 0.0f;

void setpixel(int x, int y) {
    unsigned char* p = img + (y * S + x) * 3;
    p[0] = p[1] = p[2] = 255;
}

void lineto(float tx, float ty) {
    if (px == tx) {
        int y0 = (int)fminf(py, ty), y1 = (int)fmaxf(py, ty), y;
        for (y = y0; y <= y1; y++)
            setpixel((int)(px), y);
    }
    else if (py == ty) {
        int x0 = (int)fminf(px, tx), x1 = (int)fmaxf(px, tx), x;
        for (x = x0; x <= x1; x++)
            setpixel(x, (int)(py));
    }
    px = tx;
    py = ty;
}

void hilbert(float x, float y, float xi, float xj, float yi, float yj, int n) {
    if (n) {
        hilbert(x,                   y,                    yi / 2,  yj / 2,  xi / 2,  xj / 2, n - 1);
        hilbert(x + xi / 2,          y + xj / 2,           xi / 2,  xj / 2,  yi / 2,  yj / 2, n - 1);
        hilbert(x + xi / 2 + yi / 2, y + xj / 2 + yj / 2,  xi / 2,  xj / 2,  yi / 2,  yj / 2, n - 1);
        hilbert(x + xi / 2 + yi,     y + xj / 2 + yj,     -yi / 2, -yj / 2, -xi / 2, -xj / 2, n - 1);
    }
    else
        lineto(x + (xi + yi) / 2, y + (xj + yj) / 2);
}

int main() {
    hilbert(0.0f, 0.0f, 0.0f, S, S, 0.0f, 4);
    svpng(fopen("hilbert.png", "wb"), S, S,img, 0);
}
