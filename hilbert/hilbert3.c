#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* img, *p;
const int n = 4, s = (2 << n) - 1, a[] = { 2, s * 2, -2, -s * 2 };

void step(int d) {
    int i;
    for (i = 0; i < 2; i++, p += a[d & 3])
        p[0] = p[1] = '*';
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
    memset(p = img = malloc(s * s * 2), ' ', s * s * 2);
    hilbert(0, 1, n);
    p[0] = p[1] = '*';
    for (p = img; p < img + s * s * 2; p += s * 2)
        printf("%.*s\n", s * 2, p);
    free(img);
}
