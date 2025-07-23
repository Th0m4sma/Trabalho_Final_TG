#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "interacoes.h"
#include "interface.h"




int main() {
    bool sair = false;
    int opcao;
    int num;
    FILE *file;
    bool grafo_inserido = false;


    file = fopen("grafo.dot", "r");
    if (file != NULL) {
        fclose(file);
        grafo_inserido = true;
    }


    if (!grafo_inserido) {
            menu_inicial1();
            printf("Digite a opcao preferida: ");
            scanf("%d", &num);

            switch (num) {
                case 1:
                    entrada_grafo_manual();
                    grafo_inserido = true;
                    break;
                case 2: {
                    char nome_arquivo[100];
                    FILE *origem, *destino;
                    char ch;

                    printf("Digite o nome do arquivo (com extensao .dot): ");
                    scanf(" %99s", nome_arquivo);

                    origem = fopen(nome_arquivo, "r");
                    if (origem == NULL) {
                        printf("Arquivo nao encontrado: %s\n", nome_arquivo);
                    } else {
                        destino = fopen("grafo.dot", "w");
                        if (destino == NULL) {
                            printf("Erro ao criar o arquivo grafo.dot\n");
                        } else {
                            while ((ch = fgetc(origem)) != EOF) {
                                fputc(ch, destino);
                            }
                            printf("Arquivo '%s' copiado com sucesso como 'grafo.dot'.\n", nome_arquivo);
                            fclose(destino);
                            grafo_inserido = true;
                        }
                        fclose(origem);
                    }

                    getchar();getchar();
                    break;
                }
                case 3:
                    sair = true;
                    break;
            }
    }else {
        while(!sair) {
            limpar_terminal();
            imprimir_banner();
            
            printf("Digite a opção desejada: ");
            scanf("%d",&opcao);

            switch(opcao) {
                case 1:
                    limpar_terminal();
                    resumo_geral();
                    getchar();getchar();
                    break;
                case 2:
                    limpar_terminal();

                    break;
                case 3:
                    limpar_terminal();

                    break;
                case 4:
                    limpar_terminal();
                    operacoes();
                    getchar();getchar();
                    break;
                case 5:
                    remove("grafo.dot");
                    grafo_inserido = false;
                    break;
            }

        }
    }
    
    return 0;
}

/*
int funcao_principal() {
    system("chcp 65001 > nul"); 

    bool sair = false;
    int num;
    FILE *file;
    bool grafo_inserido = false;

    while (!sair) {
        system("clear");

        // Verifica se já existe um arquivo de grafo
        file = fopen("grafo.dot", "r");
        if (file != NULL) {
            fclose(file);
            grafo_inserido = true;
        }

        if (!grafo_inserido) {
            menu_inicial1();
            printf("Digite a opcao preferida: ");
            scanf("%d", &num);

            switch (num) {
                case 1:
                    entrada_grafo_manual();
                    grafo_inserido = true;
                    break;
                case 2: {
                    char nome_arquivo[100];
                    FILE *origem, *destino;
                    char ch;

                    printf("Digite o nome do arquivo (com extensao .dot): ");
                    scanf(" %99s", nome_arquivo);

                    origem = fopen(nome_arquivo, "r");
                    if (origem == NULL) {
                        printf("Arquivo nao encontrado: %s\n", nome_arquivo);
                    } else {
                        destino = fopen("grafo.dot", "w");
                        if (destino == NULL) {
                            printf("Erro ao criar o arquivo grafo.dot\n");
                        } else {
                            while ((ch = fgetc(origem)) != EOF) {
                                fputc(ch, destino);
                            }
                            printf("Arquivo '%s' copiado com sucesso como 'grafo.dot'.\n", nome_arquivo);
                            fclose(destino);
                            grafo_inserido = true;
                        }
                        fclose(origem);
                    }

                    getchar();getchar();
                    break;
                }
                case 3:
                    sair = true;
                    break;
            }
        } else {

            // Novo menu quando o grafo já está presente
            menu_principal();
            printf("Digite a opcao preferida: ");
            scanf("%d", &num);

            switch (num) {
                case 1:
                    resumo_geral();
                    getchar();getchar();
                    break;
                case 2:
                    exibirMatrizAdjacente();
                    getchar();getchar();
                    break;
                case 3:
                    exibirMatrizIncidencia();
                    getchar();getchar();
                    break;
                case 4:
                    exibirListaDeAdjacencia();
                    getchar();getchar();
                    break;
                case 5:
                    buscando();
                    getchar();getchar();
                    break;
                case 6:
                    menuVerificacaoSubgrafo();
                    getchar();getchar();
                    break;
                case 7:
                    verificarCicloConjunto();
                    getchar();getchar();
                    break;
                case 8:
                    executar_menu_transformacao();
                    getchar();getchar();
                    break;
                case 9:
                    printf("Os grafos sao isomorfos? %s\n",verificaIsomorfismo() ? "Sim" : "Nao");
                    sleep(4);
                    getchar();getchar();
                    break;

                case 10:
                    system("clear");
                    operacoes();
                    getchar();getchar();
                    break;
                case 11:
                    system("clear");
                    arvores();
                case 12:
                    remove("grafo.dot");
                    grafo_inserido = false;
                    break;
                case 13:
                    system("clear");
                    agendador();
                    getchar();getchar();
                    break;
                case 14:
                    sair = true;
                    break;
                default:
                    printf("Opçao invalida.\n");
                    getchar();getchar();
                    break;

            }
        }
    }

    system("clear");
    return 0;
}
*/










