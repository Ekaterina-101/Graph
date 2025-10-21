import pandas as pd
import matplotlib.pyplot as plt

# Загрузка данных из файла
file_path = '../results_regular_graph_8v02.txt'
data = pd.read_csv(file_path)

# Извлечение нужных столбцов
alpha = data['probability']  # Это ваш α (доля видимости)
d_visAvg = data['d_visAvg']
d_avgMin = data['d_avgMin']
d_avgAvg = data['d_avgAvg']

# Построение графика
plt.figure(figsize=(10, 6))

# Красная линия: d_visAvg(α)
plt.plot(alpha, d_visAvg, color='red', linewidth=2, label=r'$d_{visAvg}(\alpha)$')

# Синяя сплошная линия: d_avgMin(α)
plt.plot(alpha, d_avgMin, color='blue', linewidth=2, label=r'$d_{avgMin}(\alpha)$')

# Синяя пунктирная линия: d_avgAvg(α)
plt.plot(alpha, d_avgAvg, color='blue', linewidth=2, linestyle='--', label=r'$d_{avgAvg}(\alpha)$')

# Настройки графика
plt.xlabel(r'$\alpha$')  # Ось X - доля видимости
plt.ylabel('Diameter')   # Ось Y - диаметр
plt.title('Average Diameter vs. Visibility Ratio ($\\alpha$)')
plt.legend()
plt.grid(True, alpha=0.3)
plt.xlim(0.3, 1.0)  # Установка границ по оси X, как на вашем рисунке
plt.ylim(bottom=0)   # Начинаем с нуля на оси Y

# Отображение графика
plt.show()