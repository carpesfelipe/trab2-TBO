#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define _GNU_SOURCE // Necessário para qsort_r em sistemas GNU/Linux

//criei essa struct para passar os parametros pra qsort e testar(achei feio tbm)
// ele n aceita passar os dois parametros, por isso essa struct "auxiliar"
typedef struct
{
    char **prioridades_L; // Corresponde ao L
    int tamanho_L;        
} ContextoOrdenacao; //usei esse nome pq so ta sendo usado no contexto de ordenacao

// Funcao esta sendo utilizada pra campos de juncao (L1 e L2 e pra receber cada linha de leitura)
// char **obtem_campos_juncao(char *linha, int *tamanho)

// antiga obtem campos juncao
char **linhas_para_campos(char *linha, int *tamanho)
{
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

int compara_linhas(const void *a, const void *b, void *arg)
{
    // recuperando o contexto de ordenação (L e tamanho de L)
    const ContextoOrdenacao *contexto = (const ContextoOrdenacao *)arg;
    char **L = contexto->prioridades_L;
    int tam_L = contexto->tamanho_L;

    char **linha_a_campos = *(char ***)a;
    char **linha_b_campos = *(char ***)b;

    // Itera sobre a lista de prioridades (colunas)
    for (int i = 0; i < tam_L; i++)
    {
        // Converte o campo da prioridade para um índice inteiro de coluna.
        // Por exemplo, se global_prioridades_L[i] for "0", atoi o transforma no int 0.
        int indice_coluna_prioridade = atoi(L[i]);

        // Pega os campos de cada linha na coluna atual da prioridade
        const char *campo_a = linha_a_campos[indice_coluna_prioridade];
        const char *campo_b = linha_b_campos[indice_coluna_prioridade];

        // Compara os campos.
        // Adicione verificações de NULL para campos se eles puderem estar ausentes.
        int comparacao_resultado;
        if (campo_a == NULL && campo_b == NULL){
            comparacao_resultado = 0; // Ambos nulos, considere iguais
        }
        else if (campo_a == NULL){
            comparacao_resultado = -1;
        }
        else if (campo_b == NULL){
            comparacao_resultado = 1; 
        }
        else{
            comparacao_resultado = strcmp(campo_a, campo_b);
        }

        // Se os campos forem diferentes, retorna o resultado da comparação.
        // Isso quebra o empate e define a ordem.
        if (comparacao_resultado != 0)
        {
            return comparacao_resultado;
        }
        // Se os campos forem iguais, continua para a próxima coluna de prioridade para desempate
    }

    return 0;
}

void criando_arquivos(FILE **temp_files, int P){
    for(int i = 0; i < P * 2; i++){
        char nome[260];
        
        // Cria o nome do arquivo temporário
        sprintf(nome, "%d.txt", i); 

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

int preenche_arquivos_temporarios(FILE **temp_files, int P, int M, char *input_file, char **L, int tam_L) {

    // Abre o arquivo de entrada
    FILE *arquivo1 = fopen(input_file, "r");
    if (arquivo1 == NULL) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    // Buffer para armazenar as linhas lidas do arquivo de entrada
    // Cada elemento buffer_linhas[i] será um 'char**' que conterá os campos de uma linha
    char ***buffer_linhas = (char ***)malloc(M * sizeof(char **));

    // Um array auxiliar para armazenar o número de campos de cada linha no buffer,necessário para liberar a memória corretamente (terao sempre o mesmo tamaho\)
    int *num_campos_por_linha_no_buffer = (int *)malloc(M * sizeof(int));

    // Verifica se as alocações foram bem sucedidas
    if (buffer_linhas == NULL || num_campos_por_linha_no_buffer == NULL)
    {
        perror("Erro ao alocar memória para buffer_linhas ou num_campos_por_linha_no_buffer");
        free(buffer_linhas);
        free(num_campos_por_linha_no_buffer);
        fclose(arquivo1);
        return 1;
    }

    // inicializando o buffer_linhas com NULLs 
    for (int i = 0; i < M; i++)
    {
        buffer_linhas[i] = NULL;
        num_campos_por_linha_no_buffer[i] = 0;
    }

    // Variáveis para leitura de linhas
    char *linha_lida = NULL;
    size_t tamanho = 0; // Tamanho da linha lida
    ssize_t read;  // para armazenar o retorno de get line
    int linhas_no_bloco = 0; // conta quantas linhas tem no bloco
    int current_temp_file_index = P; // começa a preencher a partir do arquivo P (que é o arquivo medio)

    //preenchendo o contexto de ordenação (pra usar na ordenacao)
    ContextoOrdenacao contexto_ordenacao;
    contexto_ordenacao.prioridades_L = L;
    contexto_ordenacao.tamanho_L = tam_L;

    // Lê o arquivo de entrada linha por linha
    while ((read = getline(&linha_lida, &tamanho, arquivo1)) != -1){
        // Remove a nova linha ao final, se tiver
        if (read > 0 && linha_lida[read - 1] == '\n') {
            linha_lida[read - 1] = '\0';
        }

        // buffer_linhas[linhas_no_bloco] = strdup(linha_lida);
        // Converte a linha lida em campos
        buffer_linhas[linhas_no_bloco] = linhas_para_campos(linha_lida, &num_campos_por_linha_no_buffer[linhas_no_bloco]);
        if (buffer_linhas[linhas_no_bloco] == NULL) {
            perror("Erro ao duplicar linha para buffer");
            free(linha_lida);
            for (int i = 0; i < linhas_no_bloco; i++)
            {
                destroi_campos_juncao(buffer_linhas[i], num_campos_por_linha_no_buffer[i]);
            }
            free(buffer_linhas);
            free(num_campos_por_linha_no_buffer);
            fclose(arquivo1);
            return 1;
        }
        linhas_no_bloco++;

        // ve se ja chegou em M linhas e se sim, ordena e escreve no arquivo temporário
        if (linhas_no_bloco == M) {
            // qsort(buffer_linhas, M, sizeof(char *), compara_linhas);

            qsort_r(buffer_linhas, M, sizeof(char **), compara_linhas, &contexto_ordenacao);

            for (int i = 0; i < M; i++){
                if (buffer_linhas[i] != NULL){
                    for (int j = 0; j < num_campos_por_linha_no_buffer[i]; j++){
                        fprintf(temp_files[current_temp_file_index], "%s%c",
                                buffer_linhas[i][j],
                                (j < num_campos_por_linha_no_buffer[i] - 1 ? ' ' : '\n'));
                    }
                    if (num_campos_por_linha_no_buffer[i] == 0){ // Pro caso da linha original sr vazia
                        fprintf(temp_files[current_temp_file_index], "\n");
                    }
                }
                else{ // Caso buffer_linhas[i] seja NULL (erro anterior)
                    fprintf(temp_files[current_temp_file_index], "\n");
                }

                // Libera a memória alocada para os campos desta linha
                destroi_campos_juncao(buffer_linhas[i], num_campos_por_linha_no_buffer[i]);
                buffer_linhas[i] = NULL; // Limpa o ponteiro após a liberação
                num_campos_por_linha_no_buffer[i] = 0;
            }
            current_temp_file_index++; 
            if(current_temp_file_index >= (P * 2)) {
                current_temp_file_index = P; 
            }
            linhas_no_bloco = 0;
        }
    }

    // Se ainda houver linhas no buffer após a leitura do arquivo (acontece quando o bloco nao completou M linhas)
    if (linhas_no_bloco > 0) {
        // qsort(buffer_linhas, linhas_no_bloco, sizeof(char **), compara_linhas);

        qsort_r(buffer_linhas, M, sizeof(char **), compara_linhas, &contexto_ordenacao);

        for (int i = 0; i < linhas_no_bloco; i++){
            if (buffer_linhas[i] != NULL){
                for (int j = 0; j < num_campos_por_linha_no_buffer[i]; j++){
                    fprintf(temp_files[current_temp_file_index], "%s%c",
                            buffer_linhas[i][j],
                            (j < num_campos_por_linha_no_buffer[i] - 1 ? ' ' : '\n'));
                }
                if (num_campos_por_linha_no_buffer[i] == 0){
                    fprintf(temp_files[current_temp_file_index], "\n");
                }
            }
            else{
                fprintf(temp_files[current_temp_file_index], "\n");
            }
            // Libera a memória para os campos desta linha
            destroi_campos_juncao(buffer_linhas[i], num_campos_por_linha_no_buffer[i]);
            buffer_linhas[i] = NULL; // Limpa o ponteiro após a liberação
            num_campos_por_linha_no_buffer[i] = 0;
        }
    }

    free(linha_lida); 
    free(buffer_linhas);
    free(num_campos_por_linha_no_buffer);
    fclose(arquivo1);

    return 0; 
}