#include "pesquisa.h"
#include "asi.h"
#include "arvoreBinaria.h"
#include "arvoreB.h"
#include "arvoreB_e.h"

int main(int argc, char *argv[]){
    if(argc < 5 || argc > 6){
        return 1; //deve ter 5 ou 6 argumentos na linha de comando
    }
    
    //argumentos da linha de comando
    //o metodo de pesquisa a ser usado
    int opcao = atoi(argv[1]); //1- ASI  2-ABP 3-AB  4-AB*

    //quantidade de registros do arquivo a serem lidos
    int quantidade = atoi(argv[2]);
    
    //situação do arquivo
    int modo = atoi(argv[3]); //1- ascendente  2- descendente  3- aleatorio

    //a chave a ser procurada
    int chave = atoi(argv[4]);

    bool imprimir = false;

    //analisar se deve imprimir ou não registros
    if(argc == 6){
        if(strcmp(argv[5], "[-P]")==0){
            imprimir = true; //deve imprimir os registros na tela
        }
    }

    //atoi usado para conversão de string para int
    
    TipoItem x;
    Analise analise1, analise2; //referentes ao pre processamento e a pesquisa, respectivamemnte

    //nome do arquico
    char nomeArq[21];

    //selecionar qual arquivo abrir
    if(modo == 1){
        strcpy(nomeArq, "ascendente.bin");
    }
    else if (modo == 2){
        strcpy(nomeArq, "descendente.bin");
    }
    else if (modo == 3){
        strcpy(nomeArq, "aleatorio.bin");
    }
    else{
        //situação inválida
        printf("\nERRO MODO INVÀLIDO!!!");
        exit(1);
    }

    //abrir arquivo
    FILE *arquivo = fopen(nomeArq, "rb");
    if(!arquivo){
        printf("ERRO AO ABRIR ARQUIVO PARA LEITURA!");
        exit(1);
    }

    //se o usuário inserir [-P] deve imprimir os registros do arquivo original
    if(imprimir){
        rewind(arquivo); //ir com o arquivo para o inicio
        for (int i=0; i<quantidade; i++){
            if(fread(&x, sizeof(TipoItem), 1, arquivo) !=1){
                return 1;
            }
            printf("\nRegistro %d: \nChave: %d\nDado1: %ld\nDado2: %s\n", i+1, x.chave, x.dado1, x.dado2);
        }
        printf("\n");
    }
    //ir para o inicio do arquivo
    rewind(arquivo);


    //Selecionar o método a ser usado
    switch (opcao){
        case 1: //Acesso Sequencial Indexado
            if(ASI(arquivo, quantidade, chave, &x, &analise1, &analise2)){
                //registro encontrado!!
                printf("\nItem encontrado:\nChave: %d\nDado1: %ld\nDado2: %s\n", x.chave, x.dado1, x.dado2);
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            else{
                //não encontrado!!
                printf("\nNão encontrado!");
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            break;
        
        case 2: //Arvore Binaria de Pesquisa
            if(ABP(arquivo, quantidade, chave, &x, &analise1, &analise2)){
                //Registro encontrado!!
                printf("\nItem encontrado:\nChave: %d\nDado1: %ld\nDado2: %s\n", x.chave, x.dado1, x.dado2);
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            else{
                //não encontrado!!
                printf("\nNão encontrado!");
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            break;
        
        case 3: //Arvore B
            if(AB(arquivo, quantidade, chave, &x, &analise1, &analise2)){
                //Registro encontrado!!
                printf("\nItem encontrado:\nChave: %d\nDado1: %ld\nDado2: %s\n", x.chave, x.dado1, x.dado2);
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            else{
                //não encontrado!!
                printf("\nNão encontrado!");
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            break;
        
        case 4: //Arvore B* 
            if(ABE(arquivo, quantidade, &x, chave, &analise1, &analise2)){
                //Registro encontrado!!
                printf("\nItem encontrado:\nChave: %d\nDado1: %ld\nDado2: %s\n", x.chave, x.dado1, x.dado2);
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            else{
                //não encontrado!!
                printf("\nNão encontrado!");
                printf("\n\nPRÉ-PROCESSAMENTO: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf", analise1.comparacoes, analise1.transeferencia, analise1.tempo);
                printf("\n\nPESQUISA: \nComparações: %d \nTransferências: %d \nTempo de execução: %.6lf\n", analise2.comparacoes, analise2.transeferencia, analise2.tempo);
            }
            break;
        
        default:
            break;
    }

    fclose(arquivo);

    return 0;
}