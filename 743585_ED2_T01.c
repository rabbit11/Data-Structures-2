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
 void cadastrar_produto(Ip *iprimary, int nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int ncat);

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

//Insere produto no índice icategory
void insere_icategory(Ip* iprimary,Ir* icategory,int* nregistros, int* ncat, Produto aux, int rrn);

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
				cadastrar_produto(iprimary, nregistros, iproduct, ibrand,
			    					icategory, iprice, ncat);
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
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
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
void cadastrar_produto(Ip *iprimary, int nregistros, Is* iproduct, Is* ibrand,
					   Ir* icategory, Isf* iprice, int ncat){
	char temp[TAM_REGISTRO];
	Produto novo;
	//obtem do usuario informações sobre o produto
	scanf("%[^\n]\n", novo.nome);
	scanf("%[^\n]\n", novo.marca);
	scanf("%[^\n]\n", novo.data);
	scanf("%[^\n]\n", novo.ano);
	scanf("%[^\n]\n", novo.preco);
	scanf("%[^\n]\n", novo.desconto);
	scanf("%[^\n]", novo.categoria);

	//TODO campo categoria a entrada ja vem com as barras separando mais de uma
	//categoria?

	//gera uma chave para este novo produto
	gerarChave(&novo);

	//checa se a chave gerada já existia no sistema, se sim a função se encerra
	if(verifica_chave(novo.pk, nregistros, iprimary) == 0)
		return;

	//imprime todos os dados do produto em uma string temporaria, para depois
	//ser passada para o arquivo principal
	sprintf(temp,"%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data,
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
	char* fimArquivo = ARQUIVO + (192 * nregistros);
	sprintf(fimArquivo, "%s", temp);
	//inserindo o produto no indice primario
		// TODO para este índice estou inserindo no fim e depois ordenando, há uma
		//melhor forma de fazer isso?
	iprimary[nregistros].rrn = nregistros;
	strcpy(iprimary[nregistros].pk, novo.pk);
	qsort(iprimary, nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);
	printf("P%s\n", novo.pk);
	//inserindo no indice secundario iproduct
	strcpy(iproduct[nregistros].string, novo.nome);
	strcpy(iproduct[nregistros].pk, iprimary[nregistros].pk);
	qsort(iproduct, nregistros, sizeof(Is),compare_nome);

	//inserindo no indice secundario ibrand
	strcpy(ibrand[nregistros].string, novo.marca);
	strcpy(ibrand[nregistros].pk, iprimary[nregistros].pk);
	qsort(ibrand, nregistros, sizeof(Is),compare_marca);

	//inserindo no índice secundario icategory

	//TODO checar insere_icategory pois deve apresentar os mesmos erros de
	//criar_icategory
	printf("vai buga: \n");
	insere_icategory(iprimary, icategory, &nregistros, &ncat, novo, iprimary[nregistros].rrn);

	//inserindo no indice secundario iprice
	double preco_temp = atof(novo.preco);

	iprice->price = (float) preco_temp;
	strcpy(iprice->pk, novo.pk);
	qsort(iprice, nregistros, sizeof(Isf), compare_preco);

	nregistros++;
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
		if(p->data[j] != '/'){
			p->pk[i] = p->data[j];
			i++;
		}
	}
	//TODO debugar chave (bug ocorre na parte da data)
	printf("fota men %s\n", p->pk);
	printf("i me salva %d\n", i);
	//inserindo os 2 digitos do ano de lançamento
	for(int j = 0;j < 2; i++, j++){
		p->pk[i] = p->ano[j];
	}
	//printf("%s\n", p->pk);
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

	if(*nregistros == 0){
		return;
	}

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
		//TODO é problemático so checar se o temp é nulo (fim do arquivo)
		//depois de tentar registrar um produto todo?
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
		// int j = iprimary[i].rrn;
		// for(int k = 0;temp[j]!= '@';k++){
		// 	iproduct->string[k] = temp[j];
		// }
		aux = recuperar_registro(i);
		//printf("%s\n", iprimary[i].pk);
		strcpy(iproduct[i].string, aux.nome);
		strcpy(iproduct[i].pk, iprimary[i].pk);
	}
	qsort(iproduct, *nregistros, sizeof(Is),compare_nome);

	//print para testes da corretude da funcao
	  // for(int j = 0; j < *nregistros; j++)
	  // 	printf("nome: %s / chave: %s\n", iproduct[j].string, iproduct[j].pk);
}

