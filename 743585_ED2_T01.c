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
void cadastrar_produto(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int* ncat);

//Altera produto correspondente a chave primaria passada como parâmetro
int alterar_produto(Ip* iprimary, int* nregistros);

//Remove produto com base na chave primaria fornecida
int remover_produto(Ip* iprimary, int* nregistros);

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
//libera memoria alocada pelo indice icategory
void libera_icategory(Ir* icategory, int* ncat);
//lista todos os produtos de diferentes formas
void listagens(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf* iprice,
	 		int* nregistros, int* ncat);
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
				cadastrar_produto(iprimary, &nregistros, iproduct, ibrand,
			    					icategory, iprice, &ncat);
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);

				//TODO posos criar essas variaveis aqui e dar scanf ou nao se
				//deve mexer em nada na main?

				if(alterar_produto(iprimary, &nregistros))
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
				listagens(iprimary, iproduct, ibrand,icategory, iprice, &nregistros, &ncat);

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

//TODO aparentemente funções de ordenacao nao estao alterando os indices secundarios

//Cadastra novo Produto nos arquivos de dados e índices
void cadastrar_produto(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
					   Ir* icategory, Isf* iprice, int* ncat){
	char temp[TAM_REGISTRO];
	Produto novo;

	//TODO funcoes de cadastrar protudos em sequencia bugam e requerem um enter a mais
	//obtem do usuario informações sobre o produto
	scanf("%[^\n]%*c", novo.nome);
	//essas mascaras funcionam ou seria melhor combinar a mascara %[^\n]\n com um getchar?
	scanf("%[^\n]%*c", novo.marca);
	scanf("%[^\n]%*c", novo.data);
	scanf("%[^\n]%*c", novo.ano);
	scanf("%[^\n]%*c", novo.preco);
	scanf("%[^\n]%*c", novo.desconto);
	scanf("%[^\n]%*c", novo.categoria);

	//gera uma chave para este novo produto
	gerarChave(&novo);

	//checa se a chave gerada já existia no sistema, se sim a função se encerra
	if(verifica_chave(novo.pk, *nregistros, iprimary) == 0)
		return;

	//imprime todos os dados do produto em uma string temporaria, para depois
	//ser passada para o arquivo principal
	sprintf(temp,"%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data,
			novo.ano, novo.preco, novo.desconto, novo.categoria);
	//TODO devo imprimir a chave primaria no arquivo de dados?

	int aux = strlen(temp);

	//verifica se o registro possui tamanho menor que 192 bytes, se sim, devemos
	//preencher o resto do espaço com #
	while(aux < 192){
		temp[aux] = '#';
		aux++;
	}
	//inserindo a string gerada no arquivo de dados
	char* fimArquivo = ARQUIVO + (192 * *nregistros);
	sprintf(fimArquivo, "%s", temp);
	fimArquivo += 192;
	*fimArquivo = '\0';

	//inserindo o produto no indice primario
	// TODO para este índice estou inserindo no fim e depois ordenando, há uma
	//melhor forma de fazer isso?
	iprimary[*nregistros].rrn = *nregistros;
	strcpy(iprimary[*nregistros].pk, novo.pk);

	//inserindo no indice secundario iproduct
	strcpy(iproduct[*nregistros].string, novo.nome);
	strcpy(iproduct[*nregistros].pk, iprimary[*nregistros].pk);

	//inserindo no indice secundario ibrand
	strcpy(ibrand[*nregistros].string, novo.marca);
	strcpy(ibrand[*nregistros].pk, iprimary[*nregistros].pk);

	//inserindo no índice secundario icategory
	insere_icategory(iprimary, icategory, nregistros, ncat, novo);

	//inserindo no indice secundario iprice
	double preco_temp = atof(novo.preco);
	iprice[*nregistros].price = (float)preco_temp;
	strcpy(iprice[*nregistros].pk, novo.pk);

	*nregistros+= 1;

	qsort(iprimary, *nregistros, sizeof(Ip), (int(*)(const void*, const void*))strcmp);
	qsort(iproduct, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_nome);
	qsort(ibrand, *nregistros, sizeof(Is), (int(*)(const void*, const void*))compare_marca);
	qsort(icategory, *nregistros, sizeof(Ir), (int(*)(const void*, const void*))compare_categoria);
	qsort(iprice, *nregistros, sizeof(Isf), (int(*)(const void*, const void*))compare_preco);
}

