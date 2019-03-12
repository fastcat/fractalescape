// a scorer that uses average/total 'brightness' to estimate interestingness

void *init_brightscore();

int brightscore_pixelcb(int, int, double, int, void *);

// returns the score for the fractal
double finish_brightscore(void *);
