#include "asi.h" 

bool ASI(FILE *arq, int quantidade, int chave, TipoItem *x, Analise *analise1, Analise *analise2){
    //PRÉ-PROCESSAMENTO

    //incio contagem de tempo
    clock_t inicio, fim;
    inicio = clock();
    
    TipoIndice tabela[MAXTABELA];
    TipoItem item;
    int pos = 0; //quantidade de páginas


    //zerar a analise
    analise1->comparacoes =0;
    analise1->transeferencia=0;

    //NÃO PRECISA ORDENAR O ARQUIVO ANTES

    //construção da tabela
    for(int i=0; i<quantidade; i+=ITENSPAGINA){
        fread(&item, sizeof(TipoItem), 1, arq); //ler primeiro item de cada página
        tabela[pos].chave = item.chave; //colocar a chave do item lido na tabela

        pos++;
        analise1->transeferencia++; //incrementar a transferência

        //pular para o inciio da outra página
        fseek(arq, (ITENSPAGINA-1)*sizeof(TipoItem), SEEK_CUR);
    }

    //fim da contagem do tempo
    fim = clock();
    analise1->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    //PESQUISA
    //incio tempo de pesquisa
    inicio = clock();
    //zerar a analise
    analise2->comparacoes =0;
    analise2->transeferencia=0;

    // Realizar pesquisa
    if (pesquisa(arq, tabela, pos, quantidade, analise2, x, chave)) {
        //ENCONTRADO!!!
        //fim da contagem do tempo
        fim = clock();
        analise2->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

        return true;
    } 
    else {
        //NÃO ENCONTRADO!!
        //fim da contagem do tempo
        fim = clock();
        analise2->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

        return false;
    }
}

//Função de fazer a pesquisa
bool pesquisa(FILE *arq, TipoIndice *tab, int NumPagina, int quantidade, Analise *analise, TipoItem* x, int chave){
    TipoItem pagina[ITENSPAGINA]; //vetor de registros, que tem na página 
    int i, quantItens;
    long desloc;

    //apontar para o inicio do arquivo
    rewind(arq);

    // Identificar página provável da chave
    i = 0;
    while (i < NumPagina && tab[i].chave <= chave) {
        i++;
        analise->comparacoes++; //incrementar o n° de comparações
    }

    //caso a chave seja menor q o primeiro item, a chave não está no arquivo!
    if (i == 0) {
        return false;
    }

    // Determinar número de itens na página
    if (i < NumPagina) {
        quantItens = ITENSPAGINA;
    } else { //caso estiver na ultima página
        int resto = quantidade % ITENSPAGINA; 
        quantItens = resto;
        if (resto == 0) { // se o resto for igual a zero, quer dizer q todas as páginas possuem o mesmo n° de itens
            quantItens = ITENSPAGINA;
        }
    }

    // Calcular deslocamento para página anterior
    desloc = (i - 1) * ITENSPAGINA * sizeof(TipoItem); 
    fseek(arq, desloc, SEEK_SET); //deslocar desde o inicio até a página

    //ler toda a página e verificar se leu tudo!
    size_t lidos= fread(pagina, sizeof(TipoItem), quantItens, arq);
    if(lidos < quantItens){
        printf("ERRO: não foi possível carregar toda a página na memória principal, talvez por falta de memória.\n");
    }
    analise->transeferencia ++; //atualizar as tranferencias

    // Pesquisa binária na página
    int busca = buscaBinaria(pagina, 0, quantItens-1, chave, analise);
    if(busca != -1){
        //encontrado
        //atribuir o registro a *x
        x->chave = pagina[busca].chave;
        x->dado1 = pagina[busca].dado1;
        strcpy(x->dado2, pagina[busca].dado2);
        return true;
    }
    else{
        //não encontrado!!
        return false;
    }
}

//pesquisa por busca binária 
int buscaBinaria(TipoItem *registros, int esquerda, int direita, int chave, Analise *analise){
    if (esquerda <= direita){
        int meio = esquerda + (direita - esquerda) / 2;

        if(registros[meio].chave == chave){
            analise->comparacoes++; //incrementa a comparação
            return meio;
        }
        else if (registros[meio].chave < chave){
            analise->comparacoes++; //incrementa a comparação
            return buscaBinaria(registros, meio + 1, direita, chave, analise);
        }
        else{
            analise->comparacoes++; //incrementa a comparação
            return buscaBinaria(registros, esquerda, meio - 1, chave, analise);
        }
    }
    return -1;
}