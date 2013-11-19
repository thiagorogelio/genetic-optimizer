#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/structs.h"

elem *linha;
list *coluna;
cromossomo *populacao;
int ncol;
int nlins;

double calcPeso(cromossomo *crom){
    double cont = 0;
    elem *cols = crom->cols;
    while(cols){
        cont = cont + coluna[cols->indice].peso;
        cols = cols->next;
    }
    return cont;
}

void insereElem(int c, int l){
    elem *el = malloc(sizeof(elem));
    el->indice = l;
    el->next = coluna[c].next;
    coluna[c].next = el;
    linha[l].indice++;
    elem *nova_col = malloc(sizeof(elem));
    nova_col->indice = c;
    nova_col->next = linha[l].next;
    linha[l].next = nova_col;
}

void splitLinha(char str[]){
    char *buff;
    buff = strtok(str," ");
    while(buff){
        if(strcmp(buff, "LINHAS") == 0 || strcmp(buff, "Linhas") == 0){
            buff = strtok(NULL," ");
            nlins = atoi(buff);
            linha = calloc(nlins+1, sizeof(elem));
        } else
        if(strcmp(buff, "COLUNAS") == 0 || strcmp(buff, "Colunas") == 0){
            buff = strtok(NULL," ");
            ncol = atoi(buff);
            coluna = calloc(ncol+1, sizeof(list));
        } else {
                int c = atoi(buff);
                if(c != 0){
                    buff = strtok(NULL," ");
                    coluna[c].peso = atof(buff);
                    buff = strtok(NULL," ");
                    while(buff){
                        int l = atoi(buff);
                        insereElem(c, l);
                        buff = strtok(NULL," ");
                    }
                }
        }
        buff = NULL;
    }
}

void importaArq(char nomearq[]){
    char c;
    FILE* fd;
    if ((fd=fopen(nomearq,"rb")) == NULL) {
       printf("\nArquivo não encontrado! :(");
    }else{
        c = getc(fd);
        char str[500];
        str[0] = '\0';
        int i = 0;
        while(c != EOF){
            if(c != '\n')
                str[i++] = c;
            else{
                str[i] = '\0';
                splitLinha(str);
                i = 0;
            }
            c = getc(fd);
        }
        fclose(fd);
    }
}

void printElems(){
    int i;
    for(i=1;i<ncol+1;i++){
        printf("\nColuna %d , peso %f elems:",i,coluna[i].peso);
        elem *el = coluna[i].next;
        while(el){
            printf("%d ",el->indice);
            el = el->next;
        }
    }
    for(i=1;i<nlins+1;i++){
        printf("\nLinha %d, Cols: %d elems:",i,linha[i].indice);
        elem *el = linha[i].next;
         while(el){
            printf("%d ",el->indice);
            el = el->next;
        }
    }
}

void insereColnoCrom(cromossomo *crom, int ncol){
    crom->peso = (coluna[ncol].peso + crom->peso);
    elem *col = malloc(sizeof(elem));
    col->indice = ncol;
    col->next = NULL;
    if(!crom->cols){
        crom->cols = col;
    } else {
        elem *c_cols = crom->cols;
        while(c_cols->next)
            c_cols = c_cols->next;
        c_cols->next = col;
    }
    elem *cobert_linhas = coluna[ncol].next;
    while(cobert_linhas){
        crom->lins[cobert_linhas->indice]++;
        cobert_linhas = cobert_linhas->next;
    }
}

void insereNapop(cromossomo *crom){
    int i = 0;
    if(populacao[i].peso == 0)
        populacao[i] = *crom;
    else {
        while((int)populacao[i].peso <= (int)crom->peso && populacao[i].peso && i < POP_SIZE-1){
            if((int)populacao[i].peso == (int)crom->peso && populacao[i].cols->indice == crom->cols->indice)
                break;
            else
                i++;
        }
        if((int)populacao[i].peso != (int)crom->peso){
        if(!populacao[POP_SIZE-1].peso || populacao[POP_SIZE-1].peso > crom->peso){
          while(populacao[i].peso && i < POP_SIZE-1){i++;}
            while(populacao[i-1].peso > crom->peso){
                    populacao[i] = populacao[i-1];
                    i--;
                    if(!i)
                        break;
            }
            populacao[i] = *crom;
        }}
    }
}

void geraPopInicial(){
    populacao = calloc(POP_SIZE+1, sizeof(cromossomo));
    srand (time(NULL));
    int i;
    for(i=0;i<POP_SIZE;i++){
        populacao[i].peso = 0;
    }
    while(!populacao[POP_SIZE-1].peso){
        cromossomo *aux = malloc(sizeof(cromossomo));
        aux->peso = 0;
        aux->lins = calloc(nlins+1,sizeof(int));
        aux->cols = NULL;
        int j;
        for(j = 1; j < nlins+1; j++){
            if(aux->lins[j] == 0){
                int el = rand() % linha[j].indice + 1;
                elem *aux2 = linha[j].next;
                while(el > 1){
                    aux2 = aux2->next;
                    el--;
                }
                insereColnoCrom(aux, aux2->indice);
            }
        }
        insereNapop(aux);
    }
}

