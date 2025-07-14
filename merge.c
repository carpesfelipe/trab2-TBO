#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "merge.h"
#include<math.h>
#include "linha.h"
#include <sys/types.h>

void limpar_arquivos_destino(FILE **temp_files, int indice_arquivo_destino, int P) {
    for (int i = indice_arquivo_destino; i < indice_arquivo_destino+P; i++) {
        char nome[260];
        sprintf(nome, "%d.txt", i);
        FILE *file = fopen(nome, "w");
        if (file == NULL) {
            perror("Erro ao limpar arquivo de destino");
            exit(EXIT_FAILURE);
        }
        fclose(file);
    }
}

int numero_linhas_arquivo(FILE *arquivo) { 
    int linhas = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        linhas++;
    }
    rewind(arquivo); // Volta ao início do arquivo
    return linhas;
}

//funcao para trocar arquivos fonte e destino
void trocar_arquivos(int *indice_arquivo_fonte, int *indice_arquivo_destino) {
    int temp = *indice_arquivo_fonte;
    *indice_arquivo_fonte = *indice_arquivo_destino;
    *indice_arquivo_destino = temp;
}

int atualiza_indice_arquivo_destino(int *indice_arquivo_destino_atual, int P) {
    return (*indice_arquivo_destino_atual + 1) % P; // Incrementa o índice do arquivo de destino e o mantém dentro dos limites
}

//essa funcao deve pegar a primeira linha de cada arquivo temporário de origem e inicializar o vetor de linhas
void inicializar_vetor_proximas_linhas(FILE **arquivos_fontes_abertos_nesta_rodada, int P, Linha *vetor_proximas_linhas, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco) {
    char *linha_lida = NULL; // Buffer para getline
    size_t len = 0; // Tamanho alocado por getline

    for (int i = 0; i < P; i++) {
        // arquivos já foam abertos em ordenar_blocos

        //zera o contador de linhas lidas para esse novo bloco
        linhas_lidas_bloco[i] = 0;

        // O vetor arquivos_fontes_abertos_nesta_rodada so diz respeito aos arquivos de leitura abertos nesta rodada (por isso podemos variar o i de 0 ate P sempre sem problemas)
        FILE *current_file = arquivos_fontes_abertos_nesta_rodada[i];

        // Inicializa a linha no vetor de linhas
        vetor_proximas_linhas[i] = inicia_linha(campos_juncao, qtd_campos_juncao);

        // Verifica se o arquivo foi aberto com sucesso por `ordenar_blocos` (so p evitar problemas)
        if (current_file == NULL) {
            // fprintf(stderr, "Aviso: Arquivo fonte temporario [%d] é NULL. Tratando como fim de bloco de fonte.\n", i);
            destroi_linha(vetor_proximas_linhas[i]);
            vetor_proximas_linhas[i].colunas = NULL; 
            vetor_proximas_linhas[i].qtd_colunas = 0;
            continue; // Pula para o próximo arquivo
        }
        
        // Lê a primeira linha do arquivo atual
        ssize_t read = getline(&linha_lida, &len, current_file); 

        if (read != -1) { // Se leu uma linha com sucesso
            // Remove o caractere de nova linha '\n' se presente
            if (read > 0 && linha_lida[read - 1] == '\n') {
                linha_lida[read - 1] = '\0';
            }
            // Adiciona os campos da linha lida
            add_campos(&(vetor_proximas_linhas[i]), linha_lida);
            linhas_lidas_bloco[i]++;
        } else { // Fim do arquivo ou erro na leitura
            // fprintf(stderr, "Aviso: Fim do arquivo ou erro ao ler do arquivo fonte [%d]. Tratando como fim de bloco.\n", i);
            destroi_linha(vetor_proximas_linhas[i]);
            vetor_proximas_linhas[i].colunas = NULL; 
            vetor_proximas_linhas[i].qtd_colunas = 0;
        }
    }

    if (linha_lida != NULL) {
        free(linha_lida);
        linha_lida = NULL;
    }
}

// essa funcao deve ler a proxima linha de um file e atualizar a linha correspondente a este file no vetor_proximas_linhas
void ler_proxima_linha(FILE *arquivo, Linha *linha_atual, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco){
    //ver se o bloco ja foi todo lido
    if(*linhas_lidas_bloco >= tamanho_atual_bloco) {
        if(linha_atual->colunas != NULL) {
            destroi_linha(*linha_atual); // Destrói a linha atual para evitar vazamento de memória
        }
        linha_atual->colunas = NULL; // Reseta p evitar problemas
        linha_atual->qtd_colunas = 0; 
        return; // esse bloco ja foi lido
    }

    //destruindo a linha atual antes de ler a proxima linha (p evitar vazamento de mem)
    if(linha_atual->colunas != NULL) {
        destroi_linha(*linha_atual);
    }

    // inicializa a linha atual
    *linha_atual = inicia_linha(campos_juncao, qtd_campos_juncao);

    char *linha_lida = NULL; // Buffer para getline (temporario)
    size_t len = 0; // Tamanho alocado por getline

    //lendo a proxima linha do arquivo
    ssize_t read = getline(&linha_lida, &len, arquivo);

    if(read != -1){ // se leu a linha com sucasso
        // Remove o caractere de nova linha '\n' se presente
        if (read > 0 && linha_lida[read - 1] == '\n') {
            linha_lida[read - 1] = '\0';
        }
        // Adiciona os campos da linha lida
        add_campos(linha_atual, linha_lida);
        (*linhas_lidas_bloco)++;
    } else { // Fim do arquivo ou erro na leitura
        destroi_linha(*linha_atual);
        linha_atual->colunas = NULL; 
        linha_atual->qtd_colunas = 0;
    }

    //liberando o buffer de leitura
    if (linha_lida != NULL) {
        free(linha_lida);
    }

}

