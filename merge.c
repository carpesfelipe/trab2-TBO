#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "merge.h"
#include<math.h>

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

int atualiza_indice_arquivo_destino(int *indice_arquivo_destino, int P) {
    return (*indice_arquivo_destino + 1) % P; // Incrementa o índice do arquivo de destino e o mantém dentro dos limites
}

void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in) {

    int tamanho_bloco_ordenado_atual = M; // os primeiros arquivos ordenados começam com tamanho M (mudam a cada iteração)
    int inicio_arquivo_fonte = P; //inicialmente o arquivo medio contem o inicio da primeira ordenação (varia sempre de P ate 2P-1)
    int inicio_arquivo_destino = 0; // Inicia as escritas na primeira metade dos arquivos temporários (varia sempre de 0 a P-1)
    
    int auxiliar_enquanto_constroi = 0;

    // //tamanho do arquivo de entrada 
    // int n_linhas_arquivo = numero_linhas_arquivo(fopen(arquivo_in, "r"));

    FILE *arquivo_original_entrada = fopen(arquivo_in, "r"); // Abre o arquivo aqui
    if (arquivo_original_entrada == NULL) {
        perror("Erro ao abrir arquivo de entrada para contar linhas");
        exit(EXIT_FAILURE);
    }

    int n_linhas_arquivo_entrada = numero_linhas_arquivo(arquivo_original_entrada); // <<< Passa o FILE* aberto
    fclose(arquivo_original_entrada); // Fecha o arquivo após usar

    while (tamanho_bloco_ordenado_atual < n_linhas_arquivo_entrada) // Enquanto todos os dados não estiverem em um único bloco ordenado
    {
        limpar_arquivos_destino(temp_files, inicio_arquivo_destino, P); // limpa os arquivos que vamos escrever
        int indice_arquivo_destino_atual = 0; // Inicia a distribuição no primeiro arquivo de destino (sera alterado pra cada bloco de merges feitos)

        int num_blocos_ord_totais = (n_linhas_arquivo_entrada + tamanho_bloco_ordenado_atual - 1) / tamanho_bloco_ordenado_atual; // arredonda pra cima
        // printf("Número total de blocos ordenados nesta passagem: %d\n", num_blocos_ord_totsais);

        // Itera sobre os grupos de P blocos ordenados para realizar o merge (faz merge dos primeiros/segundos/terceiros... grupos em cada arquivo)
        //o i varia de zero ate o indice do ultimo grupo do arquivo com mais grupos, o resultado deveser o num max de elementos em um arquivo
        for (int i=0; i<ceil(num_blocos_ord_totais / (double)P); i++){

            //abre os arquivos temporários de origem para leitura
            // for(int j=inicio_arquivo_fonte; j<inicio_arquivo_fonte+P-1; j++) {
            //     temp_files[j] = fopen(temp_files[j], "r");
            //     if (temp_files[j] == NULL) {
            //         perror("Erro ao abrir arquivo temporário de origem");
            //         exit(EXIT_FAILURE);
            //     }
            // }

            // Se Contador_Blocos_Ordenados_Abertos == 0:
            //     se nao tiver mais blocos pra ordenar, quebra esse loop (ajeitar)
            
            // abre arquivo de escrita para escrever
            
            // Vetor_Proximas_Linhas = Inicializar_Vetor_Proximas_Linhas() (vetor com os menores elementos)
            
            //Realizar_Merge_De_P_Vias()
            
            //fecha os canais de leitura e escrita
            //fecha os arquivos temporários de origem
            // for(int j=inicio_arquivo_fonte; j<inicio_arquivo_fonte+P-1; j++) {
            //     fclose(temp_files[j]);    
            // }

            //atualiza o índice do arquivo de destino (escrever em um proximo arquivo)
            indice_arquivo_destino_atual = atualiza_indice_arquivo_destino(&inicio_arquivo_destino, P);

        }

        auxiliar_enquanto_constroi++;

        if(auxiliar_enquanto_constroi == 3) {
            break;
        }

        //alterna arquivos fonte e destino
        trocar_arquivos(&inicio_arquivo_fonte, &inicio_arquivo_destino);
        tamanho_bloco_ordenado_atual *= P; // Aumenta o tamanho do bloco ordenado atual para o próximo loop
    }
    
}