import matplotlib.pyplot as plt
import numpy as np

num_vertices = []
probability_1 = []
probability_2 = []
probability_3 = []

with open('../graphic.txt', 'r') as file:
    for line in file:
        values = line.strip().split(',')
        num_vertices.append(int(values[0]))
        probability_1.append(float(values[1]))
        probability_2.append(float(values[2]))
        probability_3.append(float(values[3]))

theoretical_3 = []
theoretical_4 = []
theoretical_5 = []

for v in num_vertices:
    if v > 1:
        theoretical_3.append(3 / (v - 1))
        theoretical_4.append(4 / (v - 1))
        theoretical_5.append(5 / (v - 1))
    else:
        theoretical_3.append(0)
        theoretical_4.append(0)
        theoretical_5.append(0)

plt.plot(num_vertices, probability_1, label="degree <= 3 (Real)", marker='o')
plt.plot(num_vertices, probability_2, label="degree <= 4 (Real)", marker='o')
plt.plot(num_vertices, probability_3, label="degree <= 5 (Real)", marker='o')

#plt.plot(num_vertices, theoretical_3, label="degree <= 3 (Formula)", linestyle='--', marker='x')
#plt.plot(num_vertices, theoretical_4, label="degree <= 4 (Formula)", linestyle='--', marker='x')
#plt.plot(num_vertices, theoretical_5, label="degree <= 5 (Formula)", linestyle='--', marker='x')

plt.xlabel("Число вершин графа")
plt.ylabel("Вероятность p")
plt.title("Зависимость вероятности от числа вершин графа и теоретические линии")

plt.xlim(0, 10)
plt.ylim(0, 1)

plt.xticks(np.arange(0, 11, 1))
plt.yticks(np.arange(0, 1.1, 0.1))

plt.legend()
plt.grid(True)
plt.show()
