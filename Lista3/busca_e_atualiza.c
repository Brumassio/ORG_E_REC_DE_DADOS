/* programa que abre ou cria um arquivo de registros de tamanho fixo.
   Os registros podem ser adicionados ou modificados.
   Registros a serem modificados são buscados por RRN */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DELIM_STR "|"
#define COMP_REG 64

char *prompt[] = {
    "    Sobrenome: ",
    "Primeiro nome: ",
    "     Endereco: ",
    "       Cidade: ",
    "       Estado: ",
    "          CEP: ",
    "" // string vazia para encerrar o loop
};

// GLOBAIS
struct {
    int cont_reg;
} cab; // cabeçalho do arquivo - 4 bytes

// INTERFACES
int menu();
void pedir_campos(char *);
int pedir_rrn();
void ler_e_mostrar();
int alterar();
char ucase(char);
int input(char *str, int size);
int input_int();
char input_char();
void concatena_campo(char *buffer, char *campo);


// IMPLEMENTAÇÕES
int main() {
    FILE *arq;
    int i, opcao, rrn;
    int byte_offset;
    char nome_arq[20];
    char buffer[COMP_REG+1];

    printf("Digite o nome do arquivo de registros: ");
    input(nome_arq, 20);

    if ((arq = fopen(nome_arq, "r+b")) == NULL) {
        arq = fopen(nome_arq, "w+b");
        cab.cont_reg = 0;
        fwrite(&cab, sizeof(cab), 1, arq);
    } else {
        fread(&cab, sizeof(cab), 1, arq);
    }

    while ((opcao = menu()) < 3) {

        switch(opcao) {
            case 1: // inserção
                printf("Digite os dados para o novo registro\n\n");
                pedir_campos(buffer);
                byte_offset = cab.cont_reg * COMP_REG + sizeof(cab);
                fseek(arq, (long) byte_offset, SEEK_SET);
                fwrite(buffer, COMP_REG, 1, arq);
                cab.cont_reg++;
                break;

            case 2: // busca e alteração
                rrn = pedir_rrn();

                if (rrn >= cab.cont_reg) {
                    printf("O RRN eh muito grande... RRN max = %d", cab.cont_reg-1);
                    printf("... retornando ao menu ...");
                    break;
                }

                byte_offset = rrn * COMP_REG + sizeof(cab);
                fseek(arq, (long) byte_offset, SEEK_SET);

                ler_e_mostrar(arq);

                if (alterar()) {
                    printf("\n\nDigite os novos dados do registro: \n\n");
                    pedir_campos(buffer);
                    fseek(arq, (long) byte_offset, SEEK_SET);
                    fwrite(buffer, COMP_REG, 1, arq);
                }
                break;
        }
    }

    rewind(arq);
    fwrite(&cab, sizeof(cab), 1, arq);

    fclose(arq);

    return EXIT_SUCCESS;
}

/* função local para perguntar ao usuario pela próx. operação
   retorna um valor numérico */
int menu() {
    int opcao;
    char str[5];
    
    printf("\n\nPROGRAMA PARA CRIACAO E ALTERACAO DE ARQUIVO\n");
    printf("\n\nSuas opcoes sao:\n\n");
    printf("\t1. Adicionar um registro ao final do arquivo\n");
    printf("\t2. Recuperar um registro por RRN para alteracoes \n");
    printf("\t3. Terminar o programa\n\n");
    printf("Digite o numero da sua escolha: ");
    opcao = input_int();
    return opcao;

}

/* função para a entrada dos campos do registro,
   escrevendo-os no buffer passado por parâmetro */
void pedir_campos(char buffer[]) {
    int i;
    char campo[50];

    // limpa o buffer (inicializa como uma string vazia)
    buffer[0] = '\0';

    // obtem os campos
    for (i = 0; prompt[i] != ""; i++) {
        printf("%s", prompt[i]);
        input(campo, 50);
        concatena_campo(buffer, campo);
    }

    /* Atenção! Esta função não é segura, pois deixa que o usuário
       informe campos que, juntos, ultrapassem a capacidade do registro.
       Você pode arrumar isso se desejar...
    */
    
}

/* função para a entrada do RRN do registro a ser buscado/alterado */
int pedir_rrn() {
    int rrn;

    printf("\n\nDigite o RRN do registro: ");
    rrn = input_int();
    return rrn;
}

/* função que lê e mostra um registro, sem seek.
   a leitura começa na posição atual do arquivo */
void ler_e_mostrar(FILE *arq) {
    char buffer[COMP_REG+1];
    char *campo;

    fread(buffer, COMP_REG, 1, arq);

    printf("\n\n\n\nConteudo do registro\n");
    campo = strtok(buffer, "|");
    while (campo != NULL) {
        printf("\t%s\n",campo);
        campo = strtok(NULL, "|");
    }
}

/* função para perguntar ao usuário se ele quer modificar o registro ou nao
   Retorna 1 para SIM e 0 para NAO */
int alterar() {
    char resposta;

    printf("\n\nVoce quer modificar este registro?\n");
    printf("    Responda S ou N, seguido de <ENTER>: ");
    resposta = ucase(input_char());
    return ((resposta == 'S'));
}

/* Converte o char si para UPPERCASE e retorna em so */
char ucase(char si){
    char so = (si >= 'a' && si <= 'z') ? si & 0x5f : si;
    return so;
}

/* função que lê uma linha do stdin para a string str */
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

/* função que lê e retorna um número inteiro do stdin */
int input_int() {
    char str[25];
    if (input(str, 25) > 0)
        return atoi(str);
    return 0;
}

/* função que lê e retorna um char do stdin */
char input_char() {
    char str[2];
    input(str, 2);
    return str[0];
}

/* função para concatenar um campo campo ao buffer e 
   adicionar o delimitador de campo em seguida */
void concatena_campo(char *buffer, char *campo) {
    strcat(buffer, campo);
    strcat(buffer, DELIM_STR);
}