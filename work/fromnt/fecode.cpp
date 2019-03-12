#include "StdAfx.h"

#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <time.h>

#include "FECode.h"
#include "platform.h"

//globals for calculation
double a[12];
long n = 0;
double x = 0, y = 0;
//command line parameters and their defaults
int LogToFile = 0;
int LogToScreen = 2;
int GenInfo = 0;
int GenRandom = 1;
int GenMatlab = 0;
int MatLabMax = 256;
char FractalToGenerate[13] = "";		//12 charachters, 1 null terminator
char OutputFileName[512] = "";
double Left=-5, Right=5, Bottom=-5, Top=5;
int Width = 640, Height = 480;
int MaxGenerate = -1;
char ColorMapFile[_MAX_PATH];
int UseColorMap = 0;
//other globals
long FractalsGenerated = 0;
time_t MainStart = 0;		//initialized by main()
//global constants
const char VTag[] = "Fractal Generator 1.7";
const double escaped = 1000000L;

inline void advancexy(double &x, double &y, long &n)
{
	double xnew = a[0] + x*(a[1] + a[2]*x + a[3]*y) + y*(a[4] + a[5]*y);
	y = a[6] + x*(a[7] + a[8]*x + a[9]*y) + y*(a[10] + a[11]*y);
	x = xnew;
	n++;
};

inline void testsoln(double &x, double &y, long &n)
{
	/*  these two lines are redundant
	if (n == 1000)
		n = 0;
	*/
	if ((x*x + y*y) > escaped)
	{
		if (n > 100)
			n = 1000;
		else
			n = 0;
	};
};

void showterminate()
{
	char termstr[512];
	time_t Term = time(NULL);
	int RunTime = time(NULL)-MainStart;
	double spf = (double)RunTime / (double)FractalsGenerated;
	sprintf(termstr, "%s Terminating On %sProgram ran for %ld seconds\nProgram generated %ld fractal(s)\nAverage Speed: %g spf\nDone.\n",
	//ctime includes newline, thus none   ^Here
		VTag, ctime(&Term), RunTime, FractalsGenerated, spf);
	logstr(termstr);
};

void genrandfractals()
{
	FractalsGenerated = 0;
	while (1)
	{
		if (requestAbort() || ((FractalsGenerated >= MaxGenerate) && (MaxGenerate >= 0)))
			return;
		if (n == 0)
		{
			setparams(x, y);
			logstr("Thinking...",2);
			logstr(genfname(a, 12, GenMatlab),2);
			logstr("\n",2);
		};
		advancexy(x, y, n);
		testsoln(x, y, n);
		if (n == 1000)
		{
			display();
			n = 0;
		};
	};
};

void genspecificfractal()
{
	FractalsGenerated = 0;
	memcpy(a, genarray(FractalToGenerate, strlen(FractalToGenerate)), sizeof(double)*12);		//set a
	display();
};

void setparams(double &x, double &y)
{
	x=0;
	y=0;
	for (int i = 0; i < 12; i++)
			a[i] = ((rand()%51)-25)/10.0;
};

void display()		//writes fractal to .bmp file
{
	if (GenInfo)
		{
		logstr("Writing Info File...");
		logstr(genlname(a,12));
		logstr("\n");
		writeinfo(a,12);
	};
	logstr("Writing...");
	logstr(genfname(a, 12, GenMatlab));
	logstr("\n");
	time_t start = time(NULL);
	FILE *f;
	char *WriteBuffer;
	const unsigned long WBLen = 65536;
	if (!GenMatlab)
		WriteBuffer = new char[WBLen];		//create a dynamic 64k buffer only if it's going to be used
	unsigned long WBPos = 0;
	if (GenMatlab)											//don't write bmp header for MAT files
		f = fopen(genfname(a,12,GenMatlab), "wb");
	else
		f = prepfile(genfname(a, 12, GenMatlab));
	double x, y;
	char tmp;
	logstr("Each \'.\' is 10%% Complete:");
	int HeightD10 = Height / 10;
	double xFact = ((double)Width) / (Right - Left);
	double yFact = ((double)Height) / (Top - Bottom);
	int nmax = 252;
	if (GenMatlab)
		nmax = MatLabMax;
	for(int j = Height-1; j >= 0; j--)
	{
		if ((j % HeightD10) == 0)
			logstr(".");
		for(int i = 0; i < Width; i++)
		{
			x = Left + i/xFact;
			y = Top - j/yFact;
			n = 0;
			while ((n < nmax) && ((x*x + y*y) < escaped))
				advancexy(x, y, n);
			tmp = n%120;
			if (GenMatlab)
				fprintf(f, "  %ld", n);
			else
				bufwrite(&tmp, 1, f, WriteBuffer, WBPos, WBLen);
				//fwrite(&tmp, 1, 1, f);
		};
		tmp = 0;
		if (GenMatlab)
			fprintf(f,"\n");
		else
			if ((Width%4) > 0)
				for (int q = 0; q < 4-(Width%4); q++)
					bufwrite(&tmp, 1, f, WriteBuffer, WBPos, WBLen);
					//fwrite(&tmp,1,1,f);		//bmp is padded to 4-byte boundary
	};
	if (!GenMatlab)
	{
		if (WBPos > 0)		//need to flush buffer
			fwrite(WriteBuffer, 1, WBPos, f);
		delete[] WriteBuffer;
	};
	fclose(f);
	FractalsGenerated++;
	logstr("\n");		//log line-end for status dots
	logstr("Done writing...");
	logstr(genfname(a, 12, GenMatlab));
	logstr("\n");
	char tstr[100];
	sprintf(tstr, "Elapsed Time (seconds): %ld\n", time(NULL) - start);
	logstr(tstr);
};

