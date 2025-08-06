import matplotlib.pyplot as plt
import pandas as pd
import re


def parse_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    n_line = lines[0].strip()
    number_of_vertices = int(n_line.split("=")[1].strip())

    data_dict = {}
    for line in lines[1:]:
        if not line.strip():
            continue
        parts = line.strip().split(" | ")
        for part in parts:
            key, value = part.split(" = ")
            value = float(value)
            if key not in data_dict:
                data_dict[key] = []
            data_dict[key].append(value)

    return number_of_vertices, pd.DataFrame(data_dict)


def parse_second_file(filename):
    df2 = pd.read_csv(filename)
    return df2


filename1 = "../results_regular_graph.txt"
#filename2 = "../data/results/v8-d4-resultsFormula.csv"

n, df = parse_file(filename1)
#df2 = parse_second_file(filename2)

degree = 3
#for col_name in df2.columns:
#    if "Degree" in col_name and "<=" in col_name:
#        match = re.search(r'<=\s*(\d+)', col_name)
#        if match:
#            degree = int(match.group(1))
#            break

color_map = {
    "P(Connected)": 'red',
    "P(No AP/B & connected)": 'green',
    "P(Degree <= 3 & connected)": 'blue',
    "P(No AP/B & deg ≤ 3 & connected)": 'orange'
}

color_map2 = {
    "Connected": 'red',
    "No AP/B & connected": 'green',
    f"Degree <= {degree} & connected": 'blue',
    "All Conditions": 'orange'
}

plt.figure(figsize=(10, 6))

for col in df.columns[1:]:
    plt.plot(
        df['p'], df[col],
        label=col,
        marker='o',
        linestyle='-',
        color=color_map.get(col, 'orange')
    )

#print("Available columns in CSV:", df2.columns.tolist())

#for col in df2.columns[1:]:
#    clean_col_name = col.strip()
#    print(f"Processing column: '{clean_col_name}'")
#    print(f"Color mapping: {color_map2.get(clean_col_name, 'purple')}")

#    plt.plot(
#        df2['p'], df2[col],
#        label=clean_col_name,
#        marker='o',
#        linestyle='-',
#        color=color_map2.get(clean_col_name, 'purple')
#    )

plt.title(f"Graph Properties (V = {n})")
plt.xlabel("p")
plt.ylabel("Probability")
plt.grid(True)
plt.legend()
plt.xlim(0, 1)
plt.ylim(0, 1)
plt.xticks([i / 10 for i in range(0, 11)])
plt.yticks([i / 10 for i in range(0, 11)])

plt.tight_layout()

filename = f'../data/plots/graph_plot_v{n}_d{degree}.png'
plt.savefig(filename, dpi=300, bbox_inches='tight')
# plt.savefig('graph_plot.pdf', dpi=300, bbox_inches='tight')
# plt.savefig('graph_plot.svg', dpi=300, bbox_inches='tight')

plt.show()