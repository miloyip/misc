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

void human(state* s) {
    char c;
    do {
        printf("%c: ", "OX"[s->turn % 2]);
        c = getchar();
        while (getchar() != '\n');
        printf("\n");
    } while (c < '1' || c > '9' || !move(s, (c - '1') % 3, (c - '1') / 3));
}

int main() {
    state s;
    init(&s);
    display(&s);
    while (s.turn < 9) {
        human(&s);
        display(&s);
    } 
}