const char *genbasename(const double *a, int len, int forcea)
{
	static char res[520];
	int n;
	for (n = 0; n < 520; n++)	res[n] = 0;
/*	if (((GenRandom < 2) || (MaxGenerate = 1)) && (strlen(OutputFileName) > 0) && (!forcea))
	{	//use output file name if it is specified and we are generating only one fractal (specific or 1 random) and we're not forced otherwise
		strcpy(res, OutputFileName);
		n = strlen(OutputFileName);
	}
	else
	{*/
		for (n = 0; n < len; n++)
		{
			if (a[n] <= 0)	//lowercase
				res[n] = (char)((a[n] * 10) + 25) + 'a';
			else
				res[n] = (char)((a[n] * 10) - 1) + 'A';
		};
//	};
	res[n] = '.';
	return res;
};

const char *genfname(const double *a, int len, int useMAT)
{
	static char res[520];
	int n;
	for (n = 0; n < 520; n++)	res[n] = 0;
	if (((GenRandom < 2) || (MaxGenerate == 1)) && (strlen(OutputFileName) > 0))
		strcpy(res, OutputFileName);
	else
	{
		strcpy(res, genbasename(a,len));
		n = strlen(res);
		if (useMAT)
			strcpy(res+n,"MAT");
		else
			strcpy(res+n,"BMP");
	};
	return res;
};

const char *genlname(const double *a, int len)
{
	static char res[520];
	int n;
	for (n = 0; n < 520; n++)	res[n] = 0;
	strcpy(res, genbasename(a,len));
	n = strlen(res);
	strcpy(res+n,"TXT");
	return res;
};

const double *genarray(const char *str, int len)
{
	static double res[12];
	int n;
	for (n = 0; n < 12; n++)	res[n] = 0;
	for (n = 0; n < len; n++)
	{
		if (isupper(str[n]))
			res[n] = (str[n] - 'A' + 1) / 10.0;
		else
			res[n] = (str[n] - 'a' - 25) / 10.0;
	};
	return res;
};

FILE *prepfile(const char *fn)		//writes bitmap header, etc
{
	long tmp;
	FILE *f = fopen(fn, "wb");
	//header
	fwrite("BM", 2, 1, f);		//write signature
	int wm4, hm4;
	wm4 = (Width%4) > 0 ? 4-(Width%4) : 0;
	hm4 = (Height%4) > 0 ? 4-(Height%4) : 0;
	tmp = 1078 + ((Width + wm4) * (Height + hm4));		//calculate size of file
	fwrite(&tmp, 4, 1, f);		//write file size
	tmp = 0;
	fwrite(&tmp, 4, 1, f);		//reserved
	tmp = 0x436;
	fwrite(&tmp, 4, 1, f);		//write offset to bmp data
	//info
	tmp = 40;
	fwrite(&tmp, 4, 1, f);		//write info size
	tmp = Width;
	fwrite(&tmp, 4, 1, f);		//write width
	tmp = Height;
	fwrite(&tmp, 4, 1, f);		//write height
	tmp = 1;
	fwrite(&tmp, 2, 1, f);		//write color planes
	tmp = 8;
	fwrite(&tmp, 2, 1, f);		//write bpp
	tmp = 0;
	fwrite(&tmp, 4, 1, f);		//write compression method
	tmp = 0x4B000;
	fwrite(&tmp, 4, 1, f);		//write image size
	tmp = 0;
	fwrite(&tmp, 4, 1, f);		//write hres
	fwrite(&tmp, 4, 1, f);		//write vres
	tmp = 256;
	fwrite(&tmp, 4, 1, f);		//write indecies used
	fwrite(&tmp, 4, 1, f);		//write indecies important
	writecolormap(f);
	return f;
};

