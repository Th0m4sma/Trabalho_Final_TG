#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "interacoes.h"

#define MAX_LINE 512
#define MAX_PROFESSORES 100
#define MAX_DISC  20

// => FUNÇÕES BASICAS PARA LER GRAFO.DOT COM PROFESSORES E DISCIPLINAS:
void ler_professores(const char *nome_arquivo) {
    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char line[MAX_LINE];
    printf("Professores encontrados:\n\n");

    while (fgets(line, MAX_LINE, file)) {
        // Remove espaços/tabs do início
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        if (ptr[0] == 'P' && isdigit(ptr[1])) {
            char *start = strstr(ptr, "label=");
            if (start) {
                start += strlen("label=");
                while (*start == ' ' || *start == '\t') start++;

                if (*start == '"') {
                    start++; // pula aspa inicial
                    char *end = strchr(start, '"');
                    if (end) {
                        int length = end - start;
                        char info[MAX_LINE];
                        strncpy(info, start, length);
                        info[length] = '\0';

                        // Substitui \n por quebras de linha reais
                        for (int i = 0; i < length; i++) {
                            if (info[i] == '\\' && info[i+1] == 'n') {
                                info[i] = '\n';
                                for (int j = i + 1; j < length; j++) {
                                    info[j] = info[j+1];
                                }
                                length--;
                            }
                        }

                        printf("%s\n\n", info);
                    }
                }
            }
        }
    }

    fclose(file);
}

void ler_materias(const char *nome_arquivo) {
    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char line[MAX_LINE];
    printf("Matérias encontradas:\n\n");

    while (fgets(line, MAX_LINE, file)) {
        // Remove espaços/tabs do início
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        // Verifica se começa com M e número (ex: M1, M2...)
        if (ptr[0] == 'M' && isdigit(ptr[1])) {
            char *start = strstr(ptr, "label=");
            if (start) {
                start += strlen("label=");
                while (*start == ' ' || *start == '\t') start++;

                if (*start == '"') {
                    start++; // pula aspa inicial
                    char *end = strchr(start, '"');
                    if (end) {
                        int length = end - start;
                        char materia[MAX_LINE];
                        strncpy(materia, start, length);
                        materia[length] = '\0';

                        printf("- %s\n", materia);
                    }
                }
            }
        }
    }

    fclose(file);
}

professor* carregar_professores(const char *nome_arquivo, int *quantidade) {
    FILE *file = fopen(nome_arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    professor *lista = malloc(sizeof(professor) * MAX_PROFESSORES);
    int count = 0;
    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, file)) {
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        if (ptr[0] == 'P' && isdigit(ptr[1])) {
            int id = atoi(ptr + 1); // Extrai número do P1, P2, etc.

            char *start = strstr(ptr, "label=");
            if (!start) continue;

            start += strlen("label=");
            while (*start == ' ' || *start == '\t') start++;

            if (*start != '"') continue;
            start++;

            char *end = strchr(start, '"');
            if (!end) continue;

            int length = end - start;
            char info[MAX_LINE];
            strncpy(info, start, length);
            info[length] = '\0';

            // Substitui \n por delimitador temporário '|'
            for (int i = 0; i < length; i++) {
                if (info[i] == '\\' && info[i+1] == 'n') {
                    info[i] = '|';  // Usar '|' como separador de campos
                    for (int j = i + 1; j < length; j++) {
                        info[j] = info[j+1];
                    }
                    length--;
                }
            }

            // Separar os campos
            char *nome = strtok(info, "|");
            char *materias = strtok(NULL, "|");
            char *turno = strtok(NULL, "|");
            char *aulas = strtok(NULL, "|");

            if (!nome || !materias || !turno || !aulas) continue;

            // Remover prefixos como "Nome: ", "Matérias: ", etc.
            nome += strlen("Nome: ");
            materias += strlen("Matérias: ");
            turno += strlen("Turno: ");
            aulas += strlen("Máx. Aulas: ");

            professor p;
            p.id = id;
            p.nome = strdup(nome);
            p.materias = strdup(materias);
            p.turno = strdup(turno);
            p.max_aulas = atoi(aulas);

            lista[count++] = p;
        }
    }

    fclose(file);
    *quantidade = count;
    return lista;
}

