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

    printf("NOVO RRN = %d",(byteoffset - tamanhocab)/tamanhopag);
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
    printf("\nCONTA CHAVES ESCREVE o bendito numero %d", pag.chave[1]);
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
    pag->filho[ordem-1] =  -1;//NULL
}

void insere_na_pagina (int chave, int filho_d,PAGINA *pag){
    int i = pag->contachaves;
    while(i > 0 && chave < pag->chave[i-1]){
        pag->chave[i] = pag->chave[i-1];
        pag->filho[i+1] = pag->filho[i];
        i--;
    }
    printf("\nAntes ++%d",pag->contachaves);
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
        printf("filho-> %d", pagaux.filho[i]);
    }
    pagaux.filho[ordem-1] = pag->filho[ordem-1];
    printf("filho-> %d", pagaux.filho[ordem-1]);
    return pagaux;
}

void divide(int chave,int filho_d,PAGINA *pag, int *chave_pro,int *filho_d_pro,PAGINA *novapag){
    PAGINA pagaux;
    pagaux = copiar_pag(pag);//copie PAG para PAGAUX 
    int vet[ordem],vet1[ordem+1]; 
    /* Insira CHAVE e FILHO_D nos lugares apropriados em PAGAUX */
    printf("\nola, estou dentro da desgraça do divide");
    int i = ordem-1;
    printf("\nchave dentro do divide: %d\n",chave);

    for(int i=0;i<ordem-1;i++){
        vet[i] = pag->chave[i];
        vet1[i] = pag->filho[i];
    }
    vet1[ordem-1] = pag->filho[ordem-1];
    for(int i=0; i<ordem-1;i++){
        printf("\nvet[%d] = %d", i, vet[i]);
    }

    //vet1[ordem] = pag->filho[ordem];
    while(chave < pagaux.chave[i-1] && i>0){
        vet[i] = vet[i-1];
        //printf("\npagaux[%d] = %d",i,pagaux.chave[i]);
        vet1[i+1] = vet1[i];
        //printf("\npagaux[%d] = %d",i,pagaux.filho[i]);
        i--;
    }

    
    

   
    vet[i] = chave;
    
    // pagaux.filho[i] = -1;
    vet1[i+1] = filho_d;
    
    // printf("filho %d", filho_d);
    

    int meio = (ordem)/2;
    int rrn;
    *filho_d_pro = RRN_novapag(); //faça FILHO_D_PRO receber o RRN que a NOVAPAG terá no arquivo árvore-b 
    *chave_pro = vet[meio];
    /* Copie as chaves e ponteiros que vêm depois de CHAVE_PRO para NOVAPAG */
    i = 0;
    inicializa_pagina(pag);
    while(i<meio){
        pag->chave[i]= vet[i];
        printf("\npag->chave: %d",pag->chave[i]);
        pag->filho[i]= vet1[i];
        pag->contachaves++;
        i++;
    }
    printf("\nCONTA CHAVES DESSA POHA %d", pag->contachaves);
    

    // printf("\npag[%d] = %d",i,pag->chave[i]);
    // printf("\npag[%d] = %d",i,pag->filho[i]);

    pag->filho[i] = vet1[i];
    inicializa_pagina(novapag);
    i = meio+1;

    //printf("CONTA CHAVES DESSA OUTRA  POHA %d ANTES\n", novapag->contachaves);
    while(i<ordem){
        novapag->chave[novapag->contachaves]= vet[i];
        //printf("\nnovapag->chave: %d",novapag->chave[i]);
        novapag->filho[novapag->contachaves] = vet1[i];
        novapag->contachaves++;
        i++;
    }
    novapag->filho[novapag->contachaves] = vet1[ordem];

    for(int j=0; j<novapag->contachaves;j++){
        printf("\nnovapag[%d] = %d", j, novapag->chave[j]);
    }

    //printf("CONTA CHAVES DESSA OUTRA  POHA %d", novapag->contachaves);

  /*   for(int j=0; j<ordem; j++){
        printf("\nCURIThaN %d", novapag->filho[j]);
    } */

    //printf("\nnovapagchave[%d] = %d",novapag->contachaves,novapag->chave[novapag->contachaves]);
    //printf("\nnovapagfilho[%d] = %d",novapag->contachaves,novapag->filho[novapag->contachaves]);
    
    // novapag->filho[novapag->contachaves] = vet1[i];
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
        
        // system("pause");
        result = busca_na_pagina(chave, pag,&pos);
    }
    if(result == encontrado){
        printf("\nChave duplicada");
        return erro;
    }
    int rrn_pro=0,chv_pro=0; //n sei ta certo isso !!!
    // printf("\nRRN DO FILHO %d", pag.filho[pos]);
    // system("pause");
    
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
            //printf("\nrrn_atual = %d, chave_pro= %d, chave = %d,conta chaves = %d, rrn_pro= %d",rrn_atual,*chave_pro,chave,pag.contachaves, rrn_pro );
            

            insere_na_pagina(chv_pro,rrn_pro,&pag);//insira CHV_PRO e RRN_PRO (chave promovida e filha) em PAG
            printf("\nConta Chaves pos insere na pag = %d",pag.contachaves);
            escreve_pagina(rrn_atual,pag);//escreva PAG no arquivo em RRN_ATUAL
            return sem_promocao;
        }
        else{
            //printf("\nhouve promoção da inserção / pag.cont > ordem");
            //printf("\n vai entrar no divide, com contachaves valendo %d!", pag.contachaves);
            // printf("\n\nrrnpro %d  ", chv_pro);
            
            for(int j=0; j<ordem; j++){
                printf("%d\n",pag.filho[j]);
            }
            divide(chv_pro,rrn_pro,&pag,chave_pro,filho_d_pro,&novapag);
            //printf("\nrrn_atual: %d",rrn_atual);
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
    return -1; // Fim do Arquivo !!!
}

