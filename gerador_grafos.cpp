#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <random>
#include <cmath>
#include <iomanip>
#include <sys/stat.h>

using namespace std;

// Criar diretório (multiplataforma)
void criarDiretorio(const string& nome) {
#ifdef _WIN32
    mkdir(nome.c_str());
#else
    mkdir(nome.c_str(), 0777);
#endif
}

// Gerar aresta aleatória única
pair<int, int> indexParaAresta(int index, int n) {
    // Converte índice linear para par (u, v)
    int u = (int)((1 + sqrt(1 + 8.0 * index)) / 2);
    int v = index - (u * (u - 1)) / 2;
    return {u, v};
}

void gerarGrafo(int V, int E, const string& filename) {
    // Calcular número máximo de arestas
    int max_arestas = (V * (V - 1)) / 2;
    
    if (E > max_arestas) {
        E = max_arestas;
    }
    
    // Gerador aleatório
    random_device rd;
    mt19937 gen(rd());
    
    // Selecionar E arestas únicas aleatoriamente
    set<int> arestas_selecionadas;
    uniform_int_distribution<> dis(0, max_arestas - 1);
    
    while (arestas_selecionadas.size() < (size_t)E) {
        arestas_selecionadas.insert(dis(gen));
    }
    
    // Escrever no arquivo
    ofstream file(filename);
    file << V << " " << E << "\n";
    
    for (int idx : arestas_selecionadas) {
        auto [u, v] = indexParaAresta(idx, V);
        file << u << " " << v << "\n";
    }
    
    file.close();
}

int main() {
    cout << "========================================" << endl;
    cout << "  GERADOR DE INSTÂNCIAS DE GRAFOS" << endl;
    cout << "========================================" << endl << endl;
    
    // Criar diretório
    criarDiretorio("instancias");
    
    // Configurações
    vector<int> tamanhos = {10, 20, 30, 40, 50};
    int num_instancias = 10;
    
    struct Config {
        string nome;
        double fator;
        bool is_multiplicador; // true = E = fator*V, false = E = fator*Emax
    };
    
    // Definições baseadas em Thulasiraman and Swamy 2011
    vector<Config> configs = {
        {"esparso", 1.5, true},      // E ≈ 1.5V (bem próximo de V)
        {"medio_esparso", 3.0, true}, // E ≈ 3V (ainda esparso, mas menos)
        {"medio", 0.30, false},       // E = 30% de Emax (transição)
        {"medio_denso", 0.60, false}, // E = 60% de Emax (aproximando denso)
        {"denso", 0.85, false}        // E = 85% de Emax (bem denso)
    };
    
    int total = 0;
    
    for (int v : tamanhos) {
        int max_e = (v * (v - 1)) / 2;
        
        cout << "\nVértices: " << v << " (Emax = " << max_e << ")" << endl;
        
        for (const auto& cfg : configs) {
            int num_arestas;
            
            // Calcular número de arestas baseado no tipo
            if (cfg.is_multiplicador) {
                // Grafos esparsos: E próximo a V (fórmula: E ≈ c*V)
                num_arestas = (int)(v * cfg.fator);
            } else {
                // Grafos densos: E aproxima-se de Emax (fórmula: E = c*Emax)
                num_arestas = (int)(max_e * cfg.fator);
            }
            
            // Limites
            num_arestas = max(v - 1, num_arestas);  // Mínimo para conectividade
            num_arestas = min(max_e, num_arestas);   // Máximo possível
            
            double percentual = 100.0 * num_arestas / max_e;
            
            cout << "  " << setw(15) << left << cfg.nome << ": " 
                 << setw(5) << right << num_arestas << " arestas ("
                 << setw(5) << fixed << setprecision(1) << percentual << "%)";
            
            // Gerar múltiplas instâncias
            for (int i = 1; i <= num_instancias; i++) {
                string filename = "instancias/" + cfg.nome + "_v" + 
                                 to_string(v) + "_inst" + to_string(i) + ".txt";
                gerarGrafo(v, num_arestas, filename);
                total++;
            }
            
            cout << " → " << num_instancias << " instâncias ✓" << endl;
        }
    }
    
    cout << "\n========================================" << endl;
    cout << "Total: " << tamanhos.size() << " tamanhos × " 
         << configs.size() << " tipos × " << num_instancias << " instâncias" << endl;
    cout << "     = " << total << " arquivos gerados" << endl;
    cout << "\nArquivos salvos em: ./instancias/" << endl;
    cout << "========================================" << endl;
    
    return 0;
}