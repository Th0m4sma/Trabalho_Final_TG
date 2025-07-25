#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include "interacoes.h"
#include "interface.h"

Grafo todas_arvores_abrangentes[1000]; // Limite arbitrário, pode estourar
int num_arvores_encontradas = 0;



void entrada_grafo_manual() {
    int vertices, i, arestas, v1, v2;
    FILE *file;

    printf("Digite o numero de vertices: ");
    scanf("%d", &vertices);
    getchar(); // limpar \n após scanf

    if (vertices > MAX_VERTICES) {
        printf("Numero de vertices maior que o permitido (%d)\n", MAX_VERTICES);
        return;
    }

    char nomes[MAX_VERTICES][MAX_LABEL_LENGTH];
    for (i = 0; i < vertices; i++) {
        printf("Digite o nome do vértice %d: ", i);
        if (fgets(nomes[i], sizeof(nomes[i]), stdin) == NULL) {
            printf("Erro ao ler nome do vertice.\n");
            return;
        }
        nomes[i][strcspn(nomes[i], "\n")] = '\0'; // remove \n do final
    }

    printf("Digite o numero de arestas: ");
    scanf("%d", &arestas);

    file = fopen("grafo.dot", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "graph {\n");

    // Escreve os vertices com label
    for (i = 0; i < vertices; i++) {
        fprintf(file, "  %d [label=\"%s\"];\n", i, nomes[i]);
    }
    fprintf(file, "\n");

    // Escreve as arestas
    for (i = 0; i < arestas; i++) {
        printf("Aresta %d. Digite o indice do vertice 1 (0 a %d): ", i + 1, vertices - 1);
        scanf("%d", &v1);
        printf("Aresta %d. Digite o indice do vertice 2 (0 a %d): ", i + 1, vertices - 1);
        scanf("%d", &v2);

        if (v1 < 0 || v1 >= vertices || v2 < 0 || v2 >= vertices) {
            printf("Indice invalido. Digite novamente.\n");
            i--;
            continue;
        }
        fprintf(file, "  %d -- %d;\n", v1, v2);
    }

    fprintf(file, "}\n");
    fclose(file);
}

int contarVertices(char* nome) {
    char buffer[256];
    int vertices[MAX_VERTICES] = {0};
    int maxVertice = -1;
    int id;
    int total = 0;

    FILE* f = fopen(nome, "r");
    if (!f) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), f)) {
        if (strstr(buffer, "graph") || strstr(buffer, "}")) continue;

        if (strchr(buffer, '[')) {
            if (sscanf(buffer, " %d [label=", &id) == 1) {
                if (id >= 0 && id < MAX_VERTICES) {
                    vertices[id] = 1;
                    if (id > maxVertice) maxVertice = id;
                }
            }
        }
        else if (strchr(buffer, ';') && !strstr(buffer, "--")) {
            if (sscanf(buffer, " %d;", &id) == 1) {
                if (id >= 0 && id < MAX_VERTICES) {
                    vertices[id] = 1;
                    if (id > maxVertice) maxVertice = id;
                }
            }
        }
    }

    fclose(f);
    for (int i = 0; i <= maxVertice; i++) {
        if (vertices[i]) total++;
    }
    return total;
}

int contarArestas(char* nome) {
    char buffer[256];
    int arestas = 0;
    FILE* f = fopen(nome, "r");

    if (!f) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), f)) {
        if (strstr(buffer, "--")) {
            arestas++;
        }
    }

    fclose(f);
    return arestas;
}

void inicializarGrafo(Grafo* g) {
    for (int i = 0; i < MAX_VERTICES; i++) {
        for (int j = 0; j < MAX_VERTICES; j++) {
            g->adj[i][j] = 0;
            g->nome_are[i][j][0] = '\0';
        }
        g->nome_ver[i][0] = '\0';
    }
    g->n = 0;
}

void lerGrafo(Grafo* g, char* nome) {
    int u, v;
    char buffer[256];
    char label[MAX_LABEL_LENGTH];

    FILE* f = fopen(nome, "r");
    if (!f) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    inicializarGrafo(g);

    while (fgets(buffer, sizeof(buffer), f)) {
        // Captura nome dos vértices: formato esperado ' 0 [label="Nome"];'
        if (strstr(buffer, "[label=") && !strstr(buffer, "--")) {
            if (sscanf(buffer, " %d [label=\"%[^\"]\"]", &u, label) == 2) {
                if (u >= 0 && u < MAX_VERTICES) {
                    strncpy(g->nome_ver[u], label, MAX_LABEL_LENGTH - 1);
                    g->nome_ver[u][MAX_LABEL_LENGTH - 1] = '\0';
                    if (u >= g->n) g->n = u + 1;
                }
            }
        }
        // Captura arestas: formato esperado ' 0 -- 1;' ou ' 0 -- 1 [label="peso"];'
        else if (strstr(buffer, "--")) {
            int res = sscanf(buffer, " %d -- %d [label=\"%[^\"]\"]", &u, &v, label);
            if (res == 3) {
                if (u >= 0 && u < MAX_VERTICES && v >= 0 && v < MAX_VERTICES) {
                    strncpy(g->nome_are[u][v], label, MAX_LABEL_LENGTH - 1);
                    strncpy(g->nome_are[v][u], label, MAX_LABEL_LENGTH - 1);
                    g->nome_are[u][v][MAX_LABEL_LENGTH - 1] = '\0';
                    g->nome_are[v][u][MAX_LABEL_LENGTH - 1] = '\0';
                }
            } else if (sscanf(buffer, " %d -- %d", &u, &v) == 2) {
                if (u >= 0 && u < MAX_VERTICES && v >= 0 && v < MAX_VERTICES) {
                    g->nome_are[u][v][0] = '\0';
                    g->nome_are[v][u][0] = '\0';
                }
            }

            if (u >= 0 && u < MAX_VERTICES && v >= 0 && v < MAX_VERTICES) {
                g->adj[u][v] = 1;
                g->adj[v][u] = 1;

                if (u >= g->n) g->n = u + 1;
                if (v >= g->n) g->n = v + 1;
            }
        }
    }

    fclose(f);
}

int grauVertice(Grafo* g, int v) {
    int grau = 0;
    for (int i = 0; i < g->n; i++) {
        if (g->adj[v][i]) grau++;
    }
    return grau;
}

void verificarGrau(Grafo* g) {
    for (int v = 0; v < g->n; v++) {
        int grau = grauVertice(g, v);
        printf("Vertice: %s (id %d), Grau: %d\n", g->nome_ver[v], v, grau);
    }
}

void mostrarMaiorMenorGrau(Grafo* g) {
    if (g->n == 0) {
        printf("Grafo vazio!\n");
        return;
    }

    int maior = -1, menor = MAX_VERTICES + 1;
    int vMaior = -1, vMenor = -1;

    for (int i = 0; i < g->n; i++) {
        int grau = grauVertice(g, i);

        if (grau > maior) {
            maior = grau;
            vMaior = i;
        }
        if (grau < menor) {
            menor = grau;
            vMenor = i;
        }
    }

    if (vMaior != -1) {
        printf("Vertice com MAIOR grau: %s (id %d, grau %d)\n",
               g->nome_ver[vMaior], vMaior, maior);
    }
    if (vMenor != -1) {
        printf("Vertice com MENOR grau: %s (id %d, grau %d)\n",
               g->nome_ver[vMenor], vMenor, menor);
    }
}

void exemploCaminho(Grafo* g) {
    for (int u = 0; u < g->n; u++) {
        for (int v = 0; v < g->n; v++) {
            if (g->adj[u][v]) {
                for (int w = 0; w < g->n; w++) {
                    if (g->adj[v][w] && w != u) {
                        printf("Caminho Encontrado: %s -> %s -> %s\n",
                               g->nome_ver[u], g->nome_ver[v], g->nome_ver[w]);
                        return;
                    }
                }
            }
        }
    }
    printf("Nenhum caminho encontrado.\n");
}

void exemploSubgrafo(Grafo* g) {
    printf("Subgrafo Encontrado: ");
    int count = 0;
    for (int u = 0; u < g->n && count < 2; u++) {
        for (int v = u + 1; v < g->n && count < 2; v++) {
            if (g->adj[u][v]) {
                printf("{%s, %s} ", g->nome_ver[u], g->nome_ver[v]);
                count++;
            }
        }
    }
    if (count == 0) {
        printf("Nenhum subgrafo encontrado.\n");
    } else {
        printf("\n");
    }
}

int dfsCicloImprime(Grafo* g, int v, int visitado[], int pai[],int* cicloInicio, int* cicloFim) {
    visitado[v] = 1;

    for (int u = 0; u < g->n; u++) {
        if (g->adj[v][u]) {
            if (!visitado[u]) {
                pai[u] = v;
                if (dfsCicloImprime(g, u, visitado, pai, cicloInicio, cicloFim))
                    return 1;
            } else if (u != pai[v]) {
                *cicloInicio = u;
                *cicloFim = v;
                return 1;
            }
        }
    }
    return 0;
}

void exemploCiclo(Grafo* g) {
    int visitado[MAX_VERTICES] = {0};
    int pai[MAX_VERTICES];
    memset(pai, -1, sizeof(pai));  // Inicializa todos os pais com -1
    int cicloInicio = -1, cicloFim = -1;

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            pai[i] = -1;
            if (dfsCicloImprime(g, i, visitado, pai, &cicloInicio, &cicloFim)) {
                int ciclo[MAX_VERTICES], tam = 0;
                ciclo[tam++] = cicloInicio;

                int v = cicloFim;
                while (v != cicloInicio) {
                    ciclo[tam++] = v;
                    v = pai[v];
                }
                ciclo[tam++] = cicloInicio;

                printf("Ciclo Encontrado: ");
                for (int j = tam - 1; j >= 0; j--) {
                    printf("%s", g->nome_ver[ciclo[j]]);
                    if (j > 0) printf(" -> ");
                }
                printf("\n");
                return;
            }
        }
    }
    printf("Nenhum ciclo encontrado.\n");
}

void resumo_geral() {
    Grafo g;

    inicializarGrafo(&g);
    lerGrafo(&g,"grafo.dot");

    system("clear");
    linha_final();
    printf("][===------[-Resumo-Das-Informacoes-]------===][\n");
    linha_final();
    printf("Numero de Vertices: %d\n", contarVertices("grafo.dot"));
    printf("Numero de Arestas: %d \n", contarArestas("grafo.dot"));
    linha_final();
    verificarGrau(&g);
    linha_final();
    mostrarMaiorMenorGrau(&g);
    linha_final();
    exemploCaminho(&g);
    exemploSubgrafo(&g);
    exemploCiclo(&g);
    linha_final();
}

NodoAdj* criarNodoAdjacente(int destino) {
    NodoAdj* novo = (NodoAdj*)malloc(sizeof(NodoAdj));
    novo->destino = destino;
    novo->proximo = NULL;
    return novo;
}

void adicionarArestaGrafo(GrafoAdj* g, int origem, int destino) {
    NodoAdj* novo = criarNodoAdjacente(destino);
    novo->proximo = g->listaAdj[origem];
    g->listaAdj[origem] = novo;
}

void processarNomeVertice(char* linha, char nomes[MAX_VERTICES][100], int* contVertice) {
    int vertice;
    char nome[100];
    if (sscanf(linha, " %d [label=\"%[^\"]\"]", &vertice, nome) == 2) {
        strcpy(nomes[vertice], nome);
        (*contVertice)++;
    }
}

void processarArestas(char* linha, GrafoAdj* g) {
    int origem, destino;
    if (sscanf(linha, " %d -- %d;", &origem, &destino) == 2) {
        adicionarArestaGrafo(g, origem, destino);
        adicionarArestaGrafo(g, destino, origem);
    }
}

void construirGrafoADot(GrafoAdj* g, const char* arquivoDot) {
    FILE* arquivo = fopen(arquivoDot, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", arquivoDot);
        return;
    }

    char linha[256];
    int contVertice = 0;

    g->n = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        g->listaAdj[i] = NULL;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strstr(linha, "graph") != NULL || strstr(linha, "{") != NULL || strstr(linha, "}") != NULL)
            continue;

        processarNomeVertice(linha, g->nomes, &contVertice);
        processarArestas(linha, g);
    }

    g->n = contVertice;
    fclose(arquivo);
}

