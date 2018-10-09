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
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
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
 void cadastrar_produto(Ip *iprimary, int nregistros);

 // Verifica se a chave passada como parâmetro ja existe no sistema de arquivos
 int verifica_chave(char* chave, int nregistros, Ip* iprimary);

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
	criar_ibrand(iprimary, iproduct, &nregistros);

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
				/*cadastro*/
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				/*
				if(alterar([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				/*
				if(remover([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
			break;
			case 6:
				/*libera espaço*/
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				//imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
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
void cadastrar_produto(Ip *iprimary, int nregistros){
	char temp[TAM_REGISTRO];
	Produto novo;

	//obtem do usuario informações sobre o produto
	scanf("%s", novo.nome);
	scanf("%s", novo.marca);
	scanf("%s", novo.data);
	scanf("%s", novo.ano);
	scanf("%s", novo.preco);
	scanf("%s", novo.desconto);
	scanf("%s", novo.categoria);

	//TODO campo categoria a entrada ja vem com as barras separando mais de uma
	//categoria?

	//gera uma chave para este novo produto
	gerarChave(&novo);

	//checa se a chave gerada já existia no sistema, se sim a função se encerra
	if(verifica_chave(novo.pk, nregistros, iprimary) == 0)
		return;

	//imprime todos os dados do produto em uma string temporaria, para depois
	//ser passada para o arquivo principal
	sprintf(temp,"%s@%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.data,
			novo.ano, novo.preco, novo.desconto, novo.categoria);
			//TODO DEVO ME PREOCUPAR COM ESSES WARNINGS ACIMA?
			//TODO devo imprimir a chave primaria no arquivo de dados?

	int aux = strlen(temp);

	//verifica se o registro possui tamanho menor que 192 bytes, se sim, devemos
	//preencher o resto do espaço com #
	while(aux < 192){
		temp[aux] = '#';
		aux++;
	}
	//inserindo a string gerada no arquivo de dados
	strncpy(ARQUIVO, temp, 192);

	//TODO inserir o produto no indice primario e secundarios

	//inserindo o produto no indice primario
		//TODO para este índice estou inserindo no fim e depois ordenando, há uma
		//melhor forma de fazer isso?
	int insere = (10 * nregistros) + 1;

	iprimary[insere].rrn = nregistros;
	strcpy(iprimary->pk, novo.pk);

	qsort(iprimary, nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);


}

//Gera uma chave primária com base no produto fornecido como parâmetro
void gerarChave(Produto* p){
	int i = 0;
	//inserindo os 2 caracteres do nome
	for(i = 0;i < 2; i++){
		if(p->nome[i] > 'Z')
			p->pk[i] = p->nome[i] - 32;
		else
			p->pk[i] = p->nome[i];
	}
	//printf("chave: %c\n", p->pk);
	//inserindo os 2 caracteres da marca
	for(int j = 0;j < 2; i++, j++){
		if(p->marca[j] > 'Z')
			p->pk[i] = p->marca[j] - 32;
		else
			p->pk[i] = p->marca[j];
	}
	//inserindo os 4 digitos da data
	for(int j = 0;j < 5; j++){
		if(j != 2){
			p->pk[i] = p->data[j];
			i++;
		}
	}
	//inserindo os 2 digitos do ano de lançamento
	for(int j = 0;j < 2; i++, j++){
		p->pk[i] = p->ano[j];
	}
}

//Verifica se já existe uma chave no arquivo igual a chave passada como parâmetro
int verifica_chave(char* chave, int nregistros, Ip* iprimary){
	int *foundIt = NULL;

	// char temp[TAM_ARQUIVO];
	// strcpy(temp, ARQUIVO);

	//TODO perguntar aos monitores o porque desse xenanigans antes do strcmp
	foundIt = (int*)bsearch(chave, iprimary, nregistros, 11, (int(*)(const void*, const void*))strcmp);

	if(foundIt != 0){
		printf(ERRO_PK_REPETIDA, chave);
		return 0;
	}
	return 1;
}

//cria o índice primário com base no arquivo de dados
void criar_iprimary(Ip *indice_primario, int* nregistros){

	//char* p = ARQUIVO;
	char archive[TAM_ARQUIVO];

	if(strlen(ARQUIVO) == 0){
		indice_primario->rrn = 0;
	}else{
		strcpy(archive, ARQUIVO);
		//for(int j = 0; sscanf(p,"%c") != '#'; j++){
			// sscanf(p,"%[^@]s", aux->nome);
			// sscanf(p,"%[^@]s", aux->marca);
			// sscanf(p,"%[^@]s", aux->data);
			// sscanf(p,"%[^@]s", aux->ano);
			// sscanf(p,"%[^@]s", aux->preco);
			// sscanf(p,"%[^@]s", aux->desconto);
			// sscanf(p,"%[^@]s", aux->categoria);

		Produto aux;
		char* temp;
		temp = strtok(archive,"@#");
		int i = 0;

		//TODO seria o método abaixo muito ineficiente? pois não sabia como trabalhar
		//com o fato do sscanf nao mover o ponteiro
		while(temp != NULL){
			//printf("%s\n", temp);
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
			verifica_chave(aux.pk, *nregistros, indice_primario);
			//printf("%s\n", aux.pk);

			strcpy(indice_primario[i].pk, aux.pk);
			//TODO rrn = i ou i * 192?
			indice_primario[i].rrn = i;
			i++;
		}
		qsort(indice_primario, *nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);

		//print do indice primario para teste
		 for(int j = 0; j < *nregistros; j++)
		 	printf("chave primaria:%s / RRN: %d\n", indice_primario[j].pk, indice_primario[j].rrn);
	}

}
//cria indice secundario com nome do produto e chave primaria
void criar_iproduct(Ip* iprimary, Is* iproduct, int* nregistros){
	Produto aux;
	for(int i =0; i < *nregistros; i++){
		// int j = iprimary[i].rrn;
		// for(int k = 0;temp[j]!= '@';k++){
		// 	iproduct->string[k] = temp[j];
		// }
		aux = recuperar_registro(i);
		strcpy(iproduct[i].string, aux.nome);
		strcpy(iproduct[i].pk, iprimary[i].pk);
	}
	//TODO checar se caso está ordenado por nome e não pk
	qsort(iproduct, *nregistros, sizeof(Is),compare_nome);

	//print para testes da corretude da funcao
	 // for(int j = 0; j < *nregistros; j++)
	 // 	printf("nome: %s / chave: %s\n", iproduct[j].string, iproduct[j].pk);
}

