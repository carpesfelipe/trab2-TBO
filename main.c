#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ** obtem_campos_juncao(char * linha,int *tamanho){
    int qtd=100;
    char **campos=calloc(qtd,sizeof(char*));
    int i=0;
    char * token=strtok(linha,",");
    while(token!=NULL){
        if(i>=qtd){
            campos=realloc(campos,qtd*2);
        }
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

    //recebe p e m
    int P = atoi(argv[1]);
    int M = atoi(argv[2]);

    int tam;
    char **L1=obtem_campos_juncao(argv[3],&tam);
    char **L2=obtem_campos_juncao(argv[4],&tam);
    
    destroi_campos_juncao(L1,tam);
    destroi_campos_juncao(L2,tam);

    //###########################################################
    // Abre o arquivo de entrada
    FILE *input_file = fopen(argv[5], "r");
    if (input_file == NULL) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    //criar 2p arquivos
    FILE *temp_files[P * 2]; // Array de ponteiros para arquivos temporários
    for(int i=0;i<P*2;i++){
        char nome[260];
        sprintf(nome,"arquivo%d.txt",i);
        temp_files[i] = fopen(nome, "w");
        if (temp_files[i] == NULL) {
            perror("Erro ao criar arquivo temporário");
            // Fechar arquivos já abertos antes de sair
            for (int k = 0; k < i; k++) {
                fclose(temp_files[k]);
            }
            fclose(input_file);
            return 1;
        }
    }

    // PERCORRER OS ARQUIVOS DE ENTRADA E QUEBRAR EM BLOCOS DE TAMANHO M 
    char linha[1024]; // Buffer para ler linhas do arquivo

    int bloco_index = 0; // Índice do bloco atual (conta qtd de linhas lidas)
    int file_index = P; // Começa a "particionar" nos arquivos temporários a partir do P

    while (fgets(linha, sizeof(linha), input_file) != NULL) {
        // Verifica se o bloco atual está cheio
        if (bloco_index >= M) {
            bloco_index = 0; // Reinicia o índice do bloco
            file_index++; // Passa para o próximo arquivo temporário
            if(file_index > (P * 2 - 1)) {
                file_index = P; // Reinicia para o primeiro arquivo temporário
            }
        }

        // Escreve a linha no arquivo temporário atual
        fprintf(temp_files[file_index], "%s", linha);
        bloco_index++;
    }

    // OS BLOCOS SERAO ESCRITOS EM 2P ARQUIVOS(NO MAXIMO)

    // ORDENAR CADA BLOCO COM QUICKSORT(OU MERGESORT)

    // IMPRESSAO DO ARQUIVO 3

    return 0;
}