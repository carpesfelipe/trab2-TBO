#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ordenacao_externa.h"
#include "merge.h"
#include "juncao.h"

int main(int argc, char *argv[]){

    int P = atoi(argv[1]);
    int M = atoi(argv[2]);
    int qtd_campos_juncao; 
    char **L1 = obtem_campos_juncao(argv[3], &qtd_campos_juncao);
    char **L2 = obtem_campos_juncao(argv[4], &qtd_campos_juncao);

    //para o arquivo de entrada 1
    FILE *temp_files1[P * 2]; 
    char *arq1 = "arq1_"; // Prefixo para os arquivos temporários do primeiro arquivo de entrada
    cria_arquivos_temporarios(temp_files1, P, arq1);
    preenche_arquivos_temporarios(temp_files1, P, M, argv[5], L1, qtd_campos_juncao);
    ordena_blocos(temp_files1, P, M, argv[5], L1, qtd_campos_juncao, arq1);

    //para arquivo de entrada 2
    FILE *temp_files2[P * 2];
    char *arq2 = "arq2_"; // Prefixo para os arquivos temporários do segundo arquivo de entrada
    cria_arquivos_temporarios(temp_files2, P, arq2);
    preenche_arquivos_temporarios(temp_files2, P, M, argv[6], L2, qtd_campos_juncao);
    ordena_blocos(temp_files2, P, M, argv[6], L2, qtd_campos_juncao, arq2);

    // Juncao dos dois arquivos ordenados
    imprime_arquivo_juncao(argv[7],L1,L2,M,qtd_campos_juncao);

    destroi_campos_juncao(L1, qtd_campos_juncao);
    destroi_campos_juncao(L2, qtd_campos_juncao);
   

    return 0;
}