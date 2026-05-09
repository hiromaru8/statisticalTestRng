/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
U T I L I T I E S
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../include/externs.h"
#include "../include/utilities.h"
#include "../include/generators.h"
#include "../include/stat_fncs.h"

int
displayGeneratorOptions()
{
	int		option = 0;

	printf("           G E N E R A T O R    S E L E C T I O N \n");
	printf("           ______________________________________\n\n");
	printf("    [0] Input File                 [1] Linear Congruential\n");
	printf("    [2] Quadratic Congruential I   [3] Quadratic Congruential II\n");
	printf("    [4] Cubic Congruential         [5] XOR\n");
	printf("    [6] Modular Exponentiation     [7] Blum-Blum-Shub\n");
	printf("    [8] Micali-Schnorr             [9] G Using SHA-1\n\n");
	printf("   Enter Choice: ");
	scanf("%d", &option);
	printf("\n\n");

	return option;
}


int
generatorOptions(char** streamFile)
{
	char	file[200];
	int		option = NUMOFGENERATORS+1;
	FILE	*fp;
	
	while ( (option < 0) || (option > NUMOFGENERATORS) ) {
		option = displayGeneratorOptions();
		switch( option ) {
			case 0:
				printf("\t\tUser Prescribed Input File: ");
				scanf("%s", file);
				*streamFile = (char*)calloc(200, sizeof(char));
				sprintf(*streamFile, "%s", file);
				printf("\n");
				if ( (fp = fopen(*streamFile, "r")) == NULL ) {
					printf("File Error:  file %s could not be opened.\n",  *streamFile);
					exit(-1);
				}
				else
					fclose(fp);
				break;
			case 1:
				*streamFile = "Linear-Congruential";
				break;
			case 2:
				*streamFile = "Quadratic-Congruential-1";
				break;
			case 3:
				*streamFile = "Quadratic-Congruential-2";
				break;
			case 4:
				*streamFile = "Cubic-Congruential";
				break;
			case 5:
				*streamFile = "XOR";
				break;
			case 6:
				*streamFile = "Modular-Exponentiation";
				break;
			case 7:
				*streamFile = "Blum-Blum-Shub";
				break;
			case 8:
				*streamFile = "Micali-Schnorr";
				break;
			case 9:
				*streamFile = "G using SHA-1";
				break;
				
			/* INTRODUCE NEW PRNG NAMES HERE */
			/*
			case 10:  *streamFile = "myNewPRNG";
				break;
			*/
			default:
				printf("Error:  Out of range - Try again!\n");
				break;
		}
	}
	return option;
}

/*

*/
void
chooseTests()
{
	int		i;
	
	printf("                S T A T I S T I C A L   T E S T S\n");
	printf("                _________________________________\n\n");
	printf("    [01] Frequency                       [02] Block Frequency\n");
	printf("    [03] Cumulative Sums                 [04] Runs\n");
	printf("    [05] Longest Run of Ones             [06] Rank\n");
	printf("    [07] Discrete Fourier Transform      [08] Nonperiodic Template Matchings\n");
	printf("    [09] Overlapping Template Matchings  [10] Universal Statistical\n");
	printf("    [11] Approximate Entropy             [12] Random Excursions\n");
	printf("    [13] Random Excursions Variant       [14] Serial\n");
	printf("    [15] Linear Complexity\n\n");
	printf("         INSTRUCTIONS\n");
	printf("            Enter 0 if you DO NOT want to apply all of the\n");
	printf("            statistical tests to each sequence and 1 if you DO.\n\n");
	printf("   Enter Choice: ");
	scanf("%d", &testVector[0]);
	printf("\n");
	if ( testVector[0] == 1 )
		for( i=1; i<=NUMOFTESTS; i++ )
			testVector[i] = 1;
	else {
		printf("         INSTRUCTIONS\n");
		printf("            Enter a 0 or 1 to indicate whether or not the numbered statistical\n");
		printf("            test should be applied to each sequence.\n\n");
		printf("      123456789111111\n");
		printf("               012345\n");
		printf("      ");
		for ( i=1; i<=NUMOFTESTS; i++ ) 
			scanf("%1d", &testVector[i]);
		printf("\n\n");
	}
}


