/*
 * PROJETO DE PROGRAMACAO - GRUPO SOCIAL (PARTE 2)
 * Teoria dos Grafos - 2026-2
 * Prof. Roberto C. de Araujo
 *
 * Integrantes (turma 06N):
 *   Bruna Amorim Maia            - RA: 10431883
 *   Fabyani Tiva Yan             - RA: 10431835
 *   Rafael Araujo Cabral Moreira - RA: 10441919
 *
 * Descricao: rede de amizades de 25 moradores de um condominio
 * residencial, modelada como um grafo nao orientado e simples.
 *   - vertice = morador (nome, bloco, apartamento e profissao)
 *   - aresta  = amizade (relacao reciproca: se A e amigo de B,
 *               B e amigo de A)
 *
 * Parte 1: moradores sem contato, com um contato, com mais contatos
 *          e grupos isolados (componentes conexos, por BFS).
 * Parte 2: panelinhas (cliques maximais com 3 ou mais membros) e
 *          pessoas influentes (unico membro comum a duas panelinhas).
 *
 * Base: implementacao de grafos Grafo_2026-2.c fornecida na disciplina.
 * Ajustes feitos na base (todos comentados nos pontos em que ocorrem):
 *   1. Vertice ganhou os campos nomePessoa, bloco, apartamento e profissao.
 *   2. acrescentarAresta rejeita lacos e amizades repetidas, pois uma
 *      amizade de alguem consigo mesmo, ou registrada duas vezes, nao faz
 *      sentido na rede e atrapalharia a contagem de contatos.
 *   3. imprimirGrafo exibe nomes e dados dos moradores, nao numeros.
 *
 * Para alterar a rede, basta editar tres blocos no final do arquivo:
 * o enum Morador, o vetor moradores[] e o vetor amizades[].
 *
 * Compilacao testada: gcc -ansi -pedantic -Wall -Wextra (sem avisos).
 */


/*
 * REPRESENTACAO DE GRAFOS
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/*
 * Estrutura de dados para representar grafos (lista de adjacencia)
 */
typedef struct aresta{ /* Celula de uma lista de arestas */
	int outroExtremo;      /* numero do morador do outro lado da amizade */
	struct aresta *prox;   /* proxima amizade do mesmo morador */
}Aresta;

typedef struct vertice{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
	int nome;              /* identificador numerico do morador (0..ordem-1) */
	char nomePessoa[50];   /* AJUSTE: nome completo do morador */
	int bloco;             /* AJUSTE: bloco onde mora (letra guardada como int) */
	int apartamento;       /* AJUSTE: numero do apartamento */
	char profissao[30];    /* AJUSTE: profissao do morador */
	Aresta *a;             /* inicio da lista de amigos */
}Vertice;


/*
 * Declaracoes das funcoes para manipulacao de grafos
 */
void criarGrafo(Vertice **G, int ordem);
void destruirGrafo(Vertice **G, int ordem);
int acrescentarAresta(Vertice G[], int ordem, int v1, int v2);
void imprimirGrafo(Vertice G[], int ordem);


/*
 * Declaracoes das consultas da Parte 1
 */
void pessoasSemContato(Vertice G[], int ordem);
int contarContatos(Vertice G[], int v);
void pessoasComUmContato(Vertice G[], int ordem);
void pessoasComMaisContatos(Vertice G[], int ordem);
void gruposIsolados(Vertice G[], int ordem);


/*
 * Criacao de um grafo com ordem predefinida (passada como argumento),
 * e, inicialmente, sem nenhuma aresta
 */
void criarGrafo(Vertice **G, int ordem){
	int i;

	*G = (Vertice*) malloc(sizeof(Vertice) * ordem);

	/* sem memoria: o chamador testa se *G ficou NULL */
	if(*G == NULL)
		return;

	/* cada morador comeca sem dados e sem amigos */
	for(i = 0; i < ordem; i++){
		(*G)[i].nome = i;
		(*G)[i].nomePessoa[0] = '\0';
		(*G)[i].bloco = ' ';
		(*G)[i].apartamento = 0;
		(*G)[i].profissao[0] = '\0';
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

		/* libera a lista de amigos do morador i, celula por celula;
		 * n guarda a proxima antes de p ser liberada */
		while(p != NULL){
			n = p->prox;
			free(p);
			p = n;
		}
	}

	free(*G);
	*G = NULL;
}


