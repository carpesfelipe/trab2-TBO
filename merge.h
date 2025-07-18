#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "linha.h"

//essa funcao realiza efetivamente o merge
void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in, char **L, int tam, char *arq);
Linha ler_proxima_linha(FILE *arquivo_fonte, Linha *linha_anterior, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_do_bloco_ptr, int tamanho_do_bloco_esperado);
void inicializar_vetor_proximas_linhas(FILE **arquivos_fontes_abertos_nesta_rodada, int P, Linha *vetor_proximas_linhas, char **campos_juncao, int qtd_campos_juncao, int *linhas_lidas_bloco, int tamanho_atual_bloco, int qtd_iteracoes);
int trocar_arquivos(int *indice_arquivo_fonte, int *indice_arquivo_destino, int P);
int atualiza_indice_arquivo_destino(int *indice_arquivo_destino_atual, int P, int inicio_arquivo_destino);
int numero_linhas_arquivo(FILE *arquivo);
void limpar_arquivos_destino(FILE **temp_files, int indice_arquivo_destino, int P, char *arq);
void passa_conteudo_final_para_arq0(char *arq, int indice_ultima_escrita);