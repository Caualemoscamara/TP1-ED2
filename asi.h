#ifndef ASI_H
#define ASI_H

#include "pesquisa.h"
bool ASI(FILE *arq, int quantidade, int chave, TipoItem *x, Analise *analise1, Analise *analise2);
bool pesquisa(FILE *arq, TipoIndice *tab, int NumPagina, int quantidade, Analise *analise, TipoItem* x, int chave);
int buscaBinaria(TipoItem *registros, int esquerda, int direita, int chave, Analise *analise);

#endif //ASI_H