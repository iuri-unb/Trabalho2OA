/* 
Data: 15/12/2017

Alunos: 
Iuri Sousa Vieira - 16/0152488
Evandro Thalles Vale de Castro - 16/0119286
Vitor Ribas Bandeira - 16/0148421

Trabalho 2 da Disciplina de Organização de Arquivos ministrado pelo Doutor Flavio Barros Vidal
Universidade de Brasilia - UnB

Codigo Compilado usando: gcc -Wall -ansi nomedoarquivo.c

Testado somente em Distribuições Linux

Foi encontrado um BUG na operação de alterar quando se utiliza uma versão do GCC desatualizada,
funciona normalmente na versao 7.2, que é a mais atual!


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ORDEM 271 
/*Para o calculo da Ordem da árvore, usou o conteúdo explanado em sala de aula que foi
 d<= log(ordem/2) |Num.Registros/2| +1 ,para d que é a profundidade ,queremos que qualquer
 conteúdo da arvore seja acessado no máximo em 3 seeks,logo definimos d=3,
ao fazer as contas resultou em aproximadamente 271 */


typedef struct node {
    int n; /* n < ORDEM Numero de chaves em uma pagina devera ser sempre menor que a ORDEM da B-tree*/
    int chaves[ORDEM-1]; /* vetor de chaves*/
    int nivel;
    int pagina;
    struct node *p[ORDEM]; /* (n+1 ponteiros serao usados), Seus filhos */
        
    struct node *prox;

}node;

struct node *root = NULL;

typedef struct Lista
{
  
  int chave_primaria;

  struct Lista *prox;
  struct Lista *ant;

}Lista;

typedef struct aponta
{
  struct Lista *inicio;
  struct Lista *fim;
  int quantidade;
}aponta;

enum FlagsChaves { Duplicado,BuscaFalhou,Successo,Insere,PoucaChave };

void inserir(int chave);
void exibir(struct node *root,int);
void DelNode(int x);
void pesquisar(int x);
enum FlagsChaves ins(struct node *r, int x, int* y, struct node** u);
int pesquisaPos(int x,int *key_arr, int n);
enum FlagsChaves del(struct node *r, int x);
void eatline(void);
void exibe_ordenado(struct node *ptr);
void separa_chave(FILE * fp, aponta *elem);
void altera(int chave , int nova_chave);
int verifica_chave(int chave);
void indice(node *elem);
int max(int first, int second, int third);
int maxLevel(struct node *ptr);
void ExibeNivel(struct node *ptr);
void arvByIndice();
Lista *aloca();
aponta *Cria_Lista();
int vazia (aponta *elem);
int insere_fim(aponta *elem, int chave_primaria);
void verifIntegridade(aponta *elem);
void InsereArvB(aponta *elem);
void atualiza_arq_dados (int chave_primaria , char * novo_registro , int muda_chave);
void escolha_altera (int chave_primaria);
int insere_arqDados(int chave);
void imprimeArqDados();
void Busca_Registro_ArquivoDados(int chave_primaria);

