#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* 関数プロトタイプ宣言 */
void calc_runs(int n, double *pi, double *V, double *erfc_arg, double *p_value);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                              R U N S  T E S T 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @brief Runs Test を実行する
 *
 * NIST SP800-22 に定義される Runs Test を実行する。
 *
 * Runs Test は、ビット列中に現れる「連長 (run)」の数を調べることで、
 * 0 と 1 の並びがランダムに切り替わっているかを検定する。
 *
 * ここで run とは、同じ値のビットが連続する区間を指す。
 *
 * 例:
 *   00111001
 *
 * は以下の 5 個の run を持つ。
 *
 *   00 | 111 | 00 | 1
 *
 * 本検定では、
 *   - run の数が少なすぎる
 *   - run の数が多すぎる
 *
 * 場合に、非ランダム性が疑われる。
 *
 * また、本検定はビット列中の 0 と 1 の出現比率が
 * おおよそ等しいことを前提としているため、
 * 事前条件として Frequency Test 相当の判定を行う。
 *
 * @param[in] n
 *      検査対象ビット列の長さ
 *
 * @note
 *      以下のグローバル変数を使用する。
 *      - epsilon : 検査対象ビット列
 *      - stats   : 詳細統計情報出力ファイル
 *      - results : p-value 出力ファイル
 *
 * @note
 *      epsilon[] の各要素は 0 または 1 を想定する。
 *
 * @details
 *      処理概要:
 *
 *      1. ビット列中の 1 の割合 pi を計算
 *
 *      2. pi が 0.5 に十分近いか確認
 *
 *         判定条件:
 *         |pi - 0.5| <= 2 / sqrt(n)
 *
 *         条件を満たさない場合、
 *         Runs Test の前提条件を満たさないため
 *         p-value = 0 とする。
 *
 *      3. run の総数 V を計算
 *
 *         隣接ビットが変化した回数 + 1 が
 *         run 数となる。
 *
 *      4. Runs Test の統計量を計算
 *
 *         観測された run 数 V が、
 *         ランダム列で期待される run 数
 *
 *             2 n pi (1-pi)
 *
 *         からどれだけ離れているかを評価する。
 *
 *      5. 相補誤差関数 erfc() を使用して
 *         p-value を算出
 *
 *      6. p-value と有意水準 ALPHA を比較し、
 *         SUCCESS / FAILURE を出力
 *
 * @retval なし
 */
void
Runs(int n)
{
	double	pi;			 /* ビット列中の 1 の割合 */
	double	V;			 /* 観測された run 数 */
	double	erfc_arg;	 /* erfc() の引数 */
	double	p_value;	 /* 検定結果の p-value */


	calc_runs(n, &pi, &V, &erfc_arg, &p_value);

	if(p_value != 0.0) {
		
		fprintf(stats[TEST_RUNS], "\t\t\t\tRUNS TEST\n");
		fprintf(stats[TEST_RUNS], "\t\t------------------------------------------\n");
		fprintf(stats[TEST_RUNS], "\t\tCOMPUTATIONAL INFORMATION:\n");
		fprintf(stats[TEST_RUNS], "\t\t------------------------------------------\n");
		fprintf(stats[TEST_RUNS], "\t\t(a) Pi                        = %f\n", pi);
		fprintf(stats[TEST_RUNS], "\t\t(b) V_n_obs (Total # of runs) = %d\n", (int)V);
		fprintf(stats[TEST_RUNS], "\t\t(c) V_n_obs - 2 n pi (1-pi)\n");
		fprintf(stats[TEST_RUNS], "\t\t    -----------------------   = %f\n", erfc_arg);
		fprintf(stats[TEST_RUNS], "\t\t      2 sqrt(2n) pi (1-pi)\n");
		fprintf(stats[TEST_RUNS], "\t\t------------------------------------------\n");
		if ( isNegative(p_value) || isGreaterThanOne(p_value) )
			fprintf(stats[TEST_RUNS], "WARNING:  P_VALUE IS OUT OF RANGE.\n");

		fprintf(stats[TEST_RUNS], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_RUNS]);
	}

	fprintf(results[TEST_RUNS], "%f\n", p_value); fflush(results[TEST_RUNS]);
}


void calc_runs(int n, double *pi, double *V, double *erfc_arg, double *p_value)
{
	int		S;			 /* ビット列中の 1 の個数 */
	int		k; 		 	 /* ループカウンタ */

	/*----------------------------------------------------------
	 * 1 の個数 S を計算
	 *----------------------------------------------------------*/
	S = 0;
	for ( k=0; k<n; k++ )
		if ( epsilon[k] )
			S++;

	/*----------------------------------------------------------
	 * ビット列中の 1 の割合 pi を計算
	 *----------------------------------------------------------*/
	*pi = (double)S / (double)n;

	/*----------------------------------------------------------
	 * Runs Test の前提条件チェック
	 *
	 * 0 と 1 の比率が大きく偏っている場合、
	 * Runs Test は適用できない。
	 * 判定条件:
	 *   |pi - 0.5| <= 2 / sqrt(n)
	 * 条件を満たさない場合、
	 * p-value = 0 とする。
	 *----------------------------------------------------------*/
	if ( fabs(*pi - 0.5) > (2.0 / sqrt(n)) ) {
		fprintf(stats[TEST_RUNS], "\t\t\t\tRUNS TEST\n");
		fprintf(stats[TEST_RUNS], "\t\t------------------------------------------\n");
		fprintf(stats[TEST_RUNS], "\t\tPI ESTIMATOR CRITERIA NOT MET! PI = %f\n", *pi);
		*p_value = 0.0;
		return;
	}

	/*----------------------------------------------------------
	 * run の総数 V を計算
	 *
	 * 隣接ビットが変化した回数 + 1 が
	 * run 数となる。
	 *----------------------------------------------------------*/
	*V = 1;
	for ( k=1; k<n; k++ )
		if ( epsilon[k] != epsilon[k-1] )
			*V += 1;

	/*----------------------------------------------------------
	 * Runs Test の統計量を計算
	 *
	 * 観測された run 数 V が、
	 * ランダム列で期待される run 数
	 *
	 *     2 n pi (1-pi)
	 *
	 * からどれだけ離れているかを評価する。
	 * 分子：
	 *   |V - 2 n pi (1-pi)|
	 * 分母：
	 *   2 pi (1-pi) sqrt(2n)
	 *----------------------------------------------------------*/
	*erfc_arg = fabs(*V - 2.0 * n * (*pi) * (1-(*pi))) / 
				(2.0 * (*pi) * (1-(*pi)) * sqrt(2*n));

	/*----------------------------------------------------------
	 * 相補誤差関数 erfc() を使用して p-value を算出
	 *----------------------------------------------------------*/
	*p_value = erfc(*erfc_arg);

	return;
}
