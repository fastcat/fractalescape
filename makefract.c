#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "makefract.h"

int writefractal(const fe_opts_t *opts, const char *coeffstr, int maxval) {
	fractalparams_t fractal;
	char ofname[PATH_MAX];
	void *pngwriter;
	int cfret;

	if (opts2fract(&fractal, opts) != 0) {
		fprintf(stderr, "Couldn't init fractal from options\n");
		return 1;
	}
	if (coeffstr != NULL)
		if (initcoeffs_fromstring(&fractal, coeffstr) != 0) {
			fprintf(stderr, "Couldn't init fractal from coeffstr\n");
			return 1;
		}
	// special case - stdout
	if (strcmp(opts->output, "-") == 0)
		sprintf(ofname, "-");
	else if (opts->random >= 0 && opts->output[0] != 0)
		sprintf(ofname, "%s/%s.png", opts->output, coeffstr);
	else if (opts->random < 0 && opts->output[0] != 0)
		strcpy(ofname, opts->output);
	else {
		char ocstr[13];
		coeffstring(&fractal, ocstr);
		ocstr[12] = 0;
		sprintf(ofname, "%s.png", ocstr);
	}
	
	if (maxval == 0) maxval = opts->maxval;
	if ((pngwriter = init_pngwriter(&fractal, ofname, maxval)) == NULL) {
		fprintf(stderr, "pngwriter init failed\n");
		return 1;
	}
	cfret = computefractal(&fractal, pngwriter_pixelcb, pngwriter);
	finish_pngwriter(pngwriter);
	return cfret;
}