int main(){
    FILE* fp;
    struct node *aux;
    int a, n1, pos1;

    aponta *Lista = Cria_Lista();

    if((fp = fopen("indicelista.bt", "r")) == NULL){ /* Verifica se existe o arquivo de Indice */
        fp = fopen("data.txt", "r");
        printf("Carregando informações...\n");

        separa_chave(fp, Lista);
        verifIntegridade(Lista);
        InsereArvB(Lista);
        free(Lista);

        printf("\nArvore Criada a partir do arquivo de Dados!\n");
        
        indice(root);
        printf("Arquivo de Indices criado!\n");

    }else /* Caso exista o arquivo de Indice */
    {
        arvByIndice();
        printf("\nArvore Criada a partir do arquivo de Indices!\n");
    }
    

    int chave, opcao, valida;
    
    printf("\nB-tree com ordem %d\n",ORDEM);
    while(1)
    {

        printf("\n1.Inserir\n");
        printf("2.Excluir\n");
        printf("3.Pesquisar\n");
        printf("4.Alterar\n");
        printf("5.Exibir\n");
        printf("0.Sair\n");
        
        printf("Digite uma opcao: ");
        scanf("%d",&opcao); eatline();

        switch(opcao)
        {
            case 1:
            valida = 0;
                indice(root);
                imprimeArqDados();
                while(valida != 1){ /* Enquanto a chave digitada nao for valida */
                    printf("\nDigite a chave que deseja inserir: ");
                    scanf("%d",&chave); eatline();  
                    valida = verifica_chave(chave);
                }
                
                valida = insere_arqDados(chave);
                if (valida == 2)
                {
                    inserir(chave); 
                    indice(root); 
                    imprimeArqDados();
                    /* Mostrando o caminho percorrido até aonde chave sera inserida */
                    aux = root;
                    while(aux){
                        n1 = aux->n;
                        printf("\nFilhos:\t");
                        for (a=0; a < aux->n; a++)
                            printf(" %d",aux->chaves[a]);
                    
                        printf("\n");
                        pos1 = pesquisaPos(chave, aux->chaves, n1);
                        if (pos1 < n1 && chave == aux->chaves[pos1])
                        {
                            printf("\nChave %d inserida na posicao %d do ultimo no mostrado!\n",chave,a);
                        }
                        aux = aux->p[pos1];
                    }

                }

                break;
            case 2:
            valida = 0;
            indice(root);
            imprimeArqDados();
                while(valida != 1){ /* Enquanto a chave digitada nao for valida */
                    printf("\nDigite a chave que deseja excluir: ");
                    scanf("%d",&chave); eatline();  
                    valida = verifica_chave(chave);
                }

                /* Mostrando o caminho percorrido até a chave que sera excluida */
                aux = root;
                while(aux){
                    n1 = aux->n;
                    printf("\nFilhos:\t");
                    for (a=0; a < aux->n; a++)
                        printf(" %d",aux->chaves[a]);
                
                    printf("\n");
                    pos1 = pesquisaPos(chave, aux->chaves, n1);
                    if (pos1 < n1 && chave == aux->chaves[pos1])
                    {
                        printf("\nChave %d a ser excluída encontrada na posicao %d do ultimo no mostrado!\n",chave,a);
                        atualiza_arq_dados(chave, "excluindo", 3);
                        DelNode(chave);
                        indice(root);
                        imprimeArqDados();
                        valida = 0;
                    }
                    aux = aux->p[pos1];
                }
                if (valida == 1)
                {
                    printf("\nChave nao encontrada!\n");
                }
                
                break;
            case 3:
            valida = 0;
                while(valida != 1){ /* Enquanto a chave digitada nao for valida */
                    printf("\nDigite a chave que deseja pesquisar: ");
                    scanf("%d",&chave); eatline();
                    valida = verifica_chave(chave); 
                }

                pesquisar(chave);
                break;
            case 4: /* Caso quando deseja alterar algum registro */
            valida = 0;
            indice(root);
            imprimeArqDados();
                while(valida != 1)
                { /* Enquanto a chave digitada nao for valida */
                    printf("\nDigite a chave que deseja alterar: ");
                    scanf("%d",&chave); eatline();
                    valida = verifica_chave(chave); 
                }

                escolha_altera(chave);
                break; 
            case 5:
                printf("\nB-tree:\n");
                exibir(root,0);
                printf("\nNivel da arvore:");
                ExibeNivel(root);
                break;           
            case 0:
                exit(1);
            default:
                printf("\nOpcao invalida!\n");
                break;
        }
    }
    fclose(fp);
    return 0;
}

/* Funcao que separa a chave dos outros campos do registro */
void separa_chave(FILE * fp, aponta *elem)
{
    int k=0;
    char registro[400], aux[7];
    int chave = 0;

    while(!feof(fp))
    {
        fscanf(fp, "%s", registro); /* Le 1 registro de cada vez */     
        strncpy(aux, registro, 6); /* Copia os 6 primeiros caracteres do registro para a variavel aux */
        
        if( atoi(aux) != 0 ){
            chave = atoi(aux); /* Atoi transforma aux em numero inteiro */
            insere_fim(elem, chave); /* Insere as chaves na lista */
            
            k++;
            /*if(k == 20)
                break; */
        }        
    }
    printf("Quantidade de registros: %d\n",k );
    fclose(fp);
}

/*Funcao que insere os elementos na b-tree */
void inserir(int chave)
{
    struct node *newnode;
    int sobe_chave;
    enum FlagsChaves status;
    status = ins(root, chave, &sobe_chave, &newnode);

    if (status == Duplicado)
        printf("Chave já usada\n");

    if (status == Insere)
    {
        struct node *sobe_raiz = root;
        root = malloc(sizeof(struct node));
        root->n = 1;
        root->chaves[0] = sobe_chave;
        root->p[0] = sobe_raiz;
        root->p[1] = newnode;
    }
}

/* Funcao que realiza as operacoes necessarias na hora de inserir e retorna um status */
enum FlagsChaves ins(struct node *ptr, int chave, int *sobe_chave, struct node **newnode)
{
    /* n = Numero de chaves armazenadas no noh */
    struct node *novoPtr, *antPtr;
    int pos, i, n, splitPos;
    int novaChave, antChave;
    enum FlagsChaves status;
    
