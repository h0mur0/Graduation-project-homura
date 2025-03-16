def read_file_lines(filename):
    with open(filename, 'r') as file:
        return {line.rstrip('\n') for line in file}

# 替换为你的文件名列表
files = ['data1.txt', 'data2.txt', 'data3.txt', 'data4.txt', 'data5.txt']

# 读取每个文件的行集合
line_sets = [read_file_lines(f) for f in files]

# 计算所有集合的交集
common_lines = set.intersection(*line_sets)

# 输出共同行（按字母顺序排序）
for line in sorted(common_lines):
    print(line)

print(len(line))