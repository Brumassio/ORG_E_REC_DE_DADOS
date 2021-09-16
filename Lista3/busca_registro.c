#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

short leia_reg(char buffer[], int tam, FILE *entrada);
int input(char *str, int size);

int main() {
    FILE *entrada;
    char nome_arq[20];
    char buffer[150];
    char sobrenome_busca[15];
    char *sobrenome;
    char *campo;
    short comp_reg;
    bool achou;

    printf("Informe o nome do arquivo de entrada: ");
    input(nome_arq, 20);

    if ((entrada = fopen(nome_arq, "rb")) == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arq);
        exit(EXIT_FAILURE);
    }

    printf("\nSobrenome: ");
    input(sobrenome_busca, 15);

    achou = false;
    comp_reg = leia_reg(buffer, 150, entrada);

    while (!achou && comp_reg > 0) {
        sobrenome = strtok(buffer, "|");
        if (strcasecmp(sobrenome, sobrenome_busca) == 0) {
            achou = true;
        } else {
            comp_reg = leia_reg(buffer, 150, entrada);
        }
    }

    if (achou) {
        printf("\nRegistro encontrado:\n");
        printf("\t%s\n", sobrenome);
        campo = strtok(NULL, "|");
        while (campo != NULL) {
            printf("\t%s\n", campo);
            campo = strtok(NULL, "|");
        }
    } else {
        printf("\nRegistro nao encontrado.\n");
    }

    fclose(entrada);

    return 0;
}

short leia_reg(char buffer[], int tam, FILE *entrada) {
    short comp_reg;
    
    if (fread(&comp_reg, sizeof(short), 1, entrada) == 0) {
        return 0;
    }

    if (comp_reg < tam) {
        fread(buffer, sizeof(char), comp_reg, entrada);
        buffer[comp_reg] = '\0';
        return comp_reg;
    } else {
        return 0;
    }
}

int input(char str[], int size) {
    int i = 0;
    char c = getchar();
    while (c != '\n') {
        if (i < size-1) {
            str[i] = c;
            i++;
        }
        c = getchar();
    }
    str[i] = '\0';
    return i;
}