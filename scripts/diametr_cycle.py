# def display_grouped_by_t(filename):
#     """
#     Выводит на экран данные, сгруппированные по t
#     Сначала четные n, потом нечетные
#     """
#     with open(filename, 'r') as f:
#         content = f.read()
    
#     data_by_t = {}
#     current_n = None
#     current_t = None
    
#     # Парсинг
#     for line in content.split('\n'):
#         line = line.strip()
#         if not line:
#             continue
            
#         if line.startswith('n = '):
#             current_n = int(line.split('=')[1].strip())
#         elif line.startswith('t = '):
#             current_t = int(line.split('=')[1].strip())
#             if current_t not in data_by_t:
#                 data_by_t[current_t] = {}
#         elif line and current_n is not None and current_t is not None:
#             numbers = list(map(int, line.split()))
#             data_by_t[current_t][current_n] = numbers
    
#     # Вывод с четким разделением
#     for t in sorted(data_by_t.keys()):
#         print(f"\n{'='*60}")
#         print(f"t = {t}")
#         print(f"{'='*60}")
        
#         # Разделяем n на четные и нечетные
#         even_n = []
#         odd_n = []
        
#         for n in sorted(data_by_t[t].keys()):
#             if n % 2 == 0:
#                 even_n.append(n)
#             else:
#                 odd_n.append(n)
        
#         # Сначала выводим четные n
#         print("ЧЕТНЫЕ n:")
#         for n in sorted(even_n):
#             values = data_by_t[t][n]
#             print(f"n = {n:2d}: {values}")
        
#         print("\nНЕЧЕТНЫЕ n:")
#         for n in sorted(odd_n):
#             values = data_by_t[t][n]
#             print(f"n = {n:2d}: {values}")
        
#         print(f"{'='*60}")

# def main_simple():
#     filename = "output_results.txt"
#     display_grouped_by_t(filename)

# if __name__ == "__main__":
#     main_simple()




def display_grouped_by_t(filename):
    """
    Просто выводит на экран данные, сгруппированные по t
    """
    with open(filename, 'r') as f:
        content = f.read()
    
    data_by_t = {}
    current_n = None
    current_t = None
    
    # Парсинг
    for line in content.split('\n'):
        line = line.strip()
        if not line:
            continue
            
        if line.startswith('n = '):
            current_n = int(line.split('=')[1].strip())
        elif line.startswith('t = '):
            current_t = int(line.split('=')[1].strip())
            if current_t not in data_by_t:
                data_by_t[current_t] = {}
        elif line and current_n is not None and current_t is not None:
            numbers = list(map(int, line.split()))
            data_by_t[current_t][current_n] = numbers
    
    # Вывод с четким разделением
    for t in sorted(data_by_t.keys()):
        print(f"\n{'='*60}")
        print(f"t = {t}")
        print(f"{'='*60}")
        
        for n in sorted(data_by_t[t].keys()):
            values = data_by_t[t][n]
            print(f"n = {n:2d}: {values}")
        
        print(f"{'='*60}")

def main_simple():
    filename = "output_results.txt"
    display_grouped_by_t(filename)

if __name__ == "__main__":
    main_simple()