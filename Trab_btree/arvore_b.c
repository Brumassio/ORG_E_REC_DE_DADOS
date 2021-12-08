#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define ordem 5
#define encontrado 1
#define nao_encontrado 0
#define erro 404
#define promocao 2
#define sem_promocao 3
#define DELIM_STR "|"

typedef struct
{
    int contachaves;
    int chave[ordem - 1];
    int filho[ordem];
} PAGINA;

int RRN_novapag()
{
    FILE *arq;
    arq = fopen("Btree.dat", "r+b");
    int tamanhopag = sizeof(PAGINA);
    int tamanhocab = sizeof(sizeof(int));
    fseek(arq, 0, SEEK_END);
    int byteoffset = ftell(arq);
    fclose(arq);

    return (byteoffset - tamanhocab) / tamanhopag;
}

int busca_na_pagina(int chave, PAGINA PAG, int *POS)
{
    int i = 0;
    while ((i < PAG.contachaves) && (chave > PAG.chave[i]))
    {
        i++;
    }
    *POS = i;
    if ((*POS < PAG.contachaves) && (chave == PAG.chave[*POS]))
    {
        return encontrado;
    }
    else
    {
        return nao_encontrado;
    }
}

void le_pagina(int rrn, PAGINA *pag)
{
    FILE *arq;
    arq = fopen("Btree.dat", "r+b");
    int byteoffset = sizeof(int) + (rrn * sizeof(PAGINA));
    fseek(arq, byteoffset, SEEK_SET);
    fread(pag, sizeof(PAGINA), 1, arq);
    fclose(arq);
}

void escreve_pagina(int rrn, PAGINA pag)
{
    FILE *arq;
    arq = fopen("Btree.dat", "r+b");
    int byteoffset = sizeof(int) + (rrn * sizeof(PAGINA));
    fseek(arq, byteoffset, SEEK_SET);
    fwrite(&pag, sizeof(PAGINA), 1, arq);
    fclose(arq);
}

void inicializa_pagina(PAGINA *pag)
{
    pag->contachaves = 0;
    for (int i = 0; i < ordem - 1; i++)
    {
        pag->chave[i] = 0;
        pag->filho[i] = -1; // NULL;
    }
    pag->filho[ordem - 1] = -1; // NULL
}

void insere_na_pagina(int chave, int filho_d, PAGINA *pag)
{
    int i = pag->contachaves;
    while (i > 0 && chave < pag->chave[i - 1])
    {
        pag->chave[i] = pag->chave[i - 1];
        pag->filho[i + 1] = pag->filho[i];
        i--;
    }

    pag->contachaves++;

    pag->chave[i] = chave;
    pag->filho[i + 1] = filho_d;
}

PAGINA copiar_pag(PAGINA *pag)
{
    PAGINA pagaux;
    pagaux.contachaves = pag->contachaves;
    for (int i = 0; i < ordem - 1; i++)
    {
        pagaux.chave[i] = pag->chave[i];
        pagaux.filho[i] = pag->filho[i];
    }
    pagaux.filho[ordem - 1] = pag->filho[ordem - 1];
    return pagaux;
}

void divide(int chave, int filho_d, PAGINA *pag, int *chave_pro, int *filho_d_pro, PAGINA *novapag)
{
    PAGINA pagaux;
    pagaux = copiar_pag(pag);
    int vet[ordem], vet1[ordem + 1];

    int i = ordem - 1;

    for (int i = 0; i < ordem - 1; i++)
    {
        vet[i] = pag->chave[i];
        vet1[i] = pag->filho[i];
    }
    vet1[ordem - 1] = pag->filho[ordem - 1];

    while (chave < pagaux.chave[i - 1] && i > 0)
    {
        vet[i] = vet[i - 1];
        vet1[i + 1] = vet1[i];
        i--;
    }

    vet[i] = chave;

    vet1[i + 1] = filho_d;

    int meio = (ordem) / 2;
    int rrn;
    *filho_d_pro = RRN_novapag();
    *chave_pro = vet[meio];
    i = 0;
    inicializa_pagina(pag);
    while (i < meio)
    {
        pag->chave[i] = vet[i];
        pag->filho[i] = vet1[i];
        pag->contachaves++;
        i++;
    }

    pag->filho[i] = vet1[i];
    inicializa_pagina(novapag);
    i = meio + 1;

    while (i < ordem)
    {
        novapag->chave[novapag->contachaves] = vet[i];
        novapag->filho[novapag->contachaves] = vet1[i];
        novapag->contachaves++;
        i++;
    }
    novapag->filho[novapag->contachaves] = vet1[ordem];
}

int busca(int rrn, int chave, int *rrn_encontrado, int *pos_encontrada)
{
    if (rrn < 0)
    {
        return nao_encontrado;
    }
    PAGINA pag;
    pag.contachaves = rrn;
    int pos = 0;
    int result = busca_na_pagina(chave, pag, &pos);
    if (result == encontrado)
    {
        *rrn_encontrado = rrn;
        *pos_encontrada = pos;
        return encontrado;
    }
    else
    {
        return (busca(pag.filho[pos], chave, rrn_encontrado, pos_encontrada));
    }
}

