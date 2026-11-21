/*
 * PROJETO DE PROGRAMACAO - GRUPO SOCIAL (PARTE 2)
 * Teoria dos Grafos - 2026-2
 * Prof. Roberto C. de Araujo
 *
 * Integrantes:
 *   Bruna Amorim Maia            - RA: 10431883
 *   Fabyani Tiva Yan             - RA: 10431835
 *   Rafael Araujo Cabral Moreira - RA: 10441919
 *
 * Descricao: modelagem da rede de contatos de 25 moradores de um
 * condominio residencial por meio de um grafo nao orientado.
 * Cada vertice representa um morador e cada aresta representa um
 * contato direto (e reciproco) entre dois moradores.
 *
 * Base: implementacao de grafos Grafo_2026-2.c fornecida na disciplina.
 */

 
/*
 * REPRESENTACAO DE GRAFOS
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/* 
 * Estrutura de dados para representar grafos
 */
typedef struct aresta{ /* Celula de uma lista de arestas */
	int outroExtremo;
	struct aresta *prox;
}Aresta;

typedef struct vertice{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
	int nome;
	char nomePessoa[50];
	int bloco;
	int apartamento;
	Aresta *a;
}Vertice;


/*
 * Declaracoes das funcoes para manipulacao de grafos 
 */
void criarGrafo(Vertice **G, int ordem);
void destruirGrafo(Vertice **G, int ordem);
int acrescentarAresta(Vertice G[], int ordem, int v1, int v2);
void imprimirGrafo(Vertice G[], int ordem);


/*
 * Funcao acrescentada para o projeto, que verifica se existem pessoas que nao possuem nenhum contato com outras pessoas cadastradas na rede.
 */
void pessoasSemContato(Vertice G[], int ordem);

/*
 * Declaracoes das funcoes acrescentadas para o projeto (Pessoa 2)
 */
int contarContatos(Vertice G[], int v);
void pessoasComUmContato(Vertice G[], int ordem);
void pessoasComMaisContatos(Vertice G[], int ordem);

/*
 * Declaracao da funcao acrescentada para o projeto (Pessoa 3)
 */
void gruposIsolados(Vertice G[], int ordem);


/*
 * Criacao de um grafo com ordem predefinida (passada como argumento),
 * e, inicialmente, sem nenhuma aresta 
 */
void criarGrafo(Vertice **G, int ordem){
	int i;

	*G = (Vertice*) malloc(sizeof(Vertice) * ordem);

	if(*G == NULL)
		return;

	for(i = 0; i < ordem; i++){
		(*G)[i].nome = i;
		(*G)[i].nomePessoa[0] = '\0';
		(*G)[i].a = NULL;
	}
}


/*
 * Desaloca a memoria dinamica usada para armazenar um grafo.
 */
void destruirGrafo(Vertice **G, int ordem){
	int i;
	Aresta *p, *n;

	for(i = 0; i < ordem; i++){
		p = (*G)[i].a;

		while(p != NULL){
			n = p->prox;
			free(p);
			p = n;
		}
	}

	free(*G);
}


/*  
 * Acrescenta uma nova aresta em um grafo previamente criado.
 * Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada.
 *
 * Como o grafo nao e orientado, para uma aresta com extremos i e j,
 * sao criadas as entradas (i,j) e (j,i). Lacos e repeticoes sao rejeitados.
 */
int acrescentarAresta(Vertice G[], int ordem, int v1, int v2){
	Aresta *A1, *A2;

	if(v1 < 0 || v1 >= ordem)
		return 0;

	if(v2 < 0 || v2 >= ordem)
		return 0;

	if(v1 == v2)
		return 0;

	for(A1 = G[v1].a; A1 != NULL; A1 = A1->prox)
		if(A1->outroExtremo == v2)
			return 0;

	A1 = malloc(sizeof(Aresta));
	A2 = malloc(sizeof(Aresta));
	if(A1 == NULL || A2 == NULL){
		free(A1);
		free(A2);
		return 0;
	}

	A1->outroExtremo = v2;
	A1->prox = G[v1].a;
	G[v1].a = A1;
	A2->outroExtremo = v1;
	A2->prox = G[v2].a;
	G[v2].a = A2;

	return 1;
}


/*  
 * Imprime os moradores e seus contatos
 */
