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
//#define chaves_inseridas 20


typedef struct {
int contachaves; /* número de chaves na página */
int chave[ordem-1]; /* vetor que armazena as chaves */
int filho[ordem]; /* rrns dos filhos */
} PAGINA;

int RRN_novapag(){
    FILE *arq;
    arq = fopen("Btree.dat","r+b");
    int tamanhopag = sizeof(PAGINA);// faça TAMANHOPAG receber o tamanho em bytes de uma página
    int tamanhocab = sizeof(sizeof(int));//faça TAMANHOCAB receber o tamanho em bytes do cabeçalho -> socorro?
    fseek(arq,0,SEEK_END);
    int byteoffset = ftell(arq);
    //fread(&byteoffset,sizeof(int),1,arq); //faça BYTEOFFSET receber o byte-offset do fim do arquivo -> socorro ??
    fclose(arq);
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
    arq=fopen("Btree.dat","r+b");
    int byteoffset = sizeof(int)+(rrn*sizeof(PAGINA)); // ta certo ???
    fseek(arq,byteoffset,SEEK_SET);
    fread(pag,sizeof(PAGINA),1,arq);
    fclose(arq);
}

void escreve_pagina(int rrn, PAGINA pag){
    FILE *arq;   
    arq = fopen("Btree.dat","r+b");
    int byteoffset = sizeof(int)+(rrn*sizeof(PAGINA)); // ta certo ???
    fseek(arq,byteoffset,SEEK_SET);
    fwrite(&pag,sizeof(PAGINA),1,arq);
    fclose(arq);
}

void inicializa_pagina(PAGINA *pag){
    pag->contachaves = 0;
    for(int i=0;i<ordem-1;i++){
        pag->chave[i] = 0;
        pag->filho[i] = -1;//NULL;
    }
    pag->filho[ordem] =  -1;//NULL
}

void insere_na_pagina (int chave, int filho_d,PAGINA *pag){
    int i = pag->contachaves;
    while(i > 0 && chave < pag->chave[i-1]){
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
        pagaux.filho[i] = pag->filho[i];
    }
    pagaux.filho[ordem] = pag->filho[ordem];
    return pagaux;
}