int insere(int rrn_atual, int chave, int *filho_d_pro, int *chave_pro)
{
    PAGINA pag, novapag;
    int result = 0;
    int pos = 0;
    if (rrn_atual == -1 /* NULL */)
    {
        *chave_pro = chave;
        *filho_d_pro = -1 /* NULL */;
        return promocao;
    }
    else
    {

        le_pagina(rrn_atual, &pag);

        result = busca_na_pagina(chave, pag, &pos);
    }
    if (result == encontrado)
    {
        printf("\nChave duplicada");
        return erro;
    }
    int rrn_pro = 0, chv_pro = 0;

    int retorno = insere(pag.filho[pos], chave, &rrn_pro, &chv_pro);

    if (retorno == sem_promocao || retorno == erro)
    {

        return retorno;
    }
    else
    {
        if (pag.contachaves < ordem - 1)
        {
            insere_na_pagina(chv_pro, rrn_pro, &pag);

            escreve_pagina(rrn_atual, pag);
            return sem_promocao;
        }
        else
        {

            divide(chv_pro, rrn_pro, &pag, chave_pro, filho_d_pro, &novapag);
            escreve_pagina(rrn_atual, pag);
            escreve_pagina(*filho_d_pro, novapag);
            return promocao;
        }
    }
}

int pega_input(FILE *txt)
{
    int input;

    if (fscanf(txt, "%d|", &input) != EOF)
    {
        return input;
    }
    return -1; // Fim do Arquivo !!!
}

void imprimir_pag(FILE *Btree, int rrn)
{
    PAGINA pag;
    int raiz;
    int byteoffset = sizeof(int) + (rrn * sizeof(PAGINA));
    fseek(Btree, byteoffset, SEEK_SET);
    fread(&pag, sizeof(PAGINA), 1, Btree);
    printf("\n\nPagina : %d", rrn);
    printf("\nChaves: ");
    for (int i = 0; i < ordem - 1; i++)
    {
        printf("%d |", pag.chave[i]);
    }
    printf("\nFilhos: ");
    for (int i = 0; i < ordem; i++)
    {
        printf("%d |", pag.filho[i]);
    }
}

void imprime_informacao()
{
    int i = 0;
    int rrns, raiz;
    FILE *Btree = fopen("Btree.dat", "rb");
    if (Btree == NULL)
    {
        printf("\nO arquivo Btree.dat nao existe !!!");
        exit(EXIT_FAILURE);
    }
    rrns = RRN_novapag();
    fseek(Btree, 0, SEEK_SET);
    fread(&raiz, sizeof(int), 1, Btree);
    while (i < rrns)
    {
        if (i != raiz)
        {
            imprimir_pag(Btree, i);
            i++;
        }
        else
        {
            printf("\n\n\n- - - - Pagina Raiz - - - -");
            imprimir_pag(Btree, i);
            printf("\n- - - - - - - - - - - - - -");
            i++;
        }
    }
    fclose(Btree);
}

void atualiza_cabecalho(int raiz, FILE *Btree)
{
    fseek(Btree, 0, SEEK_SET);
    fwrite(&raiz, sizeof(int), 1, Btree);
}

void gerenciador(FILE *Btree, char *arq)
{
    FILE *txt = fopen(arq, "r+b");
    if (txt == NULL)
    {
        printf("\n Essa operacao nao foi concluida com sucesso =(");
        exit(EXIT_FAILURE);
    }

    PAGINA novapag;
    int raiz, filho_d_pro = 0, chave_pro = 0, rrn, c = 0, chave;
    raiz = 0;
    
    inicializa_pagina(&novapag);
    escreve_pagina(raiz, novapag);
    fwrite(&raiz, sizeof(int), 1, Btree);
    chave = pega_input(txt);
    while (chave != -1)
    {
        if (insere(raiz, chave, &filho_d_pro, &chave_pro) == promocao)
        {

            inicializa_pagina(&novapag);

            novapag.chave[0] = chave_pro;
            novapag.filho[0] = raiz;
            novapag.filho[1] = filho_d_pro;

            rrn = RRN_novapag();
            raiz = rrn;
            novapag.contachaves++;
            escreve_pagina(raiz, novapag);
        }
        chave = pega_input(txt);
    }
    atualiza_cabecalho(raiz, Btree);
    fclose(Btree);
    fclose(txt);
}

void criar_arvB(char *arq)
{
    FILE *Btree;
    Btree = fopen("Btree.dat", "w+b");
    if (Btree == NULL)
    {
        printf("\nEssa operacao nao foi concluida com sucesso  :(");
        exit(EXIT_FAILURE); // ERRO
    }
    gerenciador(Btree, arq);
    fclose(Btree);
    printf("\n A operacao criar arvore foi concluida com sucesso  :D ");
}

void Printar_em_Ordem_cres(int rrn)
{
    if (rrn < 0)
    {
        return;
    }
    PAGINA pag;
    le_pagina(rrn, &pag);
    for (int i = 0; i < pag.contachaves; i++)
    {
        Printar_em_Ordem_cres(pag.filho[i]);
        printf(" %d |", pag.chave[i]);
    }
    Printar_em_Ordem_cres(pag.filho[pag.contachaves]);
}

void Ordem_das_chaves()
{
    FILE *Btree = fopen("Btree.dat", "rb");
    int raiz;
    if (Btree == NULL)
    {
        printf("\nO arquivo Btree.dat nao existe !!!");
        exit(EXIT_FAILURE);
    }
    PAGINA *pag;
    fseek(Btree, 0, SEEK_SET);
    fread(&raiz, sizeof(int), 1, Btree);
    // le_pagina(raiz,pag);
    Printar_em_Ordem_cres(raiz);
    // emOrder(Btree,pag);
}

int main(int argc, char *argv[])
{

    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {

        printf("Modo de criacao da arvore-B ... nome do arquivo = %s\n", argv[2]);
        criar_arvB(argv[2]);
        // gerenciador();
    }
    else if (argc == 2 && strcmp(argv[1], "-p") == 0)
    {

        printf("Impressao das informações da arvore-B ...\n");
        imprime_informacao();
    }
    else if (argc == 2 && strcmp(argv[1], "-k") == 0)
    {

        printf("impressao das chaves em ordem crescente  ! ...\n");
        Ordem_das_chaves();
    }
    else
    {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s -c nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s (-p|-k)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
