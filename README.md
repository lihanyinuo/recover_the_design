## 目录

1. [测试用例介绍](#测试用例介绍)
2. [测试结果对比](#测试结果对比)
3. [性能分析](#性能分析)
4. [使用指南](#使用指南)

---

# 一、测试用例介绍

### 1.1 基础测试（8个）

| 编号 | 文件名 | 类型 | 规模 | 测试目的 |
|------|--------|------|------|---------|
| 1 | test_empty.txt | 边界测试 | 3×3 | 空网格（全0），验证边界处理 |
| 2 | test_single.txt | 无解测试 | 3×3 | 单个孤立连接器（度数1，无邻居） |
| 3 | test_simple.txt | 基础功能 | 2×2 | 简单有解用例，基本功能验证 |
| 4 | test_chain.txt | 特殊结构 | 1×6 | 链式结构（一条直线），线性排列 |
| 5 | test_dense.txt | 密集分布 | 4×4 | 密集连接器（几乎满网格） |
| 6 | test_degree4.txt | 度数测试 | 3×3 | 包含度数4的复杂用例（十字型） |
| 7 | test_corner.txt | 边界位置 | 5×5 | 四角连接器，测试角落位置 |
| 8 | test_conflict.txt | 约束测试 | 1×3 | 约束冲突场景 |

### 1.2 官方用例（3个）

| 编号 | 文件名 | 规模 | 说明 |
|------|--------|------|------|
| 9 | test1.txt | 5×6 | 项目标准测试1，包含多种度数 |
| 10 | test2.txt | 5×5 | 项目标准测试2，无解场景 |
| 11 | test3.txt | 3×5 | 项目标准测试3，中等复杂度 |

### 1.3 性能测试（3个）

| 编号 | 文件名 | 规模 | 连接器数量 | 测试目的 |
|------|--------|------|-----------|---------|
| 12 | test_large.txt | 20×20 | ~120个 | 中等规模性能测试 |
| 13 | test_worst_case.txt | 6×6 | 36个 | 较坏情况（度数全为2，每个连接器有6种可能） |
| 14 | test_huge.txt | 30×30 | ~225个 | 极限性能测试，大规模数据 |

---

# 二、测试结果对比

## 2.1 正确性测试结果

- **通过率**: 100%
- **一致性**: 原版和优化版输出完全一致

## 2.2 性能测试详细数据

### 完整性能对比表

| 测试用例 | 原版时间(ms) | 优化版时间(ms) | Ticks(原版) | Ticks(优化版) | 加速比 | 性能评级 |
|---------|-------------|---------------|------------|-------------|--------|---------|
| test_empty | 5.76 | 6.41 | 57,592 | 64,126 | 0.90× | 持平 |
| test_single | 5.90 | 5.82 | 59,048 | 58,183 | 1.01× | 持平 |
| test_simple | 5.78 | 6.79 | 57,768 | 67,936 | 0.85× | 稍降 |
| test_chain | 5.77 | 5.63 | 57,724 | 56,339 | 1.02× | 持平 |
| test_dense | 5.19 | 5.43 | 51,871 | 54,337 | 0.95× | 持平 |
| test_degree4 | 5.67 | 5.10 | 56,728 | 51,038 | 1.11× | 小幅提升 |
| test_corner | 5.61 | 5.07 | 56,123 | 50,713 | 1.11× | 小幅提升 |
| test_conflict | 5.59 | 5.57 | 55,931 | 55,688 | 1.00× | 持平 |
| test1 | 5.69 | 6.83 | 56,911 | 68,262 | 0.83× | 稍降 |
| test2 | 5.03 | 5.40 | 50,311 | 53,956 | 0.93× | 稍降 |
| test3 | 5.24 | 5.72 | 52,359 | 57,205 | 0.92× | 稍降 |
| test_large | 5.03 | 6.35 | 50,314 | 63,484 | 0.79× | 稍降 |
| test_worst_case | 4.56 | 5.51 | 45,642 | 55,119 | 0.83× | 稍降 |
| test_huge | 6083.25 | 4199.67 | 60,832,505 | 41,996,689 | 1.45× | 显著提升 |

---

# 三、性能分析

## 3.1 优化效果分析

### 中小规模用例
**表现**: 优化版略慢，几乎不影响使用
**原因**: 
- 预计算邻居信息的固定开销
- 额外的剪枝检查成本
- 简单问题搜索空间小，剪枝收益不明显
- 连接器分布稀疏

### 大规模用例 (30×30) 
**表现**: 优化版明显更快，性能显著提高
**原因**:
- 搜索空间巨大（225个连接器）
- 预计算避免大量重复查找
- 剪枝策略有效减少搜索分支
- 孤立检测提前终止无效路径

## 3.2 优化策略评估

### 原版 (recover_design.c)
- 基础回溯算法
- 增量验证
- 代码简洁清晰
- 适合小规模数据

### 优化版 (recover_design_optimized.c)
- 预计算邻居信息
- 度数可行性检查
- 孤立连接器检测
- 智能配置过滤
- 适合大规模数据

| 优化策略 | 小规模效果 | 大规模效果 |
|---------|-----------|-----------|
| **预计算邻居信息** | 额外开销 | 避免O(k×(n+m))重复查找 |
| **度数可行性检查** | 快速通过 | 提前剪枝不可行情况 |
| **孤立连接器检测** | 较少触发 | 动态剪枝大搜索树 |
| **智能配置过滤** | 微小收益 | 大幅减少枚举量 |
| **节点统计** | - | 性能分析工具 |

### 注意事项
1. **中小规模数据**: 优化版因预计算略慢
2. **内存开销**: 额外的NeighborInfo数组（O(k)空间）
3. **代码复杂度**: 相比原版略复杂

---

# 四、使用指南

## 4.1 快速开始

### 编译程序

```powershell
# 进入源代码目录
cd src

# 编译原版
gcc -o recover_design.exe recover_design.c

# 编译优化版
gcc -o recover_design_optimized.exe recover_design_optimized.c -O2

# 返回项目根目录
cd ..
```

### 运行测试

```powershell
# 进入测试目录
cd test

# 完整自动化测试
.\run_all_tests.ps1

# 手动测试单个用例
Get-Content test1.txt | ..\src\recover_design.exe
Get-Content test1.txt | ..\src\recover_design_optimized.exe

# 返回项目根目录
cd ..
```

## 4.2 文件清单

### 源代码 (src/)
- `recover_design.c` - 原版实现
- `recover_design_optimized.c` - 优化版实现

### 测试用例 (test/)
- `test_empty.txt` ~ `test_conflict.txt` - 基础测试（8个）
- `test1.txt` ~ `test3.txt` - 官方测试（3个）
- `test_large.txt` ~ `test_huge.txt` - 性能测试（3个）

### 测试脚本 (test/)
- `run_all_tests.ps1` - 完整测试套件 
- `test_all_original.ps1` - 原版测试
- `test_all_optimized.ps1` - 优化版测试

## 4.3 查看测试数据

### CSV文件
用Excel或Python打开 `test_results.csv`：

```python
import pandas as pd
df = pd.read_csv('test_results.csv')
print(df)
```

## 4.4 随机测试用例生成

### 编译生成器

```powershell
# 进入源代码目录
cd src

# 编译随机测试用例生成器
gcc generate_testcase.c -o generate_testcase.exe

# 返回项目根目录
cd ..
```

### 使用方法

#### 方法1: 命令行生成

```powershell
# 语法: generate_testcase.exe <行数> <列数> <密度> [种子] > 输出文件
# 密度范围: 0.0-1.0 (推荐 0.3-0.7)

# 生成小型用例 (5×5)
.\src\generate_testcase.exe 5 5 0.4 1234 > test\my_small.txt

# 生成中型用例 (10×10)
.\src\generate_testcase.exe 10 10 0.5 5678 > test\my_medium.txt

# 生成大型用例 (20×20)
.\src\generate_testcase.exe 20 20 0.6 9012 > test\my_large.txt

# 使用随机种子（每次生成不同用例）
.\src\generate_testcase.exe 15 15 0.5 > test\my_random.txt
```

#### 方法2: 交互式生成

```powershell
# 进入测试目录
cd test

# 运行交互式生成脚本
.\generate_custom.ps1

# 根据提示输入参数：
#   - 行数 (1-100)
#   - 列数 (1-100)
#   - 密度 (0.0-1.0)
#   - 输出文件名
#   - 是否使用固定种子
#   - 是否立即测试
```

#### 方法3: 批量生成预定义用例

```powershell
# 进入测试目录
cd test

# 批量生成11个随机测试用例
.\generate_random_tests.ps1

# 自动生成的用例包括：
#   - random_tiny.txt (3×3, 密度0.5)
#   - random_small_sparse.txt (5×5, 密度0.3)
#   - random_small_dense.txt (5×5, 密度0.7)
#   - random_medium.txt (10×10, 密度0.5)
#   - random_medium_sparse.txt (10×10, 密度0.3)
#   - random_medium_dense.txt (10×10, 密度0.7)
#   - random_large.txt (20×20, 密度0.5)
#   - random_large_sparse.txt (20×20, 密度0.3)
#   - random_large_dense.txt (15×15, 密度0.7)
#   - random_rect_wide.txt (5×15, 密度0.5)
#   - random_rect_tall.txt (15×5, 密度0.5)
```

### 参数说明

| 参数 | 说明 | 范围 | 推荐值 |
|------|------|------|--------|
| **行数** | 网格行数 | 1-100 | 小型:5, 中型:10, 大型:20 |
| **列数** | 网格列数 | 1-100 | 小型:5, 中型:10, 大型:20 |
| **密度** | 连接器密度 | 0.0-1.0 | 稀疏:0.3, 中等:0.5, 密集:0.7 |
| **种子** | 随机种子(可选) | 任意整数 | 固定种子可复现结果 |

### 测试生成的用例

```powershell
# 测试随机生成的用例
Get-Content test\my_small.txt | .\src\recover_design.exe
Get-Content test\my_medium.txt | .\src\recover_design_optimized.exe

# 或使用现有测试脚本
# 将生成的文件添加到 run_all_tests.ps1 的 $testFiles 数组中
```
