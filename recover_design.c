#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 51

typedef struct {
    int row, col;     
    int degree;       
    int up, down, left, right; 
} Connector;

int n, m;  
int grid[MAX_SIZE][MAX_SIZE]; 
Connector connectors[MAX_SIZE * MAX_SIZE];  
int connector_count = 0;
int solution_found = 0;

int is_valid_configuration() {
    
    for (int i = 0; i < connector_count; i++) {
        int r = connectors[i].row - 1; 
        int c = connectors[i].col - 1;
        
        // Check up connection
        if (connectors[i].up) {
            int found_connector = 0;
            for (int nr = r - 1; nr >= 0; nr--) {
                if (grid[nr][c] > 0) {
                    for (int j = 0; j < connector_count; j++) {
                        if (connectors[j].row - 1 == nr && connectors[j].col - 1 == c) {
                            if (!connectors[j].down) {
                                return 0;  
                            }
                            found_connector = 1;
                            break;
                        }
                    }
                    break; 
                }
            }
        }
        
        // Check down connection
        if (connectors[i].down) {
            int found_connector = 0;
            for (int nr = r + 1; nr < n; nr++) {
                if (grid[nr][c] > 0) {
                    for (int j = 0; j < connector_count; j++) {
                        if (connectors[j].row - 1 == nr && connectors[j].col - 1 == c) {
                            if (!connectors[j].up) {
                                return 0; 
                            }
                            found_connector = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        
        // Check left connection
        if (connectors[i].left) {
            int found_connector = 0;
            for (int nc = c - 1; nc >= 0; nc--) {
                if (grid[r][nc] > 0) {
                    for (int j = 0; j < connector_count; j++) {
                        if (connectors[j].row - 1 == r && connectors[j].col - 1 == nc) {
                            if (!connectors[j].right) {
                                return 0; 
                            }
                            found_connector = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        
        // Check right connection
        if (connectors[i].right) {
            int found_connector = 0;
            for (int nc = c + 1; nc < m; nc++) {
                if (grid[r][nc] > 0) {
                    for (int j = 0; j < connector_count; j++) {
                        if (connectors[j].row - 1 == r && connectors[j].col - 1 == nc) {
                            if (!connectors[j].left) {
                                return 0; 
                            }
                            found_connector = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return 1;
}

void backtrack(int index) {
    if (solution_found) return;
    
    if (index == connector_count) {
        if (is_valid_configuration()) {
            solution_found = 1;
        }
        return;
    }
    
    int degree = connectors[index].degree;
    
    for (int mask = 0; mask < 16; mask++) {
        int up = (mask >> 0) & 1;
        int down = (mask >> 1) & 1;
        int left = (mask >> 2) & 1;
        int right = (mask >> 3) & 1;
        
        if (up + down + left + right == degree) {
            connectors[index].up = up;
            connectors[index].down = down;
            connectors[index].left = left;
            connectors[index].right = right;
            
            backtrack(index + 1);
            
            if (solution_found) return;
        }
    }
}

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
    scanf("%d %d", &n, &m);
    
    connector_count = 0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &grid[i][j]);
            if (grid[i][j] > 0) {
                connectors[connector_count].row = i + 1;  
                connectors[connector_count].col = j + 1;  
                connectors[connector_count].degree = grid[i][j];
                connectors[connector_count].up = 0;
                connectors[connector_count].down = 0;
                connectors[connector_count].left = 0;
                connectors[connector_count].right = 0;
                connector_count++;
            }
        }
    }
    
    solution_found = 0;
    backtrack(0);
    
    if (solution_found) {
        print_solution();
    } else {
        printf("No Solution\n");
    }
    
    return 0;
}
