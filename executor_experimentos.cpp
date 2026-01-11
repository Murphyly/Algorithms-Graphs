#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define popen _popen
#define pclose _pclose
#else
#include <dirent.h>
#include <sys/wait.h>
#endif

using namespace std;

struct ResultadoExperimento {
    string arquivo;
    string tipo;
    int vertices;
    int instancia;
    string metodo;
    string resultado;
    double tempo_ms;
    long long iteracoes;
    long long memoria_kb;
};

// Extrair informações do nome do arquivo
void extrairInfo(const string& filename, string& tipo, int& vertices, int& instancia) {
    // Remove path e extensão
    size_t pos = filename.find_last_of("/\\");
    string base = (pos != string::npos) ? filename.substr(pos + 1) : filename;
    base = base.substr(0, base.find(".txt"));
    
    // Parse: tipo_vNN_instM
    stringstream ss(base);
    string temp;
    
    getline(ss, tipo, '_');
    getline(ss, temp, '_');
    vertices = stoi(temp.substr(1)); // Remove 'v'
    getline(ss, temp, '_');
    instancia = stoi(temp.substr(4)); // Remove 'inst'
}

// Executar programa e capturar saída
vector<ResultadoExperimento> executarPrograma(const string& exe, const string& arquivo) {
    vector<ResultadoExperimento> resultados;
    
    string comando = exe + " " + arquivo + " --csv";
    FILE* pipe = popen(comando.c_str(), "r");
    
    if (!pipe) {
        cerr << "Erro ao executar: " << comando << endl;
        return resultados;
    }
    
    char buffer[1024];
    string saida;
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        saida += buffer;
    }
    
    pclose(pipe);
    
    // Parse da saída CSV
    stringstream ss(saida);
    string linha;
    bool csv_section = false;
    
    string tipo_arq;
    int vertices_arq, instancia_arq;
    extrairInfo(arquivo, tipo_arq, vertices_arq, instancia_arq);
    
    while (getline(ss, linha)) {
        if (linha.find("metodo,resultado") != string::npos) {
            csv_section = true;
            continue;
        }
        
        if (csv_section && !linha.empty()) {
            stringstream ls(linha);
            ResultadoExperimento r;
            
            r.arquivo = arquivo;
            r.tipo = tipo_arq;
            r.vertices = vertices_arq;
            r.instancia = instancia_arq;
            
            string temp;
            getline(ls, r.metodo, ',');
            getline(ls, r.resultado, ',');
            getline(ls, temp, ','); r.tempo_ms = stod(temp);
            getline(ls, temp, ','); r.iteracoes = stoll(temp);
            getline(ls, temp, ','); r.memoria_kb = stoll(temp);
            
            resultados.push_back(r);
        }
    }
    
    return resultados;
}

