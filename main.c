#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

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

// Comparação de strings para qsort
int compara_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
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

    // ORDENAR CADA BLOCO de M linhas COM QUICKSORT(OU MERGESORT)
    //ordena em ordem alfabedica cada bloco de M linhas com quicksort
    
    // // Buffer para armazenar as M linhas em memória
    // char **buffer_linhas = (char **)malloc(M * sizeof(char *));
    // if (buffer_linhas == NULL) {
    //     perror("Erro ao alocar memória para buffer_linhas");
    //     fclose(input_file);
    //     return 1;
    // }

    // char *linha_lida = NULL; // Usado por getline
    // size_t len = 0;         // Tamanho do buffer de getline
    // ssize_t read;           // Retorno de getline

    // int linhas_no_bloco = 0; // Conta quantas linhas já estão no buffer (até M)
    // int run_count = 0;       // Conta quantos arquivos de "run" já foram criados

    // // PERCORRER O ARQUIVO DE ENTRADA, QUEBRAR EM BLOCOS DE TAMANHO M,
    // // ORDENAR CADA BLOCO E ESCREVER EM ARQUIVOS TEMPORÁRIOS

    // while ((read = getline(&linha_lida, &len, input_file)) != -1) {
    //     // Remove o newline no final, se existir
    //     if (read > 0 && linha_lida[read - 1] == '\n') {
    //         linha_lida[read - 1] = '\0';
    //     }

    //     // Armazena uma cópia da linha lida no buffer da memória
    //     buffer_linhas[linhas_no_bloco] = strdup(linha_lida);
    //     if (buffer_linhas[linhas_no_bloco] == NULL) {
    //         perror("Erro ao duplicar linha para buffer");
    //         // Limpeza de memória em caso de erro grave
    //         free(linha_lida);
    //         for (int i = 0; i < linhas_no_bloco; i++) free(buffer_linhas[i]);
    //         free(buffer_linhas);
    //         fclose(input_file);
    //         return 1;
    //     }
    //     linhas_no_bloco++;

    //     // Verifica se o bloco atual está cheio (atingiu M linhas)
    //     if (linhas_no_bloco == M) {
    //         // ORDENAR CADA BLOCO DE M LINHAS COM QUICKSORT
    //         qsort(buffer_linhas, M, sizeof(char *), compara_strings);

    //         if (temp_files[file_index] == NULL) {
    //             perror("Erro ao criar arquivo de run temporário");
    //             // Limpeza de memória
    //             free(linha_lida);
    //             for (int i = 0; i < M; i++) free(buffer_linhas[i]);
    //             free(buffer_linhas);
    //             fclose(input_file);
    //             return 1;
    //         }

    //         for (int i = 0; i < M; i++) {
    //             fprintf(temp_files[file_index], "%s\n", buffer_linhas[i]);
    //             free(buffer_linhas[i]); // Libera a memória da linha duplicada após escrever
    //         }
    //         fclose(temp_files[file_index]); // Fecha o arquivo do run

    //         linhas_no_bloco = 0; // Reinicia o contador para o próximo bloco
    //         run_count++;        // Incrementa o número de runs criados

    //         file_index++;
    //         if(file_index > ((2*P)-1)){
    //             printf("vou voltar a preencher em P\n");
    //             file_index = P; //volta a preencher a partir de P
    //         }
    //     }
    // }

    // // Lida com o último bloco de linhas, que pode ter menos de M linhas
    // if (linhas_no_bloco > 0) {
    //     qsort(buffer_linhas, linhas_no_bloco, sizeof(char *), compara_strings);

    //     // char nome_run[260];
    //     // sprintf(nome_run, "run_%d.txt", run_count);
    //     for (int i = 0; i < M; i++) {
    //             fprintf(temp_files[file_index], "%s\n", buffer_linhas[i]);
    //             free(buffer_linhas[i]); // Libera a memória da linha duplicada após escrever
    //     }
    //     // FILE *output_run_file = fopen(nome_run, "w");
    //     // if (output_run_file == NULL) {
    //     //     perror("Erro ao criar arquivo de run temporário (último bloco)");
    //     //     // Limpeza de memória
    //     //     free(linha_lida);
    //     //     for (int i = 0; i < linhas_no_bloco; i++) free(buffer_linhas[i]);
    //     //     free(buffer_linhas);
    //     //     fclose(input_file);
    //     //     return 1;
    //     // }

    //     // for (int i = 0; i < linhas_no_bloco; i++) {
    //     //     fprintf(output_run_file, "%s\n", buffer_linhas[i]);
    //     //     free(buffer_linhas[i]); // Libera a memória da linha
    //     // }
    //     fclose(temp_files[file_index]);
    // }

    // // Libera a memória e fecha o arquivo de entrada
    // free(linha_lida); // Liberado pela getline, se read == -1 na última chamada
    // free(buffer_linhas);
    // fclose(input_file);

    // IMPRESSAO DO ARQUIVO 3

    return 0;
}