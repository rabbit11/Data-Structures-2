/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
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
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



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


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 // Cadastra novo produto no sistema de arquivos
void cadastrar_produto(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int* ncat);

//Altera produto correspondente a chave primaria passada como parâmetro
int alterar_produto(Ip* iprimary, int* nregistros, Isf* iprice);

//Remove produto com base na chave primaria fornecida
int remover_produto(Ip* iprimary, int* nregistros);

 // Verifica se a chave passada como parâmetro ja existe no sistema de arquivos
int verifica_chave(char* chave, int* nregistros, Ip* iprimary);

 //Gera uma chave para o produto passado como parametro
void gerarChave(Produto* p);

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

//Cria o seguinte índice
void criar_iproduct(Ip* iprimary, Is* iproduct, int* nregistros);

//Cria o seguinte indice
void criar_ibrand(Ip* iprimary, Is* iproduct, int* nregistros);

//Cria o seguinte indice
void criar_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat);

//Insere produto no índice icategory
void insere_icategory(Ip* iprimary,Ir* icategory,int* nregistros, int* ncat, Produto aux);

//Cria o seguinte indice
void criar_iprice(Ip* iprimary, Isf* iprice, int* nregistros);

//compara o nome de dois produtos
int compare_nome(const void* p1, const void* p2);

//compara a marca de dois protudos
int compare_marca(const void* p1, const void* p2);

//compara dois precos
int compare_preco(const void* p1, const void* p2);

//compara a categoria de dois produtos
int compare_categoria(const void* p1, const void* p2);

//compara uma string com uma categoria do indice icategory
int compare_categoria_busca(const void* p1, const void* p2);

//busca um registro utilizando um argumento inserido pelo usuario
void busca_registro(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int* ncat);

//busca um registro utilizando a chave primaria inserida
Ip* busca_primary(Ip* iprimary, char* chaveBusca, int* nregistros);

//busca um registro utilizando o nome inserido
Is* busca_iproduct(Is* iproduct, char* chaveNome, int* nregistros);
//busca um registro utilizando a categoria
Ir* busca_icategory(Ir* icategory, char* chaveCategoria, int* nregistros);
//busca um registor utilizando a marca
Is* busca_ibrand(Is* ibrand, char* chaveMarca, int* nregistros);
//libera toda a memoria alocada pelos indices
void liberar(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice, int* nregistros, int* ncat);
//libera campos excluidos do arquivo de dados
void liberar_espaco(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice, int* nregistros, int* ncat);
//libera memoria alocada pelo indice icategory
void libera_icategory(Ir* icategory, int* ncat);
//lista todos os produtos de diferentes formas
void listagens(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice,
	 		int nregistros, int ncat);
