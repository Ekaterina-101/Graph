import networkx as nx
import math 
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
from matplotlib.lines import Line2D

# Шрифт по умолчанию
fallback_font = 'DejaVu Sans'
preferred_font = 'HSE Sans'

# Проверяем наличие шрифта
available_fonts = set(f.name for f in fm.fontManager.ttflist)
if preferred_font in available_fonts:
    font_family = preferred_font
    font_size = 14 
else:
    font_family = fallback_font
    font_size = 12

# Применяем шрифт и размер
plt.rcParams['font.family'] = font_family
plt.rcParams['font.size'] = font_size
plt.rcParams['font.weight'] = 'regular'



def read_edge_list(filename):
    edges = []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 2:
                u, v = map(int, parts)
                edges.append((u, v))
    return edges



def analyze_graph(G):
    # Максимальная степень
    degrees = dict(G.degree())
    max_deg = max(degrees.values())
    # max_deg_nodes = [node for node, deg in degrees.items() if deg == max_deg]

    # Диаметр
    if nx.is_connected(G):
        diameter = nx.diameter(G)
    else:
        diameter = None

    return max_deg, diameter



def draw_graph(edges):
    V = max(max(u, v) for u, v in edges)+1
    Visible = nx.gnp_random_graph(V, p=0.15)
    Visible.add_edges_from(edges)


    fig, ax = plt.subplots(figsize=(5, 5))
    pos = nx.circular_layout(Visible)

    theta = math.pi - math.pi/(Visible.number_of_nodes())  # поворот на π - π/V радиан 
    for node in pos:
        x, y = pos[node]
        pos[node] = (
            x * math.cos(theta) + y * math.sin(theta),
            x * math.sin(theta) - y * math.cos(theta)
        )
   

    # Только вершины
    nx.draw_networkx_nodes(Visible, pos, ax = ax, node_color='white', node_size=1800, edgecolors='black', linewidths=1.6)
    nx.draw_networkx_labels(Visible, pos, ax = ax, font_size=22, font_weight='medium', font_family=font_family)

    # Красные рёбра (видимость)
    #nx.draw_networkx_edges(Visible, pos, ax=ax, width=1.6, edge_color='red', style=':', arrows=True, connectionstyle='arc3,rad=0.06')

    # Синие рёбра (виртуальные)
    Virtual = nx.create_empty_copy(Visible)
    Virtual.add_edges_from(edges)
    nx.draw_networkx_edges(Virtual, pos, ax=ax, width=1.6, edge_color='blue', style='-')

    # Чёрные рёбра (связь)
    #Black = nx.create_empty_copy(Visible)
    #Black.add_edges_from([(0,4), (0,7), (1,2), (2,6), (5,6)])
    #nx.draw_networkx_edges(Black, pos, ax=ax, width=1.6, edge_color='black', style='--', arrows=True, connectionstyle='arc3,rad=-0.06')

    # Убрать рамку
    for spine in ax.spines.values():
        spine.set_visible(False)

    # Легенда
    legend_elements = [
        Line2D([0], [0], color='red', lw=1.6, linestyle=':', label='Visible'),
        Line2D([0], [0], color='blue', lw=1.6, linestyle='-', label='Virtual'),
        Line2D([0], [0], color='black',   lw=1.6, linestyle='--', label='Connection'),
    ]
    ax.legend(handles=legend_elements, loc='lower center',
          bbox_to_anchor=(0.5, 1.02), ncols=3, fontsize='small')    


    # Текст под графом
    max_deg, diameter = analyze_graph(Virtual)
    AP = list(nx.articulation_points(Virtual))
    B = list(nx.bridges(Virtual))
    if AP or B:
        fig.text(0.5, 0.06, f"{Virtual.number_of_nodes()} nodes, {Virtual.number_of_edges()} edges, degree ≤ {max_deg}, diameter = {diameter}",
              ha='center')
        fig.text(0.5, 0.02, f"AP : {AP}, B : {B}", ha='center')
    else:
        fig.text(0.5, 0.02, f"{Virtual.number_of_nodes()} nodes, {Virtual.number_of_edges()} edges, degree ≤ {max_deg}, diameter = {diameter}, no AP/B",
              ha='center')
        
    # plt.tight_layout()

    # Сохранение в файл 
    filename = str(f"G_v{V}_d{diameter}_deg{max_deg}.png")
    plt.savefig(filename, format='png', transparent=True, dpi=300)
    print(f"Граф сохранён в файл '{filename}'")

    plt.show()



if __name__ == "__main__":
    filename = "G_v8_d3_deg3.txt"
    edges = read_edge_list(filename)
    draw_graph(edges)
