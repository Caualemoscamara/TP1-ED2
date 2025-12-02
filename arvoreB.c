#include "arvoreB.h"

bool AB(FILE* arqDados, int quantidade, int chaveBusca, TipoItem* resultado, Analise* analiseInsercao, Analise* analiseBusca) {
    // PRÉ-PROCESSAMENTO

    //verificar se o arquivo original ta aberto
    if (!arqDados){
        return false;
    }

    //criar arquivo binario para a arvore b
    FILE* arqArvore = fopen("ab.bin", "w+b");
    if (!arqArvore){
        return false;
    }

    //zerar as analises
    analiseInsercao->comparacoes = 0;
    analiseInsercao->transeferencia = 0;

    //iniciar contagem de tempo
    clock_t tInicio = clock();

    //cria a pagina raiz
    long posRaiz = criarRaizB(arqArvore);

    rewind(arqDados); //ir para o inicio

    //ler ccada registro do arquivo para inserir na arvore b
    for (int i = 0; i < quantidade; i++) {
        TipoItem item;
        if (fread(&item, sizeof(TipoItem), 1, arqDados) != 1){
            break;
        }
        analiseInsercao->transeferencia++; //atualiza a transferencia
        long posRegistro = ftell(arqDados) - sizeof(TipoItem);
        //insere o registro
        insereB(arqArvore, &posRaiz, item, posRegistro, analiseInsercao);
    }

    //encerrar contagem de tempo
    clock_t tFim = clock();
    analiseInsercao->tempo = (double)(tFim - tInicio) / CLOCKS_PER_SEC;

    //PESQUISA

    //inicar contagem de tempo
    tInicio = clock();
    //zerar as analises
    analiseBusca->comparacoes = 0;
    analiseBusca->transeferencia = 0;

    //pesquisar
    bool achou = buscaB(arqArvore, posRaiz, chaveBusca, resultado, analiseBusca);

    //fim da contagem
    tFim = clock();
    analiseBusca->tempo = (double)(tFim - tInicio) / CLOCKS_PER_SEC;

    fclose(arqArvore);
    return achou;
}

//função que cria a raiz
long criarRaizB(FILE* abFile) {
    PaginaB raiz;
    //inicializa a página
    inicializarPagina(&raiz);

    //calcular a posição da página
    fseek(abFile, 0, SEEK_END);
    long pos = ftell(abFile) / sizeof(PaginaB);
    fwrite(&raiz, sizeof(PaginaB), 1, abFile);

    return pos;
}

//função que inicializa a página
void inicializarPagina(PaginaB* pag) {
    pag->n = 0; //inicializa com zero, zero itens
    pag->folha = 1; //inicilamnete é folha
    
    //inicializar os itens, retirar o lixo de memória
    memset(pag->itens, 0, sizeof(pag->itens));


    //coloca -1 nos filhos para indicar posição nula 
    for (int i = 0; i < 2 * M + 1; i++){
        pag->filhos[i] = -1;
    }
}