// Listar arquivos em diretório
vector<string> listarArquivos(const string& dir) {
    vector<string> arquivos;
    
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((dir + "\\*.txt").c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            arquivos.push_back(dir + "/" + findData.cFileName);
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dp = opendir(dir.c_str());
    if (dp) {
        struct dirent* entry;
        while ((entry = readdir(dp))) {
            string nome = entry->d_name;
            if (nome.size() > 4 && nome.substr(nome.size() - 4) == ".txt") {
                arquivos.push_back(dir + "/" + nome);
            }
        }
        closedir(dp);
    }
#endif
    
    sort(arquivos.begin(), arquivos.end());
    return arquivos;
}

// Calcular estatísticas
struct Estatistica {
    double media;
    double desvio;
    double minimo;
    double maximo;
};

Estatistica calcularEstatistica(const vector<double>& valores) {
    Estatistica e;
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
    e.desvio = sqrt(e.desvio / valores.size());
    
    return e;
}

int main(int argc, char* argv[]) {
    cout << "======================================================================" << endl;
    cout << "           EXECUTOR DE EXPERIMENTOS - CICLO HAMILTONIANO" << endl;
    cout << "======================================================================" << endl << endl;
    
    string executavel = "./solucao";
    string pasta = "instancias";
    string arquivo_saida = "resultados_experimento.csv";
    
    if (argc >= 2) executavel = argv[1];
    if (argc >= 3) pasta = argv[2];
    
    // Listar arquivos
    vector<string> arquivos = listarArquivos(pasta);
    
    if (arquivos.empty()) {
        cerr << "ERRO: Nenhum arquivo encontrado em '" << pasta << "/'" << endl;
        return 1;
    }
    
    cout << "Encontrados " << arquivos.size() << " arquivos de teste" << endl;
    cout << "Executável: " << executavel << endl;
    cout << "Salvando em: " << arquivo_saida << endl << endl;
    
    // Abrir arquivo de saída
    ofstream csv(arquivo_saida);
    csv << "tipo,vertices,instancia,metodo,resultado,tempo_ms,iteracoes,memoria_kb\n";
    
    vector<ResultadoExperimento> todos_resultados;
    int processados = 0;
    
    // Executar experimentos
    for (size_t i = 0; i < arquivos.size(); i++) {
        string nome_curto = arquivos[i].substr(arquivos[i].find_last_of("/\\") + 1);
        
        cout << "[" << setw(3) << (i+1) << "/" << setw(3) << arquivos.size() << "] " 
             << setw(40) << left << nome_curto << " ";
        cout.flush();
        
        auto resultados = executarPrograma(executavel, arquivos[i]);
        
        if (!resultados.empty()) {
            for (const auto& r : resultados) {
                csv << r.tipo << "," << r.vertices << "," << r.instancia << ","
                    << r.metodo << "," << r.resultado << "," << r.tempo_ms << ","
                    << r.iteracoes << "," << r.memoria_kb << "\n";
                
                todos_resultados.push_back(r);
            }
            cout << "✓ " << resultados.size() << " métodos" << endl;
            processados++;
        } else {
            cout << "✗ Falhou" << endl;
        }
    }
    
    csv.close();
    
    cout << "\n======================================================================" << endl;
    cout << "EXPERIMENTOS CONCLUÍDOS!" << endl;
    cout << "Processados: " << processados << "/" << arquivos.size() << " arquivos" << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
    cout << "======================================================================" << endl;
    
    // Análise rápida
    cout << "\nANÁLISE RÁPIDA - TEMPO MÉDIO (ms):" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    map<string, map<int, map<string, vector<double>>>> dados;
    
    for (const auto& r : todos_resultados) {
        dados[r.tipo][r.vertices][r.metodo].push_back(r.tempo_ms);
    }
    
    vector<string> tipos = {"esparso", "medio_esparso", "medio", "medio_denso", "denso"};
    vector<int> vertices = {10, 20, 30, 40, 50};
    vector<string> metodos = {"Backtracking", "Backtracking+Poda", "Prog_Dinamica"};
    
    for (const auto& tipo : tipos) {
        bool tem_dados = false;
        for (int v : vertices) {
            if (dados[tipo][v].size() > 0) {
                tem_dados = true;
                break;
            }
        }
        
        if (!tem_dados) continue;
        
        cout << "\n" << tipo << ":" << endl;
        
        for (int v : vertices) {
            if (dados[tipo][v].empty()) continue;
            
            cout << "  V=" << setw(2) << v << ": ";
            
            for (const auto& metodo : metodos) {
                if (dados[tipo][v][metodo].empty()) continue;
                
                auto stats = calcularEstatistica(dados[tipo][v][metodo]);
                
                cout << setw(20) << left << metodo << ": " 
                     << setw(8) << right << fixed << setprecision(2) << stats.media 
                     << " ± " << setw(6) << stats.desvio << " ms    ";
            }
            cout << endl;
        }
    }
    
    cout << "\n======================================================================" << endl;
    
    return 0;
}