    /*Se o bloco esta vazio , faca */
    if (ptr == NULL)
    {
        *newnode = NULL;
        *sobe_chave = chave;
        
        /*(*newnode)->nivel = 0;
        (*newnode)->pagina = 0;*/
        return Insere;
    }
    n = ptr->n;

    /* Chave = valor do arq , ptr->chaves = todas as chaves contidas no noh e n = quantidade de chaves naquele noh */
    pos = pesquisaPos(chave, ptr->chaves, n);
   
    if (pos < n && chave == ptr->chaves[pos])
        return Duplicado;

    status = ins(ptr->p[pos], chave, &novaChave, &novoPtr);
    if (status != Insere)
        return status;
    /*Se as quantidades das chaves no Nó são menores que ORDEM-1*/
    if (n < ORDEM - 1)
    {
      
        /*Deslocando a chave e o ponteiro para a direita para inserir a nova chave*/
        for (i=n; i>pos; i--)
        {
            ptr->chaves[i] = ptr->chaves[i-1];
            ptr->p[i+1] = ptr->p[i];             
        }

        /*Chave foi inserida no local certo*/
        ptr->chaves[pos] = novaChave;
        ptr->p[pos+1] = novoPtr;
        ++ptr->n; /*Incrementando o numero de chaves no nó*/
        return Successo;

    }

    /*Se o nó esta cheio e a posição do nó a ser inserido é a ultima*/
    if (pos == ORDEM - 1)
    {
        antChave = novaChave;
        antPtr = novoPtr;
    }
    else /*Se o nó esta cheio e a posição do nó a ser inserido não é a ultima*/
    {
        antChave = ptr->chaves[ORDEM-2];
        antPtr = ptr->p[ORDEM-1];
        for (i=ORDEM-2; i>pos; i--)
        {
            ptr->chaves[i] = ptr->chaves[i-1];
            ptr->p[i+1] = ptr->p[i];
        }
        ptr->chaves[pos] = novaChave;
        ptr->p[pos+1] = novoPtr;
    }

    splitPos = (ORDEM - 1)/2;
    (*sobe_chave) = ptr->chaves[splitPos];

    (*newnode)=malloc(sizeof(struct node));/*Nó direito depois do split*/
    ptr->n = splitPos; /*Numero de chaves do lado esquerdo do nó que foi feito o split*/
    (*newnode)->n = ORDEM-1-splitPos;/*Numero de chaves do lado direito do nó que foi feito o split*/
    for (i=0; i < (*newnode)->n; i++)
    {
        (*newnode)->p[i] = ptr->p[i + splitPos + 1];
        if(i < (*newnode)->n - 1)
            (*newnode)->chaves[i] = ptr->chaves[i + splitPos + 1];
        else
            (*newnode)->chaves[i] = antChave;
    }

    (*newnode)->p[(*newnode)->n] = antPtr;
    return Insere;
}

/* Funcao que exibe a arvore na tela */
void exibir(struct node *ptr, int em_branco)
{
    if (ptr)
    {
        int i;
        for(i=1; i<=em_branco; i++)
            printf(" ");

        printf("Filhos: \t");
        for (i=0; i < ptr->n; i++)
            printf("%d ",ptr->chaves[i]);
        printf("\n");
        for (i=0; i <= ptr->n; i++)
            exibir(ptr->p[i], em_branco+10);
    }
}

/* Funcao que pesquisa uma chave na arvore */
void pesquisar(int chave)
{
    int pos, i, n;
    struct node *ptr = root;
    printf("Percorrendo a B-tree:\n");
    while (ptr)
    {
        n = ptr->n;
        printf("\nFilhos:\t");
        for (i=0; i < ptr->n; i++)
            printf(" %d",ptr->chaves[i]);

        printf("\n");
        pos = pesquisaPos(chave, ptr->chaves, n);
        
        if (pos < n && chave == ptr->chaves[pos])
        {
            Busca_Registro_ArquivoDados(chave);
            printf("\nChave %d encontrada na posicao %d do ultimo no mostrado!\n",chave,i);
            return;
        }
        ptr = ptr->p[pos];
    }
    printf("\nChave %d nao encontrada na arvore\n",chave);
}

/* Funcao que pesquisa por uma chave na arvore e retorna sua posicao */
int pesquisaPos(int chave, int *a_chave, int n)
{
    int pos=0;
    while (pos < n && chave > a_chave[pos])
        pos++;
    return pos;
}

/* Funcao que exclui uma chave na arvore */
void DelNode(int chave)
{
    struct node *sobe_raiz;
    enum FlagsChaves status;
    status = del(root,chave);
    switch (status)
    {
    case BuscaFalhou:
        printf("Chave %d nao encontrada na arvore\n",chave);
        break;
    case PoucaChave:
        sobe_raiz = root;
        root = root->p[0];
        free(sobe_raiz);
        break;
    }
    
}

