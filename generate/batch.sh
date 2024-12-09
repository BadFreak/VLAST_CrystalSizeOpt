#!/bin/bash

# 定义文件路径和数组
input_file="generator.sh"  # 原始文件
values=(50 100 200 300 500 1000 2000 3000 4000 5000)  # energy 值数组

# 循环遍历数组中的每个值
for value in "${values[@]}"; do
    sed -i "s/energy=.*$/energy=$value/" $input_file  # 使用 sed 命令修改文件中的值
    echo "========== Now Energy is $value =========="
    source generator.sh 
done

