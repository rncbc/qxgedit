// XGParam.cpp
//
/****************************************************************************
   Copyright (C) 2005-2023, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "XGParam.h"

#include <QRegularExpression>

#include <cstdio>
#include <cstdlib>

#include <ctime>

// Table (2D-array) size in elements.
#define TSIZE(tab)	(sizeof(tab) / sizeof(tab[0]))


//-------------------------------------------------------------------------
// XG Effect Data Value Assign Tables
//

//-------------------------------------------------------------------------
// Table#1 - LFO frequency (Hz)

static
float ftab1[] =
{
	 0.00f,  0.08f,  0.08f,  0.16f,  0.16f,  0.25f,  0.25f,  0.33f,
	 0.33f,  0.42f,  0.42f,  0.50f,  0.50f,  0.58f,  0.58f,  0.67f,
	 0.67f,  0.75f,  0.75f,  0.84f,  0.84f,  0.92f,  0.92f,  1.00f,
	 1.00f,  1.09f,  1.09f,  1.17f,  1.17f,  1.26f,  1.26f,  1.34f,
	 1.34f,  1.43f,  1.43f,  1.51f,  1.51f,  1.59f,  1.59f,  1.68f,
	 1.68f,  1.76f,  1.76f,  1.85f,  1.85f,  1.93f,  1.93f,  2.01f,
	 2.01f,  2.10f,  2.10f,  2.18f,  2.18f,  2.27f,  2.27f,  2.35f,
	 2.35f,  2.43f,  2.43f,  2.52f,  2.52f,  2.60f,  2.60f,  2.69f,
	 2.69f,  2.77f,  2.86f,  2.94f,  3.02f,  3.11f,  3.19f,  3.28f,
	 3.36f,  3.44f,  3.53f,  3.61f,  3.70f,  3.86f,  4.03f,  4.20f,
	 4.37f,  4.54f,  4.71f,  4.87f,  5.04f,  5.21f,  5.38f,  5.55f,
	 5.72f,  6.05f,  6.39f,  6.72f,  7.06f,  7.40f,  7.73f,  8.07f,
	 8.41f,  8.74f,  9.08f,  9.42f,  9.75f, 10.00f, 10.70f, 11.40f,
	12.10f, 12.70f, 13.40f, 14.10f, 14.80f, 15.40f, 16.10f, 16.80f,
	17.40f, 18.10f, 19.50f, 20.80f, 22.20f, 23.50f, 24.80f, 26.20f,
	27.50f, 28.90f, 30.20f, 31.60f, 32.90f, 34.30f, 37.00f, 39.70f
};

static
float getvtab1 ( unsigned short c )
{
	return ftab1[c];
}

static
unsigned short getutab1 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab1) - 1; c > 0; --c) {
		if (v >= ftab1[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#2 - Modulation Delay Offset (ms)

static
float ftab2[] =
{
	 0.0f,  0.1f,  0.2f,  0.3f,  0.4f,  0.5f,  0.6f,  0.7f,
	 0.8f,  0.9f,  1.0f,  1.1f,  1.2f,  1.3f,  1.4f,  1.5f,
	 1.6f,  1.7f,  1.8f,  1.9f,  2.0f,  2.1f,  2.2f,  2.3f,
	 2.4f,  2.5f,  2.6f,  2.7f,  2.8f,  2.9f,  3.0f,  3.1f,
	 3.2f,  3.3f,  3.4f,  3.5f,  3.6f,  3.7f,  3.8f,  3.9f,
	 4.0f,  4.1f,  4.2f,  4.3f,  4.4f,  4.5f,  4.6f,  4.7f,
	 4.8f,  4.9f,  5.0f,  5.1f,  5.2f,  5.3f,  5.4f,  5.5f,
	 5.6f,  5.7f,  5.8f,  5.9f,  6.0f,  6.1f,  6.2f,  6.3f,
	 6.4f,  6.5f,  6.6f,  6.7f,  6.8f,  6.9f,  7.0f,  7.1f,
	 7.2f,  7.3f,  7.4f,  7.5f,  7.6f,  7.7f,  7.8f,  7.9f,
	 8.0f,  8.1f,  8.2f,  8.3f,  8.4f,  8.5f,  8.6f,  8.7f,
	 8.8f,  8.9f,  9.0f,  9.1f,  9.2f,  9.3f,  9.4f,  9.5f,
	 9.6f,  9.7f,  9.8f,  9.9f, 10.0f, 11.1f, 12.2f, 13.3f,
	14.4f, 15.5f, 17.1f, 18.6f, 20.2f, 21.8f, 23.3f, 24.9f,
	26.5f, 28.0f, 29.6f, 31.2f, 32.8f, 34.3f, 35.9f, 37.5f,
	39.0f, 40.6f, 42.2f, 43.7f, 45.3f, 46.9f, 48.4f, 50.0f
};

static
float getvtab2 ( unsigned short c )
{
	return ftab2[c];
}

static
unsigned short getutab2 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab2) - 1; c > 0; --c) {
		if (v >= ftab2[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#3 - EQ Frequency (Hz)

static
float ftab3[] =
{
	   20.0f,   22.0f,   25.0f,   28.0f,   32.0f,   36.0f,
	   40.0f,   45.0f,   50.0f,   56.0f,   63.0f,   70.0f,
	   80.0f,   90.0f,  100.0f,  110.0f,  125.0f,  140.0f,
	  160.0f,  180.0f,  200.0f,  225.0f,  250.0f,  280.0f,
	  315.0f,  355.0f,  400.0f,  450.0f,  500.0f,  560.0f,
	  630.0f,  700.0f,  800.0f,  900.0f, 1000.0f, 1100.0f,
	 1200.0f, 1400.0f, 1600.0f, 1800.0f, 2000.0f, 2200.0f,
	 2500.0f, 2800.0f, 3200.0f, 3600.0f, 4000.0f, 4500.0f,
	 5000.0f, 5600.0f, 6300.0f, 7000.0f, 8000.0f, 9000.0f,
	10000.0f,11000.0f,12000.0f,14000.0f,16000.0f,18000.0f,
	20000.0f
};

static
float getvtab3 ( unsigned short c )
{
	return ftab3[c];
}

static
unsigned short getutab3 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab3) - 1; c > 0; --c) {
		if (v >= ftab3[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#4 - Reverb Time (ms)

static
float ftab4[] =
{
	  0.3f,  0.4f,  0.5f,  0.6f,  0.7f,  0.8f,  0.9f,  1.0f,
	  1.1f,  1.2f,  1.3f,  1.4f,  1.5f,  1.6f,  1.7f,  1.8f,
	  1.9f,  2.0f,  2.1f,  2.2f,  2.3f,  2.4f,  2.5f,  2.6f,
	  2.7f,  2.8f,  2.9f,  3.0f,  3.1f,  3.2f,  3.3f,  3.4f,
	  3.5f,  3.6f,  3.7f,  3.8f,  3.9f,  4.0f,  4.1f,  4.2f,
	  4.3f,  4.4f,  4.5f,  4.6f,  4.7f,  4.8f,  4.9f,  5.0f,
	  5.5f,  6.0f,  6.5f,  7.0f,  7.5f,  8.0f,  8.5f,  9.0f,
	  9.5f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f,
	 17.0f, 18.0f, 19.0f, 20.0f, 25.0f, 30.0f
};

static
float getvtab4 ( unsigned short c )
{
	return ftab4[c];
}

static
unsigned short getutab4 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab4) - 1; c > 0; --c) {
		if (v >= ftab4[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#5 - Delay Time (ms)

static
float ftab5[] =
{
	  0.1f,    1.7f,    3.2f,    4.8f,    6.4f,    8.0f,    9.5f,   11.1f,
	 12.7f,   14.3f,   15.8f,   17.4f,   19.0f,   20.6f,   22.1f,   23.7f,
	 25.3f,   26.9f,   28.4f,   30.0f,   31.6f,   33.2f,   34.7f,   36.3f,
	 37.9f,   39.5f,   41.0f,   42.6f,   44.2f,   45.7f,   47.3f,   48.9f,
	 50.5f,   52.0f,   53.6f,   55.2f,   56.8f,   58.3f,   59.9f,   61.5f,
	 63.1f,   64.6f,   66.2f,   67.8f,   69.4f,   70.9f,   72.5f,   74.1f,
	 75.7f,   77.2f,   78.8f,   80.4f,   81.9f,   83.5f,   85.1f,   86.7f,
	 88.2f,   89.8f,   91.4f,   93.0f,   94.5f,   96.1f,   97.7f,   99.3f,
	100.8f,  102.4f,  104.0f,  105.6f,  107.1f,  108.7f,  110.3f,  111.9f,
	113.4f,  115.0f,  116.6f,  118.2f,  119.7f,  121.3f,  122.9f,  124.4f,
	126.0f,  127.6f,  129.2f,  130.7f,  132.3f,  133.9f,  135.5f,  137.0f,
	138.6f,  140.2f,  141.8f,  143.3f,  144.9f,  146.5f,  148.1f,  149.6f,
	151.2f,  152.8f,  154.4f,  155.9f,  157.5f,  159.1f,  160.6f,  162.2f,
	163.8f,  165.4f,  166.9f,  168.5f,  170.1f,  171.7f,  173.2f,  174.8f,
	176.4f,  178.0f,  179.5f,  181.1f,  182.7f,  184.3f,  185.8f,  187.4f,
	189.0f,  190.6f,  192.1f,  193.7f,  195.3f,  196.9f,  198.4f,  200.0f
};

static
float getvtab5 ( unsigned short c )
{
	return ftab5[c];
}

static
unsigned short getutab5 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab5) - 1; c > 0; --c) {
		if (v >= ftab5[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#6 - Room Size (m)

static
float ftab6[] =
{
	0.1f,  0.3f,  0.4f,  0.6f,  0.7f,  0.9f,  1.0f,  1.2f,
	1.4f,  1.5f,  1.7f,  1.8f,  2.0f,  2.1f,  2.3f,  2.5f,
	2.6f,  2.8f,  2.9f,  3.1f,  3.2f,  3.4f,  3.5f,  3.7f,
	3.9f,  4.0f,  4.2f,  4.3f,  4.5f,  4.6f,  4.8f,  5.0f,
	5.1f,  5.3f,  5.4f,  5.6f,  5.7f,  5.9f,  6.1f,  6.2f,
	6.4f,  6.5f,  6.7f,  6.8f,  7.0f
};

static
float getvtab6 ( unsigned short c )
{
	return ftab6[c];
}

static
unsigned short getutab6 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab6) - 1; c > 0; --c) {
		if (v >= ftab6[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#7 - Delay Time (ms)

static
float ftab7[] =
{
	  0.1f,   3.2f,   6.4f,   9.5f,  12.7f,  15.8f,  19.0f,  22.1f,
	 25.3f,  28.4f,  31.6f,  34.7f,  37.9f,  41.0f,  44.2f,  47.3f,
	 50.5f,  53.6f,  56.8f,  59.9f,  63.1f,  66.2f,  69.4f,  72.5f,
	 75.7f,  78.8f,  82.0f,  85.1f,  88.3f,  91.4f,  94.6f,  97.7f,
	100.9f, 104.0f, 107.2f, 110.3f, 113.5f, 116.6f, 119.8f, 122.9f,
	126.1f, 129.2f, 132.4f, 135.5f, 138.6f, 141.8f, 144.9f, 148.1f,
	151.2f, 154.4f, 157.5f, 160.7f, 163.8f, 167.0f, 170.1f, 173.3f,
	176.4f, 179.6f, 182.7f, 185.9f, 189.0f, 192.2f, 195.3f, 198.5f,
	201.6f, 204.8f, 207.9f, 211.1f, 214.2f, 217.4f, 220.5f, 223.7f,
	226.8f, 230.0f, 233.1f, 236.3f, 239.4f, 242.6f, 245.7f, 248.9f,
	252.0f, 255.2f, 258.3f, 261.5f, 264.6f, 267.7f, 270.9f, 274.0f,
	277.2f, 280.3f, 283.5f, 286.6f, 289.8f, 292.9f, 296.1f, 299.2f,
	302.4f, 305.5f, 308.7f, 311.8f, 315.0f, 318.1f, 321.3f, 324.4f,
	327.6f, 330.7f, 333.9f, 337.0f, 340.2f, 343.3f, 346.5f, 349.6f,
	352.8f, 355.9f, 359.1f, 362.2f, 365.4f, 368.5f, 371.7f, 374.8f,
	378.0f, 381.1f, 384.3f, 387.4f, 390.6f, 393.7f, 396.9f, 400.0f
};


static
float getvtab7 ( unsigned short c )
{
	return ftab7[c];
}

static
unsigned short getutab7 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab7) - 1; c > 0; --c) {
		if (v >= ftab7[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
// Table#8 - Reverb Width; Depth; Height (m)


static
float ftab8[] =
{
	 0.5f,  0.8f,  1.0f,  1.3f,  1.5f,  1.8f,  2.0f,
	 2.3f,  2.6f,  2.8f,  3.1f,  3.3f,  3.6f,  3.9f,
	 4.1f,  4.4f,  4.6f,  4.9f,  5.2f,  5.4f,  5.7f,
	 5.9f,  6.2f,  6.5f,  6.7f,  7.0f,  7.2f,  7.5f,
	 7.8f,  8.0f,  8.3f,  8.6f,  8.8f,  9.1f,  9.4f,
	 9.6f,  9.9f, 10.2f, 10.4f, 10.7f, 11.0f, 11.2f,
	11.5f, 11.8f, 12.1f, 12.3f, 12.6f, 12.9f, 13.1f,
	13.4f, 13.7f, 14.0f, 14.2f, 14.5f, 14.8f, 15.1f,
	15.4f, 15.6f, 15.9f, 16.2f, 16.5f, 16.8f, 17.1f,
	17.3f, 17.6f, 17.9f, 18.2f, 18.5f, 18.8f, 19.1f,
	19.4f, 19.7f, 20.0f, 20.2f, 20.5f, 20.8f, 21.1f,
	21.4f, 21.7f, 22.0f, 22.4f, 22.7f, 23.0f, 23.3f,
	23.6f, 23.9f, 24.2f, 24.5f, 24.9f, 25.2f, 25.5f,
	25.8f, 26.1f, 26.5f, 26.8f, 27.1f, 27.5f, 27.8f,
	28.1f, 28.5f, 28.8f, 29.2f, 29.5f, 29.9f, 30.2f
};

static
float getvtab8 ( unsigned short c )
{
	return ftab8[c];
}

static
unsigned short getutab8 ( float v )
{
	unsigned short c;

	for (c = TSIZE(ftab8) - 1; c > 0; --c) {
		if (v >= ftab8[c])
			return c;
	}
	return 0;
}


//-------------------------------------------------------------------------
//

static
float getv0x40 ( unsigned short c )
{
	return (float) (c - 64);
}

static
unsigned short getu0x40 ( float v )
{
	return (unsigned short) (v + 64.0f);
}


//-------------------------------------------------------------------------
//

static
float getv0x80 ( unsigned short c )
{
	return (float) (c - 128);
}

static
unsigned short getu0x80 ( float v )
{
	return (unsigned short) (v + 128.0f);
}


//-------------------------------------------------------------------------
//

static
float getv_180 ( unsigned short c )
{
	return 3.0f * getv0x40(c);
}

static
unsigned short getu_180 ( float v )
{
	return getu0x40(v / 3.0f);
}


//-------------------------------------------------------------------------
//

static
float getv0_10 ( unsigned short c )
{
	return 0.1f * (float) (c);
}

static
unsigned short getu0_10 ( float v )
{
	return (unsigned short) (10.0f * v);
}


//-------------------------------------------------------------------------
//

static
float getv_100 ( unsigned short c )
{
	return getv0x40(c) / 0.63f;
}

static
unsigned short getu_100 ( float v )
{
	return getu0x40(v * 0.63f);
}


//-------------------------------------------------------------------------
//

static
float getv94_5 ( unsigned short c )
{
	return 94.5f * getv0x40(c) / 0.63f;
}

static
unsigned short getu94_5 ( float v )
{
	return getu0x40(0.63f * v / 94.5f);
}


//-------------------------------------------------------------------------
//

static
float getv12_7 ( unsigned short c )
{
	return 0.1f * getv0x80(c);
}

static
unsigned short getu12_7 ( float v )
{
	return getu0x80(10.0f * v);
}


//-------------------------------------------------------------------------
//

static
float getv1024 ( unsigned short c )
{
	return 0.1f * (float) (c - 1024);
}

static
unsigned short getu1024 ( float v )
{
	return (unsigned short) (1024.0f + 10.0f * v);
}


//-------------------------------------------------------------------------
//

static
float getvprog ( unsigned short c )
{
	return (float) (c + 1);
}

static
unsigned short getuprog ( float v )
{
	return (unsigned short) (v - 1.0f);
}


//-------------------------------------------------------------------------
//

static
const char *getsimod ( unsigned short c )
{
	static
	const char *tabimod[] = { "Mono", "Stereo" };
	return tabimod[c];
}


static
const char *getsisel ( unsigned short c )
{
	static
	const char *tabisel[] = { "L", "R", "L&R" };
	return tabisel[c];
}


static
const char *getsreft ( unsigned short c )
{
	static
	const char *tabreft[] = { "S-H", "L-H", "Rdm", "Rvs", "Plt", "Spr" };
	return tabreft[c];
}


static
const char *getsrevt ( unsigned short c )
{
	static
	const char *tabrevt[] = { "Type A", "Type B" };
	return tabrevt[c];
}


static
const char *getspand ( unsigned short c )
{
	static
	const char *tabpand[] = { "L<->R", "L->R", "L<-R", "Lturn", "Rturn", "L/R" };
	return tabpand[c];
}


static
const char *getsampt ( unsigned short c )
{
	static
	const char *tabampt[] = { "Off", "Stack", "Combo", "Tube" };
	return tabampt[c];
}


static
const char *getsonff ( unsigned short c )
{
	static
	const char *tabonff[] = { "Off", "On" };
	return tabonff[c];
}


static
const char *getskeya ( unsigned short c )
{
	static
	const char *tabkeya[] = { "Single", "Multi", "Instr" };
	return tabkeya[c];
}


static
const char *getsmmod ( unsigned short c )
{
	static
	const char *tabmmod[] = { "Mono", "Poly" };
	return tabmmod[c];
}


static
const char *getspmod ( unsigned short c )
{
	static
	const char *tabpmod[] = { "Normal", "Drum", "Drum 1", "Drum 2" };
	return tabpmod[c];
}

static
const char *getsconn ( unsigned short c )
{
	static
	const char *tabconn[] = { "Insert", "System" };
	return tabconn[c];
}

static
const char *getslfow ( unsigned short c )
{
	static
	const char *tablfow[] = { "Saw", "Tri", "S&H" };
	return tablfow[c];
}

static
const char *getspscl ( unsigned short c )
{
	static
	const char *tabpscl[] = { "100%", "50%", "20%", "10%", "5%", "0%" };
	return tabpscl[c];
}

static
const char *getspdph ( unsigned short c )
{
	static
	const char *tabpdph[] = { "1/2oct", "1oct", "2oct", "4oct" };
	return tabpdph[c];
}

static
const char *getselem ( unsigned short c )
{
	static
	const char *tabelem[] = { "1", "2", "1+2" };
	return tabelem[c - 1];
}

static
const char *getschan ( unsigned short c )
{
	if (c < 16) {
		static char chan[4];
		snprintf(chan, sizeof(chan), "%u", c + 1);
		return chan;
	}
	else if (c == 127)
		return "Off";

	return nullptr;
}

static
const char *getsvelc ( unsigned short c )
{
	static
	const char *tabvelc[] = { "Linear", "Exp" };
	return tabvelc[c];
}

static
const char *getsvpan ( unsigned short c )
{
	if (c < 15) {
		static char vpan[3];
		snprintf(vpan, sizeof(vpan), "%2d", int(c) - 7);
		return vpan;
	}
	else if (c == 15)
		return "Scale";

	return nullptr;
}

// static
const char *getsnote ( unsigned short c )
{
	static
	const char *tabnote[] =
		{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	static char note[8];
	snprintf(note, sizeof(note), "%-2s%2d", tabnote[c % 12], (c / 12) - 1);
	return note;
}


//---------------------------------------------------------------------
// QS300 Wave List.
//

const char *getswave ( unsigned short c )
{
	static
	struct _QS300WaveItem
	{
		unsigned short     id;
		const char        *name;
		unsigned short     no;

	} wavetab[] = {

		// [Piano & Keyboard]
		{   0, "Grand",        37 }, // Ap
		{   1, "Brite",        38 }, // Ap
		{   2, "Honky",        39 }, // Ap
		{   3, "Roads",        56 }, // Ep
		{   4, "CP80",        172 }, // Ep
		{   5, "DX7",          55 }, // Ep
		{   6, "Accordion",    79 }, // Mk
		{   7, "Bandoneon",   241 }, // Mk
		{   8, "Celesta",      80 }, // Mk
		{   9, "Clavi.",       81 }, // Mk
		{  10, "Harpsichrd",   82 }, // Mk
		// [Chromatic & Organ]
		{  11, "Glocken",     105 }, // Cp
		{  12, "Marimba",     106 }, // Cp
		{  13, "SteelDrum",   107 }, // Cp
		{  14, "Timpani",     108 }, // Cp
		{  15, "TinkleBell",  109 }, // Cp
		{  16, "TabularBell", 110 }, // Cp
		{  17, "Vibes",       111 }, // Cp
		{  18, "Xylophone",   112 }, // Cp
		{  19, "Pipe",         86 }, // Ao
		{  20, "Reed",         87 }, // Ao
		{  21, "Drawbar 1",    83 }, // Eo
		{  22, "Drawbar 2",   240 }, // Eo
		{  23, "Percussive",   84 }, // Eo
		{  24, "Rock 1",       85 }, // Eo
		{  25, "Rock 2",      237 }, // Eo
		// [Guitar & Bass]
		{  26, "Nylon",        71 }, // Ag
		{  27, "Steel",        72 }, // Ag
		{  28, "12GtrUpper",  190 }, // Ag
		{  29, "Jazz Gtr",     73 }, // Eg
		{  30, "Clean",        74 }, // Eg
		{  31, "Muted",        75 }, // Eg
		{  32, "Overdrive",    76 }, // Eg
		{  33, "OverdriveLp", 183 }, // Eg
		{  34, "Distortion",   77 }, // Eg
		{  35, "Harmonics1",   78 }, // Eg
		{  36, "Harmonics2",  242 }, // Eg
		{  37, "Upright",      46 }, // Ab
		{  38, "Finger",       40 }, // Eb
		{  39, "Fretless",     41 }, // Eb
		{  40, "Picked",       42 }, // Eb
		{  41, "Slap 1",       43 }, // Eb
		{  42, "Slap 2",       44 }, // Eb
		{  43, "Thump",        45 }, // Eb
		{  44, "Bass 1",       97 }, // Mb
		{  45, "Bass 2",       98 }, // Mb
		// [Strings & Ensemble]
		{  46, "Violin",       88 }, // Bw
		{  47, "Viola",        89 }, // Bw
		{  48, "Cello",        90 }, // Bw
		{  49, "ContraBass",   91 }, // Bw
		{  50, "SectionEns",   92 }, // Oe
		{  51, "Pizzicato",    93 }, // Oe
		{  52, "StrngEnsLp",  180 }, // Oe
		{  53, "Ensemble",    231 }, // Oe
		{  54, "Aah",          53 }, // Ch
		{  55, "Ooh",          54 }, // Ch
		// [Brass & Reed]
		{  56, "Trumpet",      47 }, // Ar
		{  57, "MutedTrp",     48 }, // Ar
		{  58, "Trombone",     49 }, // Ar
		{  59, "Tuba",         50 }, // Ar
		{  60, "FrenchHorn",   51 }, // Ar
		{  61, "TrumpetEns",   52 }, // Ar
		{  62, "Brass",        99 }, // Sr
		{  63, "Bassoon",     113 }, // Rd
		{  64, "Clarinet",    114 }, // Rd
		{  65, "EnglshHorn",  115 }, // Rd
		{  66, "Oboe",        117 }, // Rd
		{  67, "SopranoSax",  120 }, // Rd
		{  68, "SprSaxAtk",   176 }, // Rd
		{  69, "AltoSax",     121 }, // Rd
		{  70, "AltoSaxAtk",  177 }, // Rd
		{  71, "TenorSax",    184 }, // Rd
		{  72, "BaritonSax",  123 }, // Rd
		{  73, "BariSaxAtk",  179 }, // Rd
		{  74, "BariSaxLp",   182 }, // Rd
		{  75, "Flute",       116 }, // Rd
		{  76, "Piccolo",     118 }, // Pi
		{  77, "SprRecordr",  119 }, // Pi
		// [Ethnic]
		{  78, "Banjo",        57 }, // Et
		{  79, "BagPipe",      58 }, // Et
		{  80, "Dulcimer",     59 }, // Et
		{  81, "Harmonica",    60 }, // Et
		{  82, "Harp",         61 }, // Et
		{  83, "Kalimba",      62 }, // Et
		{  84, "Koto",         63 }, // Et
		{  85, "Ocarina",      64 }, // Et
		{  86, "Shakuhachi",   66 }, // Et
		{  87, "Shamisen",     67 }, // Et
		{  88, "Sho",          69 }, // Et
		{  89, "Sitar",        70 }, // Et
		{  90, "Shanai",      188 }, // Et
		{  91, "PanFlute",    243 }, // Et
		// [Percusssive]
		{  92, "SideStick",     4 }, // Pc
		{  93, "Snare 1",       5 }, // Pc
		{  94, "Snare 2",       6 }, // Pc
		{  95, "Snare 3",       7 }, // Pc
		{  96, "Snare 4",       8 }, // Pc
		{  97, "Snare 5",       9 }, // Pc
		{  98, "Snare 6",      10 }, // Pc
		{  99, "SnareBrush",   11 }, // Pc
		{ 100, "Tom 1",        12 }, // Pc
		{ 101, "Tom 2",        13 }, // Pc
		{ 102, "Tom 3",        14 }, // Pc
		{ 103, "Tom 4",        15 }, // Pc
		{ 104, "Tom 5",        16 }, // Pc
		{ 105, "Tom 6",        17 }, // Pc
		{ 106, "Kick 1",       18 }, // Pc
		{ 107, "Kick 2",       19 }, // Pc
		{ 108, "Kick 3",       20 }, // Pc
		{ 109, "Kick 4",       21 }, // Pc
		{ 110, "GranCassa",    22 }, // Pc
		{ 111, "Stick",        23 }, // Pc
		{ 112, "Cymbal 1",     24 }, // Pc
		{ 113, "Cymbal 2",     25 }, // Pc
		{ 114, "Cymbal 3",     26 }, // Pc
		{ 115, "Cymbal 4",     27 }, // Pc
		{ 116, "LoopCymbal",   29 }, // Pc
		{ 117, "Agogo",        32 }, // Pc
		{ 118, "Castanet",     33 }, // Pc
		{ 119, "WoodBlock",    34 }, // Pc
		{ 120, "Taiko",        35 }, // Pc
		{ 121, "Triangle",     36 }, // Pc
		// [SFX]
		{ 122, "BassSlap",    128 }, // Me
		{ 123, "GtrStroke",   138 }, // Me
		{ 124, "GtrFretNz",   139 }, // Me
		{ 125, "KeyPad",      143 }, // Me
		{ 126, "Metronome",   146 }, // Me
		{ 127, "OrchHit",     148 }, // Me
		{ 128, "WindChime",   163 }, // Me
		{ 129, "TublrBell.p", 189 }, // Me
		{ 130, "XylophonLp",  191 }, // Me
		{ 131, "Scratch",     245 }, // Me
		{ 132, "Applause",    124 }, // Ne
		{ 133, "Bird 1",      125 }, // Ne
		{ 134, "Bird 2",      126 }, // Ne
		{ 135, "Breath",      127 }, // Ne
		{ 136, "Bubble",      129 }, // Ne
		{ 137, "CarPass",     130 }, // Ne
		{ 138, "CarCrash",    131 }, // Ne
		{ 139, "CarStart",    132 }, // Ne
		{ 140, "Dog",         133 }, // Ne
		{ 141, "DoorSlam",    134 }, // Ne
		{ 142, "DoorSqueak",  135 }, // Ne
		{ 142, "FootStep",    136 }, // Ne
		{ 144, "Gallop",      137 }, // Ne
		{ 145, "Gun",         140 }, // Ne
		{ 146, "Helicopter",  141 }, // Ne
		{ 147, "HeartBeat",   142 }, // Ne
		{ 148, "Laughing",    144 }, // Ne
		{ 149, "MachineGun",  145 }, // Ne
		{ 150, "Punch",       149 }, // Ne
		{ 151, "Rain",        150 }, // Ne
		{ 152, "Scream",      151 }, // Ne
		{ 153, "Stream",      152 }, // Ne
		{ 154, "Surf",        153 }, // Ne
		{ 155, "TelDial",     154 }, // Ne
		{ 156, "TelRing 1",   155 }, // Ne
		{ 157, "TelRing 2",   162 }, // Ne
		{ 158, "Thunder",     156 }, // Ne
		{ 159, "TireSkid",    157 }, // Ne
		{ 160, "Train",       158 }, // Ne
		{ 161, "Wind",        159 }, // Ne
		// [One-cycle Wave]
		{ 162, "Square",       94 }, // Ow
		{ 163, "Saw",          95 }, // Ow
		{ 164, "Sine",        195 }, // Ow
		{ 165, "Digi 1",      196 }, // Ow
		{ 166, "Digi 2",      197 }, // Ow
		{ 167, "Digi 3",      198 }, // Ow
		{ 168, "Digi 4",      199 }, // Ow
		{ 169, "Digi 5",      200 }, // Ow
		{ 170, "Digi 6",      201 }, // Ow
		{ 171, "Digi 7",      202 }, // Ow
		{ 172, "Digi 8",      203 }, // Ow
		{ 173, "Digi 9",      204 }, // Ow
		{ 174, "Digi 10",     205 }, // Ow
		{ 175, "Digi 11",     206 }, // Ow
		{ 176, "Digi 12",     207 }, // Ow
		{ 177, "Digi 13",     208 }, // Ow
		{ 178, "Digi 14",     209 }, // Ow
		{ 179, "Digi 15",     210 }, // Ow
		{ 180, "Digi 16",     211 }, // Ow
		{ 181, "Digi 17",     212 }, // Ow
		{ 182, "Digi 19",     213 }, // Ow
		{ 183, "Digi 20",     214 }, // Ow
		{ 184, "Digi 21",     215 }, // Ow
		{ 185, "Digi 22",     216 }, // Ow
		{ 186, "Digi 23",     217 }, // Ow
		{ 187, "Digi 24",     218 }, // Ow
		{ 188, "Digi 25",     219 }, // Ow
		{ 189, "Digi 26",     220 }, // Ow
		{ 190, "Digi 27",     221 }, // Ow
		{ 191, "Digi 28",     222 }, // Ow
		{ 192, "Digi 29",     223 }, // Ow
		{ 193, "Digi 30",     224 }, // Ow
		{ 194, "Digi 31",     225 }, // Ow
		{ 195, "Digi 32",     226 }, // Ow
		{ 196, "Pulse 10",    228 }, // Ow
		{ 197, "Pulse 25",    229 }, // Ow
		// [Loop & Misc]
		{ 198, "Pad 1",       102 }, // Lw
		{ 199, "Pad 2",       227 }, // Lw
		{ 200, "Pad 3",       230 }, // Lw
		{ 201, "Itopia",      236 }, // Lw
		{ 202, "Mallet",      104 }, // Mw
		{ 203, "Noise",       147 }, // Mw
		{ 204, "Stndrd Kit",    1 }  // Dr
	};

	static QHash<unsigned short, const char *> waves;

	if (waves.isEmpty()) {
		for (unsigned short i = 0; i < TSIZE(wavetab); ++i)
			waves.insert(wavetab[i].no, wavetab[i].name);
	}

	if (waves.contains(c))
		return waves.value(c);

	return nullptr;
}


//-------------------------------------------------------------------------
//

static
const char *unit_Hz (void)
{
	return "Hz";
}

static
const char *unit_ms (void)
{
	return "ms";
}

static
const char *unit_dB (void)
{
	return "dB";
}

static
const char *unit_deg (void)
{
	return "deg";
}

static
const char *unit_m (void)
{
	return "m";
}

static
const char *unit_cen (void)
{
	return "cents";
}

static
const char *unit_sem (void)
{
	return "semitones";
}

static
const char *unit_pct (void)
{
	return "%";
}


//---------------------------------------------------------------------
// XG Normal Voice List
//

typedef
struct _XGNormalVoiceItem
{
	unsigned short     bank;
	unsigned short     prog;
	const char        *name;
	unsigned short     elem;

} XGNormalVoiceItem;


// Piano
static
XGNormalVoiceItem PianoTab[] =
{	// bank prog  name      elem
	{    0,   1, "GrandPno",   1 },
	{    1,   1, "GrndPnoK",   1 },
	{   18,   1, "MelloGrP",   1 },
	{   40,   1, "PianoStr",   2 },
	{   41,   1, "Dream",      2 },
	{    0,   2, "BritePno",   1 },
	{    1,   2, "BritPnoK",   1 },
	{    0,   3, "E.Grand",    2 },
	{    1,   3, "ElGrPnoK",   2 },
	{   32,   3, "Det.CP80",   2 },
	{   40,   3, "ElGrPno1",   2 },
	{   41,   3, "ElGrPno2",   2 },
	{    0,   4, "HnkyTonk",   2 },
	{    1,   4, "HnkyTnkK",   2 },
	{    5,   4, "0 E.Piano1", 2 },
	{    1,   4, "El.Pno1K",   1 },
	{   18,   4, "MelloEP1",   2 },
	{   32,   4, "Chor.EP1",   2 },
	{   40,   4, "HardEl.P",   2 },
	{   45,   4, "VX El.P1",   2 },
	{   64,   4, "60sEl.P",    1 },
	{    0,   6, "E.Piano2",   2 },
	{    1,   6, "El.Pno2K",   1 },
	{   32,   6, "Chor.EP2",   2 },
	{   33,   6, "DX Hard",    2 },
	{   34,   6, "DXLegend",   2 },
	{   40,   6, "DX Phase",   2 },
	{   41,   6, "DX+Analg",   2 },
	{   42,   6, "DXKotoEP",   2 },
	{   45,   6, "VX El.P2",   2 },
	{    0,   7, "Harpsi.",    1 },
	{    1,   7, "Harpsi.K",   1 },
	{   25,   7, "Harpsi.2",   2 },
	{   35,   7, "Harpsi.3",   2 },
	{    0,   8, "Clavi.",     2 },
	{    1,   8, "Clavi. K",   1 },
	{   27,   8, "ClaviWah",   2 },
	{   64,   8, "PulseClv",   1 },
	{   65,   8, "PierceCl",   2 }
};

// Chromatic Percussion
static
XGNormalVoiceItem ChromaticPercussionTab[] =
{	// bank prog  name      elem
	{    0,   9, "Celesta",    1 },
	{    0,  10, "Glocken",    1 },
	{    0,  11, "MusicBox",   2 },
	{   64,  11, "Orgel",      2 },
	{    0,  12, "Vibes",      1 },
	{    1,  12, "VibesK",     1 },
	{   45,  12, "HardVibe",   2 },
	{    0,  13, "Marimba",    1 },
	{    1,  13, "MarimbaK",   1 },
	{   64,  13, "SineMrmb",   2 },
	{   97,  13, "Balafon2",   2 },
	{   98,  13, "Log Drum",   2 },
	{    0,  14, "Xylophon",   1 },
	{    0,  15, "TubulBel",   1 },
	{   96,  15, "ChrchBel",   2 },
	{   97,  15, "Carillon",   2 },
	{    0,  16, "Dulcimer",   1 },
	{   35,  16, "Dulcimr2",   2 },
	{   96,  16, "Cimbalom",   2 },
	{   97,  16, "Santur",     2 }
};

// Organ
static
XGNormalVoiceItem OrganTab[] =
{	// bank prog  name      elem
	{    0,  17, "DrawOrgn",   1 },
	{   32,  17, "DetDrwOr",   2 },
	{   33,  17, "60sDrOr1",   2 },
	{   34,  17, "60sDrOr2",   2 },
	{   35,  17, "70sDrOr1",   2 },
	{   36,  17, "DrawOrg2",   2 },
	{   37,  17, "60sDrOr3",   2 },
	{   38,  17, "EvenBar",    2 },
	{   40,  17, "16+2\"2/3",  2 },
	{   64,  17, "Organ Ba",   1 },
	{   65,  17, "70sDrOr2",   2 },
	{   66,  17, "CheezOrg",   2 },
	{   67,  17, "DrawOrg3",   2 },
	{    0,  18, "PercOrgn",   1 },
	{   24,  18, "70sPcOr1",   2 },
	{   32,  18, "DetPrcOr",   2 },
	{   33,  18, "LiteOrg",    2 },
	{   37,  18, "PercOrg2",   2 },
	{    0,  19, "RockOrgn",   2 },
	{   64,  19, "RotaryOr",   2 },
	{   65,  19, "SloRotar",   2 },
	{   66,  19, "FstRotar",   2 },
	{    0,  20, "ChrchOrg",   2 },
	{   32,  20, "ChurOrg3",   2 },
	{   35,  20, "ChurOrg2",   2 },
	{   40,  20, "NotreDam",   2 },
	{   64,  20, "OrgFlute",   2 },
	{   65,  20, "TrmOrgFl",   2 },
	{    0,  21, "ReedOrgn",   1 },
	{   40,  21, "Puff Org",   2 },
	{    0,  22, "Acordion",   2 },
	{   32,  22, "AccordIt",   2 },
	{    0,  23, "Harmnica",   1 },
	{   32,  23, "Harmo 2",    2 },
	{    0,  24, "TangoAcd",   2 },
	{   64,  24, "TngoAcd2",   2 }
};

// Guitar
static
XGNormalVoiceItem GuitarTab[] =
{	// bank prog  name      elem
	{    0,  25, "NylonGtr",   1 },
	{   16,  25, "NylonGt2",   1 },
	{   25,  25, "NylonGt3",   2 },
	{   43,  25, "VelGtHrm",   2 },
	{   96,  25, "Ukulele",    1 },
	{    0,  26, "SteelGtr",   1 },
	{   16,  26, "SteelGt2",   1 },
	{   35,  26, "12StrGtr",   2 },
	{   40,  26, "Nyln&Stl",   2 },
	{   41,  26, "Stl&Body",   2 },
	{   96,  26, "Mandolin",   2 },
	{    0,  27, "Jazz Gtr",   1 },
	{   18,  27, "MelloGtr",   1 },
	{   32,  27, "JazzAmp",    2 },
	{    0,  28, "CleanGtr",   1 },
	{   32,  28, "ChorusGt",   2 },
	{    0,  29, "Mute.Gtr",   1 },
	{   40,  29, "FunkGtr1",   2 },
	{   41,  29, "MuteStlG",   2 },
	{   43,  29, "FunkGtr2",   2 },
	{   45,  29, "Jazz Man",   1 },
	{    0,  30, "Ovrdrive",   1 },
	{   43,  30, "Gt.Pinch",   2 },
	{    0,  31, "Dist.Gtr",   1 },
	{   40,  31, "FeedbkGt",   2 },
	{   41,  31, "FeedbGt2",   2 },
	{    0,  32, "GtrHarmo",   1 },
	{   65,  32, "GtFeedbk",   1 },
	{   66,  32, "GtrHrmo2",   1 }
};

// Bass
static
XGNormalVoiceItem BassTab[] =
{	// bank prog  name      elem
	{    0,  33, "Aco.Bass",   1 },
	{   40,  33, "JazzRthm",   2 },
	{   45,  33, "VXUprght",   2 },
	{    0,  34, "FngrBass",   1 },
	{   18,  34, "FingrDrk",   2 },
	{   27,  34, "FlangeBa",   2 },
	{   40,  34, "Ba&DstEG",   2 },
	{   43,  34, "FngrSlap",   2 },
	{   45,  34, "FngBass2",   2 },
	{   65,  34, "ModAlem",    2 },
	{    0,  35, "PickBass",   1 },
	{   28,  35, "MutePkBa",   1 },
	{    0,  36, "Fretless",   1 },
	{   32,  36, "Fretles2",   2 },
	{   33,  36, "Fretles3",   2 },
	{   34,  36, "Fretles4",   2 },
	{   96,  36, "SynFretl",   2 },
	{   97,  36, "Smooth",     2 },
	{    0,  37, "SlapBas1",   1 },
	{   27,  37, "ResoSlap",   1 },
	{   32,  37, "PunchThm",   2 },
	{    0,  38, "SlapBas2",   1 },
	{   43,  38, "VeloSlap",   2 },
	{    0,  39, "SynBass1",   1 },
	{   18,  39, "SynBa1Dk",   1 },
	{   20,  39, "FastResB",   1 },
	{   24,  39, "AcidBass",   1 },
	{   35,  39, "Clv Bass",   2 },
	{   40,  39, "TeknoBa",    2 },
	{   64,  39, "Oscar",      2 },
	{   65,  39, "SqrBass",    1 },
	{   66,  39, "RubberBa",   2 },
	{   96,  39, "Hammer",     2 },
	{    0,  40, "SynBass2",   2 },
	{    6,  40, "MelloSB1",   1 },
	{   12,  40, "Seq Bass",   2 },
	{   18,  40, "ClkSynBa",   2 },
	{   19,  40, "SynBa2Dk",   1 },
	{   32,  40, "SmthBa 2",   2 },
	{   40,  40, "ModulrBa",   2 },
	{   41,  40, "DX Bass",    2 },
	{   64,  40, "X WireBa",   2 }
};

// Strings
static
XGNormalVoiceItem StringsTab[] =
{	// bank prog  name      elem
	{    0,  41, "Violin",     1 },
	{    8,  41, "SlowVln",    1 },
	{    0,  42, "Viola",      1 },
	{    0,  43, "Cello",      1 },
	{    0,  44, "Contrabs",   1 },
	{    0,  45, "Trem.Str",   1 },
	{    8,  45, "SlowTrStr",  1 },
	{   40,  45, "Susp Str",   2 },
	{    0,  46, "Pizz.Str",   1 },
	{    0,  47, "Harp",       1 },
	{   40,  47, "YangChin",   2 },
	{    0,  48, "Timpani",    1 }
};

// Ensemble
static
XGNormalVoiceItem EnsembleTab[] =
{	// bank prog  name      elem
	{    0,  49, "Strings1",   1 },
	{    3,  49, "S.Strngs",   2 },
	{    8,  49, "SlowStr",    1 },
	{   24,  49, "ArcoStr",    2 },
	{   35,  49, "60sStrng",   2 },
	{   40,  49, "Orchestr",   2 },
	{   41,  49, "Orchstr2",   2 },
	{   42,  49, "TremOrch",   2 },
	{   45,  49, "VeloStr",    2 },
	{    0,  50, "Strings2",   1 },
	{    3,  50, "S.SlwStr",   2 },
	{    8,  50, "LegatoSt",   2 },
	{   40,  50, "Warm Str",   2 },
	{   41,  50, "Kingdom",    2 },
	{   64,  50, "70s Str",    1 },
	{   65,  50, "Str Ens3",   1 },
	{    0,  51, "Syn.Str1",   2 },
	{   27,  51, "ResoStr",    2 },
	{   64,  51, "Syn Str4",   2 },
	{   65,  51, "SS Str",     2 },
	{    0,  52, "Syn.Str2",   2 },
	{    0,  53, "ChoirAah",   1 },
	{    3,  53, "S.Choir",    2 },
	{   16,  53, "Ch.Aahs2",   2 },
	{   32,  53, "MelChoir",   2 },
	{   40,  53, "ChoirStr",   2 },
	{    0,  54, "VoiceOoh",   1 },
	{    0,  55, "SynVoice",   1 },
	{   40,  55, "SynVox2",    2 },
	{   41,  55, "Choral",     2 },
	{   64,  55, "AnaVoice",   1 },
	{    0,  56, "Orch.Hit",   2 },
	{   35,  56, "OrchHit2",   2 },
	{   64,  56, "Impact",     2 }
};

// Brass
static
XGNormalVoiceItem BrassTab[] =
{	// bank prog  name      elem
	{    0,  57, "Trumpet",    1 },
	{   16,  57, "Trumpet2",   1 },
	{   17,  57, "BriteTrp",   2 },
	{   32,  57, "WarmTrp",    2 },
	{    0,  58, "Trombone",   1 },
	{   18,  58, "Trmbone2",   2 },
	{    0,  59, "Tuba",       1 },
	{   16,  59, "Tuba 2",     1 },
	{    0,  60, "Mute.Trp",   1 },
	{    0,  61, "Fr.Horn",    2 },
	{    6,  61, "FrHrSolo",   2 },
	{   32,  61, "FrHorn2",    1 },
	{   37,  61, "HornOrch",   2 },
	{    0,  62, "BrasSect",   1 },
	{   35,  62, "Tp&TbSec",   2 },
	{   40,  62, "BrssSec2",   2 },
	{   41,  62, "HiBrass",    2 },
	{   42,  62, "MelloBrs",   2 },
	{    0,  63, "SynBras1",   2 },
	{   12,  63, "QuackBr",    2 },
	{   20,  63, "RezSynBr",   2 },
	{   24,  63, "PolyBrss",   2 },
	{   27,  63, "SynBras3",   2 },
	{   32,  63, "JumpBrss",   2 },
	{   45,  63, "AnaVelBr",   2 },
	{   64,  63, "AnaBrss1",   2 },
	{    0,  64, "SynBras2",   1 },
	{   18,  64, "Soft Brs",   2 },
	{   40,  64, "SynBras4",   2 },
	{   41,  64, "ChorBrss",   2 },
	{   45,  64, "VelBras2",   2 },
	{   64,  64, "AnaBras2",   2 }
};

// Reed
static
XGNormalVoiceItem ReedTab[] =
{	// bank prog  name      elem
	{    0,  65, "SprnoSax",   1 },
	{    0,  66, "Alto Sax",   1 },
	{   40,  66, "Sax Sect",   2 },
	{   43,  66, "HyprAlto",   2 },
	{    0,  67, "TenorSax",   1 },
	{   40,  67, "BrthTnSx",   2 },
	{   41,  67, "SoftTenr",   2 },
	{   64,  67, "TnrSax 2",   1 },
	{    0,  68, "Bari.Sax",   1 },
	{    0,  69, "Oboe",       2 },
	{    0,  70, "Eng.Horn",   1 },
	{    0,  71, "Bassoon",    1 },
	{    0,  72, "Clarinet",   1 }
};

// Pipe
static
XGNormalVoiceItem PipeTab[] = 
{	// bank prog  name      elem
	{    0,  73, "Piccolo",    1 },
	{    0,  74, "Flute",      1 },
	{    0,  75, "Recorder",   1 },
	{    0,  76, "PanFlute",   1 },
	{    0,  77, "Bottle",     2 },
	{    0,  78, "Shakhchi",   2 },
	{    0,  79, "Whistle",    1 },
	{    0,  80, "Ocarina",    1 }
};

// Synth Lead
static
XGNormalVoiceItem SynthLeadTab[] =
{	// bank prog  name      elem
	{    0,  81, "SquareLd",   2 },
	{    6,  81, "Square 2",   1 },
	{    8,  81, "LMSquare",   2 },
	{   18,  81, "Hollow",     1 },
	{   19,  81, "Shmoog",     2 },
	{   64,  81, "Mellow",     2 },
	{   65,  81, "SoloSine",   2 },
	{   66,  81, "SineLead",   1 },
	{    0,  82, "Saw.Lead",   2 },
	{    6,  82, "Saw 2",      1 },
	{    8,  82, "ThickSaw",   2 },
	{   18,  82, "DynaSaw",    1 },
	{   19,  82, "DigiSaw",    2 },
	{   20,  82, "Big Lead",   2 },
	{   24,  82, "HeavySyn",   2 },
	{   25,  82, "WaspySyn",   2 },
	{   40,  82, "PulseSaw",   2 },
	{   41,  82, "Dr. Lead",   2 },
	{   45,  82, "VeloLead",   2 },
	{   96,  82, "Seq Ana",    2 },
	{    0,  83, "CaliopLd",   2 },
	{   65,  83, "Pure Pad",   2 },
	{    0,  84, "Chiff Ld",   2 },
	{   64,  84, "Rubby",      2 },
	{    0,  85, "CharanLd",   2 },
	{   64,  85, "DistLead",   2 },
	{   65,  85, "WireLead",   2 },
	{    0,  86, "Voice Ld",   2 },
	{   24,  86, "SynthAah",   2 },
	{   64,  86, "VoxLead",    2 },
	{    0,  87, "Fifth Ld",   2 },
	{   35,  87, "Big Five",   2 },
	{    0,  88, "Bass &Ld",   2 },
	{   16,  88, "Big&Low",    2 },
	{   64,  88, "Fat&Prky",   2 },
	{   65,  88, "SoftWurl",   2 }
};

// Synth Pad
static
XGNormalVoiceItem SynthPadTab[] =
{	// bank prog  name      elem
	{    0,  89, "NewAgePd",   2 },
	{   64,  89, "Fantasy2",   2 },
	{    0,  90, "Warm Pad",   2 },
	{   16,  90, "ThickPad",   2 },
	{   17,  90, "Soft Pad",   2 },
	{   18,  90, "SinePad",    2 },
	{   64,  90, "Horn Pad",   2 },
	{   65,  90, "RotarStr",   2 },
	{    0,  91, "PolySyPd",   2 },
	{   64,  91, "PolyPd80",   2 },
	{   65,  91, "ClickPad",   2 },
	{   66,  91, "Ana Pad",    2 },
	{   67,  91, "SquarPad",   2 },
	{    0,  92, "ChoirPad",   2 },
	{   64,  92, "Heaven2",    2 },
	{   66,  92, "Itopia",     2 },
	{   67,  92, "CC Pad",     2 },
	{    0,  93, "BowedPad",   2 },
	{   64,  93, "Glacier",    2 },
	{   65,  93, "GlassPad",   2 },
	{    0,  94, "MetalPad",   2 },
	{   64,  94, "Tine Pad",   2 },
	{   65,  94, "Pan Pad",    2 },
	{    0,  95, "Halo Pad",   2 },
	{    0,  96, "SweepPad",   2 },
	{   20,  96, "Shwimmer",   2 },
	{   27,  96, "Converge",   2 },
	{   64,  96, "PolarPad",   2 },
	{   66,  96, "Celstial",   2 }
};

// Synth Effects
static
XGNormalVoiceItem SynthEffectsTab[] =
{	// bank prog  name      elem
	{    0,  97, "Rain",       2 },
	{   45,  97, "ClaviPad",   2 },
	{   64,  97, "HrmoRain",   2 },
	{   65,  97, "AfrcnWnd",   2 },
	{   66,  97, "Caribean",   2 },
	{    0,  98, "SoundTrk",   2 },
	{   27,  98, "Prologue",   2 },
	{   64,  98, "Ancestrl",   2 },
	{    0,  99, "Crystal",    2 },
	{   12,  99, "SynDrCmp",   2 },
	{   14,  99, "Popcorn",    2 },
	{   18,  99, "TinyBell",   2 },
	{   35,  99, "RndGlock",   2 },
	{   40,  99, "GlockChi",   2 },
	{   41,  99, "ClearBel",   2 },
	{   42,  99, "ChorBell",   2 },
	{   64,  99, "SynMalet",   1 },
	{   65,  99, "SftCryst",   2 },
	{   66,  99, "LoudGlok",   2 },
	{   67,  99, "XmasBell",   2 },
	{   68,  99, "VibeBell",   2 },
	{   69,  99, "DigiBell",   2 },
	{   70,  99, "AirBells",   2 },
	{   71,  99, "BellHarp",   2 },
	{   72,  99, "Gamelmba",   2 },
	{    0, 100, "Atmosphr",   2 },
	{   18, 100, "WarmAtms",   2 },
	{   19, 100, "HollwRls",   2 },
	{   40, 100, "NylonEP",    2 },
	{   64, 100, "NylnHarp",   2 },
	{   65, 100, "Harp Vox",   2 },
	{   66, 100, "AtmosPad",   2 },
	{   67, 100, "Planet",     2 },
	{    0, 101, "Bright",     2 },
	{   64, 101, "FantaBel",   2 },
	{   96, 101, "Smokey",     2 },
	{    0, 102, "Goblins",    2 },
	{   64, 102, "GobSyn",     2 },
	{   65, 102, "50sSciFi",   2 },
	{   66, 102, "Ring Pad",   2 },
	{   67, 102, "Ritual",     2 },
	{   68, 102, "ToHeaven",   2 },
	{   70, 102, "Night",      2 },
	{   71, 102, "Glisten",    2 },
	{   96, 102, "BelChoir",   2 },
	{    0, 103, "Echoes",     2 },
	{    8, 103, "EchoPad2",   2 },
	{   14, 103, "Echo Pan",   2 },
	{   64, 103, "EchoBell",   2 },
	{   65, 103, "Big Pan",    2 },
	{   66, 103, "SynPiano",   2 },
	{   67, 103, "Creation",   2 },
	{   68, 103, "Stardust",   2 },
	{   69, 103, "Reso Pan",   2 },
	{    0, 104, "Sci-Fi",     2 },
	{   64, 104, "Starz",      2 }
};

// Ethnic
static
XGNormalVoiceItem EthnicTab[] =
{	// bank prog  name      elem
	{    0, 105, "Sitar",      1 },
	{   32, 105, "DetSitar",   2 },
	{   35, 105, "Sitar 2",    2 },
	{   96, 105, "Tambra",     2 },
	{   97, 105, "Tamboura",   2 },
	{    0, 106, "Banjo",      1 },
	{   28, 106, "MuteBnjo",   1 },
	{   96, 106, "Rabab",      2 },
	{   97, 106, "Gopichnt",   2 },
	{   98, 106, "Oud",        2 },
	{    0, 107, "Shamisen",   1 },
	{    0, 108, "Koto",       1 },
	{   96, 108, "T. Koto",    2 },
	{   97, 108, "Kanoon",     2 },
	{    0, 109, "Kalimba",    1 },
	{    0, 110, "Bagpipe",    2 },
	{    0, 111, "Fiddle",     1 },
	{    0, 112, "Shanai",     1 },
	{   64, 112, "Shanai2",    1 },
	{   96, 112, "Pungi",      1 },
	{   97, 112, "Hichriki",   2 }
};

// Percussive
static
XGNormalVoiceItem PercussiveTab[] =
{	// bank prog  name      elem
	{    0, 113, "TnklBell",   2 },
	{   96, 113, "Bonang",     2 },
	{   97, 113, "Gender",     2 },
	{   98, 113, "Gamelan",    2 },
	{   99, 113, "S.Gamlan",   2 },
	{  100, 113, "Rama Cym",   2 },
	{  101, 113, "AsianBel",   2 },
	{    0, 114, "Agogo",      2 },
	{    0, 115, "SteelDrm",   2 },
	{   97, 115, "GlasPerc",   2 },
	{   98, 115, "ThaiBell",   2 },
	{    0, 116, "WoodBlok",   1 },
	{   96, 116, "Castanet",   1 },
	{    0, 117, "TaikoDrm",   1 },
	{   96, 117, "Gr.Cassa",   1 },
	{    0, 118, "MelodTom",   2 },
	{   64, 118, "Mel Tom2",   1 },
	{   65, 118, "Real Tom",   2 },
	{   66, 118, "Rock Tom",   2 },
	{    0, 119, "Syn.Drum",   1 },
	{   64, 119, "Ana Tom",    1 },
	{   65, 119, "ElecPerc",   2 },
	{    0, 120, "RevCymbl",   1 }
};

// SoundEffects
static
XGNormalVoiceItem SoundEffectsTab[] =
{	// bank prog  name      elem
	{    0, 121, "FretNoiz",   2 },
	{    0, 122, "BrthNoiz",   2 },
	{    0, 123, "Seashore",   2 },
	{    0, 124, "Tweet",      2 },
	{    0, 125, "Telphone",   1 },
	{    0, 126, "Helicptr",   1 },
	{    0, 127, "Applause",   1 },
	{    0, 128, "Gunshot",    1 }
};

// SFX
static
XGNormalVoiceItem SFXTab[] = 
{	// bank prog  name      elem
	{ 8192,   1, "CuttngNz",   1 },
	{ 8192,   2, "CttngNz2",   2 },
	{ 8192,   4, "Str Slap",   1 },
	{ 8192,  17, "Fl.KClik",   1 },
	{ 8192,  33, "Rain",       1 },
	{ 8192,  34, "Thunder",    1 },
	{ 8192,  35, "Wind",       1 },
	{ 8192,  36, "Stream",     2 },
	{ 8192,  37, "Bubble",     2 },
	{ 8192,  38, "Feed",       2 },
	{ 8192,  49, "Dog",        1 },
	{ 8192,  50, "Horse",      1 },
	{ 8192,  51, "Bird 2",     1 },
	{ 8192,  55, "Ghost",      2 },
	{ 8192,  56, "Maou",       2 },
	{ 8192,  65, "Tel.Dial",   1 },
	{ 8192,  66, "DoorSqek",   1 },
	{ 8192,  67, "Door Slam",  1 },
	{ 8192,  68, "Scratch",    1 },
	{ 8192,  69, "Scratch 2",  2 },
	{ 8192,  70, "WindChm",    1 },
	{ 8192,  71, "Telphon2",   1 },
	{ 8192,  81, "CarEngin",   1 },
	{ 8192,  82, "Car Stop",   1 },
	{ 8192,  83, "Car Pass",   1 },
	{ 8192,  84, "CarCrash",   1 },
	{ 8192,  85, "Siren",      2 },
	{ 8192,  86, "Train",      1 },
	{ 8192,  87, "Jetplane",   2 },
	{ 8192,  88, "Starship",   2 },
	{ 8192,  89, "Burst",      2 },
	{ 8192,  90, "Coaster",    2 },
	{ 8192,  91, "SbMarine",   2 },
	{ 8192,  97, "Laughing",   1 },
	{ 8192,  98, "Scream",     1 },
	{ 8192,  99, "Punch",      1 },
	{ 8192, 100, "Heart",      1 },
	{ 8192, 101, "FootStep",   1 },
	{ 8192, 113, "MchinGun",   1 },
	{ 8192, 114, "LaserGun",   2 },
	{ 8192, 115, "Xplosion",   2 },
	{ 8192, 116, "FireWork",   2 }
};


//---------------------------------------------------------------------
// XG Normal Voice Group List
//

typedef
struct _XGNormalVoiceGroup
{
	const char        *name;
	XGNormalVoiceItem *items;
	unsigned short     size;

} XGNormalVoiceGroup;


// Normal Voice Groups
static
XGNormalVoiceGroup InstrumentTab[] =
{	// name                   items                   size
	{ "Piano",                PianoTab,               TSIZE(PianoTab)            },
	{ "Organ",                OrganTab,               TSIZE(OrganTab)            },
	{ "Guitar",               GuitarTab,              TSIZE(GuitarTab)           },
	{ "Guitar",               GuitarTab,              TSIZE(GuitarTab)           },
	{ "Bass",                 BassTab,                TSIZE(BassTab)             },
	{ "Strings",              StringsTab,             TSIZE(StringsTab)          },
	{ "Ensemble",             EnsembleTab,            TSIZE(EnsembleTab)         },
	{ "Brass",                BrassTab,               TSIZE(BrassTab)            },
	{ "Reed",                 ReedTab,                TSIZE(ReedTab)             },
	{ "Pipe",                 PipeTab,                TSIZE(PipeTab)             },
	{ "Synth Lead",           SynthLeadTab,           TSIZE(SynthLeadTab)        },
	{ "Synth Pad",            SynthPadTab,            TSIZE(SynthPadTab)         },
	{ "Synth Effects",        SynthEffectsTab,        TSIZE(SynthEffectsTab)     },
	{ "Ethnic",               EthnicTab,              TSIZE(EthnicTab)           },
	{ "Percussive",           PercussiveTab,          TSIZE(PercussiveTab)       },
	{ "Chromatic Percussion", ChromaticPercussionTab, TSIZE(ChromaticPercussionTab) },
	{ "Sound Effects",        SoundEffectsTab,        TSIZE(SoundEffectsTab)     },
	{ "SFX",                  SFXTab,                 TSIZE(SFXTab)                 }
};


//---------------------------------------------------------------------
// XG Drum Voice List
//

typedef
struct _XGDrumVoiceItem
{
	unsigned short note;
	const char    *name;

	// Default values
	unsigned short level;
	unsigned short group;
	unsigned short pan;
	unsigned short reverb;
	unsigned short chorus;
	unsigned short noteOff;

} XGDrumVoiceItem;


// Standard Kit
static
XGDrumVoiceItem StandardKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 13, "Surdo Mute",            102,   3,   51,   95,   95,  0 },
	{ 14, "Surdo Open",            121,   3,   51,   95,   95,  0 },
	{ 15, "Hi Q",                   63,   0,   51,  127,  127,  0 },
	{ 16, "Whip Slap",             127,   0,   51,  127,  127,  0 },
	{ 17, "Scratch Push",           93,   4,   52,   63,   63,  0 },
	{ 18, "Scratch Pull",          116,   4,   52,   63,   63,  0 },
	{ 19, "Finger Snap",           127,   0,   64,   75,    0,  0 },
	{ 20, "Click Noise",           127,   0,   64,  127,  127,  0 },
	{ 21, "Metronome Click",        94,   0,   64,   63,   63,  0 },
	{ 22, "Metronome Bell",         98,   0,   64,   63,   63,  0 },
	{ 23, "Seq Click L",            92,   0,   64,  127,  127,  0 },
	{ 24, "Seq Click H",           119,   0,   64,  127,  127,  0 },
	{ 25, "Brush Tap",              49,   0,   64,  127,  127,  0 },
	{ 26, "Brush Swirl L",          47,   0,   64,  127,  127,  1 },
	{ 27, "Brush Slap",             52,   0,   64,  127,  127,  0 },
	{ 28, "Brush Swirl H",          45,   0,   64,  127,  127,  1 },
	{ 29, "Snare Roll",             79,   0,   64,  127,  127,  1 },
	{ 30, "Castanet",              127,   0,   64,   63,   63,  0 },
	{ 31, "Snare L",                75,   0,   64,  127,  127,  0 },
	{ 32, "Sticks",                127,   0,   64,  127,  127,  0 },
	{ 33, "Bass Drum L",           116,   0,   64,   32,   32,  0 },
	{ 34, "Open Rim Shot",         127,   0,   64,  127,  127,  0 },
	{ 35, "Bass Drum M",           102,   0,   64,   32,   32,  0 },
	{ 36, "Bass Drum H",           127,   0,   64,   32,   32,  0 },
	{ 37, "Side Stick",             93,   0,   64,  127,  127,  0 },
	{ 38, "Snare M",               127,   0,   64,  127,  127,  0 },
	{ 39, "Hand Clap",             110,   0,   64,  127,  127,  0 },
	{ 40, "Snare H",               123,   0,   64,  127,  127,  0 },
	{ 41, "Floor Tom L",           111,   0,   24,  127,  127,  0 },
	{ 42, "Hi-Hat Closed",          91,   1,   77,   32,   32,  0 },
	{ 43, "Floor Tom H",           113,   0,   39,  127,  127,  0 },
	{ 44, "Hi-Hat Pedal",           92,   1,   77,   32,   32,  0 },
	{ 45, "Low Tom",                99,   0,   52,  127,  127,  0 },
	{ 46, "Hi-Hat Open",            96,   1,   77,   32,   32,  0 },
	{ 47, "Mid Tom L",              87,   0,   64,  127,  127,  0 },
	{ 48, "Mid Tom H",              99,   0,   83,  127,  127,  0 },
	{ 49, "Crash Cymbal 1",        127,   0,   69,  127,  127,  0 },
	{ 50, "High Tom",              116,   0,  104,  127,  127,  0 },
	{ 51, "Ride Cymbal 1",         105,   0,   34,  127,  127,  0 },
	{ 52, "Chinese Cymbal",        120,   0,   34,  127,  127,  0 },
	{ 53, "Ride Cymbal Cup",       107,   0,   46,  127,  127,  0 },
	{ 54, "Tambourine",            116,   0,   64,   63,   63,  0 },
	{ 55, "Splash Cymbal",         127,   0,   64,  127,  127,  0 },
	{ 56, "Cowbell",               118,   0,   77,   63,   63,  0 },
	{ 57, "Crash Cymbal 2",        127,   0,   51,  127,  127,  0 },
	{ 58, "Vibraslap",             106,   0,   25,  127,  127,  0 },
	{ 59, "Ride Cymbal 2",         110,   0,   46,  127,  127,  0 },
	{ 60, "Bongo H",               110,   0,  110,   95,   95,  0 },
	{ 61, "Bongo L",                87,   0,  110,   95,   95,  0 },
	{ 62, "Conga H Mute",           73,   0,   39,  127,  127,  0 },
	{ 63, "Conga H Open",           89,   0,   25,  127,  127,  0 },
	{ 64, "Conga L",               111,   0,   64,   95,   95,  0 },
	{ 65, "Timbale H",              91,   0,   64,  127,  127,  0 },
	{ 66, "Timbale L",              95,   0,   64,  127,  127,  0 },
	{ 67, "Agogo H",               108,   0,   34,  100,  100,  0 },
	{ 68, "Agogo L",               108,   0,   34,  100,  100,  0 },
	{ 69, "Cabasa",                 90,   0,   28,   63,   63,  0 },
	{ 70, "Maracas",                99,   0,   21,   63,   63,  0 },
	{ 71, "Samba Whistle H",       103,   0,  101,  127,  127,  1 },
	{ 72, "Samba Whistle L",       110,   0,  101,  127,  127,  1 },
	{ 73, "Guiro Short",           124,   0,   95,   63,   63,  0 },
	{ 74, "Guiro Long",            106,   0,  110,   63,   63,  1 },
	{ 75, "Claves",                 88,   0,   64,   95,   95,  0 },
	{ 76, "Wood Block H",          107,   0,  104,   95,   95,  0 },
	{ 77, "Wood Block L",           96,   0,  104,   95,   95,  0 },
	{ 78, "Cuica Mute",             97,   0,   21,  127,  127,  0 },
	{ 79, "Cuica Open",            107,   0,   34,  127,  127,  0 },
	{ 80, "Triangle Mute",         127,   2,   25,   95,   95,  0 },
	{ 81, "Triangle Open",         127,   2,   25,  127,  127,  0 },
	{ 82, "Shaker",                106,   0,   83,   63,   63,  0 },
	{ 83, "Jingle Bell",           123,   0,  105,  127,  127,  0 },
	{ 84, "Bell Tree",              68,   0,   64,  127,  127,  0 }
};

// Standard2 Kit
static
XGDrumVoiceItem Standard2KitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 29, "Snare Roll 2",           79,    0,  64,  127,  127,  1 },
	{ 31, "Snare L 2",              75,    0,  64,  127,  127,  0 },
	{ 34, "Open Rim Shot 2",       127,    0,  64,  127,  127,  0 },
	{ 35, "Bass Drum M 2",         102,    0,  64,   32,   32,  0 },
	{ 36, "Bass Drum H 2",         127,    0,  64,   32,   32,  0 },
	{ 38, "Snare M 2",             127,    0,  64,  127,  127,  0 },
	{ 40, "Snare H 2",             123,    0,  64,  127,  127,  0 }
};

// Room Kit
static
XGDrumVoiceItem RoomKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 38, "SD Room L",             127,   0,   64,  127,  127,  0 },
	{ 40, "SD Room H",             123,   0,   64,  127,  127,  0 },
	{ 41, "Room Tom 1",            123,   0,   24,  127,  127,  0 },
	{ 43, "Room Tom 2",            127,   0,   39,  127,  127,  0 },
	{ 45, "Room Tom 3",            117,   0,   52,  127,  127,  0 },
	{ 47, "Room Tom 4",            121,   0,   64,  127,  127,  0 },
	{ 48, "Room Tom 5",            123,   0,   83,  127,  127,  0 },
	{ 50, "Room Tom 6",            124,   0,   95,  127,  127,  0 }

};

// Rock Kit
static
XGDrumVoiceItem RockKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 31, "SD Rock M",             121,   0,   64,  127,  127,  0 },
	{ 33, "Bass Drum M",           111,   0,   64,   32,   32,  0 },
	{ 34, "Open Rim Shot 2",       127,   0,   64,  127,  127,  0 },
	{ 35, "Bass Drum H 3",         127,   0,   64,   32,   32,  0 },
	{ 36, "BD Rock",               119,   0,   64,   32,   32,  0 },
	{ 38, "SD Rock",               110,   0,   64,  127,  127,  0 },
	{ 40, "SD Rock Rim",           119,   0,   64,  127,  127,  0 },
	{ 41, "Rock Tom 1",            123,   0,   24,  127,  127,  0 },
	{ 43, "Rock Tom 2",            127,   0,   39,  127,  127,  0 },
	{ 45, "Rock Tom 3",            117,   0,   52,  127,  127,  0 },
	{ 47, "Rock Tom 4",            121,   0,   64,  127,  127,  0 },
	{ 48, "Rock Tom 5",            123,   0,   83,  127,  127,  0 },
	{ 50, "Rock Tom 6",            124,   0,   95,  127,  127,  0 }
};

// Electro Kit
static
XGDrumVoiceItem ElectroKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 28, "Reverse Cymbal",        100,   0,   64,  127,  127,  1 },
	{ 30, "Hi Q",                  127,   0,   64,   63,   63,  0 },
	{ 31, "Snare M",               114,   0,   64,  127,  127,  0 },
	{ 33, "Bass Drum H 4",         123,   0,   64,   32,   32,  0 },
	{ 35, "BD Rock",               127,   0,   64,   32,   32,  0 },
	{ 36, "BD Gate",               127,   0,   64,   32,   32,  0 },
	{ 38, "SD Rock L",             107,   0,   64,  127,  127,  0 },
	{ 40, "SD Rock H",             102,   0,   64,  127,  127,  0 },
	{ 41, "E Tom 1",                92,   0,   24,  127,  127,  0 },
	{ 43, "E Tom 2",                94,   0,   39,  127,  127,  0 },
	{ 45, "E Tom 3",                97,   0,   52,  127,  127,  0 },
	{ 47, "E Tom 4",                93,   0,   64,  127,  127,  0 },
	{ 48, "E Tom 5",               102,   0,   83,  127,  127,  0 },
	{ 50, "E Tom 6",                97,   0,  101,  127,  127,  0 },
	{ 78, "Scratch Push",           89,   4,   21,  127,  127,  0 },
	{ 79, "Scratch Pull",           94,   4,   34,  127,  127,  0 }
};

// Analog Kit
static
XGDrumVoiceItem AnalogKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 28, "Reverse Cymbal",        100,   0,   64,  127,  127,  1 },
	{ 30, "Hi Q",                  127,   0,   64,   63,   63,  0 },
	{ 31, "SD Rock H",             114,   0,   64,  127,  127,  0 },
	{ 33, "Bass Drum M",           111,   0,   64,   32,   32,  0 },
	{ 35, "BD Analog L",           123,   0,   64,   32,   32,  0 },
	{ 36, "BD Analog H",           127,   0,   64,   32,   32,  0 },
	{ 37, "Analog Side Stick",     116,   0,   64,  127,  127,  0 },
	{ 38, "Analog Snare L",        107,   0,   64,  127,  127,  0 },
	{ 40, "Analog Snare H",        102,   0,   64,  127,  127,  0 },
	{ 41, "Analog Tom 1",          127,   0,   24,  127,  127,  0 },
	{ 42, "Analog HH Closed 1",    108,   1,   77,   32,   32,  0 },
	{ 43, "Analog Tom 2",          112,   0,   39,  127,  127,  0 },
	{ 44, "Analog HH Closed 2",     91,   1,   77,   32,   32,  0 },
	{ 45, "Analog Tom 3",          108,   0,   52,  127,  127,  0 },
	{ 46, "Analog HH Open",         96,   1,   77,   32,   32,  0 },
	{ 47, "Analog Tom 4",          112,   0,   64,  127,  127,  0 },
	{ 48, "Analog Tom 5",          109,   0,   83,  127,  127,  0 },
	{ 49, "Analog Cymbal",         109,   0,   69,  127,  127,  0 },
	{ 50, "Analog Tom 6",          109,   0,  101,  127,  127,  0 },
	{ 56, "Analog Cowbell",        118,   0,   77,   63,   63,  0 },
	{ 62, "Analog Conga H",         89,   0,   39,  127,  127,  0 },
	{ 63, "Analog Conga M",         89,   0,   25,  127,  127,  0 },
	{ 64, "Analog Conga L",        115,   0,   64,   95,   95,  0 },
	{ 70, "Analog Maracas",         96,   0,   21,   63,   63,  0 },
	{ 75, "Analog Claves",          88,   0,   64,   95,   95,  0 },
	{ 78, "Scratch Push",           89,   4,   21,  127,  127,  0 },
	{ 79, "Scratch Pull",           94,   4,   34,  127,  127,  0 }
};

// Jazz Kit
static
XGDrumVoiceItem JazzKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 36, "BD Jazz",               120,   0,   64,   32,   32,  0 },
	{ 41, "Jazz Tom 1",            113,   0,   24,  127,  127,  0 },
	{ 43, "Jazz Tom 2",            122,   0,   39,  127,  127,  0 },
	{ 45, "Jazz Tom 3",            112,   0,   52,  127,  127,  0 },
	{ 47, "Jazz Tom 4",            127,   0,   64,  127,  127,  0 },
	{ 48, "Jazz Tom 5",            110,   0,   83,  127,  127,  0 },
	{ 50, "Jazz Tom 6",            116,   0,  104,  127,  127,  0 }
};

// Brush Kit
static
XGDrumVoiceItem BrushKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 31, "Brush Slap L",           85,   0,   64,  127,  127,  0 },
	{ 36, "BD Soft",               117,   0,   64,   32,   32,  0 },
	{ 38, "Brush Slap M",           84,   0,   64,  127,  127,  0 },
	{ 40, "Brush Tap H",            74,   0,   64,  127,  127,  0 },
	{ 41, "Brush Tom 1",           127,   0,   24,  127,  127,  0 },
	{ 43, "Brush Tom 2",           127,   0,   39,  127,  127,  0 },
	{ 45, "Brush Tom 3",           127,   0,   52,  127,  127,  0 },
	{ 47, "Brush Tom 4",           127,   0,   64,  127,  127,  0 },
	{ 48, "Brush Tom 5",           120,   0,   83,  127,  127,  0 },
	{ 50, "Brush Tom 6",           122,   0,  104,  127,  127,  0 }
};

// Classic Kit
static
XGDrumVoiceItem ClassicKitTab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 33, "Bass Drum L2",          116,   0,   64,   32,   32,  0 },
	{ 35, "Gran Cassa",            127,   0,   64,   32,   32,  0 },
	{ 36, "Gran Cassa Mute",       127,   0,   64,   32,   32,  0 },
	{ 38, "Marching Sn M",          79,   0,   64,  127,  127,  0 },
	{ 40, "Marching Sn H",          79,   0,   64,  127,  127,  0 },
	{ 41, "Jazz Tom 1",            113,   0,   24,  127,  127,  0 },
	{ 43, "Jazz Tom 2",            122,   0,   39,  127,  127,  0 },
	{ 45, "Jazz Tom 3",            112,   0,   52,  127,  127,  0 },
	{ 47, "Jazz Tom 4",            127,   0,   64,  127,  127,  0 },
	{ 48, "Jazz Tom 5",            110,   0,   83,  127,  127,  0 },
	{ 49, "Hand Cym Open L",       123,   0,   64,  127,  127,  0 },
	{ 50, "Jazz Tom 6",            116,   0,  104,  127,  127,  0 },
	{ 51, "Hand Cym.Closed L",     124,   0,   34,  127,  127,  0 },
	{ 57, "Hand Cym.Open H",       127,   0,   51,  127,  127,  0 },
	{ 59, "Hand Cym.Closed H",     106,   0,   46,  127,  127,  0 }
};

// SFX1
static
XGDrumVoiceItem SFX1Tab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 36, "Guitar Cutting Noise",  127,   0,   64,  127,  127,  1 },
	{ 37, "Guitar Cutting Noise 2",127,   0,   64,  127,  127,  1 },
	{ 39, "String Slap",           127,   0,   64,  127,  127,  1 },
	{ 52, "FL.Key Click",          127,   0,   64,  127,  127,  1 },
	{ 68, "Rain",                  127,   0,   64,  127,  127,  1 },
	{ 69, "Thunder",               127,   0,   64,  127,  127,  1 },
	{ 70, "Wind",                  127,   0,   64,  127,  127,  1 },
	{ 71, "Stream",                127,   0,   64,  127,  127,  1 },
	{ 72, "Bubble",                127,   0,   64,  127,  127,  1 },
	{ 73, "Feed",                  127,   0,   64,  127,  127,  1 },
	{ 84, "Dog",                   127,   0,   64,  127,  127,  1 },
	{ 85, "Horse Gallop",          127,   0,   64,  127,  127,  1 },
	{ 86, "Bird 2",                127,   0,   64,  127,  127,  1 },
	{ 90, "Ghost",                 127,   0,   64,  127,  127,  1 },
	{ 91, "Maou",                  127,   0,   64,  127,  127,  1 }
};
// SFX2
static
XGDrumVoiceItem SFX2Tab[] =
{	//     name                   level group pan reverb chorus noteOff
	{ 36, "Dial Tone",             127,   0,   64,  127,  127,  1 },
	{ 37, "Door Creaking",         127,   0,   64,  127,  127,  1 },
	{ 38, "Door Slam",             127,   0,   64,  127,  127,  1 },
	{ 39, "Scratch",               127,   0,   64,  127,  127,  1 },
	{ 40, "Scratch 2",             127,   0,   64,  127,  127,  1 },
	{ 41, "Windchime",             127,   0,   64,  127,  127,  1 },
	{ 42, "Telephone Ring2",       127,   0,   64,  127,  127,  1 },
	{ 52, "Engine Start",          127,   0,   64,  127,  127,  1 },
	{ 53, "Tire Screech",          127,   0,   64,  127,  127,  1 },
	{ 54, "Car Passing",           127,   0,   64,  127,  127,  1 },
	{ 55, "Crash",                 127,   0,   64,  127,  127,  1 },
	{ 56, "Siren",                 127,   0,   64,  127,  127,  1 },
	{ 57, "Train",                 127,   0,   64,  127,  127,  1 },
	{ 58, "Jetplane",              127,   0,   64,  127,  127,  1 },
	{ 59, "Starship",              127,   0,   64,  127,  127,  1 },
	{ 60, "Burst Noise",           127,   0,   64,  127,  127,  1 },
	{ 61, "Coaster",               127,   0,   64,  127,  127,  1 },
	{ 62, "SbMarine",              127,   0,   64,  127,  127,  1 },
	{ 68, "Laughing",              127,   0,   64,  127,  127,  1 },
	{ 69, "Screaming",             127,   0,   64,  127,  127,  1 },
	{ 70, "Punch",                 127,   0,   64,  127,  127,  1 },
	{ 71, "Heartbeat",             127,   0,   64,  127,  127,  1 },
	{ 72, "Footsteps",             127,   0,   64,  127,  127,  1 },
	{ 84, "Machine Gun",           127,   0,   64,  127,  127,  1 },
	{ 85, "Laser Gun",             127,   0,   64,  127,  127,  1 },
	{ 86, "Explosion",             127,   0,   64,  127,  127,  1 },
	{ 87, "FireWork",              127,   0,   64,  127,  127,  1 }
};


//---------------------------------------------------------------------
// XG Drum Kit List
//

typedef
struct _XGDrumKitItem
{
	unsigned short   bank;
	unsigned short   prog;
	const char      *name;
	XGDrumVoiceItem *keys;
	unsigned short   size;

} XGDrumKitItem;


static
XGDrumKitItem DrumKitTab[] =
{	// bank prog  name            keys             size
	{ 16256,  1, "Standard Kit",  StandardKitTab,  TSIZE(StandardKitTab)  },
	{ 16256,  2, "Standard2 Kit", Standard2KitTab, TSIZE(Standard2KitTab) },
	{ 16256,  9, "Room Kit",      RoomKitTab,      TSIZE(RoomKitTab)   },
	{ 16256, 17, "Rock Kit",      RockKitTab,      TSIZE(RockKitTab)   },
	{ 16256, 25, "Electro Kit",   ElectroKitTab,   TSIZE(ElectroKitTab)},
	{ 16256, 26, "Analog Kit",    AnalogKitTab,    TSIZE(AnalogKitTab) },
	{ 16256, 33, "Jazz Kit",      JazzKitTab,      TSIZE(JazzKitTab)   },
	{ 16256, 41, "Brush Kit",     BrushKitTab,     TSIZE(BrushKitTab)  },
	{ 16256, 49, "Classic Kit",   ClassicKitTab,   TSIZE(ClassicKitTab)},
	{ 16128,  1, "SFX 1",         SFX1Tab,         TSIZE(SFX1Tab)      },
	{ 16128,  2, "SFX 2",         SFX2Tab,         TSIZE(SFX2Tab)         }
};


//-------------------------------------------------------------------------
// XG Effect table helpers.
//
typedef
struct _XGEffectParamItem
{
	unsigned short   id;                    // parameter index
	const char      *name;                  // parameter name.
	unsigned short   min;                   // minimum value.
	unsigned short   max;                   // maximum value.
	float          (*getv)(unsigned short); // convert to display value.
	unsigned short (*getu)(float);          // invert to native value.
	const char *   (*gets)(unsigned short); // enumerated string value.
	const char *   (*unit)();               // unit suffix label.

} XGEffectParamItem;

typedef
struct _XGEffectItem
{
	unsigned char      msb;                 // effect type MSB
	unsigned char      lsb;                 // effect type LSB
	const char        *name;                // effect type name
	XGEffectParamItem *params;              // effect type parameters
	unsigned short    *defs;                // effect type defaults

} XGEffectItem;


//-------------------------------------------------------------------------
// XG Effect Parameter List
//

// HALL1,2, ROOM1,2,3, STAGE1,2, PLATE
static
XGEffectParamItem HALLROOMParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "[Reverb ]Time",      0,  69, getvtab4, getutab4, nullptr,  unit_ms  },
	{  1, "Diffusion",          0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "[Initial ]Delay",    0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{  3, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  4, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Rev Delay",          0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{ 11, "Density",            0,   4, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, "[Rev/Er ]Balance",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 13, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{ 14, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// WHITE ROOM, TUNNEL, CANYON, BASEMENT
static
XGEffectParamItem WHITEROOMParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "[Reverb ]Time",      0,  69, getvtab4, getutab4, nullptr,  unit_ms  },
	{  1, "Diffusion",          0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "[Initial ]Delay",    0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{  3, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  4, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  5, "Width",              0,  37, getvtab8 ,getutab8, nullptr,  unit_m   },
	{  6, "Height",             0,  73, getvtab8 ,getutab8, nullptr,  unit_m   },
	{  7, "Depth",              0, 104, getvtab8 ,getutab8, nullptr,  unit_m   },
	{  8, "Wall Vary",          0,  30, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Rev Delay",          0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{ 11, "Density",            0,   4, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, "[Rev/Er ]Balance",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 13, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{ 14, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// DELAY L,C,R
static
XGEffectParamItem DELAYLCRParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "L[ch] Delay",        1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  1, "R[ch] Delay",        1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  2, "C[ch] Delay",        1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  3, "[Feedback ]Delay",   1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  4, "[Feedback ]Level",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  5, "C[ch] Level",        0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 12, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 13, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 14, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 15, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  }
};

// DELAY L,R
static
XGEffectParamItem DELAYLRParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "L[ch ]Delay",        1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  1, "R[ch ]Delay",        1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  2, "[Feedback ]Delay 1", 1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  3, "[Feedback ]Delay 2", 1,7550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  4, "[Feedback ]Level",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  5, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 12, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 13, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 14, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 15, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  }
};

// ECHO
static
XGEffectParamItem ECHOParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "L[ch] Delay 1",      1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  1, "L[ch] Feedb[ack Level]",0,127,getv0x40,getu0x40, nullptr,  nullptr  },
	{  2, "R[ch] Delay 1",      1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  3, "R[ch] Feedb[ack Level]",1,127,getv0x40,getu0x40, nullptr,  nullptr  },
	{  4, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  5, "L[ch] Delay 2",      1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  6, "R[ch] Delay 2",      1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  7, "Delay 2[ Level]",    0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 12, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 13, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 14, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 15, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  }
};

// CROSS DELAY
static
XGEffectParamItem CROSSDELAYParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "L->R Delay",         1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  1, "R->L Delay",         1,3550, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  2, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  3, "Input[ Select]",     0,   2, nullptr,  nullptr,  getsisel, nullptr  },
	{  4, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 12, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 13, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 14, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 15, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  }
};

// EARLY REF1,2
static
XGEffectParamItem EARLYREFParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Type",               0,   5, nullptr,  nullptr,  getsreft, nullptr  },
	{  1, "Room[ Size]",        0,  44, getvtab6, getutab6, nullptr,  unit_m   },
	{  2, "Diffusion",          0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, "[Initial ]Delay",    0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{  4, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  5, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Liveness",           0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, "Density",            0,   3, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
};

// GATE REVERB, REVERSE GATE
static
XGEffectParamItem GATEREVERBParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Type",               0,   1, nullptr,  nullptr,  getsrevt, nullptr  },
	{  1, "Room[ Size]",        0,  44, getvtab6, getutab6, nullptr,  unit_m   },
	{  2, "Diffusion",          0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, "[Initial ]Delay",    0,  63, getvtab5, getutab5, nullptr,  unit_ms  },
	{  4, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  5, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Liveness",           0,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, "Density",            1,   3, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, "[High ]Damp",        1,  10, getv0_10, getu0_10, nullptr,  unit_ms  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// KARAOKE1,2,3
static
XGEffectParamItem KARAOKEParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Delay[ Time]",       0, 127, getvtab7, getutab7, nullptr,  unit_ms  },
	{  1, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  2, "HPF[ Cutoff Thru]",  0,  52, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  3, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// CHORUS1,2,3,4 CELESTE1,2,3,4
static
XGEffectParamItem CHORUSParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO PM[ Depth]",     0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  3, "Delay[ Offset]",     0, 127, getvtab2, getutab2, nullptr,  unit_ms  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, "LFO AM[ Depth]",     0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, "Input[ Mode]",       0,   1, nullptr,  nullptr,  getsimod, nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// FLANGER1,2,3
static
XGEffectParamItem FLANGERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  3, "Delay[ Offset]",     0,  63, getvtab2, getutab2, nullptr,  unit_ms  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  nullptr  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, "LFO Phase[ Diff]",   4, 124, getv_180, getu_180, nullptr,  unit_deg },
	{ 14, "Input[ Mode]",       0,   1, nullptr,  nullptr,  getsimod, nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// SYMPHONIC
static
XGEffectParamItem SYMPHONICParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "Delay[ Offset]",     0,   0, getvtab2, getutab2, nullptr,  unit_ms  },
	{  3, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ 9Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// ROTARY SPEAKER
static
XGEffectParamItem ROTSPEAKERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// TREMOLO
static
XGEffectParamItem TREMOLOParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "AM Depth",           0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "PM Depth",           0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, "LFO Phase[ Diff]",   4, 124, getv_180, getu_180, nullptr,  unit_deg },
	{ 14, "Input[ Mode]",       0,   1, nullptr,  nullptr,  getsimod, nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// AUTO PAN
static
XGEffectParamItem AUTOPANParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "L/R Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "F/R Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, "[Pan ]Direction",    0,   5, nullptr,  nullptr,  getspand, nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// PHASER1,2
static
XGEffectParamItem PHASERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "Phase[ Shift Offset]",0,127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, "Feedback[ Level]",   1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "Stage",              1,  10, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, "Diffusion",          0,   1, nullptr,  nullptr,  getsimod, nullptr  },
	{ 12, "LFO Phase[ Diff]",   4, 124, getv_180, getu_180, nullptr,  unit_deg },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// DISTORTION, OVERDRIVE
static
XGEffectParamItem DISTORTIONParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Drive",              0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  1, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  2, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  3, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  4, "Output[ Level]",     0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  7, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  8, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Edge[ (Clip Curve)]",0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// GUITAR AMP SIMULATOR
static
XGEffectParamItem AMPSIMULParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Drive",              0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  1, "AMP[ Type]",         0,   3, nullptr,  nullptr,  getsampt, nullptr  },
	{  2, "LPF[ Cutoff]",      34,  60, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  3, "Output[ Level]",     0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Edge[ (Clip Curve)]",0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// 3-BAND EQ (MONO)
static
XGEffectParamItem MONOEQParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  1, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  2, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  3, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  5, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// 2-BAND EQ (STEREO)
static
XGEffectParamItem STEREOEQParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "[EQ ]Low Freq",      4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  1, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  2, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  3, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  6, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  7, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  8, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  9, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 10, "[EQ ]Mid Freq",     14,  54, getvtab3, getutab3, nullptr,  unit_Hz  },
	{ 11, "[EQ ]Mid Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{ 12, "[EQ ]Mid Width",    10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};

// AUTO WAH (LFO)
static
XGEffectParamItem AUTOWAHParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, nullptr,  unit_Hz  },
	{  1, "LFO Depth",          0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  2, "[Cutoff Freq ]Offset",0,127, nullptr,  nullptr,  nullptr,  nullptr  },
	{  3, "Resonance",         10, 120, nullptr,  nullptr,  nullptr,  nullptr  },
	{  4, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{  5, "[EQ ]Low Frequ",     4,  40, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  6, "[EQ ]Low Gain",     52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  7, "[EQ ]High Freq",    28,  58, getvtab3, getutab3, nullptr,  unit_Hz  },
	{  8, "[EQ ]High Gain",    52,  76, getv0x40, getu0x40, nullptr,  unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 10, "Drive",              0, 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 11, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 12, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 13, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 14, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 15, nullptr,              0,   0, nullptr,  nullptr,  nullptr,  nullptr  }
};


//-------------------------------------------------------------------------
// XG Effect Default Parameter Data

// REVERB
static
unsigned short REVERBDefaultTab[][16] =
{	//  0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // NO EFFECT
	{  18,  10,   8,  13,  49,   0,   0,   0,   0,  40,   0,   4,  50,   8,  64,   0 }, // HALL 1
	{  25,  10,  28,   6,  46,   0,   0,   0,   0,  40,  13,   3,  74,   7,  64,   0 }, // HALL 2
	{   5,  10,  16,   4,  49,   0,   0,   0,   0,  40,   5,   3,  64,   8,  64,   0 }, // ROOM 1
	{  12,  10,   5,   4,  38,   0,   0,   0,   0,  40,   0,   4,  50,   8,  64,   0 }, // ROOM 2
	{   9,  10,  47,   5,  36,   0,   0,   0,   0,  40,   0,   4,  60,   8,  64,   0 }, // ROOM 3
	{  19,  10,  16,   7,  54,   0,   0,   0,   0,  40,   0,   3,  64,   6,  64,   0 }, // STAGE 1
	{  11,  10,  16,   7,  51,   0,   0,   0,   0,  40,   2,   2,  64,   6,  64,   0 }, // STAGE 2
	{  25,  10,   6,   8,  49,   0,   0,   0,   0,  40,   2,   3,  64,   5,  64,   0 }, // PLATE
	{   9,   5,  11,   0,  46,  30,  50,  70,   7,  40,  34,   4,  64,   7,  64,   0 }, // WHITE ROOM
	{  48,   6,  19,   0,  44,  33,  52,  70,  16,  40,  20,   4,  64,   7,  64,   0 }, // TUNNEL
	{   3,   6,   3,   0,  34,  26,  29,  59,  15,  40,  32,   4,  64,   8,  64,   0 }  // BASEMENT
};

// CHORUS
static
unsigned short CHORUSDefaultTab[][16] =
{	//  0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // NO EFFECT
	{   6,  54,  77, 106,   0,  28,  64,  46,  64,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 1
	{   8,  63,  64,  30,   0,  28,  62,  42,  58,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 2
	{   4,  44,  64, 110,   0,  28,  64,  46,  66,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 3
	{   9,  32,  69, 104,   0,  28,  64,  46,  64,  64,  46,  64,  10,   0,   1,   0 }, // CHORUS 4
	{  12,  32,  64,   0,   0,  28,  64,  46,  64, 127,  40,  68,  10,   0,   0,   0 }, // CELESTE 1
	{  28,  18,  90,   2,   0,  28,  62,  42,  60,  84,  40,  68,  10,   0,   0,   0 }, // CELESTE 2
	{   4,  63,  44,   2,   0,  28,  64,  46,  68, 127,  40,  68,  10,   0,   0,   0 }, // CELESTE 3
	{   8,  29,  64,   0,   0,  28,  64,  51,  66, 127,  40,  68,  10,   0,   1,   0 }, // CELESTE 4
	{  14,  14, 104,   2,   0,  28,  64,  46,  64,  96,  40,  64,  10,   4,   0,   0 }, // FLANGER 1
	{  32,  17,  26,   2,   0,  28,  64,  46,  60,  96,  40,  64,  10,   4,   0,   0 }, // FLANGER 2
	{   4, 109, 109,   2,   0,  28,  64,  46,  64, 127,  40,  64,  10,   4,   0,   0 }  // FLANGER 3
};

// VARIANTION
static
unsigned short VARIATIONDefaultTab[][16] =
{	//  0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // NO EFFECT
	{  18,  10,   8,  13,  49,   0,   0,   0,   0,  40,   0,   4,  50,   8,  64,   0 }, // HALL 1
	{  25,  10,  28,   6,  46,   0,   0,   0,   0,  40,  13,   3,  74,   7,  64,   0 }, // HALL 2
	{   5,  10,  16,   4,  49,   0,   0,   0,   0,  40,   5,   3,  64,   8,  64,   0 }, // ROOM 1
	{  12,  10,   5,   4,  38,   0,   0,   0,   0,  40,   0,   4,  50,   8,  64,   0 }, // ROOM 2
	{   9,  10,  47,   5,  36,   0,   0,   0,   0,  40,   0,   4,  60,   8,  64,   0 }, // ROOM 3
	{  19,  10,  16,   7,  54,   0,   0,   0,   0,  40,   0,   3,  64,   6,  64,   0 }, // STAGE 1
	{  11,  10,  16,   7,  51,   0,   0,   0,   0,  40,   2,   2,  64,   6,  64,   0 }, // STAGE 2
	{  25,  10,   6,   8,  49,   0,   0,   0,   0,  40,   2,   3,  64,   5,  64,   0 }, // PLATE
	{3333,1667,5000,5000,  74, 100,  10,   0,   0,  32,   0,  60,  28,  64,  46,  64 }, // DELAY L,C,R
	{2500,3750,3752,3750,  87,  10,   0,   0,   0,  32,   0,  60,  28,  64,  46,  64 }, // DELAY L,R
	{1700,  80,1780,  80,  10,1700,1780,   0,   0,  40,   0,  60,  28,  64,  46,  64 }, // ECHO
	{1700,1750, 111,   1,  10,   0,   0,   0,   0,  32,   0,  60,  28,  64,  46,  64 }, // CROSS DELAY
	{   0,  19,   5,  16,  64,   0,  46,   0,   0,  32,   5,   0,  10,   0,   0,   0 }, // EARLY REF 1
	{   2,   7,  10,  16,  64,   3,  46,   0,   0,  32,   5,   2,  10,   0,   0,   0 }, // EARLY REF 2
	{   0,  15,   6,   2,  64,   0,  44,   0,   0,  32,   4,   3,  10,   0,   0,   0 }, // GATE REVERB
	{   1,  19,   8,   3,  64,   0,  47,   0,   0,  32,   6,   3,  10,   0,   0,   0 }, // REVERSE GATE
	{  63,  97,   0,  48,   0,   0,   0,   0,   0,  64,   2,   0,   0,   0,   0,   0 }, // KARAOKE 1
	{  55, 105,   0,  50,   0,   0,   0,   0,   0,  64,   1,   0,   0,   0,   0,   0 }, // KARAOKE 2
	{  43, 110,  14,  53,   0,   0,   0,   0,   0,  64,   0,   0,   0,   0,   0,   0 }, // KARAOKE 3
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // THRU
	{   6,  54,  77, 106,   0,  28,  64,  46,  64,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 1
	{   8,  63,  64,  30,   0,  28,  62,  42,  58,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 2
	{   4,  44,  64, 110,   0,  28,  64,  46,  66,  64,  46,  64,  10,   0,   0,   0 }, // CHORUS 3
	{   9,  32,  69, 104,   0,  28,  64,  46,  64,  64,  46,  64,  10,   0,   1,   0 }, // CHORUS 4
	{  12,  32,  64,   0,   0,  28,  64,  46,  64, 127,  40,  68,  10,   0,   0,   0 }, // CELESTE 1
	{  28,  18,  90,   2,   0,  28,  62,  42,  60,  84,  40,  68,  10,   0,   0,   0 }, // CELESTE 2
	{   4,  63,  44,   2,   0,  28,  64,  46,  68, 127,  40,  68,  10,   0,   0,   0 }, // CELESTE 3
	{   8,  29,  64,   0,   0,  28,  64,  51,  66, 127,  40,  68,  10,   0,   1,   0 }, // CELESTE 4
	{  14,  14, 104,   2,   0,  28,  64,  46,  64,  96,  40,  64,  10,   4,   0,   0 }, // FLANGER 1
	{  32,  17,  26,   2,   0,  28,  64,  46,  60,  96,  40,  64,  10,   4,   0,   0 }, // FLANGER 2
	{   4, 109, 109,   2,   0,  28,  64,  46,  64, 127,  40,  64,  10,   4,   0,   0 }, // FLANGER 3
	{  12,  25,  16,   0,   0,  28,  64,  46,  64, 127,  46,  64,  10,   0,   0,   0 }, // SYMPHONIC
	{  81,  35,   0,   0,   0,  24,  60,  45,  54, 127,  33,  52,  30,   0,   0,   0 }, // ROTARY SPEAKER
	{  83,  56,   0,   0,   0,  28,  64,  46,  64, 127,  40,  64,  10,  64,   0,   0 }, // TREMOLO
	{  76,  80,  32,   5,   0,  28,  64,  46,  64, 127,  40,  64,  10,   0,   0,   0 }, // AUTO PAN
	{   8, 111,  74, 104,   0,  28,  64,  46,  64,  64,   6,   1,  64,   0,   0,   0 }, // PHASER 1
	{   8, 111,  74, 108,   0,  28,  64,  46,  64,  64,   5,   1,   4,   0,   0,   0 }, // PHASER 2
	{  40,  20,  72,  53,  48,   0,  43,  74,  10, 127, 120,   0,   0,   0,   0,   0 }, // DISTORTION
	{  29,  24,  68,  45,  55,   0,  41,  72,  10, 127, 104,   0,   0,   0,   0,   0 }, // OVERDRIVE
	{  39,   1,  48,  55,   0,   0,   0,   0,   0, 127, 112,   0,   0,   0,   0,   0 }, // AMP SIMULATOR
	{  70,  34,  60,  10,  70,  28,  46,   0,   0, 127,   0,   0,   0,   0,   0,   0 }, // 3BAND EQ (MONO)
	{  28,  70,  46,  70,   0,   0,   0,   0,   0, 127,  34,  64,  10,   0,   0,   0 }, // 2BAND EQ (STEREO)
	{  70,  56,  39,  25,   0,  28,  66,  46,  64, 127,   0,   0,   0,   0,   0,   0 }  // AUTO WAH (LFO)
};


//-------------------------------------------------------------------------
// XG Effect Type List
//

// REVERB
static
XGEffectItem REVERBEffectTab[] =
{	// msb   lsb   name             params              defs
	{ 0x00, 0x00, "NO EFFECT",      nullptr,            REVERBDefaultTab[ 0] },
	{ 0x01, 0x00, "HALL 1",         HALLROOMParamTab,   REVERBDefaultTab[ 1] },
	{ 0x01, 0x01, "HALL 2",         HALLROOMParamTab,   REVERBDefaultTab[ 2] },
	{ 0x02, 0x00, "ROOM 1",         HALLROOMParamTab,   REVERBDefaultTab[ 3] },
	{ 0x02, 0x01, "ROOM 2",         HALLROOMParamTab,   REVERBDefaultTab[ 4] },
	{ 0x02, 0x02, "ROOM 3",         HALLROOMParamTab,   REVERBDefaultTab[ 5] },
	{ 0x03, 0x00, "STAGE 1",        HALLROOMParamTab,   REVERBDefaultTab[ 6] },
	{ 0x03, 0x01, "STAGE 2",        HALLROOMParamTab,   REVERBDefaultTab[ 7] },
	{ 0x04, 0x00, "PLATE",          HALLROOMParamTab,   REVERBDefaultTab[ 8] },
	{ 0x10, 0x00, "WHITE ROOM",     WHITEROOMParamTab,  REVERBDefaultTab[ 9] },
	{ 0x11, 0x00, "TUNNEL",         WHITEROOMParamTab,  REVERBDefaultTab[10] },
	{ 0x13, 0x00, "BASEMENT",       WHITEROOMParamTab,  REVERBDefaultTab[11] }
};

// CHORUS
static
XGEffectItem CHORUSEffectTab[] =
{	// msb   lsb   name             params              defs
	{ 0x00, 0x00, "NO EFFECT",      nullptr,            CHORUSDefaultTab[ 0] },
	{ 0x41, 0x00, "CHORUS 1",       CHORUSParamTab,     CHORUSDefaultTab[ 1] },
	{ 0x41, 0x01, "CHORUS 2",       CHORUSParamTab,     CHORUSDefaultTab[ 2] },
	{ 0x41, 0x02, "CHORUS 3",       CHORUSParamTab,     CHORUSDefaultTab[ 3] },
	{ 0x41, 0x08, "CHORUS 4",       CHORUSParamTab,     CHORUSDefaultTab[ 4] },
	{ 0x42, 0x00, "CELESTE 1",      CHORUSParamTab,     CHORUSDefaultTab[ 5] },
	{ 0x42, 0x01, "CELESTE 2",      CHORUSParamTab,     CHORUSDefaultTab[ 6] },
	{ 0x42, 0x02, "CELESTE 3",      CHORUSParamTab,     CHORUSDefaultTab[ 7] },
	{ 0x42, 0x08, "CELESTE 4",      CHORUSParamTab,     CHORUSDefaultTab[ 8] },
	{ 0x43, 0x00, "FLANGER 1",      FLANGERParamTab,    CHORUSDefaultTab[ 9] },
	{ 0x43, 0x01, "FLANGER 2",      FLANGERParamTab,    CHORUSDefaultTab[10] },
	{ 0x43, 0x08, "FLANGER 3",      FLANGERParamTab,    CHORUSDefaultTab[11] }
};

// VARIATION
static
XGEffectItem VARIATIONEffectTab[] =
{	// msb   lsb   name             params              defs
	{ 0x00, 0x00, "NO EFFECT",      nullptr,            VARIATIONDefaultTab[ 0] },
	{ 0x01, 0x00, "HALL 1",         HALLROOMParamTab,   VARIATIONDefaultTab[ 1] },
	{ 0x01, 0x01, "HALL 2",         HALLROOMParamTab,   VARIATIONDefaultTab[ 2] },
	{ 0x02, 0x00, "ROOM 1",         HALLROOMParamTab,   VARIATIONDefaultTab[ 3] },
	{ 0x02, 0x01, "ROOM 2",         HALLROOMParamTab,   VARIATIONDefaultTab[ 4] },
	{ 0x02, 0x02, "ROOM 3",         HALLROOMParamTab,   VARIATIONDefaultTab[ 5] },
	{ 0x03, 0x00, "STAGE 1",        HALLROOMParamTab,   VARIATIONDefaultTab[ 6] },
	{ 0x03, 0x01, "STAGE 2",        HALLROOMParamTab,   VARIATIONDefaultTab[ 7] },
	{ 0x04, 0x00, "PLATE",          HALLROOMParamTab,   VARIATIONDefaultTab[ 8] },
	{ 0x05, 0x00, "DELAY L,C,R",    DELAYLCRParamTab,   VARIATIONDefaultTab[ 9] },
	{ 0x06, 0x00, "DELAY L,R",      DELAYLRParamTab,    VARIATIONDefaultTab[10] },
	{ 0x07, 0x00, "ECHO",           ECHOParamTab,       VARIATIONDefaultTab[11] },
	{ 0x08, 0x00, "CROSS DELAY",    CROSSDELAYParamTab, VARIATIONDefaultTab[12] },
	{ 0x09, 0x00, "EARLY REF 1",    EARLYREFParamTab,   VARIATIONDefaultTab[13] },
	{ 0x09, 0x01, "EARLY REF 2",    EARLYREFParamTab,   VARIATIONDefaultTab[14] },
	{ 0x0a, 0x00, "GATE REVERB",    GATEREVERBParamTab, VARIATIONDefaultTab[15] },
	{ 0x0b, 0x00, "REVERSE GATE",   GATEREVERBParamTab, VARIATIONDefaultTab[16] },
	{ 0x14, 0x00, "KARAOKE 1",      KARAOKEParamTab,    VARIATIONDefaultTab[17] },
	{ 0x14, 0x01, "KARAOKE 2",      KARAOKEParamTab,    VARIATIONDefaultTab[18] },
	{ 0x14, 0x02, "KARAOKE 3",      KARAOKEParamTab,    VARIATIONDefaultTab[19] },
	{ 0x40, 0x00, "THRU",           nullptr,            VARIATIONDefaultTab[20] },
	{ 0x41, 0x00, "CHORUS 1",       CHORUSParamTab,     VARIATIONDefaultTab[21] },
	{ 0x41, 0x01, "CHORUS 2",       CHORUSParamTab,     VARIATIONDefaultTab[22] },
	{ 0x41, 0x02, "CHORUS 3",       CHORUSParamTab,     VARIATIONDefaultTab[23] },
	{ 0x41, 0x08, "CHORUS 4",       CHORUSParamTab,     VARIATIONDefaultTab[24] },
	{ 0x42, 0x00, "CELESTE 1",      CHORUSParamTab,     VARIATIONDefaultTab[25] },
	{ 0x42, 0x01, "CELESTE 2",      CHORUSParamTab,     VARIATIONDefaultTab[26] },
	{ 0x42, 0x02, "CELESTE 3",      CHORUSParamTab,     VARIATIONDefaultTab[27] },
	{ 0x42, 0x08, "CELESTE 4",      CHORUSParamTab,     VARIATIONDefaultTab[28] },
	{ 0x43, 0x00, "FLANGER 1",      FLANGERParamTab,    VARIATIONDefaultTab[29] },
	{ 0x43, 0x01, "FLANGER 2",      FLANGERParamTab,    VARIATIONDefaultTab[30] },
	{ 0x43, 0x08, "FLANGER 3",      FLANGERParamTab,    VARIATIONDefaultTab[31] },
	{ 0x44, 0x00, "SYMPHONIC",      SYMPHONICParamTab,  VARIATIONDefaultTab[32] },
	{ 0x45, 0x00, "ROTARY SPEAKER", ROTSPEAKERParamTab, VARIATIONDefaultTab[33] },
	{ 0x46, 0x00, "TREMOLO",        TREMOLOParamTab,    VARIATIONDefaultTab[34] },
	{ 0x47, 0x00, "AUTO PAN",       AUTOPANParamTab,    VARIATIONDefaultTab[35] },
	{ 0x48, 0x00, "PHASER 1",       PHASERParamTab,     VARIATIONDefaultTab[36] },
	{ 0x48, 0x08, "PHASER 2",       PHASERParamTab,     VARIATIONDefaultTab[37] },
	{ 0x49, 0x00, "DISTORTION",     DISTORTIONParamTab, VARIATIONDefaultTab[38] },
	{ 0x4a, 0x00, "OVERDRIVE",      DISTORTIONParamTab, VARIATIONDefaultTab[39] },
	{ 0x4b, 0x00, "AMP SIMULATOR",  AMPSIMULParamTab,   VARIATIONDefaultTab[40] },
	{ 0x4c, 0x00, "3BAND EQ (MONO)", MONOEQParamTab,    VARIATIONDefaultTab[41] },
	{ 0x4d, 0x00, "2BAND EQ (STEREO)",STEREOEQParamTab, VARIATIONDefaultTab[42] },
	{ 0x4e, 0x00, "AUTO WAH (LFO)", AUTOWAHParamTab,    VARIATIONDefaultTab[43] }
};


//-------------------------------------------------------------------------
// XG Parameter table helpers.
//
typedef
struct _XGParamItem
{
	unsigned short   id;	// id=low address.
	unsigned short   size;  // data size in bytes.
	unsigned short   min;   // minimum value; 0=REVERB, 1=CHORUS, 2=VARIATION.
	unsigned short   max;   // maximum value; parameter index (0..15)
	const char      *name;  // parameter name; nullptr=depends on effect type.
	unsigned short   def;   // default value;
	float          (*getv)(unsigned short); // convert to display value.
	unsigned short (*getu)(float);          // invert to native value.
	const char *   (*gets)(unsigned short); // enumerated string value.
	const char *   (*unit)();               // unit suffix label.

} XGParamItem;


//-------------------------------------------------------------------------
// XG Parameter tables

// SYSTEM
// Address: 0x00 0x00 <id>
//
static
XGParamItem SYSTEMParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 4,  0, 2047, "[Master ]Tune",        1024, getv1024, getu1024, nullptr,  unit_cen },
	{ 0x04, 1,  0,  127, "[Master ]Volume",       127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x06, 1, 40,   88, "Transpose",              64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x7d, 1,  0,    1, "Drum[ Setup ]Reset",      0, nullptr,  nullptr,  nullptr,  nullptr  }, // Drumset number (0,1)
	{ 0x7e, 1,  0,    0, "XG System On",            0, nullptr,  nullptr,  nullptr,  nullptr  }, // 0=XG System ON (receive only)
	{ 0x7f, 1,  0,    0, "All[ Parameter ]Reset",   0, nullptr,  nullptr,  nullptr,  nullptr  }  // 0=ON (receive only)
};


// EFFECT
// Address: 0x02 0x01 <id>
//
static
XGParamItem EFFECTParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 2,  0,16383, "[Reverb ]Type",         128, nullptr,  nullptr,  nullptr,  nullptr  }, // msb=0x01 lsb=0x00 (HALL 1)
	{ 0x02, 1,  0,    0, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 1  - depends on REVERB TYPE
	{ 0x03, 1,  0,    1, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 2  - depends on REVERB TYPE
	{ 0x04, 1,  0,    2, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 3  - depends on REVERB TYPE
	{ 0x05, 1,  0,    3, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 4  - depends on REVERB TYPE
	{ 0x06, 1,  0,    4, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 5  - depends on REVERB TYPE
	{ 0x07, 1,  0,    5, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 6  - depends on REVERB TYPE
	{ 0x08, 1,  0,    6, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 7  - depends on REVERB TYPE
	{ 0x09, 1,  0,    7, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 8  - depends on REVERB TYPE
	{ 0x0a, 1,  0,    8, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 9  - depends on REVERB TYPE
	{ 0x0b, 1,  0,    9, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 10 - depends on REVERB TYPE
	{ 0x0c, 1,  0,  127, "[Reverb ]Return",        64, nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x0d, 1,  1,  127, "[Reverb ]Pan",           64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x10, 1,  0,   10, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 11 - depends on REVERB TYPE
	{ 0x11, 1,  0,   11, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 12 - depends on REVERB TYPE
	{ 0x12, 1,  0,   12, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 13 - depends on REVERB TYPE
	{ 0x13, 1,  0,   13, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 14 - depends on REVERB TYPE
	{ 0x14, 1,  0,   14, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 15 - depends on REVERB TYPE
	{ 0x15, 1,  0,   15, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // REVERB PARAMETER 16 - depends on REVERB TYPE
	{ 0x20, 2,  0,16383, "[Chorus ]Type",        8320, nullptr,  nullptr,  nullptr,  nullptr  }, // msb=41 lsb=00 (CHORUS 1)
	{ 0x22, 1,  1,    0, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 1  - depends on CHORUS TYPE
	{ 0x23, 1,  1,    1, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 2  - depends on CHORUS TYPE
	{ 0x24, 1,  1,    2, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 3  - depends on CHORUS TYPE
	{ 0x25, 1,  1,    3, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 4  - depends on CHORUS TYPE
	{ 0x26, 1,  1,    4, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 5  - depends on CHORUS TYPE
	{ 0x27, 1,  1,    5, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 6  - depends on CHORUS TYPE
	{ 0x28, 1,  1,    6, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 7  - depends on CHORUS TYPE
	{ 0x29, 1,  1,    7, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 8  - depends on CHORUS TYPE
	{ 0x2a, 1,  1,    8, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 9  - depends on CHORUS TYPE
	{ 0x2b, 1,  1,    9, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 10 - depends on CHORUS TYPE
	{ 0x2c, 1,  0,  127, "[Chorus ]Return",        64, nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x2d, 1,  1,  127, "[Chorus ]Pan",           64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x2e, 1,  0,  127, "[Chorus ]Reverb[ Send]",  0, nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x30, 1,  1,   10, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 11 - depends on CHORUS TYPE
	{ 0x31, 1,  1,   11, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 12 - depends on CHORUS TYPE
	{ 0x32, 1,  1,   12, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 13 - depends on CHORUS TYPE
	{ 0x33, 1,  1,   13, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 14 - depends on CHORUS TYPE
	{ 0x34, 1,  1,   14, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 15 - depends on CHORUS TYPE
	{ 0x35, 1,  1,   15, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // CHORUS PARAMETER 16 - depends on CHORUS TYPE
	{ 0x40, 2,  0,16383, "[Variation ]Type",      640, nullptr,  nullptr,  nullptr,  nullptr  }, // msb=05 lsb=00 (DELAY L,C,R)
	{ 0x42, 2,  2,    0, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 1  - depends on VARIATION TYPE
	{ 0x44, 2,  2,    1, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 2  - depends on VARIATION TYPE
	{ 0x46, 2,  2,    2, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 3  - depends on VARIATION TYPE
	{ 0x48, 2,  2,    3, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 4  - depends on VARIATION TYPE
	{ 0x4a, 2,  2,    4, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 5  - depends on VARIATION TYPE
	{ 0x4c, 2,  2,    5, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 6  - depends on VARIATION TYPE
	{ 0x4e, 2,  2,    6, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 7  - depends on VARIATION TYPE
	{ 0x50, 2,  2,    7, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 8  - depends on VARIATION TYPE
	{ 0x52, 2,  2,    8, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 9  - depends on VARIATION TYPE
	{ 0x54, 2,  2,    9, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 10 - depends on VARIATION TYPE
	{ 0x56, 1,  0,  127, "[Variation ]Return",     64, nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x57, 1,  1,  127, "[Variation ]Pan",        64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x58, 1,  0,  127, "[Variation ]Reverb[ Send]",0,nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x59, 1,  0,  127, "[Variation ]Chorus[ Send]",0,nullptr,  nullptr,  nullptr,  nullptr  }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x5a, 1,  0,    1, "[Variation ]Connection",  0, nullptr,  nullptr,  getsconn, nullptr  },
	{ 0x5b, 1,  0,  127, "[Variation ]Part",      127, nullptr,  nullptr,  getschan, nullptr  }, // part=0..15, 127=OFF
	{ 0x5c, 1,  0,  127, "[Variation ]Wheel[ Depth]",64,getv0x40,getu0x40, nullptr,  nullptr  },
	{ 0x5d, 1,  0,  127, "[Variation ]Bend[ Depth]",64,getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5e, 1,  0,  127, "[Variation ]CAT[ Depth]",64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5f, 1,  0,  127, "[Variation ]AC1[ Depth]",64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x60, 1,  0,  127, "[Variation ]AC2[ Depth]",64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x70, 1,  2,   10, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 11 - depends on VARIATION TYPE
	{ 0x71, 1,  2,   11, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 12 - depends on VARIATION TYPE
	{ 0x72, 1,  2,   12, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 13 - depends on VARIATION TYPE
	{ 0x73, 1,  2,   13, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 14 - depends on VARIATION TYPE
	{ 0x74, 1,  2,   14, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }, // VARIATION PARAMETER 15 - depends on VARIATION TYPE
	{ 0x75, 1,  2,   15, nullptr,                   0, nullptr,  nullptr,  nullptr,  nullptr  }  // VARIATION PARAMETER 16 - depends on VARIATION TYPE
};


// MULTI PART
// Address: 0x08 <part> <id>
//
static
XGParamItem MULTIPARTParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 1,  0,   32, "Element[ Reserve]",       0, nullptr,  nullptr,  nullptr,  nullptr  }, // 0=part10, 2=other
	{ 0x01, 1,  0,  127, "Bank [Select ]MSB",       0, nullptr,  nullptr,  nullptr,  nullptr  }, // 127=part10, 0=other
	{ 0x02, 1,  0,  127, "Bank [Select ]LSB",       0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x03, 1,  0,  127, "Program[ Number]",        0, getvprog, getuprog, nullptr,  nullptr  },
	{ 0x04, 1,  0,  127, "[Rcv ]Channel",           0, nullptr,  nullptr,  getschan, nullptr  }, // 0..15=partno, 127=OFF
	{ 0x05, 1,  0,    1, "Mono/Poly[ Mode]",        1, nullptr,  nullptr,  getsmmod, nullptr  },
	{ 0x06, 1,  0,    2, "Same [Note ]Key[ Assign]",1, nullptr,  nullptr,  getskeya, nullptr  },
	{ 0x07, 1,  0,    3, "[Mode ]Type",             0, nullptr,  nullptr,  getspmod, nullptr  }, // other than part10, 2=part10
	{ 0x08, 1, 40,   88, "[Note ]Shift",           64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x09, 2,  0,  255, "Detune",                128, getv12_7, getu12_7, nullptr,  unit_Hz  },
	{ 0x0b, 1,  0,  127, "Volume",                100, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x0c, 1,  0,  127, "[Velocity Sense ]Depth", 64, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x0d, 1,  0,  127, "[Velocity Sense ]Offset",64, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x0e, 1,  0,  127, "Pan",                    64, getv0x40, getu0x40, nullptr,  nullptr  }, // 0=random
	{ 0x0f, 1,  0,  127, "[Note Limit ]Low",        0, nullptr,  nullptr,  getsnote, nullptr  }, // C-2..G8
	{ 0x10, 1,  0,  127, "[Note Limit ]High",     127, nullptr,  nullptr,  getsnote, nullptr  }, // C-2..G8
	{ 0x11, 1,  0,  127, "Dry/Wet[ Level]",       127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x12, 1,  0,  127, "Chorus[ Send]",           0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x13, 1,  0,  127, "Reverb[ Send]",          40, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x14, 1,  0,  127, "Variation[ Send]",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x15, 1,  0,  127, "[Vibrato ]Rate",         64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x16, 1,  0,  127, "[Vibrato ]Depth",        64, getv0x40, getu0x40, nullptr,  nullptr  }, // drum part ignores
	{ 0x17, 1,  0,  127, "[Vibrato ]Delay",        64, getv0x40, getu0x40, nullptr,  nullptr  }, // drum part ignores
	{ 0x18, 1,  0,  127, "[Filter ]Cutoff[ Freq]", 64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x19, 1,  0,  127, "[Filter ]Resonance",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x1a, 1,  0,  127, "[EG ]Attack[ Time]",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x1b, 1,  0,  127, "[EG ]Decay[ Time]",      64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x1c, 1,  0,  127, "[EG ]Release[ Time]",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x1d, 1, 40,   88, "[Wheel ]Pitch",          64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x1e, 1,  0,  127, "[Wheel ]Filter",         64, getv94_5, getu94_5, nullptr,  nullptr  },
	{ 0x1f, 1,  1,  127, "[Wheel ]Ampl[itude]",    64, getv_100, getu_100, nullptr,  unit_pct },
	{ 0x20, 1,  0,  127, "[Wheel ]LFO Pitch",      10, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x21, 1,  0,  127, "[Whell ]LFO Filter",      0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x22, 1,  0,  127, "[Wheel ]LFO Ampl[itude]", 0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x23, 1, 40,   88, "[Bend ]Pitch",           66, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x24, 1,  0,  127, "[Bend ]Filter",          64, getv94_5, getu94_5, nullptr,  unit_cen },
	{ 0x25, 1,  0,  127, "[Bend ]Ampl[itude]",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x26, 1,  0,  127, "[Bend ]LFO Pitch",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x27, 1,  0,  127, "[Bend ]LFO Filter",       0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x28, 1,  0,  127, "[Bend ]LFO Ampl[itude]",  0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x30, 1,  0,    1, "[Rcv ]P[itch ]B[end]",    1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x31, 1,  0,    1, "[Rcv ]C[han ]A[fter ]T[ouch]",1,nullptr,nullptr, getsonff, nullptr  },
	{ 0x32, 1,  0,    1, "[Rcv ]P[rogram ]C[hange]",1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x33, 1,  0,    1, "[Rcv ]C[ontrol ]C[hange]",1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x34, 1,  0,    1, "[Rcv ]P[oly ]A[fter ]T[ouch]",1,nullptr,nullptr, getsonff, nullptr  },
	{ 0x35, 1,  0,    1, "[Rcv ]Note[ Message]",    1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x36, 1,  0,    1, "[Rcv ]RPN",               1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x37, 1,  0,    1, "[Rcv ]NRPN",              1, nullptr,  nullptr,  getsonff, nullptr  }, // 1=XG, 0=GM
	{ 0x38, 1,  0,    1, "[Rcv ]Mod[ulation]",      1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x39, 1,  0,    1, "[Rcv ]Vol[ume]",          1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3a, 1,  0,    1, "[Rcv ]Pan",               1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3b, 1,  0,    1, "[Rcv ]Expr[ession]",      1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3c, 1,  0,    1, "[Rcv ]Hold[1]",           1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3d, 1,  0,    1, "[Rcv ]Porta[mento]",      1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3e, 1,  0,    1, "[Rcv ]Sost[enuto]",       1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x3f, 1,  0,    1, "[Rcv ]S[oft ]P[edal]",    1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x40, 1,  0,    1, "[Rcv ]B[ank ]S[elect]",   1, nullptr,  nullptr,  getsonff, nullptr  }, // 1=XG, 0=GM
	{ 0x41, 1,  0,  127, "[Scale Tuning ]C",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x42, 1,  0,  127, "[Scale Tuning ]C#",      64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x43, 1,  0,  127, "[Scale Tuning ]D",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x44, 1,  0,  127, "[Scale Tuning ]D#",      64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x45, 1,  0,  127, "[Scale Tuning ]E",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x46, 1,  0,  127, "[Scale Tuning ]F",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x47, 1,  0,  127, "[Scale Tuning ]F#",      64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x48, 1,  0,  127, "[Scale Tuning ]G",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x49, 1,  0,  127, "[Scale Tuning ]G#",      64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x4a, 1,  0,  127, "[Scale Tuning ]A",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x4b, 1,  0,  127, "[Scale Tuning ]A#",      64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x4c, 1,  0,  127, "[Scale Tuning ]B",       64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x4d, 1, 40,   88, "[CAT ]Pitch",            64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x4e, 1,  0,  127, "[CAT ]Filter",           64, getv94_5, getu94_5, nullptr,  nullptr  },
	{ 0x4f, 1,  0,  127, "[CAT ]Ampl[itude]",      64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x50, 1,  0,  127, "[CAT ]LFO Pitch",         0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x51, 1,  0,  127, "[CAT ]LFO Filter",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x52, 1,  0,  127, "[CAT ]LFO Aampl[itude]",  0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x53, 1, 40,   88, "[PAT ]Pitch",            64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x54, 1,  0,  127, "[PAT ]Filter",           64, getv94_5, getu94_5, nullptr,  nullptr  },
	{ 0x55, 1,  0,  127, "[PAT ]Ampl[itude]",      64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x56, 1,  0,  127, "[PAT ]LFO Pitch",         0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x57, 1,  0,  127, "[PAT ]LFO Filter",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x58, 1,  0,  127, "[PAT ]LFO Ampl[itude]",   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x59, 1,  0,   95, "[AC1 ]Controller",       16, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x5a, 1, 40,   88, "[AC1 ]Pitch",            64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x5b, 1,  0,  127, "[AC1 ]Filter",           64, getv94_5, getu94_5, nullptr,  nullptr  },
	{ 0x5c, 1,  0,  127, "[AC1 ]Ampl[itude]",      64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5d, 1,  0,  127, "[AC1 ]LFO Pitch",         0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x5e, 1,  0,  127, "[AC1 ]LFO Filter",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x5f, 1,  0,  127, "[AC1 ]LFO Ampl[itude]",   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x60, 1,  0,   95, "[AC2 ]Controller",       17, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x61, 1, 40,   88, "[AC2 ]Pitch",            64, getv0x40, getu0x40, nullptr,  unit_sem },
	{ 0x62, 1,  0,  127, "[AC2 ]Filter",           64, getv94_5, getu94_5, nullptr,  nullptr  },
	{ 0x63, 1,  0,  127, "[AC2 ]Ampl[itude]",      64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x64, 1,  0,  127, "[AC2 ]LFO Pitch",         0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x65, 1,  0,  127, "[AC2 ]LFO Filter",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x66, 1,  0,  127, "[AC2 ]LFO Ampl[itude]",   0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x67, 1,  0,    1, "[Portamento ]Switch",     0, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x68, 1,  0,  127, "[Portamento ]Time",       0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x69, 1,  0,  127, "[PEG ]Ini[tial] Level",  64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6a, 1,  0,  127, "[PEG ]Att[ack] Time",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6b, 1,  0,  127, "[PEG ]Rel[ease] Level",  64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6c, 1,  0,  127, "[PEG ]Rel[ease] Time",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6d, 1,  1,  127, "[Velocity Limit] Low",    1, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x6e, 1,  1,  127, "[Velocity Limit] High", 127, nullptr,  nullptr,  nullptr,  nullptr  }
};


// DRUM SETUP
// Address: 0x3n <note> <id>   n=0,1 (drumset)
//
static
XGParamItem DRUMSETUPParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 1,  0,  127, "[Pitch ]Coarse",         64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x01, 1,  0,  127, "[Pitch ]Fine",           64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x02, 1,  0,  127, "Level",                 127, nullptr,  nullptr,  nullptr,  nullptr  }, // depend on the note
	{ 0x03, 1,  0,  127, "[Alternate ]Group",       0, nullptr,  nullptr,  nullptr,  nullptr  }, // depend on the note (0=OFF)
	{ 0x04, 1,  0,  127, "Pan",                    64, getv0x40, getu0x40, nullptr,  nullptr  }, // depend on the note (0=random)
	{ 0x05, 1,  0,  127, "Reverb[ Send]",           0, nullptr,  nullptr,  nullptr,  nullptr  }, // depend on the note
	{ 0x06, 1,  0,  127, "Chorus[ Send]",           0, nullptr,  nullptr,  nullptr,  nullptr  }, // depend on the note
	{ 0x07, 1,  0,  127, "Variation[ Send]",      127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x08, 1,  0,    1, "Same [Note ]Key[ Assign]",0, nullptr,  nullptr,  getskeya, nullptr  },
	{ 0x09, 1,  0,    1, "[Rcv ]Note Off",          0, nullptr,  nullptr,  getsonff, nullptr  }, // depend on the note
	{ 0x0a, 1,  0,    1, "[Rcv ]Note On",           1, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x0b, 1,  0,  127, "[Filter ]Cutoff[ Freq]", 64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x0c, 1,  0,  127, "[Filter ]Resonance",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x0d, 1,  0,  127, "[EG ]Attack[ Rate]",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x0e, 1,  0,  127, "[EG ]Decay 1[ Rate]",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x0f, 1,  0,  127, "[EG ]Decay 2[ Rate]",    64, getv0x40, getu0x40, nullptr,  nullptr  } 
};


//-------------------------------------------------------------------------
// XG MIDI Bulk Dump table (USER NORMAL VOICE) [QS300]

// USER VOICE
// Address: 0x11 <user-voice> <id>
//
static
XGParamItem USERVOICEParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	// [Common]
	{ 0x00,10, 32,  127, "[Voice ]Name",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x0b, 1,  1,    3, "Element[ Switch]",        1, nullptr,  nullptr,  getselem, nullptr  }, // 1=Element 1 on, 2=Element 2 on, 3=Element 1 and 2 on
	{ 0x0c, 1,  0,  127, "[Voice ]Level",          64, nullptr,  nullptr,  nullptr,  nullptr  },
	// [Element 1]
	{ 0x3d, 2,  1,  245, "Wave Form",              37, nullptr,  nullptr,  getswave, nullptr  },
	{ 0x3f, 1,  0,  127, "Note[ Limit] Low",        0, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x40, 1,  0,  127, "Note[ Limit] High",     127, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x41, 1,  0,  127, "Vel[ocity Limit] Low",    0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x42, 1,  0,  127, "Vel[ocity Limit] High", 127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x43, 1,  0,    1, "[FEG ]Vel Curve",         0, nullptr,  nullptr,  getsvelc, nullptr  }, // 0=Linear, 1=Exp
	{ 0x44, 1,  0,    2, "[LFO ]Wave[ Select]",     0, nullptr,  nullptr,  getslfow, nullptr  },
	{ 0x45, 1,  0,    1, "[LFO ]Phase[ Initialize]",0, nullptr,  nullptr,  getsonff, nullptr  },
	{ 0x46, 1,  0,   63, "[LFO ]Speed",            32, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x47, 1,  0,  127, "[LFO ]Delay",             0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x48, 1,  0,  127, "[LFO ]Fade[ Time]",       0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x49, 1,  0,   63, "[LFO ]Pitch[ Depth]",     0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x4a, 1,  0,   15, "[LFO ]Cutoff[ Depth]",    0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x4b, 1,  0,   31, "[LFO ]Ampl[itude Depth]", 0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x4c, 1, 32,   96, "[Note ]Shift",           64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x4d, 1, 14,  114, "Detune",                 64, getv0x40, getu0x40, nullptr,  unit_cen },
	{ 0x4e, 1,  0,    5, "[Pitch ]Scaling",         0, nullptr,  nullptr,  getspscl, nullptr  },
	{ 0x4f, 1,  0,  127, "[Pitch ]Center[ Note]",  64, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x50, 1,  0,    3, "[PEG ]Depth",             0, nullptr,  nullptr,  getspdph, nullptr  },
	{ 0x51, 1, 57,   71, "[PEG ]Vel [Sense ]Level",64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x52, 1, 57,   71, "[PEG ]Vel [Sense ]Rate", 64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x53, 1, 57,   71, "[PEG ]Rate[ Scaling]",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x54, 1,  0,  127, "[PEG ]Center[ Note]",    64, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x55, 1,  0,   63, "[PEG ]Rate 1",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x56, 1,  0,   63, "[PEG ]Rate 2",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x57, 1,  0,   63, "[PEG ]Rate 3",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x58, 1,  0,   63, "[PEG ]Rate 4",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x59, 1,  0,  127, "[PEG ]Level 0",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5a, 1,  0,  127, "[PEG ]Level 1",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5b, 1,  0,  127, "[PEG ]Level 2",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5c, 1,  0,  127, "[PEG ]Level 3",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5d, 1,  0,  127, "[PEG ]Level 4",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x5e, 1,  0,   63, "[Filter ]Resonance",     32, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x5f, 1,  0,    7, "Velocity[ Sensitivity]",  0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x60, 1,  0,  127, "Cutoff[ Frequency]",     64, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x61, 1,  0,  127, "[Cutoff Sc ]Break 1",    25, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x62, 1,  0,  127, "[Cutoff Sc ]Break 2",    50, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x63, 1,  0,  127, "[Cutoff Sc ]Break 3",    75, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x64, 1,  0,  127, "[Cutoff Sc ]Break 4",   100, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x65, 1,  0,  127, "[Cutoff Sc ]Offset 1",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x66, 1,  0,  127, "[Cutoff Sc ]Offset 2",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x67, 1,  0,  127, "[Cutoff Sc ]Offset 3",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x68, 1,  0,  127, "[Cutoff Sc ]Offset 4",   64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x69, 1, 57,   71, "[FEG ]Vel [Sense ]Level",64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6a, 1, 57,   71, "[FEG ]Vel [Sense ]Rate", 64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6b, 1, 57,   71, "[FEG Rate ]Scaling",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x6c, 1,  0,  127, "[FEG ]Center[ Note]",    64, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x6d, 1,  0,   63, "[FEG ]Rate 1",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x6e, 1,  0,   63, "[FEG ]Rate 2",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x6f, 1,  0,   63, "[FEG ]Rate 3",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x70, 1,  0,   63, "[FEG ]Rate 4",            0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x71, 1,  0,  127, "[FEG ]Level 0",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x72, 1,  0,  127, "[FEG ]Level 1",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x73, 1,  0,  127, "[FEG ]Level 2",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x74, 1,  0,  127, "[FEG ]Level 3",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x75, 1,  0,  127, "[FEG ]Level 4",          64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x76, 1,  0,  127, "[Element ]Level",       127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x77, 1,  0,  127, "[Level Sc ]Break 1",     25, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x78, 1,  0,  127, "[Level Sc ]Break 2",     50, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x79, 1,  0,  127, "[Level Sc ]Break 3",     75, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x7a, 1,  0,  127, "[Level Sc ]Break 4",    100, nullptr,  nullptr,  getsnote, nullptr  },
	{ 0x7b, 1,  0,  127, "[Level Sc ]Offset 1",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x7c, 1,  0,  127, "[Level Sc ]Offset 2",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x7d, 1,  0,  127, "[Level Sc ]Offset 3",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x7e, 1,  0,  127, "[Level Sc ]Offset 4",    64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x7f, 1,  0,    6, "Vel[ocity] Curve",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x80, 1,  0,   15, "Pan",                     7, nullptr,  nullptr,  getsvpan, nullptr  }, // 0=Left..14=Right, 15=Scaling
	{ 0x81, 1, 57,   71, "[AEG Rate ]Scaling",     64, getv0x40, getu0x40, nullptr,  nullptr  },
	{ 0x82, 1,  0,  127, "[AEG ]Center[ Note]",    64, getv0x40, getu0x40, getsnote, nullptr  },
	{ 0x83, 1,  0,   15, "[AEG Key On ]Delay",      0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x84, 1,  0,   63, "[AEG ]Attack[ Rate]",    63, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x85, 1,  0,   63, "[AEG ]Decay 1[ Rate]",   32, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x86, 1,  0,   63, "[AEG ]Decay 2[ Rate]",   32, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x87, 1,  0,   63, "[AEG ]Release[ Rate]",   32, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x88, 1,  0,  127, "[AEG Decay ]Level 1",   127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x89, 1,  0,  127, "[AEG Decay ]Level 2",   127, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x8a, 2,  0,16383, "[Address ]Offset",        0, nullptr,  nullptr,  nullptr,  nullptr  },
	{ 0x8c, 1, 57,   71, "Resonance[ Sensitivity]",64, getv0x40, getu0x40, nullptr,  nullptr  }
	// [Element 2]
	// ...
};


//-------------------------------------------------------------------------
// XG NRPN Parameter table helpers.
//
typedef
struct _XGRpnParamItem
{
	unsigned short   param; // (N)RPN parameter.
	unsigned short   hi;    // high address.
	unsigned short   lo;    // low address (id).

} XGRpnParamItem;


// NRPN map.
//
static
XGRpnParamItem NRPNParamTab[] =
{	//param  hi    lo

	// MULTIPART NRPN map...
	// Address: 0x08 <part> <id>
	{  136, 0x08, 0x15 }, // Vibrato Rate
	{  137, 0x08, 0x16 }, // Vibrato Depth
	{  138, 0x08, 0x17 }, // Vibrato Delay
	{  160, 0x08, 0x18 }, // Filter Cutoff
	{  161, 0x08, 0x19 }, // Filter Resonance
	{  227, 0x08, 0x20 }, // EG Attack
	{  228, 0x08, 0x21 }, // EG Decay
	{  230, 0x08, 0x22 }, // EG Release

	// DRUMSETUP NRPN map...
	// Address: 0x3n <note> <id>   n=0,1 (drumset)
	{ 2560, 0x30, 0x0b }, // Drum Filter Cutoff
	{ 2688, 0x30, 0x0c }, // Drum Filter Resonance
	{ 2816, 0x30, 0x0e }, // Drum EG Attack
	{ 2944, 0x30, 0x0f }, // Drum EG Decay
	{ 3072, 0x30, 0x00 }, // Drum Pitch Coarse
	{ 3200, 0x30, 0x01 }, // Drum Pitch Fine
	{ 3328, 0x30, 0x02 }, // Drum Level
	{ 3584, 0x30, 0x04 }, // Drum Pan
	{ 3712, 0x30, 0x05 }, // Drum Reverb Send
	{ 3840, 0x30, 0x06 }, // Drum Chorus Send
	{ 3968, 0x30, 0x07 }  // Drum Variation Send
};


//-------------------------------------------------------------------------
// XG Effect table helpers.

static inline
const XGEffectItem *XGEffectItem_find (
	unsigned short etype, XGEffectItem items[], unsigned short nitems )
{
	unsigned short i;

	for (i = 0; i < nitems; ++i) {
		XGEffectItem *item = &items[i];
		if (((item->msb << 7) + item->lsb) == etype)
			return item;
	}

	return nullptr;
}


static inline
const XGEffectItem *REVERBEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		REVERBEffectTab, TSIZE(REVERBEffectTab));
}

static inline
const XGEffectItem *CHORUSEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		CHORUSEffectTab, TSIZE(CHORUSEffectTab));
}

static inline
const XGEffectItem *VARIATIONEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		VARIATIONEffectTab, TSIZE(VARIATIONEffectTab));
}


//-------------------------------------------------------------------------
// XG Effect default helpers.

static inline
unsigned short XGEffectDefault_find (
	unsigned short etype, unsigned short index,
	XGEffectItem items[], unsigned short nitems,
	unsigned short defaults[][16] )
{
	unsigned short i;

	for (i = 0; i < nitems; ++i) {
		XGEffectItem *item = &items[i];
		if (((item->msb << 7) + item->lsb) == etype)
			return defaults[i][index];
	}

	return 0;
}


static inline
unsigned short REVERBEffectDefault (
	unsigned short etype, unsigned short index )
{
	return XGEffectDefault_find(etype, index,
		REVERBEffectTab, TSIZE(REVERBEffectTab),
		REVERBDefaultTab);
}

static inline
unsigned short CHORUSEffectDefault (
	unsigned short etype, unsigned short index )
{
	return XGEffectDefault_find(etype, index,
		CHORUSEffectTab, TSIZE(CHORUSEffectTab),
		CHORUSDefaultTab);
}

static inline
unsigned short VARIATIONEffectDefault (
	unsigned short etype, unsigned short index )
{
	return XGEffectDefault_find(etype, index,
		VARIATIONEffectTab, TSIZE(VARIATIONEffectTab),
		VARIATIONDefaultTab);
}


//-------------------------------------------------------------------------
// XG Parameter table helpers.

static inline
const XGParamItem *XGParamItem_find (
	unsigned short id, XGParamItem items[], unsigned short nitems )
{
	unsigned short i;

	for (i = 0; i < nitems; ++i) {
		XGParamItem *item = &items[i];
		if (item->id == id)
			return item;
	}

	return nullptr;
}


static inline
const XGParamItem *SYSTEMParamItem ( unsigned short id )
{
	return XGParamItem_find(id, SYSTEMParamTab, TSIZE(SYSTEMParamTab));
}

static inline
const XGParamItem *EFFECTParamItem ( unsigned short id )
{
	return XGParamItem_find(id, EFFECTParamTab, TSIZE(EFFECTParamTab));
}

static inline
const XGParamItem *MULTIPARTParamItem ( unsigned short id )
{
	return XGParamItem_find(id, MULTIPARTParamTab, TSIZE(MULTIPARTParamTab));
}

static inline
const XGParamItem *DRUMSETUPParamItem ( unsigned short id )
{
	return XGParamItem_find(id, DRUMSETUPParamTab, TSIZE(DRUMSETUPParamTab));
}

static inline
const XGParamItem *USERVOICEParamItem ( unsigned short id )
{
	return XGParamItem_find(id, USERVOICEParamTab, TSIZE(USERVOICEParamTab));
}


//-------------------------------------------------------------------------
// class XGInstrument - XG Instrument/Normal Voice Group descriptor.
//

// Constructor.
XGInstrument::XGInstrument ( unsigned short id )
	: m_group(nullptr)
{
	if (id < XGInstrument::count())
		m_group = &(InstrumentTab[id]);
}

// Descriptor acessor.
const XGNormalVoiceGroup *XGInstrument::group (void) const
{
	return m_group;
}


// Instrument name.
const char *XGInstrument::name (void) const
{
	return (m_group ? m_group->name : nullptr);
}


// Number of items.
unsigned short XGInstrument::size (void) const
{
	return (m_group ? m_group->size : 0);
}


// Voice index finder.
int XGInstrument::find_voice ( unsigned short bank, unsigned short prog ) const
{
	if (m_group) {
		for (int i = 0; i < m_group->size; ++i) {
			XGNormalVoiceItem *item = &(m_group->items[i]);
			if (item->bank == bank && (item->prog - 1) == prog)
				return i;
		}
	}

	return -1;
}


// Instrument list size (static).
unsigned short XGInstrument::count (void)
{
	return TSIZE(InstrumentTab);
}


//-------------------------------------------------------------------------
// class XGNormalVoice - XG Normal Voice descriptor.
//

// Constructor.
XGNormalVoice::XGNormalVoice ( XGInstrument *instr, unsigned short id )
	: m_item(nullptr)
{
	if (instr && instr->group() && id < instr->size())
		m_item = &((instr->group())->items[id]);
}


// Voice properties accessors.
unsigned short XGNormalVoice::bank (void) const
{
	return (m_item ? m_item->bank : 0);
}

unsigned short XGNormalVoice::prog (void) const
{
	return (m_item ? (m_item->prog - 1): 0);
}

const char *XGNormalVoice::name (void) const
{
	return (m_item ? m_item->name : nullptr);
}

unsigned short XGNormalVoice::elem (void) const
{
	return (m_item ? m_item->elem : 0);
}


//-------------------------------------------------------------------------
// class XGDrumKit - XG Drum Kit descriptor.
//

// Constructor.
XGDrumKit::XGDrumKit ( unsigned short id )
	: m_item(nullptr)
{
	if (id < XGDrumKit::count())
		m_item = &(DrumKitTab[id]);
}

// Descriptor acessor.
const XGDrumKitItem *XGDrumKit::item (void) const
{
	return m_item;
}

// Drum Kit property accessors.
unsigned short XGDrumKit::bank (void) const
{
	return (m_item ? m_item->bank : 0);
}

unsigned short XGDrumKit::prog (void) const
{
	return (m_item ? (m_item->prog - 1): 0);
}

const char *XGDrumKit::name (void) const
{
	return (m_item ? m_item->name : nullptr);
}

unsigned short XGDrumKit::size (void) const
{
	return (m_item ? m_item->size : 0);
}


// Voice index finder.
int XGDrumKit::find_voice ( unsigned short key ) const
{
	if (m_item) {
		for (int i = 0; i < m_item->size; ++i) {
			if (m_item->keys[i].note == key)
				return i;
		}
	}

	return -1;
}


// Drum Kit list size (static).
unsigned short XGDrumKit::count (void)
{
	return TSIZE(DrumKitTab);
}


//-------------------------------------------------------------------------
// class XGDrumVoice - XG Drum Voice descriptor.
//

// Constructor.
XGDrumVoice::XGDrumVoice ( XGDrumKit *drumkit, unsigned short id )
	: m_key(nullptr)
{
	if (drumkit && drumkit->item() && id < drumkit->size())
		m_key = &((drumkit->item())->keys[id]);
}

// Voice properties accessors.
unsigned short XGDrumVoice::note (void) const
{
	return (m_key ? m_key->note : 0);
}

const char *XGDrumVoice::name (void) const
{
	return (m_key ? m_key->name : nullptr);
}


// Default values accessors.
unsigned short XGDrumVoice::level (void) const
{
	return (m_key ? m_key->level : 127);
}

unsigned short XGDrumVoice::group (void) const
{
	return (m_key ? m_key->group : 0);
}

unsigned short XGDrumVoice::pan (void) const
{
	return (m_key ? m_key->pan : 64);
}

unsigned short XGDrumVoice::reverb (void) const
{
	return (m_key ? m_key->reverb : 127);
}

unsigned short XGDrumVoice::chorus (void) const
{
	return (m_key ? m_key->chorus : 127);
}

unsigned short XGDrumVoice::noteOff (void) const
{
	return (m_key ? m_key->noteOff : 0);
}


//-------------------------------------------------------------------------
// class XGParam - XG Generic parameter descriptor.

// Constructor.
XGParam::XGParam ( unsigned short high, unsigned short mid, unsigned short low )
	: m_param(nullptr), m_value(0),
		m_high(high), m_mid(mid), m_low(low), m_busy(false)
{
	if (m_high == 0x00 && m_mid == 0x00) {
		// SYSTEM Parameter Change...
		m_param = SYSTEMParamItem(m_low);
	}
	else
	if (m_high == 0x02 && m_mid == 0x01) {
		// EFFECT Parameter Change...
		m_param = EFFECTParamItem(m_low);
	}
	else
	if (m_high == 0x08) {
		// MULTI PART Parameter Change...
		m_param = MULTIPARTParamItem(m_low);
	}
	else
	if (high == 0x30 || high == 0x31) {
		// DRUM SETUP Parameter Change...
		m_param = DRUMSETUPParamItem(m_low);
	}
	else
	if (high == 0x11) {
		// USER VOICE Parameter Change...
		m_param = USERVOICEParamItem(
			m_low >= 0x3d ? 0x3d + ((m_low - 0x3d) % 0x50) : m_low);
	}

	// Set initial defaults.
	if (m_param)
		m_value = XGParam::def();
}


// Virtual destructor.
XGParam::~XGParam (void)
{
	m_busy = true;

	QListIterator<XGParamObserver *> iter(m_observers);
	while (iter.hasNext())
		iter.next()->set_param(nullptr);

	m_observers.clear();
}


// Address acessors.
unsigned short XGParam::high (void) const
{
	return m_high;
}

unsigned short XGParam::mid (void) const
{
	return m_mid;
}

unsigned short XGParam::low (void) const
{
	return m_low;
}


// Number of bytes needed to encode subject.
unsigned short XGParam::size (void) const
{
	return (m_param ? m_param->size : 0);
}


// Virtual accessors.
const char *XGParam::name (void) const
{
	return (m_param ? m_param->name : nullptr);
}

unsigned short XGParam::min (void) const
{
	return (m_param ? m_param->min : 0);
}

unsigned short XGParam::max (void) const
{
	return (m_param ? m_param->max : 0);
}

unsigned short XGParam::def (void) const
{
	// Take care of very special, hardcoded cases, here...
	if (m_high == 0x08) {
		switch (m_low) {
		case 0x00: // MULTIPART Element Reserve.
			return (m_mid == 9 ? 0x00 : 0x02);
		case 0x01: // MULTIPART Bank MSB.
			return (m_mid == 9 ? 0x7f : 0x00);
		case 0x04: // MULTIPART Channel.
			return m_mid;
		case 0x07: // MULTIPART Part Mode.
			return (m_mid == 9 ? 0x02 : 0x00);
		}
	}

	return (m_param ? m_param->def : 0);
}

float XGParam::getv ( unsigned short u ) const
{
	return (m_param && m_param->getv ? m_param->getv(u) : float(u));
}

unsigned short XGParam::getu ( float v ) const
{
	return (m_param && m_param->getu ? m_param->getu(v) : (unsigned short) (v));
}

const char *XGParam::gets ( unsigned short u ) const
{
	return (m_param && m_param->gets ? m_param->gets(u) : nullptr);
}

const char *XGParam::unit (void) const
{
	return (m_param && m_param->unit ? m_param->unit() : nullptr);
}


// Encode raw data from param value (7bit).
void XGParam::set_data_value ( unsigned char *data, unsigned short u ) const
{
	const unsigned short n = size();
	const unsigned short bits = (n > 2 ? 4 : 7);
	const unsigned short mask = (1 << bits) - 1;
	for (unsigned short i = 0; i < n; ++i)
		data[i] = (u >> (bits * (n - i - 1))) & mask;
}


// Decode param value from raw data (7bit).
unsigned short XGParam::data_value ( unsigned char *data ) const
{
	const unsigned short n = size();
	const unsigned short bits = (n > 2 ? 4 : 7);
	unsigned short ret = 0;
	for (unsigned short i = 0; i < n; ++i)
		ret += (data[i] << (bits * (n - i - 1)));

	return ret;
}


// Encode raw data from param value (4bit).
void XGParam::set_data_value2 ( unsigned char *data, unsigned short u ) const
{
	const unsigned short n = size();
	const unsigned short bits = 4;
	unsigned short mask = (1 << bits) - 1;
	for (unsigned short i = 0; i < n; ++i)
		data[i] = (u >> (bits * (n - i - 1))) & mask;
}


// Decode param value from raw data (4bit).
unsigned short XGParam::data_value2 ( unsigned char *data ) const
{
	const unsigned short n = size();
	const unsigned short bits = 4;
	unsigned short ret = 0;
	for (unsigned short i = 0; i < n; ++i)
		ret += (data[i] << (bits * (n - i - 1)));

	return ret;
}


// Value accessors.
void XGParam::set_value_update ( unsigned short u, XGParamObserver *sender )
{
	if (gets(min()) && !gets(u))
		return;

	m_value = u;

	notify_update(sender);
}

void XGParam::set_value ( unsigned short u, XGParamObserver *sender )
{
	if (m_value == u)
		return;

	set_value_update(u, sender);
}

unsigned short XGParam::value (void) const
{
	return m_value;
}


// Value reset (to default).
void XGParam::reset ( XGParamObserver *sender )
{
	set_value(def(), sender);
}


// Busy flag predicate.
bool XGParam::busy (void) const
{
	return m_busy;
}


// Observer/view resetter.
void XGParam::notify_reset ( XGParamObserver *sender )
{
	if (m_busy)
		return;

	m_busy = true;

	QListIterator<XGParamObserver *> iter(m_observers);
	while (iter.hasNext()) {
		XGParamObserver *observer = iter.next();
		if (sender && sender == observer)
			continue;
		observer->reset();
	}

	m_busy = false;
}


// Observer/view updater.
void XGParam::notify_update ( XGParamObserver *sender )
{
	if (m_busy)
		return;

	m_busy = true;

	QListIterator<XGParamObserver *> iter(m_observers);
	while (iter.hasNext()) {
		XGParamObserver *observer = iter.next();
		if (sender && sender == observer)
			continue;
		observer->update();
	}

	m_busy = false;
}


// Observer list accessors.
void XGParam::attach ( XGParamObserver *observer )
{
	m_observers.append(observer);
}

void XGParam::detach ( XGParamObserver *observer )
{
	m_observers.removeAll(observer);
}


const QList<XGParamObserver *>& XGParam::observers (void) const
{
	return m_observers;
}


// Textual (name parsed) representations.
QString XGParam::label (void) const
{
	QString slabel;

	const char *plabel = name();
	if (plabel) {
		slabel = plabel;
		slabel.remove(QRegularExpression("\\[[^\\]]*\\]"));
	}

	return slabel;
}

QString XGParam::text (void) const
{
	QString stext;

	const char *ptext = name();
	if (ptext) {
		stext = ptext;
		stext.remove('[').remove(']');
		const char *punit = unit();
		if (punit)
			stext += QString(" (%1)").arg(punit);
	}

	return stext;
}


// Value randomizer (p = percent deviation from v).
void XGParam::randomize ( int v, float p )
{
	if (gets(v))
		return;

	const int q = max() - min() + 1;
	if (q > 2) {
		const int r = min() + (::rand() % q);
		set_value(v + int(0.1f * p * float(r - v)));
	}
}

void XGParam::randomize_value ( float p )
{
	randomize(value(), p);
}

void XGParam::randomize_def ( float p )
{
	randomize(def(), p);
}


//-------------------------------------------------------------------------
// class XGEffectParam - XG Effect parameter descriptor.

// Constructor.
XGEffectParam::XGEffectParam (
	unsigned short high, unsigned short mid, unsigned short low,
	unsigned short etype) : XGParam(high, mid, low),
		m_etype(etype), m_eparam(nullptr)
{
	if (m_param && m_param->name == nullptr) {
		const XGEffectItem *effect = nullptr;
		switch (m_param->min) {
		case 0: // REVERB...
			effect = REVERBEffectItem(m_etype);
			break;
		case 1: // CHORUS...
			effect = CHORUSEffectItem(m_etype);
			break;
		case 2: // VARIATION...
			effect = VARIATIONEffectItem(m_etype);
			break;
		}
		if (effect && effect->params)
			m_eparam = &(effect->params[m_param->max]);
	}

	// Re(set) initial defaults.
	if (m_eparam)
		m_value = XGEffectParam::def();
}


// Sub-address accessors.
unsigned short XGEffectParam::etype (void)  const
{
	return m_etype;
}


// Virtual accessors.
const char *XGEffectParam::name (void) const
{
	return (m_eparam ? m_eparam->name : XGParam::name());
}

unsigned short XGEffectParam::min (void) const
{
	return (m_eparam ? m_eparam->min : XGParam::min());
}

unsigned short XGEffectParam::max (void) const
{
	return (m_eparam ? m_eparam->max : XGParam::max());
}

unsigned short XGEffectParam::def (void) const
{
	if (m_param && m_param->name == nullptr) {
		switch (m_param->min) {
			case 0: return REVERBEffectDefault(m_etype, m_param->max);
			case 1: return CHORUSEffectDefault(m_etype, m_param->max);
			case 2: return VARIATIONEffectDefault(m_etype, m_param->max);
		}
	}

	return XGParam::def();
}

float XGEffectParam::getv ( unsigned short u ) const
{
	return (m_eparam && m_eparam->getv ? m_eparam->getv(u) : XGParam::getv(u));
}

unsigned short XGEffectParam::getu ( float v ) const
{
	return (m_eparam && m_eparam->getu ? m_eparam->getu(v) : XGParam::getu(v));
}

const char *XGEffectParam::gets ( unsigned short u ) const
{
	return (m_eparam && m_eparam->gets ? m_eparam->gets(u) : XGParam::gets(u));
}

const char *XGEffectParam::unit (void) const
{
	return (m_eparam && m_eparam->unit ? m_eparam->unit() : XGParam::unit());
}


//-------------------------------------------------------------------------
// class XGDataParam - XG Data parameter descriptor.
//
// Constructor.
XGDataParam::XGDataParam (
	unsigned short high, unsigned short mid, unsigned short low )
	: XGParam(high, mid, low)
{
	unsigned short n = size();
	m_data = new unsigned char [n];
	::memset(m_data, ' ', n);
}

// Destructor.
XGDataParam::~XGDataParam (void)
{
	delete [] m_data;
}


// Data accessors.
void XGDataParam::set_data (
	unsigned char *data, unsigned short len, XGParamObserver *sender )
{
	const unsigned short n = size();
	if (len > n)
		len = n;
	if (data && len > 0)
		::memcpy(m_data, data, len);
	if (len < n)
		::memset(m_data + len, ' ', n - len);

	notify_update(sender);
}

unsigned char *XGDataParam::data (void) const
{
	return m_data;
}


// Data reset (to default).
void XGDataParam::reset ( XGParamObserver *sender )
{
	set_data(nullptr, 0, sender);
}


//-------------------------------------------------------------------------
// class XGParamMap - XG Parameter mapper.
//

// Constructor.
XGParamMap::XGParamMap (void)
	: m_key_param(nullptr), m_key(0), m_elements(0), m_element(0)
{
	m_observer = new XGParamMap::Observer(this);
}


// Destructor.
XGParamMap::~XGParamMap (void)
{
	delete m_observer;

	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter)
		delete iter.value();
}


// Append method.
void XGParamMap::add_param ( XGParam *param, unsigned short key )
{
	XGParamSet *paramset = find_paramset(param->low());
	paramset->insert(key, param);

	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap)
		pMasterMap->add_param_map(param, this);
}


// Map finders.
XGParam *XGParamMap::find_param ( unsigned short id )
{
	XGParamSet *paramset = find_paramset(id);
	if (paramset == nullptr)
		return nullptr;

	unsigned short key = current_key();
	return (paramset->contains(key) ? paramset->value(key) : nullptr);
}


// Key param accessors.
void XGParamMap::set_key_param ( XGParam *param )
{
	if (m_key_param) m_key_param->detach(m_observer);

	m_key_param = param;

	if (m_key_param) m_key_param->attach(m_observer);
}

XGParam *XGParamMap::key_param (void) const
{
	return m_key_param;
}


// Key value accessors.
void XGParamMap::set_current_key ( unsigned short key )
{
	if (m_key_param) {
		m_key_param->set_value(key);
	} else {
		m_key = key;
		notify_reset();
	}
}

unsigned short XGParamMap::current_key () const
{
	return (m_key_param ? m_key_param->value() : m_key);
}


// Param set/factory method.
XGParamSet *XGParamMap::find_paramset ( unsigned short id )
{
	XGParamSet *paramset = nullptr;

	if (XGParamMap::contains(id)) {
		paramset = XGParamMap::value(id);
	} else {
		paramset = new XGParamSet();
		XGParamMap::insert(id, paramset);
	}

	return paramset;
}


// Local observers notify (key change).
void XGParamMap::notify_reset (void)
{
	if (m_key_param)
		m_key = m_key_param->value();

	unsigned short id0 = 0x3d + (m_element * 0x50);
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter) {
		unsigned short id = iter.key();
		if (m_elements > 0 && id >= 0x3d && (id < id0 || id >= id0 + 0x50))
			continue;
		XGParamSet *paramset = iter.value();
		if (paramset->contains(m_key)) {
			XGParam *param = paramset->value(m_key);
			if (param)
				param->notify_reset();
		}
	}
}


// Key/type name stuff.
const XGParamMap::Keys& XGParamMap::keys (void) const
{
	return m_keys;
}

void XGParamMap::add_key ( unsigned short key, const QString& name )
{
	m_keys.insert(key, name);
}


// Special element stride settings (USERVOICE, QS300).
void XGParamMap::set_elements ( unsigned short elements )
{
	m_elements = elements;
}

unsigned short XGParamMap::elements (void) const
{
	return m_elements;
}


// Element key value accessors.
void XGParamMap::set_current_element ( unsigned short element )
{
	m_element = element;

	unsigned short key = current_key();
	unsigned short id0 = 0x3d + (m_element * 0x50);
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter) {
		unsigned short id = iter.key();
		if (id >= id0 && id < id0 + 0x50) {
			XGParamSet *paramset = iter.value();
			if (paramset->contains(key)) {
				XGParam *param = paramset->value(key);
				if (param)
					param->notify_reset();
			}
		}
	}
}

unsigned short XGParamMap::current_element (void) const
{
	return m_element;
}


// All parameter reset (to default)
void XGParamMap::reset ( XGParamObserver *sender )
{
	unsigned short key = current_key();
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter) {
		XGParamSet *paramset = iter.value();
		if (paramset->contains(key)) {
			XGParam *param = paramset->value(key);
			if (param)
				param->reset(sender);
		}
	}
}


// All parameter randomizer (p = percent from value/def).
void XGParamMap::randomize_value ( int p )
{
	unsigned short key = current_key();
	unsigned short id0 = 0x3d + (m_element * 0x50);
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter) {
		unsigned short id = iter.key();
		if (m_elements > 0 && id >= 0x3d && (id < id0 || id >= id0 + 0x50))
			continue;
		XGParamSet *paramset = iter.value();
		if (paramset->contains(key)) {
			XGParam *param = paramset->value(key);
			if (param)
				param->randomize_value(p);
		}
	}
}

void XGParamMap::randomize_def ( int p )
{
	unsigned short key = current_key();
	unsigned short id0 = 0x3d + (m_element * 0x50);
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter) {
		unsigned short id = iter.key();
		if (m_elements > 0 && id >= 0x3d && (id < id0 || id >= id0 + 0x50))
			continue;
		XGParamSet *paramset = iter.value();
		if (paramset->contains(key)) {
			XGParam *param = paramset->value(key);
			if (param)
				param->randomize_def(p);
		}
	}
}


//-------------------------------------------------------------------------
// class XGParamMasterMap - XG Parameter master state database.
//
// Pseudo-singleton reference.
XGParamMasterMap *XGParamMasterMap::g_pParamMasterMap = nullptr;

// Pseudo-singleton accessor (static).
XGParamMasterMap *XGParamMasterMap::getInstance (void)
{
	return g_pParamMasterMap;
}


// Constructor.
XGParamMasterMap::XGParamMasterMap (void)
{
	unsigned short i, j, k;

	// Initialize the randomizer seed...
	::srand(::time(nullptr));

	// XG SYSTEM...
	for (i = 0; i < TSIZE(SYSTEMParamTab); ++i) {
		XGParamItem *item = &SYSTEMParamTab[i];
		XGParam *param = new XGParam(0x00, 0x00, item->id);
		XGParamMasterMap::add_param(param);
		SYSTEM.add_param(param, 0);
	}
	
	// XG EFFECT...
	for (i = 0; i < TSIZE(EFFECTParamTab); ++i) {
		XGParamItem *item = &EFFECTParamTab[i];
		if (item->id > 0x00 && item->id < 0x20) {
			// REVERB...
			for (j = 0; j < TSIZE(REVERBEffectTab); ++j) {
				XGEffectItem  *eitem = &REVERBEffectTab[j];
				unsigned short etype = (eitem->msb << 7) + eitem->lsb;
				XGEffectParam *eparam
					= new XGEffectParam(0x02, 0x01, item->id, etype);
				XGParamMasterMap::add_param(eparam);
				REVERB.add_param(eparam, etype);
			}
		}
		else
		if (item->id > 0x20 && item->id < 0x40) {
			// CHORUS...
			for (j = 0; j < TSIZE(CHORUSEffectTab); ++j) {
				XGEffectItem  *eitem = &CHORUSEffectTab[j];
				unsigned short etype = (eitem->msb << 7) + eitem->lsb;
				XGEffectParam *eparam
					= new XGEffectParam(0x02, 0x01, item->id, etype);
				XGParamMasterMap::add_param(eparam);
				CHORUS.add_param(eparam, etype);
			}
		}
		else
		if (item->id > 0x40 && item->id < 0x80) {
			// VARIATION...
			for (j = 0; j < TSIZE(VARIATIONEffectTab); ++j) {
				XGEffectItem  *eitem = &VARIATIONEffectTab[j];
				unsigned short etype = (eitem->msb << 7) + eitem->lsb;
				XGEffectParam *eparam
					= new XGEffectParam(0x02, 0x01, item->id, etype);
				XGParamMasterMap::add_param(eparam);
				VARIATION.add_param(eparam, etype);
			}
		}
		else {
			// REVERB, CHORUS, VARIATION TYPE...
			XGParam *param = new XGParam(0x02, 0x01, item->id);
			XGParamMasterMap::add_param(param);
			switch (item->id) {
			case 0x00:
				REVERB.set_key_param(param);
				break;
			case 0x20:
				CHORUS.set_key_param(param);
				break;
			case 0x40:
				VARIATION.set_key_param(param);
				break;
			}
		}
	}

	// XG MULTI PART...
	MULTIPART.set_current_key(0);
	for (i = 0; i < TSIZE(MULTIPARTParamTab); ++i) {
		XGParamItem *item = &MULTIPARTParamTab[i];
		for (j = 0; j < 16; ++j) {
			XGParam *param = new XGParam(0x08, j, item->id);
			XGParamMasterMap::add_param(param);
			MULTIPART.add_param(param, j);
		}
	}

	// XG DRUM SETUP...
	DRUMSETUP.set_current_key(36); // Drums 1, Bass Drum (C1).
	for (i = 0; i < TSIZE(DRUMSETUPParamTab); ++i) {
		XGParamItem *item = &DRUMSETUPParamTab[i];
		for (j = 0; j < 2; ++j) {
			for (k = 13; k < 85; ++k) {
				XGParam *param = new XGParam(0x30 + j, k, item->id);
				XGParamMasterMap::add_param(param);
				DRUMSETUP.add_param(param, (j << 7) + k);
			}
		}
	}

	// QS300 USER VOICE...
	USERVOICE.set_elements(2);
	USERVOICE.set_current_key(0); // User 1.
	for (i = 0; i < TSIZE(USERVOICEParamTab); ++i) {
		XGParamItem *item = &USERVOICEParamTab[i];
		for (k = 0; k < 32; ++k) {
			for (j = 0; j < 2; ++j) {
				if (item->id >= 0x3d || j == 0) {
					unsigned short id = item->id + (j * 0x50);
					if (item->size > 4) {
						XGDataParam *param = new XGDataParam(0x11, k, id);
						XGParamMasterMap::add_param(param);
						USERVOICE.add_param(param, k);
					} else {
						XGParam *param = new XGParam(0x11, k, id);
						XGParamMasterMap::add_param(param);
						USERVOICE.add_param(param, k);
					}
				}
			}
		}
	}

	// REVERB key names...
	for (i = 0; i < TSIZE(REVERBEffectTab); ++i) {
		XGEffectItem *item = &REVERBEffectTab[i];
		REVERB.add_key((item->msb << 7) + item->lsb, item->name);
	}

	// CHORUS key names...
	for (i = 0; i < TSIZE(CHORUSEffectTab); ++i) {
		XGEffectItem *item = &CHORUSEffectTab[i];
		CHORUS.add_key((item->msb << 7) + item->lsb, item->name);
	}

	// VARIATION key names...
	for (i = 0; i < TSIZE(VARIATIONEffectTab); ++i) {
		XGEffectItem *item = &VARIATIONEffectTab[i];
		VARIATION.add_key((item->msb << 7) + item->lsb, item->name);
	}

	// NRPN parameter map...
	for (i = 0; i < TSIZE(NRPNParamTab); ++i) {
		XGRpnParamItem *item = &NRPNParamTab[i];
		// MULTIPART NRPN map...
		if (item->hi == 0x08) {
			for (j = 0; j < 16; ++j) { // parts...
				XGParam *param = find_param(item->hi, j, item->lo);
				if (param)
					NRPN.insert(XGRpnParamKey(j, item->param), param);
			}
		}
		else
		// DRUMSETUP NRPN map...
		if (item->hi == 0x30) {
			for (j = 0; j < 2; ++j) { // drumsets...
				for (k = 13; k < 85; ++k) { // notes...
					XGParam *param = find_param(item->hi + j, k, item->lo);
					if (param)
						NRPN.insert(XGRpnParamKey(j, item->param + k), param);
				}
			}
		}
	}

	// Pseudo-singleton set.
	g_pParamMasterMap = this;
}


// Destructor.
XGParamMasterMap::~XGParamMasterMap (void)
{
	// Pseudo-singleton reset.
	g_pParamMasterMap = nullptr;

	XGParamMasterMap::const_iterator iter = XGParamMasterMap::constBegin();
	for (; iter != XGParamMasterMap::constEnd(); ++iter)
		delete iter.value();
}


// Master append method.
void XGParamMasterMap::add_param ( XGParam *param )
{
	XGParamMasterMap::insert(XGParamKey(param), param);
}

// Add widget to map.
void XGParamMasterMap::add_param_map ( XGParam *param, XGParamMap *map )
{
	m_params_map.insert(param, map);
}


// Master map finders.
XGParam *XGParamMasterMap::find_param (
	unsigned short high, unsigned short mid, unsigned short low ) const
{
	unsigned short etype = 0;

	if (high == 0x02 && mid == 0x01) {
		if (low > 0x00 && low < 0x20)
			etype = REVERB.current_key();
		else
		if (low > 0x20 && low < 0x40)
			etype = CHORUS.current_key();
		else
		if (low > 0x40 && low < 0x80)
			etype = VARIATION.current_key();
	}

	return find_param(XGParamKey(high, mid, low), etype);
}


XGParam *XGParamMasterMap::find_param (
	const XGParamKey& key, unsigned short etype ) const
{
	const QList<XGParam *>& list
		= XGParamMasterMap::values(key);
	QListIterator<XGParam *> iter(list);
	while (iter.hasNext()) {
		XGParam *param = iter.next();
		if (key.high() == 0x02 &&
			key.mid()  == 0x01 &&
			key.low()  != 0x00 &&
			key.low()  != 0x20 &&
			key.low()  != 0x40) {
			XGEffectParam *eparam
				= static_cast<XGEffectParam *> (param);
			if (eparam->etype() == etype)
				return param;
		}
		else return param;
	}

	return nullptr;
}


// Find map from param.
XGParamMap *XGParamMasterMap::find_param_map ( XGParam *param ) const
{
	return (m_params_map.contains(param) ? m_params_map.value(param) : nullptr);
}


// end of XGParam.cpp
