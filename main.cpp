#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

int main() {
    std::setlocale(LC_ALL, "Russian");

    int n;
    std::cout << "Введите размер матрицы (количество вершин): " << std::endl;
    std::cin >> n;

    
    std::srand(static_cast<unsigned int>(std::time(0)));

    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));

    
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) { 
            int edge = std::rand() % 2;
            matrix[i][j] = edge;
            matrix[j][i] = edge; 
        }
    }

    
    std::cout << "\nМатрица смежности графа:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    int vertex_count = matrix.size();
    std::cout << "\nРазмер графа (количество вершин) по матр. смежности: " << vertex_count << "\n";

    
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            if (matrix[i][j] == 1) {
                
                edges.push_back({i, j});
            }
        }
    }

    int edge_count = edges.size();

    
    std::vector<std::vector<int>> incidence_matrix(vertex_count, std::vector<int>(edge_count, 0));

    for (int e = 0; e < edge_count; ++e) {
        int u = edges[e].first;
        int v = edges[e].second;

        if (u == v) {
            
            incidence_matrix[u][e] = 2;
        } else {
            
            incidence_matrix[u][e] = 1;
            incidence_matrix[v][e] = 1;
        }
    }

    
    std::cout << "\nМатрица инцидентности графа (Вершины x Ребра):\n";
    
    std::cout << "    ";
    for (int e = 0; e < edge_count; ++e) std::cout << "e" << e << " ";
    std::cout << "\n";

    for (int i = 0; i < vertex_count; ++i) {
        std::cout << "v" << i << " | ";
        for (int e = 0; e < edge_count; ++e) {
            std::cout << incidence_matrix[i][e] << "  ";
        }
        std::cout << "\n";
    }

    
    int vertex_count_from_incidence = incidence_matrix.size();
    
    int edge_count_from_incidence = (vertex_count_from_incidence > 0) ? incidence_matrix[0].size() : 0;

    std::cout << "\nРазмер графа по матр. инцидентности:\n";
    std::cout << "- Количество вершин (порядок): " << vertex_count_from_incidence << "\n";
    std::cout << "- Количество ребер (размер): " << edge_count_from_incidence << "\n";

    
    std::vector<int> degrees(vertex_count, 0);
    for (int i = 0; i < vertex_count; ++i) {
        int row_sum = 0;
        for (int e = 0; e < edge_count; ++e) {
            row_sum += incidence_matrix[i][e];
        }
        degrees[i] = row_sum;
    }

    std::cout << "\nСтепени вершин (по матр. инцидентности):\n";
    for (int i = 0; i < vertex_count; ++i) {
        std::cout << "Вершина " << i << ": степень = " << degrees[i] << "\n";
    }

    
    std::cout << "\n--- Анализ вершин ---\n";

    
    std::cout << "Изолированные вершины: ";
    bool found = false;
    for (int i = 0; i < vertex_count; ++i) {
        if (degrees[i] == 0 or (degrees[i] == 2 and matrix[i][i] == 1)) {
            std::cout << i << " ";
            found = true;
        }
    }
    if (!found) std::cout << "нет";
    std::cout << "\n";

    
    std::cout << "Концевые вершины: ";
    found = false;
    for (int i = 0; i < vertex_count; ++i) {
        if (degrees[i] == 1) {
            std::cout << i << " ";
            found = true;
        }
    }
    if (!found) std::cout << "нет";
    std::cout << "\n";

    
    std::cout << "Доминирующие вершины: ";
    found = false;
    for (int i = 0; i < vertex_count; ++i) {
        bool is_dominating = true;
        for (int j = 0; j < vertex_count; ++j) {
            if (matrix[i][j] == 0) {
                is_dominating = false;
                break;
            }
        }
        if (is_dominating) {
            std::cout << i << " ";
            found = true;
        }
    }
    if (!found) std::cout << "нет";
    std::cout << "\n";

    return 0;
}