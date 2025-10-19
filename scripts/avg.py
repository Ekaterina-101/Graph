import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Ожидаемые имена колонок
expected_columns = [
    'probability', 'connected', 'no_articulation_points',
    'degree_limited', 'all_conditions', 'has_valid_subgraph',
    'avr_blue_conn', 'avr_blue_apb', 'avr_blue_deg', 'avr_blue_all'
]

file_path = '../results_regular_graph.txt'

try:
    # Читаем файл с гибким разделителем
    df = pd.read_csv(
        file_path,
        sep=r',\s*|\s+,',  # Регулярное выражение: запятая + пробелы
        engine='python',  # Обязательно для regex
        names=expected_columns,
        skiprows=1,  # Пропускаем заголовок
        skipinitialspace=True,
        na_values=['-nan', 'nan', 'inf', '-inf'],
        keep_default_na=True
    )

    # Преобразуем колонки в числа
    for col in expected_columns:
        df[col] = pd.to_numeric(df[col], errors='coerce')

    # Сортируем по вероятности
    df = df.sort_values('probability').reset_index(drop=True)

    # Создаем копию для безусловных вероятностей
    df_unconditional = df.copy()

    # Умножаем avr_blue_all на вероятность связности красного графа
    df_unconditional['avr_blue_all'] = df_unconditional['avr_blue_all'] * df_unconditional['connected']

    print("✅ Данные загружены и отсортированы:")
    print(df_unconditional[['probability', 'avr_blue_all']].head(10))

except FileNotFoundError:
    print("❌ Файл '../results_regular_graph.txt' не найден.")
    exit()
except Exception as e:
    print(f"❌ Ошибка при чтении файла: {e}")
    exit()

# Проверка: есть ли данные?
if df_unconditional['avr_blue_all'].dropna().empty:
    print("❌ Нет данных в колонке avr_blue_all — проверьте файл.")
    exit()

# === Построение графика ===
plt.figure(figsize=(12, 7))

# Данные
data = df_unconditional[['probability', 'avr_blue_all']].dropna()

# Рисуем линию — синюю
line = plt.plot(
    data['probability'], data['avr_blue_all'],
    label='Average number of "blue" graphs',
    color='blue',
    linestyle='-',
    marker='o',
    markersize=6,
    linewidth=2,
    alpha=0.85
)

# Добавляем подписи значений над точками
for i, row in data.iterrows():
    x = row['probability']
    y = row['avr_blue_all']

    # Форматируем значение: если очень маленькое — экспоненциальный вид, иначе 2 знака
    if y < 0.01:
        label = f'{y:.1e}'  # например, 1.2e-03
    else:
        label = f'{y:.2f}'  # например, 0.45

    # Подписываем точку
    plt.annotate(
        label,
        (x, y),
        textcoords="offset points",
        xytext=(0, 8),  # выше точки на 8 пикселей
        ha='center',
        fontsize=9,
        color='darkblue',
        alpha=0.9,
        rotation=0
    )

# Оформление
plt.xlabel('Share of visibility $α$', fontsize=14)
plt.ylabel('Average number', fontsize=14)
plt.title('Average number of "blue" graphs per "red" graph (8 Vertices)', fontsize=16)

plt.legend(fontsize=12, loc='upper left')
plt.grid(True, alpha=0.3, linestyle='--', linewidth=0.7)

# Логарифмическая шкала по Y
plt.yscale('log')

# Ограничения по X
plt.xlim(0, 1)
plt.xticks(np.arange(0, 1.1, 0.1))

plt.tight_layout()

# Сохраняем
output_path = '../graph_avr_blue_subgraphs_log_unconditional.png'
plt.savefig(output_path, dpi=300, bbox_inches='tight')
print(f"✅ График сохранён как '{output_path}'")

plt.show()

# Вывод таблицы
print("\n📊 Числовые данные (среднее количество синих подграфов, все условия, безусловные):")
print(df_unconditional[['probability', 'avr_blue_all']].to_string(index=False, float_format="%.2f"))