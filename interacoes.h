#ifndef INTERACOES_H
#define INTERACOES_H

#define MAX_VERTICES 80
#define MAX_LABEL_LENGTH 80 
#define TAM_NOME 100

typedef struct {
    int adj[MAX_VERTICES][MAX_VERTICES];
    char nome_ver[MAX_LABEL_LENGTH][MAX_LABEL_LENGTH];
    char nome_are[MAX_VERTICES][MAX_VERTICES][MAX_LABEL_LENGTH];
    int n;
} Grafo;

//Lista de Adjacencia:
typedef struct NodoAdj {
    int destino;
    struct NodoAdj* proximo;
} NodoAdj;

typedef struct GrafoAdj {
    int n;  // Número de vértices
    NodoAdj* listaAdj[MAX_VERTICES];  // Lista de adjacência
    char nomes[MAX_VERTICES][80];  // Nomes dos vértices
} GrafoAdj;

//Matriz de Adjacencia:
typedef struct GrafoMatriz {
    int n;  
    int adj[MAX_VERTICES][MAX_VERTICES];  
    char nomes[MAX_VERTICES][80];  
} GrafoMatriz;


//Matriz de Incidencia:
typedef struct {
    int n;  //número de vértices
    int m;  //número de arestas
    int incid[MAX_VERTICES][MAX_VERTICES+80]; //matriz de incidência
} GrafoIncidencia;

typedef struct { int u, v; } Edge;
typedef struct { int u, v; } Aresta;









