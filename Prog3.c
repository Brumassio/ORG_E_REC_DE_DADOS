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

int main(){
  FILE *SAIDA;
  char NOME_ARQ[20];
  char buffer[tam_buffer];
  char *nome_campos[]= {"Nome", "Endereco", "Cidade" , "Estado" , "CEP"};
  char CAMPO[20];
  int comprimento;
  short comp_reg;
  printf("\nDigite o nome do arquivo ! :");
  input(NOME_ARQ,20); 
  SAIDA=fopen(NOME_ARQ,"w");
  if(SAIDA == NULL){
    fprintf(stderr,"o arquivo %s não pode ser aberto",NOME_ARQ);
    return EXIT_FAILURE;
  }
  printf("\nDigite o Sobrenome! :");
  comprimento = input(buffer,20);
  while (comprimento > 0){
    buffer[0]='\0';
    strcat(buffer,CAMPO);
    strcat(buffer,"|");
    for(int i=0;i<5;i++){
      printf("%s :",nome_campos[i]);
      input(CAMPO,20);
      strcat(buffer,CAMPO);
      strcat(buffer,"|");
    }
    comp_reg = strlen(buffer);
    fwrite(&comp_reg,sizeof(comp_reg),1,SAIDA);
    //fputs(buffer,saida);
    fwrite(buffer,sizeof(char),comp_reg,SAIDA);

    printf("Digite o Sobrenome:");
    comprimento = input(CAMPO,20);
  }
  return EXIT_SUCCESS;
}