void imprimir_pag(FILE *Btree,int rrn){
    PAGINA pag;
    int raiz;
    int byteoffset = sizeof(int)+(rrn*sizeof(PAGINA));
    fseek(Btree,byteoffset,SEEK_SET);
    fread(&pag,sizeof(PAGINA),1,Btree);
    printf("\n\nPagina : %d",rrn);
    printf("\nChaves: ");
    for(int i=0;i<ordem-1;i++){
        printf("%d |",pag.chave[i]);
    }
    printf("\nFilhos: ");
    for(int i=0;i<ordem;i++){
        printf("%d |",pag.filho[i]);
    }
}

void imprime_informacao(){
    int i =0;
    int rrns,raiz;
    FILE *Btree = fopen("Btree.dat","r");
    if(Btree == NULL){
        printf("\nO arquivo Btree.dat nao existe !!!");
        exit(EXIT_FAILURE);
    }
    rrns = RRN_novapag();
    fseek(Btree,0,SEEK_SET);
    fread(&raiz,sizeof(int),1,Btree);
    while(i<rrns){
        if(i != raiz){
            imprimir_pag(Btree,i);
            i++;
        }
        else{
            printf("\n\n\n- - - - Pagina Raiz - - - -");
            imprimir_pag(Btree,i);
            printf("\n- - - - - - - - - - - - - -");
            i++;
        }
    }
    fclose(Btree);
    //while(imprimir_pag)
}

void atualiza_cabecalho(int raiz,FILE *Btree){
    fseek(Btree,0,SEEK_SET);
    fwrite(&raiz,sizeof(int),1,Btree);
}