//colocar no linha.c? 
void imprime_linha_no_destino(Linha linha, FILE *arquivo_destino) {
    if (linha.colunas != NULL && linha.qtd_colunas > 0) {
        for (int i = 0; i < linha.qtd_colunas; i++) {
            fprintf(arquivo_destino, "%s", linha.colunas[i]);
            if (i < linha.qtd_colunas - 1) {
                fprintf(arquivo_destino, " "); // Adiciona espaço entre colunas
            }
        }
        fprintf(arquivo_destino, "\n");
    }
}

void realizar_merge(Linha *vetor_proximas_linhas, int P, FILE **arquivo_fonte, FILE *arquivo_destino, char **campos_juncao, int qtd_campos_juncao, int tamanho_atual_bloco, int *linhas_lidas_bloco) {
    // o loop deve acontecer enquando houver linhas valias no vetor
    while (1) {
        // Variável para armazenar o índice do menor elemento (o -1 indica que ainda não foi encontrado nenhum elemento)
        int indice_menor = -1;
        
        // Encontra o índice do menor elemento 
        for (int i = 0; i < P; i++) {
            if (vetor_proximas_linhas[i].colunas != NULL) {
                // if (indice_menor == -1 || strcmp(vetor_proximas_linhas[i].colunas[0], vetor_proximas_linhas[indice_menor].colunas[0]) < 0) {
                if (indice_menor == -1 || compara_linhas(&vetor_proximas_linhas[i], &vetor_proximas_linhas[indice_menor]) < 0) {
                    indice_menor = i; // Atualiza o índice do menor elemento encontrado até agora
                }
            }
        }

        // Se não encontrar um menor (ou seja, nao tem mais linhas validas), sai do loop
        if (indice_menor == -1) {
            break;
        }

        // Escreve a linha com o menor valor no arquivo de destino
        imprime_linha_no_destino(vetor_proximas_linhas[indice_menor], arquivo_destino);

        // Lê a próxima linha do arquivo correspondente ao menor elemento
        ler_proxima_linha(arquivo_fonte[indice_menor], &vetor_proximas_linhas[indice_menor], campos_juncao, qtd_campos_juncao, &linhas_lidas_bloco[indice_menor], tamanho_atual_bloco);
    }
}