void
fixParameters()
{
	int		counter, testid;
	
	//  Check to see if any parameterized tests are selected
	if ( (testVector[TEST_BLOCK_FREQUENCY] != 1) && (testVector[TEST_NONPERIODIC] != 1) && 
		 (testVector[TEST_OVERLAPPING] != 1) && (testVector[TEST_APEN] != 1) &&
		 (testVector[TEST_SERIAL] != 1) && (testVector[TEST_LINEARCOMPLEXITY] != 1) )
			return;
		
	do {
		counter = 1;
		printf("        P a r a m e t e r   A d j u s t m e n t s\n");
		printf("        -----------------------------------------\n");
		if ( testVector[TEST_BLOCK_FREQUENCY] == 1 )
			printf("    [%d] Block Frequency Test - block length(M):         %d\n", counter++, tp.blockFrequencyBlockLength);
		if ( testVector[TEST_NONPERIODIC] == 1 )
			printf("    [%d] NonOverlapping Template Test - block length(m): %d\n", counter++, tp.nonOverlappingTemplateBlockLength);
		if ( testVector[TEST_OVERLAPPING] == 1 )
			printf("    [%d] Overlapping Template Test - block length(m):    %d\n", counter++, tp.overlappingTemplateBlockLength);
		if ( testVector[TEST_APEN] == 1 )
			printf("    [%d] Approximate Entropy Test - block length(m):     %d\n", counter++, tp.approximateEntropyBlockLength);
		if ( testVector[TEST_SERIAL] == 1 )
			printf("    [%d] Serial Test - block length(m):                  %d\n", counter++, tp.serialBlockLength);
		if ( testVector[TEST_LINEARCOMPLEXITY] == 1 )
			printf("    [%d] Linear Complexity Test - block length(M):       %d\n", counter++, tp.linearComplexitySequenceLength);
		printf("\n");
		printf("   Select Test (0 to continue): ");
		scanf("%1d", &testid);
		printf("\n");
		
		counter = 0;
		if ( testVector[TEST_BLOCK_FREQUENCY] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Block Frequency Test block length: ");
				scanf("%d", &tp.blockFrequencyBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_NONPERIODIC] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter NonOverlapping Template Test block Length: ");
				scanf("%d", &tp.nonOverlappingTemplateBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_OVERLAPPING] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Overlapping Template Test block Length: ");
				scanf("%d", &tp.overlappingTemplateBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_APEN] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Approximate Entropy Test block Length: ");
				scanf("%d", &tp.approximateEntropyBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_SERIAL] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Serial Test block Length: ");
				scanf("%d", &tp.serialBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_LINEARCOMPLEXITY] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Linear Complexity Test block Length: ");
				scanf("%d", &tp.linearComplexitySequenceLength);
				printf("\n");
				continue;
			}
		}
	} while ( testid != 0 );
}


void
fileBasedBitStreams(char *streamFile)
{
	FILE	*fp;
	int		mode;
	
	printf("   Input File Format:\n");
	printf("    [0] ASCII - A sequence of ASCII 0's and 1's\n");
	printf("    [1] Binary - Each byte in data file contains 8 bits of data\n\n");
	printf("   Select input mode:  ");
	scanf("%1d", &mode);
	printf("\n");
	if ( mode == 0 ) {
		if ( (fp = fopen(streamFile, "r")) == NULL ) {
			printf("ERROR IN FUNCTION fileBasedBitStreams:  file %s could not be opened.\n",  streamFile);
			exit(-1);
		}
		readBinaryDigitsInASCIIFormat(fp, streamFile);
		fclose(fp);
	}
	else if ( mode == 1 ) {
		if ( (fp = fopen(streamFile, "rb")) == NULL ) {
			printf("ERROR IN FUNCTION fileBasedBitStreams:  file %s could not be opened.\n", streamFile);
			exit(-1);
		}
		readHexDigitsInBinaryFormat(fp);
		fclose(fp);
	}
}


void
readBinaryDigitsInASCIIFormat(FILE *fp, char *streamFile)
{
	int		i, j, num_0s, num_1s, bitsRead, bit;
	
	if ( (epsilon = (BitSequence *) calloc(tp.n, sizeof(BitSequence))) == NULL ) {
		printf("BITSTREAM DEFINITION:  Insufficient memory available.\n");
		printf("Statistical Testing Aborted!\n");
		return;
	}
	printf("     Statistical Testing In Progress.........\n\n");   
	for ( i=0; i<tp.numOfBitStreams; i++ ) {
		num_0s = 0;
		num_1s = 0;
		bitsRead = 0;
		for ( j=0; j<tp.n; j++ ) {
			if ( fscanf(fp, "%1d", &bit) == EOF ) {
				printf("ERROR:  Insufficient data in file %s.  %d bits were read.\n", streamFile, bitsRead);
				fclose(fp);
				free(epsilon);
				return;
			}
			else {
				bitsRead++;
				if ( bit == 0 ) 
					num_0s++;
				else 
					num_1s++;
				epsilon[j] = bit;
			}
		}
		fprintf(freqfp, "\t\tBITSREAD = %d 0s = %d 1s = %d\n", bitsRead, num_0s, num_1s);
		nist_test_suite();
	}
	free(epsilon);
}