/* Funcao que realiza as operacoes necessarias depois de excluir uma chave e retorna um status */
enum FlagsChaves del(struct node *ptr, int chave)
{
    int pos, i, pivot, n ,min;
    int *a_chave;
    enum FlagsChaves status;
    struct node **p,*esq_ptr,*dir_ptr;

    if (ptr == NULL)
        return BuscaFalhou;

    /*Atribui valores ao nó*/
    n = ptr->n;
    a_chave = ptr->chaves;
    p = ptr->p; /*p recebe os filhos de ptr */
    min = (ORDEM - 1)/2;/*Numero minimo de chaves*/

    /*Pesquisa a chave para excluir*/
    pos = pesquisaPos(chave, a_chave, n);
    
    /*Se p é uma folha */
    if (p[0] == NULL)
    {
        if (pos == n || chave < a_chave[pos])
            return BuscaFalhou;
        
        /*Desloca chaves e ponteiros para a esquerda*/
        for (i=pos+1; i < n; i++)
        {
            a_chave[i-1] = a_chave[i];
            p[i] = p[i+1];
        }
        return --ptr->n >= (ptr==root ? 1 : min) ? Successo : PoucaChave;
    }

    /*Se achou a chave e p não é uma folha*/
    if (pos < n && chave == a_chave[pos])
    {
        struct node *qp = p[pos], *qp1;
        int nchave;
        while(1)
        {
            nchave = qp->n;
            qp1 = qp->p[nchave];
            if (qp1 == NULL)
                break;
            qp = qp1;
        }
        a_chave[pos] = qp->chaves[nchave-1];
        qp->chaves[nchave - 1] = chave;
    }

    status = del(p[pos], chave);
    if (status != PoucaChave)
        return status;

    if (pos > 0 && p[pos-1]->n > min)
    {
        pivot = pos - 1; /*Pivô para o nó esquerdo e direito*/
        esq_ptr = p[pivot];
        dir_ptr = p[pos];
        /*Atribuindo valores pro nó direito*/
        dir_ptr->p[dir_ptr->n + 1] = dir_ptr->p[dir_ptr->n];
        for (i=dir_ptr->n; i>0; i--)
        {
            dir_ptr->chaves[i] = dir_ptr->chaves[i-1];
            dir_ptr->p[i] = dir_ptr->p[i-1];
        }
        dir_ptr->n++;
        dir_ptr->chaves[0] = a_chave[pivot];
        dir_ptr->p[0] = esq_ptr->p[esq_ptr->n];
        a_chave[pivot] = esq_ptr->chaves[--esq_ptr->n];
        return Successo;
    }

    /*Se (pos->n > min)*/
    if (pos < n && p[pos + 1]->n > min)
    {
        pivot = pos; /*Pivô para o nó esquerdo e direito*/
        esq_ptr = p[pivot];
        dir_ptr = p[pivot+1];
        /*Atribuindo valores pro nó esquerdo*/
        esq_ptr->chaves[esq_ptr->n] = a_chave[pivot];
        esq_ptr->p[esq_ptr->n + 1] = dir_ptr->p[0];
        a_chave[pivot] = dir_ptr->chaves[0];
        esq_ptr->n++;
        dir_ptr->n--;
        for (i=0; i < dir_ptr->n; i++)
        {
            dir_ptr->chaves[i] = dir_ptr->chaves[i+1];
            dir_ptr->p[i] = dir_ptr->p[i+1];
        }

        dir_ptr->p[dir_ptr->n] = dir_ptr->p[dir_ptr->n + 1];
        return Successo;
    }

    if(pos == n)
        pivot = pos-1;
    else
        pivot = pos;

    esq_ptr = p[pivot];
    dir_ptr = p[pivot+1];

    /*merge nó direito com o nó esquerdo*/
    esq_ptr->chaves[esq_ptr->n] = a_chave[pivot];
    esq_ptr->p[esq_ptr->n + 1] = dir_ptr->p[0];
    for (i=0; i < dir_ptr->n; i++)
    {
        esq_ptr->chaves[esq_ptr->n + 1 + i] = dir_ptr->chaves[i];
        esq_ptr->p[esq_ptr->n + 2 + i] = dir_ptr->p[i+1];
    }
    esq_ptr->n = esq_ptr->n + dir_ptr->n +1;
    free(dir_ptr); /*Remove nó direito*/
    for (i=pos+1; i < n; i++)
    {
        a_chave[i-1] = a_chave[i];
        p[i] = p[i+1];
    }
    return --ptr->n >= (ptr == root ? 1 : min) ? Successo : PoucaChave; /* Retorna um status */
}