void imprimirGrafo(Vertice G[], int ordem){
	int i;
	Aresta *aux;

	printf("\nRede de contatos do condominio:\n");

	for(i = 0; i < ordem; i++){

		printf("\n%02d | %s (bloco %c, ap. %d) | Amigos: ",
		       G[i].nome + 1, G[i].nomePessoa, G[i].bloco, G[i].apartamento);

		aux = G[i].a;

		if(aux == NULL){
			printf("sem contatos");
		}

		for(; aux != NULL; aux = aux->prox){
			printf("%s  ", G[aux->outroExtremo].nomePessoa);
		}
	}

	printf("\n\n");
}


/*
 * Funcao acrescentada para o projeto, que verifica se existem pessoas que nao possuem nenhum contato com outras pessoas cadastradas na rede.
 */
void pessoasSemContato(Vertice G[], int ordem){
	int i;
	int encontrou = 0;

	printf("\nMoradores sem nenhum contato na rede:\n");

	for(i = 0; i < ordem; i++){

		/*
		 * Se a lista de arestas estiver vazia,
		 * o morador nao possui contato com nenhum outro morador.
		 */
		if(G[i].a == NULL){

			printf("%s\n", G[i].nomePessoa);

			encontrou = 1;
		}
	}

	if(encontrou == 0){
		printf("Nao existem moradores sem contato na rede.\n");
	}

	printf("\n");
}


/*
 * Funcao auxiliar para contar o numero de contatos (grau) de um vertice
 */
int contarContatos(Vertice G[], int v) {
	int count = 0;
	Aresta *aux = G[v].a;
	while(aux != NULL) {
		count++;
		aux = aux->prox;
	}
	return count;
}

/*
 * Funcao para encontrar pessoas com exatamente 1 contato
 */
void pessoasComUmContato(Vertice G[], int ordem) {
	int i, count;
	int encontrou = 0;

	printf("\nMoradores com exatamente um contato na rede:\n");

	for(i = 0; i < ordem; i++) {
		count = contarContatos(G, i);
		if(count == 1) {
			printf("%s\n", G[i].nomePessoa);
			encontrou = 1;
		}
	}

	if(encontrou == 0) {
		printf("Nao existem moradores com exatamente um contato.\n");
	}
	printf("\n");
}

/*
 * Funcao para encontrar quem possui mais contatos na rede
 */
void pessoasComMaisContatos(Vertice G[], int ordem) {
	int i, count;
	int max = -1;

	/* Primeiro passo: descobre qual e o numero maximo de contatos */
	for(i = 0; i < ordem; i++) {
		count = contarContatos(G, i);
		if(count > max) {
			max = count;
		}
	}

	printf("\nMorador(es) com mais contatos na rede (%d contatos):\n", max);
	if (max == 0) {
		printf("Nenhum morador possui contatos.\n");
		return;
	}

	/* Segundo passo: imprime quem possui essa quantidade maxima de contatos */
	for(i = 0; i < ordem; i++) {
		count = contarContatos(G, i);
		if(count == max) {
			printf("%s\n", G[i].nomePessoa);
		}
	}
	printf("\n");
}


/*
 * Funcao que verifica se existem grupos
 * isolados na rede, isto e, grupos de moradores em que nenhum morador
 * de dentro do grupo tem contato com algum morador de fora do grupo.
 *
 * Cada grupo isolado corresponde a um componente
 * conexo do grafo. Para encontrar os componentes e usada uma busca em
 * largura (BFS):
 *   1. Todos os moradores comecam marcados como "nao visitados" (0).
 *   2. Para cada morador ainda nao visitado, um novo grupo e iniciado:
 *      o morador entra na fila e e marcado com o numero do grupo.
 *   3. Enquanto a fila nao estiver vazia, retira-se um morador da fila
 *      e percorre-se a sua lista de contatos. Cada contato ainda nao
 *      visitado e marcado com o mesmo numero do grupo e entra na fila.
 *   4. Quando a fila esvazia, todos os moradores alcancaveis a partir
 *      do primeiro ja foram marcados, e o grupo esta completo.
 * Ao final, se existir mais de um grupo, a rede possui grupos isolados.
 */
