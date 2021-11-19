#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ordem 5
#define encontrado 1
#define nao_encontrado 0
#define erro 404
#define promocao 2
#define sem_promocao 3
#define DELIM_STR "|"


typedef struct {
int contachaves; /* número de chaves na página */
int chave[ordem-1]; /* vetor que armazena as chaves */
int filho[ordem]; /* rrns dos filhos */
} PAGINA;

int RRN_novapag(){
    FILE *arq;
    arq = fopen("Btree.dat","w");
    int tamanhopag = sizeof(PAGINA);// faça TAMANHOPAG receber o tamanho em bytes de uma página
    int tamanhocab = sizeof(sizeof(int));//faça TAMANHOCAB receber o tamanho em bytes do cabeçalho -> socorro?
    fseek(arq,0,SEEK_END);
    int byteoffset;
    fread(&byteoffset,sizeof(int),1,arq); //faça BYTEOFFSET receber o byte-offset do fim do arquivo -> socorro ??
    return (byteoffset - tamanhocab)/tamanhopag;
}

int busca_na_pagina(int chave,PAGINA PAG,int *POS){
    int i=0;
    while((i < PAG.contachaves) && (chave > PAG.chave[i])){
        i++;
    }
    *POS = i;
    if((*POS < PAG.contachaves) && (chave == PAG.chave[*POS])){
        return encontrado;
    }
    else{
        return nao_encontrado;
    }
}

void le_pagina(int rrn,PAGINA *pag){
    FILE *arq;   
    arq=fopen("Btree.dat","r+");
    int byteoffset = rrn*sizeof(PAGINA); // ta certo ???
    fseek(arq,byteoffset,SEEK_SET);
    fread(pag,sizeof(PAGINA),1,arq);
}

void escreve_pagina(int rrn, PAGINA pag){
    FILE *arq;   
    arq = fopen("Btree.dat","r+");
    int byteoffset = rrn*sizeof(PAGINA); // ta certo ???
    fseek(arq,byteoffset,SEEK_SET);
    fwrite(&pag,sizeof(PAGINA),1,arq);
}

void inicializa_pagina(PAGINA *pag){
    pag->contachaves = 0;
    for(int i=0;i<=ordem-1;i++){
        pag->chave[i] = 0;
        pag->filho[i] = -1;//NULL;
    }
    pag->filho[ordem-1] =  -1;//NULL;
}

void insere_na_pagina (int chave, int filho_d,PAGINA *pag){
    int i = pag->contachaves;
    while(i>0 && chave<pag->chave[i-1]){
        pag->chave[i] = pag->chave[i-1];
        pag->filho[i+1] = pag->filho[i];
        i--;
    }
    pag->contachaves++;
    pag->chave[i] = chave;
    pag->filho[i+1] = filho_d;
}

PAGINA copiar_pag(PAGINA *pag){
    PAGINA pagaux;
    pagaux.contachaves = pag->contachaves;
    for(int i=0;i<ordem-1;i++){
        pagaux.chave[i] = pag->chave[i];
    }
    for(int i=0;i<ordem;i++){
        pagaux.filho[i] = pag->filho[i];
    }
    return pagaux;
}

void divide(int chave,int filho_d,PAGINA *pag, int *chave_pro,int *filho_d_pro,PAGINA *novapag){
    PAGINA pagaux;
    pagaux = copiar_pag(pag);//copie PAG para PAGAUX  
    /* Insira CHAVE e FILHO_D nos lugares apropriados em PAGAUX */
    int i = ordem-1;
    while(chave < pagaux.chave[i-1] && i>0){
        pagaux.chave[i] = pagaux.chave[i-1];
        pagaux.filho[i+1] = pagaux.filho[i];
        i--;
    }
    pagaux.chave[i] = chave;
    pagaux.filho[i+1] = filho_d;
    int meio = (ordem)/2;
    int rrn;
    *filho_d_pro = busca_na_pagina(chave,*novapag,&rrn); //faça FILHO_D_PRO receber o RRN que a NOVAPAG terá no arquivo árvore-b -> socorro !? :(
    *chave_pro = pagaux.chave[meio];
    /* Copie as chaves e ponteiros que vêm depois de CHAVE_PRO para NOVAPAG */
    inicializa_pagina(novapag);
    i = meio+1;
    while(i<ordem){
        novapag->chave[novapag->contachaves] = pagaux.chave[i];
        novapag->filho[novapag->contachaves] = pagaux.filho[i];
        novapag->contachaves++;
        i++;
    }
    novapag->filho[novapag->contachaves] = pagaux.filho[i];
}

int busca (int rrn,int chave,int  *rrn_encontrado,int *pos_encontrada){
    if(rrn < 0){
        return nao_encontrado;
    }
    PAGINA pag;
    pag.contachaves =  rrn;
    int pos = 0;
    int result = busca_na_pagina(chave, pag, &pos);
    if(result == encontrado){
        *rrn_encontrado = rrn;  /* rrn da página que contém a chave */
        *pos_encontrada = pos;  /* posição da chave na página*/
        return encontrado;
    }
    else{ /* siga o ponteiro para a próxima página da busca */
        return(busca(pag.filho[pos],chave,rrn_encontrado,pos_encontrada));
    }
}


