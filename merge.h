#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//essa funcao realiza efetivamente o merge
void ordenar_blocos(FILE **temp_files, int P, int M, char *arquivo_in, char **L, int tam);

void limpar_arquivos_destino(FILE **temp_files, int indice_arquivo_destino, int P);