void writedefaultcolormap(FILE *f)
{
	long n, tmp;
	int r,g,b;
	double blendfact = 256/42.0;
	for(n = 0; n < 252; n++)
	{
		//generate rgb color from hue
		if (n < 42)
		{
			r = 255;
			g = (int)(blendfact*n);
			b = 0;
		}
		else if (n < 84)
		{
			r = (int)(255 - (blendfact*n));
			g = 255;
			b = 0;
		}
		else if (n < 126)
		{
			r = 0;
			g = 255;
			b = (int)(blendfact*n);
		}
		else if (n < 168)
		{
			r = 0;
			g = (int)(255 - (blendfact*n));
			b = 255;
		}
		else if (n < 210)
		{
			r = (int)(blendfact*n);
			g = 0;
			b = 255;
		}
		else
		{
			r = 255;
			g = 0;
			b = (int)(255 - (blendfact*n));
		};
		r = r & 0xFF;
		g = g & 0xFF;
		b = b & 0xFF;
		tmp = (r << 16) + (g << 8) + b;
		fwrite(&tmp, 4, 1, f);		//write a color value
	};
	tmp = 0;
	for(n = 0; n < 6; n++)
		fwrite(&tmp, 4, 1, f);			//write the zero color values to pad the map to 256 colors
};

void writecolormapfromfile(FILE *f, FILE *colormapfile)
{
	//precond: colormapfile must be a valid open file for reading
	//precond: f must be a valid open file for writing
	//this will copy a winfract style colormap to the bmp header
	//the format of the winfract file is each line is a color in r g b
	//there can be extraneous comments after the three numbers
	long n=0, tmp;
	int r,g,b;
	char line[256] = "";
	//because of the way color entries are used, we must reverse the order
	//of the winfract colormap
	//temporary storage for color entries
	long maptmp[120];
	do
	{
		fgets(line, 256, colormapfile);
		//a line with format r g b has a length of at least 6 chars, including the newline
		//this checks to make sure we haven't hit the end of the file, etc
		if (strlen(line) < 6)	break;
		//retrieve the integer values out of the line and write them to the bmp header
		sscanf(line, "%d %d %d", &r,&g,&b);
		r = r & 0xFF;
		g = g & 0xFF;
		b = b & 0xFF;		//make sure we only have positive 1-byte values
		tmp = (r << 16) + (g << 8) + b;
		maptmp[n]=tmp;
		n++;
	}
	while (n<256);		//the eof case is handled by the if strlen line
	long bign = n;		//what was the last item index we read?
	for (n=0; n < bign; n++)
		fwrite(&(maptmp[n]), 4, 1, f);		//write a color value
	tmp = 0;
	for(n = bign; n < 256; n++)
		fwrite(&tmp, 4, 1, f);			//write the zero color values to pad the map to 256 colors
	//file cleanup for colormap file is handled by the calling function
};

void writecolormap(FILE *f)
{
	if (!UseColorMap)	//no color map specified
		writedefaultcolormap(f);
	else
	{
		FILE *colormapfile = fopen(ColorMapFile, "r");
		if (colormapfile != NULL)
		{
			writecolormapfromfile(f, colormapfile);
			fclose(colormapfile);
		}
		else
		{
			logstr("Error: could not open colormap ");
			logstr(ColorMapFile);
			logstr(" - using default colormap\n");
			writedefaultcolormap(f);
		};
	};
};

void writeinfo(const double *a, int len)
{
	time_t now = time(NULL);
	FILE *f = fopen(genlname(a, len), "a");
	fprintf(f,"Fractal Escape Info File Created %sFractal Parameters: %s\nLeft = %g\nRight = %g\nTop = %g\nBottom = %g\nWidth = %d\nHeight = %d\n",
		ctime(&now),genbasename(a,len,1),Left,Right,Top,Bottom,Width,Height);
	fclose(f);
};

void bufwrite(const void *w, unsigned long len, FILE *f, char *b, unsigned long &p, unsigned long blen)
{
	if (len > (blen - p))
	//write the data out
	{
		fwrite(b, 1, p, f);
		p = 0;
	};
	memcpy(b + p, w, len);
	p += len;
};
