# Experimento: Caminho Hamiltoniano

Sistema completo em C++ para análise experimental de algoritmos de detecção de caminho hamiltoniano.

## Estrutura do Projeto

```
.
├── gerador_grafos.cpp          # Gera instâncias de teste
├── solucao.cpp                 # Implementa 3 algoritmos
├── executor_experimentos.cpp   # Executa todos os testes
├── analisador.cpp              # Analisa resultados
├── Makefile                    # Automação com make
├── run_experiment.sh           # Script bash completo
└── README.md                   # Este arquivo
```

## Uso Rápido

### Opção 1: Usando o Script (Recomendado)

```bash
# Dar permissão de execução
chmod +x run_experiment.sh

# Executar pipeline completo
./run_experiment.sh pipeline
```

### Opção 2: Usando Makefile

```bash
# Pipeline completo
make pipeline

# Ou passo a passo:
make all          # Compila tudo
make gerar        # Gera instâncias
make experimentos # Executa experimentos
make analisar     # Analisa resultados
```

### Opção 3: Manual

```bash
# 1. Compilar
g++ -std=c++17 -O2 -o gerador gerador_grafos.cpp
g++ -std=c++17 -O2 -o solucao solucao.cpp
g++ -std=c++17 -O2 -o executor executor_experimentos.cpp
g++ -std=c++17 -O2 -o analisador analisador.cpp

# 2. Gerar instâncias
./gerador

# 3. Executar experimentos
./executor

# 4. Analisar resultados
./analisador
```

## Componentes

### 1. Gerador de Grafos (`gerador_grafos.cpp`)

Gera instâncias aleatórias de grafos com diferentes características:

**Tamanhos**: 10, 20, 30, 40, 50 vértices

**Tipos de densidade**:

# Comparação: Definição Formal vs Implementação

## Definições Formais (Thulasiraman & Swamy 2011)

- **Emax** = V × (V - 1) / 2
- **Grafo Esparso**: V - 1 ≤ E ≪ Emax (próximo a V)
- **Grafo Denso**: E aproxima-se de Emax

---

## Exemplos Concretos

| Vértices | Emax | Tipo | Fórmula | Arestas | % de Emax | Classificação |
|----------|------|------|---------|---------|-----------|---------------|
| **10** | 45 | Esparso | 1.5×V | 15 | 33% | ✓ Esparso (E ≈ V) |
| 10 | 45 | Médio-esparso | 3×V | 30 | 67% | ✓ Ainda esparso |
| 10 | 45 | Médio | 0.30×Emax | 13 | 30% | Transição |
| 10 | 45 | Médio-denso | 0.60×Emax | 27 | 60% | ✓ Denso |
| 10 | 45 | Denso | 0.85×Emax | 38 | 85% | ✓ Muito denso |
| **20** | 190 | Esparso | 1.5×V | 30 | 16% | ✓ Esparso (E ≈ V) |
| 20 | 190 | Médio-esparso | 3×V | 60 | 32% | ✓ Ainda esparso |
| 20 | 190 | Médio | 0.30×Emax | 57 | 30% | Transição |
| 20 | 190 | Médio-denso | 0.60×Emax | 114 | 60% | ✓ Denso |
| 20 | 190 | Denso | 0.85×Emax | 161 | 85% | ✓ Muito denso |
| **50** | 1225 | Esparso | 1.5×V | 75 | 6% | ✓ Esparso (E ≈ V) |
| 50 | 1225 | Médio-esparso | 3×V | 150 | 12% | ✓ Ainda esparso |
| 50 | 1225 | Médio | 0.30×Emax | 367 | 30% | Transição |
| 50 | 1225 | Médio-denso | 0.60×Emax | 735 | 60% | ✓ Denso |
| 50 | 1225 | Denso | 0.85×Emax | 1041 | 85% | ✓ Muito denso |

---

## Verificação da Definição

### Grafos Esparsos (E ≈ V)

- **Esparso**: E = 1.5V → 15 arestas para V=10 (15 ≈ 10 ✓)
- **Médio-esparso**: E = 3V → 30 arestas para V=10 (30 ≈ 3×10 ✓)
- Ambos satisfazem: **V - 1 ≤ E ≪ Emax** ✓

### Grafos Densos (E → Emax)

- **Médio-denso**: 60% de Emax (bem próximo do máximo)
- **Denso**: 85% de Emax (muito próximo do máximo)
- Ambos **aproximam-se de Emax** ✓

---

## Justificativa para o Relatório

### 3.1.1. Grafos Densos

Segundo [Thulasiraman and Swamy 2011], um grafo é considerado denso quando o número de arestas E aproxima-se do número máximo de arestas possíveis para um grafo completo, dado por:

$$E_{max} = \frac{V \times (V - 1)}{2}$$

Neste trabalho, utilizamos grafos com **60% e 85% de Emax** para representar diferentes graus de densidade. Por exemplo, para um grafo com 50 vértices (Emax = 1225), os grafos densos possuem 735 e 1041 arestas, respectivamente, aproximando-se significativamente do máximo possível.

### 3.1.2. Grafos Esparsos

Um grafo é considerado esparso quando o número de arestas E é significativamente menor que o máximo possível, geralmente situando-se próximo ao número de vértices. A faixa típica para grafos esparsos satisfaz a seguinte condição:

$$V - 1 \leq E \ll \frac{V \times (V - 1)}{2}$$

Utilizamos **E = 1.5V e E = 3V** para representar diferentes graus de esparsidade. Por exemplo, para V=50, os grafos esparsos possuem 75 e 150 arestas (apenas 6% e 12% de Emax), mantendo E proporcional a V e significativamente distante do máximo possível.

