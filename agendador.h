#ifndef AGENDADOR_H
#define AGENDADOR_H

// --- CONSTANTES GLOBAIS ---
#define MAX_VERTICES 50
#define MAX_DISCIPLINAS 25
#define MAX_NOME_LEN 50
#define MAX_LINHA_LEN 256
#define NUM_HORARIOS 20


// --- ESTRUTURAS DE DADOS ---
typedef struct Node {
    int vertice_idx;
    struct Node* next;
} Node;

typedef struct {
    int num_vertices;
    Node* adj_lists[MAX_VERTICES];
    char vertex_names[MAX_VERTICES][MAX_NOME_LEN];
} Graph;

typedef struct {
    int id;
    char dia[20];
    char descricao[50];
    int penalidade_base;
} HorarioInfo;

typedef struct {
    char nome_base[MAX_NOME_LEN];
    int periodo;
} DisciplinaInfo;

typedef struct {
    char disciplina[MAX_NOME_LEN];
    char professor[MAX_NOME_LEN];
} Atribuicao;



extern DisciplinaInfo METADADOS_DISCIPLINAS[];
extern const int NUM_METADADOS;
extern HorarioInfo HORARIOS_INFO[NUM_HORARIOS];
extern Atribuicao ATRIBUICOES_PROFESSORES[MAX_DISCIPLINAS];
extern int num_atribuicoes;



Graph* criar_grafo();
void adicionar_aresta(Graph* g, int src, int dest);
int obter_ou_criar_vertice_idx(Graph* g, const char* nome);
Graph* carregar_grafo_dot(const char* filename);
void liberar_grafo(Graph* g);
void carregar_atribuicoes(const char* filename);
const char* obter_professor_por_disciplina(const char* nome_disciplina);
void inicializar_horarios();
int obter_periodo_por_nome(const char* nome_disciplina);
void coloracao_inteligente(Graph* g, int* cores);
int agendador_funcao();


#endif