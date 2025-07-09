#ifndef ORD_EXTERNA_H
#define ORD_EXTERNA_H

#include <stdio.h>
#include <string.h>

char **obtem_campos_juncao(char *linha, int *tamanho);
void destroi_campos_juncao(char **l, int tam);
void criando_arquivos(FILE **temp_files, int P);
int compara_linhas(const void *a, const void *b);
int preenche_arquivos_temporarios(FILE **temp_files, int P, int M, char *input_file, char **L, int tam);


// Funcao para converter um array de strings (L1 ou L2) em um array de inteiros
// que representam os índices das colunas.
int *L_para_inteiro(char **L_fields_str, int count);
#endif