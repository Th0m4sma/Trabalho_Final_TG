#ifndef INTEFACE_H
#define INTERFACE_H

#include "interacoes.h"


void limpar_terminal();
void imprimir_banner();
void imprimir_linha_simples();
void imprimir_disciplinas_prof(professor* professores, int qtd_prof);
void imprimir_disciplinas_disc(disciplina* disciplinas, int qtd_disc);
void linha_final();
void menu_inicial1_teste();
void menu_principal_teste();
void menu_transformacao();
void menu_operacao();
void menu_arvores();

#endif