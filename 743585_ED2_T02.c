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

//struct criada para obter informações adicionais sobre o split e insercao dos nodes
//para facilitar a aproximação entre o código e o pseudo codigo fornecido
typedef struct{
	Chave_is chave; //chave e string do produto (podemos colocar pk dentro do campo string)
	int rrn;		//rrn do produto
	int desc;		//ponteiros para os descendentes
} Info_node;

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
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto product);

//*****************funcoes referentes ao pseudo codigo de insercao em arvore b
void* inserir_Btree_is_aux(Indice* ibrand, Produto product);
void* inserir_Btree_ip_aux(Indice* iprimary, Produto product);
Info_node* divide_no_ip(Indice* x, Produto product, int rrn_product, int fdireito);
Info_node* divide_no_is(Indice* x, Produto product, int rrn_product, int fdireito);
//*****************funcoes referentes ao pseudo codigo de insercao em arvore b

//funcao verifica se a chave passada como parametro ja foi inserida no indice primario
int verifica_chave(char* chave, Indice iprimary);

Produto recuperar_registro(int rrn);
//busca determinada chave no indice ip
int busca_ip(int raiz, char* pk, int imprimir);
//busca determinado nome no indice ibrand
int busca_is(int raiz, char* nome, int imprimir);
/* Exibe o jogo */
int exibir_registro(int rrn);
//imprime a arvore em pre ordem
void printPreorder(int nivelArvore, int raiz);
//imprime a arvore em ordem
void printInorder(int raiz);
//imprime a string passada da forma listar
void printString(char* string);


