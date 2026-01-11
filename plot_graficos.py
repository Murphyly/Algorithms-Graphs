#!/usr/bin/env python3
"""
Gera gráficos a partir dos resultados do experimento
Uso: python gerar_graficos.py resultados_experimento.csv
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import sys

# Configurar estilo
sns.set_style("whitegrid")
sns.set_context("paper", font_scale=1.4)
plt.rcParams['figure.dpi'] = 300
plt.rcParams['savefig.dpi'] = 300
plt.rcParams['font.family'] = 'serif'

def carregar_dados(arquivo):
    """Carrega dados do CSV"""
    df = pd.read_csv(arquivo)
    df['tempo_ms'] = pd.to_numeric(df['tempo_ms'])
    df['iteracoes'] = pd.to_numeric(df['iteracoes'])
    df['vertices'] = pd.to_numeric(df['vertices'])
    return df

def grafico1_tempo_vertices(df):
    """Gráfico 1: Tempo vs Vértices (Esparso vs Denso)"""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Filtrar apenas Backtracking
    dados = df[df['metodo'] == 'Backtracking']
    
    # Esparso vs Denso
    for tipo in ['esparso', 'denso']:
        dados_tipo = dados[dados['tipo'] == tipo]
        stats = dados_tipo.groupby('vertices')['tempo_ms'].agg(['mean', 'std']).reset_index()
        
        label = 'Esparso (E ≈ 1.5V)' if tipo == 'esparso' else 'Denso (E ≈ 85% Emax)'
        cor = 'red' if tipo == 'esparso' else 'blue'
        
        ax.plot(stats['vertices'], stats['mean'], 
                marker='o', linewidth=2.5, markersize=8,
                label=label, color=cor)
        
        # Área de desvio padrão
        ax.fill_between(stats['vertices'],
                       stats['mean'] - stats['std'],
                       stats['mean'] + stats['std'],
                       alpha=0.2, color=cor)
    
    ax.set_xlabel('Número de Vértices (V)', fontsize=14, fontweight='bold')
    ax.set_ylabel('Tempo Médio (ms)', fontsize=14, fontweight='bold')
    ax.set_title('Impacto da Densidade no Tempo de Execução\nBacktracking Simples',
                 fontsize=16, fontweight='bold', pad=20)
    ax.legend(fontsize=12, loc='upper left', framealpha=0.9)
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, which='both')
    ax.set_xticks([10, 20, 30, 40, 50])
    
    plt.tight_layout()
    plt.savefig('grafico1_tempo_densidade.png', bbox_inches='tight')
    print("✓ Gráfico 1 salvo: grafico1_tempo_densidade.png")
    plt.close()

def grafico2_speedup_poda(df):
    """Gráfico 2: Speedup da Poda (mostrando que piora)"""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    tipos = ['esparso', 'denso']
    cores = {'esparso': 'red', 'denso': 'blue'}
    
    for tipo in tipos:
        speedups = []
        vertices = []
        
        for v in sorted(df['vertices'].unique()):
            bt = df[(df['tipo'] == tipo) & 
                   (df['metodo'] == 'Backtracking') & 
                   (df['vertices'] == v)]['tempo_ms'].mean()
            
            poda = df[(df['tipo'] == tipo) & 
                     (df['metodo'] == 'Backtracking+Poda') & 
                     (df['vertices'] == v)]['tempo_ms'].mean()
            
            if poda > 0 and not np.isnan(bt) and not np.isnan(poda):
                speedups.append(bt / poda)
                vertices.append(v)
        
        if speedups:
            label = 'Esparso' if tipo == 'esparso' else 'Denso'
            ax.plot(vertices, speedups, marker='o', linewidth=2.5, 
                   markersize=8, label=label, color=cores[tipo])
    
    # Linha de referência (speedup = 1)
    ax.axhline(y=1.0, color='green', linestyle='--', linewidth=2, 
              label='Sem ganho/perda (1.0x)', alpha=0.7)
    
    ax.set_xlabel('Número de Vértices (V)', fontsize=14, fontweight='bold')
    ax.set_ylabel('Speedup (BT / BT+Poda)', fontsize=14, fontweight='bold')
    ax.set_title('Speedup da Poda por Grau\n(valores < 1.0 indicam PIORA)',
                 fontsize=16, fontweight='bold', pad=20)
    ax.legend(fontsize=12, framealpha=0.9)
    ax.grid(True, alpha=0.3)
    ax.set_xticks(sorted(df['vertices'].unique()))
    
    # Destacar região de piora
    ax.axhspan(0, 1.0, alpha=0.1, color='red', label='_nolegend_')
    ax.text(15, 0.5, 'PIORA DE DESEMPENHO', 
           fontsize=11, color='red', alpha=0.6, rotation=0,
           ha='center', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig('grafico2_speedup_poda.png', bbox_inches='tight')
    print("✓ Gráfico 2 salvo: grafico2_speedup_poda.png")
    plt.close()

def grafico3_iteracoes(df):
    """Gráfico 3: Número de Iterações (escala log)"""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Apenas Backtracking, separar por tipo
    dados = df[df['metodo'] == 'Backtracking']
    
    for tipo in ['esparso', 'denso']:
        dados_tipo = dados[dados['tipo'] == tipo]
        stats = dados_tipo.groupby('vertices')['iteracoes'].agg(['mean']).reset_index()
        
        label = 'Esparso' if tipo == 'esparso' else 'Denso'
        cor = 'red' if tipo == 'esparso' else 'blue'
        
        ax.plot(stats['vertices'], stats['mean'], 
                marker='o', linewidth=2.5, markersize=8,
                label=label, color=cor)
    
    ax.set_xlabel('Número de Vértices (V)', fontsize=14, fontweight='bold')
    ax.set_ylabel('Número de Iterações (média)', fontsize=14, fontweight='bold')
    ax.set_title('Crescimento do Espaço de Busca\nBacktracking Simples',
                 fontsize=16, fontweight='bold', pad=20)
    ax.legend(fontsize=12, framealpha=0.9)
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, which='both')
    ax.set_xticks(sorted(df['vertices'].unique()))
    
    # Anotações
    ax.annotate('Crescimento\nexponencial', 
               xy=(30, 4392118), xytext=(25, 1e6),
               arrowprops=dict(arrowstyle='->', color='red', lw=2),
               fontsize=11, color='red', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig('grafico3_iteracoes.png', bbox_inches='tight')
    print("✓ Gráfico 3 salvo: grafico3_iteracoes.png")
    plt.close()

def grafico4_comparacao_metodos(df):
    """Gráfico 4: Comparação entre todos os métodos (apenas denso)"""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Apenas grafos densos (onde todos os métodos foram executados)
    dados = df[df['tipo'] == 'denso']
    
    cores = {
        'Backtracking': 'blue',
        'Backtracking+Poda': 'orange',
        'Prog_Dinamica': 'green'
    }
    
    marcadores = {
        'Backtracking': 'o',
        'Backtracking+Poda': 's',
        'Prog_Dinamica': '^'
    }
    
    for metodo in ['Backtracking', 'Backtracking+Poda', 'Prog_Dinamica']:
        dados_metodo = dados[dados['metodo'] == metodo]
        
        if dados_metodo.empty:
            continue
        
        stats = dados_metodo.groupby('vertices')['tempo_ms'].agg(['mean', 'std']).reset_index()
        
        label_map = {
            'Backtracking': 'Backtracking',
            'Backtracking+Poda': 'BT + Poda',
            'Prog_Dinamica': 'Prog. Dinâmica (DP)'
        }
        
        ax.plot(stats['vertices'], stats['mean'], 
                marker=marcadores[metodo], linewidth=2.5, markersize=8,
                label=label_map[metodo], color=cores[metodo])
        
        ax.fill_between(stats['vertices'],
                       stats['mean'] - stats['std'],
                       stats['mean'] + stats['std'],
                       alpha=0.15, color=cores[metodo])
    
    ax.set_xlabel('Número de Vértices (V)', fontsize=14, fontweight='bold')
    ax.set_ylabel('Tempo Médio (ms)', fontsize=14, fontweight='bold')
    ax.set_title('Comparação de Métodos em Grafos Densos',
                 fontsize=16, fontweight='bold', pad=20)
    ax.legend(fontsize=12, framealpha=0.9, loc='upper left')
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, which='both')
    ax.set_xticks([10, 20, 30, 40, 50])
    
    plt.tight_layout()
    plt.savefig('grafico4_comparacao_metodos.png', bbox_inches='tight')
    print("✓ Gráfico 4 salvo: grafico4_comparacao_metodos.png")
    plt.close()

def grafico5_barras_comparativo(df):
    """Gráfico 5: Barras comparando Esparso vs Denso para V específicos"""
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    vertices_selecionados = [10, 20, 30]
    
    # Gráfico da esquerda: Backtracking
    ax = axes[0]
    dados_bt = df[df['metodo'] == 'Backtracking']
    
    x = np.arange(len(vertices_selecionados))
    largura = 0.35
    
    esparso_tempos = []
    denso_tempos = []
    
    for v in vertices_selecionados:
        t_esp = dados_bt[(dados_bt['tipo'] == 'esparso') & 
                        (dados_bt['vertices'] == v)]['tempo_ms'].mean()
        t_den = dados_bt[(dados_bt['tipo'] == 'denso') & 
                        (dados_bt['vertices'] == v)]['tempo_ms'].mean()
        esparso_tempos.append(t_esp if not np.isnan(t_esp) else 0)
        denso_tempos.append(t_den if not np.isnan(t_den) else 0)
    
    ax.bar(x - largura/2, esparso_tempos, largura, label='Esparso', color='red', alpha=0.7)
    ax.bar(x + largura/2, denso_tempos, largura, label='Denso', color='blue', alpha=0.7)
    
    ax.set_xlabel('Número de Vértices', fontsize=12, fontweight='bold')
    ax.set_ylabel('Tempo Médio (ms)', fontsize=12, fontweight='bold')
    ax.set_title('Backtracking Simples', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(vertices_selecionados)
    ax.legend()
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, axis='y')
    
    # Gráfico da direita: Iterações
    ax = axes[1]
    
    esparso_iter = []
    denso_iter = []
    
    for v in vertices_selecionados:
        i_esp = dados_bt[(dados_bt['tipo'] == 'esparso') & 
                        (dados_bt['vertices'] == v)]['iteracoes'].mean()
        i_den = dados_bt[(dados_bt['tipo'] == 'denso') & 
                        (dados_bt['vertices'] == v)]['iteracoes'].mean()
        esparso_iter.append(i_esp if not np.isnan(i_esp) else 0)
        denso_iter.append(i_den if not np.isnan(i_den) else 0)
    
    ax.bar(x - largura/2, esparso_iter, largura, label='Esparso', color='red', alpha=0.7)
    ax.bar(x + largura/2, denso_iter, largura, label='Denso', color='blue', alpha=0.7)
    
    ax.set_xlabel('Número de Vértices', fontsize=12, fontweight='bold')
    ax.set_ylabel('Iterações (média)', fontsize=12, fontweight='bold')
    ax.set_title('Número de Iterações', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(vertices_selecionados)
    ax.legend()
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig('grafico5_barras_comparativo.png', bbox_inches='tight')
    print("✓ Gráfico 5 salvo: grafico5_barras_comparativo.png")
    plt.close()

def main():
    arquivo = sys.argv[1] if len(sys.argv) > 1 else "resultados_experimento.csv"
    
    print("=" * 70)
    print("              GERADOR DE GRÁFICOS")
    print("=" * 70)
    print()
    
    try:
        df = carregar_dados(arquivo)
        print(f"Dados carregados: {len(df)} linhas\n")
        
        print("Gerando gráficos...")
        grafico1_tempo_vertices(df)
        grafico2_speedup_poda(df)
        grafico3_iteracoes(df)
        grafico4_comparacao_metodos(df)
        grafico5_barras_comparativo(df)
        
        print("\n" + "=" * 70)
        print("GRÁFICOS GERADOS COM SUCESSO!")
        print("=" * 70)
        print("\nArquivos criados:")
        print("  1. grafico1_tempo_densidade.png")
        print("  2. grafico2_speedup_poda.png")
        print("  3. grafico3_iteracoes.png")
        print("  4. grafico4_comparacao_metodos.png")
        print("  5. grafico5_barras_comparativo.png")
        
    except Exception as e:
        print(f"\nERRO: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())