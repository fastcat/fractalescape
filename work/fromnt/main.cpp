#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <time.h>

#include "FECode.h"

void processcmdline(int argc, char *argv[])
{
	int n;
	for(n = 1; n < argc; n++)
	{
		if ((argv[n][0] == '/') || (argv[n][0] == '-'))		//check for parameter prefix
		{
			switch (toupper(argv[n][1]))
			{
			case 'B':
				sscanf(argv[n]+2, "%lf,%lf,%lf,%lf", &Left, &Right, &Top, &Bottom);
				break;
			case 'C':
				if (n==argc-1)
					logstr("Error, no colormap file specified with C parameter: using default colormap\n");
				else
				{
					n++;
					strcpy(ColorMapFile, argv[n]);
					UseColorMap = 1; //true
				};
				break;
			case 'D':
				LogToFile = (argv[n][2] - '0');
				break;
			case 'I':
				GenInfo = (argv[n][2] == '+');
				break;
			case 'L':
				sscanf(argv[n]+2, "%d", &MatLabMax);
				break;
			case 'M':
				GenMatlab = (argv[n][2] == '+');
				break;
			case 'N':
				sscanf(argv[n]+2,"%d",&MaxGenerate);
				break;
			case 'O':
				sscanf(argv[n]+2,"%s",OutputFileName);
				break;
			case 'S':
				LogToScreen = (argv[n][2] - '0');
				break;
			case 'R':
				if (argv[n][2] == '+')
					GenRandom = 2;		//this specifies a forced random
				else
					GenRandom = 0;
				break;
			case 'F':
				strncpy(FractalToGenerate, argv[n]+2, 13);		//13 to ensure the copying of the NULL charachter
				break;
			case 'X':
				sscanf(argv[n]+2, "%d,%d", &Width, &Height);
				break;
			case '?':
			default:
					logstr(VTag);
					logstr(" Usage:\n");
					logstr("All parameters must be preceeded by / or -\n");
					logstr("Parameter   | Function\n");
					logstr("Bl,r,t,b    | Specify bounds of display\n");
					logstr("D(0,1,2)    | Set Disk Logging Level\n");
					logstr("C filename  | Use colormap filename\n");
					logstr("Fparameters | Generate Specific Fractal\n");
					logstr("I(+/-)      | Generate Textfile with paramaters for each fractal\n");
					logstr("Lnum        | Sets the infinity threshold for MAT files\n");
					logstr("M(+/-)      | Output to .MAT file for use with MATLAB\n");
					logstr("Nn          | Generate n fractals (Random Mode Only)\n");
					logstr("Ofilename   | Output image to specific file (do not include extension)\n");
					logstr("R(+/-)      | Generate Random Fractals\n");
					logstr("S(0,1,2)    | Set Screen Logging Level\n");
					logstr("Xw,h        | Set the width and heignt of output image\n");
					logstr("Notes:\n");
					logstr("Colormaps are ascii fles of the same format as those used by WinFract\n");
					logstr("Parameters D and S are not mutually exclusive.\n");
					logstr("If R+ is specified, F and O are ignored\n");
					logstr("If R is not specified and F is, R- is used\n");
					logstr("The parameters after F are 12 letters A-Y\n");
					logstr("When using R+ press any key to stop the program\n");
					logstr("Logging Levels: 0=None; 1=Omit Testing Log; 2=Everything\n");
					logstr("If the N parameter less than zero, fractals will be generated ad infinitum\n");
					logstr("If O is not specified, a name will automatically be assigned\n");
					logstr("The default parameters are:\n");
					logstr("/B-5,5,5,-5 /D0 /I- /L256 /M- /N-1 /S2 /R+ /X640,480\n");
					exit(0);		//this is before atexit call, so no terminate message is shown
				break;
			};
		};
	};
};

void main(int argc, char *argv[])
{
	processcmdline(argc, argv);
	atexit(termlogstr);	//calls showterminate
	initlogstr();		//initialize logging buffer
	switch(GenRandom)
	{
		case 0:
			if (FractalToGenerate[0] != 0)
				genspecificfractal();
			else
				logstr("No Specific Fractal Specified with parameter R-\n");
			break;
		case 1:
			if (FractalToGenerate[0] != 0)
				genspecificfractal();
			else
				genrandfractals();
			break;
		case 2:
		default:
			genrandfractals();
	};
	exit(0);		//make sure atexit function is called
};

