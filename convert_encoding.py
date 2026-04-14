import os

def convert_gbk_to_utf8(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.c', '.h', '.cpp', '.hpp')):
                file_path = os.path.join(root, file)
                try:
                    # 尝试以 GBK 读取
                    with open(file_path, 'r', encoding='gbk') as f:
                        content = f.read()
                    # 以 UTF-8 写入
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"成功转换: {file_path}")
                except UnicodeDecodeError:
                    print(f"无需转换或不是标准的 GBK: {file_path}")
                except Exception as e:
                    print(f"转换失败 {file_path}: {e}")

if __name__ == "__main__":
    # 替换为你的源码目录，通常转换 Core 和 Drivers 即可，也可以直接填项目根目录 "."
    convert_gbk_to_utf8("./Core")