int insere(int rrn_atual,char chave,int  *filho_d_pro,int *chave_pro){
    PAGINA pag,novapag;
    int result =0;
    int pos = 0;
    if(rrn_atual== -1/* NULL */){
        *chave_pro = chave;
        *filho_d_pro = -1/* NULL */;
        return erro; 
    }
    else{
        le_pagina(rrn_atual,&pag);//leia a página armazenada em RRN_ATUAL para PAG -> fazer função
        result = busca_na_pagina(chave, pag,&pos);
    }
    if(result == encontrado){
        printf("\nChave duplicada");
        return erro;
    }
    int rrn_pro,chv_pro; //n sei ta certo isso !!!
    int retorno = insere(pag.filho[pos],chave,&rrn_pro,&chv_pro);
    if(rrn_atual == -1/* NULL */){
        *chave_pro = chave;
        *filho_d_pro = -1/* NULL */;
        return promocao;
    }
    else{
        le_pagina(rrn_atual,&pag);//leia a página armazenada em RRN_ATUAL para PAG -> função que lê
        result = busca_na_pagina(chave,pag,&pos);
    }
    if(result == encontrado){
        printf("\nchave duplicada!");
        return erro;
    }
    retorno = insere(pag.filho[pos],chave,&rrn_pro,&chv_pro);
    if(retorno == sem_promocao || erro){
        return retorno;
    }
    else{//houve promoção da inserção
        if(pag.contachaves < ordem-1){
            insere_na_pagina(chv_pro,rrn_pro,&pag);//insira CHV_PRO e RRN_PRO (chave promovida e filha) em PAG
            escreve_pagina(rrn_atual,pag);//escreva PAG no arquivo em RRN_ATUAL
            return sem_promocao;
        }
        else{
            divide(chv_pro,rrn_pro,&pag,chave_pro,filho_d_pro,&novapag);
            escreve_pagina(rrn_atual,pag);//escreva PAG no arquivo em RRN_ATUAL
            escreve_pagina(filho_d_pro,novapag);//escreva NOVAPAG no arquivo em FILHO_D_PRO
            return promocao;
        }
    }
}


void criar_arvB(char *arq){
    PAGINA pag;
    FILE *Btree=fopen(arq,"r+");
    gerenciador();
} 

/* int acha_menor(char arq){
    FILE *Btree = fopen(arq,"r+");
} */

/* void impressao_chaves_ordem_crescente(char *arq){
    FILE *Btree = fopen(arq,"r+");
    if(Btree == NULL){
        printf("\nErro, o arquivo nao existe !!!");
        exit(EXIT_FAILURE);
    }
    //fseek(Btree,)

} */

void gerenciador(){
    FILE *Btree = fopen("Btree.dat","r+");
    PAGINA novapag;
    int raiz,chave,aux,filho_d_pro,chave_pro,rrn;
    
    if(Btree != NULL){
        fread(&raiz,sizeof(int),1,Btree); // ??
    }
    else{
        //Btree = fopen("Btree.dat","r+");
        raiz = 0;
        inicializa_pagina(&novapag);
        escreve_pagina(raiz,novapag);
    }
    fseek(Btree,0,SEEK_SET);
    fread(&chave,sizeof(int),1,Btree);
    while(fscanf(Btree,"%d|",&aux)!= -1){
        if(insere(raiz,chave,&filho_d_pro,&chave_pro) == promocao){
            inicializa_pagina(&novapag);
            novapag.chave[0] = chave_pro;  //filho esquerdo  
            novapag.filho[0] = raiz;
            novapag.filho[1] = filho_d_pro; //filho direito 
            escreve_pagina(raiz,novapag);
            rrn = RRN_novapag();
            raiz = rrn; //faça RAIZ receber o RRN de NOVAPAG??
        }
        fseek(Btree,sizeof(int),SEEK_CUR);
        fread(&chave,sizeof(int),1,Btree);
    }
    fwrite(&raiz,sizeof(int),1,Btree);
    fclose(Btree);
} 


int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {

        printf("Modo de criação da árvore-B ... nome do arquivo = %s\n", argv[2]);
        criar_arvB(argv[2]);
        gerenciador();
   

    } else if (argc == 3 && strcmp(argv[1], "-p") == 0) {

        printf("Impressão das informações da árvore-B ... nome do arquivo = %s\n", argv[2]);
        

    } else if (argc == 2 && strcmp(argv[1], "-k") == 0) {

        printf("impressão das chaves em ordem crescente (percurso na árvore-B) ...\n");
        //impressao_chaves_ordem_crescente(argv[2]);
    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-c|-k) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);

    }

    return 0;
}

