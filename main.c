
#include "escape.h"
#include "args.h"
#include "random.h"
#include "makefract.h"

/*
	int pixelcb(int x, int y, double val, int nune, void *data) {
		fprintf(stderr, "Pixel at %d, %d calculated as %.1f (%d esc)\n", x, y, val, nune);
		return 0;
	}
*/

int main(int argc, char *argv[]) {
	fe_opts_t opts;
	int paret;
	
	paret = parse_args(argc, argv, &opts);
	if (paret != 0) {
		print_usage(stderr, paret);
		return 1;
	}
	
	if (opts.random >= 0)
		return gen_random_fractals(&opts);
	else if (opts.fractal[0] != 0) {
		int cfret = writefractal(&opts, NULL, 0);
		fprintf(stderr, "Done with fractal, retval = %d\n", cfret);
		if (cfret != 0) return 1;
	} else {
		print_usage(stderr, 0);
	}
	
	return 0;
}
