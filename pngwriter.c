#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <png.h>

#include "escape.h"
#include "pngwriter.h"

// state/userdata structure for pngwriter pixel callback
typedef struct {
	int width;
	png_byte *rowbuf;
	int rowbufpos;
	int maxval;
	FILE *fp;
	png_structp pngptr;
	png_infop infoptr;
} pngwriter_state;

void *init_pngwriter(fractalparams_t *fractal, const char *filename, int maxval) {
	pngwriter_state *pws;
	
	if ((pws = malloc(sizeof(pngwriter_state))) == NULL)
		goto error_pws;
	
	pws->width = fractal->width;
	pws->maxval = (maxval == 0) ? ESCAPE_MAXITERS : maxval;
	pws->rowbufpos = 0;
	
	if (strcmp(filename, "-") == 0)
		pws->fp = stdout;
	else if ((pws->fp = fopen(filename, "wb")) == NULL)
		goto error_file;
	
	if ((pws->rowbuf = calloc(pws->width * 3, sizeof(png_byte))) == NULL)
		goto error_rowbuf;
	
	if ((pws->pngptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL)) == NULL)
		goto error_png;
	
	if ((pws->infoptr = png_create_info_struct(pws->pngptr)) == NULL)
		goto error_info;
	
	png_init_io(pws->pngptr, pws->fp);
	
	// maybe call png_set_filter and png_set_compression_level here?
	
	// 8 bits per channel, 24bit RGB total
	png_set_IHDR(pws->pngptr, pws->infoptr, fractal->width, fractal->height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	
	png_write_info(pws->pngptr, pws->infoptr);
	
	// maybe call png_set_flush here to keep memory usage under control?
	
	return (void*)pws;
	
	// error handling, unwind the creation steps
//error_all:
	png_destroy_info_struct(pws->pngptr, &pws->infoptr);
error_info:
	png_destroy_write_struct(&pws->pngptr, NULL);
error_png:
	free(pws->rowbuf);
error_rowbuf:
	fclose(pws->fp);
error_file:
	free(pws);
error_pws:
	return NULL;
}

int pngwriter_pixelcb(int x, int y, double etime, int nunescaped,
		void *userdata) {
	pngwriter_state *pws = userdata;
	int n;
	long scaledpixel;
	
	// sanity checks
	assert(x * 3 == pws->rowbufpos);
	assert(x < pws->width);
	
	// TODO: add palette mapping
	
	// add item to row buffer
	// we ignore the nunescaped since we treat unescaped as zero and thus black
	scaledpixel = lround(etime * 255 / pws->maxval);
	if (scaledpixel > 255) scaledpixel = 255;
	for (n = 0; n < 3; ++n)
		pws->rowbuf[pws->rowbufpos + n] = scaledpixel;
	pws->rowbufpos += 3;
	
	// push row buffer out to png if we're at the end of the line
	if (x == pws->width - 1) {
		png_write_row(pws->pngptr, pws->rowbuf);
		pws->rowbufpos = 0;
	}
	
	return 0;
}

void finish_pngwriter(void *userdata) {
	pngwriter_state *pws = userdata;

	png_write_end(pws->pngptr, pws->infoptr);
	png_destroy_write_struct(&pws->pngptr, &pws->infoptr);
	free(pws->rowbuf);
	fclose(pws->fp);
	free(pws);
}