/*
 * Acrescenta uma nova aresta em um grafo previamente criado.
 * Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada.
 *
 * Como o grafo nao e orientado, para uma aresta com extremos i e j,
 * sao criadas as entradas (i,j) e (j,i).
 *
 * AJUSTE em relacao a base: lacos (v1 == v2) e arestas repetidas sao
 * rejeitados (retorno 0), para que o grafo da rede seja sempre simples.
 * Retorna 1 quando a amizade e registrada.
 */
int acrescentarAresta(Vertice G[], int ordem, int v1, int v2){
	Aresta *A1, *A2;

	/* extremos precisam ser moradores validos */
	if(v1 < 0 || v1 >= ordem)
		return 0;

	if(v2 < 0 || v2 >= ordem)
		return 0;

	/* ninguem e amigo de si mesmo */
	if(v1 == v2)
		return 0;

	/* se v2 ja esta na lista de v1, a amizade ja existe */
	for(A1 = G[v1].a; A1 != NULL; A1 = A1->prox)
		if(A1->outroExtremo == v2)
			return 0;

	A1 = (Aresta*) malloc(sizeof(Aresta));
	A2 = (Aresta*) malloc(sizeof(Aresta));
	if(A1 == NULL || A2 == NULL){
		free(A1);
		free(A2);
		return 0;
	}

	/* insere v2 no inicio da lista de v1 */
	A1->outroExtremo = v2;
	A1->prox = G[v1].a;
	G[v1].a = A1;

	/* insere v1 no inicio da lista de v2 (relacao reciproca) */
	A2->outroExtremo = v1;
	A2->prox = G[v2].a;
	G[v2].a = A2;

	return 1;
}


/*
 * Imprime cada morador, seus dados e seus amigos.
 * AJUSTE em relacao a base: mostra nomes, e nao numeros de vertices.
 * Os amigos aparecem na ordem inversa do cadastro, porque
 * acrescentarAresta insere sempre no inicio da lista.
 */
void imprimirGrafo(Vertice G[], int ordem){
	int i;
	Aresta *aux;

	printf("\nRede de amizades do condominio:\n");

	for(i = 0; i < ordem; i++){

		/* nome + 1 para numerar os moradores de 01 a 25 na tela */
		printf("\n%02d | %s (%s, bloco %c, ap. %d)\n     Amigos: ",
		       G[i].nome + 1, G[i].nomePessoa, G[i].profissao,
		       G[i].bloco, G[i].apartamento);

		aux = G[i].a;

		if(aux == NULL){
			printf("sem contatos");
		}

		/* percorre a lista de adjacencia do morador i */
		for(; aux != NULL; aux = aux->prox){
			printf("%s  ", G[aux->outroExtremo].nomePessoa);
		}
	}

	printf("\n\n");
}


/*
 * Exibe os moradores que nao possuem nenhum contato na rede.
 * Um morador sem contato e um vertice de grau 0, ou seja, sua
 * lista de adjacencia esta vazia (G[i].a == NULL).
 */