### 3.1.3. Grafos Médios

Para fins de análise comparativa, incluímos também grafos de densidade intermediária (30% de Emax), que representam uma transição entre os extremos de esparsidade e densidade, permitindo uma análise mais completa do comportamento dos algoritmos em diferentes cenários.

---

## Resumo dos Parâmetros

```
Esparso:        E = 1.5 × V     (E ≈ V)
Médio-esparso:  E = 3.0 × V     (E ≈ V)
Médio:          E = 0.30 × Emax (transição)
Médio-denso:    E = 0.60 × Emax (E → Emax)
Denso:          E = 0.85 × Emax (E → Emax)
```

**Total de configurações**: 5 densidades × 5 tamanhos × 10 instâncias = **250 grafos**

**Instâncias**: 10 grafos para cada configuração

**Total**: 5 tamanhos × 5 densidades × 10 instâncias = **250 arquivos**

### 2. Solver (`solucao.cpp`)

Implementa **3 algoritmos**:

1. **Backtracking Clássico**
   - Força bruta com retrocesso
   - Explora todas as possibilidades
   - Complexidade: O(n!)

2. **Backtracking com Poda**
   - Heurística: tenta vértices com menor grau primeiro
   - Reduz espaço de busca
   - Geralmente 2-10x mais rápido

3. **Programação Dinâmica (Held-Karp)**
   - Usa bitmask para estados visitados
   - Complexidade: O(n² × 2^n)
   - Limite: n ≤ 22 (restrição de memória)

**Métricas coletadas**:
- Tempo de execução (ms)
- Número de iterações
- Memória consumida (KB)
- Resultado (SIM/NÃO)

### 3. Executor (`executor_experimentos.cpp`)

- Executa automaticamente todos os testes
- Gera arquivo CSV com todos os resultados
- Mostra progresso em tempo real
- Tratamento de erros e timeouts

### 4. Analisador (`analisador.cpp`)

Gera 3 tabelas principais:

**Tabela 1**: Tempo médio ± desvio padrão por configuração

**Tabela 2**: Número médio de iterações

**Tabela 3**: Speedup (comparação BT vs BT+Poda)

## Formato de Entrada

```
n m
u1 v1
u2 v2
...
um vm
```

**Exemplo**:
```
5 7
0 1
0 2
1 2
1 3
2 3
2 4
3 4
```

## Arquivos Gerados

### Durante a execução:

```
instancias/
├── esparso_v10_inst1.txt
├── esparso_v10_inst2.txt
├── ...
├── denso_v50_inst10.txt
```

### Resultados:

```
resultados_experimento.csv    # Dados brutos (CSV)
relatorio_analise.txt          # Análise estatística
```

## Exemplo de Saída CSV

```csv
tipo,vertices,instancia,metodo,resultado,tempo_ms,iteracoes,memoria_kb
esparso,10,1,Backtracking,SIM,0.234,1523,128
esparso,10,1,Backtracking+Poda,SIM,0.156,891,112
esparso,10,1,Prog_Dinamica,SIM,0.089,1024,2048
```

## Comandos Úteis

### Script Bash:

```bash
./run_experiment.sh compile     # Apenas compilar
./run_experiment.sh gerar       # Apenas gerar instâncias
./run_experiment.sh executar    # Apenas executar experimentos
./run_experiment.sh analisar    # Apenas analisar
./run_experiment.sh teste       # Teste rápido
./run_experiment.sh clean       # Limpar executáveis
./run_experiment.sh cleanall    # Limpar tudo
./run_experiment.sh help        # Ajuda
```

### Makefile:

```bash
make all          # Compilar tudo
make gerar        # Gerar instâncias
make experimentos # Executar experimentos
make analisar     # Analisar resultados
make pipeline     # Tudo (gerar + executar + analisar)
make teste        # Teste rápido
make clean        # Limpar executáveis
make cleanall     # Limpar tudo
make help         # Ajuda
```

### Executar um arquivo específico:

```bash
./solucao instancias/denso_v20_inst5.txt
```

### Modo CSV (para scripting):

```bash
./solucao arquivo.txt --csv
```

## Requisitos

- **Compilador**: g++ com suporte a C++17
- **Sistema**: Linux, macOS, ou Windows (com MinGW/WSL)
- **Make**: Opcional, mas recomendado
- **Bash**: Para usar o script de automação

## Para o Relatório

Este sistema fornece:

### Dados Experimentais:
- Múltiplas instâncias para cada configuração (estatisticamente significativo)
- Média e desvio padrão de todas as métricas
- Comparação entre 3 algoritmos diferentes
- Análise de escalabilidade (5 tamanhos)
- Impacto da densidade (5 tipos)

### Tabelas:
1. Tempo médio de execução por configuração
2. Número de iterações por algoritmo
3. Speedup (ganho de performance)

### Gráficos Recomendados:
Para criar gráficos, use os dados do CSV em Excel, Google Sheets, ou Python/matplotlib:

1. **Tempo × Vértices** (escala log)
2. **Iterações × Vértices** (comparar métodos)
3. **Boxplot** de tempos (distribuição)
4. **Speedup** (BT / BT+Poda)

### Análise de Complexidade:
Compare os resultados experimentais com as complexidades teóricas:
- Backtracking: O(n!)
- DP: O(n² × 2^n)

## Solução de Problemas

### Erro de compilação:
```bash
# Verificar versão do g++
g++ --version

# Deve ser >= 7.0 para C++17
```

### Permissão negada:
```bash
chmod +x run_experiment.sh
chmod +x gerador solucao executor analisador
```

### Falta memória:
Reduza o número de instâncias ou tamanhos em `gerador_grafos.cpp`

## Licença
