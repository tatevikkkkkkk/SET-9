import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

array_types = [
    "random",
    "reverse",
    "almost",
    "prefix"
]

array_names = {
    "random": "Случайные массивы",
    "reverse": "Обратно отсортированные массивы",
    "almost": "Почти отсортированные массивы",
    "prefix": "Массивы с общими префиксами"
}

metrics = [
    ("time_us", "Время выполнения (мкс)"),
    ("char_cmps", "Количество посимвольных сравнений")
]

for arr_type in array_types:

    subset = df[df["array_type"] == arr_type]

    for metric, ylabel in metrics:

        plt.figure(figsize=(10, 6))

        algorithms = subset["algorithm"].unique()

        for algo in algorithms:
            algo_data = subset[subset["algorithm"] == algo]

            plt.plot(
                algo_data["n"],
                algo_data[metric],
                marker='o',
                linewidth=2,
                label=algo
            )

        plt.title(f"{ylabel} — {array_names[arr_type]}")
        plt.xlabel("Размер массива (n)")
        plt.ylabel(ylabel)

        plt.grid(True)
        plt.legend()

        if metric == "char_cmps":
            plt.yscale("log")

        filename = f"{arr_type}_{metric}.png"

        plt.savefig(
            filename,
            dpi=300,
            bbox_inches="tight"
        )

        plt.close()

        print(f"Сохранён график: {filename}")

print("Все графики построены.")
