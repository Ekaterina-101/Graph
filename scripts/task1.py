import matplotlib.pyplot as plt
import os
import csv
from collections import defaultdict

from matplotlib import ticker


def analyze_graph_data(n, degree=3):
    """
    Анализирует данные для графа с n вершинами и заданной степенью.
    Возвращает словарь с результатами, основанный на строках 'All Conditions'.
    """
    filename = f"../data/results/v{n}-d{degree}-detailedStats.csv"

    if not os.path.exists(filename):
        print(f"Файл не найден: {filename}")
        return None

    # Словарь для хранения суммарных данных по диаметрам для 'All Conditions'
    diameter_counts = defaultdict(int)
    total_count = 0
    min_diameter = float('inf')
    max_diameter = 0

    with open(filename, 'r', newline='', encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            # Фильтруем только строки 'All Conditions'
            if row['Type'] == "All Conditions":
                diameter = int(row['Diameter'])
                count = int(row['Count'])

                # Суммируем количество графов для каждого диаметра
                diameter_counts[diameter] += count
                total_count += count

                # Отслеживаем минимальный и максимальный диаметр
                if diameter < min_diameter:
                    min_diameter = diameter
                if diameter > max_diameter:
                    max_diameter = diameter

    # Проверяем, были ли найдены какие-либо данные 'All Conditions'
    if total_count == 0:
        print(f"Нет данных для 'All Conditions' при n={n} в файле {filename}")
        return None

    # 1. d_min(n) - минимальный диаметр среди всех графов 'All Conditions'
    d_min = min_diameter

    # 2. count_dmin(n) - количество графов с диаметром d_min
    count_dmin = diameter_counts[d_min]

    # 3. d_avg(n) - средний диаметр
    # Сумма (диаметр * количество) / общее количество
    weighted_sum = sum(d * c for d, c in diameter_counts.items())
    d_avg = weighted_sum / total_count if total_count > 0 else 0

    # 4. count_full(n) - общее количество графов 'All Conditions'
    count_full = total_count

    return {
        'n': n,
        'd_min': d_min,
        'd_avg': d_avg,
        'count_dmin': count_dmin,
        'count_full': count_full,
        'diameter_distribution': dict(diameter_counts)  # Опционально: для отладки или детального анализа
    }


def run_generator_for_n(n, degree=3):
    """
    Запускает вашу программу generator для заданного n.
    Использует команду: ./generator --vertices n --degree degree
    """
    command = f"./../build/generator --vertices {n} --degree {degree}"
    print(f"Запуск: {command}")
    result = os.system(command)
    if result != 0:
        print(f"Ошибка при запуске для n={n} (код возврата {result}).")
        return False
    print(f"Успешно завершён запуск для n={n}.")
    return True


def main():
    # Диапазон n для анализа
    n_values = [4, 5, 6, 7]  # Расширил до 10, как в задаче
    degree = 3  # Степень, используемая в ваших экспериментах

    results = []

    for n in n_values:
        print(f"\n--- Обработка n={n} ---")

        # Проверяем, существует ли файл с результатами для n
        output_file = f"../data/results/v{n}-d{degree}-detailedStats.csv"

        # Если файл не существует, запускаем генератор
        if not os.path.exists(output_file):
            print(f"Файл {output_file} не найден. Запускаю generator...")
            success = run_generator_for_n(n, degree)
            if not success:
                print(f"Пропускаю n={n} из-за ошибки запуска.")
                continue  # Переходим к следующему n
        else:
            print(f"Файл {output_file} уже существует. Пропускаю запуск generator.")
        success = run_generator_for_n(n, degree)
        # Анализируем данные из файла
        data = analyze_graph_data(n, degree)
        if data is not None:
            results.append(data)
            print(f"  d_min({n}) = {data['d_min']}")
            print(f"  d_avg({n}) = {data['d_avg']:.4f}")
            print(f"  count_dmin({n}) = {data['count_dmin']}")
            print(f"  count_full({n}) = {data['count_full']}")
        else:
            print(f"Не удалось получить данные для n={n}.")

    # --- Вывод результатов ---
    print("\n" + "=" * 100)
    print("Сводка по анализу:")
    print("=" * 100)
    # Заголовок таблицы
    print(f"{'n':<5} {'d_min':<8} {'d_avg':<12} {'count_dmin':<12} {'count_full':<12}")
    print("-" * 100)
    for res in results:
        print(f"{res['n']:<5} {res['d_min']:<8} {res['d_avg']:<12.4f} {res['count_dmin']:<12} {res['count_full']:<12}")

    # --- Построение графиков ---

    # Извлекаем данные для графиков
    n_list = [res['n'] for res in results]
    d_min_list = [res['d_min'] for res in results]
    d_avg_list = [res['d_avg'] for res in results]
    count_dmin_list = [res['count_dmin'] for res in results]
    count_full_list = [res['count_full'] for res in results]

    # Создаём фигуру с двумя подграфиками
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Левый график: d_min(n) и d_avg(n)
    ax1.plot(n_list, d_min_list, marker='o', linestyle='-', linewidth=2, label='$d_{min}(n)$', color='blue')
    ax1.plot(n_list, d_avg_list, marker='s', linestyle='--', linewidth=2, label='$d_{avg}(n)$', color='orange')
    ax1.set_xlabel('Число вершин $n$')
    ax1.set_ylabel('Диаметр')
    ax1.set_title('Минимальный и средний диаметр подграфов')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))

    # Правый график: count_dmin(n) и count_full(n)
    ax2.plot(n_list, count_dmin_list, marker='o', linestyle='-', linewidth=2, label='$count_{dmin}(n)$', color='blue')
    ax2.plot(n_list, count_full_list, marker='s', linestyle='--', linewidth=2, label='$count_{full}(n)$', color='orange')
    ax2.set_xlabel('Число вершин $n$')
    ax2.set_ylabel('Количество подграфов')
    ax2.set_title('Количество подграфов с минимальным диаметром и общее количество')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))

    # Делаем макет более красивым
    plt.tight_layout()

    # Сохраняем график в файл
    plot_filename = "analysis_plots.png"
    plt.savefig(plot_filename, dpi=300, bbox_inches='tight')
    print(f"\nГрафики сохранены в файл: {plot_filename}")

    # Показываем график (если запускается интерактивно)
    plt.show()


if __name__ == "__main__":
    main()