void exibirListaAdjacente(GrafoAdj* g) {
    for (int i = 0; i < g->n; i++) {
        printf("[%s]: ", g->nomes[i]);
        NodoAdj* nodoAtual = g->listaAdj[i];
        while (nodoAtual != NULL) {
            printf("%s ", g->nomes[nodoAtual->destino]);
            nodoAtual = nodoAtual->proximo;
        }
        printf("\n");
    }
}

void exibirListaDeAdjacencia() {
    GrafoAdj g;
    system("clear");
    construirGrafoADot(&g, "grafo.dot");
    linha_final();
    printf("][===-----[-Exibindo-Lista-Adjacente-]----===][\n");
    linha_final();
    exibirListaAdjacente(&g);
    linha_final();
}

void inicializarMatriz(GrafoMatriz* g) {
    for (int i = 0; i < MAX_VERTICES; i++) {
        for (int j = 0; j < MAX_VERTICES; j++) {
            g->adj[i][j] = 0;
        }
    }
}

void processarNomeVerticeMatriz(char* linha, char nomes[MAX_VERTICES][100], int* contVertice) {
    int vertice;
    char nome[100];
    if (sscanf(linha, " %d [label=\"%[^\"]\"]", &vertice, nome) == 2) {
        strcpy(nomes[vertice], nome);
        (*contVertice)++;
    }
}

void processarArestasMatriz(char* linha, GrafoMatriz* g) {
    int origem, destino;
    if (sscanf(linha, " %d -- %d;", &origem, &destino) == 2) {
        g->adj[origem][destino] = 1;
        g->adj[destino][origem] = 1;
    }
}

void construirGrafoMatrizDeDot(GrafoMatriz* g, const char* arquivoDot) {
    FILE* arquivo = fopen(arquivoDot, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", arquivoDot);
        return;
    }

    char linha[256];
    int contVertice = 0;

    g->n = 0;
    inicializarMatriz(g);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strstr(linha, "graph") != NULL || strstr(linha, "{") != NULL || strstr(linha, "}") != NULL)
            continue;

        processarNomeVerticeMatriz(linha, g->nomes, &contVertice);
        processarArestasMatriz(linha, g);
    }

    g->n = contVertice;
    fclose(arquivo);
}

void exibirMatrizDeAdjacencia(GrafoMatriz* g) {
    // Imprimir os índices das colunas (números dos vértices)
    printf("      ");
    for (int i = 0; i < g->n; i++) {
        printf("%3d ", i);  // Exibe o índice do vértice
    }
    printf("\n");

    // Imprimir as linhas da matriz
    for (int i = 0; i < g->n; i++) {
        printf("%3d: ", i);  // Exibe o índice do vértice na linha

        // Imprimir os valores da matriz
        for (int j = 0; j < g->n; j++) {
            printf("%3d ", g->adj[i][j]);  // Exibe o valor 1 ou 0
        }
        printf("\n");
    }
}

void exibirMatrizAdjacente() {
    GrafoMatriz g;
    system("clear");
    construirGrafoMatrizDeDot(&g, "grafo.dot");
    linha_final();
    printf("][===-----[-Exibindo-Matriz-Adjacente-]-----===][\n");
    linha_final();
    exibirMatrizDeAdjacencia(&g);
    linha_final();
}
// Matriz de Incidencia:

void lerGrafoIncidencia(GrafoIncidencia* g) {
    FILE* arquivo = fopen("grafo.dot", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo grafo.dot\n");
        exit(1);
    }

    char linha[256];
    g->n = 0;
    g->m = 0;

    // Zera a matriz
    for (int i = 0; i < MAX_VERTICES; i++)
        for (int j = 0; j < 200; j++)
            g->incid[i][j] = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strstr(linha, "--")) {
            int u, v;
            if (sscanf(linha, " %d -- %d", &u, &v) == 2) {
                g->incid[u][g->m] = 1;
                g->incid[v][g->m] = 1;
                g->m++;

                if (u >= g->n) g->n = u + 1;
                if (v >= g->n) g->n = v + 1;
            }
        }
    }

    fclose(arquivo);
}

void imprimirMatrizIncidencia(GrafoIncidencia* g) {
    printf("Matriz de Incidencia (%d vertices, %d arestas):\n", g->n, g->m);

    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->m; j++) {
            printf("%d ", g->incid[i][j]);
        }
        printf("\n");
    }
}

void exibirMatrizIncidencia() {
    GrafoIncidencia g;
    system("clear");
    linha_final();
    printf("][===----[-Exibindo-Matriz-Incidencia-]----===][\n");
    linha_final();
    lerGrafoIncidencia(&g);
    imprimirMatrizIncidencia(&g);
    linha_final();
}

//Função que busca Vértice se achar retorna true se não retorna falso:
int buscarVertice(const char* nomeVertice) {
    FILE* arquivo = fopen("grafo.dot", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo grafo.dot\n");
        exit(1);
    }

    char linha[256];
    int vertice = -1;
    while (fgets(linha, sizeof(linha), arquivo)) {
        // Procura pelo padrão de nome do vértice
        if (strstr(linha, "[label=")) {
            char label[256];
            if (sscanf(linha, " %d [label=\"%[^\"]\"];", &vertice, label) == 2) {
                // Verifica se o nome do vértice corresponde ao nome procurado
                if (strcmp(nomeVertice, label) == 0) {
                    fclose(arquivo);
                    return vertice;  // Vértice encontrado, retorna o índice
                }
            }
        }
    }

    fclose(arquivo);
    return -1;  // Vértice não encontrado
}

void limparBuffer() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);  // Continua lendo até a quebra de linha ou final de arquivo
}

void buscando() {
    char nome[256];
    system("clear");
    printf("Digite o nome do vertice que deseja buscar: ");

    // Limpa o buffer antes de usar fgets para evitar problemas
    limparBuffer();

    // Usando fgets para ler a entrada, garantindo que caracteres especiais sejam tratados corretamente
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';  // Remove a quebra de linha no final da string

    linha_final();

    int vertice = buscarVertice(nome);
    if (vertice != -1) {
        printf("Vertice '%s' encontrado com indice: %d\n", nome, vertice);
    } else {
        printf("Vertice '%s' nao encontrado.\n", nome);
    }
    linha_final();
}

bool contem(int* conjunto, int tam, int valor) {
    for (int i = 0; i < tam; i++) {
        if (conjunto[i] == valor)
            return true;
    }
    return false;
}

bool dfsCiclo(GrafoMatriz* g, int v, bool* visitado, int pai, int* subconjunto, int tam) {
    visitado[v] = true;

    for (int i = 0; i < g->n; i++) {
        if (g->adj[v][i] == 1 && contem(subconjunto, tam, i)) {
            if (!visitado[i]) {
                if (dfsCiclo(g, i, visitado, v, subconjunto, tam))
                    return true;
            } else if (i != pai) {
                return true;  // ciclo detectado
            }
        }
    }
    return false;
}

void verificarCicloConjunto() {
    GrafoMatriz g;
    construirGrafoMatrizDeDot(&g, "grafo.dot");
    int subconjunto[MAX_VERTICES];
    int tam;
    char nome[256];

    system("clear");
    linha_final();
    printf("][===--------[-Verificando-Ciclos-]--------===][\n");
    linha_final();
    printf("Digite a quantidade de vertices no conjunto: ");
    scanf("%d", &tam);
    limparBuffer();  // Importante para usar fgets depois

    if (tam <= 1) {
        printf("Conjunto muito pequeno para conter ciclo.\n");
        return;
    }

    linha_final();
    printf("Digite os nomes dos vertices do conjunto:\n");
    for (int i = 0; i < tam; i++) {
        printf("Vertice %d: ", i + 1);
        fgets(nome, sizeof(nome), stdin);
        nome[strcspn(nome, "\n")] = '\0';  // Remove \n do final

        int indice = buscarVertice(nome);
        if (indice == -1) {
            printf("Vertice '%s' nao encontrado no grafo\n", nome);
            linha_final();
            return;
        }
        subconjunto[i] = indice;
    }

    bool visitado[MAX_VERTICES] = {false};

    for (int i = 0; i < tam; i++) {
        int v = subconjunto[i];
        if (!visitado[v]) {
            if (dfsCiclo(&g, v, visitado, -1, subconjunto, tam)) {
                printf("Existe um ciclo no subconjunto informado.\n");
                linha_final();
                getchar();getchar();
                return;
            }
        }
    }

    linha_final();
    printf("Nao existe ciclo no subconjunto informado.\n");
    linha_final();
    getchar();getchar();
}

// Função para imprimir matriz de adjacência
void imprimirMatrizAdj(Grafo* g) {
    printf("Matriz de Adjacencia (%d vertices):\n", g->n);
    printf("    ");
    for (int i = 0; i < g->n; i++) printf("%3d ", i);
    printf("\n");

    for (int i = 0; i < g->n; i++) {
        printf("%3d ", i);
        for (int j = 0; j < g->n; j++) {
            printf(" %2d ", g->adj[i][j]);
        }
        printf("\n");
    }
}

// Função para inicializar lista de adjacência
void inicializarGrafoAdj(GrafoAdj* ga, int n) {
    ga->n = n;
    for (int i = 0; i < n; i++) {
        ga->listaAdj[i] = NULL;
    }
}

// Função para adicionar aresta na lista de adjacência (não direcionada)
void adicionarArestaLista(GrafoAdj* ga, int u, int v) {
    NodoAdj* novo = malloc(sizeof(NodoAdj));
    novo->destino = v;
    novo->proximo = ga->listaAdj[u];
    ga->listaAdj[u] = novo;

    // Como grafo é não direcionado, adiciona a aresta contrária
    novo = malloc(sizeof(NodoAdj));
    novo->destino = u;
    novo->proximo = ga->listaAdj[v];
    ga->listaAdj[v] = novo;
}

// Função para imprimir lista de adjacência
void imprimirListaAdj(GrafoAdj* ga) {
    printf("Lista de Adjacencia (%d vertices):\n", ga->n);
    for (int i = 0; i < ga->n; i++) {
        printf("%d:", i);
        NodoAdj* p = ga->listaAdj[i];
        while (p != NULL) {
            printf(" -> %d", p->destino);
            p = p->proximo;
        }
        printf("\n");
    }
}

// Função para inicializar matriz de incidência
void inicializarGrafoIncidencia(GrafoIncidencia* gi, int n, int m) {
    gi->n = n;
    gi->m = m;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            gi->incid[i][j] = 0;
}

// Função para imprimir matriz de incidência
void imprimirMatrizIncidenci(GrafoIncidencia* gi) {
    printf("Matriz de Incidencia (n=%d, m=%d):\n", gi->n, gi->m);
    printf("     ");
    for (int j = 0; j < gi->m; j++) printf("%3d ", j);
    printf("\n");
    for (int i = 0; i < gi->n; i++) {
        printf("%3d: ", i);
        for (int j = 0; j < gi->m; j++) {
            printf(" %2d ", gi->incid[i][j]);
        }
        printf("\n");
    }
}

// Transformação 1: Matriz de adjacência -> Lista de adjacência
void matrizParaLista(Grafo* gm, GrafoAdj* ga) {
    inicializarGrafoAdj(ga, gm->n);
    for (int i = 0; i < gm->n; i++) {
        for (int j = 0; j < gm->n; j++) {
            if (gm->adj[i][j] != 0) {
                // Adiciona aresta i-j (verifica para não duplicar)
                if (i < j) {
                    adicionarArestaLista(ga, i, j);
                }
            }
        }
    }
}

// Transformação 2: Matriz de adjacência -> Matriz de incidência
void matrizParaIncidencia(Grafo* gm, GrafoIncidencia* gi) {
    int m = 0;
    // Conta número de arestas
    for (int i = 0; i < gm->n; i++) {
        for (int j = i + 1; j < gm->n; j++) {
            if (gm->adj[i][j] != 0) {
                m++;
            }
        }
    }
    inicializarGrafoIncidencia(gi, gm->n, m);

    int aresta_index = 0;
    for (int i = 0; i < gm->n; i++) {
        for (int j = i + 1; j < gm->n; j++) {
            if (gm->adj[i][j] != 0) {
                gi->incid[i][aresta_index] = 1;
                gi->incid[j][aresta_index] = 1;
                aresta_index++;
            }
        }
    }
}

