# gerador_grafos_conflito.py

# --- PASSO 1: DEFINICAO DOS DADOS ORIGINAIS ---

disciplinas_principais = {
    'Redes': {'semestre': 6},
    'CG': {'semestre': 6},
    'PCD': {'semestre': 6},
    'ES': {'semestre': 6},
    'Compiladores': {'semestre': 6},
    'PAA': {'semestre': 4},
    'AEDI': {'semestre': 2},
    'POO': {'semestre': 4},
    'BD': {'semestre': 4}
}

professores_aptos = {
    'Joahannes': ['Redes'],
    'Ana_Luiza': ['CG'],
    'Regina': ['CG', 'AEDI'],
    'Alvaro': ['PCD', 'AEDI'],
    'Fagundes': ['ES', 'POO'],
    'Otavio': ['ES', 'POO'],
    'Sanderson': ['PAA'],
    'Reginaldo': ['PAA'],
    'Rodrigo': ['Compiladores', 'POO'],
    'Galvao': ['Compiladores'],
    'Luis': ['Compiladores'],
    'Musa': ['BD']
}


# --- PASSO 2: CALCULAR O GRAFO DE CONFLITOS COMPLETO ---
# Esta é a fonte de verdade para ambos os arquivos .dot

print("Calculando o grafo de conflitos completo...")

# 1. Gera todos os vértices (sessões de aula)
sessoes_de_aula = sorted([f"{d}_{i}" for d in disciplinas_principais for i in [1, 2]])

# 2. Encontra todas as arestas de conflito
conflitos = set()
for i in range(len(sessoes_de_aula)):
    for j in range(i + 1, len(sessoes_de_aula)):
        v1_nome = sessoes_de_aula[i]
        v2_nome = sessoes_de_aula[j]
        
        d1_base = v1_nome.split('_')[0]
        d2_base = v2_nome.split('_')[0]

        # Regra 1: Conflito Interno (mesma disciplina)
        if d1_base == d2_base:
            conflitos.add(tuple(sorted((v1_nome, v2_nome))))
            continue

        # Regra 2: Conflito de Período
        if disciplinas_principais[d1_base]["semestre"] == disciplinas_principais[d2_base]["semestre"]:
            conflitos.add(tuple(sorted((v1_nome, v2_nome))))
            continue
            
        # Regra 3: Conflito de Professor Potencial
        professores_d1 = {p for p, aptidoes in professores_aptos.items() if d1_base in aptidoes}
        professores_d2 = {p for p, aptidoes in professores_aptos.items() if d2_base in aptidoes}
        if not professores_d1.isdisjoint(professores_d2):
            conflitos.add(tuple(sorted((v1_nome, v2_nome))))

print(f"...Calculado! Total de {len(sessoes_de_aula)} sessoes de aula e {len(conflitos)} conflitos potenciais.")


# --- PASSO 3: GERAR O GRAFO NUMERICO PARA O MENU ---

def gerar_grafo_para_menu(filename="grafo_conflitos_menu.dot"):
    print(f"Gerando '{filename}' (para logica do menu)...")
    
    map_nome_id = {nome: i for i, nome in enumerate(sessoes_de_aula)}

    with open(filename, "w") as f:
        f.write("graph G {\n")
        f.write("  // Nos sao as SESSOES DE AULA, com labels para identificacao\n")
        for nome, id_num in map_nome_id.items():
            f.write(f'  {id_num} [label="{nome}"];\n')
        
        f.write("\n  // Arestas representam TODOS os conflitos (periodo, professor ou interno)\n")
        for v1_nome, v2_nome in sorted(list(conflitos)):
            id1 = map_nome_id[v1_nome]
            id2 = map_nome_id[v2_nome]
            f.write(f"  {id1} -- {id2};\n")
            
        f.write("}\n")
    print("...Concluido.")


# --- PASSO 4: GERAR O GRAFO VISUAL COM CLUSTERS ---

def gerar_grafo_para_visualizacao(filename="grafo_conflitos_visual.dot"):
    print(f"Gerando '{filename}' (para visualizacao)...")
    
    # Agrupa as sessoes por disciplina para criar os clusters
    sessoes_por_disciplina = {}
    for sessao in sessoes_de_aula:
        disciplina_base = sessao.split('_')[0]
        if disciplina_base not in sessoes_por_disciplina:
            sessoes_por_disciplina[disciplina_base] = []
        sessoes_por_disciplina[disciplina_base].append(sessao)

    with open(filename, "w") as f:
        f.write("graph G {\n")
        f.write("  // Layout e estilo geral\n")
        f.write("  layout=sfdp;\n")
        f.write("  overlap=false;\n")
        f.write("  node [shape=ellipse, style=filled, color=lightgray];\n\n")

        # Escreve um subgraph para cada disciplina
        for disciplina, sessoes in sessoes_por_disciplina.items():
            f.write(f"  subgraph cluster_{disciplina} {{\n")
            f.write(f'    label = "Disciplina: {disciplina}";\n')
            f.write(f'    style = filled;\n')
            f.write(f'    color = lightgreen;\n')
            # Define os nós dentro do cluster
            for sessao in sessoes:
                f.write(f'    "{sessao}";\n')
            f.write("  }\n\n")
        
        # Escreve todas as arestas de conflito
        f.write("  // Arestas representam TODOS os conflitos\n")
        for v1_nome, v2_nome in sorted(list(conflitos)):
            # Usa aspas para garantir que os nomes sejam tratados como strings
            f.write(f'  "{v1_nome}" -- "{v2_nome}";\n')

        f.write("}\n")
    print("...Concluido.")


# --- EXECUCAO ---
if __name__ == "__main__":
    gerar_grafo_para_menu()
    print("-" * 20)
    gerar_grafo_para_visualizacao()
    print("\nDois novos arquivos de grafo de conflitos foram gerados com sucesso!")