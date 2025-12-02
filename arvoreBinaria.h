#include "pesquisa.h"
#ifndef ARVOREBINARIA_H
#define ARVOREBINARIA_H

bool ABP(FILE *arq, int quantidade, int chave, TipoItem *x, Analise *analise1, Analise *analise2);
void inserirABP(FILE *arq, TipoItem item, long posAtual, Analise *analise);
long criarRaiz(FILE *arq, TipoItem item, Analise *analise);
bool pesquisaABP(FILE *arq, long posAtual, int chave, TipoItem *x, Analise *analise);

#endif