/* Funcao feita para evitar problemas com o enter */
void eatline(void) {
  char c;
  printf("");
  while ((c=getchar())!='\n') ;
}

/* Funcao que altera uma chave na B-tree */
void altera(int chave , int nova_chave){
    int pos, i, n;
    struct node *ptr = root;
    printf("Percorrendo a B-tree:\n");
    while (ptr)
    {
        n = ptr->n;
        printf("\nFilhos:\t");
        for (i=0; i < ptr->n; i++)
            printf(" %d",ptr->chaves[i]);
        
        printf("\n");
        /* Pesquisa a posicao da chave na arvore */
        pos = pesquisaPos(chave, ptr->chaves, n);
        
        /* Se a posicao for menor que a quantidade de chaves naquela pagina e a chave que vai ser alterada
        eh igual a daquela posicao */
        if (pos < n && chave == ptr->chaves[pos]) 
        {
            printf("\nChave %d encontrada na posicao %d do ultimo no mostrado!\n",chave,i);
            DelNode(chave);
            inserir(nova_chave);
            indice(root);
            return;
        }
        ptr = ptr->p[pos];
    }
    printf("\nChave %d nao existe\n",chave);
}

/* Funcao que verifica se a chave eh valida */
int verifica_chave(int chave){
    if((chave < 100000) || (chave > 999999)){
        printf("\nChave Invalida\n");
        return 5;
    }
     return 1;
}

/* Funcao que cria e atualiza o arquivo de Indices */
void indice(node *elem)
{
    int j=0, n=0, n1=0, n2=0, n3=0;

    FILE* fp;
    fp = fopen("indicelista.bt", "w");

    if(fp == NULL){
        printf("\nFalha ao abrir o arquivo!\n");

    }else{  
        fprintf(fp, "Raizes:\n");
        while(&(root->chaves[n3]) != NULL && n3 < root->n){ /*Percorre a raiz e imprime na tela e no arq de Indice */
            printf("Raizes: %d\n", root->chaves[n3]);
            fprintf(fp, "%d\n", root->chaves[n3]);
            n3++;        
        }            
        while(elem->p[n] != NULL){ /* Percorre todas as paginas e imprime na tela e no arq dados */
            printf("\n-------------------------------------\n");
            printf("Pagina %d:\n", n);
            fprintf(fp, "\nPagina%d:\n", n);
            for(n1=0; n1 < elem->p[n]->n; n1++){
                printf("%d\n", elem->p[n]->chaves[n1]);
                fprintf(fp, "%d\n", elem->p[n]->chaves[n1]);
                n2++;
            }
            n++;
            j++;

            if (elem->p[n] == NULL)
            {   
                elem = (*elem->p);
                n=0;
            }
        }

    }

    fclose(fp);
    
}

/* Retorna o nivel maximo */
int max(int first, int second, int third) {
    int max = first;
    if (second > max) max = second;
    if (third > max) max = third;
    return max;
}

/* Funcao que retorna o nivel max da esquerda, do meio e da direita da arvore */
int maxLevel(struct node *ptr) {
    if (ptr) {
        int l = 0, mr = 0, r = 0, max_depth;
        if (ptr->p[0] != NULL) l = maxLevel(ptr->p[0]);
        if (ptr->p[1] != NULL) mr = maxLevel(ptr->p[1]);    
        if (ptr->n == 2) {
            if (ptr->p[2] != NULL) r = maxLevel(ptr->p[2]);
        }
        max_depth = max(l, mr, r) + 1;
        return max_depth;
    }
    return 0;
}

/* Exibe o nivel da arvore*/
void ExibeNivel(struct node *ptr) {
    int max = maxLevel(ptr) - 1;
    if (max == -1) printf("Arvore esta vazia!\n");
    else printf("%d\n", max+1);
}

/* Funcao que cria a arvore a partir do arq de Indice */
void arvByIndice(){
    FILE *fp = fopen("indicelista.bt", "r");
    char aux[10];
    int chave;
    char pags[7], raizes[7];

    strcpy(pags, "Pagina");
    strcpy(pags, "Raizes");

    if (fp == NULL)
    {
        printf("Falha ao tentar abrir o arquivo!\n");

    }else{   

        while(!feof(fp)){

            fscanf(fp, "%s", aux);

            if( strncmp(aux, pags, 6) == 0 ){
            }else if( strncmp(aux, raizes, 6) == 0 ){
            }else if( aux[0] == '\n' ){
            }else{
                chave = atoi(aux);
                printf("Chave inserida na B-tree: %d\n", chave );
                inserir(chave);
            }
        }
    }

    fclose(fp);
}

