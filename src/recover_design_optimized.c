#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 51

// Structure to store connector information
typedef struct {
    int row, col;      // Position (1-indexed for output)
    int degree;        // Required degree (k-way connector)
    int up, down, left, right;  // Current connection status
} Connector;

// **新增**: 邻居信息结构，用于优化查找
typedef struct {
    int up_neighbor;    // Index of nearest connector above (-1 if none)
    int down_neighbor;  // Index of nearest connector below (-1 if none)
    int left_neighbor;  // Index of nearest connector to left (-1 if none)
    int right_neighbor; // Index of nearest connector to right (-1 if none)
} NeighborInfo;

// Global variables
int n, m;  // Garden dimensions
int grid[MAX_SIZE][MAX_SIZE];  // Input grid with connector degrees
Connector connectors[MAX_SIZE * MAX_SIZE];  // List of all connectors
NeighborInfo neighbors[MAX_SIZE * MAX_SIZE];  // **新增**: 预计算的邻居信息
int connector_count = 0;
int solution_found = 0;
long long node_count = 0;  // **新增**: 用于性能分析

// Comparator for sorting connectors by position (row, then col)
int compare_connectors(const void *a, const void *b) {
    Connector *x = (Connector*)a;
    Connector *y = (Connector*)b;
    if (x->row != y->row) return x->row - y->row;
    return x->col - y->col;
}

// Find connector index by position (0-indexed position)
int find_connector_at(int r, int c) {
    for (int i = 0; i < connector_count; i++) {
        if (connectors[i].row - 1 == r && connectors[i].col - 1 == c) {
            return i;
        }
    }
    return -1;
}

// **优化1**: 预计算邻居信息，避免重复查找
void precompute_neighbors() {
    for (int i = 0; i < connector_count; i++) {
        int r = connectors[i].row - 1;
        int c = connectors[i].col - 1;
        
        // Find UP neighbor
        neighbors[i].up_neighbor = -1;
        for (int nr = r - 1; nr >= 0; nr--) {
            if (grid[nr][c] > 0) {
                neighbors[i].up_neighbor = find_connector_at(nr, c);
                break;
            }
        }
        
        // Find DOWN neighbor
        neighbors[i].down_neighbor = -1;
        for (int nr = r + 1; nr < n; nr++) {
            if (grid[nr][c] > 0) {
                neighbors[i].down_neighbor = find_connector_at(nr, c);
                break;
            }
        }
        
        // Find LEFT neighbor
        neighbors[i].left_neighbor = -1;
        for (int nc = c - 1; nc >= 0; nc--) {
            if (grid[r][nc] > 0) {
                neighbors[i].left_neighbor = find_connector_at(r, nc);
                break;
            }
        }
        
        // Find RIGHT neighbor
        neighbors[i].right_neighbor = -1;
        for (int nc = c + 1; nc < m; nc++) {
            if (grid[r][nc] > 0) {
                neighbors[i].right_neighbor = find_connector_at(r, nc);
                break;
            }
        }
    }
}

// **剪枝策略1**: 提前检查度数可行性
int check_degree_feasibility() {
    for (int i = 0; i < connector_count; i++) {
        int available = 0;
        
        if (neighbors[i].up_neighbor >= 0) available++;
        if (neighbors[i].down_neighbor >= 0) available++;
        if (neighbors[i].left_neighbor >= 0) available++;
        if (neighbors[i].right_neighbor >= 0) available++;
        
        if (available < connectors[i].degree) {
            return 0;  // 不可行
        }
    }
    return 1;
}

// **剪枝策略2**: 检测孤立连接器
int check_isolation(int current_index) {
    for (int i = current_index; i < connector_count; i++) {
        if (connectors[i].degree == 0) continue;
        
        int can_connect = 0;
        
        // 检查UP方向
        if (neighbors[i].up_neighbor >= 0) {
            int nb = neighbors[i].up_neighbor;
            if (nb >= current_index || connectors[nb].down) {
                can_connect = 1;
            }
        }
        
        // 检查DOWN方向
        if (neighbors[i].down_neighbor >= 0) {
            int nb = neighbors[i].down_neighbor;
            if (nb >= current_index || connectors[nb].up) {
                can_connect = 1;
            }
        }
        
        // 检查LEFT方向
        if (neighbors[i].left_neighbor >= 0) {
            int nb = neighbors[i].left_neighbor;
            if (nb >= current_index || connectors[nb].right) {
                can_connect = 1;
            }
        }
        
        // 检查RIGHT方向
        if (neighbors[i].right_neighbor >= 0) {
            int nb = neighbors[i].right_neighbor;
            if (nb >= current_index || connectors[nb].left) {
                can_connect = 1;
            }
        }
        
        if (!can_connect) return 0;
    }
    return 1;
}

