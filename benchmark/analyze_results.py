#!/usr/bin/env python3
"""
Script per analizzare i risultati del benchmark Boids.

Confronta le performance tra:
- AoS (Array of Structures)
- Parallel (con vari thread, dove 1 thread equivale a SoA)

Nota: SoA non è più benchmarked separatamente perché equivale
esattamente a Parallel con 1 thread.
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys

def load_data(filename="benchmark_results.csv"):
    """Carica i dati del benchmark"""
    try:
        df = pd.read_csv(filename)
        print(f"Dati caricati da {filename}: {len(df)} righe")
        return df
    except FileNotFoundError:
        print(f"Errore: File {filename} non trovato!")
        print("Esegui prima il benchmark per generare i risultati")
        sys.exit(1)

def plot_aos_vs_soa(df):
    """Confronto AoS vs SoA (Parallel con 1 thread)"""
    aos_data = df[(df['Implementation'] == 'AoS') & (df['Threads'] == 1)]
    soa_data = df[(df['Implementation'] == 'Parallel') & (df['Threads'] == 1)]
    
    plt.figure(figsize=(12, 6))
    
    # Tempi di esecuzione AoS vs SoA
    plt.subplot(1, 2, 1)
    
    if not aos_data.empty:
        aos_sorted = aos_data.sort_values('Boids')
        plt.plot(aos_sorted['Boids'], aos_sorted['Mean_Time_ms'], 'ro-', label='AoS', linewidth=2, markersize=8)
    if not soa_data.empty:
        soa_sorted = soa_data.sort_values('Boids')
        plt.plot(soa_sorted['Boids'], soa_sorted['Mean_Time_ms'], 'bs-', label='SoA (Parallel 1T)', linewidth=2, markersize=8)
    
    plt.xlabel('Numero di Boids')
    plt.ylabel('Tempo Medio (ms)')
    plt.title('Tempi di Esecuzione: AoS vs SoA')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Speedup SoA vs AoS
    plt.subplot(1, 2, 2)
    
    if not aos_data.empty and not soa_data.empty:
        merged = pd.merge(aos_data[['Boids', 'Mean_Time_ms']], 
                         soa_data[['Boids', 'Mean_Time_ms']], 
                         on='Boids', suffixes=('_aos', '_soa'), how='inner')
        
        if not merged.empty:
            merged_sorted = merged.sort_values('Boids')
            speedup_sorted = (merged_sorted['Mean_Time_ms_aos'] / merged_sorted['Mean_Time_ms_soa'])
            plt.plot(merged_sorted['Boids'], speedup_sorted, 'b^-', label='SoA vs AoS', linewidth=2, markersize=8)
        else:
            plt.text(0.5, 0.5, 'Nessun dato\ncomune trovato', ha='center', va='center', transform=plt.gca().transAxes)
    else:
        plt.text(0.5, 0.5, 'Dati AoS o SoA\nmancanti', ha='center', va='center', transform=plt.gca().transAxes)
    
    plt.axhline(y=1, color='r', linestyle='--', alpha=0.7, label='No speedup')
    plt.xlabel('Numero di Boids')
    plt.ylabel('Speedup')
    plt.title('Speedup SoA vs AoS')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('aos_vs_soa.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_parallel_vs_soa(df):
    """Confronto Parallel vs SoA (Parallel con 1 thread)"""
    parallel_data = df[df['Implementation'] == 'Parallel']
    soa_data = df[(df['Implementation'] == 'Parallel') & (df['Threads'] == 1)]
    
    plt.figure(figsize=(12, 6))
    
    # Tempi di esecuzione Parallel vs SoA
    plt.subplot(1, 2, 1)
    
    # SoA baseline (Parallel con 1 thread)
    if not soa_data.empty:
        plt.plot(soa_data['Boids'], soa_data['Mean_Time_ms'], 'b-', linewidth=3, label='SoA (Parallel 1T)', marker='s', markersize=8)
    
    # Parallel (migliore performance per ogni numero di boids)
    if not parallel_data.empty:
        best_parallel = parallel_data.groupby('Boids')['Mean_Time_ms'].min().reset_index()
        plt.plot(best_parallel['Boids'], best_parallel['Mean_Time_ms'], 'g^-', label='Parallel (best)', linewidth=2, markersize=8)
    
    plt.xlabel('Numero di Boids')
    plt.ylabel('Tempo Medio (ms)')
    plt.title('Tempi di Esecuzione: Parallel vs SoA')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Speedup Parallel vs SoA
    plt.subplot(1, 2, 2)
    
    if not soa_data.empty and not parallel_data.empty:
        speedup_values = []
        boids_values = []
        for boids in soa_data['Boids'].unique():
            soa_time = soa_data[soa_data['Boids'] == boids]['Mean_Time_ms'].iloc[0]
            parallel_best_time = parallel_data[parallel_data['Boids'] == boids]['Mean_Time_ms'].min()
            if pd.notna(parallel_best_time):
                # Speedup = quanto è più veloce parallel rispetto a SoA
                # Se parallel < SoA, speedup > 1 (più veloce)
                # Se parallel > SoA, speedup < 1 (più lento)
                speedup_values.append(soa_time / parallel_best_time)
                boids_values.append(boids)
        plt.plot(boids_values, speedup_values, 'go-', linewidth=2, markersize=8, label='Parallel vs SoA')
    
    plt.axhline(y=1, color='r', linestyle='--', alpha=0.7, label='No speedup')
    plt.xlabel('Numero di Boids')
    plt.ylabel('Speedup (>1 = più veloce)')
    plt.title('Speedup Parallel vs SoA')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('parallel_vs_soa.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_parallel_scaling(df):
    """Analisi scaling parallelo completa"""
    parallel_data = df[df['Implementation'] == 'Parallel']
    soa_data = df[(df['Implementation'] == 'Parallel') & (df['Threads'] == 1)]
    
    if parallel_data.empty:
        print("Nessun dato parallelo trovato!")
        return
    
    # PRIMA FINESTRA: Strong Scaling e Heatmap
    plt.figure(figsize=(16, 6))
    
    # Grafico 1: Tempi al variare dei thread (Strong Scaling)
    plt.subplot(1, 2, 1)
    
    # Trova il numero di boids più comune per strong scaling
    if not parallel_data.empty:
        most_common_boids = parallel_data['Boids'].mode()[0]
        strong_data = parallel_data[parallel_data['Boids'] == most_common_boids].sort_values('Threads')
        
        if not strong_data.empty:
            plt.plot(strong_data['Threads'], strong_data['Mean_Time_ms'], 'ro-', linewidth=2, markersize=8, label=f'Parallel ({most_common_boids} boids)')
            
            # Aggiunge punto SoA per confronto (Parallel 1 thread)
            soa_time = soa_data[soa_data['Boids'] == most_common_boids]['Mean_Time_ms']
            if not soa_time.empty:
                plt.axhline(y=soa_time.iloc[0], color='blue', linestyle='--', linewidth=2, label=f'SoA/Parallel 1T ({most_common_boids} boids)')
    
    plt.xlabel('Numero di Thread')
    plt.ylabel('Tempo (ms)')
    plt.title('Strong Scaling: Tempi al variare dei Thread')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Grafico 2: Heatmap performance Parallel (Boids x Thread)
    plt.subplot(1, 2, 2)
    
    # Crea una tabella pivot per la heatmap
    pivot_data = parallel_data.pivot_table(values='Mean_Time_ms', 
                                          index='Threads', 
                                          columns='Boids', 
                                          aggfunc='mean')
    
    if not pivot_data.empty and pivot_data.size > 0:
        # Usa una scala di colori semplice senza logaritmo
        im = plt.imshow(pivot_data.values, 
                       cmap='YlOrRd', 
                       aspect='auto')
        
        # Aggiungi i valori nelle celle
        for i in range(len(pivot_data.index)):
            for j in range(len(pivot_data.columns)):
                value = pivot_data.values[i, j]
                if not np.isnan(value):
                    # Usa colore del testo basato sulla luminosità
                    text_color = 'white' if value > np.nanmean(pivot_data.values) else 'black'
                    plt.text(j, i, f'{value:.1f}', ha='center', va='center', color=text_color)
        
        plt.colorbar(im, label='Tempo (ms)')
        plt.xticks(range(len(pivot_data.columns)), pivot_data.columns)
        plt.yticks(range(len(pivot_data.index)), pivot_data.index)
        plt.xlabel('Numero di Boids')
        plt.ylabel('Numero di Thread')
        plt.title('Heatmap Tempi: Boids x Thread')
    else:
        plt.text(0.5, 0.5, 'Dati insufficienti\nper heatmap', ha='center', va='center', transform=plt.gca().transAxes)
        plt.title('Heatmap Tempi: Boids x Thread')
    
    plt.tight_layout()
    plt.savefig('parallel_analysis_scaling.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # SECONDA FINESTRA: Speedup e Tempi di Esecuzione
    plt.figure(figsize=(16, 6))
    
    # Grafico 3: Speedup Parallel vs SoA con linee per diversi thread
    plt.subplot(1, 2, 1)
    
    # Per ogni numero di thread, calcola lo speedup vs SoA
    thread_values = sorted(parallel_data['Threads'].unique())
    colors = ['red', 'green', 'orange', 'purple', 'brown', 'pink']
    
    for i, threads in enumerate(thread_values[:6]):  # Massimo 6 configurazioni
        speedup_data = []
        boids_values = []
        
        thread_data = parallel_data[parallel_data['Threads'] == threads]
        for boids in thread_data['Boids'].unique():
            soa_time_match = soa_data[soa_data['Boids'] == boids]['Mean_Time_ms']
            parallel_time = thread_data[thread_data['Boids'] == boids]['Mean_Time_ms']
            
            if not soa_time_match.empty and not parallel_time.empty:
                # Speedup = soa_time / parallel_time
                # Se parallel < soa, speedup > 1 (parallel più veloce)  
                # Se parallel > soa, speedup < 1 (parallel più lento)
                speedup = soa_time_match.iloc[0] / parallel_time.iloc[0]
                speedup_data.append(speedup)
                boids_values.append(boids)
        
        if speedup_data:
            color = colors[i % len(colors)]
            plt.plot(boids_values, speedup_data, 'o-', color=color, linewidth=2, 
                    markersize=6, label=f'{threads} thread{"s" if threads > 1 else ""}')
    
    plt.axhline(y=1, color='black', linestyle='--', alpha=0.7, label='No speedup')
    plt.xlabel('Numero di Boids')
    plt.ylabel('Speedup vs SoA (>1 = più veloce)')
    plt.title('Speedup Parallel vs SoA per Thread')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Grafico 4: Tempi di esecuzione al variare dei boids per ogni thread
    plt.subplot(1, 2, 2)
    
    # Per ogni numero di thread, mostra i tempi al variare dei boids
    thread_values = sorted(parallel_data['Threads'].unique())
    colors = ['red', 'green', 'orange', 'purple', 'brown', 'pink']
    
    for i, threads in enumerate(thread_values[:6]):  # Massimo 6 configurazioni
        time_data = []
        boids_values = []
        
        thread_data = parallel_data[parallel_data['Threads'] == threads]
        for boids in sorted(thread_data['Boids'].unique()):
            parallel_time = thread_data[thread_data['Boids'] == boids]['Mean_Time_ms']
            
            if not parallel_time.empty:
                time_data.append(parallel_time.iloc[0])
                boids_values.append(boids)
        
        if time_data:
            color = colors[i % len(colors)]
            plt.plot(boids_values, time_data, 'o-', color=color, linewidth=2, 
                    markersize=6, label=f'{threads} thread{"s" if threads > 1 else ""}')
    
    plt.xlabel('Numero di Boids')
    plt.ylabel('Tempo di Esecuzione (ms)')
    plt.title('Tempi di Esecuzione per Thread')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.yscale('log')  # Scala logaritmica per vedere meglio le differenze
    
    plt.tight_layout()
    plt.savefig('parallel_analysis_performance.png', dpi=300, bbox_inches='tight')
    plt.show()

def main(filename="benchmark_results.csv"):
    """Funzione principale"""
    # Carica dati
    df = load_data(filename)
    
    # Mostra informazioni generali
    print(f"\nImplementazioni trovate: {df['Implementation'].unique()}")
    print(f"Range boids testati: {df['Boids'].min()} - {df['Boids'].max()}")
    print(f"Thread testati: {sorted(df['Threads'].unique())}")
    
    # Genera i grafici
    print("\nGenerazione grafici...")
    
    # Grafico 1: AoS vs SoA
    plot_aos_vs_soa(df)
    
    # Grafico 2: Parallel vs SoA
    plot_parallel_vs_soa(df)
    
    # Grafico 3: Analisi parallela dettagliata
    plot_parallel_scaling(df)
    
    # Stampa riassunto semplice
    print("\n" + "="*60)
    print("RIASSUNTO RISULTATI")
    print("="*60)
    
    # Speedup AoS vs SoA (Parallel 1 thread)
    aos_data = df[(df['Implementation'] == 'AoS') & (df['Threads'] == 1)]
    soa_data = df[(df['Implementation'] == 'Parallel') & (df['Threads'] == 1)]
    
    if not aos_data.empty and not soa_data.empty:
        merged = pd.merge(aos_data[['Boids', 'Mean_Time_ms']], 
                         soa_data[['Boids', 'Mean_Time_ms']], 
                         on='Boids', suffixes=('_aos', '_soa'))
        avg_speedup = (merged['Mean_Time_ms_aos'] / merged['Mean_Time_ms_soa']).mean()
        max_speedup = (merged['Mean_Time_ms_aos'] / merged['Mean_Time_ms_soa']).max()
        print(f"1. SoA (Parallel 1T) vs AoS:")
        print(f"   Speedup medio: {avg_speedup:.2f}x")
        print(f"   Speedup massimo: {max_speedup:.2f}x")
    
    # Speedup Parallel vs SoA (migliore performance)
    parallel_data = df[df['Implementation'] == 'Parallel']
    if not parallel_data.empty and not soa_data.empty:
        speedups = []
        for boids in soa_data['Boids'].unique():
            soa_time = soa_data[soa_data['Boids'] == boids]['Mean_Time_ms'].iloc[0]
            best_parallel = parallel_data[parallel_data['Boids'] == boids]['Mean_Time_ms'].min()
            if pd.notna(best_parallel):
                # Speedup = soa_time / parallel_time: >1 significa parallel più veloce, <1 più lento
                speedups.append(soa_time / best_parallel)
        if speedups:
            print(f"\n2. Parallel (migliore) vs SoA (Parallel 1T):")
            print(f"   Speedup medio: {np.mean(speedups):.2f}x")
            print(f"   Speedup massimo: {np.max(speedups):.2f}x")
    
    # Statistiche per thread
    if not parallel_data.empty:
        print(f"\n3. Statistiche Parallele:")
        thread_counts = len(parallel_data['Threads'].unique())
        boids_counts = len(parallel_data['Boids'].unique())
        print(f"   Thread testati: {sorted(parallel_data['Threads'].unique())}")
        print(f"   Configurazioni boids: {sorted(parallel_data['Boids'].unique())}")
        print(f"   Totale test paralleli: {len(parallel_data)} ({thread_counts} thread x {boids_counts} boids)")
        
        # Strong scaling efficiency per il numero di boids più comune
        most_common_boids = parallel_data['Boids'].mode()[0]
        strong_data = parallel_data[parallel_data['Boids'] == most_common_boids].sort_values('Threads')
        if len(strong_data) > 1:
            baseline = strong_data.iloc[0]['Mean_Time_ms']
            max_threads = strong_data.iloc[-1]['Threads']
            max_speedup = baseline / strong_data.iloc[-1]['Mean_Time_ms']
            efficiency = (max_speedup / max_threads) * 100
            print(f"   Strong scaling (con {most_common_boids} boids):")
            print(f"     Speedup: {max_speedup:.2f}x ({max_threads} thread)")
            print(f"     Efficienza: {efficiency:.1f}%")
    
    print("\nGrafici salvati:")
    print("- aos_vs_soa.png")
    print("- parallel_vs_soa.png")
    print("- parallel_analysis_scaling.png")
    print("- parallel_analysis_performance.png")
    print("="*60)

if __name__ == "__main__":
    import sys
    
    # Default filename
    filename = "benchmark_results.csv"
    
    # Check for command line argument
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    
    print(f"Caricamento e analisi dei risultati del benchmark da {filename}...")
    main(filename)
