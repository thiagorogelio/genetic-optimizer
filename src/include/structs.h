#define INITIAL_POP 20

typedef struct elem {
    int indice;
    struct elem *next;
} elem;

typedef struct list {
    float peso;
    struct elem *next;
    struct elem *last;
} list;

typedef struct cromossomo {
    float peso;
    elem *cols;
    int *lins;
} cromossomo;
