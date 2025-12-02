#include "arvoreBinaria.h"

bool ABP(FILE *arq, int quantidade, int chave, TipoItem *x, Analise *analise1, Analise *analise2){
    //PRÉ-PROCESSAMENTOS

    //incio contagem de tempo
    clock_t inicio, fim;
    inicio = clock();

    //zerar as analises de pré-processamento
    analise1->comparacoes =0;
    analise1->transeferencia=0;

    // criar arquivo onde a árvore será construída
    FILE *arq2 = fopen("abp.bin", "w+b"); 
    if (!arq2) {
        printf("Erro ao criar arquivo da ABP.\n");
        return false;
    }

    rewind(arq); //ir para o início do arquivo original
    TipoItem temp;
    //ArvoreBinaria no; //arvore
    
    //criar no raiz
    if(fread (&temp, sizeof(TipoItem), 1, arq) != 1){
        //se não ler
        fclose(arq2);
        return false; //não achou
    }

    analise1->transeferencia++; //atualizar transferência
    long posRaiz = criarRaiz(arq2, temp, analise1);


    //construir a árvore no arquivo
    int lidos = 1; // já leu 1 registro anteriormente (o da raiz)
    for (int i = 1; lidos < quantidade; i++) {
        int qtLer;
        if (lidos + ITENSPAGINA <= quantidade) {
            qtLer = ITENSPAGINA;
        } else {
            qtLer = quantidade - lidos;
        }

        TipoItem *item = malloc(qtLer * sizeof(TipoItem));
        if (!item) {
            printf("Erro de alocação.\n");
            fclose(arq2);
            return false;
        }

        if (fread(item, sizeof(TipoItem), qtLer, arq) != qtLer) {
            free(item);
            break;
        }

        analise1->transeferencia++;

        for (int j = 0; j < qtLer; j++) {
            inserirABP(arq2, item[j], posRaiz, analise1);
        }

        lidos += qtLer;
        free(item);
    }


    //fim da contagem do tempo de pré-processamento
    fim = clock();
    analise1->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // PESQUISA
    //incio tempo de pesquisa
    inicio = clock();

    //zerar as analises de pesquisa
    analise2->comparacoes =0;
    analise2->transeferencia=0;


    bool encontrado = pesquisaABP(arq2, posRaiz, chave, x, analise2);

    fclose(arq2);

    //fim da contagem do tempo
    fim = clock();
    analise2->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    return encontrado;
}

//Função para criar um 
long criarRaiz(FILE *arq, TipoItem item, Analise *analise){
    ArvoreBinaria raiz;
    raiz.itemArvore = item;
    //inicialmente os ponteiros apontam para a posição -1
    raiz.esq = -1; 
    raiz.dir = -1;

    fseek(arq, 0, SEEK_END);
    long posRaiz = ftell(arq) / sizeof(ArvoreBinaria); //posição do nó raiz
    //escrever a raiz no arquivo
    fwrite(&raiz, sizeof(ArvoreBinaria), 1, arq);
    analise -> transeferencia++; //deve atualizar a transferencia para colocar no arquivo?

    return posRaiz;

}

/*
//função para construir a arvore binaria, inserindo itens
void inserirABP(FILE *arq, TipoItem item, long posAtual, Analise *analise) {
    ArvoreBinaria no;

    //ir para a posição da raiz
    fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);

    //ler o nó atual
    if(fread(&no, sizeof(ArvoreBinaria), 1, arq)!= 1){
        return;
    }
    analise->transeferencia++; //atualizar transferencia??

    analise->comparacoes++; //atualizar comparações

    
    if (item.chave < no.itemArvore.chave) {// chave do item é menor q o nó atual vai para o filho da esquerda
        if (no.esq == -1) { //se o filho da esquerda for nulo, adiciona o novo item
            ArvoreBinaria novo;
            //atribui e inicializa o filho
            novo.itemArvore = item;
            novo.esq = -1;
            novo.dir = -1;

            fseek(arq, 0, SEEK_END);
            long pos = ftell(arq) / sizeof(ArvoreBinaria); //posição do item novo
            no.esq = pos;

            //escreve o item novo no arquivo
            fwrite(&novo, sizeof(ArvoreBinaria), 1, arq);
            analise->transeferencia++; //atulaizar a tranferencia

            //ir para o no pai
            fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);
            //rescrever o pai com a posição de onde está o filho
            fwrite(&no, sizeof(ArvoreBinaria), 1, arq);
            analise->transeferencia++; //atualizar tranferencia
        } 
        else { // se o no atual já tiver filho deve fazer uma chamada recursiva para procurar uma posição para o item
            inserirABP(arq, item, no.esq, analise);
        }
    } else { //se a chave não é menor, é maior, então vai para o filho da direita
        if (no.dir == -1) {
            ArvoreBinaria novo;
            //atibui e inicializa o novo filho da direita
            novo.itemArvore = item;
            novo.esq = -1;
            novo.dir = -1;

            fseek(arq, 0, SEEK_END);
            long pos = ftell(arq) /sizeof(ArvoreBinaria); //posição de onde se encontra o novo item
            no.dir = pos;
            
            //escrever novo item no arquivo
            fwrite(&novo, sizeof(ArvoreBinaria), 1, arq);
            analise->transeferencia++; //atualizar tranferencia

            //ir para a posição do pai (no atual)
            fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);
            
            //rescrever o pai com a posição do filho a direita
            fwrite(&no, sizeof(ArvoreBinaria), 1, arq);
            analise->transeferencia++; //atualizar transferencia
        } 
        else { //se o no atual ja tiver filho a direita, deve fazer uma chmada recursiva para procurar um lugar para o item
            inserirABP(arq, item, no.dir, analise);
        }
    }
}
*/

