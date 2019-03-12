#ifndef __ARGS_H__
#define __ARGS_H__

#include <limits.h>
#include <stdio.h>

#include "escape.h"

// the program options structure
typedef struct {
	char fractal[13];
	int random;
	double left, right, top, bottom;
	int pxwidth, pxheight;
	char output[PATH_MAX];
	int osfact;
	int maxval;
} fe_opts_t;

int parse_args(int argc, char * const argv[], fe_opts_t *opts);
void print_usage(FILE *fh, int failarg);

int opts2fract(fractalparams_t *fractal, const fe_opts_t *opts);

#endif
