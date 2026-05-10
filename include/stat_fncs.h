
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     S T A T I S T I C A L  T E S T  F U N C T I O N  P R O T O T Y P E S 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void	Frequency(int n);
void    calc_frequency(int n, double *sum, double *s_obs, double *p_value);

void	BlockFrequency(int M, int n);
void    calc_block_frequency(int M, int n, double *chi_squared, double *p_value);

void	CumulativeSums(int n);
void	Runs(int n);
void    calc_runs(int n, double *pi, double *V, double *erfc_arg, double *p_value);
    
void	LongestRunOfOnes(int n);
int     calc_longest_run_of_ones(int n, double *pval, double *chi2, unsigned int *nu, int *M, int *K);
void	Rank(int n);
void	DiscreteFourierTransform(int n);
void	NonOverlappingTemplateMatchings(int m, int n);
void	OverlappingTemplateMatchings(int m, int n);
void	Universal(int n);
void	ApproximateEntropy(int m, int n);
void	RandomExcursions(int n);
void	RandomExcursionsVariant(int n);
void	LinearComplexity(int M, int n);
void	Serial(int m, int n);