//Função de inserir item iterativo
void inserirABP(FILE *arq, TipoItem item, long posRaiz, Analise *analise) {
    long posAtual = posRaiz;
    ArvoreBinaria no;

    while (1) {
        //ir para a posição do no atual
        fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);
        //ler o no atual 
        fread(&no, sizeof(ArvoreBinaria), 1, arq);
        analise->transeferencia++; //atualizar a transferência
        analise->comparacoes++; //atualizar a comparação

        //se chave do item for menor que a chave do mno atual, vai analisar o filho da esquerda
        if (item.chave < no.itemArvore.chave) {
            if (no.esq == -1) { //se o filho esquerda for nulo, vai inserir o item nessa posição
                ArvoreBinaria novo;
                //atribuir e inicializar o filho
                novo.itemArvore = item;
                novo.esq = -1;
                novo.dir = -1;

                //calcular a nova posição
                fseek(arq, 0, SEEK_END);
                long novaPos = ftell(arq) / sizeof(ArvoreBinaria); //posição do item novo
                no.esq = novaPos; //posição do filho a esquerda atualizada

                //escrever o item na arvore
                fwrite(&novo, sizeof(ArvoreBinaria), 1, arq);
                analise->transeferencia++; //atualiza a tranferência

                //ir para a posição do pai (no atual)
                fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);
                //rescrever o pai (no atual) com a posição a esquerda atualizada
                fwrite(&no, sizeof(ArvoreBinaria), 1, arq);
                analise->transeferencia++; //Atualizar tranferência
                break; //sair
            } 
            else { //se o filho a esquerda ja tiver item vai atualizar a posição atual
                posAtual = no.esq;
            }
        } 
        else { //se a chave do item novo for maior que o no atual, deve ir para o filho da direita
            if (no.dir == -1) { // se o filho da direita for nulo, deve inserir o item novo
                ArvoreBinaria novo;
                //atribuição e inicialização do item novo
                novo.itemArvore = item;
                novo.esq = -1;
                novo.dir = -1;

                //calcular a posição do item novo
                fseek(arq, 0, SEEK_END);
                long novaPos = ftell(arq) / sizeof(ArvoreBinaria); //posição nova do filho a direita
                no.dir = novaPos; //posição do filho a direita atualizada

                //escrever o item novo no arquivo da arvore
                fwrite(&novo, sizeof(ArvoreBinaria), 1, arq);
                analise->transeferencia++; //atulaiza a tranferência

                //ir para a posição do pai
                fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);
                //rescrever o no atual (no pai) com a posição do filho a direita atualizado
                fwrite(&no, sizeof(ArvoreBinaria), 1, arq);
                analise->transeferencia++; //atualiza a transferência
                break;
            } 
            else { //se o filho da direita ja tiver um item deve atualizar a posição atual e continuar procurando um lugar
                posAtual = no.dir;
            }
        }
    }
}


//Função para realizar a busca
bool pesquisaABP(FILE *arq, long posAtual, int chave, TipoItem *x, Analise *analise){
    ArvoreBinaria no;
    
    
    while(posAtual != -1){
        //não encontrado, no atual nulo
        //ir para a posição do nó atual
        fseek(arq, posAtual * sizeof(ArvoreBinaria), SEEK_SET);

        //ler o nó atual
        if(fread(&no, sizeof(ArvoreBinaria), 1, arq)!=1){
            return false;
        }
        analise->transeferencia++; //atualizar as transferências

        analise->comparacoes++; //atualizar as comparações
        
        //comparar a chave com a chave do no atual
        if(chave == no.itemArvore.chave){
            //se for igual encontrou
            *x = no.itemArvore;
            return true;
        }
        //se a chave for menor, deve chamatr recursivamenteo no filho da esquerda
        else if(chave < no.itemArvore.chave){
            posAtual = no.esq;
        }
        //se a chave for maior, chama o nó direita
        else{
            posAtual = no.dir;
        }
    }

    return false;
}