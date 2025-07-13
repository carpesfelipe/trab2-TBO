#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct linha
{
    char **colunas;
    char **campos_juncao;
    int qtd_campos_juncao;
    int qtd_colunas;
}Linha;

int compara_linhas(const void *linha1, const void *linha2);

void ordena_linhas(Linha * vet_linhas,int qtd_linhas);

Linha inicia_linha(char ** campos_juncao,int qtd_campos_juncao);

void add_campos(Linha * linha_ptr, char * string);

void imprime_linhas_arquivo(Linha * vet_linha,FILE * file, int M);

void destroi_linha(Linha l);