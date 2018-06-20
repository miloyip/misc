#include "svpng.inc"
#include <stdlib.h>

unsigned char* img, *p;
const int n = 4, s = (2 << n) - 1;

void step(int d) {
    int a[] = { 3, s * 3, -3, s * -3 }, i;
    for (i = 0; i < 2; i++, p += a[d & 3])
        p[0] = p[1] = p[2] = 255;
}

void hilbert(int d, int r, int n) {
    if (n--) {
        hilbert(d + r, -r, n); step(d + r);
        hilbert(d,      r, n); step(d    );
        hilbert(d,      r, n); step(d - r);
        hilbert(d - r, -r, n);
    }
}

int main() {
    p = img = calloc(s * s, 3);
    hilbert(0, 1, n);
    p[0] = p[1] = p[2] = 255;
    svpng(fopen("hilbert2.png", "wb"), s, s, img, 0);
    free(img);
}