/* Função que aloca a Lista para verificar a integridade do arquivo*/
Lista *aloca(){
  Lista *novo = (Lista*)malloc(sizeof(Lista));
  if (novo !=NULL){
    return novo;
  }
  printf("ERRO NA ALOCACAO!\n");
  exit(1);
}
/* -------------------------------------------------------------- */

/* Função que aloca os ponteiros da Lista */
aponta *Cria_Lista()
{
  aponta *novo = (aponta *)malloc(sizeof(aponta));

  novo->inicio = NULL;
  novo->fim = NULL;
  novo->quantidade = 0;
  return novo;
}
/* -------------------------------------------------------------- */

/* Função que verifica se a Lista esta vazia */
int vazia (aponta *elem)
{

  if(elem == NULL)
  {/*Lista eh valida? */
    return 0;
  }

  else
  {
    return 1; /*Lista nao esta vazia */
  }

}
/* -------------------------------------------------------------- */

/* Função que insere os elementos na Lista */
int insere_fim(aponta *elem, int chave_primaria)
{

  Lista *novo = aloca();
  novo->prox = NULL;

  /* Inserindo as informações na Lista */
  
  novo->chave_primaria = chave_primaria;
  /* -------------------------------------------------------------- */

  if(elem->inicio == NULL)/*Se a lista esta vazia */
  {
    novo->prox=NULL;
    novo->ant = NULL;
    elem->inicio = novo;
    elem->fim = novo;
    elem->quantidade++;
  }
  else/* Lista não esta vazia*/
  {
    Lista *aux = elem->inicio;
    while(aux->prox !=NULL) /* Enquanto não chegar ao fim da lista */
    {
      aux = aux->prox;
    }    
    aux->ant = aux;
    aux->prox = novo;
    elem->quantidade++;
    elem->fim = novo;
    elem->fim->ant = aux; /* O anterior do ultimo elemento aponta para o penultimo */
  }

  return 1;
}

/* Função que exibe na tela todos os elementos da Lista */
void verifIntegridade(aponta *elem)
{
  Lista *aux = elem->inicio;
  Lista *aux2 = elem->inicio;
  int auxiliar;
  int i=0;
  for(; aux != NULL; aux = aux->prox ) /* Imprime todas as informações até chegar ao fim da Lista */
  {
    auxiliar = aux->chave_primaria;
    aux2 = aux;
    i=0;

    for(; aux2 != NULL; aux2 = aux2->prox )
    {

        if(auxiliar == aux2->chave_primaria && i>0)
        {
            printf("Uma chave esta sendo usada para dois ou mais registros no arquivo de dados!\n");
            printf("O programa sera finalizado!\n");
            exit(1);
        }
        i++;
    }       
  }
}

/* Insere na arvore a partir da lista */
void InsereArvB(aponta *elem){
    Lista *aux = elem->inicio;
    
    for(; aux != NULL; aux = aux->prox ) 
    {
        inserir(aux->chave_primaria);
        printf("Chave inserida na B-tree: %d\n", aux->chave_primaria);
    }
}

