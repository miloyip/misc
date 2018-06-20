#include <stdlib.h>
#include <stdio.h>

char* img, *p;
const int n = 4 , s = 1 << n, a[] = { 2, s * 2, -2, -s * 2 };
int l = 3;

void step(int d) {
    d &= 3;
    p[0] = "_  ||||   _|   |"[l * 4 + d];
    p[1] = "_   _    ____   "[l * 4 + d];
    p += a[d];
    l = d;
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
    p = img = malloc(s * s * 2);
    hilbert(0, 1, n);
    p[0] = p[1] = ' ';
    for (p = img; p < img + s * s * 2; p += s * 2)
        printf("%.*s\n", s * 2, p);
    free(img);
}
