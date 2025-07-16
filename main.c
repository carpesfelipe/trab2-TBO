#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ordenacao_externa.h"
#include "merge.h"
#include "juncao.h"

int main(int argc, char *argv[]){
    // Verifica se o numero de parâmetros é passado corretamente
    // if (argc < 8) {
    //     return 1;
    // }

    int P = atoi(argv[1]);
    int M = atoi(argv[2]);
    int qtd_campos_juncao; 
    char **L1 = obtem_campos_juncao(argv[3], &qtd_campos_juncao);
    char **L2 = obtem_campos_juncao(argv[4], &qtd_campos_juncao);

    FILE *temp_files[P * 2]; 
    cria_arquivos_temporarios(temp_files, P);
    preenche_arquivos_temporarios(temp_files, P, M, argv[5], L1, qtd_campos_juncao);
    //ordenar_blocos(temp_files, P, M, argv[5], L1, qtd_campos_juncao);

    // para arq2
    //preenche_arquivos_temporarios(temp_files, P, M, argv[6], L2, qtd_campos_juncao);
    //ordenar_blocos(temp_files, P, M, argv[6], L2, qtd_campos_juncao);

    // // FILE * arq1=fopen("teste_juncao/1/arquivo1.txt","r");
    // // FILE * arq2=fopen("teste_juncao/1/arquivo2.txt","r");
    //  FILE * arq1=fopen("teste_juncao/2/arquivo1.txt","r");
    //  FILE * arq2=fopen("teste_juncao/2/arquivo2.txt","r");
    // if (arq1 == NULL || arq2==NULL)
    // {
    //     perror("Erro ao abrir arquivo de entrada");
    //     exit(1);
    // }
    // imprime_arquivo_juncao(arq1,arq2,argv[7],L1,L2,M,qtd_campos_juncao);

    destroi_campos_juncao(L1, qtd_campos_juncao);
    destroi_campos_juncao(L2, qtd_campos_juncao);
    // fclose(arq1);
    // fclose(arq2);

    return 0;
}