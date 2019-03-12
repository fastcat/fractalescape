#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "escape.h"

int initcoeffs(fractalparams_t *fractal, int coeffs[12]) {
	int n;
	for (n = 0; n < 12; ++n) {
		if ((coeffs[n] < -25) || (coeffs[n] > 25)) return 1;
		fractal->intcoeffs[n] = coeffs[n];
		fractal->coeffs[n] = ((double)coeffs[n]) / 10.0;
	}
	return 0;
}

int initcoeffs_fromstring(fractalparams_t *fractal, const char *coeffstr) {
	int coeffs[12];
	int n;
	
	for (n = 0; n < 12; ++n) {
		if ((coeffstr[n] >= 'a') && (coeffstr[n] <= 'z'))
			coeffs[n] = (int)(coeffstr[n] - 'a') - 25;
		else if ((coeffstr[n] >= 'A') && (coeffstr[n] <= 'Y'))
			coeffs[n] = (int)(coeffstr[n] - 'A') + 1;
		else
			// invalid character
			return 1;
	}
	return initcoeffs(fractal, coeffs);
}

char *coeffstring(fractalparams_t *fractal, char *str) {
	int n;
	
	if (str == NULL) return NULL;
	for (n = 0; n < 12; ++n)
		if (fractal->intcoeffs[n] <= 0)
			str[n] = (char)(fractal->intcoeffs[n] + 25) + 'a';
		else
			str[n] = (char)(fractal->intcoeffs[n] - 1) + 'A';
	
	return str;
}

signed long escapetime(double x, double y, const fractalparams_t *fractal) {
	int n = 0;
	point_t pos;
	
	pos.x = x;
	pos.y = y;
	
	while ((n < ESCAPE_MAXITERS) && (!escaped(&pos))) {
		advance(&pos, fractal);
		++n;
	}
	
	return escaped(&pos) ? n : -1;
}

int computefractal(const fractalparams_t *fractal, pixelcb_t pixelcb,
		void *userdata) {
	int pixelx, pixely, samplex, sampley;
	signed long etime;
	unsigned long etimesum;
	double etimeavg;
	int nsamples;
	int nunescaped;
	double xfact, yfact, xosfact, yosfact;
	int cbret;

	if (fractal->osfact < 1) return 1;

	nsamples = fractal->osfact * fractal->osfact;
	
	xfact = (fractal->right - fractal->left) / fractal->width;
	yfact = (fractal->top - fractal->bottom) / fractal->height;
	xosfact = xfact / fractal->osfact;
	yosfact = xfact / fractal->osfact;
	
	for (pixely = 0; pixely < fractal->height; ++pixely) {
		for (pixelx = 0; pixelx < fractal->width; ++pixelx) {
			// sum all the values and then average them
			// to oversample nicely, we treat unescaped points as max escapetime,
			// unless all were unescaped, in which case we keep the didn't escape
			etimesum = 0;
			nunescaped = 0;
			for (samplex = 0; samplex < fractal->osfact; ++samplex) {
				for (sampley = 0; sampley < fractal->osfact; ++sampley) {
					etime = escapetime(
						fractal->left + pixelx * xfact + samplex * xosfact,
						fractal->top - pixely * yfact - sampley * yosfact,
						fractal);
					if (etime < 0) {
						etime = 0;
						++nunescaped;
					}
					etimesum += etime;
				}
			}
			
			etimeavg = (double)etimesum / (double)nsamples;
			if (etimeavg > ESCAPE_MAXITERS) {
				fprintf(stderr, "err: eta:%.1f ets:%ld ns:%d nune:%d\n", etimeavg, etimesum, nsamples, nunescaped);
				assert(etimeavg <= ESCAPE_MAXITERS);
			}
			
			if (pixelcb != NULL) {
				cbret = pixelcb(pixelx, pixely, etimeavg, nunescaped, userdata);;
				if (cbret != 0) return cbret;
			}
		}
	}
	
	return 0;
}
