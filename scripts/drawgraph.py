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


def read_edge_lists(filename):
    red_edges = []
    blue_edges = []
    current_section = None

    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if line == 'RED:':
                current_section = 'RED'
                continue
            elif line == 'BLUE:':
                current_section = 'BLUE'
                continue
            elif line == '' or ':' in line:
                continue

            if current_section and line:
                parts = line.split()
                if len(parts) == 2:
                    u, v = map(int, parts)
                    if current_section == 'RED':
                        red_edges.append((u, v))
                    elif current_section == 'BLUE':
                        blue_edges.append((u, v))

    return red_edges, blue_edges


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


def draw_graph(red_edges, blue_edges):
    # Создаём граф со всеми рёбрами для определения позиций
    all_edges = red_edges + blue_edges
    all_nodes = set()
    for u, v in all_edges:
        all_nodes.add(u)
        all_nodes.add(v)
    V = max(all_nodes) + 1 if all_nodes else 0

    # Создаём граф только с узлами
    G = nx.Graph()
    G.add_nodes_from(range(V))

    # Добавляем все рёбра для расчёта позиций
    G.add_edges_from(all_edges)

    fig, ax = plt.subplots(figsize=(5, 5))
    pos = nx.circular_layout(G)

    theta = math.pi - math.pi / (G.number_of_nodes())  # поворот на π - π/V радиан
    for node in pos:
        x, y = pos[node]
        pos[node] = (
            x * math.cos(theta) + y * math.sin(theta),
            x * math.sin(theta) - y * math.cos(theta)
        )

    # Только вершины
    nx.draw_networkx_nodes(G, pos, ax=ax, node_color='white', node_size=1800, edgecolors='black', linewidths=1.6)
    nx.draw_networkx_labels(G, pos, ax=ax, font_size=22, font_weight='medium', font_family=font_family)

    # Красные рёбра (видимость)
    if red_edges:
        RedGraph = nx.Graph()
        RedGraph.add_nodes_from(range(V))
        RedGraph.add_edges_from(red_edges)
        nx.draw_networkx_edges(RedGraph, pos, ax=ax, width=1.6, edge_color='red', style=':', arrows=True,
                               connectionstyle='arc3,rad=0.06')

    # Синие рёбра (виртуальные)
    if blue_edges:
        BlueGraph = nx.Graph()
        BlueGraph.add_nodes_from(range(V))
        BlueGraph.add_edges_from(blue_edges)
        nx.draw_networkx_edges(BlueGraph, pos, ax=ax, width=1.6, edge_color='blue', style='-')

    # Убрать рамку
    for spine in ax.spines.values():
        spine.set_visible(False)

    # Легенда
    legend_elements = [
        Line2D([0], [0], color='red', lw=1.6, linestyle=':', label='Visible'),
        Line2D([0], [0], color='blue', lw=1.6, linestyle='-', label='Virtual'),
    ]
    ax.legend(handles=legend_elements, loc='lower center',
              bbox_to_anchor=(0.5, 1.02), ncols=2, fontsize='small')

    # Текст под графом
    max_deg, diameter = analyze_graph(G)
    AP = list(nx.articulation_points(G))
    B = list(nx.bridges(G))
    if AP or B:
        fig.text(0.5, 0.06,
                 f"{G.number_of_nodes()} nodes, {G.number_of_edges()} edges, degree ≤ {max_deg}, diameter = {diameter}",
                 ha='center')
        fig.text(0.5, 0.02, f"AP : {AP}, B : {B}", ha='center')
    else:
        fig.text(0.5, 0.02,
                 f"{G.number_of_nodes()} nodes, {G.number_of_edges()} edges, degree ≤ {max_deg}, diameter = {diameter}, no AP/B",
                 ha='center')

    # plt.tight_layout()

    # Сохранение в файл
    filename = str(f"G_v{V}_d{diameter}_deg{max_deg}.png")
    plt.savefig(filename, format='png', transparent=True, dpi=300)
    print(f"Граф сохранён в файл '{filename}'")

    plt.show()


if __name__ == "__main__":
    filename = "edges.txt"
    red_edges, blue_edges = read_edge_lists(filename)
    draw_graph(red_edges, blue_edges)