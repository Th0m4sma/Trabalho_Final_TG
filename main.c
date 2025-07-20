#include <stdio.h>
#include <stdlib.h>
#include "interacoes.h"

int main() {
    int qtd_prof = 0;
    int qtd_disc = 0;
    professor *professores = carregar_professores("grafo.dot", &qtd_prof);
    disciplina *disciplinas = pegar_disciplinas("grafo.dot", &qtd_disc);


    if (!professores) {
        printf("Erro ao carregar professores.\n");
        return 1;
    }

    if (!disciplinas) {
        printf("Erro ao carregar disciplinas.\n");
        return 1;
    }








    printf("Professores carregados:\n\n");
    for (int i = 0; i < qtd_prof; i++) {
        printf("ID: %d\n", professores[i].id);
        printf("Nome: %s\n", professores[i].nome);
        printf("Matérias: %s\n", professores[i].materias);
        printf("Turno: %s\n", professores[i].turno);
        printf("Máx. Aulas: %d\n\n", professores[i].max_aulas);
    }


    printf("Materias carregadas:\n\n");
    for (int i = 0; i < qtd_disc; i++) {
        printf("ID: %d\n", disciplinas[i].id);
        printf("Nome: %s\n", disciplinas[i].nome);
        printf("Peso: %d\n", disciplinas[i].peso);
        printf("Tipo Sala: %d\n", disciplinas[i].tipo_sala);
        printf("Horas Disciplina: %d\n\n", disciplinas[i].horas_disc);
    }















































    
    // Liberar memória
    for (int i = 0; i < qtd_prof; i++) {
        free(professores[i].nome);
        free(professores[i].materias);
        free(professores[i].turno);
    }
    // Liberar memória
    for (int i = 0; i < qtd_disc; i++) {
        free(disciplinas[i].nome);
    }
    free(disciplinas);
    free(professores);

    return 0;
}
