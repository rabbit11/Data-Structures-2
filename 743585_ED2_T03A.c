/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 743585  
 * Aluno: Pedro de Souza Vieira Coelho
 * ========================================================================== */

 /* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtable;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;




/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
				case 1:
					cadastrar(&tabela);
					break;
				case 2:
					printf(INICIO_ALTERACAO);
					if(alterar(tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					imprimir_tabela(tabela);
					break;
				case 6:
					liberar_tabela(&tabela);
					break;
				case 10:
					printf(INICIO_ARQUIVO);
					printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
					break;
				default:
					printf(OPCAO_INVALIDA);
					break;
			}
	}
	return 0;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54; 
}

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Jogo j = recuperar_registro(rrn);
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
	strncpy(categorias, j.categoria, strlen(j.categoria));
  for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    printf("%s ", cat);
	printf("\n");
	return 1;
}