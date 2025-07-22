# gerador_grafo.py (VERSÃO ATUALIZADA)

import collections

# --- Dados (os mesmos de antes)
disciplinas_periodos = {
    'Redes': 6, 'CG': 6, 'PCD': 6, 'ES': 6, 'Compiladores': 6,
    'PAA': 4, 'POO': 4, 'BD': 4, 'AEDI': 2
}

professores_disciplinas = {
    'Redes': ['Joahannes'], 'CG': ['Ana_Luiza', 'Regina'], 'PCD': ['Alvaro'],
    'ES': ['Fagundes', 'Otavio'], 'PAA': ['Sanderson', 'Reginaldo'],
    'Compiladores': ['Rodrigo', 'Galvao', 'Luis'], 'AEDI': ['Regina', 'Alvaro'],
    'POO': ['Fagundes', 'Otavio', 'Rodrigo'], 'BD': ['Musa']
}

# --- Lógica de atribuição de professores (Fase 1)
professores_finais_atribuidos = {}
carga_professores = collections.defaultdict(int)
for prof_list in professores_disciplinas.values():
    for prof in prof_list: carga_professores[prof] += 1

disciplinas_ordenadas = sorted(disciplinas_periodos.keys(), key=lambda d: len(professores_disciplinas[d]))
for disciplina in disciplinas_ordenadas:
    opcoes_prof = professores_disciplinas[disciplina]
    if len(opcoes_prof) == 1:
        professor_escolhido = opcoes_prof[0]
    else:
        opcoes_prof_ordenados = sorted(opcoes_prof, key=lambda p: carga_professores[p])
        professor_escolhido = opcoes_prof_ordenados[0]
    professores_finais_atribuidos[disciplina] = professor_escolhido

# --- Lógica de geração de arestas
vertices = [f"{d}_{i}" for d in disciplinas_periodos for i in [1, 2]]
arestas = set()

for i in range(len(vertices)):
    for j in range(i + 1, len(vertices)):
        v1, v2 = vertices[i], vertices[j]
        d1_base, d2_base = v1.split('_')[0], v2.split('_')[0]

        conflito = False
        if d1_base == d2_base:
            conflito = True
        elif disciplinas_periodos[d1_base] == disciplinas_periodos[d2_base]:
            conflito = True
        elif professores_finais_atribuidos[d1_base] == professores_finais_atribuidos[d2_base]:
            conflito = True

        if conflito:
            aresta_ordenada = tuple(sorted((v1, v2)))
            arestas.add(aresta_ordenada)

# --- Escrita do arquivo .dot
with open("grafo_gerado.dot", "w") as f:
    f.write("graph G {\n")
    for v1, v2 in sorted(list(arestas)):
        f.write(f"    {v1} -- {v2};\n")
    f.write("}\n")

# --- NOVO: Escrita do arquivo de atribuições ---
with open("atribuicoes.txt", "w") as f:
    for disciplina, professor in professores_finais_atribuidos.items():
        f.write(f"{disciplina} {professor}\n")

print("Arquivos 'grafo_gerado.dot' e 'atribuicoes.txt' gerados com sucesso!")