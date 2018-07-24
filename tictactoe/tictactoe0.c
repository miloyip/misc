#include <stdio.h>

typedef struct {
    int board[3][3];    // -1 = empty, 0 = O, 1 = X
    int turn;           // O first
} state;

void init(state* s) {
    int i, j;
    for (j = 0; j < 3; j++)
        for (i = 0; i < 3; i++)
            s->board[j][i] = -1;
    s->turn = 0;
}

void display(const state* s) {
    int i, j;
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            switch (s->board[j][i]) {
                case -1: printf(" %d ", j * 3 + i + 1); break;
                case  0: printf(" O "); break;
                case  1: printf(" X "); break;
            }
            if (i < 2)
                printf("|");
            else
                printf("\n");
        }
        if (j < 2)
            printf("---+---+---\n");
        else
            printf("\n");
    }
}

int main() {
    state s;
    init(&s);
    display(&s);
}
