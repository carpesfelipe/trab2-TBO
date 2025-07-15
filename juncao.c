#include "juncao.h"

void imprime_arquivo_juncao(FILE *arq1, FILE *arq2, char *caminho_arquivo_saida, char **L1, char **L2, int M, int qtd_campos_juncao)
{
    FILE *arquivo_saida = fopen(caminho_arquivo_saida, "w");
    if (arquivo_saida == NULL)
    {
        perror("Erro ao abrir arquivo de saída");
        exit(1);
    }

    char *linha_lida1 = NULL;
    char *linha_lida2 = NULL;
    Linha vet_linhas[M];
    Linha linha1 = inicia_linha(L1, qtd_campos_juncao);
    Linha linha2 = inicia_linha(L2, qtd_campos_juncao);
    size_t aux1 = 0, aux2 = 0;
    int fim_de_arquivo1 = getline(&linha_lida1, &aux1, arq1);
    int fim_de_arquivo2 = getline(&linha_lida2, &aux2, arq2);
    add_campos(&linha1, linha_lida1);
    add_campos(&linha2, linha_lida2);
    int tam_bloco = 0;

    while ((fim_de_arquivo1 != -1) && (fim_de_arquivo2 != -1))
    {
        int cmp = compara_linhas((const void *)&linha1, (const void *)&linha2);
        if ((cmp == 0))
        {
            vet_linhas[tam_bloco] = mescla_linha(linha1, linha2);
            tam_bloco++;
            destroi_linha(linha1);
            destroi_linha(linha2);
            linha1 = inicia_linha(L1, qtd_campos_juncao);
            linha2 = inicia_linha(L2, qtd_campos_juncao);
            fim_de_arquivo1 = getline(&linha_lida1, &aux1, arq1);
            fim_de_arquivo2 = getline(&linha_lida2, &aux2, arq2);
            add_campos(&linha1, linha_lida1);
            add_campos(&linha2, linha_lida2);
        }
        else if (cmp < 0)
        {
            destroi_linha(linha1);
            linha1 = inicia_linha(L1, qtd_campos_juncao);
            fim_de_arquivo1 = getline(&linha_lida1, &aux1, arq1);
            add_campos(&linha1,linha_lida1);
        }
        else
        {          
            destroi_linha(linha2);
            linha2 = inicia_linha(L2, qtd_campos_juncao);
            fim_de_arquivo2 = getline(&linha_lida2, &aux1, arq2);
            add_campos(&linha2,linha_lida2);
        }

        if (tam_bloco == M)
        {
            imprime_linhas_arquivo(vet_linhas, arquivo_saida, M);
            for (int i = 0; i < tam_bloco; i++)
            {
                destroi_linha(vet_linhas[i]);
            }
            tam_bloco = 0;
        }

    }

    if (tam_bloco > 0)
    {
        imprime_linhas_arquivo(vet_linhas, arquivo_saida, tam_bloco);
        for (int i = 0; i < tam_bloco; i++)
        {
            destroi_linha(vet_linhas[i]);
        }
    }
    destroi_linha(linha1);
    destroi_linha(linha2);
    free(linha_lida1);
    free(linha_lida2);
    fclose(arquivo_saida);
}

// retorna uma linha com os campos de juncao e o restante das colunas de cada linha
Linha mescla_linha(Linha l1, Linha l2)
{
    Linha linha_mesclada;
    linha_mesclada = inicia_linha(l1.campos_juncao, l1.qtd_campos_juncao);
    int index;
    printf("linha mesclada:");
    for (int i = 0; i < l1.qtd_campos_juncao; i++)
    {
        index = atoi(l1.campos_juncao[i]);
        add_campos(&linha_mesclada, l1.colunas[index]);
    }
   
    for (int i = 0; i < l1.qtd_colunas; i++)
    {
        if (!eh_campo_juncao(i, l1))
        {
            add_campos(&linha_mesclada, l1.colunas[i]);
        }
    }
    
    for (int i = 0; i < l2.qtd_colunas; i++)
    {
        if (!eh_campo_juncao(i, l2))
        {
            add_campos(&linha_mesclada, l2.colunas[i]);
        }
    }

    return linha_mesclada;
}

int eh_campo_juncao(int j, Linha l)
{
    for (int i = 0; i < l.qtd_campos_juncao; i++)
    {
        if (j == atoi(l.campos_juncao[i]))
        {
            return 1;
        }
    }
    return 0;
}

void compara_arquivo(FILE * file1,FILE *file2){
    
}