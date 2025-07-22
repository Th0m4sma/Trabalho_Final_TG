#ifndef INTERACOES_H
#define INTERACOES_H

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

#endif