void gruposIsolados(Vertice G[], int ordem){
	int *grupo;   /* grupo[i] = numero do grupo do morador i (0 = nao visitado) */
	int *fila;    /* fila usada na busca em largura */
	int inicio, fim;
	int i, j, v, w;
	int qtdGrupos = 0;
	int tamanho;
	Aresta *aux;

	grupo = (int*) malloc(sizeof(int) * ordem);
	fila  = (int*) malloc(sizeof(int) * ordem);

	if(grupo == NULL || fila == NULL){
		printf("Erro de memoria ao procurar grupos isolados.\n");
		free(grupo);
		free(fila);
		return;
	}

	/* passo 1: ninguem foi visitado ainda */
	for(i = 0; i < ordem; i++)
		grupo[i] = 0;

	/* passos 2 a 4: uma busca em largura para cada grupo novo */
	for(i = 0; i < ordem; i++){

		if(grupo[i] != 0)
			continue; /* morador ja pertence a um grupo encontrado */

		qtdGrupos++;
		inicio = 0;
		fim = 0;

		fila[fim++] = i;
		grupo[i] = qtdGrupos;

		while(inicio < fim){
			v = fila[inicio++];

			for(aux = G[v].a; aux != NULL; aux = aux->prox){
				w = aux->outroExtremo;

				if(grupo[w] == 0){
					grupo[w] = qtdGrupos;
					fila[fim++] = w;
				}
			}
		}
	}

	printf("\nGrupos isolados na rede:\n");

	if(qtdGrupos == 1){
		printf("Nao existem grupos isolados. Todos os moradores estao ligados,\n");
		printf("direta ou indiretamente, em um unico grupo.\n\n");
	}
	else{
		printf("A rede possui %d grupos isolados. Nenhum morador de um grupo\n", qtdGrupos);
		printf("tem contato com moradores de outro grupo.\n\n");

		for(j = 1; j <= qtdGrupos; j++){

			/* conta quantos moradores o grupo j possui */
			tamanho = 0;
			for(i = 0; i < ordem; i++)
				if(grupo[i] == j)
					tamanho++;

			if(tamanho == 1)
				printf("Grupo %d (1 morador, sem contatos): ", j);
			else
				printf("Grupo %d (%d moradores): ", j, tamanho);

			/* exibe os moradores do grupo j */
			for(i = 0; i < ordem; i++)
				if(grupo[i] == j)
					printf("%s  ", G[i].nomePessoa);

			printf("\n");
		}
		printf("\n");
	}

	free(grupo);
	free(fila);
}


/*
 * Cada celula da lista guarda os IDs dos membros de uma panelinha,
 * em ordem crescente, e um ponteiro para a proxima panelinha.
 */
typedef struct panelinha {
    int tamanho;
    int *membros;
    struct panelinha *prox;
} Panelinha;

/*
 * Libera os vetores de membros e as celulas da lista de panelinhas.
 */
static void destruirPanelinhas(Panelinha *lista)
{
    Panelinha *prox;
    while(lista != NULL){
        prox = lista->prox;
        free(lista->membros);
        free(lista);
        lista = prox;
    }
}

/*
 * Testa se o morador v e vizinho de todos os membros do grupo atual.
 * A matriz adj permite consultar cada amizade de forma direta.
 */
static int amigoDeTodos(const unsigned char *adj, int ordem, int v,
                        const int membros[], int tamanho)
{
    int i;
    for(i = 0; i < tamanho; i++)
        if(!adj[(size_t)v * ordem + membros[i]])
            return 0;
    return 1;
}

/*
 * Enumera grupos completos por busca recursiva. Cada chamada tenta
 * acrescentar moradores com ID maior, evitando repetir o mesmo grupo.
 * Guarda os grupos com pelo menos tres membros que nao podem crescer.
 * O parametro fim aponta para o local onde sera ligada a proxima celula.
 * Retorna 1 ao concluir a busca ou 0 se faltar memoria.
 */
static int buscarPanelinhas(const unsigned char *adj, int ordem, int membros[],
                            int tamanho, int inicio, Panelinha ***fim)
{
    int v;
    Panelinha *nova;

    /* Acrescenta um amigo de todos e explora as extensoes do grupo. */
    for(v = inicio; v < ordem; v++){
        if(amigoDeTodos(adj, ordem, v, membros, tamanho)){
            membros[tamanho] = v;
            if(!buscarPanelinhas(adj, ordem, membros, tamanho + 1, v + 1, fim))
                return 0;
        }
    }
    if(tamanho < 3)
        return 1;

    /* Testa a maximalidade com todos os IDs, inclusive os menores que inicio.
     * Como a diagonal e zero, um membro do grupo nao pode estende-lo. */
    for(v = 0; v < ordem; v++)
        if(amigoDeTodos(adj, ordem, v, membros, tamanho))
            return 1;

    nova = malloc(sizeof(Panelinha));
    if(nova == NULL)
        return 0;
    nova->membros = malloc(sizeof(int) * tamanho);
    if(nova->membros == NULL){
        free(nova);
        return 0;
    }
    memcpy(nova->membros, membros, sizeof(int) * tamanho);
    nova->tamanho = tamanho;
    nova->prox = NULL;
    **fim = nova;
    *fim = &nova->prox;
    return 1;
}