void pessoasSemContato(Vertice G[], int ordem){
	int i;
	int encontrou = 0;   /* vira 1 quando aparece o primeiro morador sem contato */

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
 * Funcao auxiliar: devolve o numero de contatos (grau) do morador v,
 * contando as celulas da sua lista de adjacencia.
 */
int contarContatos(Vertice G[], int v) {
	int count = 0;
	Aresta *aux = G[v].a;

	/* cada celula da lista e um amigo diferente */
	while(aux != NULL) {
		count++;
		aux = aux->prox;
	}
	return count;
}

/*
 * Exibe os moradores com exatamente 1 contato (vertices de grau 1).
 */
void pessoasComUmContato(Vertice G[], int ordem) {
	int i, count;
	int encontrou = 0;   /* vira 1 quando aparece o primeiro morador de grau 1 */

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
 * Exibe o(s) morador(es) com mais contatos (vertices de grau maximo).
 * Sao feitas duas passadas: a primeira descobre o grau maximo e a
 * segunda imprime todos que o atingem, o que trata empates.
 */
void pessoasComMaisContatos(Vertice G[], int ordem) {
	int i, count;
	int max = -1;   /* menor que qualquer grau possivel */

	/* Primeiro passo: descobre qual e o numero maximo de contatos */
	for(i = 0; i < ordem; i++) {
		count = contarContatos(G, i);
		if(count > max) {
			max = count;
		}
	}

	printf("\nMorador(es) com mais contatos na rede (%d contatos):\n", max);

	/* rede sem nenhuma amizade: nao ha quem destacar */
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
 * Exibe os grupos isolados da rede: grupos em que nenhum morador de
 * dentro tem contato com algum morador de fora. Cada grupo isolado e
 * um componente conexo do grafo, encontrado por busca em largura (BFS).
 *
 * ADAPTACOES em relacao a BFS do Cormen (3a ed., secao 22.2):
 *   - a fila abstrata (ENQUEUE/DEQUEUE) virou um vetor com os indices
 *     inicio (retirada) e fim (insercao), pois C ANSI nao tem fila pronta;
 *     como cada morador entra uma unica vez, o vetor de tamanho ordem basta;
 *   - as cores BRANCO/CINZA/PRETO viraram o vetor grupo[]: 0 equivale a
 *     BRANCO e qualquer outro valor diz em qual grupo o morador esta;
 *   - a distancia d[v] e o predecessor pi[v] nao sao calculados, pois so
 *     interessa saber a que componente cada morador pertence;
 *   - o Cormen faz uma busca a partir de um unico vertice s; aqui e feita
 *     uma busca a partir de cada morador ainda nao visitado, para achar
 *     todos os componentes (e o numero de buscas = numero de grupos).
 * Complexidade: O(n + m), n = moradores e m = amizades.
 */
void gruposIsolados(Vertice G[], int ordem){
	int *grupo;   /* grupo[i] = numero do grupo do morador i (0 = nao visitado) */
	int *fila;    /* fila usada na busca em largura */
	int inicio, fim;   /* posicao de retirada e de insercao na fila */
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

	/* passo 1: ninguem foi visitado ainda (todos BRANCOS) */
	for(i = 0; i < ordem; i++)
		grupo[i] = 0;

	/* passos 2 a 4: uma busca em largura para cada grupo novo */
	for(i = 0; i < ordem; i++){

		if(grupo[i] != 0)
			continue; /* morador ja pertence a um grupo encontrado */

		/* i inicia um grupo novo: fila zerada e i como origem da busca */
		qtdGrupos++;
		inicio = 0;
		fim = 0;

		fila[fim++] = i;
		grupo[i] = qtdGrupos;

		/* enquanto houver morador na fila (fila nao vazia) */
		while(inicio < fim){
			v = fila[inicio++];   /* DEQUEUE */

			/* examina todos os amigos de v */
			for(aux = G[v].a; aux != NULL; aux = aux->prox){
				w = aux->outroExtremo;

				/* amigo ainda nao visitado: entra no grupo atual e na fila */
				if(grupo[w] == 0){
					grupo[w] = qtdGrupos;
					fila[fim++] = w;   /* ENQUEUE */
				}
			}
		}
		/* fila vazia: todos os alcancaveis a partir de i ja estao no grupo */
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


/* =====================================================================
 * PARTE 2 - PANELINHAS E PESSOAS INFLUENTES
 * =====================================================================
 *
 * Panelinha: subconjunto MAXIMAL de moradores, com pelo menos 3 pessoas,
 * em que todos os pares sao amigos. Em grafos, e um clique maximal
 * (subgrafo completo que nao esta contido em outro subgrafo completo)
 * com 3 ou mais vertices.
 *
 * Pessoa influente: participa de duas (ou mais) panelinhas e e o unico
 * participante comum a elas. Na pratica, basta existir um par de
 * panelinhas cuja intersecao seja exatamente {pessoa}.
 */

/*
 * Lista ligada com as panelinhas encontradas. Cada celula guarda os
 * identificadores dos membros em ordem crescente (a busca os gera assim)
 * e um ponteiro para a proxima panelinha.
 */
typedef struct panelinha {
	int tamanho;              /* quantidade de membros */
	int *membros;             /* identificadores dos membros, em ordem crescente */
	struct panelinha *prox;   /* proxima panelinha da lista */
} Panelinha;

/*
 * Libera os vetores de membros e as celulas da lista de panelinhas.
 */
static void destruirPanelinhas(Panelinha *lista)
{
	Panelinha *prox;

	while(lista != NULL){
		prox = lista->prox;   /* guarda a proxima antes de liberar a atual */
		free(lista->membros);
		free(lista);
		lista = prox;
	}
}

/*
 * Devolve 1 se o morador v e amigo de TODOS os membros do grupo atual
 * e 0 caso contrario. Usa a matriz adj (adj[v*ordem + u] == 1 se v e u
 * sao amigos), que responde cada consulta em tempo constante.
 */
static int amigoDeTodos(const unsigned char *adj, int ordem, int v,
                        const int membros[], int tamanho)
{
	int i;

	/* basta um membro que nao seja amigo de v para v nao servir */
	for(i = 0; i < tamanho; i++)
		if(!adj[(size_t)v * ordem + membros[i]])
			return 0;
	return 1;
}

/*
 * Busca recursiva (backtracking) que enumera os cliques do grafo e
 * guarda na lista os que sao panelinhas.
 *
 *   membros[0..tamanho-1]: clique em construcao (todos amigos entre si)
 *   inicio: menor identificador que ainda pode ser acrescentado
 *   fim: endereco do ponteiro onde a proxima panelinha sera ligada
 *        (permite inserir no final da lista sem percorre-la)
 *
 * Funcionamento:
 *   1. Tenta acrescentar cada morador v >= inicio que seja amigo de todos
 *      os membros; para cada um, chama a si mesma com o clique aumentado
 *      e inicio = v + 1. Como os moradores entram sempre em ordem
 *      crescente, cada clique e gerado uma unica vez.
 *   2. Depois de explorar as extensoes, se o clique tiver 3 ou mais
 *      membros, testa se ele e maximal: procura, entre TODOS os moradores
 *      (inclusive os de identificador menor que inicio), alguem amigo de
 *      todos. Se existir, o clique esta contido em outro maior e nao e
 *      panelinha. Se nao existir, e registrado.
 *
 * ADAPTACOES em relacao ao algoritmo de Bron e Kerbosch (1973, versao 1),
 * que enumera cliques maximais com tres conjuntos R, P e X:
 *   - R (clique atual) e o vetor membros[];
 *   - P (candidatos) nao e guardado: sao os v >= inicio que passam em
 *     amigoDeTodos, calculados no laco do passo 1;
 *   - X (ja processados) tambem nao e guardado: o teste de maximalidade
 *     do passo 2 percorre todos os moradores, o que equivale a exigir
 *     P e X vazios para registrar o clique;
 *   - nao ha pivo nem poda de ramos, pois a rede tem so 25 moradores;
 *   - foi acrescentado o filtro de tamanho minimo 3 exigido no enunciado.
 * O tempo e exponencial no pior caso (como em qualquer enumeracao de
 * cliques), mas e imediato para redes do tamanho da usada aqui.
 *
 * Retorna 1 ao concluir a busca ou 0 se faltar memoria.
 */
static int buscarPanelinhas(const unsigned char *adj, int ordem, int membros[],
                            int tamanho, int inicio, Panelinha ***fim)
{
	int v;
	Panelinha *nova;

	/* passo 1: acrescenta um amigo de todos e explora as extensoes do grupo */
	for(v = inicio; v < ordem; v++){
		if(amigoDeTodos(adj, ordem, v, membros, tamanho)){
			membros[tamanho] = v;
			if(!buscarPanelinhas(adj, ordem, membros, tamanho + 1, v + 1, fim))
				return 0;
			/* ao voltar da recursao, membros[tamanho] sera sobrescrito
			 * pelo proximo candidato: e o "retrocesso" do backtracking */
		}
	}

	/* panelinha precisa ter pelo menos tres membros */
	if(tamanho < 3)
		return 1;

	/* passo 2: testa a maximalidade com todos os IDs, inclusive os
	 * menores que inicio. Como a diagonal de adj e zero, um membro do
	 * grupo nunca e "amigo de todos" e nao pode estende-lo. */
	for(v = 0; v < ordem; v++)
		if(amigoDeTodos(adj, ordem, v, membros, tamanho))
			return 1;   /* existe extensao: nao e maximal */

	/* clique maximal com 3+ membros: copia para uma nova celula */
	nova = (Panelinha*) malloc(sizeof(Panelinha));
	if(nova == NULL)
		return 0;
	nova->membros = (int*) malloc(sizeof(int) * tamanho);
	if(nova->membros == NULL){
		free(nova);
		return 0;
	}
	memcpy(nova->membros, membros, sizeof(int) * tamanho);
	nova->tamanho = tamanho;
	nova->prox = NULL;

	/* liga no final da lista e avanca o ponteiro de fim */
	**fim = nova;
	*fim = &nova->prox;
	return 1;
}

/*
 * Prepara e dispara a busca de panelinhas.
 *
 * ADAPTACAO: a estrutura do Grafo_2026-2.c (lista de adjacencia) continua
 * sendo a representacao da rede. Aqui e montada so uma matriz de
 * adjacencia AUXILIAR, temporaria, porque a busca pergunta muitas vezes
 * "u e amigo de v?"; na lista isso custaria percorrer a lista de u,
 * na matriz e uma consulta direta. A matriz e liberada ao final.
 *
 * Devolve 1 em caso de sucesso (a lista fica em *lista; o chamador a
 * libera com destruirPanelinhas) ou 0 se faltar memoria.
 */
static int encontrarPanelinhas(Vertice G[], int ordem, Panelinha **lista)
{
	unsigned char *adj;   /* matriz ordem x ordem guardada em um vetor */
	int *membros;         /* clique em construcao durante a busca */
	int v, sucesso;
	Aresta *aresta;
	Panelinha **fim = lista;   /* comeca apontando para a cabeca da lista */

	/* calloc zera a matriz: ninguem e amigo de ninguem ainda */
	adj = (unsigned char*) calloc((size_t)ordem * ordem, 1);
	membros = (int*) malloc(sizeof(int) * ordem);
	if(adj == NULL || membros == NULL){
		free(adj);
		free(membros);
		return 0;
	}

	/* copia cada amizade da lista de adjacencia para a matriz */
	for(v = 0; v < ordem; v++)
		for(aresta = G[v].a; aresta != NULL; aresta = aresta->prox)
			adj[(size_t)v * ordem + aresta->outroExtremo] = 1;

	/* comeca com o grupo vazio, podendo usar qualquer morador */
	sucesso = buscarPanelinhas(adj, ordem, membros, 0, 0, &fim);
	free(adj);
	free(membros);
	return sucesso;
}

/*
 * Numera as panelinhas (P1, P2, ...) na ordem da lista e exibe o nome de
 * cada membro. Essa numeracao e usada depois na lista de influentes.
 */
static void exibirPanelinhas(Vertice G[], const Panelinha *lista)
{
	int numero = 1;
	int i;

	printf("\nPanelinhas da rede (grupos de 3 ou mais moradores em que todos\n");
	printf("sao amigos entre si e que nao podem ser ampliados):\n");
	if(lista == NULL)
		printf("Nenhuma panelinha encontrada.\n");

	for(; lista != NULL; lista = lista->prox){
		printf("P%d: {", numero++);
		for(i = 0; i < lista->tamanho; i++){
			if(i > 0)
				printf(", ");   /* separador so a partir do segundo nome */
			printf("%s", G[lista->membros[i]].nomePessoa);
		}
		printf("}\n");
	}
}

/*
 * Calcula a intersecao de duas panelinhas percorrendo os dois vetores
 * ordenados ao mesmo tempo (como na intercalacao do merge sort).
 * Retorna o identificador do unico membro em comum, ou -1 se a
 * intersecao tiver zero ou mais de um membro.
 */
static int unicoEmComum(const Panelinha *a, const Panelinha *b)
{
	int i = 0, j = 0, comum = -1;

	while(i < a->tamanho && j < b->tamanho){
		if(a->membros[i] < b->membros[j])
			i++;            /* menor de a nao pode estar em b: avanca em a */
		else if(a->membros[i] > b->membros[j])
			j++;            /* menor de b nao pode estar em a: avanca em b */
		else {
			if(comum != -1)
				return -1;  /* segundo membro comum: nao e unico */
			comum = a->membros[i];
			i++;
			j++;
		}
	}
	return comum;
}

/*
 * Compara cada par de panelinhas uma unica vez (P_i com P_j, i < j).
 * Quando a intersecao do par tem um so morador, ele e influente.
 * Para cada influente guarda o primeiro par que comprova o resultado,
 * e assim cada pessoa e exibida uma unica vez.
 * Retorna 0 apenas se faltar memoria.
 */
static int exibirPessoasInfluentes(Vertice G[], int ordem, const Panelinha *lista)
{
	const Panelinha *a, *b;
	int *primeira, *segunda;   /* par (P_primeira, P_segunda) que comprova cada influente; 0 = nao influente */
	int i, j, v, encontrou = 0;

	primeira = (int*) calloc(ordem, sizeof(int));
	segunda = (int*) calloc(ordem, sizeof(int));
	if(primeira == NULL || segunda == NULL){
		free(primeira);
		free(segunda);
		return 0;
	}

	/* Os numeros i e j correspondem aos exibidos na lista de panelinhas. */
	for(a = lista, i = 1; a != NULL; a = a->prox, i++){
		for(b = a->prox, j = i + 1; b != NULL; b = b->prox, j++){
			v = unicoEmComum(a, b);
			/* registra so o primeiro par encontrado para v */
			if(v >= 0 && primeira[v] == 0){
				primeira[v] = i;
				segunda[v] = j;
			}
		}
	}

	printf("\nPessoas influentes na rede (unico membro comum a duas panelinhas):\n");
	for(v = 0; v < ordem; v++){
		if(primeira[v] != 0){
			printf("%s (unica pessoa comum a P%d e P%d)\n",
			       G[v].nomePessoa, primeira[v], segunda[v]);
			encontrou = 1;
		}
	}
	if(!encontrou)
		printf("Nenhuma pessoa influente encontrada.\n");
	printf("\n");

	free(primeira);
	free(segunda);
	return 1;
}


/* =====================================================================
 * DADOS DA REDE (edite aqui para alterar a rede)
 * =====================================================================
 * Para incluir um morador: acrescente o nome no FINAL do enum e uma
 * linha no FINAL de moradores[]. Para incluir ou remover uma amizade:
 * acrescente ou apague o par em amizades[]. A ordem do grafo e o numero
 * de amizades sao calculados automaticamente pelo tamanho dos vetores.
 */

/* IDs fixos deixam as amizades legiveis (ANA em vez de 0). */
enum Morador {
	ANA, BRUNO, CARLA, DANIEL, EDUARDO, FERNANDA, GABRIEL, HELENA, IGOR,
	JULIANA, LUCAS, MARIANA, NATALIA, OTAVIO, OLIVIA, PAULA, RAFAEL, SOFIA,
	TIAGO, VITOR, YASMIN, ALICE, BIANCA, DIEGO, ELISA
};

/*
 * Cadastro fixo dos moradores. A ordem das linhas segue os IDs do enum.
 * Campos: nome completo, bloco, apartamento, profissao.
 */
static const struct {
	char nome[50];
	int bloco;
	int apartamento;
	char profissao[30];
} moradores[] = {
	{"Ana Ribeiro",     'A', 101, "advogada"},
	{"Bruno Almeida",   'A', 102, "engenheiro"},
	{"Carla Mendes",    'A', 103, "sindica"},
	{"Daniel Costa",    'A', 104, "professor"},
	{"Eduardo Lima",    'A', 105, "medico"},
	{"Fernanda Rocha",  'B', 101, "arquiteta"},
	{"Gabriel Souza",   'B', 102, "estudante"},
	{"Helena Martins",  'B', 103, "subsindica"},
	{"Igor Pereira",    'B', 104, "programador"},
	{"Juliana Castro",  'C', 101, "dentista"},
	{"Lucas Ferreira",  'C', 102, "estudante"},
	{"Mariana Lopes",   'C', 103, "enfermeira"},
	{"Natalia Gomes",   'C', 104, "designer"},
	{"Otavio Barros",   'C', 105, "aposentado"},
	{"Olivia Nunes",    'D', 101, "piloto de aviao"},
	{"Paula Teixeira",  'A', 201, "advogada"},
	{"Rafael Moura",    'A', 202, "jornalista"},
	{"Sofia Cardoso",   'B', 201, "personal trainer"},
	{"Tiago Ramos",     'B', 202, "chef de cozinha"},
	{"Vitor Azevedo",   'B', 203, "musico"},
	{"Yasmin Freitas",  'C', 201, "veterinaria"},
	{"Alice Monteiro",  'C', 202, "estudante"},
	{"Bianca Duarte",   'C', 203, "contadora"},
	{"Diego Pinto",     'D', 201, "fotografo"},
	{"Elisa Carvalho",  'D', 202, "fotografa"}
};

/*
 * Cada par representa uma amizade reciproca. Basta listar o par uma vez,
 * pois acrescentarAresta registra a ligacao nas duas listas de adjacencia.
 * Os blocos abaixo seguem os grupos da rede (ver documentacao).
 */
static const int amizades[][2] = {
	/* bloco A: Ana, Bruno, Carla, Daniel, Eduardo, Paula, Rafael */
	{ANA, BRUNO}, {ANA, CARLA}, {ANA, DANIEL}, {BRUNO, CARLA},
	{CARLA, DANIEL}, {CARLA, EDUARDO}, {DANIEL, EDUARDO},
	{ANA, PAULA}, {ANA, RAFAEL}, {PAULA, RAFAEL},
	/* bloco B: Fernanda, Gabriel, Helena, Igor, Sofia, Tiago, Vitor */
	{FERNANDA, GABRIEL}, {FERNANDA, HELENA}, {GABRIEL, HELENA},
	{GABRIEL, IGOR}, {HELENA, IGOR},
	{HELENA, SOFIA}, {HELENA, TIAGO}, {HELENA, VITOR},
	{SOFIA, TIAGO}, {SOFIA, VITOR}, {TIAGO, VITOR},
	/* bloco C: caminho Otavio - ... - Bianca, sem triangulos */
	{JULIANA, LUCAS}, {LUCAS, MARIANA}, {MARIANA, NATALIA}, {NATALIA, OTAVIO},
	{JULIANA, YASMIN}, {YASMIN, ALICE}, {ALICE, BIANCA},
	/* bloco D: Diego e Elisa (Olivia nao tem amizades) */
	{DIEGO, ELISA}
};


/*
 * Programa principal: cria a rede com os dados fixos, exibe a rede,
 * executa as consultas da Parte 1 e as da Parte 2. Nao ha leitura de
 * teclado durante as consultas.
 */
int main(void)
{
	Vertice *G;
	Panelinha *lista = NULL;
	const int ordem = sizeof(moradores) / sizeof(moradores[0]);
	const int totalAmizades = sizeof(amizades) / sizeof(amizades[0]);
	int i, sucesso;

	/* 1. cria o grafo com um vertice por morador */
	criarGrafo(&G, ordem);
	if(G == NULL){
		fprintf(stderr, "Erro ao criar a rede.\n");
		return EXIT_FAILURE;
	}

	/* 2. copia os dados de cada morador para o seu vertice */
	for(i = 0; i < ordem; i++){
		strcpy(G[i].nomePessoa, moradores[i].nome);
		G[i].bloco = moradores[i].bloco;
		G[i].apartamento = moradores[i].apartamento;
		strcpy(G[i].profissao, moradores[i].profissao);
	}

	/* 3. registra as amizades; um par invalido ou repetido interrompe o programa */
	for(i = 0; i < totalAmizades; i++){
		if(!acrescentarAresta(G, ordem, amizades[i][0], amizades[i][1])){
			fprintf(stderr, "Erro ao inserir a amizade %d.\n", i + 1);
			destruirGrafo(&G, ordem);
			return EXIT_FAILURE;
		}
	}

	/* 4. exibe a rede completa */
	printf("Condominio | %d moradores e %d amizades\n", ordem, totalAmizades);
	imprimirGrafo(G, ordem);

	/* 5. consultas da Parte 1 */
	pessoasSemContato(G, ordem);
	pessoasComUmContato(G, ordem);
	pessoasComMaisContatos(G, ordem);
	gruposIsolados(G, ordem);

	/* 6. consultas da Parte 2: so exibe influentes se as panelinhas foram achadas */
	sucesso = encontrarPanelinhas(G, ordem, &lista);
	if(sucesso){
		exibirPanelinhas(G, lista);
		sucesso = exibirPessoasInfluentes(G, ordem, lista);
	}
	if(!sucesso)
		fprintf(stderr, "Memoria insuficiente para analisar as panelinhas.\n");

	/* 7. libera toda a memoria dinamica */
	destruirPanelinhas(lista);
	destruirGrafo(&G, ordem);

	/* 8. segura a janela do console no Dev-C++ */
	printf("Pressione ENTER para terminar\n");
	getchar();

	return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
