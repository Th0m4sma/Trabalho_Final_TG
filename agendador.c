#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

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


// --- VARIÁVEIS GLOBAIS ---
DisciplinaInfo METADADOS_DISCIPLINAS[] = {
    {"Redes", 6}, {"CG", 6}, {"PCD", 6}, {"ES", 6}, {"Compiladores", 6},
    {"PAA", 4}, {"POO", 4}, {"BD", 4}, {"AED_I", 2}
};
const int NUM_METADADOS = sizeof(METADADOS_DISCIPLINAS) / sizeof(METADADOS_DISCIPLINAS[0]);
HorarioInfo HORARIOS_INFO[NUM_HORARIOS];
Atribuicao ATRIBUICOES_PROFESSORES[MAX_DISCIPLINAS];
int num_atribuicoes = 0;


// --- FUNÇÕES DO GRAFO ---
Graph* criar_grafo() {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->num_vertices = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        g->adj_lists[i] = NULL;
    }
    return g;
}

void adicionar_aresta(Graph* g, int src, int dest) {
    Node* novo_no_dest = (Node*)malloc(sizeof(Node));
    novo_no_dest->vertice_idx = dest;
    novo_no_dest->next = g->adj_lists[src];
    g->adj_lists[src] = novo_no_dest;

    Node* novo_no_src = (Node*)malloc(sizeof(Node));
    novo_no_src->vertice_idx = src;
    novo_no_src->next = g->adj_lists[dest];
    g->adj_lists[dest] = novo_no_src;
}

int obter_ou_criar_vertice_idx(Graph* g, const char* nome) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (strcmp(g->vertex_names[i], nome) == 0) {
            return i;
        }
    }
    strcpy(g->vertex_names[g->num_vertices], nome);
    return g->num_vertices++;
}

Graph* carregar_grafo_dot(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "ERRO CRITICO: Nao foi possivel abrir o arquivo '%s'.\n", filename);
        fprintf(stderr, "Certifique-se que o arquivo 'grafo.dot' existe na mesma pasta.\n");
        exit(EXIT_FAILURE);
    }

    Graph* g = criar_grafo();
    char linha[MAX_LINHA_LEN];

    while (fgets(linha, sizeof(linha), fp)) {
        if (strstr(linha, "--") == NULL) continue;

        char* v1_str = strtok(linha, " \t\n;--");
        char* v2_str = strtok(NULL, " \t\n;--");

        if (v1_str && v2_str) {
            int idx1 = obter_ou_criar_vertice_idx(g, v1_str);
            int idx2 = obter_ou_criar_vertice_idx(g, v2_str);
            adicionar_aresta(g, idx1, idx2);
        }
    }
    fclose(fp);
    return g;
}

void liberar_grafo(Graph* g) {
    for (int i = 0; i < g->num_vertices; i++) {
        Node* atual = g->adj_lists[i];
        while (atual != NULL) {
            Node* temp = atual;
            atual = atual->next;
            free(temp);
        }
    }
    free(g);
}


// --- FUNÇÕES DE SETUP E HELPERS ---
void carregar_atribuicoes(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "ERRO CRITICO: Nao foi possivel abrir o arquivo '%s'.\n", filename);
        fprintf(stderr, "Rode o script python 'gerador_grafo.py' primeiro.\n");
        exit(EXIT_FAILURE);
    }
    while (num_atribuicoes < MAX_DISCIPLINAS && fscanf(fp, "%s %s", ATRIBUICOES_PROFESSORES[num_atribuicoes].disciplina, ATRIBUICOES_PROFESSORES[num_atribuicoes].professor) == 2) {
        num_atribuicoes++;
    }
    fclose(fp);
}

const char* obter_professor_por_disciplina(const char* nome_disciplina) {
    for (int i = 0; i < num_atribuicoes; i++) {
        if (strcmp(ATRIBUICOES_PROFESSORES[i].disciplina, nome_disciplina) == 0) {
            return ATRIBUICOES_PROFESSORES[i].professor;
        }
    }
    return "N/A";
}

void inicializar_horarios() {
    const char* dias[] = {"Segunda", "Terca", "Quarta", "Quinta", "Sexta"};
    const char* periodos[] = {"8h-10h", "10h-12h", "13h30h-15h30", "15h30h-17h30"};
    int id = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            HORARIOS_INFO[id].id = id;
            strcpy(HORARIOS_INFO[id].dia, dias[i]);
            sprintf(HORARIOS_INFO[id].descricao, "%s - %s", dias[i], periodos[j]);
            HORARIOS_INFO[id].penalidade_base = (j == 3) ? 1 : 0;
            id++;
        }
    }
}

int obter_periodo_por_nome(const char* nome_disciplina){
    for(int i = 0; i < NUM_METADADOS; i++){
        if(strcmp(METADADOS_DISCIPLINAS[i].nome_base, nome_disciplina) == 0){
            return METADADOS_DISCIPLINAS[i].periodo;
        }
    }
    return -1;
}

