#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"
#include <math.h>
#include "linha.h"
#include <sys/types.h>

void limpar_arquivos_destino(FILE **temp_files, int indice_arquivo_destino, int P, char *arq)
{
    for (int i = indice_arquivo_destino; i < indice_arquivo_destino + P; i++)
    {
        char nome[260];
        sprintf(nome, "%s%d.txt", arq, i);
        FILE *file = fopen(nome, "w");
        if (file == NULL)
        {
            perror("Erro ao limpar arquivo de destino");
            exit(EXIT_FAILURE);
        }
        fclose(file);
    }
}

int numero_linhas_arquivo(FILE *arquivo)
{
    int linhas = 0;
    char *buffer = NULL;
    size_t n;
    while (getline(&buffer, &n, arquivo) != -1)
    {
        linhas++;
    }
    free(buffer);
    rewind(arquivo); // Volta ao início do arquivo
    return linhas;
}

int atualiza_indice_arquivo_destino(int *indice_arquivo_destino_atual, int P, int inicio_arquivo_destino)
{
    if (*indice_arquivo_destino_atual + 1 < inicio_arquivo_destino + P)
    {
        return (*indice_arquivo_destino_atual + 1); // Incrementa o índice do arquivo de destino
    }
    else
    {
        return inicio_arquivo_destino; // Se chegou no final, volta para o início
    }
}

// funcao para trocar arquivos fonte e destino
int trocar_arquivos(int *indice_arquivo_fonte, int *indice_arquivo_destino, int P)
{
    int temp = *indice_arquivo_fonte;
    *indice_arquivo_fonte = *indice_arquivo_destino;
    *indice_arquivo_destino = temp;

    return *indice_arquivo_destino; // esta retornando o novo indice do arquivo de destino
}

// essa funcao deve pegar a primeira linha de cada arquivo temporário de origem e inicializar o vetor de linhas
void inicializar_vetor_proximas_linhas(FILE **arquivos_fontes_abertos_nesta_rodada, int P, Linha *vetor_proximas_linhas, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco, int qtd_iteracoes)
{

    for (int i = 0; i < P; i++)
    {
        // arquivos já foam abertos em ordenar_blocos

        char *linha_lida = NULL; // Buffer para getline
        size_t len = 0;          // Tamanho alocado por getline

        // zera o contador de linhas lidas para esse novo bloco
        linhas_lidas_bloco[i] = 0;

        // O vetor arquivos_fontes_abertos_nesta_rodada so diz respeito aos arquivos de leitura abertos nesta rodada (por isso podemos variar o i de 0 ate P sempre sem problemas)
        FILE *current_file = arquivos_fontes_abertos_nesta_rodada[i];

        // Inicializa a linha no vetor de linhas
        vetor_proximas_linhas[i] = inicia_linha(campos_juncao, qtd_campos_juncao);

        // Verifica se o arquivo foi aberto com sucesso por `ordenar_blocos` (so p evitar problemas)
        if (current_file == NULL)
        {
            destroi_linha(vetor_proximas_linhas[i]);
            vetor_proximas_linhas[i].colunas = NULL;
            vetor_proximas_linhas[i].qtd_colunas = 0;
            continue; // Pula para o próximo arquivo
        }

        // Loop para encontrar a primeira linha que não seja "FIMBLOCO"
        ssize_t read;
        int indice_linha = 0;
        while ((read = getline(&linha_lida, &len, current_file)) != -1)
        {
            if ((strcmp(linha_lida, "FIMBLOCO") == 0) || indice_linha != (qtd_iteracoes * (tamanho_atual_bloco + 1)))
            {
                // continue;
            }
            else
            {
                // Se não é FIMBLOCO e estamos no bloco certo, é a primeira linha válida do bloco
                add_campos(&(vetor_proximas_linhas[i]), linha_lida);
                linhas_lidas_bloco[i]++;
                break; // Sai do loop, pois a primeira linha do bloco foi encontrada
            }

            indice_linha++;
        }

        if (read == -1)
        { // Se chegou ao fim do arquivo ou erro antes de encontrar uma linha válida
            destroi_linha(vetor_proximas_linhas[i]);
            vetor_proximas_linhas[i].colunas = NULL;
            vetor_proximas_linhas[i].qtd_colunas = 0;
        }

        if (linha_lida != NULL)
        {
            free(linha_lida);
            linha_lida = NULL;
        }
    }
}

