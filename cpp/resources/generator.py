with open("text.txt", "w", encoding="utf-8") as f:
    for i in range(1, 10000):
        f.write(f"line{i}\n")