void divide(int chave,int filho_d,PAGINA *pag, int *chave_pro,int *filho_d_pro,PAGINA *novapag){
    PAGINA pagaux;
    pagaux = copiar_pag(pag);//copie PAG para PAGAUX  
    /* Insira CHAVE e FILHO_D nos lugares apropriados em PAGAUX */
    printf("\nola, estou dentro da desgraça do divide");
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
    *filho_d_pro = RRN_novapag(); //faça FILHO_D_PRO receber o RRN que a NOVAPAG terá no arquivo árvore-b 
    *chave_pro = pagaux.chave[meio];
    /* Copie as chaves e ponteiros que vêm depois de CHAVE_PRO para NOVAPAG */
    i = 0;
    inicializa_pagina(pag);
    while(i<meio){
        pag->chave[i]= pagaux.chave[i];
        pag->filho[i]= pagaux.filho[i];
        pag->contachaves++;
        i++;
    }
    pag->filho[i] = pagaux.filho[i];
    inicializa_pagina(novapag);
    i = meio+1;
    while(i<ordem){
        novapag->chave[novapag->contachaves]= pagaux.chave[i];
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
    //printf("\nsocorro");
    PAGINA pag,novapag;
    int result =0;
    int pos = 0;
    if(rrn_atual== -1/* NULL */){
        //printf("\nalo na paz?");
        *chave_pro = chave;
        *filho_d_pro = -1/* NULL */;
       // printf("\n to no return promocao");
        return promocao; 
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
    int retorno = insere(pag.filho[pos],chave,&rrn_pro,&chv_pro); // ERRO TA AQUI ! :( ??
    //printf("\noi retorno 2");
    //retorno = insere(pag.filho[pos],chave,&rrn_pro,&chv_pro);
    if(retorno == sem_promocao || retorno == erro){
        //printf("\nreturn retorno");
        printf("\nContachaves = %d",pag.contachaves);
        return retorno;
    }
    else{//houve promoção da inserção
        if(pag.contachaves < ordem-1){
            //printf("\n nao houve promoção na insercao / pag.cont <ordem");
            printf("\nrrn_atual = %d, chave_pro= %d, chave = %d,conta chaves = %d, rrn_pro= %d",rrn_atual,*chave_pro,chave,pag.contachaves, rrn_pro );
            insere_na_pagina(chv_pro,rrn_pro,&pag);//insira CHV_PRO e RRN_PRO (chave promovida e filha) em PAG
            //printf("\nConta Chaves pos insere na pag = %d",pag.contachaves);
            escreve_pagina(rrn_atual,pag);//escreva PAG no arquivo em RRN_ATUAL
            return sem_promocao;
        }
        else{
            //printf("\nhouve promoção da inserção / pag.cont > ordem");
            printf("\n vai entrar no divide, com contachaves valendo %d!", pag.contachaves);
            divide(chv_pro,rrn_pro,&pag,chave_pro,filho_d_pro,&novapag);
            escreve_pagina(rrn_atual,pag);//escreva PAG no arquivo em RRN_ATUAL
            escreve_pagina(*filho_d_pro,novapag);//escreva NOVAPAG no arquivo em FILHO_D_PRO
            return promocao;
        }
    }
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

int pega_input(FILE *txt){
    int input;

    if(fscanf(txt,"%d|",&input) != EOF){
       return input;
    }
    return -1;
    //return convertido;
}

void imprimir_pag(FILE *Btree,int rrn){
    PAGINA pag;
    int byteoffset = sizeof(int)+(rrn*sizeof(PAGINA));
    fseek(Btree,byteoffset,SEEK_SET);
    fread(&pag,sizeof(PAGINA),1,Btree);
    printf("\nPagina : %d",rrn);
    printf("\nChaves: ");
    for(int i=0;i<ordem-1;i++){
        printf("%d |",pag.chave[i]);
    }
    printf("\nFilhos: ");
    for(int i=0;i<ordem;i++){
        printf("%d |",pag.filho[i]);
    }
}

void gerenciador(FILE *Btree,char *arq){
    FILE *txt = fopen(arq,"r+b");
    PAGINA novapag;
    int raiz,filho_d_pro,chave_pro,rrn,c=0,chave;
    raiz = 0;
    printf("\n??");
    imprimir_pag(Btree,0);
    inicializa_pagina(&novapag);
    escreve_pagina(raiz,novapag);
    imprimir_pag(Btree,0);
    fwrite(&raiz,sizeof(int),1,Btree);
    //fwrite(&novapag,sizeof(PAGINA),1,Btree);
    printf("\noi3");

    while((chave = pega_input(txt)) != -1){
        printf("\n valor da chave = %d",chave);
        if(insere(raiz,chave,&filho_d_pro,&chave_pro) == promocao){

            printf("\ndentro do ifzada");
            inicializa_pagina(&novapag);
            novapag.chave[0] = chave_pro;  //filho esquerdo  
            novapag.filho[0] = raiz;
            novapag.filho[1] = filho_d_pro; //filho direito 
            escreve_pagina(raiz,novapag);
            rrn = RRN_novapag();
            raiz = rrn; //faça RAIZ receber o RRN de NOVAPAG??
            printf("\noi5");
        }
        imprimir_pag(Btree,c);
        printf("\noi6");
        fseek(Btree,sizeof(int),SEEK_CUR);
        fread(&chave,sizeof(int),1,Btree);
    }
    fwrite(&raiz,sizeof(int),1,Btree);
    printf("\nend gerencia");
    imprimir_pag(Btree,c);
    c++;
    imprimir_pag(Btree,c);
    c++;
    imprimir_pag(Btree,c);
    fclose(Btree);
    fclose(txt);
}  

void criar_arvB(char *arq){
    FILE *Btree;
    //FILE *txt = fopen(arq,"r+b");
    //PAGINA novapag;
   // int chave,raiz = 0;
    Btree = fopen("Btree.dat","w+b");
    gerenciador(Btree,arq);
    printf("\nhello valentas");
    fclose(Btree);
}

/* void chaves_em_ordem_crescente(char *arq){

} */

int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {

        printf("Modo de criação da árvore-B ... nome do arquivo = %s\n", argv[2]);
        criar_arvB(argv[2]);
        //gerenciador();
   

    } else if (argc == 3 && strcmp(argv[1], "-p") == 0) {

        printf("Impressão das informações da árvore-B ... nome do arquivo = %s\n", argv[2]);
        

    } else if (argc == 3 && strcmp(argv[1], "-k") == 0) {

        printf("impressão das chaves em ordem crescente (percurso na árvore-B) ...\n");
        //chaves_em_ordem_crescente(argv[2]);
    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-c|-k) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);

    }

    return 0;
}

