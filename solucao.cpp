#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

using namespace std;

// === ESTRUTURAS ===
struct Grafo {
    int n, m;
    vector<vector<int>> adj;
    
    Grafo() : n(0), m(0) {}
};

struct Metricas {
    bool resultado;
    double tempo_ms;
    long long iteracoes;
    long long memoria_kb;
    string metodo;
    
    void imprimir() const {
        cout << fixed << setprecision(3);
        cout << "  Metodo: " << metodo << endl;
        cout << "  Resultado: " << (resultado ? "SIM" : "NAO") << endl;
        cout << "  Tempo: " << tempo_ms << " ms" << endl;
        cout << "  Iteracoes: " << iteracoes << endl;
        cout << "  Memoria: " << memoria_kb << " KB" << endl;
    }
    
    string toCSV() const {
        ostringstream oss;
        oss << metodo << "," << (resultado ? "SIM" : "NAO") << "," 
            << fixed << setprecision(3) << tempo_ms << "," 
            << iteracoes << "," << memoria_kb;
        return oss.str();
    }
};

// === UTILITÁRIOS ===
long long getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
#endif
}

// === LEITURA DE ARQUIVO ===
Grafo lerGrafo(const string& nomeArquivo) {
    ifstream file(nomeArquivo);
    Grafo g;
    
    if (!file.is_open()) {
        cerr << "ERRO: Nao foi possivel abrir o arquivo: " << nomeArquivo << endl;
        return g;
    }
    
    file >> g.n >> g.m;
    
    if (g.n <= 0 || g.m < 0) {
        cerr << "ERRO: Valores invalidos n=" << g.n << " m=" << g.m << endl;
        g.n = 0;
        return g;
    }
    
    g.adj.assign(g.n, vector<int>(g.n, 0));
    
    for (int i = 0; i < g.m; i++) {
        int u, v;
        file >> u >> v;
        
        if (u < 0 || u >= g.n || v < 0 || v >= g.n) {
            cerr << "AVISO: Aresta invalida (" << u << "," << v << ") ignorada" << endl;
            continue;
        }
        
        g.adj[u][v] = 1;
        g.adj[v][u] = 1;
    }
    
    file.close();
    return g;
}

// === SOLUÇÃO 1: BACKTRACKING PARA CAMINHO HAMILTONIANO ===
bool isSafe(int v, const Grafo& g, const vector<int>& path, int pos) {
    if (g.adj[path[pos - 1]][v] == 0) return false;
    
    for (int i = 0; i < pos; i++)
        if (path[i] == v) return false;
    
    return true;
}

bool hamPathBacktrackingUtil(const Grafo& g, vector<int>& path, int pos, long long& iteracoes) {
    iteracoes++;
    
    // CAMINHO: Se visitou todos os vértices, encontrou o caminho (sem retornar)
    if (pos == g.n) {
        return true;
    }
    
    for (int v = 0; v < g.n; v++) {
        if (isSafe(v, g, path, pos)) {
            path[pos] = v;
            
            if (hamPathBacktrackingUtil(g, path, pos + 1, iteracoes))
                return true;
            
            path[pos] = -1;
        }
    }
    
    return false;
}

Metricas hamPathBacktracking(const Grafo& g) {
    Metricas m;
    m.metodo = "Backtracking";
    m.iteracoes = 0;
    m.resultado = false;
    
    // Testar começando de cada vértice
    for (int start = 0; start < g.n && !m.resultado; start++) {
        vector<int> path(g.n, -1);
        path[0] = start;
        
        long long mem_inicial = getMemoryUsage();
        auto inicio = chrono::high_resolution_clock::now();
        
        if (hamPathBacktrackingUtil(g, path, 1, m.iteracoes)) {
            m.resultado = true;
        }
        
        auto fim = chrono::high_resolution_clock::now();
        long long mem_final = getMemoryUsage();
        
        m.tempo_ms += chrono::duration<double, milli>(fim - inicio).count();
        m.memoria_kb = max(m.memoria_kb, mem_final - mem_inicial);
        
        if (m.resultado) break;
    }
    
    return m;
}

// === SOLUÇÃO 2: BACKTRACKING COM PODA ===
bool hamPathBacktrackingPodaUtil(const Grafo& g, vector<int>& path, 
                                  vector<bool>& visitado, int pos, long long& iteracoes) {
    iteracoes++;
    
    // CAMINHO: Se visitou todos, encontrou
    if (pos == g.n) {
        return true;
    }
    
    // Heurística: ordenar candidatos por grau disponível
    vector<pair<int, int>> candidatos;
    
    for (int v = 0; v < g.n; v++) {
        if (!visitado[v] && g.adj[path[pos - 1]][v] == 1) {
            int grau = 0;
            for (int u = 0; u < g.n; u++) {
                if (!visitado[u] && g.adj[v][u] == 1)
                    grau++;
            }
            candidatos.push_back({grau, v});
        }
    }
    
    sort(candidatos.begin(), candidatos.end());
    
    for (const auto& p : candidatos) {
        int v = p.second;
        path[pos] = v;
        visitado[v] = true;
        
        if (hamPathBacktrackingPodaUtil(g, path, visitado, pos + 1, iteracoes))
            return true;
        
        visitado[v] = false;
        path[pos] = -1;
    }
    
    return false;
}

