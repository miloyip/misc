#include <stdio.h>

const int n = 3, s = 1 << n, a[] = { 1, s, -1, -s };
int l = 3, p, q;

void step(int d) {
    d &= 3;
    if (p == q)
        printf("%.2s", &"__    | |_| |      ___|_ _    | "[l * 8 + d * 2]);
    p += a[l = d];
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
    for (; q < s * s; q++, p = 0) {
        hilbert(0, 1, n);
        if (q % s == s - 1)
            putchar('\n');
    }
}
