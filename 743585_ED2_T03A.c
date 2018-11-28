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

/* Exibe o Produto */
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
int  alterar_desconto(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
//inicia a hashtable
void criar_tabela(Hashtable* table, int tam);
//insere um produto na hashtable
void inserir(Hashtable* table, Produto produto);
//função auxiliar para inserir um produto na hashtable
int inserir_tabela(Hashtable* table, Produto produto);
//faz uma busca pelo produto passado como parametro na hashtable
Chave* buscar_tabela(Hashtable* table, char* pk);
//remove registro do arquivo de dados
void remover_arquivodedados(int rrn);
//imprime todos os termos contidos na hashtable
void imprimir_tabela(Hashtable tabela);
//gera a chave primaria com base no produto passado como parametro
void gerarChave(Produto *p);
//verifica se determinada chave se encontra no indice indice_primario
int verifica_chave(char* pk, Hashtable table);
//retorna o produto referente aquele rrn do arquivo de Dados
Produto recuperar_registro(int rrn);

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
					if(alterar_desconto(tabela))
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
//insere um produto na hashtable
void inserir(Hashtable* table, Produto produto){
	int colisoes = inserir_tabela(table, produto);
	if(colisoes != -1){
		printf(REGISTRO_INSERIDO, produto.pk, colisoes);
	}else{
		printf(ERRO_TABELA_CHEIA);
		return;
	}
}
//função auxiliar para inserir um produto na hashtable
int inserir_tabela(Hashtable* table, Produto produto){
	//encontrando a posicao para inserir o novo produto dentro da hash
    short pos = hash(produto.pk, table->tam);
	int colisoes = 0;
	int inserido = 0;

	if(table->v[pos].estado == OCUPADO){
		pos++;
		for(colisoes = 1; colisoes < table->tam; colisoes++){
			if(pos > table->tam){
				pos = 0;
			}
			if(table->v[pos].estado != OCUPADO){
				strcpy(table->v[pos].pk, produto.pk);
				table->v[pos].rrn = nregistros;
				table->v[pos].estado = OCUPADO;
				inserido = 1;

				return colisoes;
			}
			pos++;
		}
	}else{
		strcpy(table->v[pos].pk, produto.pk);
		table->v[pos].rrn = nregistros;
		table->v[pos].estado = OCUPADO;
		inserido = 1;
	}
	if(inserido == 1){
		return colisoes;
	}else{
		return -1;
	}

}
//faz uma busca pelo produto passado como parametro na hashtable
Chave* buscar_tabela(Hashtable* table, char* pk){
	short pos = hash(pk, table->tam);

	if(table->v[pos].estado == OCUPADO){
		for(int i = 0; i < table->tam; i++){
			if(pos > table->tam){
				pos = 0;
			}
			if(strcmp(table->v[pos].pk, pk) == 0){
				return &table->v[pos];
			}
			pos++;
		}
	}
	return NULL;
}
//faz uma busca pelo produto lido da entrada na hashtable
void buscar(Hashtable table){
	char pk[TAM_PRIMARY_KEY];
	Chave* encontrou = NULL;

	scanf("%[^\n]%*c", pk);

	encontrou = buscar_tabela(&table, pk);

	if(encontrou){
		exibir_registro(encontrou->rrn);
	}else{
		printf(REGISTRO_N_ENCONTRADO);
	}
}
//cadastra determinado produto nos arquivos de indices e dados
void cadastrar(Hashtable* table)
{
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
    //TODO descomentar
    if (verifica_chave(novo.pk, *table) == 0)
        return;
    //imprime todos os dados do produto em uma string temporaria, para depois
    //ser passada para o arquivo principal
    sprintf(temp, "%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data,
            novo.ano, novo.preco, novo.desconto, novo.categoria);

    int aux = strlen(temp);
    //verifica se o registro possui tamanho menor que 192 bytes, se sim, devemos
    //preencher o resto do espaço com #
    while (aux < 192)
    {
        temp[aux] = '#';
        aux++;
    }
    //inserindo a string gerada no arquivo de dados
    char *fimArquivo = ARQUIVO + (192 * (nregistros));
    sprintf(fimArquivo, "%s", temp);
    fimArquivo += 192;
    *fimArquivo = '\0';

    //inserindo o produto no indice primario
    inserir(table, novo);

    nregistros++;
}
//carrega a tabela com base no arquivo de dados
void carregar_tabela(Hashtable *tabela){
    int tempnregistros = strlen(ARQUIVO) / TAM_REGISTRO;
    Produto novo;
    for(nregistros = 0; nregistros < tempnregistros; nregistros++){
        novo = recuperar_registro(nregistros);
        inserir_tabela(tabela, novo);
    }
}
//altera o desconto de determinado produto no arquivo de dados
int alterar_desconto(Hashtable table)
{
    char pkBusca[TAM_PRIMARY_KEY];
    //recebe pk do usuario
    scanf("%[^\n]%*c", pkBusca);
    //busca pk no indice primario
    Chave *retorno = buscar_tabela(&table, pkBusca);

    if (!retorno|| nregistros == 0)
    {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
    else
    {
        char desconto[100];
        scanf("%[^\n]%*c", desconto);

        int i = atoi(desconto);
        //conferindo se o desconto passado pelo usuario segue a formatacao necessaria
        while (i < 0 || i > 100 || strlen(desconto) != 3 || !(isdigit(desconto[0]))
                || !(isdigit(desconto[1])) || !(isdigit(desconto[2])))
        {
            printf(CAMPO_INVALIDO);
            scanf("%s%*c", desconto);
            i = atoi(desconto);
        }
        //recuperando o produto para ter acesso a seus campos
        Produto alterado = recuperar_registro(retorno->rrn);
        //obtendo o deslocamento necessario para encontrar o campo desconto do registro
        int deslocamento = strlen(alterado.pk) + strlen(alterado.nome) + strlen(alterado.marca)
        + strlen(alterado.data) + strlen(alterado.ano) + strlen(alterado.preco) + (retorno->rrn * 192) + 6;
        // //posicionando o apontador no inicio do campo desconto
        char* p = ARQUIVO + deslocamento;
        //escrevendo o novo desconto no arquivo de dados
        int z;
        for (z = 0; z < 3; z++)
        {
            p[z] = desconto[z];
        }
        return 1;
    }
}
//remove determinado produto da hashtable
int remover(Hashtable *tabela){
}
//remove determinado produto do arquivo de dados
void remover_arquivodedados(int rrn){
	if(rrn != -1 && nregistros > 0){
		char* posProduto = ARQUIVO + (192 * rrn);
		*(posProduto) = '*';
		*(posProduto + 1) = '|';
	}else{
		return;
	}
}
//imprime a hashtable como um todo
void imprimir_tabela(Hashtable tabela){

	    int imprimiu = 0;
	    for(int i = 0; i < tabela.tam; i++){
	        imprimiu = 0;
	        if(tabela.v[i].estado == OCUPADO){//imprime o primeiro registro daquela pos se nao for nulo
	            printf("[%d] Ocupado: %s", i, tabela.v[i].pk);
	            //imprime os demais registros daquela pos
	            imprimiu = 1;
	        }else{
				if(tabela.v[i].estado == LIVRE){
	            	printf("[%d] Livre", i);
				}
				else if(tabela.v[i].estado == REMOVIDO){
					printf("[%d] Removido", i);
				}
				imprimiu = 1;
	        }
	        if(imprimiu == 1){
	            printf("\n");
	        }
	    }
}
//libera a memoria alocada pela hashtable
void liberar_tabela(Hashtable* tabela){

}
//inicializa a hashtable
void criar_tabela(Hashtable* table, int tam){
    int tamanhoHash = prox_primo(tam);
    table->tam = tamanhoHash;
    table->v = (Chave*)calloc(tamanhoHash, sizeof(Chave));

    //inicializando todas as posicoes do vetor v como NULL
    for(int i = 0; i < tamanhoHash; i++){
        table->v[i].estado = LIVRE;
    }
    //inicializando nregistros
    nregistros = 0;
}
//Gera uma chave primária com base no produto fornecido como parâmetro
void gerarChave(Produto *p)
{
    int i = 0;
    //inserindo os 2 caracteres do nome
    int j;
    for (j = 0; j < 2; i++, j++)
    {
        if (p->nome[j] > 'Z')
            p->pk[i] = p->nome[j] - 32;
        else
            p->pk[i] = p->nome[j];
    }
    //inserindo os 2 caracteres da marca
    for (j = 0; j < 2; i++, j++)
    {
        if (p->marca[j] > 'Z')
            p->pk[i] = p->marca[j] - 32;
        else
            p->pk[i] = p->marca[j];
    }
    //inserindo os 4 digitos da data
    for (j = 0; j < 5; j++)
    {
        if (p->data[j] != '/')
        {
            p->pk[i] = p->data[j];
            i++;
        }
    }
    for (j = 0; j < 2; i++, j++)
    {
        p->pk[i] = p->ano[j];
    }
    p->pk[i] = '\0';
}
//verifica se a chave passada como parametro ja se encontra em ip
int verifica_chave(char* pk, Hashtable table)
{
    //TODO: finalizar implementacao
    Chave *foundIt = NULL;
    foundIt = buscar_tabela(&table, pk);
    if (foundIt)
    {
        printf(ERRO_PK_REPETIDA, pk);
        return 0;
    }
    return 1;
}
//função de hash
short hash(const char *chave, int tam){
    int posicao = 0;
    for(int i = 0; i < 8; i++){
        posicao += (i + 1) * f(chave[i]);
    }
    posicao = posicao % tam;
    return posicao;
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}
/*Retorna o primeiro número primo >= a*/
int prox_primo(int a){
    //testa-se 1 a 1 os numeros de 2 até a
    for(int i = 2; i < a; i++){
        //caso a seja divisivel por i, incrementamos a
        if(a % i == 0){
            a++;
        }
    }
    return a;
}
/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}
//Retorna um produto obtido do arquivo de dados
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp, "@");
    strcpy(j.pk, p);
    p = strtok(NULL, "@");
    strcpy(j.nome, p);
    p = strtok(NULL, "@");
    strcpy(j.marca, p);
    p = strtok(NULL, "@");
    strcpy(j.data, p);
    p = strtok(NULL, "@");
    strcpy(j.ano, p);
    p = strtok(NULL, "@");
    strcpy(j.preco, p);
    p = strtok(NULL, "@");
    strcpy(j.desconto, p);
    p = strtok(NULL, "@");
    strcpy(j.categoria, p);
    // gerarChave(&j);
    return j;
}
/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn<0)
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
	strncpy(categorias, j.categoria, strlen(j.categoria));
  for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    printf("%s ", cat);
	printf("\n");
	return 1;
}