/*Função que irá atualizar somente o arquivo de dados, de acordo com a escolha do usuario
as opções foram definidas na função escolha altera que esta logo abaixo desta,mas também funciona
para caso o usuario deseja excluir um registro */
void atualiza_arq_dados (int chave_primaria , char * novo_registro , int muda_chave)
{
  FILE *arq_dados =  fopen("data.txt" , "r") ; /*Leitura e escrita */
  FILE *arq_dados2 = fopen("teste2.txt" , "w");
    int k=0;
    char registro[200], aux[7];
    int pos = 0, fim_arq = 0, chave = 0 ;
    int j;
    fseek(arq_dados, 0L, SEEK_END);
    fim_arq = ftell(arq_dados);
    fseek(arq_dados, 0, SEEK_SET);
    char string[500];
  char comando[50];
  int tamanho;
      
   if( muda_chave == 1)/* Quando quer alterar Todo o registro */
  {
    while(!feof(arq_dados))
      {
        j=0;
          if(pos >= fim_arq)
              break;
                
          

          fgets(registro, sizeof(registro), arq_dados);  
          strcpy(string , registro);
          while(registro[j] != '\0')
          {
            if(registro[j] == '\r')
            {
              printf("/r \n");
              printf("%d\n",j );
              registro[j] = '\n';
              j++;
            }
            
            else
            {
              j++;
            }
            
            
          } 
          registro[j] = '\0';

          strncpy(aux, registro, 6);
          chave = atoi(aux);       
          
          tamanho = strlen(registro);       
        
          if(chave == chave_primaria) /* Quando encontrar a chave a ser atualizada, realiza as devidas alteracoes no arq dados.*/
          {

            printf("%s\n",novo_registro );          
            fprintf(arq_dados2, "%s\n",novo_registro);/* Como o registro já está adequando com suas alterações, salve no arquivo */
              
          }
          else
          {
            printf("\n%s\n",registro ); 
            fprintf(arq_dados2, "%s",registro );
        }
          
          pos += tamanho;          
     
          fseek(arq_dados, pos, SEEK_SET);
          k++;
          
      }
  } 

  else /* Quando deseja mudar todo registro, com exceção da Chave primária */
  {
    while(!feof(arq_dados))
      {
          if(pos >= fim_arq)
              break;
          
          
          
          fgets(registro, sizeof(registro), arq_dados);  
          
          strncpy(aux, registro, 6);

          while(registro[j] != '\0')
          {
            if(registro[j] == '\r')
            {
              printf("/r \n");
              printf("%d\n",j );
              registro[j] = '\n';
              j++;
            }
            
            else
            {
              j++;
            }
            
            
          } 
          registro[j] = '\0';
          chave = atoi(aux);
          int tamanho =0;
          tamanho = strlen(registro);
          registro[tamanho] = '\0';

         
          if(chave == chave_primaria) /* Quando encontrar a chave a ser atualizada, realiza as devidas alteracoes no arq dados.*/
          {
                        
            if(muda_chave != 3) /* Caso a pessoa deseja excluir arquivo , apenas pule esse registro */
            {
              printf("%s\n",novo_registro );
              fprintf(arq_dados2, "%s\n",novo_registro);
            }
              

              
          }
          else
          {
            printf("%s\n",registro );
            fprintf(arq_dados2, "%s",registro );
        }
          
          pos += tamanho;
          
     
          fseek(arq_dados, pos, SEEK_SET);
          k++;
          
      }
  } 
    fclose(arq_dados);
    fclose(arq_dados2);

    strcpy( comando, "mv teste2.txt " ); /*Usa o mv ou ren... não esqueça de deixar um espaço no final*/
    strcat( comando, "data.txt" ); /* Agora temos o comando completo*/
    system( comando );
   
}



void escolha_altera (int chave_primaria) /*17 rcampos no total , ou seja, 17 virgulas */
{
  int   escolha,j=0,conta_virgulas=0 , registro_valido = 0, valida = 0, nchave=0;
  char novo_registro[300] , string[350] , nova_chave[7];
  printf("Digite 1 para mudar SOMENTE os campos do registro ,sem alterar a chave primaria!\n");
  printf("Digite 2 para mudar TODO o registro \n" );
  printf("Lembrete: Como sao 16 campos(sem contar com a chave primaria),deve conter 16 virgulas como separador de campos! \n");

  printf("Digite sua escolha:");
  scanf("%d",&escolha);
  getchar();

  switch(escolha)
  {
    case 1:   
      printf("Digite o novo registro,usando as virgulas(16 no total):");
      scanf("%[^\n]",novo_registro);
      getchar();

      /*Proximos loopings apenas para verificar se o registro novo é uma registro válido, ou seja, se tem 16 virgulas*/
      while(novo_registro[j] != '\0')
        {
          if(novo_registro[j] == ',')
          {
            conta_virgulas++;
          }
        j++;
        }
      printf("%d\n",conta_virgulas );
      
      if(conta_virgulas == 16)
      {
        registro_valido = 1;        
      }
      else
      {   /* Fazendo laço até ser inserido um registro válido possuindo 16 campos com suas virgulas */

        while( registro_valido != 1)
        {
          printf("Digite o novo registro,usando as virgulas(16 no total):\n");
          scanf("%[^\n]",novo_registro);
          getchar();
          j=0;
          conta_virgulas=0;
          while(novo_registro[j] != '\0')
          {
            if(novo_registro[j] == ',')
              conta_virgulas++;
          j++;
          
          }
          if(conta_virgulas == 16)
            registro_valido =1;
        }
      }

      sprintf(string, "%i,", chave_primaria); /* Convertendo a chave primaria para string*/
      strcat(string , novo_registro);
      printf("Novo registro:%s\n",string );
      atualiza_arq_dados(chave_primaria , string , 2); /* Opcao que nao altera a chave primaria , por isso contante = 2 */
      break;
    

    case 2:  
        while(valida != 1){
            printf("Digite a nova chave primaria,SEM virgula:\n");
            scanf("%[^\n]",nova_chave);
            nchave = atoi(nova_chave);
            valida = verifica_chave(nchave);
        }
      
      DelNode(chave_primaria);
      inserir(nchave);
      strcat(nova_chave,",");
      getchar();

      strcpy(string ,nova_chave);  /*Armazenando nessa string maior , para depois concatenar com o novo registro */
      printf("Digite o novo registro,usando as virgulas(16 no total):" );
      scanf("%[^\n]",novo_registro);
      getchar();
      /*Proximos loopings apenas para verificar se o registro novo é uma registro válido, ou seja, se tem 16 virgulas*/
      conta_virgulas =0;
      j=0;
      while(novo_registro[j] != '\0')
        {
          if(novo_registro[j] == ',')
            conta_virgulas++;
          
          j++;
        }
      if(conta_virgulas == 16)
      {
        registro_valido = 1;
      }
      else
      {   /* Fazendo laço até ser inserido um registro válido possuindo 16 campos com suas virgulas */

        while( registro_valido != 1)
        {
          printf("Digite o novo registro,usando as virgulas(16 no total):\n");
          scanf("%[^\n]",novo_registro);
          getchar();
          j=0;
          conta_virgulas=0;
          while(novo_registro[j] != '\0')
          {
            if(novo_registro[j] == ',')
              conta_virgulas++;
          j++;
          
          }
          if(conta_virgulas == 16)
            registro_valido =1;
        }
      }

      strcat(string , novo_registro);
      printf("Novo registro:%s\n",string );
      atualiza_arq_dados(chave_primaria , string , 1 ); /* Quando quer alterar todo o registro,por isso constante = 1 */
      indice(root);
      imprimeArqDados();
      break;


    default: printf("Opcao invalida!\n");
      
  }
}

