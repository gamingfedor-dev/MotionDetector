#!/usr/bin/env python3
"""Motion data analysis and visualization using applied statistics."""


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, stats
from pathlib import Path
import argparse


def load_data(csv_path: str) -> pd.DataFrame:
    """Load motion data from CSV."""
    df = pd.read_csv(csv_path)
    df['time_sec'] = (df['timestamp_ms'] - df['timestamp_ms'].iloc[0]) / 1000
    return df


def compute_statistics(df: pd.DataFrame) -> dict:
    """Compute comprehensive motion statistics."""
    motion = df['motion_score']
    return {
        'mean': motion.mean(),
        'std': motion.std(),
        'median': motion.median(),
        'max': motion.max(),
        'percentile_95': np.percentile(motion, 95),
        'skewness': stats.skew(motion),
        'kurtosis': stats.kurtosis(motion),
        'motion_events': (motion > motion.mean() + 2 * motion.std()).sum(),
        'total_frames': len(df),
        'duration_sec': df['time_sec'].iloc[-1]
    }


def detect_peaks(df: pd.DataFrame, prominence: float = 0.5) -> np.ndarray:
    """Detect motion peaks using scipy signal processing."""
    motion = df['motion_score'].values
    # Smooth the signal
    window = min(11, len(motion) // 10)
    if window % 2 == 0: window += 1
    smoothed = signal.savgol_filter(motion, window, 3)
    peaks, properties = signal.find_peaks(smoothed, prominence=prominence, distance=10)
    return peaks, smoothed


def compute_fft(df: pd.DataFrame) -> tuple:
    """Compute FFT for frequency analysis of motion patterns."""
    motion = df['motion_score'].values
    n = len(motion)
    dt = df['time_sec'].diff().median()
    freq = np.fft.fftfreq(n, dt)[:n//2]
    fft_vals = np.abs(np.fft.fft(motion))[:n//2]
    return freq, fft_vals


def plot_analysis(df: pd.DataFrame, output_dir: Path):
    """Generate comprehensive visualization plots."""
    output_dir.mkdir(exist_ok=True)
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    # 1. Time series with peaks
    ax1 = axes[0, 0]
    peaks, smoothed = detect_peaks(df)
    ax1.plot(df['time_sec'], df['motion_score'], 'b-', alpha=0.5, label='Raw')
    ax1.plot(df['time_sec'], smoothed, 'r-', linewidth=2, label='Smoothed')
    ax1.scatter(df['time_sec'].iloc[peaks], smoothed[peaks], c='green', s=100, 
                marker='^', label=f'Peaks ({len(peaks)})', zorder=5)
    ax1.set_xlabel('Time (seconds)')
    ax1.set_ylabel('Motion Score (%)')
    ax1.set_title('Motion Detection Over Time')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 2. Histogram with distribution fit
    ax2 = axes[0, 1]
    motion = df['motion_score']
    ax2.hist(motion, bins=50, density=True, alpha=0.7, color='steelblue', edgecolor='white')
    
    # Fit and plot normal distribution
    mu, std = motion.mean(), motion.std()
    x = np.linspace(motion.min(), motion.max(), 100)
    ax2.plot(x, stats.norm.pdf(x, mu, std), 'r-', linewidth=2, label=f'Normal fit (μ={mu:.2f}, σ={std:.2f})')
    ax2.axvline(mu, color='orange', linestyle='--', label=f'Mean: {mu:.2f}%')
    ax2.axvline(mu + 2*std, color='red', linestyle=':', label=f'2σ threshold: {mu + 2*std:.2f}%')
    ax2.set_xlabel('Motion Score (%)')
    ax2.set_ylabel('Density')
    ax2.set_title('Motion Score Distribution')
    ax2.legend()
    
    # 3. FFT Frequency Analysis
    ax3 = axes[1, 0]
    freq, fft_vals = compute_fft(df)
    ax3.semilogy(freq[1:], fft_vals[1:], 'b-')
    ax3.set_xlabel('Frequency (Hz)')
    ax3.set_ylabel('Magnitude (log scale)')
    ax3.set_title('Frequency Analysis (FFT)')
    ax3.grid(True, alpha=0.3)
    
    # 4. Contour count vs motion score
    ax4 = axes[1, 1]
    scatter = ax4.scatter(df['contour_count'], df['motion_score'], 
                          c=df['time_sec'], cmap='viridis', alpha=0.6, s=20)
    plt.colorbar(scatter, ax=ax4, label='Time (sec)')
    ax4.set_xlabel('Number of Moving Objects')
    ax4.set_ylabel('Motion Score (%)')
    ax4.set_title('Objects vs Motion Intensity')
    
    # Fit regression line
    if df['contour_count'].nunique() > 1:
        z = np.polyfit(df['contour_count'], df['motion_score'], 1)
        p = np.poly1d(z)
        x_line = np.linspace(df['contour_count'].min(), df['contour_count'].max(), 100)
        ax4.plot(x_line, p(x_line), 'r--', linewidth=2, label=f'Trend: y={z[0]:.2f}x+{z[1]:.2f}')
        ax4.legend()
    
    plt.tight_layout()
    plt.savefig(output_dir / 'motion_analysis.png', dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Plots saved to {output_dir / 'motion_analysis.png'}")


def generate_report(df: pd.DataFrame, stats: dict, output_path: Path):
    """Generate a text report with statistics."""
    with open(output_path, 'w') as f:
        f.write("=" * 60 + "\n")
        f.write("MOTION DETECTION ANALYSIS REPORT\n")
        f.write("=" * 60 + "\n\n")
        
        f.write("SUMMARY STATISTICS\n")
        f.write("-" * 40 + "\n")
        f.write(f"Total Frames Analyzed: {stats['total_frames']}\n")
        f.write(f"Duration: {stats['duration_sec']:.2f} seconds\n")
        f.write(f"Mean Motion Score: {stats['mean']:.4f}%\n")
        f.write(f"Standard Deviation: {stats['std']:.4f}%\n")
        f.write(f"Median: {stats['median']:.4f}%\n")
        f.write(f"Maximum: {stats['max']:.4f}%\n")
        f.write(f"95th Percentile: {stats['percentile_95']:.4f}%\n")
        f.write(f"Skewness: {stats['skewness']:.4f}\n")
        f.write(f"Kurtosis: {stats['kurtosis']:.4f}\n")
        f.write(f"Significant Motion Events (>2σ): {stats['motion_events']}\n")
        
    print(f"Report saved to {output_path}")


def main():
    parser = argparse.ArgumentParser(description='Analyze motion detection data')
    parser.add_argument('csv_file', help='Input CSV file from motion detector')
    parser.add_argument('-o', '--output', default='analysis', help='Output directory')
    args = parser.parse_args()
    
    output_dir = Path(args.output)
    
    print(f"Loading data from {args.csv_file}...")
    df = load_data(args.csv_file)
    
    print("Computing statistics...")
    stats = compute_statistics(df)
    
    print("Generating visualizations...")
    plot_analysis(df, output_dir)
    
    print("Generating report...")
    generate_report(df, stats, output_dir / 'report.txt')
    
    print("\nAnalysis complete!")


if __name__ == '__main__':
    main()
