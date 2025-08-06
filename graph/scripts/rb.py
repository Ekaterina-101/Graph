import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

try:
    df = pd.read_csv('../results_regular_graph.txt')
    print("Данные успешно загружены:")
    print(df.head())
except FileNotFoundError:
    print("Файл не найден. Убедитесь, что путь правильный.")
    exit()

plt.figure(figsize=(10, 6))

plt.plot(df['probability'], df['P_red'], 'ro-', label='Вероятность что красный граф удовл. усл.', linewidth=2, markersize=6)
plt.plot(df['probability'], df['P_blue'], 'bo-', label='Вероятность что среди синих графов (подграфы красного) найден граф, удовл. усл.', linewidth=2, markersize=6)

plt.xlabel('Вероятность p', fontsize=12)
plt.ylabel('Вероятность', fontsize=12)
plt.title('Вероятность существования графов с заданными свойствами', fontsize=14)
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.xlim(0, 1)

plt.xticks(np.arange(0, 1.1, 0.1))
plt.yticks(np.arange(0, 1.1, 0.1))

for i, row in df.iterrows():
    plt.annotate(f'{row["P_red"]:.3f}',
                 (row['probability'], row['P_red']),
                 textcoords="offset points",
                 xytext=(0, 10),
                 ha='center',
                 fontsize=8,
                 color='red')

    plt.annotate(f'{row["P_blue"]:.3f}',
                 (row['probability'], row['P_blue']),
                 textcoords="offset points",
                 xytext=(0, -15),
                 ha='center',
                 fontsize=8,
                 color='blue')

plt.tight_layout()
plt.show()
