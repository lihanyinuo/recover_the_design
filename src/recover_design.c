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

// Global variables
int n, m;  // Garden dimensions
int grid[MAX_SIZE][MAX_SIZE];  // Input grid with connector degrees
Connector connectors[MAX_SIZE * MAX_SIZE];  // List of all connectors
int connector_count = 0;
int solution_found = 0;

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

// Validate current partial configuration (incremental validation)
// Check only the connector at 'index' against already configured ones
int is_valid_partial(int index) {
    int r = connectors[index].row - 1;  // 0-indexed
    int c = connectors[index].col - 1;
    
    // Check UP direction
    if (connectors[index].up) {
        // Cannot go beyond boundary
        if (r == 0) return 0;  // Top boundary - cannot have UP
        
        // Must have at least one connector above (can extend through empty cells)
        int found_connector = 0;
        for (int nr = r - 1; nr >= 0; nr--) {
            if (grid[nr][c] > 0) {
                // Found a connector above
                int neighbor_idx = find_connector_at(nr, c);
                if (neighbor_idx < 0) return 0;
                
                // If neighbor is already configured, it must have DOWN
                if (neighbor_idx < index) {
                    if (!connectors[neighbor_idx].down) {
                        return 0;  // Mismatch
                    }
                }
                found_connector = 1;
                break;  // Stop at first connector
            }
        }
        // UP requires at least one connector in that direction
        if (!found_connector) return 0;
    } else {
        // If we don't have UP, check if a configured neighbor above has DOWN
        if (r > 0) {
            for (int nr = r - 1; nr >= 0; nr--) {
                if (grid[nr][c] > 0) {
                    int neighbor_idx = find_connector_at(nr, c);
                    if (neighbor_idx >= 0 && neighbor_idx < index) {
                        if (connectors[neighbor_idx].down) {
                            return 0;  // Mismatch: neighbor wants to connect but we don't
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Check DOWN direction
    if (connectors[index].down) {
        // Cannot go beyond boundary
        if (r == n - 1) return 0;  // Bottom boundary - cannot have DOWN
        
        int found_connector = 0;
        for (int nr = r + 1; nr < n; nr++) {
            if (grid[nr][c] > 0) {
                int neighbor_idx = find_connector_at(nr, c);
                if (neighbor_idx < 0) return 0;
                
                if (neighbor_idx < index) {
                    if (!connectors[neighbor_idx].up) {
                        return 0;
                    }
                }
                found_connector = 1;
                break;
            }
        }
        // DOWN requires at least one connector in that direction
        if (!found_connector) return 0;
    } else {
        if (r < n - 1) {
            for (int nr = r + 1; nr < n; nr++) {
                if (grid[nr][c] > 0) {
                    int neighbor_idx = find_connector_at(nr, c);
                    if (neighbor_idx >= 0 && neighbor_idx < index) {
                        if (connectors[neighbor_idx].up) {
                            return 0;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Check LEFT direction
    if (connectors[index].left) {
        // Cannot go beyond boundary
        if (c == 0) return 0;  // Left boundary - cannot have LEFT
        
        int found_connector = 0;
        for (int nc = c - 1; nc >= 0; nc--) {
            if (grid[r][nc] > 0) {
                int neighbor_idx = find_connector_at(r, nc);
                if (neighbor_idx < 0) return 0;
                
                if (neighbor_idx < index) {
                    if (!connectors[neighbor_idx].right) {
                        return 0;
                    }
                }
                found_connector = 1;
                break;
            }
        }
        // LEFT requires at least one connector in that direction
        if (!found_connector) return 0;
    } else {
        if (c > 0) {
            for (int nc = c - 1; nc >= 0; nc--) {
                if (grid[r][nc] > 0) {
                    int neighbor_idx = find_connector_at(r, nc);
                    if (neighbor_idx >= 0 && neighbor_idx < index) {
                        if (connectors[neighbor_idx].right) {
                            return 0;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Check RIGHT direction
    if (connectors[index].right) {
        // Cannot go beyond boundary
        if (c == m - 1) return 0;  // Right boundary - cannot have RIGHT
        
        int found_connector = 0;
        for (int nc = c + 1; nc < m; nc++) {
            if (grid[r][nc] > 0) {
                int neighbor_idx = find_connector_at(r, nc);
                if (neighbor_idx < 0) return 0;
                
                if (neighbor_idx < index) {
                    if (!connectors[neighbor_idx].left) {
                        return 0;
                    }
                }
                found_connector = 1;
                break;
            }
        }
        // RIGHT requires at least one connector in that direction
        if (!found_connector) return 0;
    } else {
        if (c < m - 1) {
            for (int nc = c + 1; nc < m; nc++) {
                if (grid[r][nc] > 0) {
                    int neighbor_idx = find_connector_at(r, nc);
                    if (neighbor_idx >= 0 && neighbor_idx < index) {
                        if (connectors[neighbor_idx].left) {
                            return 0;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return 1;  // Valid partial configuration
}

// Backtracking function to find valid fence configuration with incremental validation
void backtrack(int index) {
    if (solution_found) return;
    
    if (index == connector_count) {
        // All connectors configured successfully (incremental validation ensures correctness)
        solution_found = 1;
        return;
    }
    
    // Try all possible combinations for current connector
    int degree = connectors[index].degree;
    
    // Generate all 4-bit combinations
    for (int mask = 0; mask < 16; mask++) {
        int up = (mask >> 0) & 1;
        int down = (mask >> 1) & 1;
        int left = (mask >> 2) & 1;
        int right = (mask >> 3) & 1;
        
        // Check if this combination matches the required degree
        if (up + down + left + right == degree) {
            connectors[index].up = up;
            connectors[index].down = down;
            connectors[index].left = left;
            connectors[index].right = right;
            
            // Incremental validation: check immediately before recursing
            if (is_valid_partial(index)) {
                backtrack(index + 1);
                if (solution_found) return;
            }
            // If validation fails, try next combination without recursing
        }
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
    
    // Use backtracking to find a valid configuration
    solution_found = 0;
    backtrack(0);
    
    if (solution_found) {
        print_solution();
    } else {
        printf("No Solution\n");
    }
    
    return 0;
}
