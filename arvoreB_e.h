#ifndef ARVOREBE_H
#define ARVOREBE_H

#include <stdbool.h>
#include "pesquisa.h"

bool ABE(FILE* f_input, int n, TipoItem *x, int chave, Analise* analise1, Analise *analise2);
long alocar_pagina_index(FILE *fp, Analise *analise);
void escrever_pagina_index(FILE *fp, TipoPagina *pagina, long offset, Analise *analise);
long inserir(FILE *fp_index, FILE *fp_data, long raiz_offset, TipoItem *reg, Analise *analise);
int inserir_rec(FILE *fp_index, FILE *fp_data, long pagina_offset, TipoItem *reg, int *promo_chave, long *promo_offset, Analise *analise);
void split_interna(FILE *fp_index, TipoPagina *interna, long interna_offset, TipoPagina *nova_interna, long *nova_interna_offset, int *promo_chave, Analise *analise);
void split_folha(FILE *fp_index, FILE *fp_data, TipoPagina *folha, long folha_offset, TipoPagina *nova_folha, long *nova_folha_offset, int *promo_chave, Analise *analise);
void ler_registro_data(FILE *fp, TipoItem *reg, long offset, Analise *analise);
long escrever_registro_data(FILE *fp, TipoItem *reg, Analise *analise);
void ler_pagina_index(FILE *fp, TipoPagina *pagina, long offset, Analise *analise);
int busca_chave(int chave, int *chaves, int nchaves, Analise *analise);
int buscar(FILE *fp_index, FILE *fp_data, long pagina_offset, int chave, TipoItem *reg, Analise *analise);

#endif