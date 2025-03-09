def find_common_lines(file1, file2):
    with open(file1, 'r', encoding='utf-8') as f1, open(file2, 'r', encoding='utf-8') as f2:
        lines1 = set(f1.readlines())
        lines2 = set(f2.readlines())

    common_lines = lines1.intersection(lines2)

    return common_lines

# 示例使用
file1 = "data1.txt"
file2 = "data2.txt"
common_lines = find_common_lines(file1, file2)

# 输出共同行
for line in common_lines:
    print(line.strip())
