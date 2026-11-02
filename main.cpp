#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <clocale>
#include <string>

using Matrix = std::vector<std::vector<int>>;

// Результаты анализа графа
struct GraphStats {
    int order = 0;                 
    int size = 0;                  
    std::vector<int> degrees;      
    std::vector<int> outDeg;       
    std::vector<int> inDeg;        
    std::vector<int> isolated;     // изолированные вершины (степень 0)
    std::vector<int> loops;        // кольцевые вершины (есть петля)
    std::vector<int> dominating;   // доминирующие вершины
};

// ---------- Анализ по матрице смежности ----------
GraphStats analyzeAdjacency(const Matrix& a, bool directed) {
    GraphStats s;
    int n = static_cast<int>(a.size());
    s.order = n;
    s.degrees.assign(n, 0);
    if (directed) {
        s.outDeg.assign(n, 0);
        s.inDeg.assign(n, 0);
    }

    // размер графа
    for (int i = 0; i < n; ++i) {
        for (int j = directed ? 0 : i; j < n; ++j) {
            if (a[i][j] == 1) ++s.size;
        }
    }

    for (int i = 0; i < n; ++i) {
        int deg = 0;
        if (directed) {
            int out = 0, in = 0;
            for (int j = 0; j < n; ++j) {
                out += a[i][j];        // дуги i -> j
                in  += a[j][i];        // дуги j -> i
            }
            s.outDeg[i] = out;
            s.inDeg[i] = in;
            deg = out + in;            // петля даёт 1 входящую и 1 исходящую = 2
        } else {
            for (int j = 0; j < n; ++j) deg += a[i][j];
            deg += a[i][i];            // петля даёт вклад 2
        }
        s.degrees[i] = deg;

        if (deg == 0) s.isolated.push_back(i);
        if (a[i][i] == 1) s.loops.push_back(i);

        // доминирующая: (из неё идёт дуга / она смежна) ко всем остальным вершинам
        bool dom = true;
        for (int j = 0; j < n; ++j) {
            if (j != i && a[i][j] == 0) { dom = false; break; }
        }
        if (dom) s.dominating.push_back(i);
    }
    return s;
}

// ---------- Анализ по матрице инцидентности ----------
// Ненаправленный: 1 у концов ребра, 2 у петли.
// Направленный:   +1 у начала дуги, -1 у конца дуги, 2 у петли.
GraphStats analyzeIncidence(const Matrix& inc, bool directed) {
    GraphStats s;
    int n = static_cast<int>(inc.size());
    int m = (n > 0) ? static_cast<int>(inc[0].size()) : 0;
    s.order = n;
    s.size = m;
    s.degrees.assign(n, 0);
    if (directed) {
        s.outDeg.assign(n, 0);
        s.inDeg.assign(n, 0);
    }

    for (int i = 0; i < n; ++i) {
        int deg = 0;
        bool has_loop = false;

        if (directed) {
            int out = 0, in = 0;
            for (int e = 0; e < m; ++e) {
                int v = inc[i][e];
                if (v == 2)       { has_loop = true; ++out; ++in; }  // петля
                else if (v == 1)  { ++out; }                         // дуга выходит
                else if (v == -1) { ++in; }                          // дуга входит
            }
            s.outDeg[i] = out;
            s.inDeg[i] = in;
            deg = out + in;
        } else {
            for (int e = 0; e < m; ++e) {
                deg += inc[i][e];
                if (inc[i][e] == 2) has_loop = true;
            }
        }
        s.degrees[i] = deg;

        if (deg == 0) s.isolated.push_back(i);
        if (has_loop) s.loops.push_back(i);

        // доминирующая: для каждой другой вершины j есть ребро/дуга от i к j
        bool dom = true;
        for (int j = 0; j < n && dom; ++j) {
            if (j == i) continue;
            bool linked = false;
            for (int e = 0; e < m; ++e) {
                if (directed) {
                    if (inc[i][e] == 1 && inc[j][e] == -1) { linked = true; break; }
                } else {
                    if (inc[i][e] > 0 && inc[j][e] > 0) { linked = true; break; }
                }
            }
            if (!linked) dom = false;
        }
        if (dom) s.dominating.push_back(i);
    }
    return s;
}

// ---------- Вывод ----------
void printList(const std::string& title, const std::vector<int>& v) {
    std::cout << title << ": ";
    if (v.empty()) {
        std::cout << "нет";
    } else {
        for (int x : v) std::cout << x << " ";
    }
    std::cout << " (кол-во: " << v.size() << ")\n";
}