// --- LÓGICA DE COLORAÇÃO ---
void coloracao_inteligente(Graph* g, int* cores) {
    const int PENALIDADE_MESMO_DIA = 100;
    const int PENALIDADE_DIAS_CONSECUTIVOS = 10;
    const int PENALIDADE_EXCESSO_AULAS = 20;

    for (int i = 0; i < g->num_vertices; i++) cores[i] = -1;

    for (int i = 0; i < g->num_vertices; i++) {
        int vertice_atual_idx = i;
        
        bool horarios_proibidos[NUM_HORARIOS] = {false};
        Node* vizinho_no = g->adj_lists[vertice_atual_idx];
        while (vizinho_no != NULL) {
            int cor_vizinho = cores[vizinho_no->vertice_idx];
            if (cor_vizinho != -1) {
                horarios_proibidos[cor_vizinho] = true;
            }
            vizinho_no = vizinho_no->next;
        }

        int melhor_horario = -1;
        int menor_pontuacao = INT_MAX;

        for (int id_horario_candidato = 0; id_horario_candidato < NUM_HORARIOS; id_horario_candidato++) {
            if (horarios_proibidos[id_horario_candidato]) continue;

            int pontuacao_atual = 0;
            pontuacao_atual += HORARIOS_INFO[id_horario_candidato].penalidade_base;

            char nome_base_atual[MAX_NOME_LEN];
            strcpy(nome_base_atual, g->vertex_names[vertice_atual_idx]);
            *strrchr(nome_base_atual, '_') = '\0';

            char sufixo_atual = g->vertex_names[vertice_atual_idx][strlen(g->vertex_names[vertice_atual_idx]) - 1];
            char nome_irmao[MAX_NOME_LEN];
            sprintf(nome_irmao, "%s_%c", nome_base_atual, (sufixo_atual == '1' ? '2' : '1'));
            
            int idx_irmao = -1;
            for(int k=0; k<g->num_vertices; k++) if(strcmp(g->vertex_names[k], nome_irmao) == 0) idx_irmao = k;

            if(idx_irmao != -1 && cores[idx_irmao] != -1){
                int cor_irmao = cores[idx_irmao];
                const char* dias[] = {"Segunda", "Terca", "Quarta", "Quinta", "Sexta"};
                if(strcmp(HORARIOS_INFO[cor_irmao].dia, HORARIOS_INFO[id_horario_candidato].dia) == 0){
                    pontuacao_atual += PENALIDADE_MESMO_DIA;
                } else {
                    int dia_idx1 = -1, dia_idx2 = -1;
                    for(int d=0; d<5; d++){
                        if(strcmp(HORARIOS_INFO[cor_irmao].dia, dias[d]) == 0) dia_idx1 = d;
                        if(strcmp(HORARIOS_INFO[id_horario_candidato].dia, dias[d]) == 0) dia_idx2 = d;
                    }
                    if(abs(dia_idx1 - dia_idx2) == 1) {
                        pontuacao_atual += PENALIDADE_DIAS_CONSECUTIVOS;
                    }
                }
            }

            int periodo_atual = obter_periodo_por_nome(nome_base_atual);
            int aulas_no_dia = 0;
            for(int k=0; k < g->num_vertices; k++){
                if(cores[k] != -1 && strcmp(HORARIOS_INFO[cores[k]].dia, HORARIOS_INFO[id_horario_candidato].dia) == 0){
                    char nome_base_k[MAX_NOME_LEN];
                    strcpy(nome_base_k, g->vertex_names[k]);
                    *strrchr(nome_base_k, '_') = '\0';
                    if(obter_periodo_por_nome(nome_base_k) == periodo_atual) aulas_no_dia++;
                }
            }
            if(aulas_no_dia >= 3) pontuacao_atual += PENALIDADE_EXCESSO_AULAS * (aulas_no_dia - 2);

            if (pontuacao_atual < menor_pontuacao) {
                menor_pontuacao = pontuacao_atual;
                melhor_horario = id_horario_candidato;
            }
        }
        cores[vertice_atual_idx] = melhor_horario;
    }
}


// --- FUNÇÃO PRINCIPAL ---
int main() {
    const char* arquivo_grafo = "grafo.dot";
    const char* arquivo_atribuicoes = "atribuicoes.txt";

    // Carrega os dados necessários
    inicializar_horarios();
    carregar_atribuicoes(arquivo_atribuicoes);
    Graph* g = carregar_grafo_dot(arquivo_grafo);
    
    printf("Grafo carregado de '%s' com %d vertices.\n", arquivo_grafo, g->num_vertices);
    printf("Atribuicoes de professores carregadas de '%s'.\n\n", arquivo_atribuicoes);

    int* cores = (int*)malloc(g->num_vertices * sizeof(int));
    coloracao_inteligente(g, cores);
    
    printf("--- Grade Horaria Proposta ---\n");
    for (int id_cor = 0; id_cor < NUM_HORARIOS; id_cor++) {
        bool horario_usado = false;
        char buffer_aulas[MAX_LINHA_LEN] = "";

        for (int i = 0; i < g->num_vertices; i++) {
            if (cores[i] == id_cor) {
                // Extrai nome base para buscar professor
                char nome_base_disciplina[MAX_NOME_LEN];
                strcpy(nome_base_disciplina, g->vertex_names[i]);
                *strrchr(nome_base_disciplina, '_') = '\0';
                
                const char* professor = obter_professor_por_disciplina(nome_base_disciplina);

                // Formata a string de saída com o nome do professor
                char aula_formatada[MAX_NOME_LEN + 30];
                sprintf(aula_formatada, "%s (%s)", g->vertex_names[i], professor);
                
                if(horario_usado) strcat(buffer_aulas, ", ");
                strcat(buffer_aulas, aula_formatada);
                horario_usado = true;
            }
        }
        if(horario_usado){
            printf("[%s]: %s\n", HORARIOS_INFO[id_cor].descricao, buffer_aulas);
        }
    }

    // Libera memória
    free(cores);
    liberar_grafo(g);

    return 0;
}