void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in, char **L, int tam) {
    int tamanho_bloco_ordenado_atual = M; // os primeiros arquivos ordenados começam com tamanho M (mudam a cada iteração)
    int inicio_arquivo_fonte = P; //inicialmente o arquivo medio contem o inicio da primeira ordenação (varia inicialmente de P ate 2P-1)
    int inicio_arquivo_destino = 0; // Inicia as escritas na primeira metade dos arquivos temporários (varia inicialmente de 0 a P-1)
    char nome[260]; // Buffer para o nome do arquivo temporário

    // int auxiliar_enquanto_constroi = 0; //vou apagar esse auxiliar depois, é só pra evitar q o loop rode mais do q a qtd que eu quero

    //abrindo o arquivo original de entrada para contar o número de linhas
    FILE *arquivo_original_entrada = fopen(arquivo_in, "r"); // Abre o arquivo aqui (equivalente ao arquivo 1 ou arquivo 2)
    if (arquivo_original_entrada == NULL) {
        perror("Erro ao abrir arquivo de entrada para contar linhas");
        exit(EXIT_FAILURE);
    }
    int n_linhas_arquivo_entrada = numero_linhas_arquivo(arquivo_original_entrada);
    fclose(arquivo_original_entrada); // Fecha o arquivo após usar
    // printf("Número total de linhas no arquivo de entrada: %d\n", n_linhas_arquivo_entrada);

    while (tamanho_bloco_ordenado_atual < n_linhas_arquivo_entrada) // Enquanto todos os dados não estiverem em um único bloco ordenado
    {
        // printf("Limpando arquivos de destino de %d a %d\n", inicio_arquivo_destino, inicio_arquivo_destino + P - 1);
        limpar_arquivos_destino(temp_files, inicio_arquivo_destino, P); // limpa os arquivos que vamos escrever
        // printf("Limpeza concluída.\n");

        int indice_arquivo_destino_atual = inicio_arquivo_destino; // Inicia a distribuição no primeiro arquivo de destino (sera alterado pra cada bloco de merges feitos)
        int num_blocos_ord_totais = (n_linhas_arquivo_entrada + tamanho_bloco_ordenado_atual - 1) / tamanho_bloco_ordenado_atual; // arredonda pra cima
        // printf("Número total de blocos ordenados nesta passagem: %d\n", num_blocos_ord_totais);

        // Itera sobre os grupos de P blocos ordenados para realizar o merge (faz merge dos primeiros/segundos/terceiros... grupos em cada arquivo)
        //o i varia de zero ate o indice do ultimo grupo do arquivo com mais grupos, o resultado deveser o num max de elementos em um arquivo
        for (int i=0; i<ceil(num_blocos_ord_totais / (double)P); i++){
            // printf("Processando grupo %d de blocos ordenados...\n", i);

            FILE *arquivos_fontes_para_esta_rodada[P];
            FILE *arquivo_destino_para_esta_rodada; // Variável local para o arquivo de destino

            // abre os arquivos temporários de origem para leitura
            for (int j = 0; j < P; j++) {
                int indice_arquivo_real_fonte = inicio_arquivo_fonte + (i * P) + j;
                
                sprintf(nome, "%d.txt", indice_arquivo_real_fonte);
                arquivos_fontes_para_esta_rodada[j] = fopen(nome, "r"); // abre o arq p leitura e associa ao vetor que vai ser usado pra manipular o vetor de proximas linhas
                // if (arquivos_fontes_para_esta_rodada[j] == NULL) {
                //     // fprintf(stderr, "Aviso: Nao foi possivel abrir %s. Tratando como fim de bloco de fonte.\n", nome);
                // }
            }

            // abre o arquivo temporário de destino para escrita
            sprintf(nome, "%d.txt", indice_arquivo_destino_atual);
            arquivo_destino_para_esta_rodada = fopen(nome, "w");
            if (arquivo_destino_para_esta_rodada == NULL) {
                perror("Erro ao abrir arquivo de destino para escrita");
                exit(EXIT_FAILURE);
            }

            //usado pra armazenar quantas lidas daquele bloco ja foram lidas (pra ele saber quando parar)
            int linhas_lidas_do_bloco[P];
                        
            // Inicializa o vetor de linhas com as primeiras linhas de cada arquivo fonte
            Linha vetor_proximas_linhas[P];
            inicializar_vetor_proximas_linhas(arquivos_fontes_para_esta_rodada, P, vetor_proximas_linhas, L, tam, linhas_lidas_do_bloco, tamanho_bloco_ordenado_atual);

            //printando vetor proximas linhas
            // printf("Vetor de próximas linhas inicializado:\n");
            // for (int k = 0; k < P; k++) {
            //     if (vetor_proximas_linhas[k].colunas != NULL) {
            //         printf("Linha %d: ", k);
            //         for (int j = 0; j < vetor_proximas_linhas[k].qtd_colunas; j++) {
            //             printf("%s ", vetor_proximas_linhas[k].colunas[j]);
            //         }
            //         printf("\n");
            //     } else {
            //         printf("Linha %d: NULL\n", k);
            //     }
            // }
            
            realizar_merge(vetor_proximas_linhas, P, arquivos_fontes_para_esta_rodada, arquivo_destino_para_esta_rodada, L, tam, tamanho_bloco_ordenado_atual, linhas_lidas_do_bloco);

            // libera o vetor de linhas
            for (int k = 0; k < P; k++) {
                // Só destrua se a linha realmente continha dados alocados (ou seja, colunas não é NULL)
                if (vetor_proximas_linhas[k].colunas != NULL) {
                    destroi_linha(vetor_proximas_linhas[k]);
                }
            }
            
            //fechando os arquivos 
            for (int j = 0; j < P; j++) {
                if (arquivos_fontes_para_esta_rodada[j] != NULL) {
                    fclose(arquivos_fontes_para_esta_rodada[j]);
                    arquivos_fontes_para_esta_rodada[j] = NULL;
                }   
            }
            if (arquivo_destino_para_esta_rodada != NULL) {
                fclose(arquivo_destino_para_esta_rodada);
                arquivo_destino_para_esta_rodada = NULL;
            }

            //atualiza o índice do arquivo de destino (escrever em um proximo arquivo)
            indice_arquivo_destino_atual = atualiza_indice_arquivo_destino(&indice_arquivo_destino_atual, P);

        }

        // vou apagar esse auxiliar depois, ele n faz parte da logica
        // auxiliar_enquanto_constroi++;
        // if(auxiliar_enquanto_constroi == 1) {
        //     break;
        // }

        trocar_arquivos(&inicio_arquivo_fonte, &inicio_arquivo_destino); //alterna arquivos fonte e destino
        tamanho_bloco_ordenado_atual *= P; // Aumenta o tamanho do bloco ordenado atual para o próximo loop
    }

    //aqui passar o conteudo final pro arquivo que eu quero a saida
    
}