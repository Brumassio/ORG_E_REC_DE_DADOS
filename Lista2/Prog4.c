#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam_buffer 120

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

short leia_reg(char buffer[],int tam, FILE *ENTRADA){
    short comp_reg;

    if(fread(&comp_reg,sizeof(short),1,ENTRADA)> 0){
        if(comp_reg < tam){
            fread(buffer,sizeof(char),comp_reg,ENTRADA);
            buffer[comp_reg]='\0';
            return comp_reg;
        }
    }
    
    return 0;
}
int main(){
    FILE *ENTRADA;
    char nome_arq[20];
    char buffer[tam_buffer];
    char *campo;
    short comp_reg;
    int i,j;
    
    printf("Digite o nome do arquivo:");
    input(nome_arq,20);
    ENTRADA = fopen(nome_arq,"r");
    if(ENTRADA == NULL){
        fprintf(stderr,"o arquivo %s não pode ser aberto",nome_arq);
        return EXIT_FAILURE;
    }

    comp_reg = leia_reg(buffer,tam_buffer,ENTRADA);

    i=0;
    while(comp_reg > 0){
        printf("\nRegistro: %d",++i);
        j=0;
        campo = strtok(buffer,"|");
        while(campo != NULL){
            printf("\nCampo #%d:%s",++j,campo);
            campo = strtok(NULL,"|");
        }
        comp_reg = leia_reg(buffer, tam_buffer, ENTRADA);
    }
    fclose(ENTRADA);
    return EXIT_SUCCESS;
}