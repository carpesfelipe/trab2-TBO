#include "linha.h"
#include <ctype.h>
#include <string.h>

char *trim(char *str) {
    char *end;

    // Avança até o primeiro caractere não branco
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // string vazia ou só com espaços
        return str;

    // Vai para o fim da string
    end = str + strlen(str) - 1;

    // Anda para trás enquanto houver espaço no final
    while (end > str && isspace((unsigned char)*end)) end--;

    // Coloca o terminador de string após o último caractere válido
    *(end + 1) = '\0';

    return str;
}

int compara_linhas(const void *linha1, const void *linha2)
{

    int index1;
    int index2;
    for (int i = 0; i < LINHA(linha1).qtd_campos_juncao; i++)
    {

        index1 = atoi(LINHA(linha1).campos_juncao[i]);
        index2 = atoi(LINHA(linha2).campos_juncao[i]);

        int ehDiferenteString = strcmp(trim(LINHA(linha1).colunas[index1]), trim(LINHA(linha2).colunas[index2]));
        if (ehDiferenteString!=0)
        {
            return ehDiferenteString;
        }
    }
    return 0;
}

void ordena_linhas(Linha *vet_linhas, int qtd_linhas)
{
    qsort(vet_linhas, qtd_linhas, sizeof(Linha), compara_linhas);
}
// Recebe uma linha do arquivo(com get_line), o vetor de campos de junção
Linha inicia_linha(char **campos_juncao, int qtd_campos)
{
    Linha L;
    L.colunas = calloc(100, sizeof(char *));
    L.campos_juncao = campos_juncao;
    L.qtd_campos_juncao = qtd_campos;
    L.qtd_colunas = 0;
    return L;
}

void add_campos(Linha *linha_ptr, char *string)
{
    char *token = strtok(string, ",");
    int i = 0;
    int qtd = 100;
    while (token != NULL)
    {
        if (i >= qtd)
        {
            qtd *= 2;
            char **tmp = realloc((*(linha_ptr)).colunas, qtd * sizeof(char *));
            if (!tmp)
            {
                perror("Erro ao realocar colunas");
                exit(1);
            }
            (*(linha_ptr)).colunas = tmp;
        }
        (*(linha_ptr)).colunas[i] = strdup(token);
        (*(linha_ptr)).qtd_colunas++;
        token = strtok(NULL, ",");
        i++;
    }
}

void imprime_linhas_arquivo(Linha *vet_linha, FILE *file, int M)
{

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < vet_linha[i].qtd_colunas; j++)
        {
            if (j != vet_linha[i].qtd_colunas - 1)
            {
                fprintf(file, "%s,", vet_linha[i].colunas[j]);
            }
            else
            {
                fprintf(file, "%s", vet_linha[i].colunas[j]);
            }
        }
    }
}
void destroi_linha(Linha l)
{
    for (int i = 0; i < l.qtd_colunas; i++)
    {
        free(l.colunas[i]);
    }
    free(l.colunas);
}
