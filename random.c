#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <signal.h>

#include "random.h"
#include "escape.h"
#include "varscore.h"
#include "makefract.h"

int abort_random = 0;

void sigint_handler(int signo) {
	// set an abort flag
	abort_random = 1;
}

int gen_random_fractals(const fe_opts_t *opts) {
	struct timeval tv;
	fractalparams_t fractal;
	double score, bestscore;
	int i, j, n, rfn, cfret, maxval, bestmaxval;
	int ics[12];
	char coeffstr[13];
	char bestcoeffstr[13];
	void *varscore;
	struct sigaction sa;
	
	// setup sigint aborting
	abort_random = 0;
	sa.sa_handler = sigint_handler;
	sa.sa_flags = SA_RESETHAND;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) != 0) {
		fprintf(stderr, "signal setup failed\n");
		return 1;
	}
	
	// copy from opts into fractal
	if (opts2fract(&fractal, opts) != 0) return 1;

	// settings for thumbnail evaluation
	fractal.osfact = 1;
	fractal.width /= 10;
	fractal.height /= 10;

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec ^ tv.tv_usec);

	for (rfn = 0; rfn < opts->random || opts->random == 0; ++rfn) {
		bestscore = -1;
		i = j = 0;
		while (i < 100) {
			if (abort_random) return 1;
			
			for (n = 0; n < 12; ++n) {
				ics[n] = (rand() % 51) - 25;
			}
			initcoeffs(&fractal, ics);
			coeffstring(&fractal, coeffstr);
			coeffstr[12] = 0;
			
			// basic filter first
			if (escapetime(0, 0, &fractal) < 0) {
				if ((varscore = init_varscore()) == NULL) {
					fprintf(stderr, "varscore init failed\n");
					return 1;
				}
				
				cfret = computefractal(&fractal, varscore_pixelcb, varscore);
				if (cfret != 0) {
					fprintf(stderr, "Fractal generation for %s failed\n", coeffstr);
					return 1;
				}
				score = finish_varscore(varscore, &maxval);
				if (score > bestscore) {
					coeffstring(&fractal, bestcoeffstr);
					bestcoeffstr[12] = 0;
					bestmaxval = maxval;
					bestscore = score;
				}
				++i;
			}
			++j;
		}

		fprintf(stderr, "Best out of %d is %s (scored %.1f)\n", j, bestcoeffstr, bestscore);
		
		cfret = writefractal(opts, bestcoeffstr, maxval);
		fprintf(stderr, "Done with fractal, retval = %d\n", cfret);
		if (cfret != 0) return 1;
	}
	
	return 0;
}
