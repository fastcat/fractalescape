// this to only be included from escape.h

extern inline void advance(point_t *pos, const fractalparams_t *fractal) {
	double xnew = fractal->coeffs[0] + pos->x*(fractal->coeffs[1] + fractal->coeffs[2]*pos->x + fractal->coeffs[3]*pos->y) + pos->y*(fractal->coeffs[4] + fractal->coeffs[5]*pos->y);
	pos->y = fractal->coeffs[6] + pos->x*(fractal->coeffs[7] + fractal->coeffs[8]*pos->x + fractal->coeffs[9]*pos->y) + pos->y*(fractal->coeffs[10] + fractal->coeffs[11]*pos->y);
	pos->x = xnew;
}

extern inline int escaped(const point_t *pos) {
	return ((pos->x * pos->x) + (pos->y * pos->y)) > ESCAPED_MINVAL
		? 1 : 0;
}
