#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { NUM, ADD, SUB, MUL, DIV } Type;

#define OPERATOR_CHAR(n) ("+-*/"[n->type - ADD])

typedef struct NodeTag {
    union {
        double number;
        struct NodeTag *children[2];
    } u;
    Type type;
} Node;

void release(Node* n) {
    int i;
    if (n->type != NUM)
        for (i = 0; i < 2; i++)
            if (n->u.children[i])
                release(n->u.children[i]);
    free(n);
}

Node* parse(char** s) {
    while (isspace(**s))
        (*s)++;    
    if (**s == '\0')
        return NULL;
    else {
        Node* n = (Node*)calloc(1, sizeof(Node));
        if (isdigit(**s)) {
            n->type = NUM;
            n->u.number = strtod(*s, s);
        }
        else {
            int i;
            switch (**s) {
                case  '+': n->type = ADD; break;
                case  '-': n->type = SUB; break;
                case  '*': n->type = MUL; break;
                case  '/': n->type = DIV; break;
                default: release(n); return NULL;
            }
            (*s)++;
            for (i = 0; i < 2; i++)
                if ((n->u.children[i] = parse(s)) == NULL) {
                    release(n);
                    return NULL;
                }
        }
        return n;
    }
}

double eval(const Node* n) {
    switch (n->type) {
        case ADD: return eval(n->u.children[0]) + eval(n->u.children[1]);
        case SUB: return eval(n->u.children[0]) - eval(n->u.children[1]);
        case MUL: return eval(n->u.children[0]) * eval(n->u.children[1]);
        case DIV: return eval(n->u.children[0]) / eval(n->u.children[1]);
        case NUM: return n->u.number;
    }
}

void printInfix(const Node *n) {
    if (n->type == NUM)
        printf("%lg", n->u.number);
    else {
        putchar('(');
        printInfix(n->u.children[0]);
        printf(" %c ", OPERATOR_CHAR(n));
        printInfix(n->u.children[1]);
        putchar(')');
    }
}

int maxDepth(const Node* n) {
    if (n->type == NUM)
        return 1;
    else {
        int maximum = 0, i, d;
        for (i = 0; i < 2; i++)
            if (maximum < (d = maxDepth(n->u.children[i])))
                maximum = d;
        return maximum + 1;
    }
}

void fillMap(Node** map, Node* n, int index) {
    int i;
    map[index] = n;
    if (n->type != NUM)
        for (i = 0; i < 2; i++)
            fillMap(map, n->u.children[i], index * 2 + i + 1);
}

void putchars(char c, int n) {
    while (n--)
        putchar(c);
}

void printLeftToParentBranchTop(int w) {
    printf("%*c", w + 1, ' ');
    putchars('_', w - 3);
    printf("/ ");
}

void printRightToParentBranchTop(int w) {
    putchar('\\');
    putchars('_', w - 3);
    printf("%*c", w + 2, ' ');
}

void printLeftToParentBranchBottom(int w) {
    printf("%*c%*c", w + 1, '/', w - 1, ' ');
}

void printRightToParentBranchBottom(int w) {
    printf("%*c%*c", w - 1, '\\', w + 1, ' ');
}

int printNode(Node* n, int w) {
    if (n->type == NUM)
        return printf("%*lg", w, n->u.number);
    else
        return printf("%*c", w, "+-*/"[n->type - ADD]);
}

void printTree(Node* n) {
    int depth = maxDepth(n), i, j, index;
    Node** map = (Node**)calloc((1 << depth) - 1, sizeof(Node*));
    fillMap(map, n, 0);
    for (j = 0, index = 0; j < depth; j++) {
        int w = 1 << (depth - j + 1);
        if (j > 0) {
            // Top part of node to parent branch
            for (i = 0; i < 1 << j; i++)
                if (map[index + i])
                    if (i % 2 == 0) printLeftToParentBranchTop(w);
                    else            printRightToParentBranchTop(w);
                else
                    putchars(' ', w * 2);
            putchar('\n');
            // Bottom part of node to parent branch
            for (i = 0; i < 1 << j; i++)
                if (map[index + i])
                    if (i % 2 == 0) printLeftToParentBranchBottom(w);
                    else            printRightToParentBranchBottom(w);
                else
                    putchars(' ', w * 2);
            putchar('\n');
        }
        // Node content
        for (i = 0; i < 1 << j; i++, index++)
            if (map[index])
                putchars(' ', w * 2 - printNode(map[index], w));
            else
                putchars(' ', w * 2);
        putchar('\n');
    }
    free(map);
}

int main(int argc, char** argv) {
    if (argc != 2)
        return printf("Help: pntree \"+*2 3 1\"");
    else {
        char** p = &argv[1];
        Node* root = parse(p);
        if (root) {
            printTree(root);
            putchar('\n');
            printInfix(root);
            printf(" = %lg\n", eval(root));
            release(root);
        }
        else
            return printf("Invalid input\n");
    }
}