void printStats(const std::string& header, const GraphStats& s, bool directed) {
    std::cout << "\n=== " << header << " ===\n";
    std::cout << "Порядок графа (кол-во вершин): " << s.order << "\n";
    std::cout << "Размер графа (кол-во " << (directed ? "дуг" : "рёбер") << "): " << s.size << "\n";

    std::cout << "Степени вершин:\n";
    for (int i = 0; i < s.order; ++i) {
        std::cout << "  Вершина " << i << ": ";
        if (directed) {
            std::cout << "исходящая = " << s.outDeg[i]
                      << ", входящая = " << s.inDeg[i]
                      << ", общая = " << s.degrees[i] << "\n";
        } else {
            std::cout << "степень = " << s.degrees[i] << "\n";
        }
    }

    printList("Изолированные вершины", s.isolated);
    printList("Кольцевые вершины (с петлёй)", s.loops);
    printList(directed ? "Доминирующие вершины (дуги ко всем остальным)" : "Доминирующие вершины",
              s.dominating);
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    // Выбор типа графа
    int type = 0;
    do {
        std::cout << "Выберите тип графа:\n"
                  << "  1 - ненаправленный\n"
                  << "  2 - направленный\n"
                  << "Ваш выбор: ";
        std::cin >> type;
        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            type = 0;
        }
    } while (type != 1 && type != 2);
    bool directed = (type == 2);

    int n;
    std::cout << "Введите размер матрицы (количество вершин): ";
    std::cin >> n;
    if (n <= 0) {
        std::cout << "Количество вершин должно быть положительным.\n";
        return 1;
    }

    double p;
    do {
        std::cout << "Введите вероятность появления ребра (от 0 до 1): ";
        std::cin >> p;
        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            p = -1;
        }
    } while (p < 0.0 || p > 1.0);

    std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution edge_dist(p);

    // Генерация матрицы смежности
    Matrix matrix(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        if (directed) {
            // у орграфа каждая ячейка независима: a[i][j] - дуга i -> j
            for (int j = 0; j < n; ++j) {
                matrix[i][j] = edge_dist(gen) ? 1 : 0;
            }
        } else {
            // у неориентированного графа матрица симметрична
            for (int j = i; j < n; ++j) {
                int edge = edge_dist(gen) ? 1 : 0;
                matrix[i][j] = edge;
                matrix[j][i] = edge;
            }
        }
    }

    std::cout << "\nМатрица смежности "
              << (directed ? "орграфа" : "графа") << ":\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) std::cout << matrix[i][j] << " ";
        std::cout << "\n";
    }

    // Список рёбер (дуг)
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < n; ++i) {
        for (int j = directed ? 0 : i; j < n; ++j) {
            if (matrix[i][j] == 1) edges.push_back({i, j});
        }
    }
    int edge_count = static_cast<int>(edges.size());

    // Матрица инцидентности
    Matrix incidence(n, std::vector<int>(edge_count, 0));
    for (int e = 0; e < edge_count; ++e) {
        int u = edges[e].first;
        int v = edges[e].second;
        if (u == v) {
            incidence[u][e] = 2;           // петля
        } else if (directed) {
            incidence[u][e] = 1;           // начало дуги
            incidence[v][e] = -1;          // конец дуги
        } else {
            incidence[u][e] = 1;
            incidence[v][e] = 1;
        }
    }

    std::cout << "\nМатрица инцидентности (Вершины x "
              << (directed ? "Дуги" : "Рёбра") << "):\n";
    if (edge_count == 0) {
        std::cout << "(рёбер нет)\n";
    } else {
        std::cout << "     ";
        for (int e = 0; e < edge_count; ++e) {
            std::cout << std::setw(4) << ("e" + std::to_string(e));
        }
        std::cout << "\n";
        for (int i = 0; i < n; ++i) {
            std::cout << std::setw(3) << ("v" + std::to_string(i)) << " |";
            for (int e = 0; e < edge_count; ++e) {
                std::cout << std::setw(4) << incidence[i][e];
            }
            std::cout << "\n";
        }
    }

    // Анализ обоими способами
    GraphStats byAdj = analyzeAdjacency(matrix, directed);
    GraphStats byInc = analyzeIncidence(incidence, directed);

    printStats("Анализ по матрице смежности", byAdj, directed);
    printStats("Анализ по матрице инцидентности", byInc, directed);

    // Проверка совпадения результатов
    bool same = byAdj.order == byInc.order &&
                byAdj.size == byInc.size &&
                byAdj.degrees == byInc.degrees &&
                byAdj.outDeg == byInc.outDeg &&
                byAdj.inDeg == byInc.inDeg &&
                byAdj.isolated == byInc.isolated &&
                byAdj.loops == byInc.loops &&
                byAdj.dominating == byInc.dominating;
    std::cout << "\nРезультаты по двум матрицам "
              << (same ? "совпадают." : "НЕ совпадают!") << "\n";

    return 0;
}