/* got rid of unused 'k' */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../../include/externs.h"
#include "../../include/cephes.h"  

/* 関数プロトタイプ宣言 */
int calc_longest_run_of_ones(int n, double *pval, double *chi2, unsigned int *nu, int *M, int *K);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                      L O N G E S T  R U N S  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * @brief Longest Run of Ones Test
 *
 * NIST SP800-22 の Longest Run of Ones Test を実行する。
 *
 * @details
 * ビット列を固定長ブロックへ分割し、
 * 各ブロック内に存在する「連続する 1 の最大長」を測定する。
 *
 * ランダムなビット列であれば、
 * 各ブロック内の最長連続 1 長の分布は
 * 理論的に期待される分布へ従う。
 *
 * 本検定では、
 * ブロックごとの最長連続 1 長をカテゴリ分類し、
 * 理論分布との一致度をカイ二乗検定によって評価する。
 *
 * 検定の流れ:
 * - ビット列を長さ M のブロックへ分割
 * - 各ブロックで最長の連続 1 長 v_n_obs を計算
 * - v_n_obs をカテゴリへ分類
 * - 理論確率 pi[] と観測度数 nu[] を比較
 * - カイ二乗統計量 chi2 を計算
 * - 不完全ガンマ関数の補関数から p-value を算出
 *
 * 入力ビット長 n に応じて、
 * NIST 推奨のブロック長 M とカテゴリ数 K を変更する。
 *
 * 使用するパラメータ:
 * - n < 6272:
 *   - M = 8
 *   - K = 3
 * - 6272 <= n < 750000:
 *   - M = 128
 *   - K = 5
 * - n >= 750000:
 *   - M = 10000
 *   - K = 6
 *
 * @param[in] n
 * ビット列長。
 *
 * @note
 * グローバル変数 epsilon を入力ビット列として使用する。
 *
 * @note
 * グローバル変数 stats[] および results[] に
 * 検定結果を書き込む。
 *
 * @note
 * n < 128 の場合は検定を実施できない。
 *
 * @warning
 * p-value が [0,1] 範囲外の場合は
 * WARNING を出力する。
 *
 * @see cephes_igamc
 * 不完全ガンマ関数の補関数を計算し、
 * カイ二乗分布の上側確率 (p-value) を求める。
 */