/*Função que irá varrer todo o arquivo dados em busca da chave primaria digitada
para imprimir na tela Todo Registro inerente da mesma chave */
void Busca_Registro_ArquivoDados(int chave_primaria)
{   
    FILE * arq_dados = fopen("data.txt" , "r");

    int k=0;
    char registro[500], aux[7];
    int pos = 0, fim_arq = 0, chave = 0 , tamanho=0;

    fseek(arq_dados, 0L, SEEK_END);
    fim_arq = ftell(arq_dados);
    fseek(arq_dados, 0, SEEK_SET);
   
    while(!feof(arq_dados))
        {
            if(pos >= fim_arq)
                break;
                        
            
            fgets(registro, sizeof(registro), arq_dados);  

            strncpy(aux, registro, 6);
            chave = atoi(aux);
            tamanho = strlen(registro);
           
            if(chave == chave_primaria) /* Quando encontrar a chave,imprima Todo o registro localizado .*/
            {
              
                printf("\nxREGISTRO=%s\n",registro );                          
            }
           
            
            pos += tamanho;
            
       
            fseek(arq_dados, pos, SEEK_SET);
            k++;
            
        }
        fclose(arq_dados);
}

int insere_arqDados(int chave){
    char registro[500];
    int i, virg=0, n=0, pos=0;
    FILE *fp;
    struct node *ptr = root;

    while (ptr)
    {
        n = ptr->n;
        pos = pesquisaPos(chave, ptr->chaves, n);
        
        if (pos < n && chave == ptr->chaves[pos])
        {
            printf("\nChave ja usada!\n");
            return 1;
        }
        ptr = ptr->p[pos];
    }

    printf("\nDigite os campos subsequentes do registro usando virgulas como separador:\n");
    printf("(Sem a chave primaria sao ao todos 17 campos, ou seja, é necessario ter 16 virgulas):\n");
    scanf("%[^\n]", registro);

    for(i = 0; i < strlen(registro); i++){ /* Percorre a string para contar as virgulas */
        if(registro[i] == ',')
            virg++;
    }

    if (virg != 16) /* Caso nao tenha 16 virgulas */
    {
        printf("\n%d digitados, Digite a quantidade certa de campos!\n", virg+1);
        insere_arqDados(chave);
    }

    fp = fopen("data.txt", "a");

    if (fp == NULL)
    {
        printf("Falha ao tentar abrir o arquivo!\n");
        exit(1);
    }

    fprintf(fp, "%d,%s", chave, registro);

    fclose(fp);

    printf("\nRegistro inserido no arquivo de dados!\n");

    return 2;

}

void imprimeArqDados(){
    FILE *fp;
    fp = fopen("data.txt", "r");
    char registro[400];
    int pos=0, fim=0, tam=0;

    if (fp == NULL)
    {
        printf("\nFalha ao tentar abrir o arquivo!\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    fim = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    while(!feof(fp)){
        fgets(registro, sizeof(registro), fp);
        printf("\n%s", registro);
        tam = (int)strlen(registro);
        registro[tam] = '\0';

        if ( (pos+tam) >= fim)
        {
          break;
        }

        pos += tam;
        fseek(fp, pos, SEEK_SET);
    }

    printf("\n");
    fclose(fp);

}