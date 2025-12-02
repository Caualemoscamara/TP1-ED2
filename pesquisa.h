#ifndef PESQUISA_H
#define PESQUISA_H

//bibliotecas
#include <stdio.h>
#include <stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


//constantes
#define ITENSPAGINA 50
#define MAXTABELA 40000
#define TAM_DADO2 5000

// constantes ARVORE B
#define ORDEM 1000
#define MAX_REGISTROS (2 * ORDEM)
#define MAX_FILHOS (2 * ORDEM + 1)

//estrutura de análise
typedef struct{
    int transeferencia;
    int comparacoes;
    double tempo;
}Analise;

//deffinir tipos dos itens 
typedef struct{
    int chave;
    long int dado1;
    char dado2[TAM_DADO2 + 1];
}TipoItem;


//ACESSO SEQUENCIAL INDEXADO
//definir entrada da tabela de índices da página
typedef struct{
    int chave;
}TipoIndice;


//ARVORE BINÁRIA DE PESQUISA
//estrutura
typedef struct {
        TipoItem itemArvore;
        long esq;
        long dir;
}ArvoreBinaria;



//ARVORE B
//constante
#define M 5  // Grau mínimo da árvore B (ajustável)
//estrutura
typedef struct {
    int n;                    // número de chaves
    int folha;                // indica se é folha (1) ou não (0)
    TipoItem itens[2 * M];        // vetor de chaves
    long filhos[2 * M + 1];   // posições no arquivo da página filha, -1 se não existe
} PaginaB;


//ARVORE B*
typedef enum { INTERNO, FOLHA } TipoNo;

typedef struct {
    int nchaves;
    int chaves[MAX_REGISTROS];
    long filhos[MAX_FILHOS];     // offsets in index file
} PaginaInterna;

typedef struct {
    int nchaves;
    long registros_offsets[MAX_REGISTROS];  // offsets in data file
    long prox_folha;
} PaginaFolha;

typedef struct {
    TipoNo tipo;
    union {
        PaginaInterna interna;
        PaginaFolha folha;
    } conteudo;
}TipoPagina;

#endif