void
LongestRunOfOnes(int n)
{

	double			pval; 		/* 検定結果 p-value */
	double			chi2;		/* カイ二乗統計量 */
	unsigned int	nu[7] = { 0, 0, 0, 0, 0, 0, 0 }; 	/* 各カテゴリの観測度数 */
	int				M;			/* ブロック長 */
	int				K;			/* カテゴリ数 */

	/*
	 * chi2 と pval を計算する関数を呼び出す
	 */
	if (calc_longest_run_of_ones(n, &pval, &chi2, nu, &M, &K) == -1) {
		return;
	}

	/*
	 * 結果を出力
	 */
	fprintf(stats[TEST_LONGEST_RUN], "\t\t\t  LONGEST RUNS OF ONES TEST\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\t(a) N (# of substrings)  = %d\n", n/M);
	fprintf(stats[TEST_LONGEST_RUN], "\t\t(b) M (Substring Length) = %d\n", M);
	fprintf(stats[TEST_LONGEST_RUN], "\t\t(c) Chi^2                = %f\n", chi2);
	fprintf(stats[TEST_LONGEST_RUN], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\t      F R E Q U E N C Y\n");
	fprintf(stats[TEST_LONGEST_RUN], "\t\t---------------------------------------------\n");

	/*
	 * カテゴリ別頻度出力
	 */
	if ( K == 3 ) {
		fprintf(stats[TEST_LONGEST_RUN], "\t\t  <=1     2     3    >=4   P-value  Assignment");
		fprintf(stats[TEST_LONGEST_RUN], "\n\t\t %3d %3d %3d  %3d ", nu[0], nu[1], nu[2], nu[3]);
	}
	else if ( K == 5 ) {
		fprintf(stats[TEST_LONGEST_RUN], "\t\t<=4  5  6  7  8  >=9 P-value  Assignment");
		fprintf(stats[TEST_LONGEST_RUN], "\n\t\t %3d %3d %3d %3d %3d  %3d ", nu[0], nu[1], nu[2],
				nu[3], nu[4], nu[5]);
	}
	else {
		fprintf(stats[TEST_LONGEST_RUN],"\t\t<=10  11  12  13  14  15 >=16 P-value  Assignment");
		fprintf(stats[TEST_LONGEST_RUN],"\n\t\t %3d %3d %3d %3d %3d %3d  %3d ", nu[0], nu[1], nu[2],
				nu[3], nu[4], nu[5], nu[6]);
	}

	/*
	 * p-value 範囲チェック
	 */
	if ( isNegative(pval) || isGreaterThanOne(pval) )
		fprintf(stats[TEST_LONGEST_RUN], "WARNING:  P_VALUE IS OUT OF RANGE.\n");
	fprintf(stats[TEST_LONGEST_RUN], "%s\t\tp_value = %f\n\n", pval < ALPHA ? "FAILURE" : "SUCCESS", pval); 
	fflush(stats[TEST_LONGEST_RUN]);

	/*
	 * 合否出力
	 */
	fprintf(results[TEST_LONGEST_RUN], "%f\n", pval); 
	fflush(results[TEST_LONGEST_RUN]);
}

int calc_longest_run_of_ones(int n, double *pval, double *chi2, unsigned int nu[], int *M, int *K)
{
	double			pi[7];  	/* 各カテゴリの理論確率 */
	int				V[7];		/* カテゴリの閾値 */
	int				N;	 		/* ブロック数 */
	int				v_n_obs; 	/* 各ブロックの最長連続 1 長 */
	int				i, j;		/* ループカウンタ */
	int				run; 		/* 連続する 1 の長さ */

	/*
	 * n が小さすぎる場合は検定不能
	 */
	if ( n < 128 ) {
		fprintf(stats[TEST_LONGEST_RUN], "\t\t\t  LONGEST RUNS OF ONES TEST\n");
		fprintf(stats[TEST_LONGEST_RUN], "\t\t---------------------------------------------\n");
		fprintf(stats[TEST_LONGEST_RUN], "\t\t   n=%d is too short\n", n);
		return -1;
	}

	/*
	 * 入力長に応じて
	 * NIST 推奨のパラメータを設定する
	 *
	 * K:
	 *   カテゴリ数 - 1
	 *
	 * M:
	 *   ブロック長
	 *
	 * V:
	 *   最長連続長カテゴリ境界
	 *
	 * pi:
	 *   理論確率分布
	 */
	if ( n < 6272 ) {
		*K = 3;
		*M = 8;
		V[0] = 1; V[1] = 2; V[2] = 3; V[3] = 4;
		pi[0] = 0.21484375;
		pi[1] = 0.3671875;
		pi[2] = 0.23046875;
		pi[3] = 0.1875;
	}
	else if ( n < 750000 ) {
		*K = 5;
		*M = 128;
		V[0] = 4; V[1] = 5; V[2] = 6; V[3] = 7; V[4] = 8; V[5] = 9;
		pi[0] = 0.1174035788;
		pi[1] = 0.242955959;
		pi[2] = 0.249363483;
		pi[3] = 0.17517706;
		pi[4] = 0.102701071;
		pi[5] = 0.112398847;
	}
	else {
		*K = 6;
		*M = 10000;
			V[0] = 10; V[1] = 11; V[2] = 12; V[3] = 13; V[4] = 14; V[5] = 15; V[6] = 16;
		pi[0] = 0.0882;
		pi[1] = 0.2092;
		pi[2] = 0.2483;
		pi[3] = 0.1933;
		pi[4] = 0.1208;
		pi[5] = 0.0675;
		pi[6] = 0.0727;
	}

	/*
	 * ブロック数
	 */
	N = n/(*M);

	/*
	 * 各ブロックについて
	 * 最長連続 1 長を測定する
	 */
	for ( i=0; i<N; i++ ) {
	
		/*
		 * ブロック内の最長連続 1 長
		 */
		v_n_obs = 0;

		/*
		 * 現在連続している 1 の長さ
		 */
		run = 0;

		/*
		 * ブロック内のビットを走査し、
		 * 連続する 1 の長さを測定する
		 */
		for ( j=0; j<*M; j++ ) {

			/*
			 * 1 が続く場合
			 */
			if ( epsilon[i**M+j] == 1 ) {
				/*
				 * 連続 1 長をインクリメント
				 */
				run++;

				/*
				 * 最大値更新
				 */
				if ( run > v_n_obs )
					// 最大連続 1 長を更新
					v_n_obs = run;
			}
			else
				/**
				 * 1 が途切れた場合、run をリセット
				 */
				run = 0;
		}

		/*
		 * 最長連続長をカテゴリへ分類
		 */
		
		/* 最小カテゴリ */
		if ( v_n_obs < V[0] )
			nu[0]++;
		
		/* 中間カテゴリ */
		for ( j=0; j<=*K; j++ ) {
			if ( v_n_obs == V[j] )
				nu[j]++;
		}
		/* 最大カテゴリ */
		if ( v_n_obs > V[*K] )
			nu[*K]++;
	}

	/*
	 * カイ二乗統計量:
	 *
	 *               (観測度数 - 期待度数)^2
	 * chi2 = Σ --------------------------------
	 *                    期待度数
	 *
	 * 観測度数:
	 *   nu[i]
	 *
	 * 期待度数:
	 *   N * pi[i]
	 */
	*chi2 = 0.0;
	for ( i=0; i<=*K; i++ )
		*chi2 += ((nu[i] - N * pi[i]) * (nu[i] - N * pi[i])) / (N * pi[i]);

	/*
	 * p-value を計算
	 *
	 * カイ二乗分布の上側確率を
	 * 不完全ガンマ関数の補関数で求める
	 */
	*pval = cephes_igamc((double)(*K/2.0), *chi2 / 2.0);

	return 0;
}
