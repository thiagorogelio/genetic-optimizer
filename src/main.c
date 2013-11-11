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

void insereElem(int c, int l){
    elem *el = malloc(sizeof(elem));
    el->indice = l;
    if(coluna[c].next == NULL){
        coluna[c].last = el;
        coluna[c].next = el;
    } else
    {
        coluna[c].last->next = el;
        coluna[c].last = el;
    }
    linha[l].indice++;
    elem *nova_col = malloc(sizeof(elem));
    nova_col->indice = c;
    nova_col->next = linha[l].next;
    linha[l].next = nova_col;
}

void splitLinha(char str[]){
    char *buff;
    buff = strtok(str," ");
    while(buff != NULL){
        if(strcmp(buff, "LINHAS") == 0 || strcmp(buff, "Linhas") == 0){
            buff = strtok(NULL," ");
            nlins = atoi(buff);
            linha = calloc(nlins+1, sizeof(elem));
            /*int i;
            for(i=0;i < atoi(buff)+2; i++)
                linha[i].indice = 0;*/
        } else
        if(strcmp(buff, "COLUNAS") == 0 || strcmp(buff, "Colunas") == 0){
            buff = strtok(NULL," ");
            ncol = atoi(buff);
            coluna = calloc(ncol+1, sizeof(list));
            //printf("%d colunas",atoi(buff)+1);
            /*int i;//
            for(i=0;i < ncol+1; i++){
                coluna[i].next = NULL;
                coluna[i].peso = 0;
            }*/
        } else {
                int c = atoi(buff);
                if(c != 0){
                    buff = strtok(NULL," ");
                    coluna[c].peso = atof(buff);
                    buff = strtok(NULL," ");
                    while(buff != NULL){
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
        char str[100];
        str[0] = '\0';
        while(c != EOF){
            if(c != '\n')
                sprintf(str,"%s%c",str,c);
            else{
                splitLinha(str);
                str[0] = '\0';
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
        while(el != NULL){
            printf("%d ",el->indice);
            el = el->next;
        }
    }
    for(i=1;i<nlins+1;i++){
        printf("\nLinha %d, Cols: %d elems:",i,linha[i].indice);
        elem *el = linha[i].next;
         while(el != NULL){
            printf("%d ",el->indice);
            el = el->next;
        }
    }
}

void insereColnoCrom(cromossomo *crom, int ncol){
    crom->peso = coluna[ncol].peso + crom->peso;
    elem *col = malloc(sizeof(elem));
    col->indice = ncol;
    col->next = crom->cols;
    crom->cols = col;
    elem *cobert_linhas = coluna[ncol].next;
    while(cobert_linhas != NULL){
        crom->lins[cobert_linhas->indice]++;
        cobert_linhas = cobert_linhas->next;
    }
}

void insereNapop(cromossomo *crom){
    int i = 0;
    if(populacao[i].peso == 0)
        populacao[i] = *crom;
    else {
          while(populacao[i].peso != 0){i++;}
          while(populacao[i-1].peso > crom->peso && i > 0){
                populacao[i] = populacao[i-1];
                i--;
                if(i == 0)
                    break;
          }
          populacao[i] = *crom;
    }
}

void geraPopInicial(){
    populacao = calloc(INITIAL_POP+1, sizeof(cromossomo));
    srand (time(NULL));
    int i;
    for(i=0;i<INITIAL_POP;i++){
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
        //printCromossomo(aux);
        insereNapop(aux);
    }
}

void cruzaCromossomos(){
    cromossomo *pai1, *pai2, novo;
    int rand1, rand2;
    rand1 = 0;
    rand2 = 0;
    while(rand1 == rand2){
        rand1 = rand() %  INITIAL_POP;
        rand2 = rand() %  INITIAL_POP;
    }
    printf("\npai1 %d , pai2 %d",rand1,rand2);
    getchar();
    pai1 = &populacao[rand1];
    pai2 = &populacao[rand2];
    novo.peso = pai1->peso + pai2->peso;
    novo.cols = pai1->cols;
    elem *cols = novo.cols;
    while(cols->next != NULL){
        cols = cols->next;
    }
    cols->next = pai2->cols;
    novo.lins = pai1->lins;
    int i;
    for(i = 1; i < nlins+1; i++)
        novo.lins[i] += pai2->lins[i];

    printf("novo crom peso pai1 %f pai2 %f res %f",pai1->peso,pai2->peso,novo.peso);
    //insereNapop(aux);
}

void geneticOptimizer(){
    geraPopInicial();
    float menorpeso = populacao[0].peso;
    int ciclosSemMelhoria = 0;
    while(ciclosSemMelhoria < 100){
        cruzaCromossomos();
        if(populacao[0].peso < menorpeso){
            menorpeso = populacao[0].peso;
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
}

void imprimePop(){
    int i;
    for(i=0;i<INITIAL_POP;i++){
        printf("\ncromossomo %d, peso %f cols ->",i,populacao[i].peso);
        elem *prox_col = populacao[i].cols;
        while(prox_col != NULL){
            printf(" %d,",prox_col->indice);
            prox_col = prox_col->next;
       }
    }
}

int main( int argc, char *argv[] ){
   importaArq("../tests/Teste_01.txt");
   geneticOptimizer();
  return 0;
}
