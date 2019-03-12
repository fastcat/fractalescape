#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <getopt.h>

#include "args.h"
#include "escape.h"

int parse_args(int argc, char * const argv[], fe_opts_t *opts) {
	struct option longopts[] = {
		{"fractal", 1, 0, 'f'},
		{"random", 2, 0, 'r'},
		{"coords", 1, 0, 'c'},
		{"size", 1, 0, 's'},
		{"osfact", 1, 0, 'O'},
		{"output", 1, 0, 'o'},
		{"maxval", 1, 0, 'm'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	int theopt, n;
	char *endptr;
	char *endptr2;
	
	// initialize default args
	opts->random = -1;
	opts->left = opts->bottom = -1;
	opts->right = opts->top = 1;
	opts->pxwidth = opts->pxheight = 100;
	opts->osfact = 1;
	opts->maxval = 0;
	memset(opts->output, 0, sizeof(opts->output));
	memset(opts->fractal, 0, sizeof(opts->fractal));
	
	while ((theopt = getopt_long(argc, argv, "f:r::c:s:O:o:m:h", longopts, NULL)) != -1) {
		switch(theopt) {
			case 'f':
				if (strlen(optarg) != 12)
					return 'f';
				for (n = 0; n < 12; ++n)
					if (!isalpha(optarg[n]))
						return 'f';
				strcpy(opts->fractal, optarg);
				break;
			case 'r':
				if (optarg) {
					opts->random = strtol(optarg, &endptr, 10);
					if (opts->random < 0 || endptr == optarg)
						return 'r';
				} else {
					opts->random = 0;
				}
				break;
			case 'c':
				opts->left = strtod(optarg, &endptr);
				if (endptr == optarg || *endptr != ',') return 'c';
				endptr2 = endptr;
				opts->right = strtod(endptr + 1, &endptr);
				if (endptr == endptr2 || *endptr != ',') return 'c';
				endptr2 = endptr;
				opts->top = strtod(endptr + 1, &endptr);
				if (endptr == endptr2 || *endptr != ',') return 'c';
				endptr2 = endptr;
				opts->bottom = strtod(endptr + 1, &endptr);
				if (endptr == endptr2 || *endptr != 0) return 'c';
				break;
			case 's':
				opts->pxwidth = strtol(optarg, &endptr, 10);
				if (opts->pxwidth < 1 || endptr == optarg || *endptr != ',') return 'c';
				endptr2 = endptr;
				opts->pxheight = strtol(endptr + 1, &endptr, 10);
				if (opts->pxheight < 1 || endptr == endptr2 || *endptr != 0) return 'c';
				break;
			case 'O':
				opts->osfact = strtol(optarg, &endptr, 10);
				if (opts->osfact < 1 || endptr == optarg)
					return 'O';
				break;
			case 'o':
				if (strlen(optarg) >= PATH_MAX) return 'o';
				strcpy(opts->output, optarg);
				break;
			case 'm':
				opts->maxval = strtol(optarg, &endptr, 10);
				if (opts->maxval < 1 || endptr == optarg)
					return 'm';
				break;
			case 'h':
				return 'h';
				break;
			default:
				return theopt;
		} // switch(theopt)
	} // while getopt
	
	return 0;
}

void print_usage(FILE *fh, int failarg) {
	switch(failarg) {
		case 'f':
			fprintf(fh, "fractal option expects 12 letters\n");
			break;
		case 'r':
			fprintf(fh, "random option expects no arg or an integer >= 0\n");
			break;
		case 'c':
			fprintf(fh, "coords option expects 4 comma-separated decimal numbers\n");
			break;
		case 's':
			fprintf(fh, "size option expects 2 comma-separated integers > 0\n");
			break;
		case 'O':
			fprintf(fh, "osfact option expects an integer >= 1\n");
			break;
		case 'o':
			fprintf(fh, "output option expects an file or directory name\n");
			break;
		case 'm':
			fprintf(fh, "maxval option expects integer >= 1\n");
			break;
		case 'h': default:
			fprintf(fh,
				"fractalescape options:\n"
				"  Long    Short  Description\n"
				"  --------\\---/---------------------------------------------------\n"
				"  fractal   f   12 letters representing fractal coefficients\n"
				"  random    r   Activate random mode, optionally specifing number to generate\n"
				"  coords    c   Left, right, top, bottom of the rectangle to render\n"
				"  size      s   Width, height of the output image\n"
				"  osfact    O   Over-sampling factor: NxN samples for each pixel\n"
				"  output    o   Output file for single fractal, directory for random fractals\n"
				"  maxval    m   Maximum value for output (scaling for pretty-izing)\n"
				"                Only applicable in single mode (random autodetects)\n"
				"  help      h   Print this help\n"
			);
	}
}

int opts2fract(fractalparams_t *fractal, const fe_opts_t *opts) {
	fractal->width = opts->pxwidth;
	fractal->height = opts->pxheight;
	fractal->left = opts->left;
	fractal->right = opts->right;
	fractal->top = opts->top;
	fractal->bottom = opts->bottom;
	fractal->osfact = opts->osfact;
	fractal->maxval = opts->maxval;
	if (opts->fractal[0] != 0)
		return initcoeffs_fromstring(fractal, opts->fractal);
	else
		return 0;
}