//Função principal de inserção
void insereB(FILE* abFile, long* raizPosRef, TipoItem item, long posDado, Analise* analise) {
    PaginaB raiz;

    //ir ate a posição da página
    fseek(abFile, *raizPosRef * sizeof(PaginaB), SEEK_SET);
    //ler a página
    fread(&raiz, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++; //atualizar as transferencias

    //se a qtd de itens estiver no limite (pag cheia) entra e então cria outra página
    if (raiz.n == 2 * M) {

        //criar nova página
        PaginaB novaRaiz;
        inicializarPagina(&novaRaiz); //inicializar
        novaRaiz.folha = 0; //não sera folha
        novaRaiz.filhos[0] = *raizPosRef; //a raiz antiga se torna a nova

        //calcular a posição da nova raiz
        fseek(abFile, 0, SEEK_END);
        long novaRaizPos = ftell(abFile) / sizeof(PaginaB);
        //escrever a raiz no arquivo
        fwrite(&novaRaiz, sizeof(PaginaB), 1, abFile);
        analise->transeferencia++; //atualizar

        //dividir a antiga raiz cheia, levando um item a nova raiz
        dividirPagina(abFile, novaRaizPos, 0, *raizPosRef, analise);
        *raizPosRef = novaRaizPos;
    }

    //se não tiver cheia insere o item na função insere não cheia.
    insereNaoCheiaB(abFile, *raizPosRef, item, posDado, analise); //levar a página filha que está cheia e a nova páguina (pai)
}

void dividirPagina(FILE* abFile, long paiPos, int i, long filhoCheioPos, Analise* analise) {

    //ler a página pai e filho ( que está cheia)
    //ler a pág pai
    PaginaB pai, filho, novo;
    fseek(abFile, paiPos * sizeof(PaginaB), SEEK_SET);
    fread(&pai, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++;

    //ler a pág filho, que está cheia
    fseek(abFile, filhoCheioPos * sizeof(PaginaB), SEEK_SET);
    fread(&filho, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++;

    //criar outra página, irmã direita
    inicializarPagina(&novo); //inicializar
    novo.folha = filho.folha; // mesmo nível da filho
    novo.n = M; // metade dos itens vai para essa página

    //copiar os itens da metdade superior (maiores) para a nova págijna (pag irmaa a direita)
    for (int j = 0; j < M; j++) {
        novo.itens[j] = filho.itens[j + M + 1];
    }

    //copiar os ponteiros filho se a página não for folha
    if (!filho.folha){
        for (int j = 0; j <= M; j++) {
            novo.filhos[j] = filho.filhos[j + M + 1];
        }
    }

    filho.n = M; //filho original tmb tem metade dos itens, so que a metade de itens menores

    //ir para a posição da nova página e escrever
    fseek(abFile, 0, SEEK_END);
    long novoPos = ftell(abFile) / sizeof(PaginaB);
    fwrite(&novo, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++; //atualizar a transferencia

    //move filhos e itens no pai para a direita para inserir a posição do novo filho 
    for (int j = pai.n; j >= i + 1; j--) {
        pai.filhos[j + 1] = pai.filhos[j];
    }
    pai.filhos[i + 1] = novoPos;


    for (int j = pai.n - 1; j >= i; j--) {
        pai.itens[j + 1] = pai.itens[j];
    }
    pai.itens[i] = filho.itens[M];
    pai.n++;

    //atualizar o pai no aqruivo
    fseek(abFile, paiPos * sizeof(PaginaB), SEEK_SET);
    fwrite(&pai, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++;

    //atualizar o filho no arquivo
    fseek(abFile, filhoCheioPos * sizeof(PaginaB), SEEK_SET);
    fwrite(&filho, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++;
}

void insereNaoCheiaB(FILE* abFile, long posAtual, TipoItem item, long posDado, Analise* analise) {
    PaginaB pag;
    //ir ate a página e ler
    fseek(abFile, posAtual * sizeof(PaginaB), SEEK_SET);
    fread(&pag, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++; //atualizar transferencia

    int i = pag.n - 1; //procurar pelo ultimo iten extistente

    //se for um nó folha a inserção ocorrre
    if (pag.folha) {
        //comparações para encontrar a posição correta para inserir
        while (i >= 0 && item.chave < pag.itens[i].chave) {
            analise->comparacoes++;
            pag.itens[i + 1] = pag.itens[i]; //abrir espaço para a chave entrar
            i--;
        }
        analise->comparacoes++;
        pag.itens[i + 1] = item; //inserir o item
        pag.n++; //incrementar a quantidade de itens na página

        //salvar a página atualizada no arquivo
        fseek(abFile, posAtual * sizeof(PaginaB), SEEK_SET);
        fwrite(&pag, sizeof(PaginaB), 1, abFile);
        analise->transeferencia++;
    } 
    else { //se não for folha deve descobrir para qual filho deve seguir
        while (i >= 0 && item.chave < pag.itens[i].chave) {
            analise->comparacoes++;
            i--;
        }
        analise->comparacoes++;
        i++;


        long filhoPos = pag.filhos[i]; //pega a posição do filho
        PaginaB filho;
        //carrega a página filho (transferencia)
        fseek(abFile, filhoPos * sizeof(PaginaB), SEEK_SET);
        fread(&filho, sizeof(PaginaB), 1, abFile);
        analise->transeferencia++; //atualizar transferencia

        //se o filho tiver cheio deve dividir
        if (filho.n == 2 * M) {
            // dividir a página
            dividirPagina(abFile, posAtual, i, filhoPos, analise);

            //ir para a posição e ler a página atual
            fseek(abFile, posAtual * sizeof(PaginaB), SEEK_SET);
            fread(&pag, sizeof(PaginaB), 1, abFile);

            analise->transeferencia++; //atualizar a trasferencia
            if (item.chave > pag.itens[i].chave){
                i++;   
            }
        }

        //se não deve chamar recursivamente para inserir na propria chave
        insereNaoCheiaB(abFile, pag.filhos[i], item, posDado, analise);
    }
}

/*
bool buscaB(FILE* abFile, long posAtual, int chave, TipoItem* encontrado, Analise* analise) {
    if (posAtual == -1) return false;
    PaginaB pag;
    fseek(abFile, posAtual * sizeof(PaginaB), SEEK_SET);
    fread(&pag, sizeof(PaginaB), 1, abFile);
    analise->transeferencia++;

    int i = 0;
    while (i < pag.n && chave > pag.itens[i].chave) {
        analise->comparacoes++;
        i++;
    }
    analise->comparacoes++;

    if (i < pag.n && chave == pag.itens[i].chave) {
        *encontrado = pag.itens[i];
        return true;
    }
    if (pag.folha) return false;
    return buscaB(abFile, pag.filhos[i], chave, encontrado, analise);
}
*/

bool buscaB(FILE* abFile, long posAtual, int chave, TipoItem* encontrado, Analise* analise) {
    while (posAtual != -1) {
        PaginaB pag;
        fseek(abFile, posAtual * sizeof(PaginaB), SEEK_SET);
        fread(&pag, sizeof(PaginaB), 1, abFile);
        analise->transeferencia++;

        int i = 0;
        while (i < pag.n && chave > pag.itens[i].chave) {
            analise->comparacoes++;
            i++;
        }
        analise->comparacoes++; // última comparação

        if (i < pag.n && chave == pag.itens[i].chave) {
            *encontrado = pag.itens[i];
            return true;
        }

        if (pag.folha) {
            return false; // chave não está na árvore
        }

        posAtual = pag.filhos[i]; // avança para o próximo nó
    }

    return false;
}
