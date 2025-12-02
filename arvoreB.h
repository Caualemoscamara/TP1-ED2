#ifndef ARVOREB_H
#define ARVOREB_H

#include "pesquisa.h"

bool AB(FILE* arqDados, int quantidade, int chaveBusca, TipoItem* resultado, Analise* analiseInsercao, Analise* analiseBusca);

long criarRaizB(FILE* abFile);
void inicializarPagina(PaginaB* pag);
void insereB(FILE* abFile, long* raizPosRef, TipoItem item, long posDado, Analise* analise);
void dividirPagina(FILE* abFile, long paiPos, int i, long filhoCheioPos, Analise* analise);
void insereNaoCheiaB(FILE* abFile, long posAtual, TipoItem item, long posDado, Analise* analise);
bool buscaB(FILE* abFile, long posAtual, int chave, TipoItem* encontrado, Analise* analise);

void imprimirArvoreB(FILE* abFile, long posAtual, int nivel);


#endif