// Validate current partial configuration (incremental validation)
// **优化**: 使用预计算的邻居信息，提升查找速度
int is_valid_partial(int index) {
    int r = connectors[index].row - 1;  // 0-indexed
    int c = connectors[index].col - 1;
    
    // Check UP direction
    if (connectors[index].up) {
        if (r == 0) return 0;
        if (neighbors[index].up_neighbor < 0) return 0;
        
        int nb = neighbors[index].up_neighbor;
        if (nb < index && !connectors[nb].down) {
            return 0;
        }
    } else {
        if (neighbors[index].up_neighbor >= 0) {
            int nb = neighbors[index].up_neighbor;
            if (nb < index && connectors[nb].down) {
                return 0;
            }
        }
    }
    
    // Check DOWN direction
    if (connectors[index].down) {
        if (r == n - 1) return 0;
        if (neighbors[index].down_neighbor < 0) return 0;
        
        int nb = neighbors[index].down_neighbor;
        if (nb < index && !connectors[nb].up) {
            return 0;
        }
    } else {
        if (neighbors[index].down_neighbor >= 0) {
            int nb = neighbors[index].down_neighbor;
            if (nb < index && connectors[nb].up) {
                return 0;
            }
        }
    }
    
    // Check LEFT direction
    if (connectors[index].left) {
        if (c == 0) return 0;
        if (neighbors[index].left_neighbor < 0) return 0;
        
        int nb = neighbors[index].left_neighbor;
        if (nb < index && !connectors[nb].right) {
            return 0;
        }
    } else {
        if (neighbors[index].left_neighbor >= 0) {
            int nb = neighbors[index].left_neighbor;
            if (nb < index && connectors[nb].right) {
                return 0;
            }
        }
    }
    
    // Check RIGHT direction
    if (connectors[index].right) {
        if (c == m - 1) return 0;
        if (neighbors[index].right_neighbor < 0) return 0;
        
        int nb = neighbors[index].right_neighbor;
        if (nb < index && !connectors[nb].left) {
            return 0;
        }
    } else {
        if (neighbors[index].right_neighbor >= 0) {
            int nb = neighbors[index].right_neighbor;
            if (nb < index && connectors[nb].left) {
                return 0;
            }
        }
    }
    
    return 1;  // Valid partial configuration
}

// Backtracking function to find valid fence configuration with incremental validation
// **增强版**: 集成多种剪枝策略
void backtrack(int index) {
    if (solution_found) return;
    
    node_count++;  // 统计搜索节点数
    
    if (index == connector_count) {
        // All connectors configured successfully (incremental validation ensures correctness)
        solution_found = 1;
        return;
    }
    
    // **剪枝策略2**: 检测孤立连接器
    if (!check_isolation(index)) {
        return;
    }
    
    // Try all possible combinations for current connector
    int degree = connectors[index].degree;
    int r = connectors[index].row - 1;
    int c = connectors[index].col - 1;
    
    // **剪枝策略3**: 预先过滤无效配置，减少枚举量
    int valid_masks[16];
    int valid_count = 0;
    
    // Generate all 4-bit combinations
    for (int mask = 0; mask < 16; mask++) {
        int up = (mask >> 0) & 1;
        int down = (mask >> 1) & 1;
        int left = (mask >> 2) & 1;
        int right = (mask >> 3) & 1;
        
        // Check if this combination matches the required degree
        if (up + down + left + right != degree) continue;
        
        // **快速边界检查**
        if (up && r == 0) continue;
        if (down && r == n - 1) continue;
        if (left && c == 0) continue;
        if (right && c == m - 1) continue;
        
        // **快速邻居存在性检查** - 关键优化！
        if (up && neighbors[index].up_neighbor < 0) continue;
        if (down && neighbors[index].down_neighbor < 0) continue;
        if (left && neighbors[index].left_neighbor < 0) continue;
        if (right && neighbors[index].right_neighbor < 0) continue;
        
        valid_masks[valid_count++] = mask;
    }
    
    // 只尝试预先过滤后的有效配置
    for (int i = 0; i < valid_count; i++) {
        int mask = valid_masks[i];
        
        connectors[index].up = (mask >> 0) & 1;
        connectors[index].down = (mask >> 1) & 1;
        connectors[index].left = (mask >> 2) & 1;
        connectors[index].right = (mask >> 3) & 1;
        
        // Incremental validation: check immediately before recursing
        if (is_valid_partial(index)) {
            backtrack(index + 1);
            if (solution_found) return;
        }
        // If validation fails, try next combination without recursing
    }
}

// Function to print the solution
void print_solution() {
    for (int i = 0; i < connector_count; i++) {
        printf("%d %d %d %d %d %d\n", 
               connectors[i].row, 
               connectors[i].col,
               connectors[i].up,
               connectors[i].down,
               connectors[i].left,
               connectors[i].right);
    }
}

int main() {
    // Read input
    scanf("%d %d", &n, &m);
    
    connector_count = 0;
    
    // Read grid and collect connectors
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &grid[i][j]);
            if (grid[i][j] > 0) {
                connectors[connector_count].row = i + 1;  // 1-indexed
                connectors[connector_count].col = j + 1;  // 1-indexed
                connectors[connector_count].degree = grid[i][j];
                connectors[connector_count].up = 0;
                connectors[connector_count].down = 0;
                connectors[connector_count].left = 0;
                connectors[connector_count].right = 0;
                connector_count++;
            }
        }
    }
    
    // Sort connectors by position (row, then col) as required by output format
    qsort(connectors, connector_count, sizeof(Connector), compare_connectors);
    
    // **优化1**: 预计算邻居信息
    precompute_neighbors();
    
    // **剪枝策略1**: 提前检查度数可行性
    if (!check_degree_feasibility()) {
        printf("No Solution\n");
        return 0;
    }
    
    // Use backtracking to find a valid configuration
    solution_found = 0;
    node_count = 0;
    backtrack(0);
    
    if (solution_found) {
        print_solution();
        // 可选：输出性能分析（调试时使用）
        // fprintf(stderr, "Nodes explored: %lld\n", node_count);
    } else {
        printf("No Solution\n");
    }
    
    return 0;
}
