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

//*****************funcoes referentes ao pseudo codigo de insercao em arvore b
void* inserir_Btree_is_aux(Indice* ibrand, Produto product);
void* inserir_Btree_ip_aux(Indice* iprimary, Produto product);
node_Btree_ip* divide_no_ip(Indice* x, Produto product, int fdireito);
node_Btree_is* divide_no_is(Indice* x, Produto product, int fdireito);
//*****************funcoes referentes ao pseudo codigo de insercao em arvore b

//funcao verifica se a chave passada como parametro ja foi inserida no indice primario
int verifica_chave(char* chave, Indice* iprimary);

Produto recuperar_registro(int rrn);
//busca determinada chave no indice ip
int busca_ip(int raiz, char* pk, int imprimir);
//busca determinado nome no indice ibrand
int busca_is(int raiz, char* nome, int imprimir);
/* Exibe o jogo */
int exibir_registro(int rrn);

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
			// if (alterar(iprimary))
			// 	printf(SUCESSO);
			// else
			// 	printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			//listar(iprimary, ibrand);
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
		 node_Btree_ip* aux = (node_Btree_ip*)salvar;
		 //TODO nao esquecer de alterar o rrn de salvar
		 //TODO CONTABILIZACAO DO NUMERO DE CHAVES ESTA ERRADA
		 //movimentando ponteiro para ponto do arquivo onde ocorrerá a escrita
		 char* p = ARQUIVO_IP + ((tamanho_registro_ip) * (nregistrosip));
		 char tempChaves[ordem_is];
 		 sprintf(tempChaves, "%03d", aux->num_chaves);
		 //verificando se é necessario completar com # os espaços em branco
		 int hash = 0;
		 char tempHash[TAM_STRING_INDICE];
		 char* ptempHash = tempHash;
		 if(aux->num_chaves < ordem_ip - 1){
			 hash = (ordem_ip - 1) - aux->num_chaves;
			 hash = hash * 14;
			 //passando pk e rrn das chaves para um auxiliar
			 for(int i = 0; i < aux->num_chaves; i++){
				 sprintf(tempHash, "%s", aux->chave[i].pk);
				 ptempHash += 10;
				 for(int j = 0; j < 4;j++){
					 ptempHash[i + j] = aux->chave[j].rrn;
					 ptempHash += 4;
				 }
			 }
			 //colocando o numero de # necessarios em uma string auxiliars
			 for(int i = 0; i < hash; i++){
				 ptempHash[i] = '#';
				 ptempHash++;
			 }
			 *ptempHash = '\0';
	 	}
		//verificando se e necessario completar com * o espaco dos descendentes
		//daquele node
		int filhos = 0;
		for(int j = 0; j < ordem_ip; j++){
			if(aux->desc[j] != -1){
				filhos++;
			}
		}
		char tempFilhos[ordem_ip];
		if(filhos){
			int i;
			for(i = 0; i < filhos * (ordem_ip) * 3; i++){
				tempFilhos[i] = aux->desc[i];
			}
			if(filhos < ordem_ip){
				for(int j = 0; j < filhos - ordem_ip; j++, i++){
					tempFilhos[i] = '*';
				}
			}
			tempFilhos[i] = '\0';
		}else{
			int j;
			for(j = 0; j < ordem_ip * 3;j++){
				tempFilhos[j] = '*';
			}
			tempFilhos[j] = '\0';
		}
		sprintf(p,"%s%s%d%s", tempChaves, tempHash, aux->folha, tempFilhos);
		// strcpy(ARQUIVO_IP, p);
	 }
	 else if(ip == 's'){
		node_Btree_is* aux = (node_Btree_is*)salvar;
		char* p = ARQUIVO_IS + (tamanho_registro_is * nregistrosis);

		//escrevendo o numero de chaves naquele no em uma string temporaria
		char tempChaves[ordem_is];
		sprintf(tempChaves, "%03d", aux->num_chaves);

		int hash = 0;
		char tempHash[TAM_STRING_INDICE];
		char* ptempHash = tempHash;
		//passando pk e strings das chaves para um auxiliar
		for(int i = 0; i < aux->num_chaves; i++){
			sprintf(ptempHash, "%s %s", aux->chave[i].pk, aux->chave[i].string);
			ptempHash += 10 + strlen(aux->chave[i].string);
			// hash = ((ordem_is - 1) * 10 + 101) - strlen(aux->chave[i].string);
			hash = 10 + 101 - strlen(aux->chave[i].string);
			if(hash > 0){
				ptempHash++;
				//colocando o numero de # necessarios em uma string auxiliar
				for(int j = 0; j < hash; j++){
					sprintf(ptempHash, "%c", '#');
					// printf("%d\n", j);
					ptempHash++;
				}
			}
			*ptempHash = '\0';
	   }
	   //verificando se e necessario completar com * o espaco dos descendentes
	   //daquele node
	   int filhos = 0;
	   for(int j = 0; j < ordem_ip; j++){
		   if(aux->desc[j] != -1){
			   filhos++;
		   }
	   }
	   char tempFilhos[ordem_is];
	   if(filhos){
		   int i;
		   //passando os filhos para uma string auxiliar
		   //TODO esta certo a condicao deste for?
		   for(i = 0; i < filhos * (ordem_ip) * 3; i++){
			   tempFilhos[i] = aux->desc[i];
		   }
		   //inserindo * nessa string auxiliar de acordo com o numero de descendentes
		   //em branco
		   if(filhos < ordem_ip){
			   for(int j = 0; j < filhos - ordem_ip; j++, i++){
				   tempFilhos[i] = '*';
			   }
		   }
		   tempFilhos[i] = '\0';
	   }else{
		   int j;
		   //inserindo * nessa string auxiliar de acordo com o numero de descendentes
		   //em branco
		   for(j = 0; j < ordem_ip * 3;j++){
			   tempFilhos[j] = '*';
		   }
		   tempFilhos[j] = '\0';
	   }
	   //TODO consertar num_chaves pois esta de 1 byte e deve ser 3
	   sprintf(p,"%s%s%c%s", tempChaves, tempHash, aux->folha, tempFilhos);
	   // strcpy(ARQUIVO_IS, p);
	   printf("%s\n", p);
	   //TODO erro da depois do printf ?????
	 }
 }
 /*Lê um nó do arquivo de índice e retorna na estrutura*/
 void *read_btree(int rrn, char ip){
	 //se a arvore estiver vazia, retorna NULL
	 //TODO necessario esse primeiro if? dava segfault sem ele
	 if(rrn == -1){
		 return NULL;
	 }
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
 //realiza procedimentos de alocacao de um novo no
 void* criar_no(char ip){
	 if(ip == 'p'){
		 //TODO precisa dar malloc nos campos do nó?
		 node_Btree_ip* novoIp = (node_Btree_ip*)calloc(1, sizeof(node_Btree_ip));
		 novoIp->num_chaves = 0;
		 novoIp->chave = (Chave_ip*)malloc((ordem_ip - 1) * sizeof(Chave_ip));
		 novoIp->folha = 'F';
		 novoIp->desc = (int*)malloc(ordem_ip * sizeof(int));
		 for(int i = 0; i < ordem_ip;i++){
			 novoIp->desc[i] = -1;
		 }
		 return novoIp;
	 }if(ip == 's'){
		 node_Btree_is* novoIs = (node_Btree_is*)calloc(1, sizeof(node_Btree_is));
		 novoIs->num_chaves = 0;
		 novoIs->chave = (Chave_is*)malloc((ordem_is - 1) * sizeof(Chave_is));
		 novoIs->folha = 'F';
		 novoIs->desc = (int*)malloc(ordem_is * sizeof(int));
		 for(int i = 0; i < ordem_is;i++){
			 novoIs->desc[i] = -1;
		 }
		 return novoIs;
	 }else{
		 return NULL;
	 }
 }
//libera memoria alocada pelo nó, incluindo ele mesmo
void libera_no(void *node, char ip){
	if(ip == 'p'){
		node_Btree_ip* temp = (node_Btree_ip*)node;
		free(temp->chave);
		free(temp->desc);
		free(temp);
		return;
	}
	if(ip == 's'){
		node_Btree_is* temp = (node_Btree_is*)node;
		free(temp->chave);
		free(temp->desc);
		free(temp);
		return;
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
		libera_no(novoIp, 'p');
		nregistrosip++;
	}else{//caso a árvore não esteja vazia, inserimos o novo nó na posicao correta
		//rotina de insercao em Ip (pseudo codigo de insercao)
		node_Btree_ip* promocaoIp;
		promocaoIp = (node_Btree_ip*)inserir_Btree_ip_aux(iprimary, product);
		if(promocaoIp){
			node_Btree_ip* x = (node_Btree_ip*)criar_no('p');
			x->folha = 'N';
			x->num_chaves = 1;
			//TODO CHECAR SE O RETORNO DE INSERIR RETORNA OS CAMPOS REQUISITADOS
			strcpy(x->chave[1].pk, promocaoIp->chave[0].pk);
			x->desc[0] = iprimary->raiz;
			x->desc[1] = promocaoIp->desc[0];
			iprimary->raiz = nregistrosip;

			write_btree(x, nregistrosip, 'p');
			libera_no(x, 'p');
			nregistrosip++;
		}
	}
	if(ibrand->raiz == -1){
		//rotina de insercao do novo nó no indice ibrand
		node_Btree_is* novoIs = (node_Btree_is*)criar_no('s');
		novoIs->num_chaves = 1;
	 	novoIs->folha = 'F';
		strcpy(novoIs->chave[0].pk , product.pk);
		sprintf(novoIs->chave[0].string, "%s$%s", product.marca, product.nome);
		ibrand->raiz = 0;
		//escrita do novo no no arquivo de dados do indice ibrand
		write_btree(novoIs, ibrand->raiz, 's');
		printf("YUCK FOU\n");
		libera_no(novoIs, 's');
		nregistrosis++;
	}else{//caso a árvore não esteja vazia, inserimos o novo nó na posicao correta
		//rotina de insercao em Is (pseudo codigo de insercao)
		node_Btree_is* promocaoIs;
		promocaoIs = (node_Btree_is*)inserir_Btree_is_aux(ibrand, product);
		if(promocaoIs){
			node_Btree_is* y = (node_Btree_is*)criar_no('s');
			y->folha = 'N';
			y->num_chaves = 1;
			//TODO CHECAR SE O RETORNO DE INSERIR RETORNA OS CAMPOS REQUISITADOS
			strcpy(y->chave[1].pk, promocaoIs->chave[0].pk);
			strcpy(y->chave[1].string, promocaoIs->chave[0].string);
			y->desc[0] = iprimary->raiz;
			y->desc[1] = promocaoIs->desc[0];
			iprimary->raiz = nregistrosip;

			write_btree(y, nregistrosis, 's');
			libera_no(y, 's');
			nregistrosis++;
		}
	}
 }
 //insercao na arvore b, indice ip (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_ip_aux(Indice* iprimary, Produto product){
	 //recupera o registro do arquivo de indice iprimary
	 node_Btree_ip* x = (node_Btree_ip*)read_btree(iprimary->raiz, 'p');
	 node_Btree_ip*	node = (node_Btree_ip*)calloc(1, sizeof(node_Btree_ip));
	 int i;
	 //TODO dar free no "retorno"
	 //verifica se o no recuperado é folha
	 if(x->folha == 'F'){
		 //verifica se o numero de chaves alocada no nó é menor que o máximo
		 if(x->num_chaves < (ordem_ip - 1)){
			 //posiciona i na posicao do ultimo elemento daquele no
			 //(num_chaves - 1, para acessar a posicao correta, ex: 1 chave, esta
		   	 //se econtra na pos 0)
			 i = x->num_chaves - 1;
			 //encotrando a posicao correta para a insercao do novo node
			 while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
				 x->chave[i + 1] = x->chave[i];
				 i += -1;
			 }
			 //inserindo o novo no na posicao correta
			 strcpy(x->chave[i + 1].pk, product.pk);
			 x->num_chaves += 1;
			 x->chave[i + 1].rrn = nregistros;
			 //escrevendo no nó na arvore
			 write_btree(x, iprimary->raiz, 'p');
			 libera_no(x, 'p');
			 libera_no(node, 'p');
			 return NULL;
		 	}
			else{
			 	libera_no(x, 'p');
	   			libera_no(node, 'p');
				divide_no_ip(iprimary, product, -1);
		 	}
		}
		//caso o node encontrado nao seja uma folha
		else if(x->folha == 'N'){
			i = x->num_chaves - 1;
			//encontrando a posicao correta dentro do node
			while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
				i = i - 1;
			}
			i = i + 1;
			Indice* promo = (Indice *)calloc(1 , sizeof(Indice));
			promo->raiz = x->desc[i];
			node = (node_Btree_ip*)inserir_Btree_ip_aux(promo, product);
		}
		//caso a promocao tenha ocorrido
		if(node){
			//TODO como passar os parâmetros de node para o produto?
			strcpy(product.pk, node->chave[0].pk);

			if(x->num_chaves < ordem_ip - 1){
				i = x->num_chaves;

				while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
					strcpy(x->chave[i + 1].pk, x->chave[i].pk);
					x->desc[i + 2] = x->desc[i + 1];
					i = i - 1;
				}
				x->chave[i + 1].rrn = node->chave[0].rrn;
				strcpy(x->chave[i + 1].pk, product.pk);
				//TODO COMO OBTER O FILHO DIREITO?
				x->desc[i + 2] = node->desc[0];
				x->num_chaves += 1;
				write_btree(x, iprimary->raiz, 'p');
				libera_no(x, 'p');
   			 	libera_no(node, 'p');
				return node;
			}else{
				libera_no(x, 'p');
   			 	libera_no(node, 'p');
				return divide_no_ip(iprimary, product, node->desc[0]);
			}
		}else{
			libera_no(x, 'p');
			libera_no(node, 'p');
			return NULL;
		}
}
//insercao na arvore b, indice is (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_is_aux(Indice* ibrand, Produto product){
	//recupera o registro do arquivo de indice iprimary
	node_Btree_is* x = (node_Btree_is*)read_btree(ibrand->raiz, 'p');
	node_Btree_is* node = (node_Btree_is*)calloc(1, sizeof(node_Btree_is));
	int i;
	//TODO dar free no "retorno"
	//verifica se o no recuperado é folha
	if(x->folha == 'F'){
		//verifica se o numero de chaves alocada no nó é menor que o máximo
		if(x->num_chaves < (ordem_ip - 1)){
			//posiciona i na posicao do ultimo elemento daquele no
			//(num_chaves - 1, para acessar a posicao correta, ex: 1 chave, esta
			//se econtra na pos 0)
			i = x->num_chaves - 1;
			//encotrando a posicao correta para a insercao do novo node
			while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
				x->chave[i + 1] = x->chave[i];
				i += -1;
			}
			//inserindo o novo no na posicao correta
			strcpy(x->chave[i + 1].pk, product.pk);
			x->num_chaves += 1;
			strcpy(x->chave[i + 1].string, product.nome);
			//escrevendo no nó na arvore
			write_btree(x, ibrand->raiz, 's');
			libera_no(x, 's');
			libera_no(node, 's');
			return NULL;
		   }
		   else{
			   libera_no(x, 's');
			   libera_no(node, 's');
			   divide_no_is(ibrand, product, -1);
		   }
	   }
	   //caso o node encontrado nao seja uma folha
	   else{
		   i = x->num_chaves - 1;
		   //encontrando a posicao correta dentro do node
		   while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
			   i = i - 1;
		   }
		   i = i + 1;
		   Indice* promo = (Indice *)calloc(1, sizeof(Indice));
		   promo->raiz = x->desc[i];
		   node = (node_Btree_is*)inserir_Btree_is_aux(promo, product);
	   }
	   //caso a promocao tenha ocorrido
	   if(node){
		   //TODO como passar os parâmetros de node para o produto?
		   strcpy(product.pk, node->chave[0].pk);

		   if(x->num_chaves < ordem_ip - 1){
			   i = x->num_chaves;
			   while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
				   strcpy(x->chave[i + 1].pk, x->chave[i].pk);
				   x->desc[i + 2] = x->desc[i + 1];
				   i = i - 1;
			   }
			   strcpy(x->chave[i + 1].string, node->chave[0].string);
			   strcpy(x->chave[i + 1].pk, product.pk);
			   //TODO COMO OBTER O FILHO DIREITO?
			   x->desc[i + 2] = node->desc[0];
			   x->num_chaves += 1;
			   write_btree(x, ibrand->raiz, 's');
			   libera_no(x, 's');
  			   libera_no(node, 's');
			   return node;
		   }else{
			   libera_no(x, 's');
  			   libera_no(node, 's');
			   return divide_no_is(ibrand, product, node->desc[0]);
		   }
	   }else{
		   libera_no(x, 's');
		   libera_no(node, 's');
		   return NULL;
	   }
}
 node_Btree_ip* divide_no_ip(Indice* x, Produto product, int fdireito){
	 //TODO CONFERIR
	 //recupera-se a pagina x
	 node_Btree_ip* node = (node_Btree_ip*)read_btree(x->raiz, 'p');
	 //colocamos i na posicao do ultimo elemento da pagina
	 int i = node->num_chaves - 1;
	 int chave_alocada = 0;
	 node_Btree_ip* y = (node_Btree_ip*)criar_no('p');

	 y->folha = node->folha;
	 y->num_chaves = (ordem_ip - 1) / 2;
	 y->desc[0] = -1;

	 for(int j = y->num_chaves; i > 0; i--){
		 if(!chave_alocada && strcmp(product.pk, node->chave[i].pk) > 0){
			 strcpy(y->chave[j].pk, product.pk);
			 if(fdireito != -1){
				 y->desc[j + 1] = node->desc[i + 1];
				 chave_alocada = 1;
		 	}else{
				y->desc[j + 1] = -1;
			}
		 }
		 else{
			 y->chave[j] = node->chave[i];
			 y->desc[j + 1] = node->desc[i + 1];
			 i = i - 1;
		 }
	 }
	 if(!chave_alocada){
		 while(i >= 0 && strcmp(product.pk, node->chave[i].pk) < 0){
			 node->chave[i + 1] = node->chave[i];
			 node->desc[i + 2] = node->desc[i + 1];
			 i = i - 1;
		 }
		 strcpy(node->chave[i + 1].pk, product.pk);
		 if(fdireito != -1)
		 	node->desc[i + 2] = fdireito;
		else
			node->desc[i + 2] = -1;
		 //TODO precisa de um caso fdireito == NULL?
	 }
	 node_Btree_ip* chave_promovida;
	 strcpy(chave_promovida->chave[0].pk, node->chave[ordem_ip / 2 + 1].pk);
	 y->desc[1] = node->desc[ordem_ip / 2 + 2];
	 node->num_chaves = ordem_ip / 2;

	 write_btree(y, nregistrosip, 'p');
	 write_btree(node, x->raiz, 'p');
	 libera_no(node, 'p');
	 libera_no(y, 'p');
	 nregistrosip++;

	 return chave_promovida;
}
 node_Btree_is* divide_no_is(Indice* x, Produto product, int fdireito){
	//TODO CONFERIR
	//recupera-se a pagina x
	node_Btree_is* node = (node_Btree_is*)read_btree(x->raiz, 's');
	//colocamos i na posicao do ultimo elemento da pagina
	int i = node->num_chaves - 1;
	int chave_alocada = 0;
	node_Btree_is* y = (node_Btree_is*)criar_no('p');

	//criando no novo node
	sprintf(y->chave->string, "%s$%s", product.marca, product.nome);
	y->folha = node->folha;
	y->num_chaves = (ordem_ip - 1) / 2;
	y->desc[0] = -1;

	for(int j = y->num_chaves; i > 0; i--){
		if(!chave_alocada && strcmp(product.pk, node->chave[i].pk) > 0){
			strcpy(y->chave[j].pk, product.pk);
			sprintf(y->chave[j].string, "%s$%s", product.marca, product.nome);
			if(fdireito != -1){
				y->desc[j + 1] = node->desc[i + 1];
				chave_alocada = 1;
		   }else{
			   y->desc[j + 1] = -1;
		   }
		}
		else{
			y->chave[j] = node->chave[i];
			y->desc[j + 1] = node->desc[i + 1];
			i = i - 1;
		}
	}
	if(!chave_alocada){
		while(i >= 0 && strcmp(product.pk, node->chave[i].pk) < 0){
			node->chave[i + 1] = node->chave[i];
			node->desc[i + 2] = node->desc[i + 1];
			i = i - 1;
		}
		strcpy(node->chave[i + 1].pk, product.pk);
		sprintf(y->chave[i + 1].string, "%s$%s", product.marca, product.nome);
		if(fdireito != -1){
			node->desc[i + 2] = fdireito;
		}else{
			node->desc[i + 2] = -1;
		}
		//TODO precisa de um caso fdireito == NULL?
	}
	node_Btree_is* chave_promovida;
	strcpy(chave_promovida->chave[0].pk, node->chave[ordem_ip / 2 + 1].pk);
	strcpy(chave_promovida->chave[0].string, node->chave[ordem_ip / 2 + 1].string);
	y->desc[1] = node->desc[ordem_ip / 2 + 2];
	node->num_chaves = ordem_ip / 2;

	write_btree(y, nregistrosip, 's');
	write_btree(node, x->raiz, 's');
	libera_no(node, 's');
	libera_no(y, 's');
	nregistrosis++;
	return chave_promovida;
}
//efetua busca no indice iprimary
 int busca_ip(int raiz, char* pk, int imprimir){
	 if(raiz == -1){
		 return 0;
	 }
 	node_Btree_ip* x = (node_Btree_ip*)read_btree(raiz, 'p');
 	//TODO seria melhor retornar o nó ou sua posicao de alguma forma?
 	// node_Btree_ip* retorno;
 	int i = 0;

 	//busca pelas chaves armazenadas em determinado nó
 	while(i < x->num_chaves && strcmp(pk, x->chave[i].pk) > 0){
 		i = i + 1;
 	}
 	//checa se o ponteiro i aponta para o produto procurado
 	if(i <= x->num_chaves && strcmp(pk, x->chave[i].pk) == 0){
		libera_no(x, 'p');
		//TODO pode ser util retornar o rrn do registro no indice primario?
 		// return x->chave[i].rrn;
		char* p = ARQUIVO_IP + (tamanho_registro_ip * x->chave[i].rrn);
		char* tempRRN = NULL;
		//posicionando o ponteiro na posicao onde se encontra o rrn do arquivo de dados
		p += 3 + 10;

		for(int i = 0; i < 4; i++){
			tempRRN[i] = p[i];
		}

		return atoi(tempRRN);
 	}
 	if(x->folha == 'F'){
		libera_no(x, 'p');
 		return 0;
 	}
 	else{
		libera_no(x, 'p');
		if(imprimir){
			printf("%s", x->chave[0].pk);
			for(int k = 1; k < x->num_chaves; k++){
				printf(" ,%s", x->chave[k].pk);
			}
			return busca_is(x->desc[i], pk, 1);
		}
		else
			return busca_ip(x->desc[i], pk, 0);
 	}
 }
 //efetua busca no indice ibrand
 int busca_is(int raiz, char* nome, int imprimir){
	 if(raiz){
		 return 0;
	 }
 	node_Btree_is* x = (node_Btree_is*)read_btree(raiz, 's');
 	//TODO seria melhor retornar o nó ou sua posicao de alguma forma?
 	// node_Btree_ip* retorno;
 	int i = 0;

 	//busca pelas chaves armazenadas em determinado nó
 	while(i < x->num_chaves && strcmp(nome, x->chave[i].string) > 0){
 		i = i + 1;
 	}
 	//checa se o ponteiro i aponta para o produto procurado
 	if(i <= x->num_chaves && strcmp(nome, x->chave[i].string) == 0){
		libera_no(x, 's');
 		return 1;
 	}
 	if(x->folha == 'F'){
		libera_no(x, 's');
 		return 0;
 	}
 	else{
		libera_no(x, 's');
		if(imprimir){
			printf("%s", x->chave[0].string);
			for(int k = 1; k < x->num_chaves; k++){
				printf(", %s", x->chave[k].string);
			}
			return busca_is(x->desc[i], nome, 1);
		}
		else
			return busca_is(x->desc[i], nome, 0);
 	}
 }
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}
//Gera uma chave primária com base no produto fornecido como parâmetro
void gerarChave(Produto* p){
	int i = 0;
	//inserindo os 2 caracteres do nome
	int j;
	for(j = 0;j < 2; i++, j++){
		if(p->nome[j] > 'Z')
			p->pk[i] = p->nome[j] - 32;
		else
			p->pk[i] = p->nome[j];
	}
	//inserindo os 2 caracteres da marca
	for(j = 0;j < 2; i++, j++){
		if(p->marca[j] > 'Z')
			p->pk[i] = p->marca[j] - 32;
		else
			p->pk[i] = p->marca[j];
	}
	//inserindo os 4 digitos da data
	for(j = 0;j < 5; j++){
		if(p->data[j] != '/'){
			p->pk[i] = p->data[j];
			i++;
		}
	}
	for(j = 0;j < 2; i++, j++){
		p->pk[i] = p->ano[j];
	}
	p->pk[i] = '\0';
}
//Retorna um produto obtido do arquivo de dados
Produto recuperar_registro(int rrn){
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	gerarChave(&j);
	return j;
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
//TODO devo desalocar todos os nós das arvores que eu usar?

//verifica se a chave passada como parametro ja se encontra em ip
int verifica_chave(char* chave, Indice* iprimary){
	int foundIt = 0;
	foundIt = busca_ip(iprimary->raiz, chave, 0);
	if(foundIt){
		printf(ERRO_PK_REPETIDA, chave);
		return 0;
	}
	return foundIt;
}
//********************funcoes de interacao com o usuario*************************************
//cadastra determinado produto nos arquivos de indices e dados
void cadastrar(Indice* iprimary, Indice* ibrand){
	char temp[TAM_REGISTRO];
	Produto novo;

	scanf("%[^\n]%*c", novo.nome);
	scanf("%[^\n]%*c", novo.marca);
	scanf("%[^\n]%*c", novo.data);
	scanf("%[^\n]%*c", novo.ano);
	scanf("%[^\n]%*c", novo.preco);
	scanf("%[^\n]%*c", novo.desconto);
	scanf("%[^\n]%*c", novo.categoria);

	//gera uma chave para este novo produto
	gerarChave(&novo);
	//checa se a chave gerada já existia no sistema, se sim a função se encerra
	int flagChave = verifica_chave(novo.pk, iprimary);
	if(flagChave)
		return;
	//imprime todos os dados do produto em uma string temporaria, para depois
	//ser passada para o arquivo principal
	sprintf(temp,"%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data,
			novo.ano, novo.preco, novo.desconto, novo.categoria);

	int aux = strlen(temp);
	//verifica se o registro possui tamanho menor que 192 bytes, se sim, devemos
	//preencher o resto do espaço com #
	while(aux < 192){
		temp[aux] = '#';
		aux++;
	}
	//inserindo a string gerada no arquivo de dados
	char* fimArquivo = ARQUIVO + (192 * (nregistros));
	sprintf(fimArquivo, "%s", temp);
	fimArquivo += 192;
	*fimArquivo = '\0';

	// printf("%s\n", ARQUIVO);
	//inserindo o produto no indice primario
	inserir_registro_indices(iprimary, ibrand, novo);
}
//altera o desconto do produto referente a chave primaria inserida
int alterar_desconto(Indice* iprimary){
	char pkBusca[TAM_PRIMARY_KEY];
	//recebe pk do usuario
	scanf("%[^\n]%*c", &pkBusca);
	//busca pk no indice primario
	int retornoRRN = busca_ip(iprimary->raiz, pkBusca, 0);

	if(!retornoRRN){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	else{
		char* desconto;
		scanf("%[^\n]%*c", desconto);

		int i = atoi(desconto);
		//conferindo se o desconto passado pelo usuario segue a formatacao necessaria
		while(i < 0 || i > 100 || strlen(desconto) != 3){
			printf(CAMPO_INVALIDO);
			scanf("%s%*c", desconto);
			i = atoi(desconto);
		}

		char* p = ARQUIVO_IP + (tamanho_registro_ip * retornoRRN);
		char* tempRRN = NULL;
		//posicionando o ponteiro na posicao onde se encontra o rrn do arquivo de dados
		p += 3 + 10;
		//passando o rrn do arquivo de dados para uma string temporaria
		for(int j = 0; j < 4; j++){
			tempRRN[j] = p[j];
		}
		int rrnDados = atoi(tempRRN);
		//recuperando o produto para ter acesso a seus campos
		Produto alterado = recuperar_registro(rrnDados);
		//obtendo o deslocamento necessario para encontrar o campo desconto do registro
		int deslocamento = strlen(alterado.nome) + strlen(alterado.marca) + strlen(alterado.data)
		 				+ strlen(alterado.ano) + strlen(alterado.preco) + (rrnDados * 192) + 5;

		//posicionando o apontador no inicio do campo desconto
		p = ARQUIVO + deslocamento;
		//escrevendo o novo desconto no arquivo de dados
		int z;
		for(z = 0; z < 3; z++){
			p[z] = desconto[z];
		}
		return 1;
	}
}
//efetua busca por um registro em um dos arquivos de indice
 void buscar(Indice iprimary,Indice ibrand){
	 //TODO falta impressao na tela
	 int opBusca;
	 char* chaveBusca;

	 scanf("%d%*c", &opBusca);

	 int foundIt = -1;
	 switch(opBusca){
		 case 1://busca pela pk
		 	scanf("%[^\n]%*c", chaveBusca);
			foundIt = busca_ip(iprimary.raiz, chaveBusca, 1);
			if(foundIt == -1)
				printf(REGISTRO_N_ENCONTRADO);
			else
				exibir_registro(foundIt);
		 break;

		 case 2://busca pela marca/nome
			 scanf("%[^\n]%*c", chaveBusca);
			 foundIt = busca_ip(ibrand.raiz, chaveBusca, 1);
			 if(foundIt == -1)
			 	printf(REGISTRO_N_ENCONTRADO);
			 else
			 	exibir_registro(foundIt);
		 break;
	 }

 }
//lista todos os registros presentes nos arquivos de indice
void listar(Indice iprimary,Indice ibrand){
	int opLista = 0;

	scanf("%d%*c", opLista);

	if(opLista == 1){

	}
	else if(opLista == 2){
		
	}
}
