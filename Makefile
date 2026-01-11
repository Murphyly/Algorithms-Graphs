CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

# Alvos principais
TARGETS = gerador solucao executor analisador

all: $(TARGETS)

# Gerador de grafos
gerador: gerador_grafos.cpp
	$(CXX) $(CXXFLAGS) -o gerador gerador_grafos.cpp
	@echo "✓ Gerador compilado"

# Solver principal
solucao: solucao.cpp
	$(CXX) $(CXXFLAGS) -o solucao solucao.cpp
	@echo "✓ Solver compilado"

# Executor de experimentos
executor: executor_experimentos.cpp
	$(CXX) $(CXXFLAGS) -o executor executor_experimentos.cpp
	@echo "✓ Executor compilado"

# Analisador de resultados
analisador: analisador.cpp
	$(CXX) $(CXXFLAGS) -o analisador analisador.cpp
	@echo "✓ Analisador compilado"

# Gerar instâncias
gerar: gerador
	@echo "Gerando instâncias de grafos..."
	./gerador
	@echo "✓ Instâncias geradas"

# Executar experimentos
experimentos: solucao executor
	@echo "Executando experimentos..."
	./executor
	@echo "✓ Experimentos concluídos"

# Analisar resultados
analisar: analisador
	@echo "Analisando resultados..."
	./analisador
	@echo "✓ Análise concluída"

# Pipeline completo
pipeline: all gerar experimentos analisar
	@echo ""
	@echo "======================================"
	@echo "  PIPELINE COMPLETO EXECUTADO!"
	@echo "======================================"

# Teste rápido (apenas instâncias pequenas)
teste: solucao
	@echo "Executando teste rápido..."
	./solucao instancias/esparso_v10_inst1.txt
	./solucao instancias/denso_v10_inst1.txt

# Limpar arquivos compilados
clean:
	rm -f $(TARGETS)
	@echo "✓ Arquivos compilados removidos"

# Limpar tudo (incluindo resultados)
cleanall: clean
	rm -rf instancias/
	rm -f resultados_experimento.csv
	@echo "✓ Todos os arquivos gerados removidos"

# Ajuda
help:
	@echo "Uso do Makefile:"
	@echo "  make all          - Compila todos os programas"
	@echo "  make gerar        - Gera instâncias de grafos"
	@echo "  make experimentos - Executa todos os experimentos"
	@echo "  make analisar     - Analisa os resultados"
	@echo "  make pipeline     - Executa tudo (gerar + experimentos + análise)"
	@echo "  make teste        - Teste rápido com grafos pequenos"
	@echo "  make clean        - Remove executáveis"
	@echo "  make cleanall     - Remove tudo"

.PHONY: all gerar experimentos analisar pipeline teste clean cleanall help