disciplina* pegar_disciplinas(const char* arquivo_dot, int* total) {
    FILE* arquivo = fopen(arquivo_dot, "r");
    int i;

    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    disciplina* lista = malloc(MAX_DISC * sizeof(disciplina));
    if (!lista) {
        fclose(arquivo);
        perror("Erro ao alocar memória para disciplinas");
        return NULL;
    }

    int contador = 0;
    char linha[MAX_LINE];
    int em_materias = 0;

    while (fgets(linha, MAX_LINE, arquivo)) {
        // Detecta início do subgrafo de matérias
        if (strstr(linha, "subgraph cluster_materias")) {
            em_materias = 1;
            continue;
        }

        // Detecta fim do subgrafo
        if (em_materias && linha[0] == '}') {
            break;
        }

        // Processa linhas dentro do subgrafo
        if (em_materias) {
            char* label_pos = strstr(linha, "label=\"");
            if (label_pos) {
                label_pos += strlen("label=\"");
                char* fim = strchr(label_pos, '"');
                if (fim) {
                    *fim = '\0';

                    disciplina d;
                    d.id = contador + 1;
                    d.nome = NULL;
                    d.horas_disc = 0;
                    d.tipo_sala = 0;
                    d.peso = 0;
                    d.semestre = 0;

                    // Divide o label por \n
                    char* token = strtok(label_pos, "\\n");
                    while (token) {
                        if (strstr(token, "nome:")) {
                            char* nome = strchr(token, ':');
                            if (nome) {
                                nome++;
                                while (*nome == ' ') nome++;
                                d.nome = strdup(nome);
                            }
                        } else if (strstr(token, "Horas:")) {
                            sscanf(token, "Horas: %d", &d.horas_disc);
                        } else if (strstr(token, "Sala:")) {
                            sscanf(token, "Sala: %d", &d.tipo_sala);
                        } else if (strstr(token, "semestre:")) {
                            sscanf(token, "semestre: %d", &d.semestre);
                        } else if (strstr(token, "peso:")) {
                            sscanf(token, "peso: %d", &d.peso);
                        } else {
                            // Caso esteja no formato direto: AED1\nHoras...\nSala...
                            if (!d.nome) {
                                d.nome = strdup(token);
                            }
                        }
                        token = strtok(NULL, "\\n");
                    }

                    if (d.nome != NULL) {
                        lista[contador++] = d;
                    }
                }
            }
        }
    }
    fclose(arquivo);

    *total = contador;
    return lista;
}















// => PARTE PARA CONSTRUIR LISTA DE ADJACENCIA, JUNTAMENTE COM FUNÇAO DE IMPRIMIR:

// Remove espaços à esquerda e à direita de uma string
char* trim(char* str) {
    while (isspace((unsigned char)*str)) str++;  // remove espaços do início

    if (*str == 0) return str;  // string vazia

    // remove espaços do final
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return str;
}

// Função auxiliar para verificar se uma disciplina está nas matérias do professor
bool professor_pode_dar(professor* prof, const char* nome_disciplina) {
    char materias_copia[512];
    strcpy(materias_copia, prof->materias);

    char nome_disciplina_limpa[128];
    strncpy(nome_disciplina_limpa, nome_disciplina, sizeof(nome_disciplina_limpa) - 1);
    nome_disciplina_limpa[sizeof(nome_disciplina_limpa) - 1] = '\0';
    strcpy(nome_disciplina_limpa, trim(nome_disciplina_limpa));  // Limpa espaços

    char* token = strtok(materias_copia, ",");
    while (token != NULL) {
        char* materia_limpa = trim(token);

        //printf("Comparando disciplina '%s' com '%s'\n", materia_limpa, nome_disciplina_limpa);

        if (strcmp(materia_limpa, nome_disciplina_limpa) == 0) {
            return true;
        }
        token = strtok(NULL, ",");
    }
    return false;
}

// Cria a lista de adjacência
NoDisc* construir_lista_adjacencia(disciplina* disciplinas, int qtd_disc, professor* professores, int qtd_prof) {
    NoDisc* lista = malloc(qtd_disc * sizeof(NoDisc));

    if (!lista) {
        perror("Erro ao alocar lista de adjacência");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < qtd_disc; i++) {
        lista[i].disc = &disciplinas[i];
        lista[i].lista_profs = NULL;

        for (int j = 0; j < qtd_prof; j++) {
            if (professor_pode_dar(&professores[j], disciplinas[i].nome)) {
                // Criar novo nó de professor
                ProfAdj* novo = malloc(sizeof(ProfAdj));
                novo->prof = &professores[j];
                novo->prox = lista[i].lista_profs;
                lista[i].lista_profs = novo;
            }
        }
    }

    return lista;
}

// Função que imprimi a lista de adjacencia
void imprimir_lista_adjacencia(NoDisc* lista, int qtd_disc) {
    printf("\n--- Lista de Adjacência (Disciplinas -> Professores) ---\n");
    for (int i = 0; i < qtd_disc; i++) {
        printf("Disciplina: %s (ID %d) -> ", lista[i].disc->nome, lista[i].disc->id);
        
        ProfAdj* atual = lista[i].lista_profs;
        if (!atual) {
            printf("Nenhum professor disponível.");
        }
        
        while (atual) {
            printf("[%s (ID %d)]", atual->prof->nome, atual->prof->id);
            atual = atual->prox;
            if (atual) printf(" -> ");
        }
        printf("\n");
    }
}


// => APLICANDO ESTRATEGIA PARA DETERMINAR OS PROFESSORES(BACKTRACKING):

int* determinando_professores_disciplina() {

}








