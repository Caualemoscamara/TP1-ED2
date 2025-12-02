#include "arvoreB_e.h"

// Função principal para controle da ABE (construção e busca)
bool ABE(FILE* f_input, int n, TipoItem *x, int chave, Analise *analise1, Analise *analise2){
    //PRÉ-PROCESSAMENTO

    //iniciar a contagem de tempo
    clock_t inicio, fim;
    inicio = clock();

    FILE *fp_index, *fp_data;

    //zerar as analises do pré-processamento
    analise1->comparacoes = 0;
    analise1->transeferencia = 0;

    // Arquivos de índice e dados
    char index_filename[] = "index_abe.bin";
    char data_filename[] = "data_abe.bin"; 

    //distancia ate a raiz
    long root_offset;

    // Abertura dos arquivos
    fp_index = fopen(index_filename, "w+b");
    fp_data = fopen(data_filename, "w+b");

    if (fp_index == NULL || fp_data == NULL) {
        perror("Erro ao criar arquivos da arvore b*\n");
        exit(1);
    }

    // Inicialização da raiz da árvore B*
    TipoPagina root;
    root.tipo = FOLHA;
    root.conteudo.folha.nchaves = 0;
    root.conteudo.folha.prox_folha = -1;

    // Aloca a raiz no arquivo e escreve a página raiz
    root_offset = alocar_pagina_index(fp_index, analise1);
    escrever_pagina_index(fp_index, &root, root_offset, analise1);


    // Leitura e inserção de registros
    TipoItem reg;
    for (int i = 0; i < n; i++) {
        if (fread(&reg, sizeof(TipoItem), 1, f_input) != 1) {
            printf("Error de leitura do registro  %d\n", i);
            break;
        }
        analise1->transeferencia++; // Contabiliza a leitura do item

        // Insere o item na árvore
        root_offset = inserir(fp_index, fp_data, root_offset, &reg, analise1);
    }

    fim = clock();
    analise1->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // --- PESQUISA ---
    inicio = clock();
    analise2->comparacoes = 0;
    analise2->transeferencia = 0;

    // Busca a chave fornecida
    bool busca = buscar(fp_index, fp_data, root_offset, chave, x, analise2);

    // Fecha os arquivos
    fclose(fp_index);
    fclose(fp_data);

    fim = clock();
    analise2->tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    return busca;
}

// Aloca uma nova página no arquivo de índice
long alocar_pagina_index(FILE *fp, Analise *analise) {
    fseek(fp, 0, SEEK_END); // Move ponteiro para o fim
    long offset = ftell(fp); // Obtém posição
    TipoPagina pagina_vazia = {0}; // Página vazia
    fwrite(&pagina_vazia, sizeof(TipoPagina), 1, fp);
    analise->transeferencia++; // Conta escrita
    return offset;
}

// Escreve uma página no arquivo de índice
void escrever_pagina_index(FILE *fp, TipoPagina *pagina, long offset, Analise *analise) {
    fseek(fp, offset, SEEK_SET); // Vai até posição desejada
    fwrite(pagina, sizeof(TipoPagina), 1, fp); // Escreve página
    analise->transeferencia++; // Conta escrita
}

// Função principal de inserção na árvore
long inserir(FILE *fp_index, FILE *fp_data, long raiz_offset, TipoItem *reg, Analise *analise) {
    int promo_chave;
    long promo_offset;

    // Inserção recursiva
    int cresceu = inserir_rec(fp_index, fp_data, raiz_offset, reg, &promo_chave, &promo_offset, analise);

    // Se não houve promoção, retorna a raiz antiga
    if (!cresceu) return raiz_offset;

    // Caso contrário, cria nova raiz
    TipoPagina nova_raiz;
    nova_raiz.tipo = INTERNO;
    nova_raiz.conteudo.interna.nchaves = 1;
    nova_raiz.conteudo.interna.chaves[0] = promo_chave;
    nova_raiz.conteudo.interna.filhos[0] = raiz_offset;
    nova_raiz.conteudo.interna.filhos[1] = promo_offset;

    // Aloca nova raiz e escreve
    long novo_offset = alocar_pagina_index(fp_index, analise);
    escrever_pagina_index(fp_index, &nova_raiz, novo_offset, analise);
    return novo_offset;
}