// Transformação 3: Lista de adjacência -> Matriz de adjacência
void listaParaMatriz(GrafoAdj* ga, Grafo* gm) {
    inicializarGrafo(gm);
    gm->n = ga->n;

    for (int i = 0; i < ga->n; i++) {
        NodoAdj* p = ga->listaAdj[i];
        while (p != NULL) {
            gm->adj[i][p->destino] = 1;
            p = p->proximo;
        }
    }
}

// Transformação 4: Lista de adjacência -> Matriz de incidência
void listaParaIncidencia(GrafoAdj* ga, GrafoIncidencia* gi) {
    int m = 0;
    // Conta arestas (considerando não duplicar)
    int aux[MAX_VERTICES][MAX_VERTICES] = {{0}};
    for (int i = 0; i < ga->n; i++) {
        NodoAdj* p = ga->listaAdj[i];
        while (p != NULL) {
            int u = i;
            int v = p->destino;
            if (u < v && aux[u][v] == 0) {
                aux[u][v] = 1;
                m++;
            }
            p = p->proximo;
        }
    }

    inicializarGrafoIncidencia(gi, ga->n, m);

    int aresta_index = 0;
    for (int i = 0; i < ga->n; i++) {
        NodoAdj* p = ga->listaAdj[i];
        while (p != NULL) {
            int u = i;
            int v = p->destino;
            if (u < v && gi->incid[u][aresta_index] == 0) {
                gi->incid[u][aresta_index] = 1;
                gi->incid[v][aresta_index] = 1;
                aresta_index++;
            }
            p = p->proximo;
        }
    }
}

// Transformação 5: Matriz de incidência -> Matriz de adjacência
void incidenciaParaMatriz(GrafoIncidencia* gi, Grafo* gm) {
    inicializarGrafo(gm);
    gm->n = gi->n;

    for (int j = 0; j < gi->m; j++) {
        int vertices_conectados[2];
        int count = 0;
        for (int i = 0; i < gi->n; i++) {
            if (gi->incid[i][j] == 1) {
                vertices_conectados[count++] = i;
                if (count == 2) break;
            }
        }
        if (count == 2) {
            int u = vertices_conectados[0];
            int v = vertices_conectados[1];
            gm->adj[u][v] = 1;
            gm->adj[v][u] = 1;
        }
    }
}

// Transformação 6: Matriz de incidência -> Lista de adjacência
void incidenciaParaLista(GrafoIncidencia* gi, GrafoAdj* ga) {
    int m = gi->m;
    int n = gi->n;

    inicializarGrafoAdj(ga, n);

    for (int j = 0; j < m; j++) {
        int vertices_conectados[2];
        int count = 0;

        for (int i = 0; i < n; i++) {
            if (gi->incid[i][j] == 1) {
                vertices_conectados[count++] = i;
                if (count == 2) break;
            }
        }
        if (count == 2) {
            int u = vertices_conectados[0];
            int v = vertices_conectados[1];

            // Adiciona aresta u-v
            NodoAdj* novo = malloc(sizeof(NodoAdj));
            novo->destino = v;
            novo->proximo = ga->listaAdj[u];
            ga->listaAdj[u] = novo;

            novo = malloc(sizeof(NodoAdj));
            novo->destino = u;
            novo->proximo = ga->listaAdj[v];
            ga->listaAdj[v] = novo;
        }
    }
}

// Função auxiliar para comparar inteiros (para qsort)
int compararInteiros(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Função para calcular graus dos vértices
void calcularGraus(Grafo *g, int *graus) {
    for(int i = 0; i < g->n; i++) {
        graus[i] = 0;
        for(int j = 0; j < g->n; j++) {
            graus[i] += g->adj[i][j];
        }
    }
}

// Função recursiva de backtracking para isomorfismo
int isomorfismoBacktrack(Grafo *g1, Grafo *g2, int *mapeamento, int *usado, int k) {
    if(k == g1->n) {
        // Verificar todas as adjacências
        for(int i = 0; i < g1->n; i++) {
            for(int j = 0; j < g1->n; j++) {
                if(g1->adj[i][j] != g2->adj[mapeamento[i]][mapeamento[j]]) {
                    return 0;
                }
            }
        }
        return 1;
    }

    // Tentar mapear vértice k para todos os vértices não usados
    for(int v = 0; v < g2->n; v++) {
        if(!usado[v]) {

            int grau_g1 = 0, grau_g2 = 0;
            for(int i = 0; i < g1->n; i++) {
                grau_g1 += g1->adj[k][i];
                grau_g2 += g2->adj[v][i];
            }
            if(grau_g1 != grau_g2) continue;

            mapeamento[k] = v;
            usado[v] = 1;

            if(isomorfismoBacktrack(g1, g2, mapeamento, usado, k + 1)) {
                return 1;
            }

            usado[v] = 0;
        }
    }
    return 0;
}

bool verificaIsomorfismo() {
    Grafo g1, g2;
    char nomeGrafo2[100];
    int mapeamento[MAX_VERTICES] = {0};
    int usado[MAX_VERTICES] = {0};

    system("clear");
    printf("Digite o nome do segundo grafo (ex: outro.dot): ");
    scanf("%99s", nomeGrafo2);

    // Carregar grafos
    lerGrafo(&g1, "grafo.dot");
    lerGrafo(&g2, nomeGrafo2);

    printf("Realizando a verificacao: \n");


    if(g1.n != g2.n) return false;
    if(contarArestas("grafo.dot") != contarArestas(nomeGrafo2)) return false;

    int graus1[MAX_VERTICES], graus2[MAX_VERTICES];
    calcularGraus(&g1, graus1);
    calcularGraus(&g2, graus2);

    qsort(graus1, g1.n, sizeof(int), compararInteiros);
    qsort(graus2, g2.n, sizeof(int), compararInteiros);

    if(memcmp(graus1, graus2, g1.n * sizeof(int)) != 0) {
        return false;
    }

    // Verificar isomorfismo com backtracking
    return isomorfismoBacktrack(&g1, &g2, mapeamento, usado, 0);
}

bool verificaSubgrafo(Grafo *G_asterisco, Grafo *G, bool *disjuncaoAresta, bool *disjuncaoVertice) {
    *disjuncaoAresta = true;
    *disjuncaoVertice = true;

    // Verificação de disjunção em vértices (por nome)
    for (int i = 0; i < G_asterisco->n; i++) {
        for (int j = 0; j < G->n; j++) {
            if (strcmp(G_asterisco->nome_ver[i], G->nome_ver[j]) == 0) {
                *disjuncaoVertice = false;
                break;
            }
        }
        if (!(*disjuncaoVertice)) break;
    }

    // Verificação de disjunção em arestas (por nome dos vértices)
    for (int i = 0; i < G_asterisco->n; i++) {
        for (int j = 0; j < G_asterisco->n; j++) {
            if (G_asterisco->adj[i][j]) {
                int u_g = -1, v_g = -1;
                // Encontra os índices correspondentes em G
                for (int k = 0; k < G->n; k++) {
                    if (strcmp(G_asterisco->nome_ver[i], G->nome_ver[k]) == 0) u_g = k;
                    if (strcmp(G_asterisco->nome_ver[j], G->nome_ver[k]) == 0) v_g = k;
                }
                // Se a aresta existe em G, marca como não disjunta
                if (u_g != -1 && v_g != -1 && G->adj[u_g][v_g]) {
                    *disjuncaoAresta = false;
                    break;
                }
            }
        }
        if (!(*disjuncaoAresta)) break;
    }

    // Verificação de subgrafo (por nome)
    // 1. Todos os vértices de G_asterisco devem existir em G
    for (int i = 0; i < G_asterisco->n; i++) {
        bool encontrado = false;
        for (int j = 0; j < G->n; j++) {
            if (strcmp(G_asterisco->nome_ver[i], G->nome_ver[j]) == 0) {
                encontrado = true;
                break;
            }
        }
        if (!encontrado) return false;
    }

    // 2. Todas as arestas de G_asterisco devem existir em G
    for (int i = 0; i < G_asterisco->n; i++) {
        for (int j = 0; j < G_asterisco->n; j++) {
            if (G_asterisco->adj[i][j]) {
                int u_g = -1, v_g = -1;
                for (int k = 0; k < G->n; k++) {
                    if (strcmp(G_asterisco->nome_ver[i], G->nome_ver[k]) == 0) u_g = k;
                    if (strcmp(G_asterisco->nome_ver[j], G->nome_ver[k]) == 0) v_g = k;
                }
                if (u_g == -1 || v_g == -1 || !G->adj[u_g][v_g]) {
                    return false;
                }
            }
        }
    }

    return true;
}

void menuVerificacaoSubgrafo() {
    Grafo G, G_asterisco;
    char nomeArquivoG[100], nomeArquivoGAsterisco[100];
    bool disjuncaoAresta, disjuncaoVertice;

    system("clear");
    printf("Digite o nome do grafo original G: ");
    scanf("%99s", nomeArquivoG);
    printf("Digite o nome do grafo candidato a subgrafo: ");
    scanf("%99s", nomeArquivoGAsterisco);

    lerGrafo(&G, nomeArquivoG);
    lerGrafo(&G_asterisco, nomeArquivoGAsterisco);

    bool ehSubgrafo = verificaSubgrafo(&G_asterisco, &G, &disjuncaoAresta, &disjuncaoVertice);
    printf("\nDisjuncao em arestas: %s\n", disjuncaoAresta ? "Sim" : "Nao");
    printf("Disjuncao em vertices: %s\n", disjuncaoVertice ? "Sim" : "Nao");
    printf(ehSubgrafo ? "\nO grafo e subgrafo.\n" : "\nNao e subgrafo.\n");
}

// Procura o índice do vértice com determinado nome em um grafo
int encontrarIndicePorNome(Grafo* g, const char* nome) {
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->nome_ver[i], nome) == 0) {
            return i;
        }
    }
    return -1; // Não encontrado
}

void uniaoGrafos(Grafo* g1, Grafo* g2, Grafo* g_union) {
    inicializarGrafo(g_union);

    // Adiciona vértices de g1
    for (int i = 0; i < g1->n; i++) {
        strncpy(g_union->nome_ver[g_union->n], g1->nome_ver[i], MAX_LABEL_LENGTH - 1);
        g_union->nome_ver[g_union->n][MAX_LABEL_LENGTH - 1] = '\0';
        g_union->n++;
    }

    // Adiciona vértices de g2 que ainda não estão na união
    for (int i = 0; i < g2->n; i++) {
        if (encontrarIndicePorNome(g_union, g2->nome_ver[i]) == -1) {
            strncpy(g_union->nome_ver[g_union->n], g2->nome_ver[i], MAX_LABEL_LENGTH - 1);
            g_union->nome_ver[g_union->n][MAX_LABEL_LENGTH - 1] = '\0';
            g_union->n++;
        }
    }

    // Adiciona arestas de g1
    for (int i = 0; i < g1->n; i++) {
        for (int j = 0; j < g1->n; j++) {
            if (g1->adj[i][j]) {
                int u = encontrarIndicePorNome(g_union, g1->nome_ver[i]);
                int v = encontrarIndicePorNome(g_union, g1->nome_ver[j]);
                if (u != -1 && v != -1) {
                    g_union->adj[u][v] = 1;
                    strncpy(g_union->nome_are[u][v], g1->nome_are[i][j], MAX_LABEL_LENGTH - 1);
                    g_union->nome_are[u][v][MAX_LABEL_LENGTH - 1] = '\0';
                }
            }
        }
    }

    // Adiciona arestas de g2
    for (int i = 0; i < g2->n; i++) {
        for (int j = 0; j < g2->n; j++) {
            if (g2->adj[i][j]) {
                int u = encontrarIndicePorNome(g_union, g2->nome_ver[i]);
                int v = encontrarIndicePorNome(g_union, g2->nome_ver[j]);
                if (u != -1 && v != -1) {
                    g_union->adj[u][v] = 1;
                    // Se quiser sobrescrever sempre o nome da aresta, use:
                    strncpy(g_union->nome_are[u][v], g2->nome_are[i][j], MAX_LABEL_LENGTH - 1);
                    g_union->nome_are[u][v][MAX_LABEL_LENGTH - 1] = '\0';
                }
            }
        }
    }
}

