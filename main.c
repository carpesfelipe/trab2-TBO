#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ordenacao_externa.h"
#include "merge.h"

int main(int argc, char *argv[]){
    // Verifica se o numero de parâmetros é passado corretamente
    // if (argc < 8) {
    //     return 1;
    // }

    // armazena os valores de P e M
    //M quantidade de registros na memória
    int P = atoi(argv[1]);
    int M = atoi(argv[2]);

    int tam; 
    char **L1 = obtem_campos_juncao(argv[3], &tam);
    char **L2 = obtem_campos_juncao(argv[4], &tam);

    //cria os 2P arquivos
    FILE *temp_files[P * 2]; 
    criando_arquivos(temp_files, P);
    // preenche os arquivos temporários com as m linhas ordenadas
    // para aquivo 1
    preenche_arquivos_temporarios(temp_files, P, M, argv[5], L1, tam);
    
    // if(x != 0) {
    //     return x; 
    // }

    ordenar_blocos(temp_files, P, M, argv[5], L1, tam);


    destroi_campos_juncao(L1, tam);
    destroi_campos_juncao(L2, tam);

    return 0;
}