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

int atualiza_indice_arquivo_destino(int *indice_arquivo_destino_atual, int P, int inicio_arquivo_destino) {
    if(*indice_arquivo_destino_atual +1 < inicio_arquivo_destino + P) {
        return (*indice_arquivo_destino_atual + 1); // Incrementa o índice do arquivo de destino
    }else{
        return inicio_arquivo_destino; // Se chegou no final, volta para o início
    }
    // return (*indice_arquivo_destino_atual + 1) % P; // Incrementa o índice do arquivo de destino e o mantém dentro dos limites
}

//funcao para trocar arquivos fonte e destino
int trocar_arquivos(int *indice_arquivo_fonte, int *indice_arquivo_destino, int P) {
    int temp = *indice_arquivo_fonte;
    *indice_arquivo_fonte = *indice_arquivo_destino;
    *indice_arquivo_destino = temp;

    // printf("Arquivo de saida pos troca: %d\n", *indice_arquivo_destino);
    // //quando trocar tem q atualizar o indice do arquivo destino
    // return atualiza_indice_arquivo_destino(indice_arquivo_destino, P, *indice_arquivo_fonte);
    return *indice_arquivo_destino; //esta retornando o novo indice do arquivo de destino
}

// essa funcao deve pegar a primeira linha de cada arquivo temporário de origem e inicializar o vetor de linhas
void inicializar_vetor_proximas_linhas(FILE **arquivos_fontes_abertos_nesta_rodada, int P, Linha *vetor_proximas_linhas, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco, int qtd_iteracoes) {
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
        // ssize_t read = getline(&linha_lida, &len, current_file); 

        // le a linha M * qtd_iteracoes vezes (pra conseguir ler todos os blocos)
        // int inicio_bloco = 0;
        // while (inicio_bloco != ((tamanho_atual_bloco+1)*qtd_iteracoes) + 1){
        //     ssize_t read = getline(&linha_lida, &len, current_file);
        //     if (read == -1) {
        //         // Se não conseguiu ler mais linhas, sai do loop
        //         break;
        //     }
        //     inicio_bloco++;
        // }

        // if(qtd_iteracoes == 0) {
        //     // Se for a primeira iteração, le apenas uma linha
        //     ssize_t read = getline(&linha_lida, &len, current_file);
        //     if (read != -1) {
        //         // Remove o caractere de nova linha '\n' se presente
        //         if (read > 0 && linha_lida[read - 1] == '\n') {
        //             linha_lida[read - 1] = '\0';
        //         }
        //         // Adiciona os campos da linha lida
        //         add_campos(&(vetor_proximas_linhas[i]), linha_lida);
        //         linhas_lidas_bloco[i]++;
        //     }
        // }

        // if(inicio_bloco ==  ((tamanho_atual_bloco+1)*qtd_iteracoes) + 1) { // Se conseguiu ler M linhas
        //     // Adiciona os campos da linha lida
        //     add_campos(&(vetor_proximas_linhas[i]), linha_lida);
        //     linhas_lidas_bloco[i]++;
        // } else {
        //     destroi_linha(vetor_proximas_linhas[i]);
        //     vetor_proximas_linhas[i].colunas = NULL; 
        //     vetor_proximas_linhas[i].qtd_colunas = 0;
        // }

        // if (read != -1) { // Se leu uma linha com sucesso
        //     // Remove o caractere de nova linha '\n' se presente
        //     if (read > 0 && linha_lida[read - 1] == '\n') {
        //         linha_lida[read - 1] = '\0';
        //     }
        //     // Adiciona os campos da linha lida
        //     add_campos(&(vetor_proximas_linhas[i]), linha_lida);
        //     linhas_lidas_bloco[i]++;
        // } else { // Fim do arquivo ou erro na leitura
        //     // fprintf(stderr, "Aviso: Fim do arquivo ou erro ao ler do arquivo fonte [%d]. Tratando como fim de bloco.\n", i);
        //     destroi_linha(vetor_proximas_linhas[i]);
        //     vetor_proximas_linhas[i].colunas = NULL; 
        //     vetor_proximas_linhas[i].qtd_colunas = 0;
        // }

        // Loop para encontrar a primeira linha que não seja "FIMBLOCO"
        ssize_t read;
        int indice_linha = 0;
        while ((read = getline(&linha_lida, &len, current_file)) != -1) {
            // // Remove o '\n' se presente
            // if (read > 0 && linha_lida[read - 1] == '\n') {
            //     linha_lida[read - 1] = '\0';
            // }
            
            // Se a linha é "FIMBLOCO", ignora e tenta ler a próxima
            // printf("indice linha %d\n", indice_linha);
            // printf("qtd iteracoes %d\n", qtd_iteracoes);
            // printf("tamanho atual bloco %d\n", tamanho_atual_bloco);
            if ((strcmp(linha_lida, "FIMBLOCO") == 0) || indice_linha != (qtd_iteracoes * (tamanho_atual_bloco+1))) {
                // Não faça nada, apenas continue o loop para ler a próxima linha
                // continue; 
            } else {
                // Se não é FIMBLOCO e estamos no bloco certo, é a primeira linha válida do bloco
                add_campos(&(vetor_proximas_linhas[i]), linha_lida);
                linhas_lidas_bloco[i]++; // Uma linha lida para este novo bloco
                break; // Sai do loop, pois a primeira linha do bloco foi encontrada
            }

            // printf("Linha lida do arquivo %d: %s\n", i, linha_lida);
            indice_linha++;
        }

        if (read == -1) { // Se chegou ao fim do arquivo ou erro antes de encontrar uma linha válida
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
// void ler_proxima_linha(FILE *arquivo, Linha *linha_atual, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco){
//     //ver se o bloco ja foi todo lido
//     if(*linhas_lidas_bloco >= tamanho_atual_bloco) {
//         if(linha_atual->colunas != NULL) {
//             destroi_linha(*linha_atual); // Destrói a linha atual para evitar vazamento de memória
//         }
//         linha_atual->colunas = NULL; // Reseta p evitar problemas
//         linha_atual->qtd_colunas = 0; 
//         return; // esse bloco ja foi lido
//     }

//     //destruindo a linha atual antes de ler a proxima linha (p evitar vazamento de mem)
//     if(linha_atual->colunas != NULL) {
//         destroi_linha(*linha_atual);
//     }

//     // inicializa a linha atual
//     *linha_atual = inicia_linha(campos_juncao, qtd_campos_juncao);

//     char *linha_lida = NULL; // Buffer para getline (temporario)
//     size_t len = 0; // Tamanho alocado por getline

//     //lendo a proxima linha do arquivo
//     ssize_t read = getline(&linha_lida, &len, arquivo);

//     if(read != -1){ // se leu a linha com sucasso
//         // Remove o caractere de nova linha '\n' se presente
//         if (read > 0 && linha_lida[read - 1] == '\n') {
//             linha_lida[read - 1] = '\0';
//         }
//         // Adiciona os campos da linha lida
//         add_campos(linha_atual, linha_lida);
//         (*linhas_lidas_bloco)++;
//     } else { // Fim do arquivo ou erro na leitura
//         destroi_linha(*linha_atual);
//         linha_atual->colunas = NULL; 
//         linha_atual->qtd_colunas = 0;
//     }

//     //liberando o buffer de leitura
//     if (linha_lida != NULL) {
//         free(linha_lida);
//     }

// }

// void ler_proxima_linha(FILE *arquivo, Linha *linha_atual, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco){

// Função que lê a próxima linha de UM arquivo fonte específico
Linha ler_proxima_linha(FILE *arquivo_fonte, Linha *linha_anterior, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_do_bloco_ptr, int tamanho_do_bloco_esperado) {
    char *linha_lida = NULL;
    size_t len = 0;

    // Libera a linha anterior se ela foi alocada
    if (linha_anterior->colunas != NULL) {
        destroi_linha(*linha_anterior);
        linha_anterior->colunas = NULL;
    }
    
    // Tenta ler a próxima linha do arquivo
    // ssize_t read = getline(&linha_lida, &len, arquivo_fonte);

    // if (read != -1) {
    //     // Remove o '\n' se presente
    //     if (read > 0 && linha_lida[read - 1] == '\n') {
    //         linha_lida[read - 1] = '\0';
    //     }

    //     // Verifica se é o delimitador FIMBLOCO
    //     if (strcmp(linha_lida, "FIMBLOCO") == 0) {
    //         // Sinaliza o fim do bloco lógico.
    //         // A próxima chamada a inicializar_vetor_proximas_linhas para este arquivo
    //         // (ou a próxima iteração do merge que pegar este arquivo como fonte)
    //         // terá que avançar para o próximo bloco.
    //         Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
    //         nova_linha.colunas = NULL; // Sinaliza fim do bloco para o merge
    //         *linhas_lidas_do_bloco_ptr = tamanho_do_bloco_esperado; // "Força" o contador a indicar que o bloco terminou
    //         if (linha_lida != NULL) { free(linha_lida); }
    //         return nova_linha;
    //     } else {
    //         // É uma linha de dado válida
    //         Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
    //         add_campos(&nova_linha, linha_lida);
    //         (*linhas_lidas_do_bloco_ptr)++; // Incrementa o contador para o bloco atual
    //         if (linha_lida != NULL) { free(linha_lida); }
    //         return nova_linha;
    //     }
    // } else { // Fim do arquivo (EOF) ou erro na leitura
    //     // Sinaliza o fim do arquivo/blocos
    //     Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
    //     nova_linha.colunas = NULL; 
    //     if (linha_lida != NULL) { free(linha_lida); }
    //     return nova_linha;
    // }

    ssize_t read;
    // Loop para ler a próxima linha que não seja "FIMBLOCO"
    while ((read = getline(&linha_lida, &len, arquivo_fonte)) != -1) {
        if (read > 0 && linha_lida[read - 1] == '\n') {
            linha_lida[read - 1] = '\0';
        }

        if (strcmp(linha_lida, "FIMBLOCO") == 0) {
            // Se encontrou FIMBLOCO, o bloco lógico atual terminou para este arquivo.
            // Precisamos sinalizar isso claramente.
            Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
            nova_linha.colunas = NULL; // Marcar como NULL para indicar que não há mais linha para este bloco/arquivo
            // Não precisa setar *linhas_lidas_do_bloco_ptr = tamanho_do_bloco_esperado aqui.
            // A condição de parada da realizar_merge agora será simplesmente 'colunas == NULL'.
            if (linha_lida != NULL) { free(linha_lida); }
            return nova_linha;
        } else {
            // Se é uma linha de dado válida
            Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
            add_campos(&nova_linha, linha_lida);
            (*linhas_lidas_do_bloco_ptr)++; // Incrementa para o bloco atual
            if (linha_lida != NULL) { free(linha_lida); }
            return nova_linha;
        }
    } 
    
    // Se chegou aqui, significa que 'getline' retornou -1 (EOF ou erro).
    Linha nova_linha = inicia_linha(campos_juncao, qtd_campos_juncao);
    nova_linha.colunas = NULL; // Sinaliza fim do arquivo
    if (linha_lida != NULL) { free(linha_lida); }
    return nova_linha;
}


//colocar no linha.c? 
void imprime_linha_no_destino(Linha linha, FILE *arquivo_destino) {
    if (linha.colunas != NULL && linha.qtd_colunas > 0) {
        for (int i = 0; i < linha.qtd_colunas; i++) {
            fprintf(arquivo_destino, "%s", linha.colunas[i]);
            if (i < linha.qtd_colunas - 1) {
                fprintf(arquivo_destino, ","); // Adiciona espaço entre colunas
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
        printf("Escrevendo menor linha no arquivo de destino: ");
        for (int i = 0; i < vetor_proximas_linhas[indice_menor].qtd_colunas; i++) {
            printf("%s ", vetor_proximas_linhas[indice_menor].colunas[i]);
        }
        printf("\n");

        // Lê a próxima linha do arquivo correspondente ao menor elemento
        printf("Lendo próxima linha do arquivo fonte %d...\n", indice_menor);
        ler_proxima_linha(arquivo_fonte[indice_menor], &vetor_proximas_linhas[indice_menor], campos_juncao, qtd_campos_juncao, &linhas_lidas_bloco[indice_menor], tamanho_atual_bloco);
        // vetor_proximas_linhas[indice_menor] = ler_proxima_linha(
        //     arquivo_fonte[indice_menor], 
        //     &vetor_proximas_linhas[indice_menor], // Passa o ponteiro para a linha atual para ser destruída
        //     campos_juncao, qtd_campos_juncao, 
        //     &linhas_lidas_bloco[indice_menor], 
        //     tamanho_atual_bloco
        // );
    }
    // fprintf(arquivo_destino, "FIMBLOCO\n"); // Descomente se for o caso
}

void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in, char **L, int tam) {
    int tamanho_bloco_ordenado_atual = M; // os primeiros arquivos ordenados começam com tamanho M (mudam a cada iteração)
    int inicio_arquivo_fonte = P; //inicialmente o arquivo medio contem o inicio da primeira ordenação (varia inicialmente de P ate 2P-1)
    int inicio_arquivo_destino = 0; // Inicia as escritas na primeira metade dos arquivos temporários (varia inicialmente de 0 a P-1)
    char nome[260]; // Buffer para o nome do arquivo temporário

    //abrindo o arquivo original de entrada para contar o número de linhas
    FILE *arquivo_original_entrada = fopen(arquivo_in, "r"); // Abre o arquivo aqui (equivalente ao arquivo 1 ou arquivo 2)
    if (arquivo_original_entrada == NULL) {
        perror("Erro ao abrir arquivo de entrada para contar linhas");
        exit(EXIT_FAILURE);
    }
    int n_linhas_arquivo_entrada = numero_linhas_arquivo(arquivo_original_entrada);
    fclose(arquivo_original_entrada); // Fecha o arquivo após usar
    // printf("Número total de linhas no arquivo de entrada: %d\n", n_linhas_arquivo_entrada);

    int aux = 0;

    while (tamanho_bloco_ordenado_atual < n_linhas_arquivo_entrada) // Enquanto todos os dados não estiverem em um único bloco ordenado
    {
        int indice_arquivo_destino_atual = inicio_arquivo_destino; // Inicia a distribuição no primeiro arquivo de destino (sera alterado pra cada bloco de merges feitos)
        int num_blocos_ord_totais = (n_linhas_arquivo_entrada + tamanho_bloco_ordenado_atual - 1) / tamanho_bloco_ordenado_atual; // arredonda pra cima
         
        // Itera sobre os grupos de P blocos ordenados para realizar o merge (faz merge dos primeiros/segundos/terceiros... grupos em cada arquivo)
        //o i varia de zero ate o indice do ultimo grupo do arquivo com mais grupos, o resultado deveser o num max de elementos em um arquivo
        for (int i=0; i<ceil(num_blocos_ord_totais / (double)P); i++){
            // printf("Processando grupo %d de blocos ordenados...\n", i); //ok, perfeito

            // printf("iteracao coluna %d\n", i);

            FILE *arquivos_fontes_para_esta_rodada[P];
            FILE *arquivo_destino_para_esta_rodada; // Variável local para o arquivo de destino

            // abre os arquivos temporários de origem para leitura
            for (int j = 0; j < P; j++) {
                int indice_arquivo_real_fonte = inicio_arquivo_fonte + j;

                if(indice_arquivo_real_fonte >= inicio_arquivo_fonte + P){
                    indice_arquivo_real_fonte = inicio_arquivo_fonte;
                }
                
                // printf("Abrindo arquivo temporário de origem %d para leitura...\n", indice_arquivo_real_fonte);

                sprintf(nome, "%d.txt", indice_arquivo_real_fonte);
                arquivos_fontes_para_esta_rodada[j] = fopen(nome, "r"); // abre o arq p leitura e associa ao vetor que vai ser usado pra manipular o vetor de proximas linhas
                // if (arquivos_fontes_para_esta_rodada[j] == NULL) {
                //     // fprintf(stderr, "Aviso: Nao foi possivel abrir %s. Tratando como fim de bloco de fonte.\n", nome);
                // }
            }

            // abre o arquivo temporário de destino para escrita
            sprintf(nome, "%d.txt", indice_arquivo_destino_atual);
            arquivo_destino_para_esta_rodada = fopen(nome, "a"); //ATUALIZACAO (p nao apagar o conteudo anterior)
            // printf("Abrindo arquivo temporário de destino %s para escrita...\n", nome);
            if (arquivo_destino_para_esta_rodada == NULL) {
                perror("Erro ao abrir arquivo de destino para escrita");
                exit(EXIT_FAILURE);
            }

            //##############################################################
            //usado pra armazenar quantas lidas daquele bloco ja foram lidas (pra ele saber quando parar)
            int linhas_lidas_do_bloco[P];
                        
            // Inicializa o vetor de linhas com as primeiras linhas de cada arquivo fonte
            Linha vetor_proximas_linhas[P];
            inicializar_vetor_proximas_linhas(arquivos_fontes_para_esta_rodada, P, vetor_proximas_linhas, L, tam, linhas_lidas_do_bloco, tamanho_bloco_ordenado_atual, i);

            //printando vetor proximas linhas
            printf("Vetor de próximas linhas inicializado:\n");
            for (int k = 0; k < P; k++) {
                if (vetor_proximas_linhas[k].colunas != NULL) {
                    printf("Linha %d: ", k);
                    for (int j = 0; j < vetor_proximas_linhas[k].qtd_colunas; j++) {
                        printf("%s ", vetor_proximas_linhas[k].colunas[j]);
                    }
                    printf("\n");
                } else {
                    printf("Linha %d: NULL\n", k);
                }
            }
            
            // realizar_merge(vetor_proximas_linhas, P, arquivos_fontes_para_esta_rodada, arquivo_destino_para_esta_rodada, L, tam, tamanho_bloco_ordenado_atual, linhas_lidas_do_bloco);
            // printf("Merge realizado e escrito no arquivo %s.\n", nome);

            // libera o vetor de linhas
            for (int k = 0; k < P; k++) {
                // Só destrua se a linha realmente continha dados alocados (ou seja, colunas não é NULL)
                if (vetor_proximas_linhas[k].colunas != NULL) {
                    destroi_linha(vetor_proximas_linhas[k]);
                }
            }
            //##############################################################

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
            indice_arquivo_destino_atual = atualiza_indice_arquivo_destino(&indice_arquivo_destino_atual, P, inicio_arquivo_destino);
            // printf("Próximo índice de arquivo de destino: %d\n", indice_arquivo_destino_atual);

        }

        // if(aux == 0){
        //     break;
        // }

        indice_arquivo_destino_atual = trocar_arquivos(&inicio_arquivo_fonte, &inicio_arquivo_destino, P); //alterna arquivos fonte e destino
        // printf("Trocando arquivos fonte e destino. Novo início de arquivo fonte: %d, novo início de arquivo destino: %d\n", inicio_arquivo_fonte, inicio_arquivo_destino);
        // printf("houve troca. Proximo indice de arquivo de destino: %d\n", indice_arquivo_destino_atual);

        //ok
        tamanho_bloco_ordenado_atual *= P; // Aumenta o tamanho do bloco ordenado atual para o próximo loop
        // printf("Tamanho do bloco ordenado atual aumentado para: %d\n", tamanho_bloco_ordenado_atual);
        if(tamanho_bloco_ordenado_atual >= n_linhas_arquivo_entrada) {
            tamanho_bloco_ordenado_atual = n_linhas_arquivo_entrada; // Garante que não ultrapasse o número total de linhas
        }

        // printf("Limpando arquivos de destino de %d a %d\n", inicio_arquivo_destino, inicio_arquivo_destino + P - 1);
        limpar_arquivos_destino(temp_files, inicio_arquivo_destino, P); // limpa os arquivos que vamos escrever
    }
    
    // printf("Quantidade de linhas no arquivo de entrada: %d\n", n_linhas_arquivo_entrada);
    // aqui ainda fazer a logica pra escrever o conteudo no arquivo saida desejado
}