void uniao_grafos() {
    Grafo G, G_asterisco, G_union;
    char nomeArquivoG[100], nomeArquivoGAsterisco[100];

    system("clear");
    printf("Digite o nome do grafo original G: ");
    scanf("%99s", nomeArquivoG);
    printf("Digite o nome do grafo candidato a subgrafo: ");
    scanf("%99s", nomeArquivoGAsterisco);

    system("clear");
    lerGrafo(&G, nomeArquivoG);
    lerGrafo(&G_asterisco, nomeArquivoGAsterisco);

    // Faz a união
    uniaoGrafos(&G, &G_asterisco, &G_union);

    printf("Caracteristicas GRAFO-1\n");
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoG));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoG));

    printf("Caracteristicas GRAFO-2\n");
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoGAsterisco));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoGAsterisco));

    printf("\nGRAFO-UNIAO\n");
    imprimirMatrizAdj(&G_union);
    getchar();getchar();
}

void intersecaoGrafos(Grafo* g1, Grafo* g2, Grafo* g_intersecao) {
    int i, j;

    inicializarGrafo(g_intersecao);

    // Considera todos os vértices de g1
    for (i = 0; i < g1->n; i++) {
        // Procura se o vértice também existe em g2 (comparando nomes)
        int existeEmG2 = 0;
        for (j = 0; j < g2->n; j++) {
            if (strcmp(g1->nome_ver[i], g2->nome_ver[j]) == 0) {
                existeEmG2 = 1;
                break;
            }
        }
        if (existeEmG2) {
            // Adiciona o vértice à interseção
            strncpy(g_intersecao->nome_ver[g_intersecao->n], g1->nome_ver[i], MAX_LABEL_LENGTH - 1);
            g_intersecao->nome_ver[g_intersecao->n][MAX_LABEL_LENGTH - 1] = '\0';
            g_intersecao->n++;
        }
    }

    // Agora as arestas: se existe em ambos e vértices existem na interseção
    for (i = 0; i < g1->n; i++) {
        for (j = 0; j < g1->n; j++) {
            if (g1->adj[i][j]) {
                // Descobre se vértices existem na interseção
                int idx_i = -1, idx_j = -1;
                for (int k = 0; k < g_intersecao->n; k++) {
                    if (strcmp(g1->nome_ver[i], g_intersecao->nome_ver[k]) == 0) idx_i = k;
                    if (strcmp(g1->nome_ver[j], g_intersecao->nome_ver[k]) == 0) idx_j = k;
                }
                // Se ambos existem na interseção e aresta existe também em g2
                if (idx_i != -1 && idx_j != -1 && g2->adj[i][j]) {
                    g_intersecao->adj[idx_i][idx_j] = 1;
                    strncpy(g_intersecao->nome_are[idx_i][idx_j], g1->nome_are[i][j], MAX_LABEL_LENGTH - 1);
                    g_intersecao->nome_are[idx_i][idx_j][MAX_LABEL_LENGTH - 1] = '\0';
                }
            }
        }
    }
}

void executarIntersecao() {
    Grafo G, G_asterisco, G_intersecao;
    char nomeArquivoG[100], nomeArquivoGAsterisco[100];

    system("clear");
    printf("Digite o nome do grafo original G: ");
    scanf("%99s", nomeArquivoG);
    printf("Digite o nome do grafo candidato a subgrafo: ");
    scanf("%99s", nomeArquivoGAsterisco);

    system("clear");

    // Carrega os grafos dos arquivos
    lerGrafo(&G, nomeArquivoG);
    lerGrafo(&G_asterisco, nomeArquivoGAsterisco);

    // Faz a interseção
    intersecaoGrafos(&G, &G_asterisco, &G_intersecao);

    // Exibe características
    printf("Caracteristicas GRAFO-1\n");
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoG));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoG));

    printf("Caracteristicas GRAFO-2\n");
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoGAsterisco));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoGAsterisco));

    printf("\nGRAFO-INTERSECAO\n");
    imprimirMatrizAdj(&G_intersecao);

    getchar();getchar();
}

void diferencaSimetricaGrafos(Grafo* g1, Grafo* g2, Grafo* g_dif) {
    int i, j, k, l;

    inicializarGrafo(g_dif);

    // Adiciona vértices exclusivos de g1
    for (i = 0; i < g1->n; i++) {
        int existeEmG2 = 0;
        for (j = 0; j < g2->n; j++) {
            if (strcmp(g1->nome_ver[i], g2->nome_ver[j]) == 0) {
                existeEmG2 = 1;
                break;
            }
        }
        if (!existeEmG2) {
            strncpy(g_dif->nome_ver[g_dif->n], g1->nome_ver[i], MAX_LABEL_LENGTH - 1);
            g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
            g_dif->n++;
        }
    }

    // Adiciona vértices exclusivos de g2
    for (i = 0; i < g2->n; i++) {
        int existeEmG1 = 0;
        for (j = 0; j < g1->n; j++) {
            if (strcmp(g2->nome_ver[i], g1->nome_ver[j]) == 0) {
                existeEmG1 = 1;
                break;
            }
        }
        if (!existeEmG1) {
            strncpy(g_dif->nome_ver[g_dif->n], g2->nome_ver[i], MAX_LABEL_LENGTH - 1);
            g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
            g_dif->n++;
        }
    }

    // Adiciona arestas exclusivas de g1
    for (i = 0; i < g1->n; i++) {
        for (j = 0; j < g1->n; j++) {
            if (g1->adj[i][j]) {
                int existeEmG2 = 0;
                for (k = 0; k < g2->n; k++) {
                    if (strcmp(g1->nome_ver[i], g2->nome_ver[k]) == 0) {
                        for (l = 0; l < g2->n; l++) {
                            if (strcmp(g1->nome_ver[j], g2->nome_ver[l]) == 0 && g2->adj[k][l]) {
                                existeEmG2 = 1;
                                break;
                            }
                        }
                        if (existeEmG2) break;
                    }
                }
                if (!existeEmG2) {
                    // Verifica ou adiciona os vértices no grafo de diferença
                    int idx_i = -1, idx_j = -1;
                    for (k = 0; k < g_dif->n; k++) {
                        if (strcmp(g_dif->nome_ver[k], g1->nome_ver[i]) == 0) idx_i = k;
                        if (strcmp(g_dif->nome_ver[k], g1->nome_ver[j]) == 0) idx_j = k;
                    }
                    if (idx_i == -1) {
                        idx_i = g_dif->n;
                        strncpy(g_dif->nome_ver[g_dif->n], g1->nome_ver[i], MAX_LABEL_LENGTH - 1);
                        g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
                        g_dif->n++;
                    }
                    if (idx_j == -1) {
                        idx_j = g_dif->n;
                        strncpy(g_dif->nome_ver[g_dif->n], g1->nome_ver[j], MAX_LABEL_LENGTH - 1);
                        g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
                        g_dif->n++;
                    }
                    g_dif->adj[idx_i][idx_j] = 1;
                    strncpy(g_dif->nome_are[idx_i][idx_j], g1->nome_are[i][j], MAX_LABEL_LENGTH - 1);
                    g_dif->nome_are[idx_i][idx_j][MAX_LABEL_LENGTH - 1] = '\0';
                }
            }
        }
    }

    // Adiciona arestas exclusivas de g2
    for (i = 0; i < g2->n; i++) {
        for (j = 0; j < g2->n; j++) {
            if (g2->adj[i][j]) {
                int existeEmG1 = 0;
                for (k = 0; k < g1->n; k++) {
                    if (strcmp(g2->nome_ver[i], g1->nome_ver[k]) == 0) {
                        for (l = 0; l < g1->n; l++) {
                            if (strcmp(g2->nome_ver[j], g1->nome_ver[l]) == 0 && g1->adj[k][l]) {
                                existeEmG1 = 1;
                                break;
                            }
                        }
                        if (existeEmG1) break;
                    }
                }
                if (!existeEmG1) {
                    int idx_i = -1, idx_j = -1;
                    for (k = 0; k < g_dif->n; k++) {
                        if (strcmp(g_dif->nome_ver[k], g2->nome_ver[i]) == 0) idx_i = k;
                        if (strcmp(g_dif->nome_ver[k], g2->nome_ver[j]) == 0) idx_j = k;
                    }
                    if (idx_i == -1) {
                        idx_i = g_dif->n;
                        strncpy(g_dif->nome_ver[g_dif->n], g2->nome_ver[i], MAX_LABEL_LENGTH - 1);
                        g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
                        g_dif->n++;
                    }
                    if (idx_j == -1) {
                        idx_j = g_dif->n;
                        strncpy(g_dif->nome_ver[g_dif->n], g2->nome_ver[j], MAX_LABEL_LENGTH - 1);
                        g_dif->nome_ver[g_dif->n][MAX_LABEL_LENGTH - 1] = '\0';
                        g_dif->n++;
                    }
                    g_dif->adj[idx_i][idx_j] = 1;
                    strncpy(g_dif->nome_are[idx_i][idx_j], g2->nome_are[i][j], MAX_LABEL_LENGTH - 1);
                    g_dif->nome_are[idx_i][idx_j][MAX_LABEL_LENGTH - 1] = '\0';
                }
            }
        }
    }
}

void imprimirGrafoDOT(Grafo* g) {
    int i, j;
    printf("graph {\n");

    // imprime os vértices com índice e label
    for (i = 0; i < g->n; i++) {
        printf("  %d [label=\"%s\"];\n", i, g->nome_ver[i]);
    }
    printf("\n");

    // imprime as arestas (apenas uma vez para cada par, já que é não direcionado)
    for (i = 0; i < g->n; i++) {
        for (j = i + 1; j < g->n; j++) {  // j começa em i+1 para evitar duplicatas
            if (g->adj[i][j]) {
                printf("  %d -- %d [label=\"%s\"];\n", i, j, g->nome_are[i][j]);
            }
        }
    }

    printf("}\n");
}

void executarDiferencaSimetrica() {
    Grafo G, G_asterisco, G_diferenca;
    char nomeArquivoG[100], nomeArquivoGAsterisco[100];

    system("clear");
    printf("Digite o nome do grafo original G: ");
    scanf("%99s", nomeArquivoG);
    printf("Digite o nome do grafo candidato a subgrafo: ");
    scanf("%99s", nomeArquivoGAsterisco);

    system("clear");

    lerGrafo(&G, nomeArquivoG);
    lerGrafo(&G_asterisco, nomeArquivoGAsterisco);

    // Calcula a diferença simétrica
    diferencaSimetricaGrafos(&G, &G_asterisco, &G_diferenca);

    linha_final();
    printf("Caracteristicas GRAFO-1\n");
    linha_final();
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoG));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoG));
    imprimirGrafoDOT(&G);

    linha_final();
    printf("Caracteristicas GRAFO-2\n");
    linha_final();
    printf("Numero de Vertices: %d\n", contarVertices(nomeArquivoGAsterisco));
    printf("Numero de Arestas: %d \n", contarArestas(nomeArquivoGAsterisco));
    imprimirGrafoDOT(&G_asterisco);

    printf("\nGRAFO-DIFERENCA SIMETRICA\n");
    imprimirGrafoDOT(&G_diferenca);

    getchar();getchar();
}

void removerVertice(Grafo* G, int vi, Grafo* G2) {
    int i, j;
    int novoIndice = 0;

    // G2 terá um vértice a menos
    G2->n = G->n - 1;

    // Copiar nomes dos vértices, pulando o vi
    for (i = 0; i < G->n; i++) {
        if (i == vi) continue;
        strcpy(G2->nome_ver[novoIndice], G->nome_ver[i]);
        novoIndice++;
    }

    // Copiar matriz de adjacência e nomes das arestas, pulando linha e coluna vi
    int linhaNova, colunaNova;
    linhaNova = 0;
    for (i = 0; i < G->n; i++) {
        if (i == vi) continue;
        colunaNova = 0;
        for (j = 0; j < G->n; j++) {
            if (j == vi) continue;

            G2->adj[linhaNova][colunaNova] = G->adj[i][j];
            strcpy(G2->nome_are[linhaNova][colunaNova], G->nome_are[i][j]);

            colunaNova++;
        }
        linhaNova++;
    }
}

void executarRemoverVertice() {
    Grafo G, G2;
    char nomeArquivoG[100];
    int vi;

    system("clear");
    printf("Digite o nome do grafo original G1: ");
    scanf("%99s", nomeArquivoG);

    lerGrafo(&G, nomeArquivoG);

    printf("Digite o indice do vertice a ser removido: ");
    scanf("%d", &vi);

    if (vi < 0 || vi >= G.n) {
        printf("Vertice %d nao existe no grafo G1.\n", vi);
        system("pause");
        return;
    }

    // Função que cria G2 sem o vértice vi
    removerVertice(&G, vi, &G2);

    printf("\nGRAFO SEM O VERTICE %d\n", vi);
    imprimirGrafoDOT(&G2);

    getchar();getchar();
}

