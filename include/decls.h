
#include "../include/defs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                   G L O B A L   D A T A  S T R U C T U R E S 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

BitSequence	*epsilon;				// BIT STREAM
TP			tp;						// TEST PARAMETER STRUCTURE
FILE		*stats[NUMOFTESTS+1];	// stats.txt 各検定の詳細ログを出力するためのファイルポインタ配列
FILE		*results[NUMOFTESTS+1];	// FILE OUTPUT STREAM
FILE		*freqfp;				// freq.txt 乱数のビット統計を出力するためのファイルポインタ
FILE		*summary;				// finalAnalysisReport.txt　最終分析レポートを出力するためのファイルポインタ
int			testVector[NUMOFTESTS+1]; // TEST SELECTION VECTOR

char	generatorDir[NUMOFGENERATORS][20] = { "AlgorithmTesting", "LCG", "QCG1", "QCG2","CCG", "XOR",
			"MODEXP", "BBS", "MS", "G-SHA1" };
				
char	testNames[NUMOFTESTS+1][32] = { " ", "Frequency", "BlockFrequency", "CumulativeSums", "Runs", "LongestRun", "Rank",
			"FFT", "NonOverlappingTemplate", "OverlappingTemplate", "Universal", "ApproximateEntropy", "RandomExcursions",
			"RandomExcursionsVariant", "Serial", "LinearComplexity" };