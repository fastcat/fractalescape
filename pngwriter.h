#include "escape.h"

// returns NULL on failure/error
void *init_pngwriter(fractalparams_t *fractal, const char *filename, int maxval);

int pngwriter_pixelcb(int, int, double, int, void *);

// finishes out the png writer: closes all filehandles, frees memory, etc.
void finish_pngwriter(void *);
