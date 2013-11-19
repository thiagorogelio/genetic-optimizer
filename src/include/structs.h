// ************ PARÂMETROS QUE PODEM SER MODIFICADOS PARA MELHOR DESEMPENHO ***********

#define POP_SIZE 40 // tamanho da população
#define SELECT_FROM_BESTS 40 // melhores cromossomos selecionados para cruzamento
#define STOP_CRIT_IT 500 // critério de parada por iterações sem melhoria.
#define MUTATION_PERCENT 10 // porcentagem de mutação por iteração da pop
#define NEIGHBORHOOD_TYPES 5 // tipos de vizinhança

//*************************************************************************************

typedef struct elem {
    int indice;
    struct elem *next;
} elem;

typedef struct list {
    double peso;
    struct elem *next;
} list;

typedef struct cromossomo {
    double peso;
    elem *cols;
    int *lins;
} cromossomo;

#define MUTATION_PARAM POP_SIZE*MUTATION_PERCENT/100
