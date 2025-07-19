#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

int main() {
    FILE *file = fopen("grafo.dot", "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char line[MAX_LINE];

    printf("Professores encontrados:\n\n");

    while (fgets(line, MAX_LINE, file)) {
        // Verifica se a linha começa com 'P' seguido de dígito (ex: P1, P2 ...)
        if (line[0] == 'P' && (line[1] >= '0' && line[1] <= '9')) {
            // Procura pela string label= para pegar o conteúdo dentro das aspas
            char *start = strstr(line, "label=");
            if (start) {
                start += strlen("label=");
                // pula espaços em branco até encontrar a aspa
                while (*start == ' ' || *start == '\t') start++;

                if (*start == '"') {
                    start++; // pula a aspa inicial

                    // Agora copia até a aspa final (antes do ']')
                    char *end = strchr(start, '"');
                    if (end) {
                        int length = end - start;
                        char professorInfo[MAX_LINE];
                        strncpy(professorInfo, start, length);
                        professorInfo[length] = '\0';

                        // Trocar "\n" por nova linha no printf
                        for (int i = 0; i < length; i++) {
                            if (professorInfo[i] == '\\' && professorInfo[i+1] == 'n') {
                                professorInfo[i] = '\n';
                                // remove o 'n' seguinte
                                for (int j = i + 1; j < length; j++) {
                                    professorInfo[j] = professorInfo[j+1];
                                }
                                length--;
                            }
                        }

                        printf("%s\n\n", professorInfo);
                    }
                }
            }
        }
    }

    fclose(file);
    return 0;
}