/*
 * Monta uma matriz auxiliar a partir das listas de adjacencia e inicia
 * a busca com um grupo vazio. A matriz e o vetor de trabalho sao
 * liberados ao fim da busca. O chamador libera a lista encontrada.
 */
static int encontrarPanelinhas(Vertice G[], int ordem, Panelinha **lista)
{
    unsigned char *adj;
    int *membros;
    int v, sucesso;
    Aresta *aresta;
    Panelinha **fim = lista;

    adj = calloc((size_t)ordem, (size_t)ordem);
    membros = malloc(sizeof(int) * ordem);
    if(adj == NULL || membros == NULL){
        free(adj);
        free(membros);
        return 0;
    }
    for(v = 0; v < ordem; v++)
        for(aresta = G[v].a; aresta != NULL; aresta = aresta->prox)
            adj[(size_t)v * ordem + aresta->outroExtremo] = 1;

    sucesso = buscarPanelinhas(adj, ordem, membros, 0, 0, &fim);
    free(adj);
    free(membros);
    return sucesso;
}

/*
 * Numera as panelinhas na ordem da lista e exibe o nome de cada membro.
 */
static void exibirPanelinhas(Vertice G[], const Panelinha *lista)
{
    int numero = 1;
    int i;

    printf("\nPanelinhas da rede:\n");
    if(lista == NULL)
        printf("Nenhuma panelinha encontrada.\n");

    for(; lista != NULL; lista = lista->prox){
        printf("P%d: {", numero++);
        for(i = 0; i < lista->tamanho; i++){
            if(i > 0)
                printf(", ");
            printf("%s", G[lista->membros[i]].nomePessoa);
        }
        printf("}\n");
    }
}

/*
 * Percorre os dois vetores ordenados para calcular a intersecao.
 * Retorna o ID do unico membro em comum ou -1 se houver zero ou mais de um.
 */
static int unicoEmComum(const Panelinha *a, const Panelinha *b)
{
    int i = 0, j = 0, comum = -1;

    while(i < a->tamanho && j < b->tamanho){
        if(a->membros[i] < b->membros[j])
            i++;
        else if(a->membros[i] > b->membros[j])
            j++;
        else {
            if(comum != -1)
                return -1;
            comum = a->membros[i];
            i++;
            j++;
        }
    }
    return comum;
}

/*
 * Compara cada par de panelinhas uma vez. Para cada morador que aparece
 * sozinho na intersecao, guarda o primeiro par que comprova sua influencia.
 * Exibe cada pessoa uma vez e retorna 0 apenas se faltar memoria.
 */
static int exibirPessoasInfluentes(Vertice G[], int ordem, const Panelinha *lista)
{
    const Panelinha *a, *b;
    int *primeira, *segunda;
    int i, j, v, encontrou = 0;

    primeira = calloc(ordem, sizeof(int));
    segunda = calloc(ordem, sizeof(int));
    if(primeira == NULL || segunda == NULL){
        free(primeira);
        free(segunda);
        return 0;
    }

    /* Os numeros i e j correspondem aos exibidos na lista de panelinhas. */
    for(a = lista, i = 1; a != NULL; a = a->prox, i++){
        for(b = a->prox, j = i + 1; b != NULL; b = b->prox, j++){
            v = unicoEmComum(a, b);
            if(v >= 0 && primeira[v] == 0){
                primeira[v] = i;
                segunda[v] = j;
            }
        }
    }

    printf("\nPessoas influentes na rede:\n");
    for(v = 0; v < ordem; v++){
        if(primeira[v] != 0){
            printf("%s (unica pessoa comum a P%d e P%d)\n",
                   G[v].nomePessoa, primeira[v], segunda[v]);
            encontrou = 1;
        }
    }
    if(!encontrou)
        printf("Nenhuma pessoa influente encontrada.\n");

    free(primeira);
    free(segunda);
    return 1;
}

/* IDs fixos mantem as amizades legiveis quando a rede e ampliada. */
enum Morador {
    ANA, BRUNO, CARLA, DANIEL, EDUARDO, FERNANDA, GABRIEL, HELENA, IGOR,
    JULIANA, LUCAS, MARIANA, NATALIA, OTAVIO, OLIVIA, PAULA, RAFAEL, SOFIA,
    TIAGO, VITOR, YASMIN, ALICE, BIANCA, DIEGO, ELISA
};

