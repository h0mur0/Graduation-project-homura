import random
import string
import subprocess
import time

def generate_random_strings(n, length=50):
    """生成n个随机字符串，每个字符串长度为length"""
    return [''.join(random.choices(string.ascii_letters + string.digits, k=length)) for _ in range(n)]

def write_random_strings_to_files(n, t, m):
    """
    创建n个随机字符串，分布写入data1.txt ~ data5.txt，确保全覆盖。
    data1.txt写入t个字符串，data2.txt ~ data5.txt分别写入m个字符串。
    """
    if 4 * m < n:
        raise ValueError("t和m的设置无法覆盖n个字符串，请调整参数。")
    
    # 生成n个随机字符串
    time1 = time.time()
    all_strings = generate_random_strings(n)
    # print("1:",n,":",time.time()-time1)
    random.shuffle(all_strings)

    # 确保分配的字符串能够覆盖全部n个
    while True:
        selected_strings = set()
        # "data1.txt": random.sample(all_strings, t)
        time2 = time.time()
        data_files = {
            "data2.txt": random.sample(all_strings, m),
            "data3.txt": random.sample(all_strings, m),
            "data4.txt": random.sample(all_strings, m),
            "data5.txt": random.sample(all_strings, m)
        }
        # print("2:",n,":",time.time()-time2)
        for strings in data_files.values():
            selected_strings.update(strings)
        
        if len(selected_strings) == n:
            break  # 全覆盖则跳出循环

    # 写入各文件
    st = {
        "data1-1.txt":random.sample(all_strings, 1),
        "data1-2.txt":random.sample(all_strings, 10),
        "data1-3.txt":random.sample(all_strings, 100),
        }
    # time3 = time.time()
    for filename, strings in st.items():
        with open(filename, 'w') as f:
            f.write('\n'.join(strings))
    # print("3:",n,":",time.time()-time3)
    for filename, strings in data_files.items():
        with open(filename, 'w') as f:
            f.write('\n'.join(strings))

# write_random_strings_to_files(n=100000, t=1, m=95000)

write_random_strings_to_files(n=1000, t=1, m=950)
subprocess.run(["main.exe","-m","5","-t","data1-1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-2.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-3.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
write_random_strings_to_files(n=10000, t=1, m=9500)
subprocess.run(["main.exe","-m","5","-t","data1-1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-2.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-3.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
write_random_strings_to_files(n=100000, t=1, m=95000)
subprocess.run(["main.exe","-m","5","-t","data1-1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-2.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
subprocess.run(["main.exe","-m","5","-t","data1-3.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])



# 测试
# write_random_strings_to_files(n=1000, t=1, m=700)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=1000, t=10, m=700)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=1000, t=100, m=700)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=10000, t=1, m=8500)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=10000, t=10, m=8500)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=10000, t=100, m=8500)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=100000, t=1, m=95000)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=100000, t=10, m=95000)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
# write_random_strings_to_files(n=100000, t=100, m=95000)
# subprocess.run(["main.exe","-m","5","-t","data1.txt","data2.txt","data3.txt","data4.txt","data5.txt","-n","11","11","11","11","11"])
