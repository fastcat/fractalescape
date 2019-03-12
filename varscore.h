// a scorer that uses variation in pixel values

void *init_varscore();

int varscore_pixelcb(int, int, double, int, void *);

// returns the score for the fractal, optionally fills in a max value seen
double finish_varscore(void *, int *maxval);