//Altera produto correspondente a chave primaria passada como parâmetro
int alterar_produto(Ip* iprimary, int* nregistros){
	char chave[TAM_PRIMARY_KEY];
	char desconto[TAM_DESCONTO];

	scanf("%[^\n]\n", chave);
	scanf("%[^\n]\n", desconto);
	if(*nregistros == 0){
		printf(ARQUIVO_VAZIO);
		return 0;
	}
	int i = atoi(desconto);

	while(i < 0 || i > 100 || strlen(desconto) != 3){
		printf(CAMPO_INVALIDO);
		scanf("%s", desconto);
		i = atoi(desconto);
	}
	Produto* aux = (Produto*)bsearch(chave, iprimary, *nregistros, TAM_PRIMARY_KEY
			,(int(*)(const void*, const void*))strcmp);
	if(aux){
		strcpy(aux->desconto, desconto);
		return 1;
	}else{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
}

//Remove produto com base na chave primaria inserida
int remover_produto(Ip* iprimary, int* nregistros){
	char chave[TAM_PRIMARY_KEY];
	Ip *aux;

	scanf("%s", chave);
	if(!nregistros){
		printf(ARQUIVO_VAZIO);
		return 0;
	}

	aux = busca_primary(iprimary, chave, nregistros);

	if(aux){
		char* posProduto = ARQUIVO + (192 * aux->rrn);
		*(posProduto) = '|';
		*(posProduto + 1) = '*';
		aux->rrn = -1;

		return 1;
	}else{
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
int verifica_chave(char* chave, int nregistros, Ip* iprimary){
	int *foundIt = NULL;

	foundIt = (int*)bsearch(chave, iprimary, nregistros, sizeof(Ip),
	 			(int(*)(const void*, const void*))strcmp);
	if(foundIt){
		printf(ERRO_PK_REPETIDA, chave);
		return 0;
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

		//TODO é problemático so checar se o temp é nulo (fim do arquivo)
		//depois de tentar registrar um produto todo?
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

			if(verifica_chave(aux.pk, *nregistros, indice_primario) == 0)
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
		strcpy(iproduct[i].string, aux.nome);
		strcpy(iproduct[i].pk, aux.pk);
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
		strcpy(ibrand[i].string, aux.marca);
		strcpy(ibrand[i].pk, aux.pk);
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
		insere_icategory(iprimary, icategory, nregistros, ncat, aux);
	}
	qsort(icategory, *nregistros, sizeof(Ir), (int(*)(const void*, const void*))compare_categoria);
}

//insere no indice secundario icategory
//TODO algumas categorias estão sendo inseridas mais de uma vez
void insere_icategory(Ip* iprimary, Ir* icategory, int* nregistros, int* ncat, Produto aux){
	char temp[TAM_CATEGORIA], *p;

	//TODO VER Quando entra na condição de encontrar igual e oq acontece e onde cria a nova categoria
	strcpy(temp, aux.categoria);
	p = strtok(temp, "|");
	while(p != NULL){
			//verifica se determinada categoria ja se encontra no indice
			//se sim, adiciona uma nova chave primaria a lista, em ordem
		if(*ncat > 0){
				//if ncat > 0 evita que compare na primeira insercao, ja que
				//em ncat == 0 nao ha elementos no indice
			Ir* findrepetida = (Ir*)bsearch(p, icategory, *ncat, sizeof(Ir),
								(int(*)(const void*, const void*))compare_categoria_busca);
			if(findrepetida){
				ll* percorre = findrepetida->lista;
				//recebo o nó raiz daquela posicao no vetor
				// ll* percorre = icategory[i].lista;
				ll* novo = (ll*)malloc(sizeof(ll));
				novo->prox = NULL;
				ll* aux2 = NULL;

				 //percorro o vetor até a char a posicao correta
				 //para inserir o novo nó
				while(percorre->prox != NULL){
					if(strcmp(aux.pk, percorre->pk) < 0){
						//inserindo o no na posicao correta
						strcpy(novo->pk, aux.pk);
						novo->prox = percorre;
						aux2->prox = novo;
						break;
					}
					aux2 = percorre;
					percorre = percorre->prox;
				 }
				 if(percorre->prox == NULL){
					 if(strcmp(aux.pk, percorre->pk) < 0){
						 strcpy(novo->pk, aux.pk);
						 novo->prox = percorre;
					 }else{
						 strcpy(novo->pk, aux.pk);
						 percorre->prox = novo;
					 }
				 }
			}else{
				strcpy(icategory[*ncat].cat, p);
				ll* novo = (ll*)malloc(sizeof(ll));
				novo->prox = NULL;
				strcpy(novo->pk, aux.pk);
				icategory[*ncat].lista = novo;
				printf("%s %s\n", icategory[*ncat].cat, icategory[*ncat].lista->pk);
				*ncat += 1;
			}
			//se nao, insere a nova categoria ao vet de categorias e inicia
			//a lista dentro do mesmo
		}else{
			strcpy(icategory[*ncat].cat, p);
			ll* novo = (ll*)malloc(sizeof(ll));
			novo->prox = NULL;
			strcpy(novo->pk, aux.pk);
			icategory[*ncat].lista = novo;
			printf("%s %s\n", icategory[*ncat].cat, icategory[*ncat].lista->pk);
			*ncat += 1;
		}
		p = strtok(NULL, "|");
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
		iprice[i].price = (float) preco_temp;
		strcpy(iprice[i].pk, aux.pk);
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
	   return strcmp(produto1->lista->pk, produto2->lista->pk);
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
//CASO NECESSARIO FAZER AS FUNCOES DE COMPARACOES PARA OS OUTROS indices
//ANALISANDO CAMPO ESPECIFICO DA STRUCT E COM UM SEGUNDO CRITERIO, CASO O
//PRIMEIRO DE EMPATE (RETURN 0) COLOCAR A COMPARACAO DO SEGUNDO CRITERIO
//PARA O PRIMEIRO CAMPO, PROVAVELMENTE DA CERTO CHAMAR STRCOMP, APENAS PASSANDO
//O CAMPO DA STRUCT DESEJADO

//busca um registro utilizando diversos argumentos
//TODO não encontra registros quando procurado por quaisquer outros meios
//além da pk
void busca_registro(Ip *iprimary, int* nregistros, Is* iproduct, Is* ibrand,
 						Ir* icategory, Isf* iprice, int* ncat){
	int opBusca = 0;
	char chavePrim[TAM_PRIMARY_KEY];
	char chaveNome[TAM_NOME];
	char chaveMarca[TAM_MARCA];
	char chaveCategoria[TAM_CATEGORIA];

	scanf("%d", &opBusca);
	if(!nregistros){
		printf(ARQUIVO_VAZIO);
		return;
	}
	switch(opBusca){
		case 1:
			scanf("%s*\n", chavePrim);

			Produto temp;
			Ip* aux;
			aux = busca_primary(iprimary, chavePrim, nregistros);

			if(aux){
				temp = recuperar_registro(aux->rrn);
				printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", temp.pk, temp.nome,
				 temp.marca, temp.data, temp.ano, temp.preco, temp.desconto, temp.categoria);
		 	}else{
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}
		break;

		case 2:
			getchar();
			scanf("%[^\n]s", chaveNome);

			Is* aux2;
			aux2 = busca_iproduct(iproduct, chaveNome, nregistros);
			printf("entrou no if\n");

			//TODO devo fazer -sizeof(Is) ou -1?
			if(aux2){
				Ip* prim;
				prim = busca_primary(iprimary, aux2->pk, nregistros);
				// temp = recuperar_registro(prim->rrn);
				// printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", temp.pk, temp.nome,
				// temp.marca, temp.data, temp.ano, temp.preco, temp.desconto, temp.categoria);
				exibir_registro(prim->rrn, 0);

				//imprimindo os produtos de mesmo nome
				while((aux2 - 1) && ((aux2 - 1)->string == aux2->string)){
					prim = busca_primary(iprimary, aux2->pk, nregistros);
					// temp = recuperar_registro(prim->rrn);
					// printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", temp.pk, temp.nome,
					//  temp.marca, temp.data, temp.ano, temp.preco, temp.desconto, temp.categoria);
					exibir_registro(prim->rrn, 0);
				}
				while((aux2 + 1) && ((aux2 + 1)->string == aux2->string)){
					prim = busca_primary(iprimary, aux2->pk, nregistros);
					// temp = recuperar_registro(prim->rrn);
					// printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", temp.pk, temp.nome,
					//  temp.marca, temp.data, temp.ano, temp.preco, temp.desconto, temp.categoria);
					exibir_registro(prim->rrn, 0);
				 }
			 }else{
				 printf(REGISTRO_N_ENCONTRADO);
			 }
		break;

		case 3:
			getchar();
			scanf("%[^\n]s", chaveMarca);
			getchar();
			scanf("%[^\n]s", chaveCategoria);

			Is* auxMarca;
			Ir* auxCategoria;
			auxMarca = busca_ibrand(ibrand, chaveMarca, nregistros);
			auxCategoria = busca_icategory(icategory, chaveCategoria, nregistros);

			if(auxMarca && auxCategoria){
				ll* percorre= (ll*)bsearch(auxMarca->pk, auxCategoria->lista, *ncat, sizeof(Ir),
									(int(*)(const void*, const void*))strcmp);
				if(percorre){
					Is* temp;
					ll* auxPercorre = percorre;
					while(auxPercorre){
						temp = (Is*)bsearch(percorre->pk, ibrand, *nregistros, sizeof(Is),compare_marca);
						if(temp->string == chaveMarca){
							Ip* prim = (Ip*)bsearch(percorre->pk, iprimary, *nregistros,
							 			sizeof(Ip), (int(*)(const void*, const void*))strcmp);
							exibir_registro(prim->rrn, 0);
						}
						auxPercorre = auxPercorre->prox;
					}
				}else{
					printf(REGISTRO_N_ENCONTRADO);
				}
			}else{
				printf(REGISTRO_N_ENCONTRADO);
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
	(int(*)(const void*, const void*))compare_nome);

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
	 		int* nregistros, int* ncat){
	int opLista = 0;
	Ip* temp;
	ll* aux;
	Ip* tempbrand;
	Ip* tempprice;

	scanf("%d%*c", &opLista);

	if(!nregistros){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	switch(opLista){
		case 1:
			for(int j = 0; j < *nregistros; j++){
				exibir_registro(iprimary[j].rrn, 0);
				printf("\n");
			}

		break;
		case 2:
		for(int i = 0; i < *ncat; i++){
			aux =  icategory[i].lista;
			while(aux != NULL){
				temp = (Ip*)bsearch(aux->pk, iprimary, *nregistros, sizeof(Ip),
						(int(*)(const void*, const void*))strcmp);
				exibir_registro(temp->rrn, 0);
				aux = aux->prox;
			}
			printf("\n");
		}
		break;
		case 3:
			for(int j = 0; j < *nregistros; j++){
				tempbrand = (Ip*)bsearch(ibrand[j].pk, iprimary, *nregistros, sizeof(Ip),
							(int(*)(const void*, const void*))strcmp);
				exibir_registro(tempbrand->rrn, 0);
				printf("\n");
			}
		break;
		case 4:
		for(int j = 0; j < *nregistros; j++){
			tempprice = (Ip*)bsearch(iprice[j].pk, iprimary, *nregistros, sizeof(Ip),
						(int(*)(const void*, const void*))strcmp);
			exibir_registro(tempprice->rrn, 1);
			printf("\n");
		}
		break;
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