/*
 * Cadastro fixo da rede. A ordem das linhas segue os IDs do enum Morador.
 */
static const struct {
    char nome[50];
    int bloco;
    int apartamento;
} moradores[] = {
    {"Ana", 'A', 101}, {"Bruno", 'A', 102}, {"Carla", 'A', 103},
    {"Daniel", 'A', 104}, {"Eduardo", 'A', 105}, {"Fernanda", 'B', 101},
    {"Gabriel", 'B', 102}, {"Helena", 'B', 103}, {"Igor", 'B', 104},
    {"Juliana", 'C', 101}, {"Lucas", 'C', 102}, {"Mariana", 'C', 103},
    {"Natalia", 'C', 104}, {"Otavio", 'C', 105}, {"Olivia", 'D', 101},
    {"Paula", 'A', 201}, {"Rafael", 'A', 202}, {"Sofia", 'B', 201},
    {"Tiago", 'B', 202}, {"Vitor", 'B', 203}, {"Yasmin", 'C', 201},
    {"Alice", 'C', 202}, {"Bianca", 'C', 203}, {"Diego", 'D', 201},
    {"Elisa", 'D', 202}
};

/*
 * Cada par representa uma amizade reciproca. Basta listar o par uma vez,
 * pois acrescentarAresta registra a ligacao nas duas listas de adjacencia.
 */
static const int amizades[][2] = {
    {ANA, BRUNO}, {ANA, CARLA}, {ANA, DANIEL}, {BRUNO, CARLA},
    {CARLA, DANIEL}, {CARLA, EDUARDO}, {DANIEL, EDUARDO},
    {FERNANDA, GABRIEL}, {FERNANDA, HELENA}, {GABRIEL, HELENA},
    {GABRIEL, IGOR}, {HELENA, IGOR},
    {JULIANA, LUCAS}, {LUCAS, MARIANA}, {MARIANA, NATALIA}, {NATALIA, OTAVIO},
    {ANA, PAULA}, {ANA, RAFAEL}, {PAULA, RAFAEL},
    {HELENA, SOFIA}, {HELENA, TIAGO}, {HELENA, VITOR},
    {SOFIA, TIAGO}, {SOFIA, VITOR}, {TIAGO, VITOR},
    {JULIANA, YASMIN}, {YASMIN, ALICE}, {ALICE, BIANCA}, {DIEGO, ELISA}
};

/*
 * Cria a rede com os dados fixos, executa as consultas da Parte 1 e
 * analisa as panelinhas e as pessoas influentes da Parte 2.
 */
int main(void)
{
    Vertice *G;
    Panelinha *lista = NULL;
    const int ordem = sizeof(moradores) / sizeof(moradores[0]);
    const int totalAmizades = sizeof(amizades) / sizeof(amizades[0]);
    int i, sucesso;

    criarGrafo(&G, ordem);
    if(G == NULL){
        fprintf(stderr, "Erro ao criar a rede.\n");
        return EXIT_FAILURE;
    }

    for(i = 0; i < ordem; i++){
        strcpy(G[i].nomePessoa, moradores[i].nome);
        G[i].bloco = moradores[i].bloco;
        G[i].apartamento = moradores[i].apartamento;
    }
    for(i = 0; i < totalAmizades; i++){
        if(!acrescentarAresta(G, ordem, amizades[i][0], amizades[i][1])){
            fprintf(stderr, "Erro ao inserir a amizade %d.\n", i + 1);
            destruirGrafo(&G, ordem);
            return EXIT_FAILURE;
        }
    }

    printf("Condominio | %d moradores e %d amizades\n", ordem, totalAmizades);
    imprimirGrafo(G, ordem);
    pessoasSemContato(G, ordem);
    pessoasComUmContato(G, ordem);
    pessoasComMaisContatos(G, ordem);
    gruposIsolados(G, ordem);

    sucesso = encontrarPanelinhas(G, ordem, &lista);
    if(sucesso){
        exibirPanelinhas(G, lista);
        sucesso = exibirPessoasInfluentes(G, ordem, lista);
    }
    if(!sucesso)
        fprintf(stderr, "Memoria insuficiente para analisar as panelinhas.\n");

    destruirPanelinhas(lista);
    destruirGrafo(&G, ordem);
    return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
