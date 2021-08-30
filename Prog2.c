#include <stdio.h>
#include <stdlib.h>

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

int leia_campos(char str[], int size,FILE *entrada) {
int i = 0;
char c = fgetc(entrada);
while ((c != EOF)&&(c != '|')) {
    if (i < size-1) {
    str[i] = c;
    i++;
    }
  c = fgetc(entrada);
 }
str[i] = '\0';
return i;
}



int main(void){
    FILE *entrada;
    char nome_arq[20];
    int comp_campo;
    char campo[50];
    int i = 1;
    printf("Digite o nome do arquivo:");
    input(nome_arq,20);
    entrada = fopen(nome_arq,"r");
    if(entrada == NULL){
        fprintf(stderr,"o arquivo %s não pode ser aberto",nome_arq);
        return EXIT_FAILURE;
    }
    comp_campo = leia_campos(campo,50,entrada);
    while(comp_campo > 0){
        printf("Campo #%d : %s\n",i,campo);
        comp_campo = leia_campos(campo,50,entrada);
        i++;
    }
    fclose(entrada);
    return EXIT_SUCCESS;
}