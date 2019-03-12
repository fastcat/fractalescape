#ifndef __ESCAPE_H__
#define __ESCAPE_H__

//// constants

// this is really the square of the escape value
#define ESCAPED_MINVAL ((double)1e6)

// the number of iterations to try before concluding it didn't escape
#define ESCAPE_MAXITERS 100

//// structs and typedefs

// storage for coefficients representing the fractal
// the integer coefficients are the official ones used for generating
// string representations, the doubles are used for actual calculation
// width and height specify the image size
// top,...,right specify the numeric bounds of the image
// osfact (oversample factr) is the factor by which to oversample in both the
// x and y axes, i.e. a value of 2 will cause 4 values to be computed and
// averaged for each pixel
typedef struct {
	int intcoeffs[12];
	double coeffs[12];
	int width, height;
	double top, left, bottom, right;
	int osfact;
	int maxval;
} fractalparams_t;

// simple struct representing a point
typedef struct {
	double x;
	double y;
} point_t;

// function pointer for pixel writing callbacks
// functions like int funcname(int, int, signed long, void *)
// the two ints are the image pixel x and y positions (0-based)
// the double is the escape time, <0 means didn't escape
// the escape time is a double so that averages can be handled nicely
// the next int is the number of samples that were unescaped and therefore
// averaged in as zero
// the void* is for user data
// the return value should be 0 to indicate okay, something else to bail out
// of the fractal generation
typedef int (*pixelcb_t)(int, int, double, int, void *);

//// functions

// initializes the coefficients for a fractal
// int coeffs are the real coeffs * 10
// real coeffs must be >= -2.5 and <= 2.5
// a return value of zero indicates all OK, otherwise a coefficient was bad
int initcoeffs(fractalparams_t *fractal, int coeffs[12]);

// initializes coefficients from a string, see coeffstring for more info
int initcoeffs_fromstring(fractalparams_t *fractal, const char *coeffstr);

// writes a string representation of a fractal's coefficients
// exactly 12 bytes will be written
// no null terminator is written
// returns the string back for convenience
char *coeffstring(fractalparams_t *fractal, char *str);

// runs the fractal equation against point and sets point to the next point
// along the attractor
inline void advance(point_t *pos, const fractalparams_t *fractal);

// returns a boolean indicating whether or not the point qualifies as having
// escaped
inline int escaped(const point_t *pos);

// calculates the escape time for a point, given a fractal
// returns -1 if the point never escaped
signed long escapetime(double x, double y, const fractalparams_t *fractal);

// generates a fractal, reporting pixel values via the pixelcb
// pixels will be reported in order, left to right, top to bottom
// returns 0 on success, something else on error
int computefractal(const fractalparams_t *fractal, pixelcb_t pixelcb,
	void *userdata);

// put the inline functions in a separate file to keep header clean
#include "escape-inlines.h"

#endif // __ESCAPE_H__
