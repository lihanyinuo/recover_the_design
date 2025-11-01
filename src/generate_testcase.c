/*
 * 随机测试用例生成器 - 围栏恢复设计问题
 * 功能：生成指定规模的随机测试用例
 * 编译：gcc generate_testcase.c -o generate_testcase.exe
 * 使用：generate_testcase.exe <rows> <cols> <density> [seed] > output.txt
 *       density: 连接器密度 (0.0-1.0)，建议 0.3-0.7
 *       seed: 可选的随机种子
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int row, col;
    int degree;
    int up, down, left, right;
} Connector;

int main(int argc, char *argv[]) {
    int rows, cols;
    double density;
    unsigned int seed;
    
    // 解析命令行参数
    if (argc < 4) {
        fprintf(stderr, "使用方法: %s <rows> <cols> <density> [seed]\n", argv[0]);
        fprintf(stderr, "  rows    : 网格行数 (1-100)\n");
        fprintf(stderr, "  cols    : 网格列数 (1-100)\n");
        fprintf(stderr, "  density : 连接器密度 (0.0-1.0)，推荐 0.3-0.7\n");
        fprintf(stderr, "  seed    : 可选的随机种子\n");
        fprintf(stderr, "\n示例:\n");
        fprintf(stderr, "  小型: %s 5 5 0.4 > test_small.txt\n", argv[0]);
        fprintf(stderr, "  中型: %s 10 10 0.5 > test_medium.txt\n", argv[0]);
        fprintf(stderr, "  大型: %s 20 20 0.6 > test_large.txt\n", argv[0]);
        return 1;
    }
    
    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    density = atof(argv[3]);
    
    if (rows < 1 || rows > 100 || cols < 1 || cols > 100) {
        fprintf(stderr, "错误: 行列数必须在 1-100 之间\n");
        return 1;
    }
    
    if (density < 0.0 || density > 1.0) {
        fprintf(stderr, "错误: 密度必须在 0.0-1.0 之间\n");
        return 1;
    }
    
    // 设置随机种子
    if (argc >= 5) {
        seed = (unsigned int)atoi(argv[4]);
    } else {
        seed = (unsigned int)time(NULL);
    }
    srand(seed);
    
    fprintf(stderr, "生成参数: %dx%d 网格, 密度=%.2f, 种子=%u\n", 
            rows, cols, density, seed);
    
    // 分配网格
    int **grid = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        grid[i] = (int *)calloc(cols, sizeof(int));
    }
    
    // 随机放置连接器
    int total_cells = rows * cols;
    int num_connectors = (int)(total_cells * density);
    if (num_connectors < 1) num_connectors = 1;
    
    fprintf(stderr, "目标连接器数量: %d\n", num_connectors);
    
    // 生成随机位置
    int placed = 0;
    int attempts = 0;
    int max_attempts = total_cells * 10;
    
    while (placed < num_connectors && attempts < max_attempts) {
        int r = rand() % rows;
        int c = rand() % cols;
        
        if (grid[r][c] == 0) {
            grid[r][c] = 1;  // 标记为有连接器
            placed++;
        }
        attempts++;
    }
    
    fprintf(stderr, "实际放置连接器: %d\n", placed);
    
    // 为每个连接器分配随机度数和方向
    Connector *connectors = (Connector *)malloc(placed * sizeof(Connector));
    int idx = 0;
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 1) {
                connectors[idx].row = i;
                connectors[idx].col = j;
                
                // 计算可能的最大度数
                int max_degree = 0;
                int can_up = (i > 0);
                int can_down = (i < rows - 1);
                int can_left = (j > 0);
                int can_right = (j < cols - 1);
                
                if (can_up) max_degree++;
                if (can_down) max_degree++;
                if (can_left) max_degree++;
                if (can_right) max_degree++;
                
                // 随机选择度数 (1 到 max_degree)
                if (max_degree > 0) {
                    connectors[idx].degree = 1 + rand() % max_degree;
                } else {
                    connectors[idx].degree = 0;
                }
                
                // 随机分配方向（确保度数匹配）
                connectors[idx].up = 0;
                connectors[idx].down = 0;
                connectors[idx].left = 0;
                connectors[idx].right = 0;
                
                int directions[4] = {0, 1, 2, 3}; // 0=上, 1=下, 2=左, 3=右
                int available[4];
                int avail_count = 0;
                
                if (can_up) available[avail_count++] = 0;
                if (can_down) available[avail_count++] = 1;
                if (can_left) available[avail_count++] = 2;
                if (can_right) available[avail_count++] = 3;
                
                // 随机选择方向
                for (int d = 0; d < connectors[idx].degree && avail_count > 0; d++) {
                    int pick = rand() % avail_count;
                    int dir = available[pick];
                    
                    switch (dir) {
                        case 0: connectors[idx].up = 1; break;
                        case 1: connectors[idx].down = 1; break;
                        case 2: connectors[idx].left = 1; break;
                        case 3: connectors[idx].right = 1; break;
                    }
                    
                    // 移除已选方向
                    available[pick] = available[avail_count - 1];
                    avail_count--;
                }
                
                idx++;
            }
        }
    }
    
    // 输出测试用例
    printf("%d %d\n", rows, cols);
    printf("%d\n", placed);
    
    for (int i = 0; i < placed; i++) {
        printf("%d %d %d %d %d %d %d\n",
               connectors[i].row,
               connectors[i].col,
               connectors[i].degree,
               connectors[i].up,
               connectors[i].down,
               connectors[i].left,
               connectors[i].right);
    }
    
    // 释放内存
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
    free(connectors);
    
    fprintf(stderr, "生成完成！\n");
    
    return 0;
}