/*
 * readHexDigitsInBinaryFormat
 *
 * 入力ファイルから 4 バイト単位でデータを読み込み、
 * ビット列へ変換して NIST 乱数検定を実行する。
 *
 * 処理概要:
 *   1. 検定対象ビット列 epsilon を確保
 *   2. 入力ファイルから 32bit(4byte)ずつ読み込み
 *   3. convertToBits() によりバイナリ列（ビット列）へ変換
 *   4. 指定ビット長 tp.n に到達するまで繰り返し
 *   5. 生成されたビット列に対して nist_test_suite() を実行
 *
 * 引数:
 *   fp : 入力データファイルポインタ
 *
 * グローバル変数:
 *   epsilon : 検定対象ビット列格納領域
 *   tp      : 検定パラメータ構造体
 *   freqfp  : ビット統計出力用ファイル
 *
 * 注意:
 *   - 入力ファイルには十分なデータ量が必要
 *   - convertToBits() は epsilon へ直接ビットを書き込む
 *   - tp.numOfBitStreams 回だけ検定を実行する
 */
void
readHexDigitsInBinaryFormat(FILE *fp)
{
	int		i;
	int		done;          // 必要ビット数(tp.n)の読み込み完了フラグ
	int		num_0s;        // 読み込んだ 0 の総数
	int		num_1s;        // 読み込んだ 1 の総数
	int		bitsRead;      // 現在までに読み込んだビット数
	// 4byte = 32bit 読み込み用バッファ
	BYTE	buffer[4];
	
	/*
	 * 1. 検定対象ビット列 epsilon を確保
	 * tp.n ビット分の領域を確保し、ビット列を格納するための配列 epsilon を0で初期化する。
	 * メモリ確保に失敗した場合はエラーメッセージを表示して関数を終了する。
	 * なお、epsilon は convertToBits() 内でビット列を直接書き込むため、ここで初期化しておく必要がある。
	 */
	if ( (epsilon = (BitSequence *) calloc(tp.n,sizeof(BitSequence))) == NULL ) {
		printf("BITSTREAM DEFINITION:  Insufficient memory available.\n");
		return;
	}

	printf("     Statistical Testing In Progress.........\n\n");   

	/*
	 * 指定された数(tp.numOfBitStreams)だけ
	 * ビットストリーム生成および検定を繰り返す
	 */
	for ( i=0; i<tp.numOfBitStreams; i++ ) {
		// 統計情報初期化
		num_0s   = 0;
		num_1s   = 0;
		bitsRead = 0;
		done     = 0;
		/*
		 * tp.n ビット分読み込むまで繰り返す
		 */
		do {
			/*
			 * ファイルから 1回の読み込みで 4 バイト(32ビット)を読み込む。
			 * 読み込みに失敗した場合はエラーメッセージを表示し、確保した epsilon を解放して関数を終了する。
			 */			
			if ( fread(buffer, sizeof(unsigned char), 4, fp) != 4 ) {
				printf("READ ERROR:  Insufficient data in file.\n");
				free(epsilon);
				return;
			}
			/*
			 * convertToBits() を呼び出して、読み込んだ 4 バイト(32ビット)をビット列へ変換する。
			 * convertToBits() は epsilon へ直接ビットを書き込み、必要なビット数(tp.n)に到達したかどうかを返す。
			 * ビット数が足りない場合は、再度ファイルから 4 バイト読み込んで変換を繰り返す。
			 * ビット数が足りている場合は、ループを抜けて統計情報を出力し、検定を実行する。
			 * 引数:
			 *   buffer    : 入力データ
			 *   32        : buffer のビット数
			 *   tp.n      : 必要総ビット数
			 *   &num_0s   : 0 のカウント更新
			 *   &num_1s   : 1 のカウント更新
			 *   &bitsRead : 読み込み済みビット数更新
			 *
			 * 戻り値:
			 *   done = 1 → tp.n ビット到達
			 *   done = 0 → 継続
			 */
			done = convertToBits(buffer, 32, tp.n, &num_0s, &num_1s, &bitsRead);
		} while ( !done );
		/*
		 * 読み込んだビット数と 0 と 1 の統計情報を freqfp に出力する。
		 * 出力後、ファイルバッファをフラッシュしてすぐに書き込む。
		 */
		fprintf(freqfp, "\t\tBITSREAD = %d 0s = %d 1s = %d\n", bitsRead, num_0s, num_1s);
		fflush(freqfp); // 実行中にログをリアルタイム監視するためにフラッシュする

		/*
		 * NIST SP800-22 統計検定を実行
		 * epsilon を入力として各種乱数検定を行う
		 */
		nist_test_suite();
		
	}
	free(epsilon);
}