// Inserção recursiva de um registro
int inserir_rec(FILE *fp_index, FILE *fp_data, long pagina_offset, TipoItem *reg, int *promo_chave, long *promo_offset, Analise *analise) {
    TipoPagina pagina;
    ler_pagina_index(fp_index, &pagina, pagina_offset, analise);

    // Caso a página seja uma folha
    if (pagina.tipo == FOLHA) {
        PaginaFolha *folha = &pagina.conteudo.folha;

        // Escreve o registro no arquivo de dados
        long reg_offset = escrever_registro_data(fp_data, reg, analise);

        // Encontra posição para inserção
        int i = folha->nchaves - 1;
        while (i >= 0) {
            TipoItem current_reg;
            ler_registro_data(fp_data, &current_reg, folha->registros_offsets[i], analise);
            analise->comparacoes += 2; // Comparações para ordenação
            if (reg->chave > current_reg.chave || (reg->chave == current_reg.chave && reg->dado1 >= current_reg.dado1)) 
                break;
            folha->registros_offsets[i + 1] = folha->registros_offsets[i];
            i--;
        }
        folha->registros_offsets[i + 1] = reg_offset;
        folha->nchaves++;

        // Se não ultrapassou o limite, escreve e retorna
        if (folha->nchaves <= MAX_REGISTROS) {
            escrever_pagina_index(fp_index, &pagina, pagina_offset, analise);
            return 0;
        }

        // Se ultrapassou, divide folha
        TipoPagina nova_folha;
        long nova_folha_offset;
        split_folha(fp_index, fp_data, &pagina, pagina_offset, &nova_folha, &nova_folha_offset, promo_chave, analise);
        *promo_offset = nova_folha_offset;
        return 1;

    } else {
        // Se for nó interno
        PaginaInterna *interna = &pagina.conteudo.interna;
        int pos = busca_chave(reg->chave, interna->chaves, interna->nchaves, analise);
        long filho_offset = interna->filhos[pos];

        // Recursão
        int promo;
        long promo_filhos_offset;
        int cresceu = inserir_rec(fp_index, fp_data, filho_offset, reg, &promo, &promo_filhos_offset, analise);

        if (!cresceu) return 0;

        // Inserção da chave promovida no nó atual
        int i = interna->nchaves - 1;
        while (i >= 0 && promo < interna->chaves[i]) {
            analise->comparacoes++;
            interna->chaves[i + 1] = interna->chaves[i];
            interna->filhos[i + 2] = interna->filhos[i + 1];
            i--;
        }
        interna->chaves[i + 1] = promo;
        interna->filhos[i + 2] = promo_filhos_offset;
        interna->nchaves++;

        // Se não ultrapassou o limite, escreve
        if (interna->nchaves <= MAX_REGISTROS) {
            escrever_pagina_index(fp_index, &pagina, pagina_offset, analise);
            return 0;
        }

        // Caso contrário, divide o nó interno
        TipoPagina nova_interna;
        long nova_interna_offset;
        split_interna(fp_index, &pagina, pagina_offset, &nova_interna, &nova_interna_offset, promo_chave, analise);
        *promo_offset = nova_interna_offset;
        return 1;
    }
}

// Busca na árvore B* (recursiva)
int buscar(FILE *fp_index, FILE *fp_data, long pagina_offset, int chave, TipoItem *reg, Analise *analise) {
    TipoPagina pagina;
    // Lê a página do índice (conta como transferência)
    ler_pagina_index(fp_index, &pagina, pagina_offset, analise);

    if (pagina.tipo == FOLHA) {
        PaginaFolha *folha = &pagina.conteudo.folha;
        // Percorre todas as chaves da folha
        for (int i = 0; i < folha->nchaves; i++) {
            TipoItem current_reg;
            // Lê registro do arquivo de dados (transferência)
            ler_registro_data(fp_data, &current_reg, folha->registros_offsets[i], analise);

            // Compara chave lida com a buscada
            analise->comparacoes++;
            if (current_reg.chave == chave) {
                // Encontrou a chave, copia registro para retorno
                *reg = current_reg;
                return 1;
            }

            // Se chave atual maior que buscada, para busca na folha
            analise->comparacoes++;
            if (current_reg.chave > chave) {
                break;
            }
        }
        return 0;

    } else {
        // Nó interno: realiza busca para achar filho correto
        PaginaInterna *interna = &pagina.conteudo.interna;

        int pos = 0;
        // Avança até achar posição para descida
        while (pos < interna->nchaves) {
            analise->comparacoes++;
            if (chave <= interna->chaves[pos]) break;
            pos++;
        }

        // Se chave igual à chave do nó, tenta subárvore direita
        analise->comparacoes++;
        if (pos < interna->nchaves && chave == interna->chaves[pos]) {
            if (buscar(fp_index, fp_data, interna->filhos[pos+1], chave, reg, analise)) {
                return 1;
            }
        }

        // Verifica subárvore esquerda (filho correspondente)
        return buscar(fp_index, fp_data, interna->filhos[pos], chave, reg, analise);
    }
}

