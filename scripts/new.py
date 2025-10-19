import matplotlib.pyplot as plt
import pandas as pd
import re

def parse_csv_file(filename):
    df = pd.read_csv(filename)
    return df

filename_degree3 = "../data/results/v8-d3-resultsFormula.csv"
filename_degree4 = "../data/results/v8-d4-resultsFormula.csv"

try:
    df3 = parse_csv_file(filename_degree3)
    df4 = parse_csv_file(filename_degree4)
except FileNotFoundError as e:
    print(f"Ошибка: файл не найден - {e}")
    print("Пожалуйста, проверьте имена файлов и пути к ним.")
    exit(1)

plt.figure(figsize=(10, 8))

plt.plot(df3['p'], df3['All Conditions'],
         label='Degree <= 3',
         marker='o',
         linestyle='-',
         color='orange',
         linewidth=2,
         markersize=3)

plt.plot(df4['p'], df4['All Conditions'],
         label='Degree <= 4',
         marker='s',
         linestyle='-',
         color='blue',
         linewidth=2,
         markersize=3)

plt.title("Graph Properties (V = 8)", fontsize=14, pad=20)
plt.xlabel("p", fontsize=12)
plt.ylabel("Probability of All Conditions", fontsize=12)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=12)
plt.xlim(0, 1)
plt.ylim(0, 0.1)

plt.xticks([i / 10 for i in range(0, 11)])
plt.yticks([i / 100 for i in range(0, 11)])  # 0, 0.01, 0.02, ..., 0.1

plt.tight_layout()

output_filename = '../data/plots/comparison_degree3_4.png'
plt.savefig(output_filename, dpi=300, bbox_inches='tight')
plt.savefig(output_filename.replace('.png', '.pdf'), dpi=300, bbox_inches='tight')

plt.show()