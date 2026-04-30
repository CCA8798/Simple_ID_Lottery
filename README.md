# Simple ID Lottery
![C++17](https://img.shields.io/badge/C%2B%2B_Version-14+-blue.svg)
![MIT License](https://img.shields.io/badge/License-MIT-green.svg)
> - CPP构建的轻量抽奖工具
> - 支持本地Json文件或Web Json API输入

## 功能
- 本地JSON文件输入 格式为字符串数组 `["@A" "@B"]`
- Web API输入 自动请求URL并保存为临时文件
- 多轮抽奖 每轮可设不同中奖人数
- 可选每轮后删除已中奖用户
- 可选逐步展示结果或一次性输出
- 自动保存每轮抽奖记录到 `results/随机ID/` 目录

## 建议构建要求
- CMake 3.31+
- C++14及以上编译器
- libcurl 静态库
- jsonxx 头文件库

## 快速构建
```bash
git clone https://github.com/CCA8798/Simple_ID_Lottery.git

cd Simple_ID_Lottery

mkdir build && cd build

cmake .. -DCURL_STATICLIB=ON

cmake --build .
```

## 使用
运行程序后按提示选择输入模式 本地需提供JSON文件路径 Web模式需输入URL和临时文件名

抽奖记录保存于 `results/` 文件夹 每次运行生成随机子目录

## 依赖
- libcurl 处理HTTP请求
- jsonxx 解析JSON
- C++14 标准库功能

## 许可
MIT License