/*
 * convertToBits
 *
 * バイト列データをビット列へ変換し、
 * グローバル配列 epsilon に格納する。
 *
 * 処理概要:
 *   - 入力データ x を MSB(Most Significant Bit) から順に取り出す
 *   - 1bit ごとに epsilon[] へ格納
 *   - 0/1 の出現数をカウント
 *   - 必要ビット数(bitsNeeded)に達したら終了
 *
 * 引数:
 *   x           : 入力バイト列
 *   xBitLength  : x の有効ビット長
 *   bitsNeeded  : 必要な総ビット数
 *   num_0s      : 全体の 0 カウントを加算（呼び出し元で初期化しておくこと）
 *   num_1s      : 全体の 1 カウントを加算（呼び出し元で初期化しておくこと）
 *   bitsRead    : 現在までに読み込んだビット数を加算（呼び出し元で初期化しておくこと）
 *
 * 戻り値:
 *   1 : bitsNeeded に到達した
 *   0 : まだ必要ビット数に達していない
 *
 * 注意:
 *   - epsilon はグローバル領域
 *   - ビットは MSB → LSB の順で処理される
 *   - xBitLength が 8 の倍数でなくても対応可能
 */
int
convertToBits(BYTE *x, int xBitLength, int bitsNeeded, int *num_0s, int *num_1s, int *bitsRead)
{
	int		i, j;
	int		count;      // 現在処理したビット数
	int		bit;        // 抽出した1bit値

	BYTE	mask;       // ビット抽出用マスク

	int		zeros;      // この関数呼び出し内での0数(デバッグ用)
	int		ones;       // この関数呼び出し内での1数(デバッグ用)

	count = 0;
	//この関数呼び出し内での0と1のカウントを初期化
	zeros = ones = 0;
	/*
	 * xBitLength をバイト単位へ変換してループ
	 *   (xBitLength + 7) / 8
	 *    → 切り上げ除算
	 */
	for ( i=0; i<(xBitLength+7)/8; i++ ) {
		/*
		 * 0x80 = 10000000b
		 * MSB から順番にビット抽出する
		 */
		mask = 0x80;

		// 1byte 内の8bitを順番に処理
		for ( j=0; j<8; j++ ) {
	
			/*
			 * 対象ビット判定
			 *   - x[i] と mask の AND 演算で対象ビットが 1 か 0 かを判定
			 *   - 1 の場合は bit = 1 とし、1のカウントを加算
			 *   - 0 の場合は bit = 0 とし、0のカウントを加算
			 *   - mask を右シフトして次のビットへ移動
			 *   - epsilon[bitsRead] に bit を格納し、bitsRead を加算
			 *   - bitsNeeded に到達したら 1 を返す（必要なビット数に到達した）
			 *   - xBitLength に達したら 0 を返す（バイト列の末尾に到達した。また、必要なビット数に達していない）
			 */
			if ( *(x+i) & mask ) {
				bit = 1;		// 対象ビットが 1 の場合
				(*num_1s)++;    // 全体の1のカウントを加算
				ones++;         // この関数呼び出し内での1のカウントを加算
			}
			else {
				bit = 0;		// 対象ビットが 0 の場合
				(*num_0s)++;	// 全体の0のカウントを加算
				zeros++;        // この関数呼び出し内での0のカウントを加算
			}
			mask >>= 1; 				// 次のビットへ移動
			epsilon[*bitsRead] = bit; 	// ビット列へ格納
			(*bitsRead)++; 				// ビット数を加算
			if ( *bitsRead == bitsNeeded ) // 必要なビット数に到達したら終了
				return 1;
			if ( ++count == xBitLength )	// バイト列の末尾に到達したら終了
				return 0;
		}
	}
	// 実質的にはここには到達しないはずだが、念のため 0 を返す（必要なビット数に達していない）
	return 0;
}


