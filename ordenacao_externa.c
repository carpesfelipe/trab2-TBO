#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

// Funcao para obter os campos de junção de uma linha
char **obtem_campos_juncao(char *linha, int *tamanho){
    int qtd=100;
    char **campos=calloc(qtd,sizeof(char*));
    int i=0;

    // copia temp da linha para evitar modificar a original
    char *copia_linha = strdup(linha);
    char * token=strtok(copia_linha,",");
    while(token!=NULL){
        if(i>=qtd){
            qtd *= 2; 
            campos=realloc(campos,qtd*sizeof(char*));
            if (campos == NULL) {
                perror("Erro ao realocar memória para campos");
                exit(EXIT_FAILURE);
            }
        }
        campos[i] = strdup(token);
        token=strtok(NULL,",");
        i++;
    }
    *tamanho=i;
    
    // libera a copia da linha
    free(copia_linha);

    return campos;
}

void destroi_campos_juncao(char ** l, int tam){
     for(int i=0;i<tam;i++){
        free(l[i]);
    }
    free(l);
}

int compara_linhas(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}


void criando_arquivos(FILE **temp_files, int P){
    for(int i = 0; i < P * 2; i++){
        char nome[260];
        
        // Cria o nome do arquivo temporário
        sprintf(nome, "%d.txt", i); 
        // Abre o arquivo temporário para leitura e escrita

        temp_files[i] = fopen(nome, "w+"); // uso de w+ para permitir leitura e escrita
        if (temp_files[i] == NULL) {
            perror("Erro ao criar arquivo temporário");
            // Fecha os arquivos já abertos antes de sair
            for (int k = 0; k < i; k++) {
                fclose(temp_files[k]);
            }
            exit(EXIT_FAILURE);
        }
    }
}

int preenche_arquivos_temporarios(FILE **temp_files, int P, int M, char *input_file, char **L, int tam) {

    FILE *arquivo1 = fopen(input_file, "r");
    if (arquivo1 == NULL) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }
    // Buffer para armazenar as linhas lidas do arquivo de entrada
    char **buffer_linhas = (char **)malloc(M * sizeof(char *));

    // Verifica se a alocação foi bem-sucedida
    if (buffer_linhas == NULL) {
        perror("Erro ao alocar memória para buffer_linhas");
        fclose(arquivo1);
        for(int i = 0; i < P * 2; i++) {
            if (temp_files[i]) fclose(temp_files[i]);
        }
        return 1;
    }


    char *linha_lida = NULL;
    size_t len = 0; // Tamanho da linha lida
    ssize_t read;  // para armazenar o retorno de get line
    int linhas_no_bloco = 0; // conta quantas linhas tem no bloco
    int current_temp_file_index = P; // começa a preencher a partir do arquivo P (que é o arquivo medio)

    // Lê o arquivo de entrada linha por linha
    while ((read = getline(&linha_lida, &len, arquivo1)) != -1) {
        // Remove a nova linha ao final, se tiver
        if (read > 0 && linha_lida[read - 1] == '\n') {
            linha_lida[read - 1] = '\0';
        }

        buffer_linhas[linhas_no_bloco] = strdup(linha_lida);
        if (buffer_linhas[linhas_no_bloco] == NULL) {
            perror("Erro ao duplicar linha para buffer");
            free(linha_lida);
            for (int i = 0; i < linhas_no_bloco; i++) free(buffer_linhas[i]);
            free(buffer_linhas);
            fclose(arquivo1);
            for(int i = 0; i < P * 2; i++) {
                if (temp_files[i]) fclose(temp_files[i]);
            }
            return 1;
        }
        linhas_no_bloco++;

        // ve se ja chegou em M linhas
        if (linhas_no_bloco == M) {
            qsort(buffer_linhas, M, sizeof(char *), compara_linhas);

            // preenche o arquivo temporário atual com as linhas ordenadas
            for (int i = 0; i < M; i++) {
                fprintf(temp_files[current_temp_file_index], "%s\n", buffer_linhas[i]);
                free(buffer_linhas[i]);
            }
            
            current_temp_file_index++; 
            if(current_temp_file_index >= (P * 2)) {
                current_temp_file_index = P; 
            }
            linhas_no_bloco = 0;
        }
    }

    // Se ainda houver linhas no buffer após a leitura do arquivo (nao completou M linhas)
    if (linhas_no_bloco > 0) {
        qsort(buffer_linhas, linhas_no_bloco, sizeof(char *), compara_linhas);

        for (int i = 0; i < linhas_no_bloco; i++) {
            fprintf(temp_files[current_temp_file_index], "%s\n", buffer_linhas[i]);
            free(buffer_linhas[i]);
        }
    }

    free(linha_lida); 
    free(buffer_linhas);
    fclose(arquivo1);

    for(int i = 0; i < P * 2; i++) {
        if (temp_files[i]) fclose(temp_files[i]);
    }

    return 0; 
}