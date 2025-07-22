#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "interacoes.h"
#include "interface.h"


int main() {
    int qtd_prof = 0;
    int qtd_disc = 0;
    int max_disciplinas = 0;
    int resultado;
    int opcao_menu;
    bool sair = false;

    NoDisc* lista_adjacencia;
    professor *professores = carregar_professores("grafo.dot", &qtd_prof);
    disciplina *disciplinas = pegar_disciplinas("grafo.dot", &qtd_disc);
    int* lista = (int *) calloc(qtd_disc * sizeof(int),0);
    int* lista_aulas_max = lista_professores_aulas(professores, qtd_prof);

    lista_adjacencia = construir_lista_adjacencia(disciplinas,qtd_disc,professores,qtd_prof);

    while(!sair) {
        limpar_terminal();
        imprimir_banner();

        printf("Digite a opção desejada: ");
        scanf("%d",&opcao_menu);

        
        switch (opcao_menu) {
        case 1:
            limpar_terminal();
            imprimindo_caracteristicas(disciplinas, qtd_disc, professores, qtd_prof);

            sleep(3);
            break;
        case 2:
            limpar_terminal();
            imprimir_lista_adjacencia(lista_adjacencia, qtd_disc);

            sleep(3);

            printf("REALIZANDO BACKTRACKING\n");
            resultado = (lista_adjacencia, disciplinas, qtd_disc, professores,
                             qtd_prof, 0, lista, lista_aulas_max, &max_disciplinas, 0);

            printf("Resultado BackTracking: %d", max_disciplinas);

            for(int i = 0;i<qtd_disc;i++) {
                printf("Posição %d = %d\n",i,lista[i]);
            }

            sleep(3);
            break;
        case 3:
            break;
        default:
            break;
        }
        
    }




    if (!professores) {
        printf("Erro ao carregar professores.\n");
        return 1;
    }

    if (!disciplinas) {
        printf("Erro ao carregar disciplinas.\n");
        return 1;
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
