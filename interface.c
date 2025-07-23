#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "interface.h"
#include "interacoes.h"


void limpar_terminal() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear"); // Linux e macOS
    #endif
}

void imprimir_linha_simples() {
    printf("][═════════════════════════════════════════════════════════════════════][\n");

}

void imprimir_banner() {
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                       ║\n");
    printf("║                  ╔═════════════════════════════════╗                  ║\n");
    printf("║                  ║   BEM-VINDO AO MENU PRINCIPAL   ║                  ║\n");
    printf("║                  ╚═════════════════════════════════╝                  ║\n");
    printf("║                                                                       ║\n");
    printf("║      ───────────────────── MATRIZ HORÁRIA ─────────────────────       ║\n");
    printf("║                                                                       ║\n");
    printf("║═══════════════════════════════════════════════════════════════════════║\n");
    printf("║═─────────────────────[ 1 ]─[RESUMO-GERAL-GRAFO]──────────────────────═║\n");
    printf("║═─────────────────────[ 2 ]─[VISUALIZANDO-REPRESENTACOES]─────────────═║\n");
    printf("║═─────────────────────[ 3 ]─[TRANSFORMAR-GRAFO-]──────────────────────═║\n");
    printf("║═─────────────────────[ 4 ]─[OPERAÇÕES-GRAFOS]────────────────────────═║\n");
    printf("║═─────────────────────[ 5 ]─[ARVORES]─────────────────────────────────═║\n");
    printf("║═─────────────────────[ 6 ]─[TROCAR-GRAFOS]───────────────────────────═║\n");
    printf("║═─────────────────────[ 7 ]─[TROCAR-GRAFOS]───────────────────────────═║\n");
    printf("║═─────────────────────[ 8 ]─[AGENDADOR]───────────────────────────────═║\n");
    printf("║═─────────────────────[ 9 ]─[SAIR]────────────────────────────────────═║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
}

void menu_inicial1() {
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║────────────────────────── INSERINDO GRAFO ────────────────────────────║\n");
    printf("║═══════════════════════════════════════════════════════════════════════║\n");
    printf("║═──────────────────────[ DIGITAR MANUALMENTE ]────────────────────────═║\n");
    printf("║═─────────────────────────[ BUSCAR ARQUIVO ]──────────────────────────═║\n");
    printf("║═──────────────────────────────[ SAIR ]───────────────────────────────═║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
}

void menu_transformacao() {
    linha_final();
    printf("][===-------[-MENU--TRANSFORMACAO-]--------===][\n");
    linha_final();
    printf("][===[-(1)-Matriz-Adjacencia-->-Lista-Adj-]===][\n");
    printf("][===[-(2)-Matriz-Adjacencia-->-Incidencia]===][\n");
    printf("][===[-(3)-Lista-Adjacencia-->-Matriz-Adj-]===][\n");
    printf("][===[-(4)-Lista-Adjacencia-->-Incidencia-]===][\n");
    printf("][===[-(5)-Matriz-Incidencia-->-Adjacencia]===][\n");
    printf("][===[-(6)-Matriz-Incidencia-->-Lista-Adj-]===][\n");
    printf("][===----------[-(7)--Voltar--]------------===][\n");
    linha_final();
}

void menu_operacao() {
    linha_final();
    printf("][===---------[-MENU--OPERACOES-]----------===][\n");
    linha_final();
    printf("][===-------------[-(1)-UNIAO-]------------===][\n");
    printf("][===----------[-(2)-INTERSECCAO-]---------===][\n");
    printf("][===------[-(3)-DIFERENCA-SIMETRICA-]-----===][\n");
    printf("][===--------[-(4)--RETIRA-VERTICE-]-------===][\n");
    printf("][===--------[-(5)--RETIRA-ARESTAS-]-------===][\n");
    printf("][===--------[-(6)-FUNDIR-VERTICES-]-------===][\n");
    printf("][===----------[-(7)-GRAFO-EULER-]---------===][\n");
    printf("][===----------[-(8)-HAMILTONIANO-]--------===][\n");
    printf("][===------------[-(9)-Voltar-]------------===][\n");
    linha_final();
}

void menu_arvores() {
    linha_final();
    printf("][===-----------[-MENU--ÁRVORES-]----------===][\n");
    linha_final();
    printf("][===-----------[-(1)-EH-ÁRVORE-]----------===][\n"); // Pergunta 3
    printf("][===-------[-(2)-ÁRVORE-ABRANGÊNCIA-]-----===][\n"); // Pergunta 2b
    printf("][===------------[-(3)-CENTROS-]-----------===][\n"); // Pergunta 4
    printf("][===---------[-(4)--EXCENTRICIDADE-]------===][\n"); // Pergunta 5
    printf("][===-------------[-(5)-RAIO-]-------------===][\n"); // Pergunta 6
    printf("][===----[-(6)-GERAR-ÁRVORE-ABRANGÊNCIA-]--===][\n"); // Pergunta 7
    printf("][===---[-(7)-DISTÂNCIA-ENTRE-ÁRVORES-]----===][\n"); // Pergunta 8
    printf("][===--------[-(8)-ÁRVORE-CENTRAL-]--------===][\n"); // Pergunta 9
    printf("][===------------[-(9)-Voltar-]------------===][\n");
    linha_final();
}

void linha_final() {
    printf("================================================\n");
}












/*
void imprimir_disciplinas_prof(professor* professores,int qtd_prof) {
    int i;

    printf("Professores carregados:\n\n");
    for (int i = 0; i < qtd_prof; i++) {
        printf("ID: %d\n", professores[i].id);
        printf("Nome: %s\n", professores[i].nome);
        printf("Matérias: %s\n", professores[i].materias);
        printf("Turno: %s\n", professores[i].turno);
        printf("Máx. Aulas: %d\n\n", professores[i].max_aulas);
    }
}

void imprimir_disciplinas_disc(disciplina* disciplinas,int qtd_disc) {
    int i;

    printf("Materias carregadas:\n\n");
    for (int i = 0; i < qtd_disc; i++) {
        printf("ID: %d\n", disciplinas[i].id);
        printf("Nome: %s\n", disciplinas[i].nome);
        printf("Peso: %d\n", disciplinas[i].peso);
        printf("Tipo Sala: %d\n", disciplinas[i].tipo_sala);
        printf("Horas Disciplina: %d\n\n", disciplinas[i].horas_disc);
    }
}

*/