void removerAresta(Grafo* G, int v1, int v2, Grafo* G2) {
    int i, j;

    // G2 terá o mesmo número de vértices que G
    G2->n = G->n;

    // Copiar nomes dos vértices
    for (i = 0; i < G->n; i++) {
        strcpy(G2->nome_ver[i], G->nome_ver[i]);
    }

    // Copiar matriz de adjacência e nomes das arestas
    for (i = 0; i < G->n; i++) {
        for (j = 0; j < G->n; j++) {
            G2->adj[i][j] = G->adj[i][j];
            strcpy(G2->nome_are[i][j], G->nome_are[i][j]);
        }
    }

    // Remover a aresta v1 -- v2
    G2->adj[v1][v2] = 0;
    G2->adj[v2][v1] = 0;

    // Limpar o nome da aresta removida (opcional)
    strcpy(G2->nome_are[v1][v2], "");
    strcpy(G2->nome_are[v2][v1], "");
}

int buscarIndiceVertice(Grafo* G, char* nome) {
    for (int i = 0; i < G->n; i++) {
        if (strcmp(G->nome_ver[i], nome) == 0) {
            return i;
        }
    }
    return -1; // não encontrou
}

void executarRemoverAresta() {
    Grafo G, G2;
    char nomeArquivoG[100];
    char nomeV1[50], nomeV2[50];
    int v1, v2;

    system("clear");
    printf("Digite o nome do grafo original G1: ");
    scanf("%99s", nomeArquivoG);

    lerGrafo(&G, nomeArquivoG);

    printf("Digite os nomes dos dois vertices da aresta a ser removida:\n");
    printf("Vertice 1: ");
    scanf("%49s", nomeV1);
    printf("Vertice 2: ");
    scanf("%49s", nomeV2);

    v1 = buscarIndiceVertice(&G, nomeV1);
    v2 = buscarIndiceVertice(&G, nomeV2);

    if (v1 == -1 || v2 == -1) {
        printf("Um dos vertices nao foi encontrado no grafo.\n");
        getchar();getchar();
        return;
    }

    if (!G.adj[v1][v2]) {
        printf("Aresta (%s -- %s) nao existe no grafo G1.\n", nomeV1, nomeV2);
        getchar();getchar();
        return;
    }

    // Função que cria G2 sem a aresta (v1, v2)
    removerAresta(&G, v1, v2, &G2);

    printf("\nGRAFO SEM A ARESTA %s -- %s\n", nomeV1, nomeV2);
    imprimirGrafoDOT(&G2);

    getchar();getchar();
}

int buscarIndiceVerticePorNome(Grafo* G, const char* nome) {
    for (int i = 0; i < G->n; i++) {
        if (strcmp(G->nome_ver[i], nome) == 0) {
            return i;
        }
    }
    return -1; // não encontrado
}

int remapIndice(int x, int vj) {
    return (x < vj) ? x : x - 1;
}

void fundirVertices(Grafo* G, const char* nome_vi, const char* nome_vj, Grafo* G2) {
    int i, j;
    int vi = buscarIndiceVerticePorNome(G, nome_vi);
    int vj = buscarIndiceVerticePorNome(G, nome_vj);

    if (vi == -1 || vj == -1) {
        printf("Um ou ambos os vertices nao existem no grafo.\n");
        G2->n = 0; // para indicar erro
        return;
    }
    if (vi == vj) {
        printf("Os vertices sao iguais. Nao ha nada para fundir.\n");
        G2->n = 0;
        return;
    }

    // Novo número de vértices = original - 1
    G2->n = G->n - 1;

    // Copiar nomes dos vértices, exceto o vj (que será removido)
    int idx = 0;
    for (i = 0; i < G->n; i++) {
        if (i == vj) continue;
        strcpy(G2->nome_ver[idx], G->nome_ver[i]);
        idx++;
    }

    // Criar matriz de adjacência zerada para G2
    for (i = 0; i < G2->n; i++) {
        for (j = 0; j < G2->n; j++) {
            G2->adj[i][j] = 0;
            strcpy(G2->nome_are[i][j], "");
        }
    }

    // Copiar todas as arestas que não envolvam vj
    for (i = 0; i < G->n; i++) {
        if (i == vj) continue;
        for (j = 0; j < G->n; j++) {
            if (j == vj) continue;

            int i2 = remapIndice(i, vj);
            int j2 = remapIndice(j, vj);

            G2->adj[i2][j2] = G->adj[i][j];
            strcpy(G2->nome_are[i2][j2], G->nome_are[i][j]);
        }
    }

    // Fundir as arestas do vj em vi:
    int vi2 = remapIndice(vi, vj);

    for (i = 0; i < G->n; i++) {
        if (i == vi || i == vj) continue;

        if (G->adj[vj][i]) {
            int i2 = remapIndice(i, vj);

            if (!G2->adj[vi2][i2]) {
                G2->adj[vi2][i2] = 1;
                G2->adj[i2][vi2] = 1;

                snprintf(G2->nome_are[vi2][i2], 100, "fundida_%s_%s", G->nome_are[vi][i], G->nome_are[vj][i]);
                strcpy(G2->nome_are[i2][vi2], G2->nome_are[vi2][i2]);
            }
        }
    }
}

void executarFundirVertices() {
    Grafo G, G2;
    char nomeArquivoG[100];
    char nome_vi[50], nome_vj[50];

    system("clear");
    printf("Digite o nome do grafo original G1: ");
    scanf("%99s", nomeArquivoG);

    lerGrafo(&G, nomeArquivoG);

    printf("Digite o nome do primeiro vertice (vi): ");
    scanf("%49s", nome_vi);

    printf("Digite o nome do segundo vertice (vj): ");
    scanf("%49s", nome_vj);

    // Inicializar G2 para zeros
    memset(&G2, 0, sizeof(Grafo));

    fundirVertices(&G, nome_vi, nome_vj, &G2);

    if (G2.n == 0) {
        printf("Erro ao fundir vertices.\n");
        getchar();getchar();
        return;
    }

    printf("\nGRAFO APOS FUNDIR VERTICES %s e %s\n", nome_vi, nome_vj);
    imprimirGrafoDOT(&G2);

    getchar();getchar();
}

void dfsEuler(Grafo* g, int v, int visitado[]) {
    visitado[v] = 1;
    for (int i = 0; i < g->n; i++) {
        if (g->adj[v][i] && !visitado[i]) {
            dfsEuler(g, i, visitado);
        }
    }
}

bool ehConexo(Grafo* g) {
    int visitado[MAX_VERTICES] = {0};

    // Encontra primeiro vértice com grau > 0
    int inicio = -1;
    for (int i = 0; i < g->n; i++) {
        if (grauVertice(g, i) > 0) {
            inicio = i;
            break;
        }
    }

    if (inicio == -1) {
        return true; // Grafo sem arestas é considerado conexo para Euler
    }

    dfsEuler(g, inicio, visitado);

    for (int i = 0; i < g->n; i++) {
        if (grauVertice(g, i) > 0 && !visitado[i]) {
            return false;
        }
    }
    return true;
}

bool grafoDeEuler(Grafo* g) {
    if (!ehConexo(g)) {
        return false;
    }

    for (int i = 0; i < g->n; i++) {
        if (grauVertice(g, i) % 2 != 0) {
            return false;
        }
    }

    return true;
}

void verificarEuler() {
    char nomeArquivo[100];
    Grafo g;

    system("clear");
    printf("Digite o nome do arquivo do grafo (.dot): ");
    scanf("%s", nomeArquivo);

    lerGrafo(&g, nomeArquivo);

    if (g.n == 0) {
        printf("Falha ao carregar o grafo ou grafo vazio.\n");
        return;
    }

    if (grafoDeEuler(&g)) {
        printf("O grafo É um GRAFO DE EULER (possui ciclo euleriano).\n");
    } else {
        printf("O grafo NÃO é um grafo de Euler.\n");
    }
    getchar();getchar();
}

bool hamiltonianoUtil(Grafo* g, int pos, int caminho[], bool visitado[]) {
    if (pos == g->n) {
        // Verifica se há aresta de volta ao início (fechando o circuito)
        if (g->adj[caminho[pos - 1]][caminho[0]]) {
            return true;
        } else {
            return false;
        }
    }

    for (int v = 1; v < g->n; v++) {
        if (g->adj[caminho[pos - 1]][v] && !visitado[v]) {
            caminho[pos] = v;
            visitado[v] = true;

            if (hamiltonianoUtil(g, pos + 1, caminho, visitado)) {
                return true;
            }

            // backtrack
            visitado[v] = false;
        }
    }

    return false;
}

bool existeCircuitoHamiltoniano(Grafo* g) {
    int caminho[MAX_VERTICES];
    bool visitado[MAX_VERTICES] = {false};

    caminho[0] = 0;         // começa do vértice 0
    visitado[0] = true;

    return hamiltonianoUtil(g, 1, caminho, visitado);
}

void verificarHamiltoniano() {
    char nomeArquivo[100];
    Grafo g;

    system("clear");
    printf("Digite o nome do arquivo do grafo (.dot): ");
    scanf("%s", nomeArquivo);

    lerGrafo(&g, nomeArquivo);

    if (g.n == 0) {
        printf("Falha ao carregar o grafo ou grafo vazio.\n");
        return;
    }

    if (existeCircuitoHamiltoniano(&g)) {
        printf("O grafo POSSUI um circuito Hamiltoniano.\n");
    } else {
        printf("O grafo NÃO possui circuito Hamiltoniano.\n");
    }
    getchar();getchar();
}

bool eh_arvore() {
    // Verifica se o grafo é conexo
    Grafo g;
    int arestas = contarArestas("grafo.dot");
    inicializarGrafo(&g);
    lerGrafo(&g,"grafo.dot");
    
    if(ehConexo(&g) && arestas == g.n - 1) {
        return true;
    }
     return false;
}
// Função para verificar se 'T' é uma árvore abrangente de 'G'
bool eh_arvore_abrangente(Grafo* T, Grafo* G) {
    bool disjuncaoAresta, disjuncaoVertice; 
    if (!verificaSubgrafo(T, G, &disjuncaoAresta, &disjuncaoVertice)) {
        printf("Erro: O grafo candidato nao eh um subgrafo do grafo original.\n");
        return false;
    }
    if (T->n != G->n) {
        printf("Erro: O grafo candidato nao contem todos os vertices do grafo original (%d vs %d).\n", T->n, G->n);
        return false;
    }

    // Condição 3: T deve ser uma arvore
    if (!eh_arvore(T)) { // eh_arvore já verifica conexidade, ausência de ciclo e V-1 arestas
        printf("Erro: O grafo candidato nao eh uma arvore.\n");
        return false;
    }

    printf("O grafo candidato EH uma arvore abrangente do grafo original.\n");
    return true;
}