void gerenciador(FILE *Btree,char *arq){
    FILE *txt = fopen(arq,"r+b");
    PAGINA novapag;
    int raiz,filho_d_pro=0,chave_pro=0,rrn,c=0,chave;
    raiz = 0;
    //printf("\n??");
    imprimir_pag(Btree,0);
    inicializa_pagina(&novapag);

    escreve_pagina(raiz,novapag);
    imprimir_pag(Btree,0);
    fwrite(&raiz,sizeof(int),1,Btree);
    //fwrite(&novapag,sizeof(PAGINA),1,Btree);
    //printf("\noi3");

    chave = pega_input(txt);
    while(chave != -1){
        printf("\n valor da chave a ser inserida = %d",chave);

        if(insere(raiz,chave,&filho_d_pro,&chave_pro) == promocao){

            printf("\ndentro do ifzada");
            inicializa_pagina(&novapag);
            
            novapag.chave[0] = chave_pro;  //filho esquerdo
            novapag.filho[0] = raiz;
            novapag.filho[1] = filho_d_pro; //filho direito 
            
            rrn = RRN_novapag();
            raiz = rrn; //faça RAIZ receber o RRN de NOVAPAG
            novapag.contachaves++; // VAI TOMAR NO CU CARALHO, 26 gay !!!
            escreve_pagina(raiz,novapag);
            printf("\noi5");
        }
        imprimir_pag(Btree,raiz);
        //printf("\noi6");
        // fseek(Btree,sizeof(int),SEEK_CUR);
        // fread(&chave,sizeof(int),1,Btree);
        chave = pega_input(txt);
    }
    atualiza_cabecalho(raiz,Btree);
    //printf("\nend gerencia");
    fclose(Btree);
    fclose(txt);
}  

void criar_arvB(char *arq){
    FILE *Btree;
    Btree = fopen("Btree.dat","w+b");
    gerenciador(Btree,arq);
    fclose(Btree);
}

void Printar_em_Ordem_cres (int rrn) {
    if(rrn<0){
        //printf("\n returnzada");
        return;
    }
    PAGINA *pag;
    le_pagina(rrn,pag);
    //printf("\noi john 2");
    for(int i=0;i<pag->contachaves;i++){
        //printf("\noi john 3");
        Printar_em_Ordem_cres(pag->filho[i]);
        printf("%d |",pag->chave[i]);
    }
    //printf("\noi john 4");
    Printar_em_Ordem_cres(pag->filho[pag->contachaves]);
}

void Ordem_das_chaves(){
    FILE *Btree = fopen("Btree.dat","r");
    int raiz;
    if(Btree == NULL){
        printf("\nO arquivo Btree.dat nao existe !!!");
        exit(EXIT_FAILURE);
    }
    //printf("\noi john");
    fseek(Btree,0,SEEK_SET);
    fread(&raiz,sizeof(int),1,Btree);
    //printf("\noi john 5");
    Printar_em_Ordem_cres(raiz);
}

/* void emOrder(FILE *Btree, PAGINA* raiz){
    if(raiz == NULL) return;
    PAGINA pag;
    if (raiz != NULL){
        for(int i=0; i<raiz->contachaves; i++){
            if(i==0){
                int byteoffset = sizeof(int)+(raiz->filho[i]*sizeof(PAGINA));
                fseek(Btree,byteoffset,SEEK_SET);
                fread(&pag,sizeof(PAGINA),1,Btree);
                emOrder(Btree, &pag);
                printf(" %d ", raiz->chave[i]);
            }

            int byteoffset = sizeof(int)+(raiz->filho[i+1]*sizeof(PAGINA));
            fseek(Btree,byteoffset,SEEK_SET);
            fread(&pag,sizeof(PAGINA),1,Btree);
            emOrder(Btree, &pag);
        }
    }
} */

int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {

        printf("Modo de criação da árvore-B ... nome do arquivo = %s\n", argv[2]);
        criar_arvB(argv[2]);
        //gerenciador();
   

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Impressão das informações da árvore-B ...\n");
        imprime_informacao();

    } else if (argc == 2 && strcmp(argv[1], "-k") == 0) {

        printf("impressão das chaves em ordem crescente (percurso na árvore-B) ! ...\n");
        Ordem_das_chaves();
        
    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s -c nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s (-p|-k)\n", argv[0]);
        exit(EXIT_FAILURE);

    }

    return 0;
}

