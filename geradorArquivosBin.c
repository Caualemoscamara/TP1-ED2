#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_REGISTROS 2000000
#define TAM_DADO2 5000
#define TAM_DADO3 5000

typedef struct {
    int chave;
    long int dado1;
    char dado2[TAM_DADO2 + 1]; // +1 for '\0'
} TipoRegistro;

void gerar_string_aleatoria(char *str, int len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < len; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[len] = '\0';
}

void embaralhar(int *array, int tamanho) {
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <modo: aleatorio | ascendente | descendente>\n", argv[0]);
        return 1;
    }

    const char *modo = argv[1];
    int *chaves = malloc(NUM_REGISTROS * sizeof(int));
    if (!chaves) {
        perror("Erro ao alocar memória para chaves");
        return 1;
    }

    for (int i = 0; i < NUM_REGISTROS; i++) {
        chaves[i] = i + 1; // 1 a NUM_REGISTROS
    }

    if (strcmp(modo, "aleatorio") == 0) {
        embaralhar(chaves, NUM_REGISTROS);
    } else if (strcmp(modo, "descendente") == 0) {
        // já estão em ordem ascendente, então invertendo
        for (int i = 0; i < NUM_REGISTROS / 2; i++) {
            int temp = chaves[i];
            chaves[i] = chaves[NUM_REGISTROS - 1 - i];
            chaves[NUM_REGISTROS - 1 - i] = temp;
        }
    } else if (strcmp(modo, "ascendente") != 0) {
        printf("Modo inválido. Use: aleatorio, ascendente ou descendente.\n");
        free(chaves);
        return 1;
    }

    FILE *arquivo = fopen("registros.bin", "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo binário para escrita");
        free(chaves);
        return 1;
    }

    srand((unsigned)time(NULL));

    TipoRegistro reg;
    for (int i = 0; i < NUM_REGISTROS; i++) {
        reg.chave = chaves[i];
        reg.dado1 = ((long)rand() << 16) | rand();  // long aleatório
        gerar_string_aleatoria(reg.dado2, TAM_DADO2);

        fwrite(&reg, sizeof(TipoRegistro), 1, arquivo);

        if ((i + 1) % 200000 == 0)
            printf("Gerados %d registros...\n", i + 1);
    }

    fclose(arquivo);
    free(chaves);
    printf("Arquivo 'registros.bin' criado com sucesso em modo '%s' com %d registros.\n", modo, NUM_REGISTROS);

    return 0;
}