int calcularGrau(Grafo* g, int v) {
    int grau = 0;
    for (int i = 0; i < g->n; i++) {
        if (g->adj[v][i]) {
            grau++;
        }
    }
    return grau;
}
// --- Função Principal para Encontrar Centros ---
void encontrarCentrosArvore(Grafo* g_original) {
    // 1. Criar uma cópia modificável do grafo
    Grafo g_copia;
    inicializarGrafo(&g_copia);
    g_copia.n = g_original->n;

    // Copiar matriz de adjacência e nomes dos vértices
    for (int i = 0; i < g_original->n; i++) {
        strcpy(g_copia.nome_ver[i], g_original->nome_ver[i]);
        for (int j = 0; j < g_original->n; j++) {
            g_copia.adj[i][j] = g_original->adj[i][j];
        }
    }

    //Array para armazenar o grau atual de cada vértice na cópia
    int graus[MAX_VERTICES];
    
    // Array para marcar se um vértice foi "removido" (logicamente)
    bool removido[MAX_VERTICES];
    memset(removido, false, sizeof(removido));

    // Fila para armazenar vértices de grau 1 (folhas)
    int fila[MAX_VERTICES];
    int frente = 0, tras = 0;

    // 2. Inicializar graus e identificar folhas iniciais
    int vertices_restantes = g_copia.n;
    for (int i = 0; i < g_copia.n; i++) {
        graus[i] = calcularGrau(&g_copia, i);
        if (graus[i] == 1) { // Se for uma folha, adiciona à fila
            fila[tras++] = i;
            removido[i] = true; // Marca como logicamente removido
            vertices_restantes--;
        } else if (graus[i] == 0 && g_copia.n > 1) { // Vértice isolado em grafo com mais de 1 vértice
            // Se o grafo original tem mais de 1 vértice e este está isolado,
            // ele não pode ser parte de uma árvore abrangente, mas para o algoritmo
            // de centro, ele é uma "folha" de grau 0.
            // Para árvores válidas, não teríamos vértices de grau 0 se n > 1.
            // No entanto, para robustez, podemos tratá-lo como removido.
            fila[tras++] = i;
            removido[i] = true;
            vertices_restantes--;
        }
    }

    // 3. Processar a fila (remover folhas iterativamente)
    while (vertices_restantes > 2) { // Continua enquanto houver mais de 2 vértices restantes
        int num_folhas_na_camada = tras - frente; // Quantas folhas na camada atual
        if (num_folhas_na_camada == 0) { // Se não há mais folhas para remover e ainda há muitos vértices
            // Isso pode acontecer se o grafo não for uma árvore (ex: um ciclo)
            // ou se for uma árvore com apenas 2 vértices e um deles já foi removido.
            // Para uma árvore válida, sempre haverá folhas até restar 1 ou 2 vértices.
            printf("Erro: O grafo pode nao ser uma arvore valida ou ocorreu um problema no algoritmo.\n");
            return;
        }

        for (int k = 0; k < num_folhas_na_camada; k++) {
            int u = fila[frente++]; // Pega a folha atual da fila

            // Para cada vizinho de 'u'
            for (int v = 0; v < g_copia.n; v++) {
                if (g_copia.adj[u][v] && !removido[v]) { // Se 'v' é vizinho de 'u' e não foi removido
                    graus[v]--; // Decrementa o grau do vizinho
                    if (graus[v] == 1) { // Se o vizinho se tornou uma nova folha
                        fila[tras++] = v; // Adiciona à fila
                        removido[v] = true; // Marca como logicamente removido
                        vertices_restantes--;
                    }
                }
            }
        }
    }

    // 4. Os vértices restantes são os centros
    printf("Centro(s) da arvore: ");
    bool primeiro = true;
    for (int i = 0; i < g_copia.n; i++) {
        if (!removido[i]) { // Se o vértice não foi logicamente removido
            if (!primeiro) {
                printf(", ");
            }
            printf("%s (id %d)", g_copia.nome_ver[i], i);
            primeiro = false;
        }
    }
    printf("\n");
}

void bfs_distancias(Grafo* g, int origem, int distancias[]) {
    for (int i = 0; i < g->n; i++) {
        distancias[i] = -1; // -1 significa não visitado/inalcançável
    }

    int fila[MAX_VERTICES];
    int frente = 0, tras = 0;

    fila[tras++] = origem;
    distancias[origem] = 0;

    while (frente < tras) {
        int u = fila[frente++];

        for (int v = 0; v < g->n; v++) {
            if (g->adj[u][v] && distancias[v] == -1) { // Se há aresta e v não foi visitado
                distancias[v] = distancias[u] + 1;
                fila[tras++] = v;
            }
        }
    }
}

// Função para calcular a excentricidade de todos os vértices
void calcularExcentricidade(Grafo* g) {
    if (g->n == 0) {
        printf("Grafo vazio. Nao eh possivel calcular excentricidade.\n");
        return;
    }

    printf("\n--- Excentricidades dos Vertices ---\n");
    int excentricidades[MAX_VERTICES];
    int distancias[MAX_VERTICES];

    for (int i = 0; i < g->n; i++) {
        bfs_distancias(g, i, distancias);
        int max_dist = 0;
        for (int j = 0; j < g->n; j++) {
            if (distancias[j] > max_dist) {
                max_dist = distancias[j];
            }
        }
        excentricidades[i] = max_dist;
        printf("Excentricidade de %s (id %d): %d\n", g->nome_ver[i], i, excentricidades[i]);
    }
}

// Função para calcular o raio de uma árvore
void calcularRaio(Grafo* g) {
    if (g->n == 0) {
        printf("Grafo vazio. Nao eh possivel calcular o raio.\n");
        return;
    }

    int excentricidades[MAX_VERTICES];
    int distancias[MAX_VERTICES];

    // Primeiro, calcula todas as excentricidades
    for (int i = 0; i < g->n; i++) {
        bfs_distancias(g, i, distancias);
        int max_dist = 0;
        for (int j = 0; j < g->n; j++) {
            if (distancias[j] > max_dist) {
                max_dist = distancias[j];
            }
        }
        excentricidades[i] = max_dist;
    }

    // Encontra o menor valor entre as excentricidades (o raio)
    int raio = MAX_VERTICES + 1; // Inicializa com um valor muito alto
    if (g->n > 0) { // Garante que há pelo menos um vértice
        raio = excentricidades[0]; // Assume a primeira excentricidade como inicial
        for (int i = 1; i < g->n; i++) {
            if (excentricidades[i] < raio) {
                raio = excentricidades[i];
            }
        }
    } else { // Grafo vazio
        raio = 0; // Ou algum valor que indique vazio
    }

    printf("\nRaio da arvore: %d\n", raio);
}

// Função auxiliar para copiar um grafo
void copiarGrafo(Grafo* origem, Grafo* destino) {
    inicializarGrafo(destino);
    destino->n = origem->n;
    for (int i = 0; i < origem->n; i++) {
        strcpy(destino->nome_ver[i], origem->nome_ver[i]);
        for (int j = 0; j < origem->n; j++) {
            destino->adj[i][j] = origem->adj[i][j];
            strcpy(destino->nome_are[i][j], origem->nome_are[i][j]);
        }
    }
}

// DFS para encontrar ciclo e retornar uma aresta do ciclo (diferente de added_edge)
// Esta é uma versão simplificada e pode precisar de mais robustez para casos complexos.
Aresta findCycleAndEdgeToRemove(Grafo* g, int start_node, int parent[], bool visited[], int added_u, int added_v) {
    visited[start_node] = true;

    for (int neighbor = 0; neighbor < g->n; neighbor++) {
        if (g->adj[start_node][neighbor]) {
            if (!visited[neighbor]) {
                parent[neighbor] = start_node;
                Aresta result = findCycleAndEdgeToRemove(g, neighbor, parent, visited, added_u, added_v);
                if (result.u != -1) return result; // Cycle found in deeper recursion
            } else if (neighbor != parent[start_node]) { // Cycle detected
                // Found a cycle. Now find an edge in this cycle to remove.
                // Prioritize removing an edge that is NOT the newly added one.
                int curr = start_node;
                while (curr != neighbor) {
                    // Check if the edge (curr, parent[curr]) is the added edge
                    if (!((curr == added_u && parent[curr] == added_v) || (curr == added_v && parent[curr] == added_u))) {
                        return (Aresta){curr, parent[curr]};
                    }
                    curr = parent[curr];
                }
                // If we reach here, it means the only edge in the cycle (other than the back-edge) is the added edge itself.
                // This implies a cycle of length 2 (parallel edges), which shouldn't happen in a simple graph.
                // Or, it's a more complex cycle where the added edge is critical.
                // For simplicity, if no other edge is found, return the added edge (this might break the tree structure if not handled carefully).
                // A more robust solution would involve finding a non-bridge edge in the cycle.
                return (Aresta){added_u, added_v}; // Fallback: return the added edge (might not be ideal)
            }
        }
    }
    return (Aresta){-1, -1}; // No cycle found from this path
}
// Função para gerar UMA árvore abrangente usando BFS
bool gerarArvoreAbrangencia(Grafo* g_original, Grafo* a1_resultante) {
    inicializarGrafo(a1_resultante);
    a1_resultante->n = g_original->n;
    for (int i = 0; i < g_original->n; i++) {
        strcpy(a1_resultante->nome_ver[i], g_original->nome_ver[i]);
    }

    if (g_original->n == 0) return false;

    bool visitado[MAX_VERTICES] = {false};
    int fila[MAX_VERTICES];
    int frente = 0, tras = 0;
    int arestas_adicionadas = 0;

    int start_node = -1;
    for(int i=0; i<g_original->n; i++) {
        if(calcularGrau(g_original, i) > 0 || g_original->n == 1) {
            start_node = i;
            break;
        }
    }
    if(start_node == -1 && g_original->n > 0) {
        printf("Grafo sem arestas, nao eh possivel gerar arvore abrangente (a menos que seja um unico vertice).\n");
        return false;
    }
    if(g_original->n == 1) {
        return true;
    }

    fila[tras++] = start_node;
    visitado[start_node] = true;

    while (frente < tras) {
        int u = fila[frente++];

        for (int v = 0; v < g_original->n; v++) {
            if (g_original->adj[u][v] && !visitado[v]) {
                visitado[v] = true;
                fila[tras++] = v;
                a1_resultante->adj[u][v] = 1;
                a1_resultante->adj[v][u] = 1;
                strcpy(a1_resultante->nome_are[u][v], g_original->nome_are[u][v]);
                strcpy(a1_resultante->nome_are[v][u], g_original->nome_are[v][u]);
                arestas_adicionadas++;
            }
        }
    }

    for (int i = 0; i < g_original->n; i++) {
        if (calcularGrau(g_original, i) > 0 && !visitado[i]) {
            printf("O grafo original nao eh conexo, nao eh possivel gerar uma arvore abrangente.\n");
            return false;
        }
    }

    return true;
}

// Gera k outras árvores abrangentes por trocas cíclicas
void gerarOutrasArvoresAbrangencia(Grafo* g_original, Grafo* a1_base, int k) {
    if (k <= 0) return;

    Grafo current_spanning_tree;
    copiarGrafo(a1_base, &current_spanning_tree);

    printf("\nGerando %d outras arvores abrangentes...\n", k);

    for (int count = 0; count < k; count++) {
        int u_add = -1, v_add = -1;
        for (int i = 0; i < g_original->n; i++) {
            for (int j = i + 1; j < g_original->n; j++) {
                if (g_original->adj[i][j] && !current_spanning_tree.adj[i][j]) {
                    u_add = i;
                    v_add = j;
                    break;
                }
            }
            if (u_add != -1) break;
        }

        if (u_add == -1) {
            printf("Nao ha mais arestas para adicionar e formar novas arvores abrangentes.\n");
            break;
        }

        current_spanning_tree.adj[u_add][v_add] = 1;
        current_spanning_tree.adj[v_add][u_add] = 1;
        strcpy(current_spanning_tree.nome_are[u_add][v_add], g_original->nome_are[u_add][v_add]);
        strcpy(current_spanning_tree.nome_are[v_add][u_add], g_original->nome_are[v_add][u_add]);

        bool visited[MAX_VERTICES] = {false};
        int parent[MAX_VERTICES];
        memset(parent, -1, sizeof(parent));

        Aresta edge_to_remove = findCycleAndEdgeToRemove(&current_spanning_tree, u_add, parent, visited, u_add, v_add);

        if (edge_to_remove.u != -1) {
            current_spanning_tree.adj[edge_to_remove.u][edge_to_remove.v] = 0;
            current_spanning_tree.adj[edge_to_remove.v][edge_to_remove.u] = 0;
            printf("  Gerada Arvore Abrangente %d (adicionou %s--%s, removeu %s--%s)\n",
                   count + 1, g_original->nome_ver[u_add], g_original->nome_ver[v_add],
                   g_original->nome_ver[edge_to_remove.u], g_original->nome_ver[edge_to_remove.v]);
            // Opcional: imprimir a matriz da nova árvore abrangente
            // imprimirMatrizAdj(&current_spanning_tree);
        } else {
            printf("  Nao foi possivel encontrar uma aresta para remover do ciclo. Parando.\n");
            break;
        }
    }
}

// Calcula a distância entre duas árvores abrangentes
int calcularDistanciaEntreArvores(Grafo* a1, Grafo* a2) {
    if (a1->n != a2->n) {
        printf("As arvores devem ter o mesmo numero de vertices para calcular a distancia.\n");
        return -1;
    }

    int distancia = 0;
    for (int i = 0; i < a1->n; i++) {
        for (int j = i + 1; j < a1->n; j++) {
            if (a1->adj[i][j] && !a2->adj[i][j]) {
                distancia++;
            }
            else if (!a1->adj[i][j] && a2->adj[i][j]) {
                distancia++;
            }
        }
    }
    return distancia;
}

