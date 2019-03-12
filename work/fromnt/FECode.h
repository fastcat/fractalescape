#ifndef __FECODE_H__
#define __FECODE_H__
//fractal escape code
#include <stdio.h>
#include <time.h>
#include "platform.h"


//function declarations
//inline void advancexy(double &x, double &y, long &n);	//inlined in .cpp
void display();		//writes fractal to .bmp file
const double *genarray(const char *str, int len);
const char *genbasename(const double *a, int len, int forcea = 0);
const char *genfname(const double *a, int len, int useMAT);
const char *genlname(const double *a, int len);
void genrandfractals();		//generate random fractals
void genspecificfractal();		//generate specific fractal from FractalToGenerate
FILE *prepfile(const char *fn);		//writes bitmap header, etc
void writedefaultcolormap(FILE *f);		//generate the default colormap
void writecolormapfromfile(FILE *f, FILE *colormapfile);		//copy a colormap from a winfract-style .map file
void writecolormap(FILE *f);		//generate the appropriate color map for a file
void setparams(double &x, double &y);
void showterminate();		//atexit hook function
void clearlogbuffer();	//atexit hook function
//inline void testsoln(double &x, double &y, long &n);	//inlined
void writeinfo(const double *a, int len);
void bufwrite(const void *w, unsigned long len, FILE *f, char *b, unsigned long &p, unsigned long blen);

//globals for calculation
extern double a[12];
extern long n;
extern double x, y;
//command line parameters and their defaults
extern int LogToFile;
extern int LogToScreen;
extern int GenInfo;
extern int GenRandom;
extern int GenMatlab;
extern int MatLabMax;
extern char FractalToGenerate[13];		//12 charachters, 1 null terminator
extern char OutputFileName[512];
extern double Left, Right, Bottom, Top;
extern int Width, Height;
extern int MaxGenerate;
extern char ColorMapFile[_MAX_PATH];
extern int UseColorMap;
//other globals
extern long FractalsGenerated;
extern time_t MainStart;		//initialized by main()
//global constants
extern const char VTag[];

#endif //__FECODE_H__