//cria indice secundario com nome da marca e chave primaria
void criar_ibrand(Ip* iprimary, Is* ibrand, int* nregistros){
	Produto aux;

	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);
		strcpy(ibrand[i].string, aux.marca);
		strcpy(ibrand[i].pk, iprimary[i].pk);
	}
	qsort(ibrand, *nregistros, sizeof(Is),compare_marca);

	//print para testes da corretude da funcao
	 // for(int j = 0; j < *nregistros; j++)
	 // 	printf("nome: %s / chave: %s\n", ibrand[j].string, ibrand[j].pk);
}

//cria indice secundario com as categorias e chave primaria
void criar_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat){
	Produto aux;
	char temp[TAM_CATEGORIA], *p;
	//TODO analisar como o strcmp (quais campos eles utiliza)
	// está comparando os registros

	for(int i =0; i < *nregistros;i++){
		aux = recuperar_registro(i);
		strcpy(temp, aux.categoria);

		p = strtok(temp, "|");

		while(p != NULL){
			// printf("%s\n", icategory->cat);
			//verifica se determinada categoria ja se encontra no indice
			//se sim, adiciona uma nova chave primaria a lista, em ordem
			if(i > 0){
				if(bsearch(p, icategory->cat, *ncat, TAM_CATEGORIA,
					(int(*)(const void*, const void*))strcmp)){
						//recebo o nó raiz daquela posicao no vetor
						 ll* percorre = icategory[i].lista;
						 ll* novo = NULL;
						 ll* aux2 = NULL;

						 //percorro o vetor até a char a posicao correta
						 //para inserir o novo nó
						 while(percorre){
							 if(strcmp(aux.pk, percorre->pk) < 0){
								 aux2 = percorre;
								 percorre = percorre->prox;
							 }
					 	}
						//inserindo o no na posicao correta
						 strcpy(novo->pk, aux.pk);
						 novo->prox = percorre;
						 aux2->prox = novo;
				}
			//se nao, insere a nova categoria ao vet de categorias e inicia
			//a lista dentro do mesmo
			}else{
				strcpy(icategory[*ncat].cat, p);
				ll* novo = NULL;
				printf("da segfault nao men %s\n", aux.pk);
				strcpy(novo->pk, aux.pk);
				novo->prox = NULL;
				icategory[*ncat].lista = novo;
				ncat++;
			}
			p = strtok(NULL, "|");
		}
		// strcpy(icategory[0].cat, p);
		//
		// for(int j = 1; j < *ncat;j++){
		// 	p = strtok(NULL,"|");
		// 	strcpy(icategory[j].cat, p);
		// }
	}
}

//cria indice secundario com preco e chave primaria
void criar_iprice(Ip* iprimary, Isf* iprice, int* nregistros){
	Produto aux;

	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);

		double preco_temp = atof(aux.preco);
		iprice->price = (float) preco_temp;
		strcpy(iprice->pk, aux.pk);
	}
	qsort(iprice, *nregistros, sizeof(Isf), compare_preco);

}

//compara os campos de nome de dois produtos
int compare_nome(const void* nome1, const void* nome2){
	 Produto* produto1 = (Produto*)nome1;
	 Produto* produto2 = (Produto*)nome2;

	if(strcmp(produto1->nome, produto2->nome) < 0)
		return -1;
	else if(strcmp(produto1->nome, produto2->nome) == 0){
		return strcmp(produto1->pk, produto2->pk);
	}else if(strcmp(produto1->nome, produto2->nome) > 0){
		return 1;
	}
}

//compara os campos de marca de dois produtos
int compare_marca(const void* marca1, const void* marca2){
	 Produto* produto1 = (Produto*)marca1;
	 Produto* produto2 = (Produto*)marca2;

	if(strcmp(produto1->marca, produto2->marca) < 0)
		return -1;
	else if(strcmp(produto1->marca, produto2->marca) == 0){
		return strcmp(produto1->pk, produto2->pk);
	}else if(strcmp(produto1->marca, produto2->marca) > 0){
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
int compra_categoria(const void* p1, const void* p2){
	//TODO talvez apenas a função de strcmp seja o suficnete
	//para ordenar as categorias?
}
//TODO CASO NECESSARIO FAZER AS FUNCOES DE COMPARACOES PARA OS OUTROS indices
//ANALISANDO CAMPO ESPECIFICO DA STRUCT E COM UM SEGUNDO CRITERIO, CASO O
//PRIMEIRO DE EMPATE (RETURN 0) COLOCAR A COMPARACAO DO SEGUNDO CRITERIO
//PARA O PRIMEIRO CAMPO, PROVAVELMENTE DA CERTO CHAMAR STRCOMP, APENAS PASSANDO
//O CAMPO DA STRUCT DESEJADO

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