int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
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
			// printf(INICIO_ALTERACAO);
			// if (alterar(iprimary))
			//  	printf(SUCESSO);
			// else
			//  	printf(FALHA);
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
	 //TODO rrn no arquivo de indices errado
	 if(ip == 'p'){
	     node_Btree_ip* aux = (node_Btree_ip*)salvar;
	     //movimentando ponteiro para ponto do arquivo onde ocorrerá a escrita
	     char *p = NULL;
	     char temp[1000];

	     p = ARQUIVO_IP + (tamanho_registro_ip * rrn);
	     sprintf(temp, "%03d", aux->num_chaves);
	     //verificando se é necessario completar com # os espaços em branco
	     int hash = 0;
	     int tam = 3;

	    for(int i = 0; i < aux->num_chaves; i++){
	        sprintf((temp + tam), "%s", aux->chave[i].pk);
	        tam += 10;
	        sprintf((temp + tam),"%04d", aux->chave[i].rrn);
	        tam += 4;
	         //colocando o numero de # necessarios em uma string auxiliars
	         if(aux->num_chaves < ordem_ip - 1){
	             hash = (ordem_ip - 1) - aux->num_chaves;
	             hash = hash * 14;
	             //passando pk e rrn das chaves para um auxiliar
	             for(int i = 0; i < hash; i++){
	                 temp[tam] = '#';
	                 tam++;
	             }
	        }
	    }
	    temp[tam] = aux->folha;
	    tam++;
	    //verificando se e necessario completar com * o espaco dos descendentes
	    //daquele node
		//TODO copiar no ARQUIVO_IS
	    if(aux->folha != 'F'){
	        int i;
	        for(i = 0; i < aux->num_chaves + 1; i++){
	            sprintf(temp + tam, "%03d", aux->desc[i]);
	            tam += 3;
	        }
			i--;
			for( ; i < ordem_ip; i++){
				temp[tam++] = '*';
				temp[tam++] = '*';
				temp[tam++] = '*';
			}
	    }else{
	        int j;
	        for(j = 0; j < ordem_ip * 3;j++){
	            temp[tam + j] = '*';
	        }
	    }
	    // sprintf(temp,"%s%s%c%s", tempChaves, tempHash, aux->folha, tempFilhos);
	    strncpy(p, temp, tamanho_registro_ip);
		p[tamanho_registro_ip * (nregistrosip + 1)] = '\0';
	    // strncat(p, temp, tamanho_registro_ip);
		// printf("A %s\n", ARQUIVO_IP);
	 }
	 else if(ip == 's'){
		 //TODO ao remover uma das chaves do nó, esta chave continua no arquivo
		node_Btree_is* aux = (node_Btree_is*)salvar;
		char* p = ARQUIVO_IS + (tamanho_registro_is * rrn);
		char temp[1000];

		//escrevendo o numero de chaves naquele no em uma string temporaria
		sprintf(temp, "%03d", aux->num_chaves);
		int hash = 0;
		int tam = 3;
		//passando pk e strings das chaves para um auxiliar
		for(int i = 0; i < aux->num_chaves; i++){
			sprintf((temp + tam), "%s %s", aux->chave[i].pk, aux->chave[i].string);
			tam += 11 + strlen(aux->chave[i].string);
			// hash = ((ordem_is - 1) * 10 + 101) - strlen(aux->chave[i].string);
			hash = 101 - strlen(aux->chave[i].string);
			if(hash > 0){
				//colocando o numero de # necessarios em uma string auxiliar
				for(int j = 0; j < hash; j++){
					temp[tam] = '#';
					tam++;
				}
			}
	   }

	   temp[tam] = aux->folha;
	   tam++;
	   //verificando se e necessario completar com * o espaco dos descendentes
	   //daquele node
	   if(aux->folha != 'F'){
		   int i;
		   for(i = 0; i < aux->num_chaves + 1; i++){
			   sprintf(temp + tam, "%03d", aux->desc[i]);
			   tam += 3;
		   }
		   i--;
		   for( ; i < ordem_ip; i++){
			   temp[tam++] = '*';
			   temp[tam++] = '*';
			   temp[tam++] = '*';
		   }
		   temp[tam++] = '\0';
	   }else{
		   int j;
		   for(j = 0; j < ordem_ip * 3;j++){
			   temp[tam + j] = '*';
		   }
		   temp[tam + j] = '\0';
	   }
	   strncpy(p, temp, tamanho_registro_is);
	   p[tamanho_registro_is * (nregistrosis + 1)] = '\0';
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
		 int i = 0;
		 char nchaves[4];
		 //obtendo o numero de chaves naquele no
		 for(i = 0; i < 3; i++){
			nchaves[i] = p[i];
		 }
		 nchaves[i] = '\0';

		 x->num_chaves = (int)atoi(nchaves);
		 //obtendo as chaves presentes naquele no
		 char tempRRN[4];
		 for(int j = 0; j < x->num_chaves; j++){
			 strncpy(x->chave[j].pk, (p + i), 10);
			 strncpy(tempRRN, (p + i + 10), 4);
			 tempRRN[4] = '\0';
			 x->chave[j].rrn = (int)atoi(tempRRN);
			 x->chave[j].pk[10] = '\0';
 			 i += 14;
		 }

		 //obtendo se aquele nó é ou não folha
		 if(x->num_chaves <= 1)
		 	i+= 14;
		 x->folha = p[i];
		 i++;
		 //obtendo os descendentes daquele nó
		 char descendentes[ordem_ip], tempDesc[4];
		 int z = 0, k = 1;
		 if(x->folha == 'N'){
			 for(int j = 0; p[i] != '*' && p[i] != '\0' && j < ordem_ip * 3;j++){
				 descendentes[j] = p[i];
				 if(k % 3 == 0){
					 strncpy(tempDesc, descendentes + (z * 3), 3);
					 tempDesc[3] = '\0';
					 x->desc[z] = (int)atoi(tempDesc);
					 z++;
				 }
				 i++;
				 k++;
			 }
		 }
		 // for(int i = 0; i < x->num_chaves; i++){
		 // 	printf("%d | %s | %c | %d\n", x->num_chaves, x->chave[i].pk, x->folha, x->desc[2]);
	 	 // }
		 return x;
 	}
	if(ip == 's'){
		char* s = ARQUIVO_IS + (rrn * tamanho_registro_is);
		node_Btree_is* x = (node_Btree_is*)criar_no('s');
		int i;
		char nchaves[4];
		//obtendo o numero de chaves naquele no
		for(i = 0; i < 3; i++){
		   nchaves[i] = s[i];
		}
		nchaves[i] = '\0';

		x->num_chaves = (int)atoi(nchaves);
		//obtendo as chaves presentes naquele no
		int k = 0, tamString = 0;
		for(int j = 0; j < x->num_chaves; j++){
			k = 0, tamString = 0;
			strncpy(x->chave[j].pk, (s + i), 10);
			i+= 11;
			//descobrindo o tamanho da string sem # e \0
			for(k = 0; s[i + k] != '#' && k < 101; k++){
				tamString++;
			}
			strncpy(x->chave[j].string, (s + i), tamString);
			x->chave[j].string[tamString + 1] = '\0';
			x->chave[j].pk[10] = '\0';
			i+= 101;
		}
		//obtendo se aquele nó é ou não folha
		x->folha = s[i];
		i++;
		//obtendo os descendentes daquele nó
		char descendentes[ordem_ip], tempDesc[4];
		int z = 0;
		k = 1;
		if(x->folha == 'N'){
			for(int j = 0; s[i] != '*' && s[i] != '\0' && j < ordem_ip * 3;j++){
				descendentes[j] = s[i];
				if(k % 3 == 0){
					strncpy(tempDesc, descendentes + (z * 3), 3);
					tempDesc[3] = '\0';
					x->desc[z] = (int)atoi(tempDesc);
					z++;
				}
				i++;
				k++;
			}
		}
		// for(int i = 0; i < x->num_chaves; i++){
		// 	printf("%d !! %s !! %s !! %c !! %d\n", x->num_chaves, x->chave[i].pk, x->chave[i].string, x->folha, x->desc[i]);
		// }
		return x;
	}
	else{
		return NULL;
	}
 }
 //realiza procedimentos de alocacao de um novo no
 void* criar_no(char ip){
	 if(ip == 'p'){
		 node_Btree_ip* novoIp = (node_Btree_ip*)calloc(1, sizeof(node_Btree_ip));
		 novoIp->num_chaves = 0;
		 novoIp->chave = (Chave_ip*)calloc(ordem_ip - 1, sizeof(Chave_ip));
		 novoIp->folha = 'F';
		 novoIp->desc = (int*)calloc(ordem_ip, sizeof(int));
		 for(int i = 0; i < ordem_ip;i++){
			 novoIp->desc[i] = -1;
		 }
		 return novoIp;
	 }if(ip == 's'){
		 node_Btree_is* novoIs = (node_Btree_is*)calloc(1, sizeof(node_Btree_is));
		 novoIs->num_chaves = 0;
		 novoIs->chave = (Chave_is*)calloc(ordem_is - 1, sizeof(Chave_is));
		 novoIs->folha = 'F';
		 novoIs->desc = (int*)calloc(ordem_is, sizeof(int));
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
	if(!node){
		return;
	}
	if(ip == 'p'){
		node_Btree_ip* temp = (node_Btree_ip*)node;
		if(temp){
			free(temp->chave);
			free(temp->desc);
			free(temp);
		}
		return;
	}
	if(ip == 's'){
		node_Btree_is* temp = (node_Btree_is*)node;
		if(temp){
			free(temp->chave);
			//TODO por algum motivo o free abaixo da segfault
			free(temp->desc);
			free(temp);
		}
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
		libera_no(novoIp, 'p');
		nregistrosip++;
	}else{//caso a árvore não esteja vazia, inserimos o novo nó na posicao correta
		//rotina de insercao em Ip (pseudo codigo de insercao)
		// node_Btree_ip* promocaoIp = (node_Btree_ip*)criar_no('p');
		Info_node* promocaoIp = (Info_node*)calloc(1,sizeof(Info_node));
		promocaoIp = (Info_node*)inserir_Btree_ip_aux(iprimary, product);
		if(promocaoIp){
			//TODO filho dir retornado em promocaoIp esta correto?
			node_Btree_ip* x = (node_Btree_ip*)criar_no('p');
			x->folha = 'N';
			x->num_chaves = 1;
			strcpy(x->chave[0].pk, promocaoIp->chave.pk);
			x->chave[0].rrn = promocaoIp->rrn;
			x->desc[0] = iprimary->raiz;
			x->desc[1] = promocaoIp->desc;

			iprimary->raiz = nregistrosip;
			write_btree(x, nregistrosip, 'p');
			libera_no(x, 'p');
			free(promocaoIp);
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
		libera_no(novoIs, 's');
		nregistrosis++;
	}else{//caso a árvore não esteja vazia, inserimos o novo nó na posicao correta
		//rotina de insercao em Is (pseudo codigo de insercao)
		// node_Btree_is* promocaoIs;
		Info_node* promocaoIs = (Info_node*)calloc(1, sizeof(Info_node));
		promocaoIs = (Info_node*)inserir_Btree_is_aux(ibrand, product);

		if(promocaoIs){
			node_Btree_is* y = (node_Btree_is*)criar_no('s');
			y->folha = 'N';
			y->num_chaves = 1;
			//TODO CHECAR SE O RETORNO DE INSERIR RETORNA OS CAMPOS REQUISITADOS
			strcpy(y->chave[0].pk, promocaoIs->chave.pk);
			sprintf(y->chave[0].string, "%s", promocaoIs->chave.string);
			y->desc[0] = ibrand->raiz;
			y->desc[1] = promocaoIs->desc;
			ibrand->raiz = nregistrosis;

			write_btree(y, nregistrosis, 's');
			libera_no(y, 's');
			free(promocaoIs);
			nregistrosis++;
		}
	}
 }
 //insercao na arvore b, indice ip (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_ip_aux(Indice* iprimary, Produto product){
	 //recupera o registro do arquivo de indice iprimary
	 node_Btree_ip* x = (node_Btree_ip*)read_btree(iprimary->raiz, 'p');
	 if(!x){
		 libera_no(x, 'p');
		 return NULL;
	 }

	 node_Btree_ip*	node = (node_Btree_ip*)criar_no('p');
	 // node_Btree_ip*	chave_promovida = (node_Btree_ip*)criar_no('p');
	 Info_node* chave_promovida = (Info_node*)calloc(1,sizeof(Info_node));
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
				 i = i - 1;
			 }
			 //inserindo o novo no na posicao correta
			 strcpy(x->chave[i + 1].pk, product.pk);
			 x->num_chaves += 1;
			 x->chave[i + 1].rrn = nregistros;
			 //escrevendo no nó da arvore
			 write_btree(x, iprimary->raiz, 'p');

			 libera_no(x, 'p');
			 libera_no(node, 'p');
			 free(chave_promovida);

			 return NULL;
		 	}
			else{
			 	libera_no(x, 'p');
	   			libera_no(node, 'p');
				free(chave_promovida);
				return divide_no_ip(iprimary, product, nregistros, -1);
		 	}
		}
		//caso o node encontrado nao seja uma folha
		else{
			i = x->num_chaves - 1;
			//encontrando a posicao correta dentro do node
			Indice* promo = (Indice *)calloc(1 , sizeof(Indice));
			while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
				i = i - 1;
			}
			i = i + 1;
			promo->raiz = x->desc[i];
			chave_promovida = (Info_node*)inserir_Btree_ip_aux(promo, product);
			free(promo);
			//caso a promocao tenha ocorrido
			if(chave_promovida){
				strcpy(product.pk, chave_promovida->chave.pk);

				if(x->num_chaves < ordem_ip - 1){
					i = x->num_chaves - 1;

					while(i >= 0 && strcmp(product.pk, x->chave[i].pk) < 0){
						x->chave[i + 1] = x->chave[i];
						x->desc[i + 2] = x->desc[i + 1];
						i = i - 1;
					}
					strcpy(x->chave[i + 1].pk, chave_promovida->chave.pk);
					x->chave[i + 1].rrn = chave_promovida->rrn;
					x->desc[i + 2] = chave_promovida->desc;
					x->num_chaves += 1;

					write_btree(x, iprimary->raiz, 'p');

					libera_no(x, 'p');
	   			 	libera_no(node, 'p');
					// free(chave_promovida);

					//TODO ou devo retornar o chave_promovida?
					return NULL;
			}else{
				libera_no(x, 'p');
   			 	libera_no(node, 'p');
				free(chave_promovida);
				//TODO erro esta aqui ou em divide_no_ip?
				return divide_no_ip(iprimary, product, chave_promovida->rrn, chave_promovida->desc);
			}
		}else{
			write_btree(x, iprimary->raiz, 'p');
			libera_no(x, 'p');
			// libera_no(node, 'p'); // da problema por algum motivo
			free(chave_promovida);
			return NULL;
		}
	}
}
//insercao na arvore b, indice is (pseudo codigo de insercao em arvore b)
 void* inserir_Btree_is_aux(Indice* ibrand, Produto product){
	 //recupera o registro do arquivo de indice iprimary
	 node_Btree_is* x = (node_Btree_is*)read_btree(ibrand->raiz, 's');
	 if(!x){
	 	libera_no(x, 's');
	 	return NULL;
	 }

	 node_Btree_is*	node = (node_Btree_is*)criar_no('s');
	 // node_Btree_ip*	chave_promovida = (node_Btree_ip*)criar_no('p');
	 Info_node* chave_promovida = (Info_node*)calloc(1,sizeof(Info_node));

	 char tempString[TAM_STRING_INDICE];
	 sprintf(tempString, "%s$%s", product.marca, product.nome);

	 int i;
	 //TODO dar free no "retorno"
	 //verifica se o no recuperado é folha
	 if(x->folha == 'F'){
	 	//verifica se o numero de chaves alocada no nó é menor que o máximo
	 	if(x->num_chaves < (ordem_is - 1)){
	 		//posiciona i na posicao do ultimo elemento daquele no
	 		//(num_chaves - 1, para acessar a posicao correta, ex: 1 chave, esta
	 		//se econtra na pos 0)
	 		i = x->num_chaves - 1;
	 		//encotrando a posicao correta para a insercao do novo node
	 		while(i >= 0 && strcmp(tempString, x->chave[i].string) < 0){
				//TODO ta certo isso aqui?
				x->chave[i + 1] = x->chave[i];
	 			i = i - 1;
	 		}
	 		//inserindo o novo no na posicao correta
	 		strcpy(x->chave[i + 1].pk, product.pk);
			sprintf(x->chave[i + 1].string, "%s$%s", product.marca, product.nome);
	 		x->num_chaves += 1;
	 		// x->chave[i + 1].rrn = nregistrosip;
	 		//escrevendo no nó da arvore
	 		write_btree(x, ibrand->raiz, 's');

	 		libera_no(x, 's');
	 		// libera_no(node, 's');
			// free(chave_promovida);
	 		return NULL;
	 	   }
	 	   else{
	 		   libera_no(x, 's');
	 		   libera_no(node, 's');
			   // free(chave_promovida);
	 		   return divide_no_is(ibrand, product, nregistros, -1);
	 	   }
	    }
	    //caso o node encontrado nao seja uma folha
	    else{
	 	   i = x->num_chaves - 1;
	 	   //encontrando a posicao correta dentro do node
	 	   while(i >= 0 && strcmp(tempString, x->chave[i].string) < 0){
	 		   i = i - 1;
	 	   }
	 	   i = i + 1;
	 	   Indice* promo = (Indice *)calloc(1 , sizeof(Indice));
	 	   promo->raiz = x->desc[i];
	 	   chave_promovida = (Info_node*)inserir_Btree_is_aux(promo, product);
		   free(promo);
		    //caso a promocao tenha ocorrido
		    if(chave_promovida){
		 	   //TODO como passar os parâmetros de node para o produto?
		 	   strcpy(product.pk, chave_promovida->chave.pk);
			   int j;
			   for(j = 0; chave_promovida->chave.string[j] != '$'; j++){
				   product.marca[j] = chave_promovida->chave.string[j];
			   }
			   product.marca[j] = '\0';
			   int k;
			   for(k = 0; chave_promovida->chave.string[j] != '\0'; k++, j++){
				   product.nome[k] = chave_promovida->chave.string[j];
			   }
			   product.nome[k] = '\0';

		 	   if(x->num_chaves < ordem_is - 1){
		 		   i = x->num_chaves - 1;

		 		   while(i >= 0 && strcmp(tempString, x->chave[i].string) < 0){
					   x->chave[i + 1] = x->chave[i];
					   x->desc[i + 2] = x->desc[i + 1];
		 			   i = i - 1;
		 		   }
		 		   // x->chave[i + 1].rrn = node->chave[0].rrn;
		 		   strcpy(x->chave[i + 1].pk, product.pk);
				   sprintf(x->chave[i + 1].string, "%s$%s", product.marca, product.nome);
		 		   //TODO COMO OBTER O FILHO DIREITO?
		 		   x->desc[i + 2] = chave_promovida->desc;
		 		   x->num_chaves += 1;
		 		   write_btree(x, ibrand->raiz, 's');// ta certo isso?

		 		   libera_no(x, 's');
		 		   libera_no(node, 's');
		 		   free(chave_promovida);
				   //TODO return node ou chave_promovida?
		 		   return NULL;
		 	   }else{
		 		   libera_no(x, 's');
		 		   libera_no(node, 's');
		 		   free(chave_promovida);
		 		   return divide_no_is(ibrand, product, 0, chave_promovida->desc);
		 	   }
		    }else{
				write_btree(x, ibrand->raiz, 's');
		 	   // libera_no(node, 's');
			   // libera_no(x, 's');
		 	   free(chave_promovida);
		 	   return NULL;
		   }
	}
}
 Info_node* divide_no_ip(Indice* x, Produto product, int rrn_product, int fdireito){
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
	 for(int j = y->num_chaves - 1; j >= 0; j--){
		 if(!chave_alocada && strcmp(product.pk, node->chave[i].pk) > 0){
			 strcpy(y->chave[j].pk, product.pk);
			 y->desc[j + 1] = fdireito;
			 y->chave[j].rrn = rrn_product;
			 chave_alocada = 1;
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
		 node->chave[i + 1].rrn = rrn_product;
		 node->desc[i + 2] = fdireito;
	 }
	 Info_node* chave_promovida = (Info_node*)calloc(1, sizeof(Info_node));
	 strcpy(chave_promovida->chave.pk, node->chave[(ordem_ip / 2)].pk);
	 chave_promovida->rrn = node->chave[(ordem_ip / 2)].rrn;
	 chave_promovida->desc = nregistrosip;

	 y->desc[0] = node->desc[(ordem_ip / 2) + 1];
	 node->num_chaves = ordem_ip / 2;
	 write_btree(node, x->raiz, 'p');
	 //TODO aqui possue um caso em que y->chave[0].pk == node->chave[1].pk
	 write_btree(y, nregistrosip, 'p');
	 libera_no(node, 'p');
	 libera_no(y, 'p');
	 //TODO dou free aqui ou dando free no insere aux ja esta tudo certo?

	 //TODO pseudo codigo fala pra retornar chave_promovida e y, o que eu devo
	 //retornar?
	 nregistrosip++;
	 return chave_promovida;
}
 Info_node* divide_no_is(Indice* x, Produto product, int rrn_product, int fdireito){
	 //TODO sempre que eu copiar pk preciso copiar a string tambem?
	 //recupera-se a pagina x
	 node_Btree_is* node = (node_Btree_is*)read_btree(x->raiz, 's');
	 //colocamos i na posicao do ultimo elemento da pagina
	 int i = node->num_chaves - 1;
	 int chave_alocada = 0;

	 node_Btree_is* y = (node_Btree_is*)criar_no('s');

	 y->folha = node->folha;
	 y->num_chaves = (ordem_ip - 1) / 2;
	 y->desc[0] = -1;

	 char tempString[TAM_STRING_INDICE];
	 sprintf(tempString, "%s$%s", product.marca, product.nome);

	 //TODO j = y->num_chaves -1, certo?
	 for(int j = y->num_chaves - 1; j >= 0; j--){
		 if(!chave_alocada && strcmp(tempString, node->chave[i].string) > 0){
			 strcpy(y->chave[j].pk, product.pk);
			 sprintf(y->chave[j].string, "%s$%s", product.marca, product.nome);
			 // if(fdireito != -1){
				 y->desc[j + 1] = fdireito;
				 chave_alocada = 1;
		 	// }else{
			// 	y->desc[j + 1] = -1;
			// }
		 }
		 else{
			 y->chave[j] = node->chave[i];
			 y->desc[j + 1] = node->desc[i + 1];
			 i = i - 1;
		 }
	 }
	 if(!chave_alocada){
		 while(i >= 0 && strcmp(tempString, node->chave[i].string) < 0){
			 node->chave[i + 1] = node->chave[i];
			 node->desc[i + 2] = node->desc[i + 1];
			 i = i - 1;
		 }
		 strcpy(node->chave[i + 1].pk, product.pk);
		 sprintf(node->chave[i + 1].string, "%s$%s", product.marca, product.nome);
		 // if(fdireito != -1)
		 	node->desc[i + 2] = fdireito;
		// else
			// node->desc[i + 2] = -1;
	 }
	 // node_Btree_ip* chave_promovida = (node_Btree_ip*)criar_no('p');
	 Info_node* chave_promovida = (Info_node*)calloc(1, sizeof(Info_node));

	 strcpy(chave_promovida->chave.pk, node->chave[(ordem_is / 2)].pk);
	 strcpy(chave_promovida->chave.string, node->chave[(ordem_is / 2)].string);
	 chave_promovida->rrn = -1; //TODO o que fazer aqui?
	 chave_promovida->desc = nregistrosis;

	 y->desc[0] = node->desc[(ordem_is / 2) + 1];
	 node->num_chaves = ordem_is / 2;

	 //TODO erro possivelmente no read do arquivo IS
	 write_btree(node, x->raiz, 's');
	 write_btree(y, nregistrosis, 's');
	 nregistrosis++;
	 libera_no(node, 's');
	 libera_no(y, 's');
	 //TODO dou free aqui ou dando free no insere aux ja esta tudo certo?
	 // libera_no(chave_promovida, 'p');

	 //TODO pseudo codigo fala pra retornar chave_promovida e y, o que eu devo
	 //retornar?
	 return chave_promovida;
}
//efetua busca no indice iprimary
 int busca_ip(int raiz, char* pk, int imprimir){
	 if(raiz == -1){
		 return 0;
	 }
 	node_Btree_ip* x = (node_Btree_ip*)read_btree(raiz, 'p');

 	int i = 0;
 	//busca pelas chaves armazenadas em determinado nó
 	while(i < x->num_chaves && strcmp(pk, x->chave[i].pk) > 0){
 		i = i + 1;
 	}
 	//checa se o ponteiro i aponta para o produto procurado
 	if(i <= x->num_chaves && strcmp(pk, x->chave[i].pk) == 0){
		return x->chave[i].rrn;
 	}
	//verifica se o no atual é folha, se sim a busca é encerrada
 	if(x->folha == 'F'){
		libera_no(x, 'p');
 		return 0;
 	}
	//se o no nao for folha, podemos imprimir no no atual e chamar a funcao para um
	//de seus descendentes
 	else{
		if(imprimir){
			printf("%c\n", x->folha);
			printf("%s", x->chave[0].pk);
			for(int k = 1; k < x->num_chaves; k++){
				printf(", %s", x->chave[k].pk);
			}
			// libera_no(x, 'p');
			return busca_ip(x->desc[i], pk, 1);
		}
		else{
			// libera_no(x, 'p');
			return busca_ip(x->desc[i], pk, 0);
		}
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
 		return raiz;
 	}
 	if(x->folha == 'F'){
		libera_no(x, 's');
 		return 0;
 	}
 	else{
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
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
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
//verifica se a chave passada como parametro ja se encontra em ip
int verifica_chave(char* chave, Indice iprimary){
	int foundIt = 0;
	foundIt = busca_ip(iprimary.raiz, chave, 0);
	if(foundIt){
		printf(ERRO_PK_REPETIDA, chave);
		return 0;
	}
	return 1;
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
	//TODO nao encontra chave repetida
	if(verifica_chave(novo.pk, *iprimary) == 0)
		return;
	//imprime todos os dados do produto em uma string temporaria, para depois
	//ser passada para o arquivo principal
	sprintf(temp,"%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data,
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

	//inserindo o produto no indice primario
	inserir_registro_indices(iprimary, ibrand, novo);
	nregistros++;
}
//altera o desconto do produto referente a chave primaria inserida
int alterar_desconto(Indice* iprimary){
	char pkBusca[TAM_PRIMARY_KEY];
	//recebe pk do usuario
	scanf("%[^\n]%*c", pkBusca);
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
	 char* chaveBusca = (char*)calloc(1, sizeof(char));

	 scanf("%d%*c", &opBusca);

	 int foundIt = -1;
	 switch(opBusca){
		 case 1://busca pela pk
		 	scanf("%[^\n]%*c", chaveBusca);
			printf("Busca por %s. Nos percorridos:\n", chaveBusca);
			foundIt = busca_ip(iprimary.raiz, chaveBusca, 1);
			if(foundIt == -1){
				free(chaveBusca);
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}
			else{
				printf("\n");
				exibir_registro(foundIt);
				free(chaveBusca);
				return;
			}
		 break;

		 case 2://busca pela marca/nome
			 scanf("%[^\n]%*c", chaveBusca);
			 foundIt = busca_ip(ibrand.raiz, chaveBusca, 1);
			 if(foundIt == -1){
			 	printf(REGISTRO_N_ENCONTRADO);
				free(chaveBusca);
				return;
			}
			 else{
				 //TODO checar formatacao impressao
			 	exibir_registro(foundIt);
				free(chaveBusca);
				return;
			}
		 break;

		 default:
		 break;
	 }
	 return;
 }
//lista todos os registros presentes nos arquivos de indice
void listar(Indice iprimary,Indice ibrand){
	int opLista = 0;

	scanf("%d%*c", &opLista);

	if(opLista == 1){
		if(iprimary.raiz == -1){
			printf(REGISTRO_N_ENCONTRADO);
		}
		printPreorder(1, iprimary.raiz);
	}
	else if(opLista == 2){
		if(ibrand.raiz == -1){
			printf(REGISTRO_N_ENCONTRADO);
		}
		printInorder(ibrand.raiz);
	}
}

void printPreorder(int nivelArvore, int raiz){
	node_Btree_ip* node = (node_Btree_ip*)criar_no('p');
	node = (node_Btree_ip*)read_btree(raiz, 'p');

	int imprimiu = 0;
	if(node){
		for(int i = 0; i < node->num_chaves; i++){
			if(imprimiu == 1){
				printf(", %s", node->chave[i].pk);
			}else{
				printf("%d - %s", nivelArvore, node->chave[i].pk);
				imprimiu = 1;
			}
		}
		printf("\n");

		for(int j = 0; j < node->num_chaves + 1; j++){
			if(node->desc[j] != -1){
				printPreorder(nivelArvore++, node->desc[j]);
			}else{
				return;
			}
		}
	}
}

void printInorder(int raiz){
	node_Btree_is* node = (node_Btree_is*)criar_no('s');
	node = (node_Btree_is*)read_btree(raiz, 's');

	char tempString[TAM_STRING_INDICE], *p;

	if(node){
		if(node->folha == 'N'){
			for(int i = 0; i < node->num_chaves + 1; i++){
				printInorder(node->desc[i]);
				if(i < node->num_chaves)
					printString(node->chave[i].string);
			}
		}else{
			for(int j = 0; j < node->num_chaves; j++){
				printString(node->chave[j].string);
			}
			return;
		}
	}
}

void printString(char* string){
	char tempString[TAM_STRING_INDICE];
	char* p;

	int j = 0;

	strncpy(tempString, string, TAM_STRING_INDICE);

	p = strtok(tempString, "$\0");
	printf("%s", p);

	int complete = strlen(p);
	while(complete < TAM_MARCA){
		printf("-");
		complete++;
	}

	p = strtok(NULL, "$\0");
	printf("%s", p);

	complete = strlen(p);
	while(complete < TAM_MARCA){
		printf("-");
		complete++;
	}
	printf("\n");
}