int obterRaio(Grafo* g) {
    if (g->n == 0) return 0; // Raio de grafo vazio

    int excentricidades[MAX_VERTICES];
    int distancias[MAX_VERTICES];

    for (int i = 0; i < g->n; i++) {
        bfs_distancias(g, i, distancias);
        int max_dist = 0;
        for (int j = 0; j < g->n; j++) {
            if (distancias[j] > max_dist) {
                max_dist = distancias[j];
            }
        }
        excentricidades[i] = max_dist;
    }

    int raio = MAX_VERTICES + 1;
    if (g->n > 0) {
        raio = excentricidades[0];
        for (int i = 1; i < g->n; i++) {
            if (excentricidades[i] < raio) {
                raio = excentricidades[i];
            }
        }
    }
    return raio;
}

// --- Funções Auxiliares para Geração de Todas as Árvores Abrangentes ---

// Compara duas árvores abrangentes (matrizes de adjacência)
// Retorna true se são idênticas, false caso contrário
bool compararArvores(Grafo* t1, Grafo* t2) {
    if (t1->n != t2->n) return false;
    for (int i = 0; i < t1->n; i++) {
        for (int j = 0; j < t1->n; j++) {
            if (t1->adj[i][j] != t2->adj[i][j]) return false;
        }
    }
    return true;
}

// Adiciona uma árvore abrangente à lista se ela ainda não estiver lá
void adicionarArvoreUnica(Grafo* t) {
    for (int i = 0; i < num_arvores_encontradas; i++) {
        if (compararArvores(t, &todas_arvores_abrangentes[i])) {
            return; // Já existe
        }
    }
    if (num_arvores_encontradas < 1000) {
        copiarGrafo(t, &todas_arvores_abrangentes[num_arvores_encontradas]);
        num_arvores_encontradas++;
    } else {
        printf("Aviso: Limite de arvores abrangentes excedido. Algumas podem nao ser consideradas.\n");
    }
}

// Função recursiva para gerar todas as árvores abrangentes
// current_tree: a árvore abrangente atual
// g_original: o grafo original
void generateAllSpanningTrees(Grafo* current_tree, Grafo* g_original) {
    adicionarArvoreUnica(current_tree);

    // Para cada aresta (u,v) em g_original que NÃO está em current_tree
    for (int u_add = 0; u_add < g_original->n; u_add++) {
        for (int v_add = u_add + 1; v_add < g_original->n; v_add++) {
            if (g_original->adj[u_add][v_add] && !current_tree->adj[u_add][v_add]) {
                // Adiciona a aresta (u_add, v_add) temporariamente para formar um ciclo
                current_tree->adj[u_add][v_add] = 1;
                current_tree->adj[v_add][u_add] = 1;

                // Encontra o ciclo formado
                bool visited[MAX_VERTICES] = {false};
                int path[MAX_VERTICES];
                int path_len = 0;
                Edge cycle_edges[MAX_VERTICES]; // Max edges in a cycle
                int cycle_len = 0;

                // Encontra o ciclo que contém (u_add, v_add)
                // Inicia DFS de u_add, com v_add como "pai" para encontrar o ciclo
                // (Isso é uma simplificação, uma DFS mais robusta seria necessária)
                // Uma forma mais segura é rodar BFS/DFS para encontrar o caminho entre u_add e v_add na current_tree
                // e esse caminho + (u_add, v_add) forma o ciclo.

                // Para simplificar, vamos encontrar o caminho entre u_add e v_add na current_tree
                // (sem a aresta (u_add,v_add) que acabamos de adicionar)
                // e depois adicionar (u_add,v_add) para formar o ciclo.
                int dists[MAX_VERTICES];
                int parents_bfs[MAX_VERTICES];
                for(int i=0; i<g_original->n; i++) parents_bfs[i] = -1;

                // Temporariamente remove a aresta adicionada para rodar BFS na árvore
                current_tree->adj[u_add][v_add] = 0;
                current_tree->adj[v_add][u_add] = 0;

                // Roda BFS para encontrar caminho entre u_add e v_add na árvore
                int q[MAX_VERTICES];
                int q_f = 0, q_t = 0;
                bool bfs_visited[MAX_VERTICES] = {false};

                q[q_t++] = u_add;
                bfs_visited[u_add] = true;

                while(q_f < q_t) {
                    int curr = q[q_f++];
                    if (curr == v_add) break; // Path found

                    for(int neighbor = 0; neighbor < g_original->n; neighbor++) {
                        if (current_tree->adj[curr][neighbor] && !bfs_visited[neighbor]) {
                            bfs_visited[neighbor] = true;
                            parents_bfs[neighbor] = curr;
                            q[q_t++] = neighbor;
                        }
                    }
                }

                // Re-adiciona a aresta para formar o ciclo
                current_tree->adj[u_add][v_add] = 1;
                current_tree->adj[v_add][u_add] = 1;

                // Constrói o ciclo
                cycle_len = 0;
                int curr_node = v_add;
                while(curr_node != u_add && curr_node != -1) {
                    cycle_edges[cycle_len++] = (Edge){curr_node, parents_bfs[curr_node]};
                    curr_node = parents_bfs[curr_node];
                }
                cycle_edges[cycle_len++] = (Edge){u_add, v_add}; // The added edge itself

                // Para cada aresta (x,y) no ciclo (exceto a aresta adicionada)
                for (int i = 0; i < cycle_len; i++) {
                    Edge e_remove = cycle_edges[i];
                    // Garante que não removemos a aresta que acabamos de adicionar
                    if (!((e_remove.u == u_add && e_remove.v == v_add) || (e_remove.u == v_add && e_remove.v == u_add))) {
                        // Remove a aresta (x,y)
                        current_tree->adj[e_remove.u][e_remove.v] = 0;
                        current_tree->adj[e_remove.v][e_remove.u] = 0;

                        // Recursivamente chama para a nova árvore abrangente
                        generateAllSpanningTrees(current_tree, g_original);

                        // Volta a aresta (backtrack)
                        current_tree->adj[e_remove.u][e_remove.v] = 1;
                        current_tree->adj[e_remove.v][e_remove.u] = 1;
                    }
                }

                // Remove a aresta (u_add, v_add) que foi adicionada no início do loop
                current_tree->adj[u_add][v_add] = 0;
                current_tree->adj[v_add][u_add] = 0;
            }
        }
    }
}
// --- Função Principal para Encontrar a Árvore Central ---
void encontrarArvoreCentral(Grafo* g_original) {
    if (g_original->n == 0) {
        printf("Erro: Grafo vazio. Nao eh possivel encontrar a arvore central.\n");
        return;
    }

    // 1. Gerar uma árvore abrangente inicial
    Grafo initial_spanning_tree;
    inicializarGrafo(&initial_spanning_tree);
    if (!gerarArvoreAbrangencia(g_original, &initial_spanning_tree)) {
        printf("Nao foi possivel gerar uma arvore abrangente inicial (grafo pode nao ser conexo).\n");
        return;
    }

    // Resetar a lista de árvores encontradas
    num_arvores_encontradas = 0;

    // 2. Gerar todas as árvores abrangentes a partir da inicial
    printf("Iniciando a geracao de TODAS as arvores abrangentes...\n");
    generateAllSpanningTrees(&initial_spanning_tree, g_original);
    printf("Total de arvores abrangentes unicas encontradas: %d\n", num_arvores_encontradas);

    if (num_arvores_encontradas == 0) {
        printf("Nenhuma arvore abrangente encontrada. Nao eh possivel determinar a arvore central.\n");
        return;
    }

    // 3. Encontrar a(s) árvore(s) com o menor raio
    int min_raio = MAX_VERTICES + 1;
    for (int i = 0; i < num_arvores_encontradas; i++) {
        int current_raio = obterRaio(&todas_arvores_abrangentes[i]);
        if (current_raio < min_raio) {
            min_raio = current_raio;
        }
    }

    printf("\nRaio minimo encontrado: %d\n", min_raio);
    printf("Arvore(s) Central(is):\n");
    bool found_one = false;
    for (int i = 0; i < num_arvores_encontradas; i++) {
        if (obterRaio(&todas_arvores_abrangentes[i]) == min_raio) {
            printf("----------------------------------------\n");
            imprimirMatrizAdj(&todas_arvores_abrangentes[i]);
            found_one = true;
        }
    }
    if (!found_one) {
        printf("Nenhuma arvore central encontrada (erro logico).\n");
    }
    printf("----------------------------------------\n");
}

























































void operacoes() {
    bool sair = false;
    int opcao;

    while(!sair) {
        system("clear");
        menu_operacao();

        printf("Digite a opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                uniao_grafos();
                break;
            case 2:
                executarIntersecao();
                break;
            case 3:
                executarDiferencaSimetrica();
                break;
            case 4:
                executarRemoverVertice();
                break;
            case 5:
                executarRemoverAresta();
                break;
            case 6:
                executarFundirVertices();
                break;
            case 7:
                verificarEuler();
                break;
            case 8:
                verificarHamiltoniano();
                break;
        default:
            sair = true;
            break;
        }
    }
}

