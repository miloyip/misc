#include "svpng.inc"
#include <stdlib.h> // rand(), RAND_MAX
#include <stdio.h> // fopen()
#include <math.h>  // powf()
#include <limits.h> // INT_MAX

#define W 1024
#define H 1024

int p[H][W / 2];
unsigned char img[W * H * 3], *q = img, c;

int f(float x, float y, float cx, float cy, int n) {
    if (!n)
        return 0;
    if (x * x + y * y > 10 || f(x * x - y * y + cx, 2 * x * y + cy, cx, cy, n - 1)) {
        int ix = (int)((0.3f * y) * W), iy = (int)((0.3f * x + 0.65f) * H);
        if (x != 0 && y != 0 && ix >= 0 && iy >= 0 && ix < W / 2 && iy < H)
            p[iy][ix]++;
        return 1;
    }
    return 0;
}

int main() {
    int i, x, y, minp = INT_MAX, maxp = 0;
    for (i = 0; i < 10000000; i++)
        f(0, 0, rand() * (6.0f / RAND_MAX) - 3, rand() * (3.0f / RAND_MAX), 100000);
    for (y = 0; y < H; y++)
        for (x = 0; x < W / 2; x++) {
            minp = p[y][x] < minp ? p[y][x] : minp;
            maxp = p[y][x] > maxp ? p[y][x] : maxp;
        }
    for (y = 0; y < H; y++, q += W * 3)
        for (x = 0; x < W / 2; x++) {
            c = (unsigned char)(255 * powf((p[y][x] - minp) / (float)(maxp - minp), 0.5f));
            q[(W / 2 - x) * 3] = q[(W / 2 - x) * 3 + 1] = 
            q[(W / 2 + x - 1) * 3] = q[(W / 2 + x - 1) * 3 + 1] = c;
            q[(W / 2 - x) * 3 + 2] = q[(W / 2 + x - 1) * 3 + 2] = c * 2 < 255 ? c * 2 : 255;
        }
    svpng(fopen("buddhabrot.png", "wb"), W, H, img, 0);
}
