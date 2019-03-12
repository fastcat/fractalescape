#include <stdlib.h>

#include "varscore.h"
#include "escape.h"

typedef struct {
	int valcounts[ESCAPE_MAXITERS + 1];
	unsigned long pixelcount;
} varscore_state;

void *init_varscore() {
	varscore_state *vss;
	int n;
	
	if ((vss = malloc(sizeof(varscore_state))) == NULL)
		return NULL;
	for (n = 0; n <= ESCAPE_MAXITERS; ++n)
		vss->valcounts[n] = 0;
	vss->pixelcount = 0;
	
	return vss;
}

int varscore_pixelcb(int x, int y, double etime, int nune, void *userdata) {
	varscore_state *vss = userdata;
	int bin = (int)etime;
	
	++vss->valcounts[bin];
	++vss->pixelcount;
	
	return 0;
}

double finish_varscore(void *userdata, int *maxval) {
	varscore_state *vss = userdata;
	int n, peakcount, bincount, dpixcount, stopcount, skipnum, skipcount;
	double ret;
	
	dpixcount = 0;
	bincount = 0;
	skipnum = 0;
	skipcount = 0;
	stopcount = 0;
	if (maxval != NULL) *maxval = 0;
	for (n = ESCAPE_MAXITERS; n >= 0; --n) {
		if (vss->valcounts[n] > 0) {
			if (maxval != NULL) *maxval = n;
			break;
		}
	}
	if (*maxval > 0) *maxval = *maxval * 3 / 2;
	if (*maxval > 255) *maxval = 255;
	// we break out of this loop manually
	// this would be faster if we sorted the valcounts first, but
	// the list is short enough it doesn't matter much
	while (1) {
		peakcount = 0;
		for (n = 1; n <= ESCAPE_MAXITERS; ++n) {
			if (vss->valcounts[n] > vss->valcounts[peakcount])
				peakcount = n;
		}
		// ignore the most common values
		if (skipnum < 2) {
			skipcount += vss->valcounts[peakcount];
			++skipnum;
			if (skipnum == 2)
				stopcount = (vss->pixelcount - skipcount) * 2/3;
		} else {
			if (dpixcount + vss->valcounts[peakcount] >= stopcount) {
				ret = bincount + (double)(stopcount - dpixcount) /
					(double)vss->valcounts[peakcount];
				// too much variation generally means noise, which is bad
				// 5-10 seems a good range
				// less than 5 is too boring
				// greater than 10 generates more and more noise
				if (ret > 10)
					ret = ESCAPE_MAXITERS - ret + 5;
				else if (ret > 5)
					ret = ESCAPE_MAXITERS - 10 + ret;
				// we modulate ret by the fraction that was ignored as background
				ret *= 1.0 - ((double)skipcount / (double)vss->pixelcount);
				break;
			} else {
				dpixcount += vss->valcounts[peakcount];
				++bincount;
			}
		}
		vss->valcounts[peakcount] = 0;
	}
	
	free(vss);
	
	return ret;
}
