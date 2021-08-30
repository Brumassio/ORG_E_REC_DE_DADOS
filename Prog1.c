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

int main(void) {
    char nome_arq[20];
    char sobrenome[15];
    char nome[15];
    char endereco[20];
    char cidade[20];
    char estado[3];
    char CEP[9];
    FILE *arq;
    int comprimento;
    printf("\nDigite o nome do arquivo:");
    input(nome_arq,20);
    arq = fopen(nome_arq,"w");
    if(arq == NULL){
        fprintf(stderr,"o arquivo %s não pode ser aberto",nome_arq);
        return EXIT_FAILURE;
    }
    printf("Digite o sobrenome :");
    comprimento= input(sobrenome,15);
    while(comprimento > 0){
        printf("Digite o nome :");
        input(nome,15);
        printf("Digite o endereço :");
        input(endereco,20);
        printf("Digite a cidade :");
        input(cidade,20);
        printf("Digite o estado :");
        input(estado,3);
        printf("Digite o CEP :");
        input(CEP,9);
        
        /*Pode usar a função fputs() também !!!;
        fputs(sobrenome,arq);
        fputs("|",arq);
        fputs(nome,arq);
        fputs("|",arq);
        fputs(endereço,arq);
        fputs("|",arq);
        fputs(cidade,arq);
        fputs("|",arq);
        fputs(estado,arq);
        fputs("|",arq);
        fputs(CEP,arq);
        fputs("|",arq);
        */
       fprintf(arq,"%s|%s|%s|%s|%s|%s|",sobrenome,nome,endereco,cidade,estado,CEP);

       printf("\n Digite o sobrenome:");
       comprimento= input(sobrenome,15);
    }
    return EXIT_SUCCESS;
}