//busca linear no indice iprimary
Ip* lsearch_iprimary(Ip* iprimary, char* pk, int nregistros);
//busca linear no indice icategory
Ir* lsearch_icategory(Ir* icategory, int ncat, char* categoria);
//busca linear para encontrar um produto dentro de uma categoria
ll* lsearch_icategory_pk(Ir* listaCategoria, char* chave);
//busca linear no indice iproduct
Is* lsearch_iproduct(Is* iproduct, int nregistros, char* nome);
//busca linear no indice ibrand pela pk
Is* lsearch_ibrand_pk(Is* ibrand, int nregistros, char* pk);
//busca pelos semelhantes em iproduct
Is* lsearch_iproduct_semelhantes(Is* iproduct, int nregistros, char* chaveNome);
/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);
	/*Alocar e criar índices secundários*/

	//alocando e criando iproduct
	Is *iproduct = (Is*) malloc(MAX_REGISTROS * sizeof(Is));
	if(!iproduct){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	 criar_iproduct(iprimary, iproduct, &nregistros);

	//alocando e criando ibrand
	Is *ibrand = (Is*) malloc(MAX_REGISTROS * sizeof(Is));
	if(!ibrand){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_ibrand(iprimary, ibrand, &nregistros);
	//alocando e criando icategory
	Ir *icategory = (Ir*) malloc(MAX_REGISTROS * sizeof(Ir));
	if(!icategory){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_icategory(iprimary, icategory, &nregistros, &ncat);
	//alocando e criando iprice
	Isf *iprice = (Isf*) malloc(MAX_REGISTROS * sizeof(Isf));
	if(!iprice){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprice(iprimary, iprice, &nregistros);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				cadastrar_produto(iprimary, &nregistros, iproduct, ibrand, icategory, iprice, &ncat);
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);

				if(alterar_produto(iprimary, &nregistros, iprice))
					printf(SUCESSO);
				else
					printf(FALHA);
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);

				if(remover_produto(iprimary, &nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);

			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
				busca_registro(iprimary, &nregistros, iproduct, ibrand,	icategory, iprice, &ncat);

			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
				listagens(iprimary, iproduct, ibrand,icategory, iprice, nregistros, ncat);

			break;
			case 6:
				/*libera espaço*/
				liberar_espaco(iprimary, iproduct, ibrand, icategory, iprice, &nregistros, &ncat);
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				if(strlen(ARQUIVO) == 0){
					printf(ARQUIVO_VAZIO);
				}
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
				liberar(iprimary, iproduct, ibrand,icategory, iprice, &nregistros, &ncat);
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* ==========================================================================
 * ============================ FUNÇÕES IMPLEMENTADAS ============================
 * =============================== PELO ALUNO ============================== */

//Cadastra novo Produto nos arquivos de dados e índices
void cadastrar_produto(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
					   Ir* icategory, Isf* iprice, int* ncat){
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
	if(verifica_chave(novo.pk, nregistros, iprimary) == 0)
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
	char* fimArquivo = ARQUIVO + (192 * (*nregistros));
	sprintf(fimArquivo, "%s", temp);
	fimArquivo += 192;
	*fimArquivo = '\0';

	//verificando se o novo produto havia sido removido anteriormente
	Ip* removido = lsearch_iprimary(iprimary, novo.pk, *nregistros);
	if(removido && removido->rrn == -1){
		iprimary[*nregistros].rrn = *nregistros;
	}else{
		//inserindo o produto no indice primario
		iprimary[*nregistros].rrn = *nregistros;
		strcpy(iprimary[*nregistros].pk, novo.pk);
	}
	//inserindo no indice secundario iproduct
	strcpy(iproduct[*nregistros].string, novo.nome);
	strcpy(iproduct[*nregistros].pk, iprimary[*nregistros].pk);

	//inserindo no indice secundario ibrand
	strcpy(ibrand[*nregistros].string, novo.marca);
	strcpy(ibrand[*nregistros].pk, iprimary[*nregistros].pk);

	//inserindo no índice secundario icategory
	insere_icategory(iprimary, icategory, nregistros, ncat, novo);

	//inserindo no indice secundario iprice
	float desconto = atof(novo.desconto);
	float preco = atof(novo.preco);
	preco = (preco * (100-desconto))/ 100.0;
	preco = preco * 100;
	preco = ((int)preco/(float)100);
	iprice[*nregistros].price = (float)preco;
	strcpy(iprice[*nregistros].pk, novo.pk);

	*nregistros+= 1;

	qsort(iprimary, *nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);
	qsort(iproduct, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_nome);
	qsort(ibrand, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_marca);
	qsort(icategory, *ncat, sizeof(Ir), (int(*)(const void*, const void*))compare_categoria);
	qsort(iprice, *nregistros, sizeof(Isf), (int(*)(const void*, const void*))compare_preco);
}
//Altera produto correspondente a chave primaria passada como parâmetro
int alterar_produto(Ip* iprimary, int* nregistros, Isf* iprice){
	char chave[TAM_PRIMARY_KEY];
	char desconto[TAM_DESCONTO];

	scanf("%[^\n]%*c", chave);

	// printf("E OTRAS PARADA %s %s\n",chave, desconto);
	if(*nregistros == 0){
		printf(ARQUIVO_VAZIO);
		return 0;
	}
	int i = atoi(desconto);//transformando desconto em um int

	//buscando se a chave passada pelo usuario se encontra no arquivo
	Ip* temp = lsearch_iprimary(iprimary, chave, *nregistros);
	if(!temp || temp->rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}else{
		scanf("%[^\n]%*c", desconto);
		//conferindo se o desconto passado pelo usuario segue a formatacao necessaria
		while(i < 0 || i > 100 || strlen(desconto) != 3){
			printf(CAMPO_INVALIDO);
			scanf("%s%*c", desconto);
			i = atoi(desconto);
		}
		//checa se o registro nao foi removido
		if(temp->rrn != -1){
			int deslocamento = 0;
			Produto aux = recuperar_registro(temp->rrn);

			//encontrando deslocamento necessario para a escrita no arquivo de dados
			deslocamento = strlen(aux.nome) + strlen(aux.marca) + strlen(aux.data) + strlen(aux.ano)
							+ strlen(aux.preco) + (temp->rrn * 192) + 5;
			char* auxiliar = ARQUIVO + deslocamento;
			//escrevendo o novo desconto no arquivo de dados
			for(int i = 0; i < 3; i++){
				auxiliar[i] = desconto[i];
			}
			//recriando o indice iprice com os novos valores
			criar_iprice(iprimary, iprice, nregistros);
		}
	}
	return 1;
}
//Remove produto com base na chave primaria inserida
int remover_produto(Ip* iprimary, int* nregistros){
	char chave[TAM_PRIMARY_KEY];
	Ip *aux;

	scanf("%s%*c", chave);
	if(*nregistros <= 0){
		printf(ARQUIVO_VAZIO);
		return 0;
	}
	aux = lsearch_iprimary(iprimary, chave, *nregistros);

	if(aux && aux->rrn != -1){
		char* posProduto = ARQUIVO + (192 * aux->rrn);
		*(posProduto) = '*';
		*(posProduto + 1) = '|';
		aux->rrn = -1;
		return 1;
	}else{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
}
//Gera uma chave primária com base no produto fornecido como parâmetro
void gerarChave(Produto* p){
	int i = 0;
	//inserindo os 2 caracteres do nome
	for(int j = 0;j < 2; i++, j++){
		if(p->nome[j] > 'Z')
			p->pk[i] = p->nome[j] - 32;
		else
			p->pk[i] = p->nome[j];
	}
	//inserindo os 2 caracteres da marca
	for(int j = 0;j < 2; i++, j++){
		if(p->marca[j] > 'Z')
			p->pk[i] = p->marca[j] - 32;
		else
			p->pk[i] = p->marca[j];
	}
	//inserindo os 4 digitos da data
	for(int j = 0;j < 5; j++){
		if(p->data[j] != '/'){
			p->pk[i] = p->data[j];
			i++;
		}
	}
	for(int j = 0;j < 2; i++, j++){
		p->pk[i] = p->ano[j];
	}
	p->pk[i] = '\0';
}
//Verifica se já existe uma chave no arquivo igual a chave passada como parâmetro
int verifica_chave(char* chave, int* nregistros, Ip* iprimary){
	Ip* foundIt = NULL;
	foundIt = lsearch_iprimary(iprimary, chave, *nregistros);
	if(foundIt){
		if(foundIt->rrn != -1){
			printf(ERRO_PK_REPETIDA, chave);
			return 0;
		}
	}
	return 1;
}
//cria o índice primário com base no arquivo de dados
void criar_iprimary(Ip *indice_primario, int* nregistros){
	char archive[TAM_ARQUIVO];

	if(*nregistros == 0){
		indice_primario->rrn = 0;
	}else{
		strcpy(archive, ARQUIVO);
		Produto aux;
		char* temp;
		temp = strtok(archive,"@#");
		int i = 0;

		while(temp != NULL){
			strcpy(aux.nome, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.marca, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.data, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.ano, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.preco, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.desconto, temp);
			temp = strtok(NULL,"@#");
			strcpy(aux.categoria, temp);
			temp = strtok(NULL,"@#");
			gerarChave(&aux);

			if(verifica_chave(aux.pk, nregistros, indice_primario) == 0)
				return;

			strcpy(indice_primario[i].pk, aux.pk);
			indice_primario[i].rrn = i;
			i++;
		}
		qsort(indice_primario, *nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);

		//print do indice primario para teste
		 // for(int j = 0; j < *nregistros; j++)
		 // 	printf("chave primaria:%s / RRN: %d\n", indice_primario[j].pk, indice_primario[j].rrn);
	}

}
//cria indice secundario com nome do produto e chave primaria
void criar_iproduct(Ip* iprimary, Is* iproduct, int* nregistros){
	Produto aux;

	if(*nregistros == 0){
		return;
	}
	//inserindo produto no indice secundario
	for(int i =0; i < *nregistros; i++){
		aux = recuperar_registro(i);
		//checando se o produto a ser inserido não foi removido
		Ip* prim = lsearch_iprimary(iprimary, aux.pk, *nregistros);
		if(prim){
			if(prim->rrn != -1){
				strcpy(iproduct[i].string, aux.nome);
				strcpy(iproduct[i].pk, aux.pk);
			}
		}
	}
	qsort(iproduct, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_nome);

	//print para testes da corretude da funcao
	  // for(int j = 0; j < *nregistros; j++)
	  	// printf("nome: %s / chave: %s\n", iproduct[j].string, iproduct[j].pk);
}
//cria indice secundario com nome da marca e chave primaria
void criar_ibrand(Ip* iprimary, Is* ibrand, int* nregistros){
	Produto aux;

	if(strlen(ARQUIVO) == 0){
		return;
	}
	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);
		//checando se o produto a ser inserido não foi removido
		Ip* prim = lsearch_iprimary(iprimary, aux.pk, *nregistros);
		if(prim){
			if(prim->rrn != -1){
				strcpy(ibrand[i].string, aux.marca);
				strcpy(ibrand[i].pk, aux.pk);
			}
		}
	}
	qsort(ibrand, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_marca);

	//print para testes da corretude da funcao
	  // for(int j = 0; j < *nregistros; j++)
	  // 	printf("marca: %s / chave: %s\n", ibrand[j].string, ibrand[j].pk);
}
//cria indice secundario com as categorias e chave primaria
void criar_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat){
	Produto aux;

	if(*nregistros == 0){
		return;
	}
	for(int i =0; i < *nregistros;i++){
		aux = recuperar_registro(i);
		//checando se o produto a ser inserido já não foi removido
		Ip* prim = lsearch_iprimary(iprimary, aux.pk, *nregistros);
		if(prim){
			if(prim->rrn != -1){
			insere_icategory(iprimary, icategory, nregistros, ncat, aux);
			}
		}
	}
	qsort(icategory, *ncat, sizeof(Ir), (int(*)(const void*, const void*))compare_categoria);
}
//insere no indice secundario icategory
void insere_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat, Produto aux){
	char temp[TAM_CATEGORIA], *p;
	int inserido = 0;
	//TODO VER Quando entra na condição de encontrar igual e oq acontece e onde cria a nova categoria
	strcpy(temp, aux.categoria);
	p = strtok(temp, "|");
	while(p != NULL){
			//verifica se determinada categoria ja se encontra no indice
			//se sim, adiciona uma nova chave primaria a lista, em ordem
		if(*ncat > 0){
				//if ncat > 0 evita que compare na primeira insercao, ja que
				//em ncat == 0 nao ha elementos no indice
			Ir* findrepetida = lsearch_icategory(icategory, *ncat, p);
			if(findrepetida){
				ll* percorre = findrepetida->lista;
				ll* novo = (ll*)calloc(sizeof(ll), 1);
				novo->prox = NULL;
				strcpy(novo->pk, aux.pk);
				ll* aux2 = NULL;

				//inserindo na primeira posicao
				if(strcmp(aux.pk, percorre->pk) < 0){
					novo->prox = findrepetida->lista;
					findrepetida->lista = novo;
				//inserindo como segundo elemento da lista
				}else if(percorre->prox == NULL){
					percorre->prox = novo;
				//inserindo nas demais posicoes
				}else{
					aux2 = percorre;
					percorre = percorre->prox;
					while(percorre->prox != NULL){
						if(strcmp(aux.pk, percorre->pk) < 0){
							aux2->prox = novo;
							novo->prox = percorre;
							inserido = 1;
							break;
						}
						aux2 = percorre;
						percorre = percorre->prox;
					//inserindo na ultima posicao
					}if(inserido == 0){
						if(strcmp(aux.pk, percorre->pk) < 0){
							aux2->prox = novo;
							novo->prox = percorre;
						}else{
							percorre->prox = novo;
						}
					}
				}
			//insercao em nova categoria com ncat > 0
			}else{
				strcpy(icategory[*ncat].cat, p);
				ll* novo = (ll*)calloc(sizeof(ll), 1);
				novo->prox = NULL;
				strcpy(novo->pk, aux.pk);
				icategory[*ncat].lista = novo;
				*ncat += 1;
			}
		//insercao em nova categoria com ncat == 0
		}else{
			strcpy(icategory[*ncat].cat, p);
			ll* novo = (ll*)calloc(sizeof(ll), 1);
			strcpy(novo->pk, aux.pk);
			icategory[*ncat].lista = novo;
			novo->prox = NULL;
			*ncat += 1;
		}
		p = strtok(NULL, "|");
	}
}
//cria indice secundario com preco e chave primaria
void criar_iprice(Ip* iprimary, Isf* iprice, int* nregistros){
	Produto aux;
	float desconto;

	if(*nregistros == 0){
		return;
	}
	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);
		//checando se o produto a ser inserido já não foi removido
		Ip* prim = lsearch_iprimary(iprimary, aux.pk, *nregistros);
		if(prim){
			if(prim->rrn != -1){
				desconto = atof(aux.desconto);
				float preco = (float)atof(aux.preco);
				preco = (preco * (100-desconto))/ 100.0;
				preco = preco * 100;
				preco = ((int)preco/(float)100);
				iprice[i].price = (float)preco;
				strcpy(iprice[i].pk, aux.pk);
			}
		}
	}
	qsort(iprice, *nregistros, sizeof(Isf), (int(*)(const void*, const void*))compare_preco);

	// for(int j = 0; j < *nregistros; j++)
	//   printf("preco: %lf / chave: %s\n", iprice[j].price, iprice[j].pk);
}
//compara os campos de nome de dois produtos
int compare_nome(const void* nome1, const void* nome2){
	Is* produto1 = (Is*)nome1;
	Is* produto2 = (Is*)nome2;

	if(strcmp(produto1->string, produto2->string) < 0)
	   return -1;
	else if(strcmp(produto1->string, produto2->string) == 0){
	   return strcmp(produto1->pk, produto2->pk);
	}else if(strcmp(produto1->string, produto2->string) > 0){
	   return 1;
	}
}
//compara os campos de marca de dois produtos
int compare_marca(const void* marca1, const void* marca2){
	 Is* produto1 = (Is*)marca1;
	 Is* produto2 = (Is*)marca2;

	if(strcmp(produto1->string, produto2->string) < 0)
		return -1;
	else if(strcmp(produto1->string, produto2->string) == 0){
		return strcmp(produto1->pk, produto2->pk);
	}else if(strcmp(produto1->string, produto2->string) > 0){
		return 1;
	}
}
//compara os campos de preco de dois produtos
int compare_preco(const void* preco1, const void* preco2){
	if(*(float*)preco1 < *(float*) preco2)
		return -1;
	else if(*(float*)preco1 == *(float*) preco2)
		return 0;
	else if(*(float*)preco1 > *(float*) preco2)
		return 1;

}
//compara o campo de categoria de dois produtos
int compare_categoria(const void* p1, const void* p2){
	Ir* produto1 = (Ir*)p1;
	Ir* produto2 = (Ir*)p2;

   if(strcmp(produto1->cat, produto2->cat) < 0)
	   return -1;
   else if(strcmp(produto1->cat, produto2->cat) == 0){
	   return 0;
   }else if(strcmp(produto1->cat, produto2->cat) > 0){
	   return 1;
   }
}
//compara uma string com uma categoria do indice icategory
int compare_categoria_busca(const void* p1, const void* p2){
	char* categoriaP = (char*)p1;
	Ir* categoriaIndice = (Ir*)p2;

   if(strcmp(categoriaP, categoriaIndice->cat) < 0)
	   return -1;
   else if(strcmp(categoriaP, categoriaIndice->cat) == 0){
	   return 0;
   }else if(strcmp(categoriaP, categoriaIndice->cat) > 0){
	   return 1;
   }
}
//busca um registro utilizando diversos argumentos
void busca_registro(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int* ncat){
	int opBusca = 0;
	char chavePrim[TAM_PRIMARY_KEY];
	char chaveNome[TAM_NOME];
	char chaveMarca[TAM_MARCA];
	char chaveCategoria[TAM_CATEGORIA];
	int imprimiu = 0;

	scanf("%d%*c", &opBusca);
	if(!nregistros){
		printf(ARQUIVO_VAZIO);
		return;
	}
	switch(opBusca){
		case 1://busca pela chave primaria
			scanf("%[^\n]%*c", chavePrim);

			Produto temp;
			Ip* aux;
			aux = lsearch_iprimary(iprimary, chavePrim, *nregistros);

			if(aux && aux->rrn != -1){
				exibir_registro(aux->rrn, 0);
				imprimiu = 1;
		 	}else{
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}
		break;
		//TODO busca pelo nome produto
		case 2://busca pelo nome do produto
			scanf("%[^\n]%*c", chaveNome);
			Is* aux2;
			aux2 = lsearch_iproduct(iproduct, *nregistros, chaveNome);
			if(aux2){
				Ip* prim;
				prim = lsearch_iprimary(iprimary, aux2->pk, *nregistros);
				if(prim){
					//imprimindo os produtos de mesmo nome
					Is* temp = aux2;
					while(temp && strcmp(temp->string, chaveNome) == 0){
						prim = lsearch_iprimary(iprimary, temp->pk, *nregistros);

						if(prim && prim->rrn != -1){
							exibir_registro(prim->rrn, 0);
							imprimiu = 1;
						}
						temp = temp - 1;
					}
					temp = aux2 + 1;
					while(temp && strcmp(temp->string, chaveNome) == 0){
						prim = lsearch_iprimary(iprimary, temp->pk, *nregistros);

						if(prim && prim->rrn != -1){
							exibir_registro(prim->rrn, 0);
							imprimiu = 1;
						}
						temp = temp + 1;
					}
				}
			 }if(imprimiu == 0){
				 printf(REGISTRO_N_ENCONTRADO);
				 return;
			 }
		break;
		//TODO printa o mesmo termo mais de uma vez
		case 3://busca pela marca e categoria
			scanf("%[^\n]%*c", chaveMarca);
			scanf("%[^\n]%*c", chaveCategoria);

			Is* auxMarca;
			Ir* auxCategoria;
			//procuro marca e categoria em seus respectivos indices
			auxMarca = lsearch_iproduct(ibrand, *nregistros, chaveMarca);
			printf("NA ROPA DO HOMI ARANHA %s\n", auxMarca->pk);
			auxCategoria = lsearch_icategory(icategory, *ncat, chaveCategoria);
			if(auxMarca && auxCategoria){
				printf("EU SOU O PITER PARKER %s\n", auxCategoria->cat);
				//vejo se o pk correspondente a marca se encontra naquela categoria
				ll* percorre = lsearch_icategory_pk(auxCategoria, auxMarca->pk);
				if(percorre){
					Is* temp;
					ll* auxPercorre = percorre;
					//imprimindo e encontrando os semelhantes
					while(auxPercorre){
						//vejo se o pk correspondente aquele termo da categoria, se encontra
						//na marca (redundante para o primeiro termo a ser impresso)
						temp = lsearch_ibrand_pk(ibrand, *nregistros, auxPercorre->pk);
						if(temp && strcmp(temp->string, auxMarca->string) == 0){
							Ip* prim = lsearch_iprimary(iprimary, auxPercorre->pk, *nregistros);
							if(prim && prim->rrn != -1){
								exibir_registro(prim->rrn, 0);
								imprimiu = 1;
							}
						}
						auxPercorre = auxPercorre->prox;
					}
				}else{
					printf(REGISTRO_N_ENCONTRADO);
					return;
				}
			}else{
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}if(imprimiu == 0){
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}

		break;
	}
}
//realiza busca no indice primario
Ip* busca_primary(Ip* iprimary, char* chaveBusca, int* nregistros){
	Ip* aux;
	aux = (Ip*)bsearch(chaveBusca, iprimary, *nregistros, sizeof(Ip),
			(int(*)(const void*, const void*))strcmp);
	if(aux){
		return aux;
	}else{
		return NULL;
	}
}
//realiza busca no indice secundario iproduct
Is* busca_iproduct(Is* iproduct, char* chaveNome, int* nregistros){
	Is* aux;
	aux = (Is*)bsearch(chaveNome, iproduct, *nregistros, sizeof(Is),
	(int(*)(const void*, const void*))strcmp);

	if(aux)
		return aux;
	else
		return NULL;
}
//realiza busca no indice secundario ibrand
Is* busca_ibrand(Is* ibrand, char* chaveMarca, int* nregistros){
	Is* aux;

	aux = (Is*)bsearch(chaveMarca, ibrand, *nregistros, sizeof(Is),
			(int(*)(const void*, const void*)) compare_marca);
	if(aux)
		return aux;
	else
		return NULL;
}
//realiza busca no indice secundario icategory
Ir* busca_icategory(Ir* icategory, char* chaveCategoria, int* nregistros){
	Ir* aux;

	aux = (Ir*)bsearch(chaveCategoria, icategory, *nregistros, sizeof(Ir),
			(int(*)(const void*, const void*)) compare_categoria);
	if(aux)
		return aux;
	else
		return NULL;
}
//lista registros de diferentes maneiras
void listagens(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice,
	 		int nregistros, int ncat){
	int opLista;
	Ip* temp;
	ll* aux;
	Ip* tempbrand;
	Ip* tempprice;
	char cat[TAM_CATEGORIA];
	int print = 0;

	scanf("%d%*c", &opLista);

	if(nregistros < 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	switch(opLista){
		case 1://lista por pk
			for(int j = 0; j < nregistros; j++){
				if(iprimary[j].rrn != -1){
					if(print == 1 && j != nregistros -1)
						printf("\n");
					exibir_registro(iprimary[j].rrn, 0);
					print = 1;
				}
			}
		break;
		case 2://lista por categoria
			scanf("%[^\n]%*c", cat);
			aux = NULL;

			for(int i = 0; i < ncat; i++){
				if(strcmp(icategory[i].cat, cat) == 0){
					aux = icategory[i].lista;
				}while(aux != NULL){
					temp = lsearch_iprimary(iprimary, aux->pk, nregistros);
					if(temp && temp->rrn != -1){
						if(print == 1 && aux->prox)
							printf("\n");
						exibir_registro(temp->rrn, 0);
						print = 1;
					}
					aux = aux->prox;
				}
			}
		break;
		case 3://listar por marca em ordem alfabética
			print = 0;
			for(int j = 0; j < nregistros; j++){
					tempbrand = lsearch_iprimary(iprimary, ibrand[j].pk, nregistros);
					if(tempbrand && tempbrand->rrn != -1){
						if(print == 1 && j != nregistros -1)
							printf("\n");
						exibir_registro(tempbrand->rrn, 0);
						print = 1;
					}
			}
		break;
		case 4://listar por preço com desconto aplicado
			print = 0;
			for(int j = 0; j < nregistros; j++){
				tempprice = lsearch_iprimary(iprimary, iprice[j].pk, nregistros);
				if(tempprice && tempprice->rrn != -1){
					if(print == 1 && j != nregistros -1)
						printf("\n");
					exibir_registro(tempprice->rrn, 1);
					print = 1;
				}
			}
		break;
		default:
		break;
	}if(print == 0){
		printf(REGISTRO_N_ENCONTRADO);
	}
}
//libera toda memória antes alocada
void liberar(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice, int* nregistros, int* ncat){
	//liberando memoria de icategory
	libera_icategory(icategory, ncat);

	//liberando memoria de iprice
	free(iprice);

	//liberando memoria de ibrand
	free(ibrand);

	//liberando memoria de iproduct
	free(iproduct);

	//liberando memoria de iprimary
	free(iprimary);
}
//libera memoria de icategory
void libera_icategory(Ir* icategory, int* ncat){
	ll *aux, *temp;

	for(int i = 0; i < *ncat; i++){
		aux = icategory[i].lista;
		while(aux != NULL){
			temp = aux;
			aux = aux->prox;
			free(temp);
		}
	}
}
//libera do arquivo de dados os registros removidos
void liberar_espaco(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice, int* nregistros, int* ncat){
	char* p = ARQUIVO;
	char arquivoAux[TAM_ARQUIVO];
	int alterou = 0, j = 0;

	for(int i = 0;p[i] != '\0' && i < TAM_ARQUIVO; i++){
		if(p[i] == '*' && p[i + 1] == '|'){
			alterou++;
			p += 191;
		}else{
			arquivoAux[j] = p[i];
			j++;
		}
	}
	arquivoAux[j] = '\0';
	strcpy(ARQUIVO, arquivoAux);

	if(alterou > 0){
		*nregistros = *nregistros - alterou;
		*ncat = 0;
		//desfazendo os indices antes de refazelos
		free(iprimary);
		iprimary = (Ip *) calloc (MAX_REGISTROS, sizeof(Ip));
		free(iproduct);
		iproduct = (Is *) calloc (MAX_REGISTROS, sizeof(Is));
		free(ibrand);
		ibrand = (Is *) calloc (MAX_REGISTROS, sizeof(Is));
		free(iprice);
		iprice = (Isf *) calloc (MAX_REGISTROS, sizeof(Isf));
		libera_icategory(icategory, ncat);
		free(icategory);
		icategory = (Ir *) calloc(MAX_REGISTROS, sizeof(Ir));

		criar_iprimary(iprimary, nregistros);
		criar_iproduct(iprimary, iproduct, nregistros);
		criar_ibrand(iprimary, ibrand, nregistros);
		criar_icategory(iprimary, icategory, nregistros, ncat);
		criar_iprice(iprimary, iprice, nregistros);
	}
}
//busca linear para encontrar uma categoria no icategory
Ir* lsearch_icategory(Ir* icategory, int ncat, char* categoria){
	for(int i = 0;i < ncat; i++){
		if(strcmp(icategory[i].cat, categoria) == 0){
			return &icategory[i];
		}
	}
	return NULL;
}
//busca linear para encontrar produto dentro de iprimary
Ip* lsearch_iprimary(Ip* iprimary, char* pk, int nregistros){
	for(int i = 0; i < nregistros ;i++){
		if(strcmp(iprimary[i].pk, pk) == 0){
			return &iprimary[i];
		}
	}
	return NULL;
}
//busca linear para encontrar um produto dentro de uma categoria
ll* lsearch_icategory_pk(Ir* listaCategoria, char* chave){
	ll* aux = listaCategoria->lista;
	while(aux != NULL){
		if(strcmp(aux->pk, chave) == 0){
			return aux;
		}
		aux = aux->prox;
	}
	return NULL;
}
//busca linear para encontrar um nome no iproduct
Is* lsearch_iproduct(Is* iproduct, int nregistros, char* nome){
	for(int i =0; i <nregistros;i++){
		if(strcmp(iproduct[i].string, nome) == 0){
			return &iproduct[i];
		}
	}
	return NULL;
}
//busca linear pela pk no indice ibrand
Is* lsearch_ibrand_pk(Is* ibrand, int nregistros, char* pk){
	for(int i =0; i <nregistros;i++){
		if(strcmp(ibrand[i].pk, pk) == 0){
			return &ibrand[i];
		}
	}
	return NULL;
}
//busca pelos semelhantes em iproduct
Is* lsearch_iproduct_semelhantes(Is*iproduct, int nregistros, char*nome){
	Is saida[nregistros];
	int j = 0;
	for(int i =0; i <nregistros;i++){
		if(strcmp(iproduct[i].string, nome) == 0){
			saida[j] = iproduct[i];
			j++;
		}
	}
}
// ==========================================================================

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
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
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);
	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}


//Carrega arquivo e retorna o número de registros presentes no mesmo
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
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


/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}
