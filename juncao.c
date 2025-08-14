#include "juncao.h"
//Dado dois arquivos ordenados, imprime junção(combinação dos registros) dos dois arquivos de 
//cada um em CAMINHO_ARQUIVO_SAIDA
void imprime_arquivo_juncao(char *caminho_arquivo_saida, char **L1, char **L2, int M, int qtd_campos_juncao)
{
    FILE * arq1=fopen("arq1_0.txt","r");
    FILE * arq2=fopen("arq2_0.txt","r");
    if (arq1 == NULL || arq2==NULL)
    {
        perror("Erro ao abrir arquivo de entrada");
        exit(1);
    }
    FILE *arquivo_saida = fopen(caminho_arquivo_saida, "w");
    if (arquivo_saida == NULL)
    {
        perror("Erro ao abrir arquivo de saída");
        exit(1);
    }

    char *linha_lida1 = NULL;
    char *linha_lida2 = NULL;
    //Linha vet_linhas[M];
    Linha *vet_linhas=calloc(M,sizeof(Linha));
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
        if(!strcmp(linha1.colunas[0],"FIMBLOCO") || !strcmp(linha2.colunas[0],"FIMBLOCO")){
            break;
        }
        int cmp = compara_linhas((const void *)&linha1, (const void *)&linha2);
        if ((cmp == 0))
        {
            //caso as linhas sejam iguais(cmp==0) fazemos a mesclagem da linha(com k1+k2-l campos)
            //e armazenamos o resultado em um vetor de Linhas
            vet_linhas[tam_bloco] = mescla_linha(linha1, linha2);
            tam_bloco++;
            //destruímos os itens dentro das linhas utilizadas para mesclar
            //e avançamos nos dois arquivos, criando novas Linhas para serem comparadas
            destroi_linha(linha1);
            destroi_linha(linha2);
            linha1 = inicia_linha(L1, qtd_campos_juncao);
            linha2 = inicia_linha(L2, qtd_campos_juncao);
            fim_de_arquivo1 = getline(&linha_lida1, &aux1, arq1);
            fim_de_arquivo2 = getline(&linha_lida2, &aux2, arq2);
            add_campos(&linha1, linha_lida1);
            add_campos(&linha2, linha_lida2);
        }
        //caso a Linha do primeiro arquivo seja "menor" do que a do segundo, destruímos a Linha do primeiro
        //e avançamos no arquivo 1, comparando-a com a mesma linha do segundo arquivo.
        else if (cmp < 0)
        {
            destroi_linha(linha1);
            linha1 = inicia_linha(L1, qtd_campos_juncao);
            fim_de_arquivo1 = getline(&linha_lida1, &aux1, arq1);
            add_campos(&linha1,linha_lida1);
        }
        //caso a Linha do segundo arquivo seja "menor" do que a do primeiro, destruímos a Linha do segundo
        //e avançamos no arquivo 2, comparando-a com a mesma linha do primeiro arquivo.
        else
        {          
            destroi_linha(linha2);
            linha2 = inicia_linha(L2, qtd_campos_juncao);
            fim_de_arquivo2 = getline(&linha_lida2, &aux1, arq2);
            add_campos(&linha2,linha_lida2);
        }
        //caso tenhamos um bloco de M linhas mescladas, devemos escrever no arquivo 
        //para liberar a memória principal
        if (tam_bloco == M)
        {
            imprime_vet_linhas_arquivo(vet_linhas, arquivo_saida, M);
            for (int i = 0; i < tam_bloco; i++)
            {
                destroi_linha(vet_linhas[i]);
            }
            tam_bloco = 0;
        }

    }
    //Se o resto da divisão de N/M for diferente de zero então ainda restam linhas 
    //a serem escritas no arquivo
    if (tam_bloco > 0)
    {
        imprime_vet_linhas_arquivo(vet_linhas, arquivo_saida, tam_bloco);
        for (int i = 0; i < tam_bloco; i++)
        {
            destroi_linha(vet_linhas[i]);
        }
    }
    fclose(arquivo_saida);
    free(linha_lida1);
    free(linha_lida2);
    free(vet_linhas);
    fclose(arq1);
    fclose(arq2);
    destroi_linha(linha1);
    destroi_linha(linha2);
}

// retorna uma linha com os campos de juncao e o restante das colunas de cada linha
Linha mescla_linha(Linha l1, Linha l2)
{
    Linha linha_mesclada;
    linha_mesclada = inicia_linha(l1.campos_juncao, l1.qtd_campos_juncao);
    int index;
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
//verifica se um indice corresponde ao campo de junção de uma Linha
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

