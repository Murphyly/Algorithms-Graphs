#!/bin/bash

# Script de automação completa do experimento
# Uso: ./run_experiment.sh [opcao]

set -e  # Sair em caso de erro

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função de log
log() {
    echo -e "${BLUE}[$(date +'%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✓${NC} $1"
}

warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

error() {
    echo -e "${RED}✗${NC} $1"
    exit 1
}

# Banner
echo "======================================================================"
echo "        EXPERIMENTO - CICLO HAMILTONIANO"
echo "======================================================================"
echo ""

# Verificar se make está disponível
if ! command -v make &> /dev/null; then
    error "Make não encontrado. Compilando manualmente..."
fi

# Função: Compilar tudo
compilar() {
    log "Compilando programas..."
    
    if [ -f "Makefile" ]; then
        make all
    else
        g++ -std=c++17 -O2 -o gerador gerador_grafos.cpp
        g++ -std=c++17 -O2 -o solucao solucao.cpp
        g++ -std=c++17 -O2 -o executor executor_experimentos.cpp
        g++ -std=c++17 -O2 -o analisador analisador.cpp
    fi
    
    success "Compilação concluída"
}

# Função: Gerar instâncias
gerar_instancias() {
    log "Gerando instâncias de grafos..."
    
    if [ ! -f "./gerador" ]; then
        error "Gerador não compilado. Execute: make gerador"
    fi
    
    ./gerador
    
    # Contar arquivos gerados
    num_arquivos=$(ls -1 instancias/*.txt 2>/dev/null | wc -l)
    success "Geradas $num_arquivos instâncias"
}

# Função: Executar experimentos
executar_experimentos() {
    log "Executando experimentos..."
    
    if [ ! -f "./solucao" ]; then
        error "Solver não compilado. Execute: make solucao"
    fi
    
    if [ ! -f "./executor" ]; then
        error "Executor não compilado. Execute: make executor"
    fi
    
    if [ ! -d "instancias" ] || [ -z "$(ls -A instancias)" ]; then
        error "Nenhuma instância encontrada. Execute: make gerar"
    fi
    
    # Executar com timeout de 30 minutos
    timeout 30m ./executor || warning "Alguns experimentos podem ter falhado"
    
    if [ -f "resultados_experimento.csv" ]; then
        num_linhas=$(wc -l < resultados_experimento.csv)
        success "Experimentos concluídos ($((num_linhas - 1)) resultados)"
    else
        error "Arquivo de resultados não gerado"
    fi
}

# Função: Analisar resultados
analisar_resultados() {
    log "Analisando resultados..."
    
    if [ ! -f "./analisador" ]; then
        error "Analisador não compilado. Execute: make analisador"
    fi
    
    if [ ! -f "resultados_experimento.csv" ]; then
        error "Arquivo de resultados não encontrado. Execute experimentos primeiro"
    fi
    
    ./analisador > relatorio_analise.txt
    
    success "Análise concluída (salva em relatorio_analise.txt)"
    
    # Mostrar resumo
    echo ""
    echo "======================================================================"
    cat relatorio_analise.txt
    echo "======================================================================"
}

# Função: Teste rápido
teste_rapido() {
    log "Executando teste rápido..."
    
    if [ ! -f "./solucao" ]; then
        error "Solver não compilado"
    fi
    
    # Gerar um grafo pequeno de teste
    mkdir -p instancias
    echo "5 7
0 1
0 2
1 2
1 3
2 3
2 4
3 4" > instancias/teste.txt
    
    log "Testando com grafo de 5 vértices..."
    ./solucao instancias/teste.txt
    
    success "Teste concluído"
}

# Função: Limpar
limpar() {
    log "Limpando arquivos..."
    
    rm -f gerador solucao executor analisador
    success "Executáveis removidos"
}

# Função: Limpar tudo
limpar_tudo() {
    limpar
    
    log "Removendo resultados e instâncias..."
    rm -rf instancias/
    rm -f resultados_experimento.csv relatorio_analise.txt
    
    success "Todos os arquivos gerados removidos"
}

# Função: Pipeline completo
pipeline_completo() {
    echo "Executando pipeline completo..."
    echo ""
    
    compilar
    echo ""
    
    gerar_instancias
    echo ""
    
    executar_experimentos
    echo ""
    
    analisar_resultados
    echo ""
    
    success "PIPELINE COMPLETO EXECUTADO!"
    
    echo ""
    echo "Arquivos gerados:"
    echo "  - instancias/*.txt (grafos de teste)"
    echo "  - resultados_experimento.csv (dados brutos)"
    echo "  - relatorio_analise.txt (análise estatística)"
}

# Função: Ajuda
mostrar_ajuda() {
    echo "Uso: ./run_experiment.sh [opcao]"
    echo ""
    echo "Opções:"
    echo "  compile      - Compilar todos os programas"
    echo "  gerar        - Gerar instâncias de grafos"
    echo "  executar     - Executar experimentos"
    echo "  analisar     - Analisar resultados"
    echo "  pipeline     - Executar tudo (compile + gerar + executar + analisar)"
    echo "  teste        - Teste rápido"
    echo "  clean        - Limpar executáveis"
    echo "  cleanall     - Limpar tudo"
    echo "  help         - Mostrar esta ajuda"
    echo ""
    echo "Exemplo: ./run_experiment.sh pipeline"
}

# Main
case "${1:-pipeline}" in
    compile)
        compilar
        ;;
    gerar)
        compilar
        gerar_instancias
        ;;
    executar)
        executar_experimentos
        ;;
    analisar)
        analisar_resultados
        ;;
    pipeline)
        pipeline_completo
        ;;
    teste)
        compilar
        teste_rapido
        ;;
    clean)
        limpar
        ;;
    cleanall)
        limpar_tudo
        ;;
    help|--help|-h)
        mostrar_ajuda
        ;;
    *)
        error "Opção inválida: $1. Use 'help' para ver opções disponíveis"
        ;;
esac

echo ""