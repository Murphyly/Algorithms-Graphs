#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

struct Dados {
    vector<double> tempo;
    vector<long long> iteracoes;
    vector<long long> memoria;
};

struct Estatistica {
    double media;
    double desvio;
    double minimo;
    double maximo;
    int n;
};

Estatistica calcular(const vector<double>& valores) {
    Estatistica e;
    e.n = valores.size();
    
    if (valores.empty()) {
        e.media = e.desvio = e.minimo = e.maximo = 0;
        return e;
    }
    
    e.media = 0;
    e.minimo = valores[0];
    e.maximo = valores[0];
    
    for (double v : valores) {
        e.media += v;
        e.minimo = min(e.minimo, v);
        e.maximo = max(e.maximo, v);
    }
    
    e.media /= valores.size();
    
    e.desvio = 0;
    for (double v : valores) {
        e.desvio += (v - e.media) * (v - e.media);
    }
    
    if (valores.size() > 1) {
        e.desvio = sqrt(e.desvio / valores.size());
    }
    
    return e;
}

int main(int argc, char* argv[]) {
    string arquivo = "resultados_experimento.csv";
    if (argc >= 2) arquivo = argv[1];
    
    cout << "======================================================================" << endl;
    cout << "              ANÁLISE DE RESULTADOS - CICLO HAMILTONIANO" << endl;
    cout << "======================================================================" << endl << endl;
    
    ifstream file(arquivo);
    if (!file.is_open()) {
        cerr << "ERRO: Não foi possível abrir " << arquivo << endl;
        return 1;
    }
    
    // Estrutura: [tipo][vertices][metodo] -> Dados
    map<string, map<int, map<string, Dados>>> database;
    
    string linha;
    getline(file, linha); // Pular cabeçalho
    
    int total_linhas = 0;
    while (getline(file, linha)) {
        if (linha.empty()) continue;
        
        stringstream ss(linha);
        string tipo, metodo, resultado;
        int vertices, instancia;
        double tempo_ms;
        long long iteracoes, memoria_kb;
        
        getline(ss, tipo, ',');
        ss >> vertices; ss.ignore();
        ss >> instancia; ss.ignore();
        getline(ss, metodo, ',');
        getline(ss, resultado, ',');
        ss >> tempo_ms; ss.ignore();
        ss >> iteracoes; ss.ignore();
        ss >> memoria_kb;
        
        database[tipo][vertices][metodo].tempo.push_back(tempo_ms);
        database[tipo][vertices][metodo].iteracoes.push_back(iteracoes);
        database[tipo][vertices][metodo].memoria.push_back(memoria_kb);
        
        total_linhas++;
    }
    
    file.close();
    
    cout << "Carregadas " << total_linhas << " linhas de dados" << endl;
    cout << "======================================================================" << endl << endl;
    
    // ========== TABELA 1: TEMPO MÉDIO ==========
    cout << "TABELA 1: TEMPO MÉDIO DE EXECUÇÃO (ms)" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    vector<string> tipos = {"esparso", "medio_esparso", "medio", "medio_denso", "denso"};
    vector<int> tamanhos = {10, 20, 30, 40, 50};
    vector<string> metodos = {"Backtracking", "Backtracking+Poda", "Prog_Dinamica"};
    
    for (const auto& tipo : tipos) {
        if (database.find(tipo) == database.end()) continue;
        
        cout << "\n" << tipo << ":" << endl;
        cout << "  V   | " << setw(25) << "Backtracking" << " | " 
             << setw(25) << "Backtracking+Poda" << " | " 
             << setw(25) << "Prog_Dinamica" << endl;
        cout << "  " << string(98, '-') << endl;
        
        for (int v : tamanhos) {
            if (database[tipo].find(v) == database[tipo].end()) continue;
            
            cout << "  " << setw(3) << v << " | ";
            
            for (const auto& metodo : metodos) {
                if (database[tipo][v].find(metodo) == database[tipo][v].end() ||
                    database[tipo][v][metodo].tempo.empty()) {
                    cout << setw(25) << "-" << " | ";
                    continue;
                }
                
                auto stats = calcular(database[tipo][v][metodo].tempo);
                
                ostringstream oss;
                oss << fixed << setprecision(2) << stats.media << " ± " << stats.desvio;
                cout << setw(25) << oss.str() << " | ";
            }
            cout << endl;
        }
    }
    
    // ========== TABELA 2: ITERAÇÕES MÉDIAS ==========
    cout << "\n\n";
    cout << "TABELA 2: NÚMERO MÉDIO DE ITERAÇÕES" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    for (const auto& tipo : tipos) {
        if (database.find(tipo) == database.end()) continue;
        
        cout << "\n" << tipo << ":" << endl;
        cout << "  V   | " << setw(20) << "Backtracking" << " | " 
             << setw(20) << "Backtracking+Poda" << endl;
        cout << "  " << string(50, '-') << endl;
        
        for (int v : tamanhos) {
            if (database[tipo].find(v) == database[tipo].end()) continue;
            
            cout << "  " << setw(3) << v << " | ";
            
            for (int i = 0; i < 2; i++) { // Apenas BT e BT+Poda
                string metodo = metodos[i];
                
                if (database[tipo][v].find(metodo) == database[tipo][v].end() ||
                    database[tipo][v][metodo].iteracoes.empty()) {
                    cout << setw(20) << "-" << " | ";
                    continue;
                }
                
                vector<double> iter_double;
                for (auto val : database[tipo][v][metodo].iteracoes) {
                    iter_double.push_back((double)val);
                }
                
                auto stats = calcular(iter_double);
                
                ostringstream oss;
                oss << fixed << setprecision(0) << stats.media;
                cout << setw(20) << oss.str() << " | ";
            }
            cout << endl;
        }
    }
    
    // ========== TABELA 3: SPEEDUP (BT / BT+Poda) ==========
    cout << "\n\n";
    cout << "TABELA 3: SPEEDUP (Backtracking / Backtracking+Poda)" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    for (const auto& tipo : tipos) {
        if (database.find(tipo) == database.end()) continue;
        
        cout << "\n" << tipo << ":" << endl;
        cout << "  V   | Speedup | Redução Iterações (%)" << endl;
        cout << "  " << string(45, '-') << endl;
        
        for (int v : tamanhos) {
            if (database[tipo].find(v) == database[tipo].end()) continue;
            
            auto& bt = database[tipo][v]["Backtracking"];
            auto& poda = database[tipo][v]["Backtracking+Poda"];
            
            if (bt.tempo.empty() || poda.tempo.empty()) continue;
            
            auto stats_bt = calcular(bt.tempo);
            auto stats_poda = calcular(poda.tempo);
            
            vector<double> iter_bt_double, iter_poda_double;
            for (auto val : bt.iteracoes) iter_bt_double.push_back((double)val);
            for (auto val : poda.iteracoes) iter_poda_double.push_back((double)val);
            
            auto iter_bt_stats = calcular(iter_bt_double);
            auto iter_poda_stats = calcular(iter_poda_double);
            
            double speedup = stats_bt.media / stats_poda.media;
            double reducao = 100.0 * (1.0 - iter_poda_stats.media / iter_bt_stats.media);
            
            cout << "  " << setw(3) << v << " | " 
                 << setw(7) << fixed << setprecision(2) << speedup << " | "
                 << setw(20) << fixed << setprecision(1) << reducao << "%" << endl;
        }
    }
    
    // ========== ESTATÍSTICAS GERAIS ==========
    cout << "\n\n";
    cout << "ESTATÍSTICAS GERAIS" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    map<string, int> total_sim, total_nao;
    
    for (const auto& [tipo, vertices_map] : database) {
        for (const auto& [v, metodo_map] : vertices_map) {
            for (const auto& [metodo, dados] : metodo_map) {
                // Aqui precisaríamos do resultado, mas não temos na estrutura Dados
                // Vamos pular essa parte ou ler novamente do CSV
            }
        }
    }
    
    cout << "\nMaior tempo registrado:" << endl;
    double max_tempo = 0;
    string max_config;
    
    for (const auto& [tipo, vertices_map] : database) {
        for (const auto& [v, metodo_map] : vertices_map) {
            for (const auto& [metodo, dados] : metodo_map) {
                auto stats = calcular(dados.tempo);
                if (stats.maximo > max_tempo) {
                    max_tempo = stats.maximo;
                    ostringstream oss;
                    oss << tipo << ", V=" << v << ", " << metodo;
                    max_config = oss.str();
                }
            }
        }
    }
    
    cout << "  " << max_config << ": " << fixed << setprecision(2) 
         << max_tempo << " ms" << endl;
    
    cout << "\n======================================================================" << endl;
    cout << "FIM DA ANÁLISE" << endl;
    cout << "======================================================================" << endl;
    
    return 0;
}