void arvores() {
    bool sair = false;
    int opcao;
    Grafo g_original; // Grafo G (para abrangencia, etc.)
    Grafo g_candidato; // Grafo T (para abrangencia, etc.)

    while(!sair) {
        system("clear");
        // Assume que menu_arvores() está definida e exibe o menu correto
        menu_arvores();

        printf("Digite a opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: // EH-ÁRVORE (Pergunta 3)
                {
                    Grafo g_temp;
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_temp, "grafo.dot");
                    if (g_temp.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado de 'grafo.dot'.\n");
                    } else {
                        // Assume que eh_arvore() está definida
                        if(eh_arvore(&g_temp)==true) {
                            printf("O grafo eh uma arvore.\n");
                        }else{
                            printf("O grafo nao eh uma arvore.\n");
                        }
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 2: // ÁRVORE-ABRANGÊNCIA (Pergunta 2b)
                {
                    char nome_arquivo_g[100];
                    char nome_arquivo_t[100];

                    printf("Digite o nome do arquivo do grafo ORIGINAL (G): ");
                    scanf("%99s", nome_arquivo_g);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_original, nome_arquivo_g);

                    printf("Digite o nome do arquivo do grafo CANDIDATO (T): ");
                    scanf("%99s", nome_arquivo_t);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_candidato, nome_arquivo_t);

                    if (g_original.n == 0 || g_candidato.n == 0) {
                        printf("Erro: Um ou ambos os grafos nao foram carregados corretamente.\n");
                    } else {
                        printf("\nVerificando se '%s' eh uma arvore abrangente de '%s'...\n", nome_arquivo_t, nome_arquivo_g);
                        // Assume que eh_arvore_abrangente() está definida
                        eh_arvore_abrangente(&g_candidato, &g_original);
                    }
                    // Assume que getchar() está definida
                    getchar(); getchar();
                }
                break;
            case 3: // CENTROS (Pergunta 4)
                {
                    Grafo g_para_operacao;
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_para_operacao, "grafo.dot");

                    if (g_para_operacao.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado de 'grafo.dot'.\n");
                    } else {
                        printf("\nDeterminando o(s) centro(s) da arvore em 'grafo.dot'...\n");
                        // Assume que encontrarCentrosArvore() está definida
                        encontrarCentrosArvore(&g_para_operacao);
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 4: // EXCENTRICIDADE (Pergunta 5)
                {
                    Grafo g_para_operacao;
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_para_operacao, "grafo.dot");

                    if (g_para_operacao.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado de 'grafo.dot'.\n");
                    } else {
                        printf("\nCalculando excentricidades para o grafo em 'grafo.dot'...\n");
                        // Assume que calcularExcentricidade() está definida
                        calcularExcentricidade(&g_para_operacao);
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 5: // RAIO (Pergunta 6)
                {
                    Grafo g_para_operacao;
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_para_operacao, "grafo.dot");

                    if (g_para_operacao.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado de 'grafo.dot'.\n");
                    } else {
                        printf("\nCalculando o raio para o grafo em 'grafo.dot'...\n");
                        // Assume que calcularRaio() está definida
                        calcularRaio(&g_para_operacao);
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 6: // GERAR ÁRVORE ABRANGÊNCIA (Pergunta 7)
                {
                    char nome_arquivo_g[100];
                    Grafo g_para_gerar_abrangencia;
                    Grafo a1_gerada;

                    printf("Digite o nome do arquivo do grafo ORIGINAL (G) para gerar a arvore abrangente: ");
                    scanf("%99s", nome_arquivo_g);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_para_gerar_abrangencia, nome_arquivo_g);

                    if (g_para_gerar_abrangencia.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado.\n");
                    } else {
                        printf("\nGerando uma arvore abrangente de '%s'...\n", nome_arquivo_g);
                        // Assume que gerarArvoreAbrangencia() está definida
                        if (gerarArvoreAbrangencia(&g_para_gerar_abrangencia, &a1_gerada)) {
                            printf("Arvore abrangente gerada com sucesso:\n");
                            // Assume que imprimirMatrizAdj() está definida
                            imprimirMatrizAdj(&a1_gerada);
                            // Parte 7a: Gerar outras k arvores
                            int k_outras;
                            printf("Quantas outras arvores abrangentes deseja gerar por trocas ciclicas? ");
                            scanf("%d", &k_outras);
                            // Assume que gerarOutrasArvoresAbrangencia() está definida
                            gerarOutrasArvoresAbrangencia(&g_para_gerar_abrangencia, &a1_gerada, k_outras);
                        } else {
                            printf("Nao foi possivel gerar uma arvore abrangente (grafo pode nao ser conexo).\n");
                        }
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 7: // DISTÂNCIA ENTRE ÁRVORES ABRANGÊNCIA (Pergunta 8)
                {
                    char nome_a1[100], nome_a2[100];
                    Grafo arvore1, arvore2;

                    printf("Digite o nome do arquivo da primeira arvore abrangente (A1): ");
                    scanf("%99s", nome_a1);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&arvore1, nome_a1);

                    printf("Digite o nome do arquivo da segunda arvore abrangente (A2): ");
                    scanf("%99s", nome_a2);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&arvore2, nome_a2);

                    if (arvore1.n == 0 || arvore2.n == 0) {
                        printf("Erro: Uma ou ambas as arvores nao foram carregadas corretamente.\n");
                    } else {
                        // Assume que calcularDistanciaEntreArvores() está definida
                        int dist = calcularDistanciaEntreArvores(&arvore1, &arvore2);
                        if (dist != -1) {
                            printf("\nDistancia entre as arvores '%s' e '%s': %d\n", nome_a1, nome_a2, dist);
                        }
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 8: // ÁRVORE CENTRAL (Pergunta 9)
                {
                    Grafo g_para_arvore_central;
                    printf("Digite o nome do arquivo do grafo ORIGINAL (G) para encontrar a arvore central: ");
                    char nome_arquivo_g_central[100];
                    scanf("%99s", nome_arquivo_g_central);
                    // Assume que lerGrafo() está definida
                    lerGrafo(&g_para_arvore_central, nome_arquivo_g_central);

                    if (g_para_arvore_central.n == 0) {
                        printf("Erro: Grafo vazio ou nao carregado de '%s'.\n", nome_arquivo_g_central);
                    } else {
                        // Assume que encontrarArvoreCentral() está definida
                        encontrarArvoreCentral(&g_para_arvore_central);
                    }
                    // Assume que getchar() está definida
                    getchar();getchar();
                }
                break;
            case 9: // Voltar
                sair = true;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                // Assume que getchar() está definida
                getchar();getchar();
                break;
        }
    }
}

void executar_menu_transformacao() {
    int opcao;
    Grafo matriz; // matriz de adjacência
    GrafoAdj lista;
    GrafoIncidencia incidencia;
    bool sair = false;

    // Lê o grafo do arquivo grafo.dot para matriz de adjacência
    lerGrafo(&matriz,"grafo.dot");

    while (!sair) {
        system("clear"); // ou use system("clear") se for Linux/macOS
        menu_transformacao();
        printf("Digite a opcao desejada: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: // Matriz -> Lista
                system("clear");
                linha_final();
                printf("][===[-Matriz-De-Adjacencia-->--Lista-Adj-]===][\n");
                linha_final();
                imprimirMatrizAdj(&matriz);
                linha_final();
                matrizParaLista(&matriz, &lista);
                imprimirListaAdj(&lista);
                break;

            case 2: // Matriz -> Incidência
                system("clear");
                linha_final();
                printf("][===[-Matriz-De-Adjacencia-->-Incidencia-]===][\n");
                linha_final();
                imprimirMatrizAdj(&matriz);
                linha_final();
                matrizParaIncidencia(&matriz, &incidencia);
                imprimirMatrizIncidencia(&incidencia);
                break;

            case 3: // Lista -> Matriz
                system("clear");
                linha_final();
                printf("][===[-Lista-De-Adjacencia-->--Matriz-Adj-]===][\n");
                linha_final();
                matrizParaLista(&matriz, &lista); // gera a lista para simular a transformação inversa
                imprimirListaAdj(&lista);
                linha_final();
                Grafo novaMatriz;
                listaParaMatriz(&lista, &novaMatriz);
                imprimirMatrizAdj(&novaMatriz);
                break;

            case 4: // Lista -> Incidência
                system("clear");
                linha_final();
                printf("][===[-Lista-De-Adjacencia-->--Incidencia-]===][\n");
                linha_final();
                matrizParaLista(&matriz, &lista); // gera a lista
                imprimirListaAdj(&lista);
                linha_final();
                listaParaIncidencia(&lista, &incidencia);
                imprimirMatrizIncidenci(&incidencia);
                break;

            case 5: // Incidência -> Matriz
                system("clear");
                linha_final();
                printf("][===[-Matriz-De-Incidencia-->--Adjacencia]===][\n");
                linha_final();
                matrizParaIncidencia(&matriz, &incidencia);
                imprimirMatrizIncidenci(&incidencia);
                linha_final();
                Grafo novaMatriz2;
                incidenciaParaMatriz(&incidencia, &novaMatriz2);
                imprimirMatrizAdj(&novaMatriz2);
                break;

            case 6: // Incidência -> Lista
                system("clear");
                linha_final();
                printf("][===[-Matriz-De-Incidencia-->--Lista-Adj-]===][\n");
                linha_final();
                matrizParaIncidencia(&matriz, &incidencia);
                imprimirMatrizIncidenci(&incidencia);
                linha_final();
                GrafoAdj novaLista;
                incidenciaParaLista(&incidencia, &novaLista);
                imprimirListaAdj(&novaLista);
                break;

            case 7:
                printf("Voltando ao menu anterior...\n");
                sair = true;
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
        if(sair == false) {
            linha_final();
            printf("\nPressione Enter para continuar...");
            getchar(); getchar(); // pausa até usuário pressionar Enter
        }
    }
}


void menu_visualizacao() {
    int opcao;
    int i;
    bool sair = false;

    while(!sair) {
        limpar_terminal();
        menu_visualizacao_impressao();

        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                limpar_terminal();
                exibirMatrizAdjacente();
                getchar();
                break;
            case 2:
                limpar_terminal();
                exibirMatrizIncidencia();
                getchar();
                break;
            case 3:
                limpar_terminal();
                exibirListaDeAdjacencia();
                getchar();
                break;
            default:
                sair = true;
                break;
        }
    }
    
}


void menu_ciclos_buscas() {
    int i;
    bool sair = false;
    int opcao;


    while(!sair) {
        limpar_terminal();
        menu_ciclo_buscas_impressao();
        printf("Digite a opção desejada: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                limpar_terminal();
                buscando();
                getchar();getchar();
                break;
            case 2:
                limpar_terminal();
                menuVerificacaoSubgrafo();
                getchar();getchar();
                break;
            case 3:
                limpar_terminal();
                verificarCicloConjunto();
                getchar();getchar();
                break;
            case 4:
                limpar_terminal();
                printf("Os grafos sao isomorfos? %s\n",verificaIsomorfismo() ? "Sim" : "Nao");
                sleep(4);
                getchar();getchar();
                break;
            case 5:
                sair = true;
                break;
        }
    }
}














































































/*
#define MAX_LINE 512
#define MAX_PROFESSORES 100
#define MAX_DISC  20
#define MAX_VERTICES 80
#define MAX_LABEL_LENGTH 80 




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

void imprimindo_caracteristicas(disciplina* disciplinas, int qtd_disc, professor* professores, int qtd_prof) {
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                      QUANTIDADE DISCIPLINAS: %d                        ║\n",qtd_disc);
    printf("║                      QUANTIDADE PROFESSORES: %d                        ║\n",qtd_prof);
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
}



// => APLICANDO ESTRATEGIA PARA DETERMINAR OS PROFESSORES(BACKTRACKING):
disciplina* ordenar_disciplinas_importantes(disciplina* disciplinas, int qtd_disc, int semestre) {
    int i, j, menor_idx;
    disciplina aux;

    // Duplicar peso para as disciplinas do semestre informado
    for(i = 0; i < qtd_disc; i++) {
        if(disciplinas[i].semestre == semestre) {
            disciplinas[i].peso *= 2;
        }
    }

    // Selection Sort crescente pelo peso
    for(i = 0; i < qtd_disc - 1; i++) {
        menor_idx = i;
        for(j = i + 1; j < qtd_disc; j++) {
            if(disciplinas[j].peso < disciplinas[menor_idx].peso) {
                menor_idx = j;
            }
        }
        // Troca os elementos nas posições i e menor_idx
        if(menor_idx != i) {
            aux = disciplinas[i];
            disciplinas[i] = disciplinas[menor_idx];
            disciplinas[menor_idx] = aux;
        }
    }

    return disciplinas;
}

int* lista_professores_aulas(professor *professores, int qtd_prof) {
    // Aloca memória para a lista de inteiros.
    int* lista = (int*)malloc(qtd_prof * sizeof(int));
    if (lista == NULL) {
        // É uma boa prática verificar se a alocação de memória foi bem-sucedida.
        return NULL;
    }
    // Preenche a lista com o valor de max_aulas de cada professor.
    for (int i = 0; i < qtd_prof; i++) {
        lista[i] = professores[i].max_aulas;
    }
    
    return lista;
}

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
) {
    // Condição de parada: chegou ao fim da lista de disciplinas
    if (pos == qtd_disc) {
        if (disciplinas_alocadas > *max_disciplinas) {
            *max_disciplinas = disciplinas_alocadas;
        }
        return 0; // Retorna para continuar explorando outros ramos
    }
    int encontrou_professor_valido = 0;
    ProfAdj* aux = lista_adjacencia[pos].lista_profs;

    // --- TENTATIVA 1: Alocar um professor para a disciplina 'pos' ---
    while (aux != NULL) {
        int prof_id = aux->prof->id;

        // Verifica se o professor pode dar mais uma aula
        if (prof_aulas[prof_id] < aux->prof->max_aulas) {
            encontrou_professor_valido = 1; // Marca que encontramos uma opção

            // Aloca
            lista[pos] = prof_id;
            prof_aulas[prof_id]++;

            // Chama a recursão para a próxima disciplina
            backtracking_max_disciplinas(lista_adjacencia, disciplinas, qtd_disc, professores,
                                          qtd_prof, pos + 1, lista, prof_aulas, max_disciplinas, disciplinas_alocadas + 1);

            // Desfaz a alocação (backtrack) para testar outros caminhos
            prof_aulas[prof_id]--;
            lista[pos] = -1;
        }
        aux = aux->prox;
    }

    // --- TENTATIVA 2: Não alocar esta disciplina (pular) ---
    // Esta chamada só deve acontecer se NENHUM professor foi (ou pôde ser) alocado.
    // Se você sempre fizer essa chamada, estará explorando caminhos redundantes e incorretos.
    // A lógica correta é: OU você aloca um professor (explorado no while), OU você pula.
    // O `if` abaixo garante isso.
    if (!encontrou_professor_valido) {
         backtracking_max_disciplinas(lista_adjacencia, disciplinas, qtd_disc, professores,
                                      qtd_prof, pos + 1, lista, prof_aulas, max_disciplinas, disciplinas_alocadas);
    }
    
    // Se você QUER explorar o caminho de pular a disciplina MESMO que ela possa ser alocada,
    // a chamada deve ser feita fora do if, mas isso muda o objetivo do problema.
    // A lógica que implementei acima maximiza o número de disciplinas alocadas.

    return 0;
}

*/




