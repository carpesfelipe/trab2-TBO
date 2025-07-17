#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "linha.h"
typedef struct linha Linha;

//Funcao para separar os campos de juncao em um vetor de strings
char **obtem_campos_juncao(char *linha, int *tamanho)
{
    int qtd = 100;
    char **campos = calloc(qtd, sizeof(char *));
    int i = 0;

    // copia temp da linha para evitar modificar a original
    char *copia_linha = strdup(linha);
    char *token = strtok(copia_linha, ",");
    while (token != NULL)
    {
        if (i >= qtd)
        {
            qtd *= 2;
            campos = realloc(campos, qtd * sizeof(char *));
            if (campos == NULL)
            {
                perror("Erro ao realocar memória para campos");
                exit(EXIT_FAILURE);
            }
        }
        campos[i] = strdup(token);
        token = strtok(NULL, ",");
        i++;
    }
    *tamanho = i;

    // libera a copia da linha
    free(copia_linha);

    return campos;
}

void destroi_campos_juncao(char **l, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        free(l[i]);
    }
    free(l);
}
//cria os 2P dispositivos auxiliares para ordenacao externa
void cria_arquivos_temporarios(FILE **temp_files, int P, int M, char *input_file)
{
    for (int i = 0; i < P * 2; i++)
    {
        char nome[260];

        // Cria o nome do arquivo temporário
        sprintf(nome, "%d.txt", i);
        // Abre o arquivo temporário para leitura e escrita

        temp_files[i] = fopen(nome, "w+"); // uso de w+ para permitir leitura e escrita
        if (temp_files[i] == NULL)
        {
            perror("Erro ao criar arquivo temporário");
            // Fecha os arquivos já abertos antes de sair
            for (int k = 0; k < i; k++)
            {
                fclose(temp_files[k]);
            }
            exit(EXIT_FAILURE);
        }
    }
}
//preenche os arquivos temporários em blocos de M registros(Linhas) ordenados 
void preenche_arquivos_temporarios(FILE **temp_files, int P, int M, char *input_file, char **L, int tam)
{
    // Abre arquivo
    FILE *arquivo1 = fopen(input_file, "r");
    if (arquivo1 == NULL)
    {
        perror("Erro ao abrir arquivo de entrada");
        exit(1);
    }
    char *linha_lida = NULL;
    int tam_bloco = 0; // conta quantas linhas tem no bloco
    size_t aux = 0;
    int current_temp_file_index = P; // começa a preencher a partir do arquivo P (que é o arquivo medio)
    Linha *vet_linhas = calloc(M, sizeof(Linha));
    // Lê o arquivo de entrada linha por linha
    while (getline(&linha_lida, &aux, arquivo1) != -1)
    {
        
        vet_linhas[tam_bloco] = inicia_linha(L, tam);
        // separa os campos da linha lida e adiciona na struct
        add_campos(&(vet_linhas[tam_bloco]), linha_lida);
        tam_bloco++;
        if (tam_bloco == M)
        {
            qsort(vet_linhas, M, sizeof(Linha), compara_linhas);
            imprime_linhas_arquivo(vet_linhas, temp_files[current_temp_file_index], tam_bloco);

            // Adiciona o ponto de quebra (FIMBLOCO) imediatamente após o bloco
            fprintf(temp_files[current_temp_file_index], "FIMBLOCO\n");

            for (int i = 0; i < tam_bloco; i++)
            {
                destroi_linha(vet_linhas[i]);  
            }
            current_temp_file_index++;
            if (current_temp_file_index >= (P * 2))
            {
                current_temp_file_index = P;
            }
            tam_bloco = 0;
        }
    }
    if (tam_bloco > 0)
    {
        qsort(vet_linhas, tam_bloco, sizeof(Linha), compara_linhas);
        imprime_linhas_arquivo(vet_linhas, temp_files[current_temp_file_index], tam_bloco);

        // Adiciona o ponto de quebra (FIMBLOCO) imediatamente após o bloco
        fprintf(temp_files[current_temp_file_index], "FIMBLOCO\n");

        for (int i = 0; i < tam_bloco; i++)
        {
            
            destroi_linha(vet_linhas[i]);

        }
    }
    free(linha_lida);
    free(vet_linhas);
    fclose(arquivo1);

    for (int i = 0; i < P * 2; i++)
    {
        if (temp_files[i])
            fclose(temp_files[i]);
    }
}