// Função que lê a próxima linha de UM arquivo fonte específico
Linha ler_proxima_linha(FILE *arquivo_fonte, Linha *linha_anterior, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_do_bloco_ptr, int tamanho_do_bloco_esperado)
{
    char *linha_lida = NULL;
    size_t len = 0;

    // Libera a linha anterior se ela foi alocada
    if (linha_anterior->colunas != NULL)
    {
        destroi_linha(*linha_anterior);
        linha_anterior->colunas = NULL;
    }

    // Tenta ler a próxima linha do arquivo
    ssize_t read = getline(&linha_lida, &len, arquivo_fonte);

    Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);

    if (read != -1)
    {
        // Remove o '\n' se presente
        if (read > 0 && linha_lida[read - 1] == '\n')
        {
            linha_lida[read - 1] = '\0';
        }

        // Verifica se é o delimitador FIMBLOCO
        if (strcmp(linha_lida, "FIMBLOCO") == 0)
        {
            if (nova_linha.colunas != NULL)
            { // Garante que não está tentando liberar um ponteiro NULL
                free(nova_linha.colunas);
                nova_linha.colunas = NULL;
            }
            *linhas_lidas_do_bloco_ptr = tamanho_do_bloco_esperado;
        }
        else
        {
            add_campos(&nova_linha, linha_lida);
            (*linhas_lidas_do_bloco_ptr)++; // Incrementa o contador para o bloco atual
        }
    }
    else
    {

        if (nova_linha.colunas != NULL)
        {
            free(nova_linha.colunas);
            nova_linha.colunas = NULL;
        }
    }

    if (linha_lida != NULL)
    {
        free(linha_lida);
    }
    return nova_linha;
}

void imprime_linha_arquivo(Linha linha, FILE *arquivo_destino)
{
    if (linha.colunas != NULL && linha.qtd_colunas > 0)
    {
        for (int i = 0; i < linha.qtd_colunas; i++)
        {
            fprintf(arquivo_destino, "%s", linha.colunas[i]);
            if (i < linha.qtd_colunas - 1)
            {
                fprintf(arquivo_destino, ",");
            }
        }
        fprintf(arquivo_destino, "\n");
    }
}

void realizar_merge(Linha *vetor_proximas_linhas, int P, FILE **arquivo_fonte, FILE *arquivo_destino, char **campos_juncao, int qtd_campos_juncao, int tamanho_atual_bloco, int *linhas_lidas_bloco, int n_linhas_arquivo_entrada)
{

    // o loop deve acontecer enquando houver linhas valias no vetor
    while (1)
    {
        // Variável para armazenar o índice do menor elemento (o -1 indica que ainda não foi encontrado nenhum elemento)
        int indice_menor = -1;

        // Encontra o índice do menor elemento
        for (int i = 0; i < P; i++)
        {
            if (vetor_proximas_linhas[i].colunas != NULL)
            {
                if (indice_menor == -1 || compara_linhas(&vetor_proximas_linhas[i], &vetor_proximas_linhas[indice_menor]) < 0)
                {
                    indice_menor = i; // Atualiza o índice do menor elemento encontrado até agora
                }
            }
        }

        // Se não encontrar um menor (ou seja, nao tem mais linhas validas), sai do loop
        if (indice_menor == -1)
        {
            if (tamanho_atual_bloco + P < n_linhas_arquivo_entrada)
            {
                fprintf(arquivo_destino, "FIMBLOCO\n"); // Marca o fim do bloco
            }
            break;
        }

        // Escreve a linha com o menor valor no arquivo de destino
        imprime_linha_arquivo(vetor_proximas_linhas[indice_menor], arquivo_destino);

        // Lê a próxima linha do arquivo correspondente ao menor elemento
        vetor_proximas_linhas[indice_menor] = ler_proxima_linha(arquivo_fonte[indice_menor], &vetor_proximas_linhas[indice_menor], campos_juncao, qtd_campos_juncao, &linhas_lidas_bloco[indice_menor], tamanho_atual_bloco);
    }
}

