import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Пути к файлу
file_path = '../results_regular_graph.txt'

# Ожидаемые колонки
expected_columns = [
    'probability', 'connected', 'no_articulation_points',
    'degree_limited', 'all_conditions', 'has_valid_subgraph',
    'avr_blue_conn', 'avr_blue_apb', 'avr_blue_deg', 'avr_blue_all'
]

try:
    # Читаем файл с гибким разделителем
    df = pd.read_csv(
        file_path,
        sep=r',\s*|\s+,',
        engine='python',
        skipinitialspace=True,
        na_values=['-nan', 'nan', 'inf', '-inf'],
        comment=None
    )

    # Очищаем имена колонок
    df.columns = df.columns.str.strip()

    # Проверка колонок
    if list(df.columns)[:len(expected_columns)] != expected_columns:
        print("⚠️ Имена колонок не совпадают:")
        print("Ожидалось:", expected_columns)
        print("Получено:", list(df.columns))
        if len(df.columns) >= 6:
            df = df.iloc[:, :10]
            df.columns = expected_columns
            print("✅ Колонки переименованы вручную.")
        else:
            raise ValueError("Недостаточно колонок в файле.")

    # Оставляем только нужные
    cols_to_keep = ['probability', 'connected', 'has_valid_subgraph']
    df = df[cols_to_keep]

    # Конвертируем в числа, заменяя NaN на 0
    for col in df.columns:
        if col != 'probability':
            df[col] = pd.to_numeric(df[col], errors='coerce').fillna(0)

    print("✅ Данные успешно загружены и очищены (NaN заменены на 0):")
    print(df.head(10))

except FileNotFoundError:
    print("❌ Файл не найден: '../results_regular_graph.txt'")
    exit()
except Exception as e:
    print(f"❌ Ошибка при загрузке данных: {e}")
    exit()

# Сортируем по вероятности
df = df.sort_values('probability').reset_index(drop=True)

# === Построение графика ===
plt.figure(figsize=(16, 10))

# Линии
line_configs = [
    {
        'column': 'connected',
        'color': 'red',
        'label': 'The "red" graph is connected',
        'linestyle': '-',
        'marker': 'o',
        'linewidth': 2.5,
        'markersize': 7,
        'alpha': 0.95
    },
    {
        'column': 'has_valid_subgraph',
        'color': 'blue',
        'label': 'Among the subgraphs of the "red" graph there is a "blue" graph',
        'linestyle': '--',
        'marker': 'o',
        'linewidth': 2.5,
        'markersize': 7,
        'alpha': 0.9
    }
]

# Строим линии
for config in line_configs:
    col = config['column']
    plt.plot(
        df['probability'], df[col],
        color=config['color'],
        linestyle=config['linestyle'],
        marker=config['marker'],
        label=config['label'],
        linewidth=config['linewidth'],
        markersize=config['markersize'],
        alpha=config['alpha']
    )

# Добавляем значения вероятностей рядом с точками
for idx, row in df.iterrows():
    p = row['probability']
    y_red = row['connected']
    y_blue = row['has_valid_subgraph']

    # Красная точка
    if y_red > 0:
        plt.annotate(f'{y_red:.3f}',
                     xy=(p, y_red), xytext=(5, 5), textcoords='offset points',
                     fontsize=8, color='red', ha='left', va='bottom')

    # Синяя точка
    if y_blue > 0:
        plt.annotate(f'{y_blue:.3f}',
                     xy=(p, y_blue), xytext=(5, -5), textcoords='offset points',
                     fontsize=8, color='blue', ha='left', va='top')

# Оформление
plt.xlabel('Share of visibility $\\alpha$', fontsize=14)
plt.ylabel('Probability', fontsize=14)
plt.title('Probability Estimation (8 Vertices)', fontsize=16)

# Компактная легенда (внутри графика, справа)
plt.legend(fontsize=15, loc='lower right', bbox_to_anchor=(0.98, 0.02), borderaxespad=0.)

plt.grid(True, alpha=0.3, linestyle='--', linewidth=0.8)
plt.yscale('linear')
plt.ylim(0, 1.05)
plt.xlim(0, 1)
plt.xticks(np.arange(0, 1.1, 0.1))
plt.yticks(np.arange(0, 1.1, 0.1))

plt.tight_layout()

# Сохраняем
plt.savefig('../graph_results_connected_and_blue_linear_with_values.png', dpi=300, bbox_inches='tight')
print("✅ График сохранён как '../graph_results_connected_and_blue_linear_with_values.png'")

plt.show()

# Вывод таблицы
print("\n📊 Числовые данные:")
print(df.to_string(index=False, float_format="%.4f"))