Metricas hamPathBacktrackingPoda(const Grafo& g) {
    Metricas m;
    m.metodo = "Backtracking+Poda";
    m.iteracoes = 0;
    m.resultado = false;
    
    // Testar começando de cada vértice
    for (int start = 0; start < g.n && !m.resultado; start++) {
        vector<int> path(g.n, -1);
        vector<bool> visitado(g.n, false);
        path[0] = start;
        visitado[start] = true;
        
        long long mem_inicial = getMemoryUsage();
        auto inicio = chrono::high_resolution_clock::now();
        
        if (hamPathBacktrackingPodaUtil(g, path, visitado, 1, m.iteracoes)) {
            m.resultado = true;
        }
        
        auto fim = chrono::high_resolution_clock::now();
        long long mem_final = getMemoryUsage();
        
        m.tempo_ms += chrono::duration<double, milli>(fim - inicio).count();
        m.memoria_kb = max(m.memoria_kb, mem_final - mem_inicial);
        
        if (m.resultado) break;
    }
    
    return m;
}

// === SOLUÇÃO 3: PROGRAMAÇÃO DINÂMICA PARA CAMINHO HAMILTONIANO ===
Metricas hamPathDP(const Grafo& g) {
    Metricas m;
    m.metodo = "Prog_Dinamica";
    m.iteracoes = 0;
    
    int n = g.n;
    
    if (n > 22) {
        m.resultado = false;
        m.tempo_ms = 0;
        m.memoria_kb = 0;
        return m;
    }
    
    long long mem_inicial = getMemoryUsage();
    auto start = chrono::high_resolution_clock::now();
    
    // dp[mask][i] = pode visitar conjunto 'mask' terminando em 'i'
    vector<vector<bool>> dp(1 << n, vector<bool>(n, false));
    
    // CAMINHO: Pode começar de qualquer vértice
    for (int i = 0; i < n; i++) {
        dp[1 << i][i] = true;
    }
    
    for (int mask = 1; mask < (1 << n); mask++) {
        m.iteracoes++;
        for (int i = 0; i < n; i++) {
            if ((mask & (1 << i)) && dp[mask][i]) {
                for (int j = 0; j < n; j++) {
                    if (!(mask & (1 << j)) && g.adj[i][j]) {
                        dp[mask | (1 << j)][j] = true;
                    }
                }
            }
        }
    }
    
    // CAMINHO: Verifica se existe caminho que visita todos (termina em qualquer vértice)
    m.resultado = false;
    for (int i = 0; i < n; i++) {
        if (dp[(1 << n) - 1][i]) {
            m.resultado = true;
            break;
        }
    }
    
    auto end = chrono::high_resolution_clock::now();
    long long mem_final = getMemoryUsage();
    
    m.tempo_ms = chrono::duration<double, milli>(end - start).count();
    m.memoria_kb = max(0LL, mem_final - mem_inicial);
    
    return m;
}

// === FUNÇÃO PRINCIPAL ===
int main(int argc, char* argv[]) {
    bool csv_mode = false;
    
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--csv") {
            csv_mode = true;
        }
    }
    
    if (!csv_mode) {
        cout << "========================================" << endl;
        cout << "  DETECTOR DE CAMINHO HAMILTONIANO" << endl;
        cout << "========================================" << endl << endl;
    }
    
    if (argc < 2 || (argc == 2 && csv_mode)) {
        cout << "Uso: " << argv[0] << " <arquivo_grafo> [--csv]" << endl;
        return 1;
    }
    
    string arquivo = argv[1];
    Grafo g = lerGrafo(arquivo);
    
    if (g.n == 0) return 1;
    
    if (!csv_mode) {
        cout << "Arquivo: " << arquivo << endl;
        cout << "Vertices: " << g.n << " | Arestas: " << g.m << endl;
        double densidade = (g.n > 1) ? (2.0 * g.m) / (g.n * (g.n - 1)) : 0;
        cout << "Densidade: " << fixed << setprecision(2) << (densidade * 100) << "%" << endl;
        cout << "========================================" << endl << endl;
    }
    
    vector<Metricas> resultados;
    
    // Executar algoritmos
    if (!csv_mode) cout << "[1/3] Executando Backtracking..." << endl;
    resultados.push_back(hamPathBacktracking(g));
    if (!csv_mode) {
        resultados.back().imprimir();
        cout << endl;
    }
    
    if (!csv_mode) cout << "[2/3] Executando Backtracking com Poda..." << endl;
    resultados.push_back(hamPathBacktrackingPoda(g));
    if (!csv_mode) {
        resultados.back().imprimir();
        cout << endl;
    }
    
    if (g.n <= 22) {
        if (!csv_mode) cout << "[3/3] Executando Programacao Dinamica..." << endl;
        resultados.push_back(hamPathDP(g));
        if (!csv_mode) resultados.back().imprimir();
    } else {
        if (!csv_mode) cout << "[3/3] Programacao Dinamica pulada (n > 22)" << endl;
    }
    
    // Modo CSV
    if (csv_mode) {
        cout << "metodo,resultado,tempo_ms,iteracoes,memoria_kb" << endl;
        for (const auto& m : resultados) {
            cout << m.toCSV() << endl;
        }
    }
    
    return 0;
}