void passa_conteudo_final_para_arq0(char *arq, int indice_ultima_escrita)
{
    char nome_arquivo[260];
    sprintf(nome_arquivo, "%s0.txt", arq); // Abre o arquivo
    FILE *arquivo_final = fopen(nome_arquivo, "w");
    if (arquivo_final == NULL)
    {
        perror("Erro ao abrir arquivo final para escrita");
        exit(EXIT_FAILURE);
    }

    // abrir o arquivo de destino final de indice indice_ultima_escrita e escrever no arquivo_final
    char nome_arquivo_destino[260];
    sprintf(nome_arquivo_destino, "%s%d.txt", arq, indice_ultima_escrita);
    FILE *arquivo_destino_final = fopen(nome_arquivo_destino, "r");
    if (arquivo_destino_final == NULL)
    {
        perror("Erro ao abrir arquivo temporário de destino para leitura");
        fclose(arquivo_final);
        exit(EXIT_FAILURE);
    }

    char *linha_lida = NULL;
    size_t len = 0;

    while (getline(&linha_lida, &len, arquivo_destino_final) != -1)
    {
        fprintf(arquivo_final, "%s", linha_lida);
    }
    // Copia o conteúdo do arquivo temporário de destino para o arquivo final
    fclose(arquivo_destino_final);
    fclose(arquivo_final);
    if (linha_lida != NULL)
    {
        free(linha_lida);
        linha_lida = NULL;
    }
}
void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in, char **L, int tam, char *arq)
{
    int tamanho_bloco_ordenado_atual = M; // os primeiros arquivos ordenados começam com tamanho M (mudam a cada iteração)
    int inicio_arquivo_fonte = P;         // inicialmente o arquivo medio contem o inicio da primeira ordenação (varia inicialmente de P ate 2P-1)
    int inicio_arquivo_destino = 0;       // Inicia as escritas na primeira metade dos arquivos temporários (varia inicialmente de 0 a P-1)
    char nome[260];                       // Buffer para o nome do arquivo temporário

    // abrindo o arquivo original de entrada para contar o número de linhas
    FILE *arquivo_original_entrada = fopen(arquivo_in, "r"); // Abre o arquivo aqui (equivalente ao arquivo 1 ou arquivo 2)
    if (arquivo_original_entrada == NULL)
    {
        perror("Erro ao abrir arquivo de entrada para contar linhas");
        exit(EXIT_FAILURE);
    }
    int n_linhas_arquivo_entrada = numero_linhas_arquivo(arquivo_original_entrada);
    fclose(arquivo_original_entrada); // Fecha o arquivo após usar

    int indice_ultima_escrita = -1;

    while (tamanho_bloco_ordenado_atual < n_linhas_arquivo_entrada) // Enquanto todos os dados não estiverem em um único bloco ordenado
    {
        int indice_arquivo_destino_atual = inicio_arquivo_destino;                                                                // Inicia a distribuição no primeiro arquivo de destino (sera alterado pra cada bloco de merges feitos)
        int num_blocos_ord_totais = (n_linhas_arquivo_entrada + tamanho_bloco_ordenado_atual - 1) / tamanho_bloco_ordenado_atual; // arredonda pra cima

        // Itera sobre os grupos de P blocos ordenados para realizar o merge (faz merge dos primeiros/segundos/terceiros... grupos em cada arquivo)
        // o i varia de zero ate o indice do ultimo grupo do arquivo com mais grupos, o resultado deveser o num max de elementos em um arquivo
        for (int i = 0; i < ceil(num_blocos_ord_totais / (double)P); i++)
        {

            FILE *arquivos_fontes_para_esta_rodada[P];
            FILE *arquivo_destino_para_esta_rodada; // Variável local para o arquivo de destino

            // abre os arquivos temporários de origem para leitura
            for (int j = 0; j < P; j++)
            {
                int indice_arquivo_real_fonte = inicio_arquivo_fonte + j;

                if (indice_arquivo_real_fonte >= inicio_arquivo_fonte + P)
                {
                    indice_arquivo_real_fonte = inicio_arquivo_fonte;
                }

                sprintf(nome, "%s%d.txt", arq, indice_arquivo_real_fonte);
                arquivos_fontes_para_esta_rodada[j] = fopen(nome, "r"); // abre o arq p leitura e associa ao vetor que vai ser usado pra manipular o vetor de proximas linhas
            }

            // abre o arquivo temporário de destino para escrita
            sprintf(nome, "%s%d.txt", arq, indice_arquivo_destino_atual);
            arquivo_destino_para_esta_rodada = fopen(nome, "a"); // ATUALIZACAO (p nao apagar o conteudo anterior)
            if (arquivo_destino_para_esta_rodada == NULL)
            {
                perror("Erro ao abrir arquivo de destino para escrita");
                exit(EXIT_FAILURE);
            }

            // usado pra armazenar quantas lidas daquele bloco ja foram lidas (pra ele saber quando parar)
            int linhas_lidas_do_bloco[P];

            // Inicializa o vetor de linhas com as primeiras linhas de cada arquivo fonte
            Linha vetor_proximas_linhas[P];
            inicializar_vetor_proximas_linhas(arquivos_fontes_para_esta_rodada, P, vetor_proximas_linhas, L, tam, linhas_lidas_do_bloco, tamanho_bloco_ordenado_atual, i);

            realizar_merge(vetor_proximas_linhas, P, arquivos_fontes_para_esta_rodada, arquivo_destino_para_esta_rodada, L, tam, tamanho_bloco_ordenado_atual, linhas_lidas_do_bloco, n_linhas_arquivo_entrada);

            // libera o vetor de linhas
            for (int k = 0; k < P; k++)
            {
                // Só destrua se a linha realmente continha dados alocados (ou seja, colunas não é NULL)
                if (vetor_proximas_linhas[k].colunas != NULL)
                {
                    destroi_linha(vetor_proximas_linhas[k]);
                }
            }
            // fechando os arquivos
            for (int j = 0; j < P; j++)
            {
                if (arquivos_fontes_para_esta_rodada[j] != NULL)
                {
                    fclose(arquivos_fontes_para_esta_rodada[j]);
                    arquivos_fontes_para_esta_rodada[j] = NULL;
                }
            }
            if (arquivo_destino_para_esta_rodada != NULL)
            {
                fclose(arquivo_destino_para_esta_rodada);
                arquivo_destino_para_esta_rodada = NULL;
            }

            indice_ultima_escrita = indice_arquivo_destino_atual;
            // atualiza o índice do arquivo de destino (escrever em um proximo arquivo)
            indice_arquivo_destino_atual = atualiza_indice_arquivo_destino(&indice_arquivo_destino_atual, P, inicio_arquivo_destino);
        }

        indice_arquivo_destino_atual = trocar_arquivos(&inicio_arquivo_fonte, &inicio_arquivo_destino, P); // alterna arquivos fonte e destino

        tamanho_bloco_ordenado_atual *= P; // Aumenta o tamanho do bloco ordenado atual para o próximo loop
        if (tamanho_bloco_ordenado_atual >= n_linhas_arquivo_entrada)
        {
            tamanho_bloco_ordenado_atual = n_linhas_arquivo_entrada; // Garante que não ultrapasse o número total de linhas
        }

        limpar_arquivos_destino(temp_files, inicio_arquivo_destino, P, arq); // limpa os arquivos que vamos escrever
    }
    // passar conteudo do arquivo temporario de destino para o arquivo de saida final(que deve ser sempre o zero)
    if (indice_ultima_escrita > 0)
    {
        passa_conteudo_final_para_arq0(arq, indice_ultima_escrita);
    }
    else if (indice_ultima_escrita == -1)
    { // caso n precisou de merge, ele n vai saber o indice da ultima escrita
        passa_conteudo_final_para_arq0(arq, inicio_arquivo_fonte);
    }
}