#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ** obtem_campos_juncao(char * linha,int *tamanho){
    char **campos=calloc(100,sizeof(char*));
    int i=0;
    char * token=strtok(linha,",");
    while(token!=NULL){
        campos[i] = strdup(token);
        token=strtok(NULL,",");
        i++;
    }
    *tamanho=i;
    return campos;
}
void destroi_campos_juncao(char ** l, int tam){
     for(int i=0;i<tam;i++){
        free(l[i]);
    }
    free(l);
}
int main(int argc, char *argv[])
{
    // Verifica se o numero de parâmetros é passado corretamente
    // if (argc < 8) {
    //     return 1;
    // }
    // int P = argv[1];
    // int M= argv[2];

    int tam;
    char **L1=obtem_campos_juncao(argv[3],&tam);
    char **L2=obtem_campos_juncao(argv[4],&tam);
    
    
    destroi_campos_juncao(L1,tam);
    destroi_campos_juncao(L2,tam);

    // PERCORRER OS ARQUIVOS DE ENTRADA E QUEBRAR EM BLOCOS DE TAMANHO M

    // OS BLOCOS SERAO ESCRITOS EM 2P ARQUIVOS(NO MAXIMO)

    // ORDENAR CADA BLOCO COM QUICKSORT(OU MERGESORT)

    // IMPRESSAO DO ARQUIVO 3

    return 0;
}