//cria indice secundario com nome da marca e chave primaria
void criar_ibrand(Ip* iprimary, Is* ibrand, int* nregistros){
	Produto aux;

	if(*nregistros == 0){
		return;
	}

	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);
		strcpy(ibrand[i].string, aux.marca);
		strcpy(ibrand[i].pk, iprimary[i].pk);
	}
	qsort(ibrand, *nregistros, sizeof(Is),compare_marca);

	//print para testes da corretude da funcao
	  // for(int j = 0; j < *nregistros; j++)
	  // 	printf("marca: %s / chave: %s\n", ibrand[j].string, ibrand[j].pk);
}

//cria indice secundario com as categorias e chave primaria
//TODO talvez incluir o insere_icategory dentro para evitar repetição de codigo?
void criar_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat){
	Produto aux;

	if(*nregistros == 0){
		return;
	}
	for(int i =0; i < *nregistros;i++){
		aux = recuperar_registro(i);
		insere_icategory(iprimary, icategory, nregistros, ncat, aux, i);
	}
}

//insere no indice secundario icategory
void insere_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat, Produto aux, int rrn){
	char temp[TAM_CATEGORIA], *p;

	strcpy(temp, aux.categoria);
	p = strtok(temp, "|\n\0");

	while(p != NULL){
			// printf("%s\n", icategory->cat);
			//verifica se determinada categoria ja se encontra no indice
			//se sim, adiciona uma nova chave primaria a lista, em ordem
		if(*ncat > 0){
				//if ncat > 0 evita que compare na primeira insercao, ja que
				//em ncat == 0 nao ha elementos no indice
			ll* percorre = (ll*)bsearch(p, icategory, *ncat, sizeof(Ir),
								(int(*)(const void*, const void*))strcmp);
			if(percorre != NULL){
				printf("%s\n", p);
						//recebo o nó raiz daquela posicao no vetor
						 // ll* percorre = icategory[i].lista;
					ll* novo = (ll*)malloc(sizeof(ll));
					ll* aux2 = NULL;

						 //percorro o vetor até a char a posicao correta
						 //para inserir o novo nó
					while(percorre){
						if(strcmp(aux.pk, percorre->pk) > 0){
						 	aux2 = percorre;
							 percorre = percorre->prox;
						}
					 }
						//inserindo o no na posicao correta
					strcpy(novo->pk, iprimary[rrn].pk);
					novo->prox = percorre;
					aux2->prox = novo;
			}else{
				strcpy(icategory[*ncat].cat, p);
				ll* novo = (ll*)malloc(sizeof(ll));
				strcpy(novo->pk, iprimary[rrn].pk);
				icategory[*ncat].lista = novo;
				*ncat += 1;
			}
			//se nao, insere a nova categoria ao vet de categorias e inicia
			//a lista dentro do mesmo
		}else{
			strcpy(icategory[*ncat].cat, p);
			ll* novo = (ll*)malloc(sizeof(ll));
			printf("fala ai nemesis\n");
			printf("opora%s\n", iprimary[0].pk);
			strcpy(novo->pk, iprimary[rrn].pk);
			icategory[*ncat].lista = novo;
			*ncat += 1;
		}
		p = strtok(NULL, "|\n\0");
	}
}

//cria indice secundario com preco e chave primaria
void criar_iprice(Ip* iprimary, Isf* iprice, int* nregistros){
	Produto aux;

	if(*nregistros == 0){
		return;
	}

	for(int i = 0; i < *nregistros; i++){
		aux = recuperar_registro(i);

		double preco_temp = atof(aux.preco);
		iprice->price = (float) preco_temp;
		strcpy(iprice->pk, iprimary[i].pk);
	}
	qsort(iprice, *nregistros, sizeof(Isf), compare_preco);

	// for(int j = 0; j < *nregistros; j++)
	//   printf("preco: %lf / chave: %s\n", iprice[j].price, iprice[j].pk);
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
int compara_categoria(const void* p1, const void* p2){
	//TODO talvez apenas a função de strcmp seja o suficnete
	//para ordenar as categorias?
}
//CASO NECESSARIO FAZER AS FUNCOES DE COMPARACOES PARA OS OUTROS indices
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
	//TODO descobrir pq gerar chave chamado varias vezes por essa funçãon
	//buga o produto.pk
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
