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
    for (j = 0; j < 3; printf(++j < 3 ? "---+---+---\n" : "\n"))
        for (i = 0; i < 3; putchar("||\n"[i++]))
            printf(" %c ", s->board[j][i] == -1 ? '1' + j * 3 + i : "OX"[s->board[j][i]]);
}

int move(state* s, int i, int j) {
    if (s->board[j][i] != -1)
        return 0;
    s->board[j][i] = s->turn++ % 2;
    return 1;
}

int main() {
    state s;
    init(&s);
    display(&s);
    move(&s, 1, 1);
    display(&s);
    move(&s, 0, 1);
    display(&s);
}