// Divide nó interno quando está cheio
void split_interna(FILE *fp_index, TipoPagina *interna, long interna_offset, TipoPagina *nova_interna, long *nova_interna_offset, int *promo_chave, Analise *analise){
    // Aloca nova página para nó interno
    *nova_interna_offset = alocar_pagina_index(fp_index, analise);

    nova_interna->tipo = INTERNO;
    nova_interna->conteudo.interna.nchaves = 0;

    int mid = interna->conteudo.interna.nchaves / 2;
    int total = interna->conteudo.interna.nchaves;

    // Chave do meio é promovida para o pai
    *promo_chave = interna->conteudo.interna.chaves[mid];

    // Move chaves e filhos após meio para o novo nó
    for (int i = mid + 1; i < total; i++) {
        nova_interna->conteudo.interna.chaves[nova_interna->conteudo.interna.nchaves] = 
            interna->conteudo.interna.chaves[i];
        nova_interna->conteudo.interna.filhos[nova_interna->conteudo.interna.nchaves] = 
            interna->conteudo.interna.filhos[i];
        nova_interna->conteudo.interna.nchaves++;   
    }
    // Último filho
    nova_interna->conteudo.interna.filhos[nova_interna->conteudo.interna.nchaves] = 
        interna->conteudo.interna.filhos[total];

    // Reduz quantidade de chaves do nó original
    interna->conteudo.interna.nchaves = mid;

    // Escreve nós atualizados no arquivo
    escrever_pagina_index(fp_index, interna, interna_offset, analise);
    escrever_pagina_index(fp_index, nova_interna, *nova_interna_offset, analise);
}

// Divide nó folha quando está cheio
void split_folha(FILE *fp_index, FILE *fp_data, TipoPagina *folha, long folha_offset, TipoPagina *nova_folha, long *nova_folha_offset, int *promo_chave, Analise *analise) {
    // Aloca nova folha
    *nova_folha_offset = alocar_pagina_index(fp_index, analise);

    nova_folha->tipo = FOLHA;
    nova_folha->conteudo.folha.nchaves = 0;
    nova_folha->conteudo.folha.prox_folha = folha->conteudo.folha.prox_folha;

    int mid = folha->conteudo.folha.nchaves / 2; // ponto médio para divisão
    int total = folha->conteudo.folha.nchaves;  

    // Chave promovida é a primeira da nova folha (posição mid)
    TipoItem reg;
    ler_registro_data(fp_data, &reg, folha->conteudo.folha.registros_offsets[mid], analise);
    *promo_chave = reg.chave;

    // Move metade dos registros para nova folha
    for (int i = mid; i < total; i++) {
        nova_folha->conteudo.folha.registros_offsets[nova_folha->conteudo.folha.nchaves++] = 
            folha->conteudo.folha.registros_offsets[i];
    }

    // Atualiza ponteiros da folha original
    folha->conteudo.folha.nchaves = mid;
    folha->conteudo.folha.prox_folha = *nova_folha_offset;

    // Escreve folhas atualizadas no arquivo
    escrever_pagina_index(fp_index, folha, folha_offset, analise);
    escrever_pagina_index(fp_index, nova_folha, *nova_folha_offset, analise);
}

// Lê uma página do arquivo de índice no offset especificado
void ler_pagina_index(FILE *fp, TipoPagina *pagina, long offset, Analise *analise) {
    fseek(fp, offset, SEEK_SET);
    fread(pagina, sizeof(TipoPagina), 1, fp);
    analise->transeferencia++;  // Conta leitura de página (transferência)
}

// Escreve um registro no arquivo de dados e retorna offset
long escrever_registro_data(FILE *fp, TipoItem *reg, Analise *analise) {
    fseek(fp, 0, SEEK_END);
    long offset = ftell(fp);
    fwrite(reg, sizeof(TipoItem), 1, fp);
    analise->transeferencia++; // Conta escrita (transferência)
    return offset;
}

// Lê registro do arquivo de dados no offset especificado
void ler_registro_data(FILE *fp, TipoItem *reg, long offset, Analise *analise) {
    fseek(fp, offset, SEEK_SET);
    fread(reg, sizeof(TipoItem), 1, fp);
    analise->transeferencia++; // Conta leitura (transferência)
}

// Busca posição correta para inserir chave no nó interno
int busca_chave(int chave, int *chaves, int nchaves, Analise *analise) {
    int i = 0;
    while (i < nchaves) {
        analise->comparacoes++;
        if (chave <= chaves[i]) break;
        i++;
    }
    return i;
}
