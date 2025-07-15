#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linha.h"
void imprime_arquivo_juncao(FILE * file1, FILE * file2,char * caminho_arquivo_saida, char **L1, char **L2,int M,int qtd_campos_juncao);
Linha mescla_linha(Linha l1, Linha l2);
int eh_campo_juncao(int j, Linha l1);


