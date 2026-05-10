#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"

/* 関数プロトタイプ宣言 */
void calc_frequency(int n,
                    double *sum,
                    double *s_obs,
                    double *p_value);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * @brief Frequency Test (Monobit Test) を実行する
 * 
 * @details
 * NIST SP800-22 の Frequency Test を実行する。
 *
 * 目的:
 *   ビット列全体において、
 *   0 と 1 の出現頻度がほぼ同数であるかを検定する。
 *
 * 概要:
 *   各ビットを以下へ変換する。
 *
 *      0 -> -1
 *      1 -> +1
 *
 *   全体和 S_n を求め、
 *   偏りの大きさから p-value を算出する。
 *
 * 仮説:
 *   H0 : 0 と 1 は均等に出現する
 *   H1 : 0 と 1 に偏りがある
 * 判定:
 *   p_value >= ALPHA → SUCCESS
 *   p_value <  ALPHA → FAILURE
 * 
 * @param[in] n : 検定対象ビット数
 * @note
 * グローバル:
 *   epsilon : 検定対象ビット列
 *   stats   : 詳細ログ出力先
 *   results : p-value 出力先
 *
 */
void
Frequency(int n)
{

	double	s_obs;      // 観測統計量
	double	p_value;    // 検定結果 p-value
	double	sum;        // S_n = 全ビット変換後の総和
	
	// S_n, s_obs, p_value を計算する関数を呼び出す
	calc_frequency(n, &sum, &s_obs, &p_value);

	/*
	 * 詳細ログ出力
	 */
	fprintf(stats[TEST_FREQUENCY], "\t\t\t      FREQUENCY TEST\n");
	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");
	/* 
	 * S_n の値
	 */
	fprintf(stats[TEST_FREQUENCY], "\t\t(a) The nth partial sum = %d\n", (int)sum);
	/*
	 * 平均偏り量
	 *
	 * S_n / n:
	 *   0 に近いほど理想的
	 */
	fprintf(stats[TEST_FREQUENCY], "\t\t(b) S_n/n               = %f\n", sum/(double)n);

	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");

	/*
	 * SUCCESS / FAILURE 判定
	 *
	 * ALPHA:
	 *   有意水準
	 *
	 * p_value < ALPHA:
	 *   ランダム性棄却
	 */
	fprintf(stats[TEST_FREQUENCY], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); 
	fflush(stats[TEST_FREQUENCY]);
	
	/*
	 * p-value のみ結果ファイルへ保存
	 */
	fprintf(results[TEST_FREQUENCY], "%f\n", p_value); 
	fflush(results[TEST_FREQUENCY]);
}


/*
 * Frequency Test (Monobit Test) のための計算関数
*/
void
calc_frequency(int n, double *sum, double *s_obs, double *p_value)
{
	int		i;
	double	f;          // 正規化統計量

	/*
	 * sqrt(2)
	 * erfc() 計算用定数
	 */
	double	sqrt2 = 1.41421356237309504880;
	
	/*
	 * ビット列全体の和 S_n を計算
	 *
	 * 変換:
	 *   epsilon[i] = 0 -> -1
	 *   epsilon[i] = 1 -> +1
	 *
	 * 式:
	 *   2*bit - 1
	 *
	 * 例:
	 *   bit=0 -> -1
	 *   bit=1 -> +1
	 */
	*sum = 0.0;
	for ( i=0; i<n; i++ )
		*sum += 2*(int)epsilon[i]-1;
	
	/*
	 * 観測統計量:
	 *
	 *            |S_n|
	 *   s_obs = --------
	 *            sqrt(n)
	 *
	 * 偏りの大きさを表す. 0 に近いほど理想的なビット列であることを示す。
	 * S_n はビット列全体の偏りを表すため、S_n が大きいほど偏りが大きいことを示す。
	 * s_obs は S_n を n で正規化したもので、ビット列の長さに依存しない偏りの大きさを表す。
	 * つまり、nが大きいほど S_n も大きくなりやすいため、s_obs を用いて偏りの大きさを評価する。
	 * よって
	 * s_obs が小さいほど、0 と 1 の出現頻度が均等であることを示す。
	 * s_obs が大きいほど、0 と 1 の出現頻度に偏りがあることを示す。
	 */
	*s_obs = fabs(*sum)/sqrt(n);
	
	/*
	 * erfc() 用に正規化
	 *
	 *        s_obs
	 *   f = --------
	 *        sqrt(2)
	 */
	f = *s_obs/sqrt2;
	
	/*
	 * p-value 計算
	 *
	 * erfc():
	 *   相補誤差関数
	 *
	 * 「ランダム列である」という仮説の下で、現在の偏り以上が偶然に発生する確率を表す。
	 * 	p-value が小さいほど 0/1 の偏りが大きい
	 */
	*p_value = erfc(f);

}
