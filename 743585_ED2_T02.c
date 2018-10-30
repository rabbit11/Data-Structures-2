/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 743585
 * Aluno: Pedro de Souza Vieira Coelho
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAx_REGISTROS 1000
#define MAx_ORDEM 150
#define TAM_ARQUIVO (MAx_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAx_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* ibrand);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice ibrand);

 void listar(Indice iprimary,Indice ibrand);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_ibrand(Indice *ibrand) ;

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto j);

/* Exibe o jogo */
int exibir_registro(int rrn);

//insercao na arvore b, indice ip (pseudo codigo de insercao em arvore b)
void* inserir_Btree_ip(Indice* iprimary, Produto product);
//insercao na arvore b, indice is (pseudo codigo de insercao em arvore b)
void* inserir_Btree_is(Indice* ibrand, Produto product);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (	TAM_STRING_INDICE +9);

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

//cria o indice iprimary
 void criar_iprimary(Indice *iprimary){
	 iprimary->raiz = -1;
 }
//cria o indice ibrand
 void criar_ibrand(Indice *ibrand){
	 ibrand->raiz = -1;
 }
 /*Escreve um nó da árvore no arquivo de índice,
 * O terceiro parametro serve para informar qual indice se trata */
 void write_btree(void *salvar, int rrn, char ip){
	 if(ip == 'p'){
		 //movimentando ponteiro para ponto do arquivo onde ocorrerá a escrita
		 char* p = ARQUIVO_IP + (tamanho_registro_ip) * (nregistrosip);

	 }
 }
 /*Lê um nó do arquivo de índice e retorna na estrutura*/
 void *read_btree(int rrn, char ip){
	 if(ip == 'p'){
		 char* p = ARQUIVO_IP + (rrn * tamanho_registro_ip);
		 node_Btree_ip* x = (node_Btree_ip*)criar_no('p');
		 int i;
		 char* nchaves;

		 //obtendo o numero de chaves naquele no
		 for(i = 0; i < 3; i++){
			nchaves[i] = p[i];
		 }

		 x->num_chaves = (int)atoi(nchaves);

		 //TODO PARTE DA CHAVE ESTÁ ERRADA
		 //obtendo as chaves presentes naquele no
		 int z = 0, posEscrita = 0;
		 for(int j = 0; j < x->num_chaves * (10 + 4); j++){
			 x->chave[z].pk[posEscrita] = p[i];
			 i++;
			 posEscrita++;
			 //a cada 10 caracteres, se inicia uma nova chave
			 if(i % 10 == 0){
			 	z++;
				i+= 4;
				x->chave[z].pk[posEscrita+1] = '\0';
				posEscrita = 0;
			}
		 }
		 //obtendo se aquele nó é ou não folha
		 x->folha = p[i];
		 i++;

		 //obtendo os descendentes daquele nó
		 char *descendentes;
		 z = 0;
		 if(x->folha == 'N'){
			 for(int j = 0; p[i] != '*';j++){
				 descendentes[j] = p[i];
				 i++;
				 if(i % 3 == 0){
					 x->desc[z] = (int)atoi(descendentes);
					 z++;
				 }
			 }
		 }
		 return x;
 	}
	if(ip == 's'){
		char* s = ARQUIVO_IS + (rrn * tamanho_registro_ip);
		node_Btree_ip* x = (node_Btree_ip*)criar_no('p');
		int i;
		char* nchaves;

		//obtendo o numero de chaves naquele no
		for(i = 0; i < 3; i++){
		   nchaves[i] = s[i];
		}
		x->num_chaves = (int)atoi(nchaves);

		//obtendo as chaves presentes naquele no
		int z = 0, posEscrita = 0;
		for(int j = 0; j < x->num_chaves * (10 + 101); j++){
			x->chave[z].pk[posEscrita] = s[i];
			i++;
			posEscrita++;
			//a cada 10 caracteres, se inicia uma nova chave
			if(i % 10 == 0){
			   z++;
			   i+= 101;
			   x->chave[z].pk[posEscrita + 1] = '\0';
			   posEscrita = 0;
		   }
		}

		//obtendo se aquele nó é ou não folha
		x->folha = s[i];
		i++;

		//obtendo os descendentes daquele nó
		char *descendentes;
		z = 0;
		if(x->folha == 'N'){
			for(int j = 0; s[i] != '*';j++){
				descendentes[j] = s[i];
				i++;
				if(i % 3 == 0){
					x->desc[z] = (int)atoi(descendentes);
				}
			}
		}
		return x;
	}
	else{
		return NULL;
	}
 }

 void* criar_no(char ip){
	 if(ip == 'p'){
		 node_Btree_ip* novoIp = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
		 novoIp->num_chaves = 0;
		 novoIp->chave = (Chave_ip*)malloc((ordem_ip) - 1 * sizeof(Chave_ip));
		 novoIp->desc = (int*)malloc(ordem_ip * sizeof(int));
		 novoIp->folha = 'F';
		 return novoIp;
	 }if(ip == 's'){
		 node_Btree_is* novoIs = (node_Btree_is*)malloc(sizeof(node_Btree_is));
		 novoIs->num_chaves = 0;
		 novoIs->chave = (Chave_is*)malloc((ordem_is) - 1 * sizeof(Chave_is));
		 novoIs->desc = (int*)malloc(ordem_is * sizeof(int));
		 novoIs->folha = 'F';
		 return novoIs;
	 }else{
		 return NULL;
	 }
 }
 /* Atualiza os dois índices com o novo registro inserido */
 void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto product){
	 if(iprimary->raiz == -1){//caso o indice esteja vazio, criamos o primeiro indice

		 //rotina de insercao do novo no no indice iprimary
		node_Btree_ip* novoIp = (node_Btree_ip*)criar_no('p');
		novoIp->num_chaves = 1;
	 	novoIp->folha = 'F';
		strcpy(novoIp->chave[0].pk ,product.pk);
		novoIp->chave[0].rrn = nregistros; //rrn do produto no arquivo de dados
		iprimary->raiz = 0;
		//escrita do novo no no arquivo de dados do indice iprimary
		write_btree(novoIp, iprimary->raiz, 'p');
		//TODO devo dar free no nó e em seus campos aqui?
		nregistrosip++;

		//rotina de insercao do novo nó no indice ibrand
		node_Btree_is* novoIs = (node_Btree_is*)criar_no('s');
		novoIs->num_chaves = 1;
	 	novoIs->folha = 'F';
		strcpy(novoIs->chave[0].pk , product.pk);
		sprintf(novoIs->chave[0].string, "%s$%s", product.marca, product.nome);
		ibrand->raiz = 0;
		//escrita do novo no no arquivo de dados do indice ibrand
		write_btree(novoIs, ibrand->raiz, 's');
		nregistrosis++;
	}else{//caso a árvore não esteja vazia, inserimos o novo nó na posicao correta
		//rotina de insercao em Ip (pseudo codigo de insercao)
		inserir_Btree_ip(iprimary, product);

		//rotina de insercao em Is (pseudo codigo de insercao)
		inserir_Btree_is(ibrand, product);
	}
 }

 //insercao na arvore b, indice ip (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_ip(Indice* iprimary, Produto product){
	 //recupera o registro do arquivo de indice iprimary
	 node_Btree_ip* x = (node_Btree_ip*)read_btree(iprimary->raiz, 'p');
	 node_Btree_ip*	node = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
	 int i;
	 //verifica se o no recuperado é folha
	 if(x->folha == 'F'){
		 //verifica se o numero de chaves alocada no nó é menor que o máximo
		 if(x->num_chaves < (ordem_ip - 1)){
			 //posiciona i na posicao do ultimo elemento daquele no
			 //(num_chaves - 1, para acessar a posicao correta, ex: 1 chave, esta
		   	 //se econtra na pos 0)
			 i = x->num_chaves - 1;
		 }
		 //encotrando a posicao correta para a insercao do novo node
		 while(i >= 1 && strcmp(product.pk, x->chave[i].pk) < 0){
			 x->chave[i + 1] = x->chave[i];
			 i += -1;
		 }
		 //inserindo o novo no na posicao correta
		 strcpy(x->chave[i + 1].pk, product.pk);
		 x->num_chaves += 1;
		 x->chave[i + 1].rrn = nregistros;

		 write_btree(x, iprimary->raiz, 'p');

		 //TODO terminar insercao
		 return NULL;
	 }
}
 void divide_no_ip(node_Btree_ip* x, char* chaveK, node_Btree_ip* fdireito){

}
 //insercao na arvore b, indice is (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_is(){

 }
 void divide_no_is(node_Btree_ip* x, char* chaveK, node_Btree_ip* fdireito){
}
//efetua busca por um registro em um dos arquivos de indice
 void buscar(Indice iprimary,Indice ibrand){
	 int opBusca;
	 char* pk;

	 scanf("%d%*c", opBusca);

	 switch(opBusca){
		 case 1://busca pela pk
		 	scanf("%[^\n]%*c", pk);

		 break;

		 case 2://busca pela marca/nome

		 break;
	 }

 }

//efetua busca por um registro no indice iprimary
int busca_ip(Indice no, Produto produto){
	node_Btree_ip *p;
}
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);

	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}
