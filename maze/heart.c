#include <stdlib.h>
#include "svpng.inc"

#define C(x, y) (img + ((y) * 6 + 3) * a + (x) * 6 + 3)
#define W(o) c = C(x, y) + o; c[0] = 255; c[1] = c[2] = d % 512 < 256 ? d % 512 : 511 - d % 512
#define V(x, y) (x) >= 0 && (x) < w && (y) >= 0 && (y) < h && !*C(x, y) && m(x, y)

static const int w = 256, h = 256, a = w * 2 + 1, b = h * 2 + 1;
static unsigned char img[a * b * 3], *c;

int m(int x, int y) {
    float u = 2.4f * x / w - 1.2f, v = -2.4f * y / h + 1.3f, w = u * u + v * v - 1;
    return w * w * w - u * u * v * v * v < 0;
}

void f(int x, int y, int d) {
    W(0);
    for (int j = rand() % 24 * 4, i = 0; i < 4; i++)
        switch ("ABCDABDCACBDACDBADBCADCBBACDBADCBCADBCDABDACBDCACABDCADBCBADCBDACDABCDBADABCDACBDBACDBCADCABDCBA"[i + j]) {
            case 'A': if (V(x - 1, y)) { W(    -3); f(x - 1, y, d + 1); } break;
            case 'B': if (V(x + 1, y)) { W(     3); f(x + 1, y, d + 1); } break;
            case 'C': if (V(x, y - 1)) { W(a * -3); f(x, y - 1, d + 1); } break;
            case 'D': if (V(x, y + 1)) { W(a *  3); f(x, y + 1, d + 1); } break;
        }
}

int main() {
    f(w / 2, h / 2, 0);
    svpng(fopen("heart.png", "wb"), a, b, img, 0);
}