void entrada_grafo_manual();
int contarVertices(char* nome);
int contarArestas(char* nome);
void inicializarGrafo(Grafo* g);
void lerGrafo(Grafo* g, char* nome);
int grauVertice(Grafo* g, int v);
void verificarGrau(Grafo* g);
void mostrarMaiorMenorGrau(Grafo* g);
void exemploCaminho(Grafo* g);
void exemploSubgrafo(Grafo* g);
int dfsCicloImprime(Grafo* g, int v, int visitado[], int pai[],int* cicloInicio, int* cicloFim);
void exemploCiclo(Grafo* g);
void resumo_geral();
NodoAdj* criarNodoAdjacente(int destino);
void adicionarArestaGrafo(GrafoAdj* g, int origem, int destino);
void processarNomeVertice(char* linha, char nomes[MAX_VERTICES][100], int* contVertice);
void processarArestas(char* linha, GrafoAdj* g);
void construirGrafoADot(GrafoAdj* g, const char* arquivoDot);
void exibirListaAdjacente(GrafoAdj* g);
void exibirListaDeAdjacencia();
void inicializarMatriz(GrafoMatriz* g);
void processarNomeVerticeMatriz(char* linha, char nomes[MAX_VERTICES][100], int* contVertice);
void processarArestasMatriz(char* linha, GrafoMatriz* g);
void construirGrafoMatrizDeDot(GrafoMatriz* g, const char* arquivoDot);
void exibirMatrizDeAdjacencia(GrafoMatriz* g);
void exibirMatrizAdjacente();
void lerGrafoIncidencia(GrafoIncidencia* g);
void imprimirMatrizIncidencia(GrafoIncidencia* g);
void exibirMatrizIncidencia();
int buscarVertice(const char* nomeVertice);
void limparBuffer();
void buscando();
bool contem(int* conjunto, int tam, int valor);
bool dfsCiclo(GrafoMatriz* g, int v, bool* visitado, int pai, int* subconjunto, int tam);
void verificarCicloConjunto();
void imprimirMatrizAdj(Grafo* g) ;
void inicializarGrafoAdj(GrafoAdj* ga, int n);
void adicionarArestaLista(GrafoAdj* ga, int u, int v);
void imprimirListaAdj(GrafoAdj* ga);
void inicializarGrafoIncidencia(GrafoIncidencia* gi, int n, int m);
void imprimirMatrizIncidenci(GrafoIncidencia* gi);
void matrizParaLista(Grafo* gm, GrafoAdj* ga);
void matrizParaIncidencia(Grafo* gm, GrafoIncidencia* gi);
void listaParaMatriz(GrafoAdj* ga, Grafo* gm);
void listaParaIncidencia(GrafoAdj* ga, GrafoIncidencia* gi);
void incidenciaParaMatriz(GrafoIncidencia* gi, Grafo* gm);
void incidenciaParaLista(GrafoIncidencia* gi, GrafoAdj* ga);
void executar_menu_transformacao();
int compararInteiros(const void *a, const void *b);
void calcularGraus(Grafo *g, int *graus);
int isomorfismoBacktrack(Grafo *g1, Grafo *g2, int *mapeamento, int *usado, int k);
bool verificaIsomorfismo();
bool verificaSubgrafo(Grafo *G_asterisco, Grafo *G, bool *disjuncaoAresta, bool *disjuncaoVertice);
void menuVerificacaoSubgrafo();
int encontrarIndicePorNome(Grafo* g, const char* nome);
void uniaoGrafos(Grafo* g1, Grafo* g2, Grafo* g_union);
void uniao_grafos();
void intersecaoGrafos(Grafo* g1, Grafo* g2, Grafo* g_intersecao);
void executarIntersecao() ;
void diferencaSimetricaGrafos(Grafo* g1, Grafo* g2, Grafo* g_dif);
void imprimirGrafoDOT(Grafo* g);
void executarDiferencaSimetrica();
void removerVertice(Grafo* G, int vi, Grafo* G2);
void executarRemoverVertice();
void removerAresta(Grafo* G, int v1, int v2, Grafo* G2) ;
int buscarIndiceVertice(Grafo* G, char* nome);
void executarRemoverAresta();
int buscarIndiceVerticePorNome(Grafo* G, const char* nome) ;
int remapIndice(int x, int vj);
void fundirVertices(Grafo* G, const char* nome_vi, const char* nome_vj, Grafo* G2);
void executarFundirVertices();
void dfsEuler(Grafo* g, int v, int visitado[]);
bool ehConexo(Grafo* g);
bool grafoDeEuler(Grafo* g);
void verificarEuler();
bool hamiltonianoUtil(Grafo* g, int pos, int caminho[], bool visitado[]);
bool existeCircuitoHamiltoniano(Grafo* g);
void verificarHamiltoniano();
bool eh_arvore();
bool eh_arvore_abrangente(Grafo* T, Grafo* G);
void encontrarCentrosArvore(Grafo* g_original);
void bfs_distancias(Grafo* g, int origem, int distancias[]);
void calcularExcentricidade(Grafo* g);
void calcularRaio(Grafo* g);
void copiarGrafo(Grafo* origem, Grafo* destino);
Aresta findCycleAndEdgeToRemove(Grafo* g, int start_node, int parent[], bool visited[], int added_u, int added_v);
bool gerarArvoreAbrangencia(Grafo* g_original, Grafo* a1_resultante);
void gerarOutrasArvoresAbrangencia(Grafo* g_original, Grafo* a1_base, int k);
int calcularDistanciaEntreArvores(Grafo* a1, Grafo* a2);
int obterRaio(Grafo* g);
bool compararArvores(Grafo* t1, Grafo* t2);
void adicionarArvoreUnica(Grafo* t);
void generateAllSpanningTrees(Grafo* current_tree, Grafo* g_original);
void encontrarArvoreCentral(Grafo* g_original);
void operacoes();
void arvores();




















































/*
typedef struct professores {
    int id;
    char* nome;
    char* materias;
    char* turno;
    int max_aulas;
} professor;

typedef struct disciplinas{
    int id;
    char* nome;
    int horas_disc;
    int tipo_sala;
    int peso;
    int semestre;
} disciplina;


typedef struct ProfAdj {
    professor* prof;
    struct ProfAdj *prox;
} ProfAdj;

typedef struct NoDisc {
    disciplina* disc;
    ProfAdj *lista_profs;  // Lista de professores que podem dar essa disciplina
} NoDisc;

disciplina* pegar_disciplinas(const char* arquivo_dot, int* total);
professor* carregar_professores(const char *nome_arquivo, int *quantidade);
void ler_professores(const char *nome_arquivo);
void ler_materias(const char *nome_arquivo);
char* trim(char* str);
void imprimir_lista_adjacencia(NoDisc* lista, int qtd_disc);
NoDisc* construir_lista_adjacencia(disciplina* disciplinas, int qtd_disc, professor* professores, int qtd_prof);
int* lista_professores_aulas(professor *professores, int qtd_prof);
int backtracking_max_disciplinas(
    NoDisc* lista_adjacencia,
    disciplina* disciplinas,
    int qtd_disc,
    professor* professores,
    int qtd_prof,
    int pos,
    int* lista,
    int* prof_aulas,
    int* max_disciplinas,
    int disciplinas_alocadas
);
*/

#endif