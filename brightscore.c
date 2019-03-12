#include <stdlib.h>

#include "brightscore.h"
#include "escape.h"

typedef struct {
	unsigned long long pixelsum;
	unsigned long nunecount;
	unsigned long pixelcount;
} brightscore_state;

void *init_brightscore() {
	brightscore_state *bss;
	
	if ((bss = malloc(sizeof(brightscore_state))) == NULL)
		return NULL;
	bss->pixelsum = 0;
	bss->nunecount = 0;
	bss->pixelcount = 0;
	
	return bss;
}

int brightscore_pixelcb(int x, int y, double etime, int nune, void *userdata) {
	brightscore_state *bss = userdata;
	
	bss->pixelsum += etime;
	bss->nunecount += nune;
	++bss->pixelcount;
	
	return 0;
}

double finish_brightscore(void *userdata) {
	brightscore_state *bss = userdata;
	
	double ret = (double)bss->pixelsum / (double)bss->pixelcount;
	// nonescaping is good, in moderation
	double nunep = (double)bss->nunecount / (double)bss->pixelcount;
	if (nunep > ret)
		ret -= nunep - ret;
	
	free(bss);
	
	return ret;
}
