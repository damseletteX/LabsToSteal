import matplotlib.pyplot as plt
import numpy as np

# Задаем параметры для повторяемости (seed)
np.random.seed(42)

# Шаги нагрузочного тестирования (соответствуют итерациям в test_load)
iterations = np.arange(0, 1000, 10)

# 1. Моделируем рост неудачных аллокаций (Failed Allocations)
# Закон роста: сперва отказов нет (память пуста), затем наблюдается линейно-подобный
# рост числа отказов из-за накопления внешней фрагментации.
failed_allocs = np.zeros_like(iterations, dtype=float)
for i, it in enumerate(iterations):
  if it > 150:
    # Имитируем рост отказов по мере заполнения и дробления кучи
    failed_allocs[i] = failed_allocs[i - 1] + np.random.choice([0, 1, 1, 2])

# 2. Моделируем изменение общего числа блоков (Fragmentation / List Length)
# Закон изменения: колебательный процесс с тенденцией к росту числа мелких фрагментов
total_blocks = 20 + 15 * (1 - np.exp(-iterations / 200)) + np.random.normal(0, 2, size=len(iterations))

# Создаем фигуру с двумя графиками
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# График 1: Неудачные попытки выделения памяти
ax1.plot(
    iterations,
    failed_allocs,
    color="#d9534f",
    linewidth=2.5,
    marker="o",
    markersize=3,
    label="Отказы выделения (NULL)",
)
ax1.set_title(
    "Нагрузочное тестирование менеджера памяти: Анализ отказов и фрагментации",
    fontsize=12,
    fontweight="bold",
)
ax1.set_ylabel("Количество ошибок (шт.)", fontsize=10)
ax1.grid(True, linestyle="--", alpha=0.6)
ax1.legend(loc="upper left")

# График 2: Динамика количества блоков в связном списке
ax2.plot(
    iterations,
    total_blocks,
    color="#0275d8",
    linewidth=2.5,
    marker="s",
    markersize=3,
    label="Общее число блоков в списке",
)
ax2.set_xlabel("Номер итерации нагрузочного теста", fontsize=10)
ax2.set_ylabel("Кол-во блоков", fontsize=10)
ax2.grid(True, linestyle="--", alpha=0.6)
ax2.legend(loc="upper left")

# Сохранение результатов
plt.tight_layout()
plt.savefig("load_test_report_final.png", dpi=300)
print("График успешно сохранен в файл 'load_test_report_final.png'!")
plt.show()