void optimizeCrom(cromossomo *crom){
    elem *cols = crom->cols;
    elem *ant = crom->cols;
    while(cols){
        elem *lins = coluna[cols->indice].next;
        while(lins){
            if(crom->lins[lins->indice] > 1){
                lins = lins->next;
            }
            else
                break;
        }
        if(!lins){
            elem *lins = coluna[cols->indice].next;
            while(lins){
                crom->lins[lins->indice] = crom->lins[lins->indice]-1;
                lins = lins->next;
            }
            crom->peso = (crom->peso - coluna[cols->indice].peso);
            if(cols->next)
                    *cols = *cols->next;
            else{
                 ant->next = NULL;
                 cols = NULL;
            }
        } else {
            ant = cols;
            cols = cols->next;
        }
    }
}

void cruzaCromossomos(){
    cromossomo *pai1, *pai2, *novo;
    novo = malloc(sizeof(cromossomo));
    novo->lins = calloc(nlins,sizeof(int));
    int rand1, rand2;
    while(rand1 == rand2){
        rand1 = rand() %  SELECT_FROM_BESTS;
        rand2 = rand() %  SELECT_FROM_BESTS;
    }
    pai1 = &populacao[rand1];
    pai2 = &populacao[rand2];
    elem *cols = pai1->cols;
    int tano2 = 0;
    novo->cols = NULL;
    novo->peso = 0;
    while(cols){
        insereColnoCrom(novo, cols->indice);
        cols = cols->next;
        if(!cols && tano2 == 0){
            cols = pai2->cols;
            tano2++;
        }
    }
    int i;
    for(i = 0; i < nlins+1; i++)
        novo->lins[i] = pai1->lins[i] + pai2->lins[i];
    optimizeCrom(novo);
    insereNapop(novo);
}

cromossomo* newCrom(cromossomo *base){
    cromossomo *aux = malloc(sizeof(cromossomo));
    aux->cols = NULL;
    aux->lins = calloc(nlins+1,sizeof(int));
    elem *elm = base->cols;
    while(elm){
        insereColnoCrom(aux,elm->indice);
        elm = elm->next;
    }
    return aux;
}

cromossomo* melhorVizinho(cromossomo *slin){
    cromossomo *best = slin;
    int j;
    for(j=1;j<5;j++){
        cromossomo *ncrom = newCrom(slin);
        int i;
        for(i=1;i<NEIGHBORHOOD_COUNT; i++){
            int rcol = rand() % ncol + 1;
            insereColnoCrom(ncrom, rcol);
        }
        optimizeCrom(ncrom);
        if(ncrom->peso < best->peso)
            best = ncrom;
    }
    return best;
}

void VND(cromossomo *crom){
    cromossomo *slin = crom;
    int k = 1;
    while(k <= NEIGHBORHOOD_COUNT){
        cromossomo *s2lin = melhorVizinho(slin);
        if(s2lin->peso < slin->peso){
            k = 1;
            slin = s2lin;
        } else
            k++;
    }
    if(slin->peso < crom->peso)
        insereNapop(slin);
}

void mutation(){
    int i;
    for(i=POP_SIZE-MUTATION_PARAM;i<POP_SIZE;i++){
        populacao[i].peso = 999999;
    }
    for(i=0;i<MUTATION_PARAM;i++){
        cromossomo *aux = malloc(sizeof(cromossomo));
        aux->peso = 0;
        aux->lins = calloc(nlins+1,sizeof(int));
        aux->cols = NULL;
        int j;
        for(j = 1; j < nlins+1; j++){
            if(aux->lins[j] == 0){
                int el = rand() % linha[j].indice + 1;
                elem *aux2 = linha[j].next;
                while(el > 1){
                    aux2 = aux2->next;
                    el--;
                }
                insereColnoCrom(aux, aux2->indice);
            }
        }
        insereNapop(aux);
    }
}

void geneticOptimizer(){
    geraPopInicial();
    double maiorpeso = populacao[0].peso;
    int ciclosSemMelhoria = 0;
    while(ciclosSemMelhoria < STOP_CRIT_IT){
        cruzaCromossomos();
        mutation();
        VND(&populacao[rand() % POP_SIZE]);
        if(populacao[0].peso != maiorpeso){
            maiorpeso = populacao[0].peso;
            ciclosSemMelhoria = 0;
        } else
            ciclosSemMelhoria++;
    }
}

void printCromossomo(cromossomo *crom){
    printf("\nCromossomo peso: %f , linhas: ",crom->peso);
    int i;
    for(i=1;i<nlins+1;i++)
        printf("%d ",crom->lins[i]);
    printf(" cols->");
    elem *prox_col = crom->cols;
        while(prox_col){
            printf(" %d,",prox_col->indice);
            prox_col = prox_col->next;
       }

}

void printPop(){
    int i;
    for(i=0;i<POP_SIZE;i++){
        printCromossomo(&populacao[i]);
    }
}

int main( int argc, char *argv[] ){
    importaArq("../tests/Teste_01.txt");
    geneticOptimizer();
    printPop();
    printCromossomo(populacao);
  return 0;
}