void
openOutputStreams(int option)
{
	int		i, numOfBitStreams, numOfOpenFiles = 0;
	char	freqfn[200], summaryfn[200], statsDir[200], resultsDir[200];
	
	// OPEN FILES FOR LOGGING FREQUENCIES, STATISTICS, AND P-VALUES
	sprintf(freqfn, "experiments/%s/freq.txt", generatorDir[option]);
	if ( (freqfp = fopen(freqfn, "w")) == NULL ) {
		printf("\t\tMAIN:  Could not open freq file: <%s>", freqfn);
		exit(-1);
	}
	// OPEN FILE FOR LOGGING FINAL ANALYSIS REPORT
	sprintf(summaryfn, "experiments/%s/finalAnalysisReport.txt", generatorDir[option]);
	if ( (summary = fopen(summaryfn, "w")) == NULL ) {
		printf("\t\tMAIN:  Could not open stats file: <%s>", summaryfn);
		exit(-1);
	}
	
	for( i=1; i<=NUMOFTESTS; i++ ) {
		if ( testVector[i] == 1 ) {
			sprintf(statsDir, "experiments/%s/%s/stats.txt", generatorDir[option], testNames[i]);
			sprintf(resultsDir, "experiments/%s/%s/results.txt", generatorDir[option], testNames[i]);
			if ( (stats[i] = fopen(statsDir, "w")) == NULL ) {	/* STATISTICS LOG */
				printf("ERROR: LOG FILES COULD NOT BE OPENED.\n");
				printf("       MAX # OF OPENED FILES HAS BEEN REACHED = %d\n", numOfOpenFiles);
				printf("-OR-   THE OUTPUT DIRECTORY DOES NOT EXIST.\n");
				exit(-1);
			}
			else
				numOfOpenFiles++;
			if ( (results[i] = fopen(resultsDir, "w")) == NULL ) {	/* P_VALUES LOG   */
				 printf("ERROR: LOG FILES COULD NOT BE OPENED.\n");
				 printf("       MAX # OF OPENED FILES HAS BEEN REACHED = %d\n", numOfOpenFiles);
				 printf("-OR-   THE OUTPUT DIRECTORY DOES NOT EXIST.\n");
				 exit(-1);
			}
			else
				numOfOpenFiles++;
		}
	}
	printf("   How many bitstreams? ");
	scanf("%d", &numOfBitStreams);
	tp.numOfBitStreams = numOfBitStreams;
	printf("\n");
}


void
invokeTestSuite(int option, char *streamFile)
{
	fprintf(freqfp, "________________________________________________________________________________\n\n");
	fprintf(freqfp, "\t\tFILE = %s\t\tALPHA = %6.4f\n", streamFile, ALPHA);
	fprintf(freqfp, "________________________________________________________________________________\n\n");
	if ( option != 0 )
		printf("     Statistical Testing In Progress.........\n\n");
	switch( option ) {
		case 0:
			fileBasedBitStreams(streamFile);
			break;
		case 1:
			lcg();
			break;
		case 2:
			quadRes1();
			break;
		case 3:
			quadRes2();
			break;
		case 4:
			cubicRes();
			break;
		case 5:
			exclusiveOR();
			break;
		case 6:
			modExp();
			break;
		case 7:
			bbs();
			break;
		case 8:
			micali_schnorr();
			break;
		case 9:
			SHA1();
			break;
			
		/* INTRODUCE NEW PSEUDO RANDOM NUMBER GENERATORS HERE */
			
		default:
			printf("Error in invokeTestSuite!\n");
			break;
	}
	printf("     Statistical Testing Complete!!!!!!!!!!!!\n\n");
}


void
nist_test_suite()
{
	if ( (testVector[0] == 1) || (testVector[TEST_FREQUENCY] == 1) ) 
		Frequency(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_BLOCK_FREQUENCY] == 1) ) 
		BlockFrequency(tp.blockFrequencyBlockLength, tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_CUSUM] == 1) )
		CumulativeSums(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_RUNS] == 1) )
		Runs(tp.n); 
	
	if ( (testVector[0] == 1) || (testVector[TEST_LONGEST_RUN] == 1) )
		LongestRunOfOnes(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_RANK] == 1) )
		Rank(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_FFT] == 1) )
		DiscreteFourierTransform(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_NONPERIODIC] == 1) )
		NonOverlappingTemplateMatchings(tp.nonOverlappingTemplateBlockLength, tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_OVERLAPPING] == 1) )
		OverlappingTemplateMatchings(tp.overlappingTemplateBlockLength, tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_UNIVERSAL] == 1) )
		Universal(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_APEN] == 1) )
		ApproximateEntropy(tp.approximateEntropyBlockLength, tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_RND_EXCURSION] == 1) )
		RandomExcursions(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_RND_EXCURSION_VAR] == 1) )
		RandomExcursionsVariant(tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_SERIAL] == 1) )
		Serial(tp.serialBlockLength,tp.n);
	
	if ( (testVector[0] == 1) || (testVector[TEST_LINEARCOMPLEXITY] == 1) )
		LinearComplexity(tp.linearComplexitySequenceLength, tp.n);
}