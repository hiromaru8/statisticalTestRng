#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* 関数プロトタイプ宣言 */
void calc_block_frequency(int M, int n, double *chi_squared, double *p_value);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                    B L O C K  F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @brief Block Frequency Test を実行する
 * 
 * @details
 * NIST SP800-22 の Block Frequency Test を実装した関数。
 * ビット列を M ビットごとのブロックに分割し、
 * 各ブロック内の 1 の出現割合が 0.5 に近いかを検定する。
 *
 * Frequency Test（Monobit Test）が全体の偏りを確認するのに対し、
 * 本テストは「局所的な偏り」を検出する目的を持つ。
 *
 * @param M 1ブロックあたりのビット長(例: 128)
 * @param n 入力ビット列長(例: 1000000)
 * @note
 * グローバル:
 *   epsilon : 検定対象ビット列
 *   stats   : 詳細ログ出力先
 *   results : p-value 出力先
 */
void
BlockFrequency(int M, int n)
{

	double	p_value;        /* 検定結果の p-value */
	double	chi_squared;    /* カイ二乗統計量 */
	
	calc_block_frequency(M, n, &chi_squared, &p_value);
	
	/* ===== 統計情報をログファイルへ出力 ===== */
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t\tBLOCK FREQUENCY TEST\n");
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(a) Chi^2           = %f\n", chi_squared);
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(b) # of substrings = %d\n", n/M);
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(c) block length    = %d\n", M);
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(d) Note: %d bits were discarded.\n", n % M);
	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");

	/*
	 * p-value と判定結果を出力
	 *
	 * p-value < ALPHA :
	 *     帰無仮説（ランダムである）を棄却 → FAILURE
	 *
	 * p-value >= ALPHA :
	 *     ランダム性を棄却できない → SUCCESS
	 */
	fprintf(stats[TEST_BLOCK_FREQUENCY], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_BLOCK_FREQUENCY]);
	fprintf(results[TEST_BLOCK_FREQUENCY], "%f\n", p_value); fflush(results[TEST_BLOCK_FREQUENCY]);
}

void calc_block_frequency(int M, int n, double *chi_squared, double *p_value)
{
	int		i, j;			/* ループカウンタ */
	int		N;              /* ブロック数 (= n / M) */
	int		blockSum;       /* 各ブロック内の 1 の個数 */

	double	sum;            /* 偏差平方和 */
	double	pi;             /* ブロック内の 1 の割合 */
	double	v;              /* 0.5 からの偏差 */
	
	/* 
	 * 入力ビット列を M ビット単位で分割したときの
	 * 完全なブロック数を求める。
	 * 余りビットは使用しない。
	 */
	N = n/M; 		/* # OF SUBSTRING BLOCKS      */


	/* 偏差平方和の初期化 */
	sum = 0.0;
	
	/*
	 * 各ブロックについて、
	 * 1 の割合が 0.5 からどれだけ離れているかを計算（偏差平方和）し、合計（sum）する。
	 * ブロック内の 1 の割合が 0.5 に近いほど、v は小さくなり、sum も小さくなる。
	 * ブロック内の 1 の割合が 0.5 から大きく乖離しているほど、v は大きくなり、sum も大きくなる。
	 *  偏差平方和へ加算 
	 * 		Σ(pi - 0.5)^2
	 * 		0.5 からの偏差の二乗をブロック数 N で合計する。
	 * 		ブロック内の 1 の割合 pi は、ブロック内の 1 の個数を M で割ったもの。
	 */
	for ( i=0; i<N; i++ ) {

		/* ブロック内の 1 の数を初期化 */
		blockSum = 0;

		/*
		 * epsilon[] は 0/1 のビット列。
		 * 対象ブロック中の 1 の個数を数える。
		 */
		for ( j=0; j<M; j++ )
			blockSum += epsilon[j+i*M];

		/* ブロック内の 1 の割合 */
		pi = (double)blockSum/(double)M;

		/* 理想値 0.5 との差分 */
		v = pi - 0.5;

		/* 偏差平方和へ加算 
		 *   Σ(pi - 0.5)^2
		 *   0.5 からの偏差の二乗をブロック数 N で合計する。
		 */
		sum += v*v;
	}

	/*
	 * Block Frequency Test のカイ二乗統計量
	 *
	 * χ² = 4M Σ(pi - 0.5)^2
	 */
	*chi_squared = 4.0 * M * sum;

	/*
	 * 不完全ガンマ関数の補関数を使用して
	 * p-value を算出する。
	 *
	 * p-value が小さいほど、
	 * 「ブロックごとの 1 の割合が偏っている」
	 * 可能性が高い。
	 * 
	 * N/2 : カイ二乗分布の自由度
	 * chi_squared/2 : 観測されたカイ二乗統計量を 2 で割った値
	 * カイ二乗分布は、ガンマ分布の特殊なケースであるため、ガンマ関数を用いて p-value を計算する。
	 * 自由度 k のカイ二乗分布は、ガンマ分布の形状パラメータ a = k/2、尺度パラメータ θ = 2 の場合に対応する。
	 */
	*p_value = cephes_igamc(N/2.0, *chi_squared/2.0);
}
