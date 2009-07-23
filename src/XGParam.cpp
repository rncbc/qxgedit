// XGParam.cpp
//
/****************************************************************************
   Copyright (C) 2005-2009, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include <cstdio>

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
	return (unsigned short) (v) + 64;
}


//-------------------------------------------------------------------------
//

static
float getv0x7f ( unsigned short c )
{
	return (float) (c - 127);
}

static
unsigned short getu0x7f ( float v )
{
	return (unsigned short) (v) + 127;
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
float getv9450 ( unsigned short c )
{
	return 94.5f * getv0x40(c) / 0.63f;
}

static
unsigned short getu9450 ( float v )
{
	return getu0x40(0.63f * v / 94.5f);
}


//-------------------------------------------------------------------------
//

static
float getv12_7 ( unsigned short c )
{
	return 0.1f * getv0x7f(c);
}

static
unsigned short getu12_7 ( float v )
{
	return getu0x7f(10.0f * v);
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
	return (unsigned short) (10.0f * v) + 1024;
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
	const char *tabrevt[] = { "TypeA", "TypeB" };
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
	const char *tabonff[] = { "OFF", "ON" };
	return tabonff[c];
}


static
const char *getskeya ( unsigned short c )
{
	static
	const char *tabkeya[] = { "SINGLE", "MULTI", "INST(DRUM)" };
	return tabkeya[c];
}


static
const char *getsmmod ( unsigned short c )
{
	static
	const char *tabmmod[] = { "MONO", "POLY" };
	return tabmmod[c];
}


static
const char *getspmod ( unsigned short c )
{
	static
	const char *tabpmod[] = { "NORMAL", "DRUM", "DRUM 1", "DRUM 2" };
	return tabpmod[c];
}

static
const char *getsconn ( unsigned short c )
{
	static
	const char *tabconn[] = { "INSERT", "SYSTEM" };
	return tabconn[c];
}

static
const char *getswave ( unsigned short c )
{
	static
	const char *tabwave[] = { "SAW", "TRI", "S&H" };
	return tabwave[c];
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
	unsigned char      prog;
	const char        *name;
	unsigned char      elem;

} XGNormalVoiceItem;


// Piano
static
XGNormalVoiceItem PianoTab[] = 
{	// bank prog  name      elem
	{    1,   0, "GrandPno",   1 },
	{    1,   1, "GrndPnoK",   1 },
	{    1,  18, "MelloGrP",   1 },
	{    1,  40, "PianoStr",   2 },
	{    1,  41, "Dream",      2 },
	{    2,   0, "BritePno",   1 },
	{    2,   1, "BritPnoK",   1 },
	{    3,   0, "E.Grand",    2 },
	{    3,   1, "ElGrPnoK",   2 },
	{    3,  32, "Det.CP80",   2 },
	{    3,  40, "ElGrPno1",   2 },
	{    3,  41, "ElGrPno2",   2 },
	{    4,   0, "HnkyTonk",   2 },
	{    4,   1, "HnkyTnkK",   2 },
	{    4,   5, "0 E.Piano1", 2 },
	{    4,   1, "El.Pno1K",   1 },
	{    4,  18, "MelloEP1",   2 },
	{    4,  32, "Chor.EP1",   2 },
	{    4,  40, "HardEl.P",   2 },
	{    4,  45, "VX El.P1",   2 },
	{    4,  64, "60sEl.P",    1 },
	{    6,   0, "E.Piano2",   2 },
	{    6,   1, "El.Pno2K",   1 },
	{    6,  32, "Chor.EP2",   2 },
	{    6,  33, "DX Hard",    2 },
	{    6,  34, "DXLegend",   2 },
	{    6,  40, "DX Phase",   2 },
	{    6,  41, "DX+Analg",   2 },
	{    6,  42, "DXKotoEP",   2 },
	{    6,  45, "VX El.P2",   2 },
	{    7,   0, "Harpsi.",    1 },
	{    7,   1, "Harpsi.K",   1 },
	{    7,  25, "Harpsi.2",   2 },
	{    7,  35, "Harpsi.3",   2 },
	{    8,   0, "Clavi.",     2 },
	{    8,   1, "Clavi. K",   1 },
	{    8,  27, "ClaviWah",   2 },
	{    8,  64, "PulseClv",   1 },
	{    8,  65, "PierceCl",   2 }
};

// Chromatic Percussion
static
XGNormalVoiceItem ChromaticPercussionTab[] = 
{	// bank prog  name      elem
	{    9,   0, "Celesta",    1 },
	{   10,   0, "Glocken",    1 },
	{   11,   0, "MusicBox",   2 },
	{   11,  64, "Orgel",      2 },
	{   12,   0, "Vibes",      1 },
	{   12,   1, "VibesK",     1 },
	{   12,  45, "HardVibe",   2 },
	{   13,   0, "Marimba",    1 },
	{   13,   1, "MarimbaK",   1 },
	{   13,  64, "SineMrmb",   2 },
	{   13,  97, "Balafon2",   2 },
	{   13,  98, "Log Drum",   2 },
	{   14,   0, "Xylophon",   1 },
	{   15,   0, "TubulBel",   1 },
	{   15,  96, "ChrchBel",   2 },
	{   15,  97, "Carillon",   2 },
	{   16,   0, "Dulcimer",   1 },
	{   16,  35, "Dulcimr2",   2 },
	{   16,  96, "Cimbalom",   2 },
	{   16,  97, "Santur",     2 }
};

// Organ
static
XGNormalVoiceItem OrganTab[] = 
{	// bank prog  name      elem
	{   17,   0, "DrawOrgn",   1 },
	{   17,  32, "DetDrwOr",   2 },
	{   17,  33, "60sDrOr1",   2 },
	{   17,  34, "60sDrOr2",   2 },
	{   17,  35, "70sDrOr1",   2 },
	{   17,  36, "DrawOrg2",   2 },
	{   17,  37, "60sDrOr3",   2 },
	{   17,  38, "EvenBar",    2 },
	{   17,  40, "16+2\"2/3",  2 },
	{   17,  64, "Organ Ba",   1 },
	{   17,  65, "70sDrOr2",   2 },
	{   17,  66, "CheezOrg",   2 },
	{   17,  67, "DrawOrg3",   2 },
	{   18,   0, "PercOrgn",   1 },
	{   18,  24, "70sPcOr1",   2 },
	{   18,  32, "DetPrcOr",   2 },
	{   18,  33, "LiteOrg",    2 },
	{   18,  37, "PercOrg2",   2 },
	{   19,   0, "RockOrgn",   2 },
	{   19,  64, "RotaryOr",   2 },
	{   19,  65, "SloRotar",   2 },
	{   19,  66, "FstRotar",   2 },
	{   20,   0, "ChrchOrg",   2 },
	{   20,  32, "ChurOrg3",   2 },
	{   20,  35, "ChurOrg2",   2 },
	{   20,  40, "NotreDam",   2 },
	{   20,  64, "OrgFlute",   2 },
	{   20,  65, "TrmOrgFl",   2 },
	{   21,   0, "ReedOrgn",   1 },
	{   21,  40, "Puff Org",   2 },
	{   22,   0, "Acordion",   2 },
	{   22,  32, "AccordIt",   2 },
	{   23,   0, "Harmnica",   1 },
	{   23,  32, "Harmo 2",    2 },
	{   24,   0, "TangoAcd",   2 },
	{   24,  64, "TngoAcd2",   2 }
};

// Guitar
static
XGNormalVoiceItem GuitarTab[] = 
{	// bank prog  name      elem
	{   25,   0, "NylonGtr",   1 },
	{   25,  16, "NylonGt2",   1 },
	{   25,  25, "NylonGt3",   2 },
	{   25,  43, "VelGtHrm",   2 },
	{   25,  96, "Ukulele",    1 },
	{   26,   0, "SteelGtr",   1 },
	{   26,  16, "SteelGt2",   1 },
	{   26,  35, "12StrGtr",   2 },
	{   26,  40, "Nyln&Stl",   2 },
	{   26,  41, "Stl&Body",   2 },
	{   26,  96, "Mandolin",   2 },
	{   27,   0, "Jazz Gtr",   1 },
	{   27,  18, "MelloGtr",   1 },
	{   27,  32, "JazzAmp",    2 },
	{   28,   0, "CleanGtr",   1 },
	{   28,  32, "ChorusGt",   2 },
	{   29,   0, "Mute.Gtr",   1 },
	{   29,  40, "FunkGtr1",   2 },
	{   29,  41, "MuteStlG",   2 },
	{   29,  43, "FunkGtr2",   2 },
	{   29,  45, "Jazz Man",   1 },
	{   30,   0, "Ovrdrive",   1 },
	{   30,  43, "Gt.Pinch",   2 },
	{   31,   0, "Dist.Gtr",   1 },
	{   31,  40, "FeedbkGt",   2 },
	{   31,  41, "FeedbGt2",   2 },
	{   32,   0, "GtrHarmo",   1 },
	{   32,  65, "GtFeedbk",   1 },
	{   32,  66, "GtrHrmo2",   1 }
};

// Bass
static
XGNormalVoiceItem BassTab[] = 
{	// bank prog  name      elem
	{   33,   0, "Aco.Bass",   1 },
	{   33,  40, "JazzRthm",   2 },
	{   33,  45, "VXUprght",   2 },
	{   34,   0, "FngrBass",   1 },
	{   34,  18, "FingrDrk",   2 },
	{   34,  27, "FlangeBa",   2 },
	{   34,  40, "Ba&DstEG",   2 },
	{   34,  43, "FngrSlap",   2 },
	{   34,  45, "FngBass2",   2 },
	{   34,  65, "ModAlem",    2 },
	{   35,   0, "PickBass",   1 },
	{   35,  28, "MutePkBa",   1 },
	{   36,   0, "Fretless",   1 },
	{   36,  32, "Fretles2",   2 },
	{   36,  33, "Fretles3",   2 },
	{   36,  34, "Fretles4",   2 },
	{   36,  96, "SynFretl",   2 },
	{   36,  97, "Smooth",     2 },
	{   37,   0, "SlapBas1",   1 },
	{   37,  27, "ResoSlap",   1 },
	{   37,  32, "PunchThm",   2 },
	{   38,   0, "SlapBas2",   1 },
	{   38,  43, "VeloSlap",   2 },
	{   39,   0, "SynBass1",   1 },
	{   39,  18, "SynBa1Dk",   1 },
	{   39,  20, "FastResB",   1 },
	{   39,  24, "AcidBass",   1 },
	{   39,  35, "Clv Bass",   2 },
	{   39,  40, "TeknoBa",    2 },
	{   39,  64, "Oscar",      2 },
	{   39,  65, "SqrBass",    1 },
	{   39,  66, "RubberBa",   2 },
	{   39,  96, "Hammer",     2 },
	{   40,   0, "SynBass2",   2 },
	{   40,   6, "MelloSB1",   1 },
	{   40,  12, "Seq Bass",   2 },
	{   40,  18, "ClkSynBa",   2 },
	{   40,  19, "SynBa2Dk",   1 },
	{   40,  32, "SmthBa 2",   2 },
	{   40,  40, "ModulrBa",   2 },
	{   40,  41, "DX Bass",    2 },
	{   40,  64, "X WireBa",   2 }
};

// Strings
static
XGNormalVoiceItem StringsTab[] = 
{	// bank prog  name      elem
	{   41,   0, "Violin",     1 },
	{   41,   8, "SlowVln",    1 },
	{   42,   0, "Viola",      1 },
	{   43,   0, "Cello",      1 },
	{   44,   0, "Contrabs",   1 },
	{   45,   0, "Trem.Str",   1 },
	{   45,   8, "SlowTrStr",  1 },
	{   45,  40, "Susp Str",   2 },
	{   46,   0, "Pizz.Str",   1 },
	{   47,   0, "Harp",       1 },
	{   47,  40, "YangChin",   2 },
	{   48,   0, "Timpani",    1 }
};

// Ensemble
static
XGNormalVoiceItem EnsembleTab[] = 
{	// bank prog  name      elem
	{   49,   0, "Strings1",   1 },
	{   49,   3, "S.Strngs",   2 },
	{   49,   8, "SlowStr",    1 },
	{   49,  24, "ArcoStr",    2 },
	{   49,  35, "60sStrng",   2 },
	{   49,  40, "Orchestr",   2 },
	{   49,  41, "Orchstr2",   2 },
	{   49,  42, "TremOrch",   2 },
	{   49,  45, "VeloStr",    2 },
	{   50,   0, "Strings2",   1 },
	{   50,   3, "S.SlwStr",   2 },
	{   50,   8, "LegatoSt",   2 },
	{   50,  40, "Warm Str",   2 },
	{   50,  41, "Kingdom",    2 },
	{   50,  64, "70s Str",    1 },
	{   50,  65, "Str Ens3",   1 },
	{   51,   0, "Syn.Str1",   2 },
	{   51,  27, "ResoStr",    2 },
	{   51,  64, "Syn Str4",   2 },
	{   51,  65, "SS Str",     2 },
	{   52,   0, "Syn.Str2",   2 },
	{   53,   0, "ChoirAah",   1 },
	{   53,   3, "S.Choir",    2 },
	{   53,  16, "Ch.Aahs2",   2 },
	{   53,  32, "MelChoir",   2 },
	{   53,  40, "ChoirStr",   2 },
	{   54,   0, "VoiceOoh",   1 },
	{   55,   0, "SynVoice",   1 },
	{   55,  40, "SynVox2",    2 },
	{   55,  41, "Choral",     2 },
	{   55,  64, "AnaVoice",   1 },
	{   56,   0, "Orch.Hit",   2 },
	{   56,  35, "OrchHit2",   2 },
	{   56,  64, "Impact",     2 }
};

// Brass
static
XGNormalVoiceItem BrassTab[] = 
{	// bank prog  name      elem
	{   57,   0, "Trumpet",    1 },
	{   57,  16, "Trumpet2",   1 },
	{   57,  17, "BriteTrp",   2 },
	{   57,  32, "WarmTrp",    2 },
	{   58,   0, "Trombone",   1 },
	{   58,  18, "Trmbone2",   2 },
	{   59,   0, "Tuba",       1 },
	{   59,  16, "Tuba 2",     1 },
	{   60,   0, "Mute.Trp",   1 },
	{   61,   0, "Fr.Horn",    2 },
	{   61,   6, "FrHrSolo",   2 },
	{   61,  32, "FrHorn2",    1 },
	{   61,  37, "HornOrch",   2 },
	{   62,   0, "BrasSect",   1 },
	{   62,  35, "Tp&TbSec",   2 },
	{   62,  40, "BrssSec2",   2 },
	{   62,  41, "HiBrass",    2 },
	{   62,  42, "MelloBrs",   2 },
	{   63,   0, "SynBras1",   2 },
	{   63,  12, "QuackBr",    2 },
	{   63,  20, "RezSynBr",   2 },
	{   63,  24, "PolyBrss",   2 },
	{   63,  27, "SynBras3",   2 },
	{   63,  32, "JumpBrss",   2 },
	{   63,  45, "AnaVelBr",   2 },
	{   63,  64, "AnaBrss1",   2 },
	{   64,   0, "SynBras2",   1 },
	{   64,  18, "Soft Brs",   2 },
	{   64,  40, "SynBras4",   2 },
	{   64,  41, "ChorBrss",   2 },
	{   64,  45, "VelBras2",   2 },
	{   64,  64, "AnaBras2",   2 }
};

// Reed
static
XGNormalVoiceItem ReedTab[] = 
{	// bank prog  name      elem
	{   65,   0, "SprnoSax",   1 },
	{   66,   0, "Alto Sax",   1 },
	{   66,  40, "Sax Sect",   2 },
	{   66,  43, "HyprAlto",   2 },
	{   67,   0, "TenorSax",   1 },
	{   67,  40, "BrthTnSx",   2 },
	{   67,  41, "SoftTenr",   2 },
	{   67,  64, "TnrSax 2",   1 },
	{   68,   0, "Bari.Sax",   1 },
	{   69,   0, "Oboe",       2 },
	{   70,   0, "Eng.Horn",   1 },
	{   71,   0, "Bassoon",    1 },
	{   72,   0, "Clarinet",   1 }
};

// Pipe
static
XGNormalVoiceItem PipeTab[] = 
{	// bank prog  name      elem
	{   73,   0, "Piccolo",    1 },
	{   74,   0, "Flute",      1 },
	{   75,   0, "Recorder",   1 },
	{   76,   0, "PanFlute",   1 },
	{   77,   0, "Bottle",     2 },
	{   78,   0, "Shakhchi",   2 },
	{   79,   0, "Whistle",    1 },
	{   80,   0, "Ocarina",    1 }
};

// Synth Lead
static
XGNormalVoiceItem SynthLeadTab[] = 
{	// bank prog  name      elem
	{   81,   0, "SquareLd",   2 },
	{   81,   6, "Square 2",   1 },
	{   81,   8, "LMSquare",   2 },
	{   81,  18, "Hollow",     1 },
	{   81,  19, "Shmoog",     2 },
	{   81,  64, "Mellow",     2 },
	{   81,  65, "SoloSine",   2 },
	{   81,  66, "SineLead",   1 },
	{   82,   0, "Saw.Lead",   2 },
	{   82,   6, "Saw 2",      1 },
	{   82,   8, "ThickSaw",   2 },
	{   82,  18, "DynaSaw",    1 },
	{   82,  19, "DigiSaw",    2 },
	{   82,  20, "Big Lead",   2 },
	{   82,  24, "HeavySyn",   2 },
	{   82,  25, "WaspySyn",   2 },
	{   82,  40, "PulseSaw",   2 },
	{   82,  41, "Dr. Lead",   2 },
	{   82,  45, "VeloLead",   2 },
	{   82,  96, "Seq Ana",    2 },
	{   83,   0, "CaliopLd",   2 },
	{   83,  65, "Pure Pad",   2 },
	{   84,   0, "Chiff Ld",   2 },
	{   84,  64, "Rubby",      2 },
	{   85,   0, "CharanLd",   2 },
	{   85,  64, "DistLead",   2 },
	{   85,  65, "WireLead",   2 },
	{   86,   0, "Voice Ld",   2 },
	{   86,  24, "SynthAah",   2 },
	{   86,  64, "VoxLead",    2 },
	{   87,   0, "Fifth Ld",   2 },
	{   87,  35, "Big Five",   2 },
	{   88,   0, "Bass &Ld",   2 },
	{   88,  16, "Big&Low",    2 },
	{   88,  64, "Fat&Prky",   2 },
	{   88,  65, "SoftWurl",   2 }
};

// Synth Pad
static
XGNormalVoiceItem SynthPadTab[] = 
{	// bank prog  name      elem
	{   89,   0, "NewAgePd",   2 },
	{   89,  64, "Fantasy2",   2 },
	{   90,   0, "Warm Pad",   2 },
	{   90,  16, "ThickPad",   2 },
	{   90,  17, "Soft Pad",   2 },
	{   90,  18, "SinePad",    2 },
	{   90,  64, "Horn Pad",   2 },
	{   90,  65, "RotarStr",   2 },
	{   91,   0, "PolySyPd",   2 },
	{   91,  64, "PolyPd80",   2 },
	{   91,  65, "ClickPad",   2 },
	{   91,  66, "Ana Pad",    2 },
	{   91,  67, "SquarPad",   2 },
	{   92,   0, "ChoirPad",   2 },
	{   92,  64, "Heaven2",    2 },
	{   92,  66, "Itopia",     2 },
	{   92,  67, "CC Pad",     2 },
	{   93,   0, "BowedPad",   2 },
	{   93,  64, "Glacier",    2 },
	{   93,  65, "GlassPad",   2 },
	{   94,   0, "MetalPad",   2 },
	{   94,  64, "Tine Pad",   2 },
	{   94,  65, "Pan Pad",    2 },
	{   95,   0, "Halo Pad",   2 },
	{   96,   0, "SweepPad",   2 },
	{   96,  20, "Shwimmer",   2 },
	{   96,  27, "Converge",   2 },
	{   96,  64, "PolarPad",   2 },
	{   96,  66, "Celstial",   2 }
};

// Synth Effects
static
XGNormalVoiceItem SynthEffectsTab[] = 
{	// bank prog  name      elem
	{   97,   0, "Rain",       2 },
	{   97,  45, "ClaviPad",   2 },
	{   97,  64, "HrmoRain",   2 },
	{   97,  65, "AfrcnWnd",   2 },
	{   97,  66, "Caribean",   2 },
	{   98,   0, "SoundTrk",   2 },
	{   98,  27, "Prologue",   2 },
	{   98,  64, "Ancestrl",   2 },
	{   99,   0, "Crystal",    2 },
	{   99,  12, "SynDrCmp",   2 },
	{   99,  14, "Popcorn",    2 },
	{   99,  18, "TinyBell",   2 },
	{   99,  35, "RndGlock",   2 },
	{   99,  40, "GlockChi",   2 },
	{   99,  41, "ClearBel",   2 },
	{   99,  42, "ChorBell",   2 },
	{   99,  64, "SynMalet",   1 },
	{   99,  65, "SftCryst",   2 },
	{   99,  66, "LoudGlok",   2 },
	{   99,  67, "XmasBell",   2 },
	{   99,  68, "VibeBell",   2 },
	{   99,  69, "DigiBell",   2 },
	{   99,  70, "AirBells",   2 },
	{   99,  71, "BellHarp",   2 },
	{   99,  72, "Gamelmba",   2 },
	{  100,   0, "Atmosphr",   2 },
	{  100,  18, "WarmAtms",   2 },
	{  100,  19, "HollwRls",   2 },
	{  100,  40, "NylonEP",    2 },
	{  100,  64, "NylnHarp",   2 },
	{  100,  65, "Harp Vox",   2 },
	{  100,  66, "AtmosPad",   2 },
	{  100,  67, "Planet",     2 },
	{  101,   0, "Bright",     2 },
	{  101,  64, "FantaBel",   2 },
	{  101,  96, "Smokey",     2 },
	{  102,   0, "Goblins",    2 },
	{  102,  64, "GobSyn",     2 },
	{  102,  65, "50sSciFi",   2 },
	{  102,  66, "Ring Pad",   2 },
	{  102,  67, "Ritual",     2 },
	{  102,  68, "ToHeaven",   2 },
	{  102,  70, "Night",      2 },
	{  102,  71, "Glisten",    2 },
	{  102,  96, "BelChoir",   2 },
	{  103,   0, "Echoes",     2 },
	{  103,   8, "EchoPad2",   2 },
	{  103,  14, "Echo Pan",   2 },
	{  103,  64, "EchoBell",   2 },
	{  103,  65, "Big Pan",    2 },
	{  103,  66, "SynPiano",   2 },
	{  103,  67, "Creation",   2 },
	{  103,  68, "Stardust",   2 },
	{  103,  69, "Reso Pan",   2 },
	{  104,   0, "Sci-Fi",     2 },
	{  104,  64, "Starz",      2 }
};

// Ethnic
static
XGNormalVoiceItem EthnicTab[] = 
{	// bank prog  name      elem
	{  105,   0, "Sitar",      1 },
	{  105,  32, "DetSitar",   2 },
	{  105,  35, "Sitar 2",    2 },
	{  105,  96, "Tambra",     2 },
	{  105,  97, "Tamboura",   2 },
	{  106,   0, "Banjo",      1 },
	{  106,  28, "MuteBnjo",   1 },
	{  106,  96, "Rabab",      2 },
	{  106,  97, "Gopichnt",   2 },
	{  106,  98, "Oud",        2 },
	{  107,   0, "Shamisen",   1 },
	{  108,   0, "Koto",       1 },
	{  108,  96, "T. Koto",    2 },
	{  108,  97, "Kanoon",     2 },
	{  109,   0, "Kalimba",    1 },
	{  110,   0, "Bagpipe",    2 },
	{  111,   0, "Fiddle",     1 },
	{  112,   0, "Shanai",     1 },
	{  112,  64, "Shanai2",    1 },
	{  112,  96, "Pungi",      1 },
	{  112,  97, "Hichriki",   2 }
};

// Percussive
static
XGNormalVoiceItem PercussiveTab[] = 
{	// bank prog  name      elem
	{  113,   0, "TnklBell",   2 },
	{  113,  96, "Bonang",     2 },
	{  113,  97, "Gender",     2 },
	{  113,  98, "Gamelan",    2 },
	{  113,  99, "S.Gamlan",   2 },
	{  113, 100, "Rama Cym",   2 },
	{  113, 101, "AsianBel",   2 },
	{  114,   0, "Agogo",      2 },
	{  115,   0, "SteelDrm",   2 },
	{  115,  97, "GlasPerc",   2 },
	{  115,  98, "ThaiBell",   2 },
	{  116,   0, "WoodBlok",   1 },
	{  116,  96, "Castanet",   1 },
	{  117,   0, "TaikoDrm",   1 },
	{  117,  96, "Gr.Cassa",   1 },
	{  118,   0, "MelodTom",   2 },
	{  118,  64, "Mel Tom2",   1 },
	{  118,  65, "Real Tom",   2 },
	{  118,  66, "Rock Tom",   2 },
	{  119,   0, "Syn.Drum",   1 },
	{  119,  64, "Ana Tom",    1 },
	{  119,  65, "ElecPerc",   2 },
	{  120,   0, "RevCymbl",   1 }
};

// SoundEffects
static
XGNormalVoiceItem SoundEffectsTab[] = 
{	// bank prog  name      elem
	{  121,   0, "FretNoiz",   2 },
	{  122,   0, "BrthNoiz",   2 },
	{  123,   0, "Seashore",   2 },
	{  124,   0, "Tweet",      2 },
	{  125,   0, "Telphone",   1 },
	{  126,   0, "Helicptr",   1 },
	{  127,   0, "Applause",   1 },
	{  128,   0, "Gunshot",    1 }
};

// SFX
static
XGNormalVoiceItem SFXTab[] = 
{	// bank prog  name      elem
	{ 8192,   0, "CuttngNz",   1 },
	{ 8192,   1, "CttngNz2",   2 },
	{ 8192,   3, "Str Slap",   1 },
	{ 8192,  16, "Fl.KClik",   1 },
	{ 8192,  32, "Rain",       1 },
	{ 8192,  33, "Thunder",    1 },
	{ 8192,  34, "Wind",       1 },
	{ 8192,  35, "Stream",     2 },
	{ 8192,  36, "Bubble",     2 },
	{ 8192,  37, "Feed",       2 },
	{ 8192,  48, "Dog",        1 },
	{ 8192,  49, "Horse",      1 },
	{ 8192,  50, "Bird 2",     1 },
	{ 8192,  54, "Ghost",      2 },
	{ 8192,  55, "Maou",       2 },
	{ 8192,  64, "Tel.Dial",   1 },
	{ 8192,  65, "DoorSqek",   1 },
	{ 8192,  66, "Door Slam",  1 },
	{ 8192,  67, "Scratch",    1 },
	{ 8192,  68, "Scratch 2",  2 },
	{ 8192,  69, "WindChm",    1 },
	{ 8192,  70, "Telphon2",   1 },
	{ 8192,  80, "CarEngin",   1 },
	{ 8192,  81, "Car Stop",   1 },
	{ 8192,  82, "Car Pass",   1 },
	{ 8192,  83, "CarCrash",   1 },
	{ 8192,  84, "Siren",      2 },
	{ 8192,  85, "Train",      1 },
	{ 8192,  86, "Jetplane",   2 },
	{ 8192,  87, "Starship",   2 },
	{ 8192,  88, "Burst",      2 },
	{ 8192,  89, "Coaster",    2 },
	{ 8192,  90, "SbMarine",   2 },
	{ 8192,  96, "Laughing",   1 },
	{ 8192,  97, "Scream",     1 },
	{ 8192,  98, "Punch",      1 },
	{ 8192,  99, "Heart",      1 },
	{ 8192, 100, "FootStep",   1 },
	{ 8192, 112, "MchinGun",   1 },
	{ 8192, 113, "LaserGun",   2 },
	{ 8192, 114, "Xplosion",   2 },
	{ 8192, 115, "FireWork",   2 }
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
XGNormalVoiceGroup InstrumentTab[] =
{	// name                   items                   size
	{ "Piano",                PianoTab,               TSIZE(PianoTab)               },
	{ "Organ",                OrganTab,               TSIZE(OrganTab)               },
	{ "Guitar",               GuitarTab,              TSIZE(GuitarTab)              },
	{ "Guitar",               GuitarTab,              TSIZE(GuitarTab)              },
	{ "Bass",                 BassTab,                TSIZE(BassTab)                },
	{ "Strings",              StringsTab,             TSIZE(StringsTab)             },
	{ "Ensemble",             EnsembleTab,            TSIZE(EnsembleTab)            },
	{ "Brass",                BrassTab,               TSIZE(BrassTab)               },
	{ "Reed",                 ReedTab,                TSIZE(ReedTab)                },
	{ "Pipe",                 PipeTab,                TSIZE(PipeTab)                },
	{ "Synth Lead",           SynthLeadTab,           TSIZE(SynthLeadTab)           },
	{ "Synth Pad",            SynthPadTab,            TSIZE(SynthPadTab)            },
	{ "Synth Effects",        SynthEffectsTab,        TSIZE(SynthEffectsTab)        },
	{ "Ethnic",               EthnicTab,              TSIZE(EthnicTab)              },
	{ "Percussive",           PercussiveTab,          TSIZE(PercussiveTab)          },
	{ "Chromatic Percussion", ChromaticPercussionTab, TSIZE(ChromaticPercussionTab) },
	{ "Sound Effects",        SoundEffectsTab,        TSIZE(SoundEffectsTab)        },
	{ "SFX",                  SFXTab,                 TSIZE(SFXTab)                 }
};


//---------------------------------------------------------------------
// XG Drum Voice List
//

typedef
struct _XGDrumVoiceItem
{
	unsigned char  note;
	const char    *name;

} XGDrumVoiceItem;


// Standard Kit 
static
XGDrumVoiceItem StandardKitTab[] = 
{
	{ 13, "Surdo Mute"      },
	{ 14, "Surdo Open"      },
	{ 15, "Hi Q"            },
	{ 16, "Whip Slap"       },
	{ 17, "Scratch Push"    },
	{ 18, "Scratch Pull"    },
	{ 19, "Finger Snap"     },
	{ 20, "Click Noise"     },
	{ 21, "Metronome Click" },
	{ 22, "Metronome Bell"  },
	{ 23, "Seq Click L"     },
	{ 24, "Seq Click H"     },
	{ 25, "Brush Tap"       },
	{ 26, "Brush Swirl L"   },
	{ 27, "Brush Slap"      },
	{ 28, "Brush Swirl H"   },
	{ 29, "Snare Roll"      },
	{ 30, "Castanet"        },
	{ 31, "Snare L"         },
	{ 32, "Sticks"          },
	{ 33, "Bass Drum L"     },
	{ 34, "Open Rim Shot"   },
	{ 35, "Bass Drum M"     },
	{ 36, "Bass Drum H"     },
	{ 37, "Side Stick"      },
	{ 38, "Snare M"         },
	{ 39, "Hand Clap"       },
	{ 40, "Snare H"         },
	{ 41, "Floor Tom L"     },
	{ 42, "Hi-Hat Closed"   },
	{ 43, "Floor Tom H"     },
	{ 44, "Hi-Hat Pedal"    },
	{ 45, "Low Tom"         },
	{ 46, "Hi-Hat Open"     },
	{ 47, "Mid Tom L"       },
	{ 48, "Mid Tom H"       },
	{ 49, "Crash Cymbal 1"  },
	{ 50, "High Tom"        },
	{ 51, "Ride Cymbal 1"   },
	{ 52, "Chinese Cymbal"  },
	{ 53, "Ride Cymbal Cup" },
	{ 54, "Tambourine"      },
	{ 55, "Splash Cymbal"   },
	{ 56, "Cowbell"         },
	{ 57, "Crash Cymbal 2"  },
	{ 58, "Vibraslap"       },
	{ 59, "Ride Cymbal 2"   },
	{ 60, "Bongo H"         },
	{ 61, "Bongo L"         },
	{ 62, "Conga H Mute"    },
	{ 63, "Conga H Open"    },
	{ 64, "Conga L"         },
	{ 65, "Timbale H"       },
	{ 66, "Timbale L"       },
	{ 67, "Agogo H"         },
	{ 68, "Agogo L"         },
	{ 69, "Cabasa"          },
	{ 70, "Maracas"         },
	{ 71, "Samba Whistle H" },
	{ 72, "Samba Whistle L" },
	{ 73, "Guiro Short"     },
	{ 74, "Guiro Long"      },
	{ 75, "Claves"          },
	{ 76, "Wood Block H"    },
	{ 77, "Wood Block L"    },
	{ 78, "Cuica Mute"      },
	{ 79, "Cuica Open"      },
	{ 80, "Triangle Mute"   },
	{ 81, "Triangle Open"   },
	{ 82, "Shaker"          },
	{ 83, "Jingle Bell"     },
	{ 84, "Bell Tree"       }
};

// Standard2 Kit 
static
XGDrumVoiceItem Standard2KitTab[] = 
{ 
	{ 29, "Snare Roll 2"    },
	{ 31, "Snare L 2"       },
	{ 34, "Open Rim Shot 2" },
	{ 35, "Bass Drum M 2"   },
	{ 36, "Bass Drum H 2"   },
	{ 38, "Snare M 2"       },
	{ 40, "Snare H 2"       }
}; 

// Room Kit 
static
XGDrumVoiceItem RoomKitTab[] = 
{
	{ 38, "SD Room L"       },
	{ 40, "SD Room H"       },
	{ 41, "Room Tom 1"      },
	{ 43, "Room Tom 2"      },
	{ 45, "Room Tom 3"      },
	{ 47, "Room Tom 4"      },
	{ 48, "Room Tom 5"      },
	{ 50, "Room Tom 6"      }

};

// Rock Kit 
static
XGDrumVoiceItem RockKitTab[] = 
{
	{ 31, "SD Rock M"       },
	{ 33, "Bass Drum M"     },
	{ 34, "Open Rim Shot 2" },
	{ 35, "Bass Drum H 3"   },
	{ 36, "BD Rock"         },
	{ 38, "SD Rock"         },
	{ 40, "SD Rock Rim"     },
	{ 41, "Rock Tom 1"      },
	{ 43, "Rock Tom 2"      },
	{ 45, "Rock Tom 3"      },
	{ 47, "Rock Tom 4"      },
	{ 48, "Rock Tom 5"      },
	{ 50, "Rock Tom 6"      }
};

// Electro Kit 
static
XGDrumVoiceItem ElectroKitTab[] = 
{
	{ 28, "Reverse Cymbal"  },
	{ 30, "Hi Q"            },
	{ 31, "Snare M"         },
	{ 33, "Bass Drum H 4"   },
	{ 35, "BD Rock"         },
	{ 36, "BD Gate"         },
	{ 38, "SD Rock L"       },
	{ 40, "SD Rock H"       },
	{ 41, "E Tom 1"         },
	{ 43, "E Tom 2"         },
	{ 45, "E Tom 3"         },
	{ 47, "E Tom 4"         },
	{ 48, "E Tom 5"         },
	{ 50, "E Tom 6"         },
	{ 78, "Scratch Push"    },
	{ 79, "Scratch Pull"    } 
};

// Analog Kit 
static
XGDrumVoiceItem AnalogKitTab[] = 
{
	{ 28, "Reverse Cymbal"  },
	{ 30, "Hi Q"            },
	{ 31, "SD Rock H"       },
	{ 33, "Bass Drum M"     },
	{ 35, "BD Analog L"     },
	{ 36, "BD Analog H"     },
	{ 37, "Analog Side Stick" },
	{ 38, "Analog Snare L"  },
	{ 40, "Analog Snare H"  },
	{ 41, "Analog Tom 1"    },
	{ 42, "Analog HH Closed 1" },
	{ 43, "Analog Tom 2"    },
	{ 44, "Analog HH Closed 2" },
	{ 45, "Analog Tom 3"    },
	{ 46, "Analog HH Open"  },
	{ 47, "Analog Tom 4"    },
	{ 48, "Analog Tom 5"    },
	{ 49, "Analog Cymbal"   },
	{ 50, "Analog Tom 6"    },
	{ 56, "Analog Cowbell"  },
	{ 62, "Analog Conga H"  },
	{ 63, "Analog Conga M"  },
	{ 64, "Analog Conga L"  },
	{ 70, "Analog Maracas"  },
	{ 75, "Analog Claves"   },
	{ 78, "Scratch Push"    },
	{ 79, "Scratch Pull"    }
};

// Jazz Kit 
static
XGDrumVoiceItem JazzKitTab[] = 
{
	{ 36, "BD Jazz"         },
	{ 41, "Jazz Tom 1"      },
	{ 43, "Jazz Tom 2"      },
	{ 45, "Jazz Tom 3"      },
	{ 47, "Jazz Tom 4"      },
	{ 48, "Jazz Tom 5"      },
	{ 50, "Jazz Tom 6"      }
};

// Brush Kit 
static
XGDrumVoiceItem BrushKitTab[] = 
{
	{ 31, "Brush Slap L"    },
	{ 36, "BD Soft"         },
	{ 38, "Brush Slap M"    },
	{ 40, "Brush Tap H"     },
	{ 41, "Brush Tom 1"     },
	{ 43, "Brush Tom 2"     },
	{ 45, "Brush Tom 3"     },
	{ 47, "Brush Tom 4"     },
	{ 48, "Brush Tom 5"     },
	{ 50, "Brush Tom 6"     }
};

// Classic Kit 
static
XGDrumVoiceItem ClassicKitTab[] = 
{
	{ 33, "Bass Drum L2"    },
	{ 35, "Gran Cassa"      },
	{ 36, "Gran Cassa Mute" },
	{ 38, "Marching Sn M"   },
	{ 40, "Marching Sn H"   },
	{ 41, "Jazz Tom 1"      },
	{ 43, "Jazz Tom 2"      },
	{ 45, "Jazz Tom 3"      },
	{ 47, "Jazz Tom 4"      },
	{ 48, "Jazz Tom 5"      },
	{ 49, "Hand Cym Open L" },
	{ 50, "Jazz Tom 6"      },
	{ 51, "Hand Cym.Closed L" },
	{ 57, "Hand Cym.Open H" },
	{ 59, "Hand Cym.Closed H" }
};

// SFX1 
static
XGDrumVoiceItem SFX1Tab[] = 
{
	{ 36, "Guitar Cutting Noise" },
	{ 37, "Guitar Cutting Noise 2" },
	{ 39, "String Slap"     },
	{ 52, "FL.Key Click"    },
	{ 68, "Rain"            },
	{ 69, "Thunder"         },
	{ 70, "Wind"            },
	{ 71, "Stream"          },
	{ 72, "Bubble"          },
	{ 73, "Feed"            },
	{ 84, "Dog"             },
	{ 85, "Horse Gallop"    },
	{ 86, "Bird 2"          },
	{ 90, "Ghost"           },
	{ 91, "Maou"            }
};

// SFX2
static
XGDrumVoiceItem SFX2Tab[] = 
{
	{ 36, "Dial Tone"       },
	{ 37, "Door Creaking"   },
	{ 38, "Door Slam"       },
	{ 39, "Scratch"         },
	{ 40, "Scratch 2"       },
	{ 41, "Windchime"       },
	{ 42, "Telephone Ring2" },
	{ 52, "Engine Start"    },
	{ 53, "Tire Screech"    },
	{ 54, "Car Passing"     },
	{ 55, "Crash"           },
	{ 56, "Siren"           },
	{ 57, "Train"           },
	{ 58, "Jetplane"        },
	{ 59, "Starship"        },
	{ 60, "Burst Noise"     },
	{ 61, "Coaster"         },
	{ 62, "SbMarine"        },
	{ 68, "Laughing"        },
	{ 69, "Screaming"       },
	{ 70, "Punch"           },
	{ 71, "Heartbeat"       },
	{ 72, "Footsteps"       },
	{ 84, "Machine Gun"     },
	{ 85, "Laser Gun"       },
	{ 86, "Explosion"       },
	{ 87, "FireWork"        }
};


//---------------------------------------------------------------------
// XG Drum Kit List
//

typedef
struct _XGDrumKitItem
{
	unsigned short   bank;
	unsigned char    prog;
	const char      *name;
	XGDrumVoiceItem *keys;
	unsigned short   size;

} XGDrumKitItem;


static
XGDrumKitItem DrumKitTab[] = 
{	// bank prog  name            keys             size
	{ 16256,  1, "Standard Kit",  StandardKitTab,  TSIZE(StandardKitTab)  },
	{ 16256,  2, "Standard2 Kit", Standard2KitTab, TSIZE(Standard2KitTab) },
	{ 16256,  9, "Room Kit",      RoomKitTab,      TSIZE(RoomKitTab)      },
	{ 16256, 17, "Rock Kit",      RockKitTab,      TSIZE(RockKitTab)      },
	{ 16256, 25, "Electro Kit",   ElectroKitTab,   TSIZE(ElectroKitTab)   },
	{ 16256, 26, "Analog Kit",    AnalogKitTab,    TSIZE(AnalogKitTab)    },
	{ 16256, 33, "Jazz Kit",      JazzKitTab,      TSIZE(JazzKitTab)      },
	{ 16256, 41, "Brush Kit",     BrushKitTab,     TSIZE(BrushKitTab)     },
	{ 16256, 49, "Classic Kit",   ClassicKitTab,   TSIZE(ClassicKitTab)   },
	{ 16128,  1, "SFX 1",         SFX1Tab,         TSIZE(SFX1Tab)         },
	{ 16128,  2, "SFX 2",         SFX2Tab,         TSIZE(SFX2Tab)         }
};


//-------------------------------------------------------------------------
// XG Effect Parameter List
//

// HALL1,2, ROOM1,2,3, STAGE1,2, PLATE
static
XGEffectParamItem HALLROOMParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Reverb Time",        0,  69, getvtab4, getutab4, NULL,     unit_ms  },
	{  1, "Diffusion",          0,  10, NULL,     NULL,     NULL,     NULL     },
	{  2, "Initial Delay",      0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{  3, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{  4, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Rev Delay",          0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{ 11, "Density",            0,   4, NULL,     NULL,     NULL,     NULL     },
	{ 12, "Rev/Er Balance",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 13, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{ 14, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// WHITE ROOM, TUNNEL, CANYON, BASEMENT
static
XGEffectParamItem WHITEROOMParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Reverb Time",        0,  69, getvtab4, getutab4, NULL,     unit_ms  },
	{  1, "Diffusion",          0,  10, NULL,     NULL,     NULL,     NULL     },
	{  2, "Initial Delay",      0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{  3, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{  4, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  5, "Width",              0,  37, getvtab8 ,getutab8, NULL,     unit_m   },
	{  6, "Heigt",              0,  73, getvtab8 ,getutab8, NULL,     unit_m   },
	{  7, "Depth",              0, 104, getvtab8 ,getutab8, NULL,     unit_m   },
	{  8, "Wall Vary",          0,  30, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Rev Delay",          0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{ 11, "Density",            0,   4, NULL,     NULL,     NULL,     NULL     },
	{ 12, "Rev/Er Balance",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 13, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{ 14, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// DELAY L,C,R
static
XGEffectParamItem DELAYLCRParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Lch Delay",          1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  1, "Rch Delay",          1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  2, "Cch Delay",          1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  3, "Feedback Delay",     1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  4, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  5, "Cch Level",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  6, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 12, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 13, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 14, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 15, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  }
};

// DELAY L,R
static
XGEffectParamItem DELAYLRParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Lch Delay",          1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  1, "Rch Delay",          1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  2, "Feedback Delay 1",   1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  3, "Feedback Delay 2",   1,7550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  4, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  5, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 12, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 13, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 14, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 15, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  }
};

// ECHO
static
XGEffectParamItem ECHOParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Lch Delay1",         1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  1, "Lch Feedback Level", 0, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  2, "Rch Delay1",         1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  3, "Rch Feedback Level", 1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  4, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{  5, "Lch Delay2",         1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  6, "Rch Delay2",         1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  7, "Delay2 Level",       0, 127, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 12, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 13, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 14, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 15, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  }
};

// CROSS DELAY
static
XGEffectParamItem CROSSDELAYParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "L->R Delay2",        1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  1, "R->L Delay",         1,3550, getv0_10, getu0_10, NULL,     unit_ms  },
	{  2, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  3, "Input Select",       0,   2, NULL,     NULL,     getsisel, NULL     },
	{  4, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 12, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 13, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 14, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 15, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  }
};

// EARLY REF1,2
static
XGEffectParamItem EARLYREFParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Type",               0,   5, NULL,     NULL,     getsreft, NULL     },
	{  1, "Room Size",          0,  44, getvtab6, getutab6, NULL,     unit_m   },
	{  2, "Diffusion",          0,  10, NULL,     NULL,     NULL,     NULL     },
	{  3, "Initial Delay",      0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{  4, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  5, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Liveness",           0,  10, NULL,     NULL,     NULL,     NULL     },
	{ 11, "Density",            0,   3, NULL,     NULL,     NULL,     NULL     },
	{ 12, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
};

// GATE REVERB, REVERSE GATE
static
XGEffectParamItem GATEREVERBParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Type",               0,   1, NULL,     NULL,     getsrevt, NULL     },
	{  1, "Room Size",          0,  44, getvtab6, getutab6, NULL,     unit_m   },
	{  2, "Diffusion",          0,  10, NULL,     NULL,     NULL,     NULL     },
	{  3, "Initial Delay",      0,  63, getvtab5, getutab5, NULL,     unit_ms  },
	{  4, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  5, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Liveness",           0,  10, NULL,     NULL,     NULL,     NULL     },
	{ 11, "Density",            1,   3, NULL,     NULL,     NULL,     NULL     },
	{ 12, "High Damp",          1,  10, getv0_10, getu0_10, NULL,     unit_ms  },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// KARAOKE1,2,3
static
XGEffectParamItem KARAOKEParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Delay Time",         0, 127, getvtab7, getutab7, NULL,     unit_ms  },
	{  1, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  2, "HPF Cutoff Thru",    0,  52, getvtab3, getutab3, NULL,     unit_Hz  },
	{  3, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 11, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 12, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// CHORUS1,2,3,4 CELESTE1,2,3,4
static
XGEffectParamItem CHORUSParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO PM Depth",       0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  3, "Delay Offset",       0, 127, getvtab2, getutab2, NULL,     unit_ms  },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, "LFO AM Depth",       0, 127, NULL,     NULL,     NULL,     NULL     },
	{ 14, "Input Mode",         0,   1, NULL,     NULL,     getsimod, NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// FLANGER1,2,3
static
XGEffectParamItem FLANGERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  3, "Delay Offset",       0,  63, getvtab2, getutab2, NULL,     unit_ms  },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     NULL     },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, "LFO Phase Diff",     4, 124, getv_180, getu_180, NULL,     unit_deg },
	{ 14, "Input Mode",         0,   1, NULL,     NULL,     getsimod, NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// SYMPHONIC
static
XGEffectParamItem SYMPHONICParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "Delay Offset",       0,   0, getvtab2, getutab2, NULL,     unit_ms  },
	{  3, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, NULL,     NULL,     NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// ROTARY SPEAKER
static
XGEffectParamItem ROTSPEAKERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  3, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, NULL,     NULL,     NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// TREMOLO
static
XGEffectParamItem TREMOLOParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "AM Depth",           0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "PM Depth",           0, 127, NULL,     NULL,     NULL,     NULL     },
	{  3, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, "LFO Phase Diff",     4, 124, getv_180, getu_180, NULL,     unit_deg },
	{ 14, "Input Mode",         0,   1, NULL,     NULL,     getsimod, NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// AUTO PAN
static
XGEffectParamItem AUTOPANParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "L/R Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "F/R Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  3, "PAN Direction",      0,   5, NULL,     NULL,     getspand, NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// PHASER1,2
static
XGEffectParamItem PHASERParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "Phase Shift Offset", 0, 127, NULL,     NULL,     NULL,     NULL     },
	{  3, "Feedback Level",     1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, NULL,     NULL,     NULL,     NULL     },
	{ 10, "Stage",              1,  10, NULL,     NULL,     NULL,     NULL     },
	{ 11, "Diffusion",          0,   1, NULL,     NULL,     getsimod, NULL     },
	{ 12, "LFO Phase Diff",     4, 124, getv_180, getu_180, NULL,     unit_deg },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// DISTORTION, OVERDRIVE
static
XGEffectParamItem DISTORTIONParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Drive",              0, 127, NULL,     NULL,     NULL,     NULL     },
	{  1, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  2, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  3, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  4, "Output Level",       0, 127, NULL,     NULL,     NULL,     NULL     },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{  7, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  8, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Edge (Clip Curve)",  0, 127, NULL,     NULL,     NULL,     NULL     },
	{ 11, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 12, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// GUITAR AMP SIMULATOR
static
XGEffectParamItem AMPSIMULParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "Drive",              0, 127, NULL,     NULL,     NULL,     NULL     },
	{  1, "AMP Type",           0,   3, NULL,     NULL,     getsampt, NULL     },
	{  2, "LPF Cutoff",        34,  60, getvtab3, getutab3, NULL,     unit_Hz  },
	{  3, "Output Level",       0, 127, NULL,     NULL,     NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Edge (Clip Curve)",  0, 127, NULL,     NULL,     NULL,     NULL     },
	{ 11, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 12, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// 3-BAND EQ (MONO)
static
XGEffectParamItem MONOEQParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  1, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{  2, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  3, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{  4, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  5, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 10, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 11, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 12, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// 2-BAND EQ (STEREO)
static
XGEffectParamItem STEREOEQParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "EQ Low Freq",        4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  1, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  2, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  3, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  6, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  7, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  8, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  9, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 10, "EQ Mid Freq",       14,  54, getvtab3, getutab3, NULL,     unit_Hz  },
	{ 11, "EQ Mid Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{ 12, "EQ Mid Width",      10, 120, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
};

// AUTO WAH (LFO)
static
XGEffectParamItem AUTOWAHParamTab[] =
{	// id  name               min  max  getv      getu      gets      unit
	{  0, "LFO Freq",           0, 127, getvtab1, getutab1, NULL,     unit_Hz  },
	{  1, "LFO Depth",          0, 127, NULL,     NULL,     NULL,     NULL     },
	{  2, "Cutoff Freq Offset", 0, 127, NULL,     NULL,     NULL,     NULL     },
	{  3, "Resonance",         10, 120, NULL,     NULL,     NULL,     NULL     },
	{  4, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{  5, "EQ Low Frequ",       4,  40, getvtab3, getutab3, NULL,     unit_Hz  },
	{  6, "EQ Low Gain",       52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  7, "EQ High Freq",      28,  58, getvtab3, getutab3, NULL,     unit_Hz  },
	{  8, "EQ High Gain",      52,  76, getv0x40, getu0x40, NULL,     unit_dB  },
	{  9, "Dry/Wet",            1, 127, getv0x40, getu0x40, NULL,     NULL     },
	{ 10, "Drive",              0, 127, NULL,     NULL,     NULL,     NULL     },
	{ 11, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 12, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 13, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 14, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     },
	{ 15, NULL,                 0,   0, NULL,     NULL,     NULL,     NULL     }
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
	{ 0x00, 0x00, "NO EFFECT",      NULL,               REVERBDefaultTab[ 0]    },
	{ 0x01, 0x00, "HALL 1",         HALLROOMParamTab,   REVERBDefaultTab[ 1]    },
	{ 0x01, 0x01, "HALL 2",         HALLROOMParamTab,   REVERBDefaultTab[ 2]    },
	{ 0x02, 0x00, "ROOM 1",         HALLROOMParamTab,   REVERBDefaultTab[ 3]    },
	{ 0x02, 0x01, "ROOM 2",         HALLROOMParamTab,   REVERBDefaultTab[ 4]    },
	{ 0x02, 0x02, "ROOM 3",         HALLROOMParamTab,   REVERBDefaultTab[ 5]    },
	{ 0x03, 0x00, "STAGE 1",        HALLROOMParamTab,   REVERBDefaultTab[ 6]    },
	{ 0x03, 0x01, "STAGE 2",        HALLROOMParamTab,   REVERBDefaultTab[ 7]    },
	{ 0x04, 0x00, "PLATE",          HALLROOMParamTab,   REVERBDefaultTab[ 8]    },
	{ 0x10, 0x00, "WHITE ROOM",     WHITEROOMParamTab,  REVERBDefaultTab[ 9]    },
	{ 0x11, 0x00, "TUNNEL",         WHITEROOMParamTab,  REVERBDefaultTab[10]    },
	{ 0x13, 0x00, "BASEMENT",       WHITEROOMParamTab,  REVERBDefaultTab[11]    }
};

// CHORUS
static
XGEffectItem CHORUSEffectTab[] =
{	// msb   lsb   name             params              defs
	{ 0x00, 0x00, "NO EFFECT",      NULL,               CHORUSDefaultTab[ 0]    },
	{ 0x41, 0x00, "CHORUS 1",       CHORUSParamTab,     CHORUSDefaultTab[ 1]    },
	{ 0x41, 0x01, "CHORUS 2",       CHORUSParamTab,     CHORUSDefaultTab[ 2]    },
	{ 0x41, 0x02, "CHORUS 3",       CHORUSParamTab,     CHORUSDefaultTab[ 3]    },
	{ 0x41, 0x08, "CHORUS 4",       CHORUSParamTab,     CHORUSDefaultTab[ 4]    },
	{ 0x42, 0x00, "CELESTE 1",      CHORUSParamTab,     CHORUSDefaultTab[ 5]    },
	{ 0x42, 0x01, "CELESTE 2",      CHORUSParamTab,     CHORUSDefaultTab[ 6]    },
	{ 0x42, 0x02, "CELESTE 3",      CHORUSParamTab,     CHORUSDefaultTab[ 7]    },
	{ 0x42, 0x08, "CELESTE 4",      CHORUSParamTab,     CHORUSDefaultTab[ 8]    },
	{ 0x43, 0x00, "FLANGER 1",      FLANGERParamTab,    CHORUSDefaultTab[ 9]    },
	{ 0x43, 0x01, "FLANGER 2",      FLANGERParamTab,    CHORUSDefaultTab[10]    },
	{ 0x43, 0x08, "FLANGER 3",      FLANGERParamTab,    CHORUSDefaultTab[11]    }
};

// VARIATION
static
XGEffectItem VARIATIONEffectTab[] =
{	// msb   lsb   name             params              defs
	{ 0x00, 0x00, "NO EFFECT",      NULL,               VARIATIONDefaultTab[ 0] },
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
	{ 0x40, 0x00, "THRU",           NULL,               VARIATIONDefaultTab[20] },
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
// XG Parameter Change tables

// SYSTEM
// Address: 0x00 0x00 <id>
//
static
XGParamItem SYSTEMParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 4,  0, 2047, "MASTER TUNE",          1024, getv1024, getu1024, NULL,     unit_cen },
	{ 0x04, 1,  0,  127, "MASTER VOLUME",         127, NULL,     NULL,     NULL,     NULL     },
	{ 0x06, 1, 40,   88, "TRANSPOSE",              64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x7d, 1,  0,    1, "DRUM SETUP RESET",        0, NULL,     NULL,     NULL,     NULL     }, // Drumset number (0,1)
	{ 0x7e, 1,  0,    0, "XG SYSTEM ON",            0, NULL,     NULL,     NULL,     NULL     }, // 0=XG System ON (receive only)
	{ 0x7f, 1,  0,    0, "ALL PARAMETER RESET",     0, NULL,     NULL,     NULL,     NULL     }  // 0=ON (receive only)
};


// EFFECT
// Address: 0x02 0x01 <id>
//
static
XGParamItem EFFECTParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 2,  0,16383, "REVERB TYPE",           128, NULL,     NULL,     NULL,     NULL     }, // msb=0x01 lsb=0x00 (HALL 1)
	{ 0x02, 1,  0,    0, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 1  - depends on REVERB TYPE
	{ 0x03, 1,  0,    1, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 2  - depends on REVERB TYPE
	{ 0x04, 1,  0,    2, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 3  - depends on REVERB TYPE
	{ 0x05, 1,  0,    3, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 4  - depends on REVERB TYPE
	{ 0x06, 1,  0,    4, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 5  - depends on REVERB TYPE
	{ 0x07, 1,  0,    5, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 6  - depends on REVERB TYPE
	{ 0x08, 1,  0,    6, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 7  - depends on REVERB TYPE
	{ 0x09, 1,  0,    7, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 8  - depends on REVERB TYPE
	{ 0x0a, 1,  0,    8, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 9  - depends on REVERB TYPE
	{ 0x0b, 1,  0,    9, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 10 - depends on REVERB TYPE
	{ 0x0c, 1,  0,  127, "REVERB RETURN",          64, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x0d, 1,  1,  127, "REVERB PAN",             64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x10, 1,  0,   10, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 11 - depends on REVERB TYPE
	{ 0x11, 1,  0,   11, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 12 - depends on REVERB TYPE
	{ 0x12, 1,  0,   12, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 13 - depends on REVERB TYPE
	{ 0x13, 1,  0,   13, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 14 - depends on REVERB TYPE
	{ 0x14, 1,  0,   14, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 15 - depends on REVERB TYPE
	{ 0x15, 1,  0,   15, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // REVERB PARAMETER 16 - depends on REVERB TYPE
	{ 0x20, 2,  0,16383, "CHORUS TYPE",          8320, NULL,     NULL,     NULL,     NULL     }, // msb=41 lsb=00 (CHORUS 1)
	{ 0x22, 1,  1,    0, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 1  - depends on CHORUS TYPE
	{ 0x23, 1,  1,    1, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 2  - depends on CHORUS TYPE
	{ 0x24, 1,  1,    2, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 3  - depends on CHORUS TYPE
	{ 0x25, 1,  1,    3, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 4  - depends on CHORUS TYPE
	{ 0x26, 1,  1,    4, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 5  - depends on CHORUS TYPE
	{ 0x27, 1,  1,    5, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 6  - depends on CHORUS TYPE
	{ 0x28, 1,  1,    6, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 7  - depends on CHORUS TYPE
	{ 0x29, 1,  1,    7, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 8  - depends on CHORUS TYPE
	{ 0x2a, 1,  1,    8, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 9  - depends on CHORUS TYPE
	{ 0x2b, 1,  1,    9, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 10 - depends on CHORUS TYPE
	{ 0x2c, 1,  0,  127, "CHORUS RETURN",          64, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x2d, 1,  1,  127, "CHORUS PAN",             64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x2e, 1,  0,  127, "CHORUS REVERB SEND",      0, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x30, 1,  1,   10, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 11 - depends on CHORUS TYPE
	{ 0x31, 1,  1,   11, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 12 - depends on CHORUS TYPE
	{ 0x32, 1,  1,   12, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 13 - depends on CHORUS TYPE
	{ 0x33, 1,  1,   13, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 14 - depends on CHORUS TYPE
	{ 0x34, 1,  1,   14, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 15 - depends on CHORUS TYPE
	{ 0x35, 1,  1,   15, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // CHORUS PARAMETER 16 - depends on CHORUS TYPE
	{ 0x40, 2,  0,16383, "VARIATION TYPE",        640, NULL,     NULL,     NULL,     NULL     }, // msb=05 lsb=00 (DELAY L,C,R)
	{ 0x42, 2,  2,    0, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 1  - depends on VARIATION TYPE
	{ 0x44, 2,  2,    1, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 2  - depends on VARIATION TYPE
	{ 0x46, 2,  2,    2, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 3  - depends on VARIATION TYPE
	{ 0x48, 2,  2,    3, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 4  - depends on VARIATION TYPE
	{ 0x4a, 2,  2,    4, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 5  - depends on VARIATION TYPE
	{ 0x4c, 2,  2,    5, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 6  - depends on VARIATION TYPE
	{ 0x4e, 2,  2,    6, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 7  - depends on VARIATION TYPE
	{ 0x50, 2,  2,    7, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 8  - depends on VARIATION TYPE
	{ 0x52, 2,  2,    8, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 9  - depends on VARIATION TYPE
	{ 0x54, 2,  2,    9, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 10 - depends on VARIATION TYPE
	{ 0x56, 1,  0,  127, "VARIATION RETURN",       64, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x57, 1,  1,  127, "VARIATION PAN",          64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x58, 1,  0,  127, "VARIATION REVERB SEND",   0, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x59, 1,  0,  127, "VARIATION CHORUS SEND",   0, NULL,     NULL,     NULL,     NULL     }, // -infdB..0dB..+6dB (0..64..127)
	{ 0x5a, 1,  0,    1, "VARIATION CONNECTION",    0, NULL,     NULL,     getsconn, NULL     },
	{ 0x5b, 1,  0,  127, "VARIATION PART",        127, NULL,     NULL,     NULL,     NULL     }, // part=0..15, 127=OFF
	{ 0x5c, 1,  0,  127, "VARIATION MW DEPTH",     64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x5d, 1,  0,  127, "VARIATION BEND DEPTH",   64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x5e, 1,  0,  127, "VARIATION CAT DEPTH",    64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x5f, 1,  0,  127, "VARIATION AC1 DEPTH",    64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x60, 1,  0,  127, "VARIATION AC1 DEPTH",    64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x70, 1,  2,   10, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 11 - depends on VARIATION TYPE
	{ 0x71, 1,  2,   11, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 12 - depends on VARIATION TYPE
	{ 0x72, 1,  2,   12, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 13 - depends on VARIATION TYPE
	{ 0x73, 1,  2,   13, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 14 - depends on VARIATION TYPE
	{ 0x74, 1,  2,   14, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 15 - depends on VARIATION TYPE
	{ 0x75, 1,  2,   15, NULL,                      0, NULL,     NULL,     NULL,     NULL     }, // VARIATION PARAMETER 16 - depends on VARIATION TYPE
};


// MULTI PART
// Address: 0x08 <part> <id>
//
static
XGParamItem MULTIPARTParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 1,  0,   32, "ELEMENT RESERVE",         0, NULL,     NULL,     NULL,     NULL     }, // 0=part10, 2=other
	{ 0x01, 1,  0,  127, "BANK SELECT MSB",         0, NULL,     NULL,     NULL,     NULL     }, // 127=part10, 0=other
	{ 0x02, 1,  0,  127, "BANK SELECT LSB",         0, NULL,     NULL,     NULL,     NULL     },
	{ 0x03, 1,  0,  127, "PROGRAM NUMBER",          0, NULL,     NULL,     NULL,     NULL     },
	{ 0x04, 1,  0,  127, "Rcv CHANNEL",             0, NULL,     NULL,     NULL,     NULL     }, // 0..15=partno, 127=OFF
	{ 0x05, 1,  0,    1, "MONO/POLY MODE",          1, NULL,     NULL,     getsmmod, NULL     },
	{ 0x06, 1,  0,    2, "SAME NOTE KEY ASSIGN",    1, NULL,     NULL,     getskeya, NULL     },
	{ 0x07, 1,  0,    3, "PART MODE",               0, NULL,     NULL,     getspmod, NULL     }, // other than part10, 2=part10
	{ 0x08, 1, 40,   88, "NOTE SHIFT",             64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x09, 2,  0,  255, "DETUNE",                127, getv12_7, getu12_7, NULL,     unit_Hz  },
	{ 0x0b, 1,  0,  127, "VOLUME",                 64, NULL,     NULL,     NULL,     NULL     },
	{ 0x0c, 1,  0,  127, "VELOCITY SENSE DEPTH",   64, NULL,     NULL,     NULL,     NULL     },
	{ 0x0d, 1,  0,  127, "VELOCITY SENSE OFFSET",  64, NULL,     NULL,     NULL,     NULL     },
	{ 0x0e, 1,  0,  127, "PAN",                    64, getv0x40, getu0x40, NULL,     NULL     }, // 0=random
	{ 0x0f, 1,  0,  127, "NOTE LIMIT LOW",          0, NULL,     NULL,     NULL,     NULL     }, // C-2..G8
	{ 0x10, 1,  0,  127, "NOTE LIMIT HIGH",       127, NULL,     NULL,     NULL,     NULL     }, // C-2..G8
	{ 0x11, 1,  0,  127, "DRY LEVEL",             127, NULL,     NULL,     NULL,     NULL     },
	{ 0x12, 1,  0,  127, "CHORUS SEND",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x13, 1,  0,  127, "REVERB SEND",            40, NULL,     NULL,     NULL,     NULL     },
	{ 0x14, 1,  0,  127, "VARIATION SEND",          0, NULL,     NULL,     NULL,     NULL     },
	{ 0x15, 1,  0,  127, "VIBRATO RATE",           64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x16, 1,  0,  127, "VIBRATO DEPTH",          64, getv0x40, getu0x40, NULL,     NULL     }, // drum part ignores
	{ 0x17, 1,  0,  127, "VIBRATO DELAY",          64, getv0x40, getu0x40, NULL,     NULL     }, // drum part ignores
	{ 0x18, 1,  0,  127, "FILTER CUTOFF FREQ",     64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x19, 1,  0,  127, "FILTER RESONANCE",       64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x1a, 1,  0,  127, "EG ATTACK TIME",         64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x1b, 1,  0,  127, "EG DECAY TIME",          64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x1c, 1,  0,  127, "EG RELEASE TIME",        64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x1d, 1, 40,   88, "MW PITCH CONTROL",       64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x1e, 1,  0,  127, "MW FILTER CONTROL",      64, getv9450, getu9450, NULL,     NULL     },
	{ 0x1f, 1,  1,  127, "MW AMPLITUDE CONTROL",   64, getv_100, getu_100, NULL,     unit_pct },
	{ 0x20, 1,  0,  127, "MW LFO PMOD DEPTH",      10, NULL,     NULL,     NULL,     NULL     },
	{ 0x21, 1,  0,  127, "MW LFO FMOD DEPTH",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x22, 1,  0,  127, "MW LFO AMOD DEPTH",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x23, 1, 40,   88, "BEND PITCH CONTROL",     66, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x24, 1,  0,  127, "BEND FILTER CONTROL",    64, getv9450, getu9450, NULL,     unit_cen },
	{ 0x25, 1,  0,  127, "BEND AMPLITUDE CONTROL", 64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x26, 1,  1,  127, "BEND LFO PMOD DEPTH",    64, getv_100, getu_100, NULL,     unit_pct },
	{ 0x27, 1,  1,  127, "BEND LFO FMOD DEPTH",    64, getv_100, getu_100, NULL,     unit_pct },
	{ 0x28, 1,  1,  127, "BEND LFO AMOD DEPTH",    64, getv_100, getu_100, NULL,     unit_pct },
	{ 0x30, 1,  0,    1, "Rcv PITCH BEND",          1, NULL,     NULL,     getsonff, NULL     },
	{ 0x31, 1,  0,    1, "Rcv CH AFTER TOUCH",      1, NULL,     NULL,     getsonff, NULL     },
	{ 0x32, 1,  0,    1, "Rcv PROGRAM CHANGE",      1, NULL,     NULL,     getsonff, NULL     },
	{ 0x33, 1,  0,    1, "Rcv CONTROL CHANGE",      1, NULL,     NULL,     getsonff, NULL     },
	{ 0x34, 1,  0,    1, "Rcv POLY AFTER TOUCH",    1, NULL,     NULL,     getsonff, NULL     },
	{ 0x35, 1,  0,    1, "Rcv NOTE MESSAGE",        1, NULL,     NULL,     getsonff, NULL     },
	{ 0x36, 1,  0,    1, "Rcv RPN",                 1, NULL,     NULL,     getsonff, NULL     },
	{ 0x37, 1,  0,    1, "Rcv NRPN",                1, NULL,     NULL,     getsonff, NULL     }, // 1=XG, 0=GM
	{ 0x38, 1,  0,    1, "Rcv MODULATION",          1, NULL,     NULL,     getsonff, NULL     },
	{ 0x39, 1,  0,    1, "Rcv VOLUME",              1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3a, 1,  0,    1, "Rcv PAN",                 1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3b, 1,  0,    1, "Rcv EXPRESSION",          1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3c, 1,  0,    1, "Rcv HOLD1",               1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3d, 1,  0,    1, "Rcv PORTAMENTO",          1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3e, 1,  0,    1, "Rcv SOSTENUTO",           1, NULL,     NULL,     getsonff, NULL     },
	{ 0x3f, 1,  0,    1, "Rcv SOFT PEDAL",          1, NULL,     NULL,     getsonff, NULL     },
	{ 0x40, 1,  0,    1, "Rcv BANK SELECT",         1, NULL,     NULL,     getsonff, NULL     }, // 1=XG, 0=GM
	{ 0x41, 1,  0,  127, "SCALE TUNING C",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x42, 1,  0,  127, "SCALE TUNING C#",        64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x43, 1,  0,  127, "SCALE TUNING D",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x44, 1,  0,  127, "SCALE TUNING D#",        64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x45, 1,  0,  127, "SCALE TUNING E",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x46, 1,  0,  127, "SCALE TUNING F",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x47, 1,  0,  127, "SCALE TUNING F#",        64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x48, 1,  0,  127, "SCALE TUNING G",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x49, 1,  0,  127, "SCALE TUNING G#",        64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x4a, 1,  0,  127, "SCALE TUNING A",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x4b, 1,  0,  127, "SCALE TUNING A#",        64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x4c, 1,  0,  127, "SCALE TUNING B",         64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x4d, 1, 40,   88, "CAT PITCH CONTROL",      64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x4e, 1,  0,  127, "CAT FILTER CONTROL",     64, getv9450, getu9450, NULL,     NULL     },
	{ 0x4f, 1,  0,  127, "CAT AMPLITUDE CONTROL",  64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x50, 1,  0,  127, "CAT LFO PMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x51, 1,  0,  127, "CAT LFO FMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x52, 1,  0,  127, "CAT LFO AMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x53, 1, 40,   88, "PAT PITCH CONTROL",      64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x54, 1,  0,  127, "PAT FILTER CONTROL",     64, getv9450, getu9450, NULL,     NULL     },
	{ 0x55, 1,  0,  127, "PAT AMPLITUDE CONTROL",  64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x56, 1,  0,  127, "PAT LFO PMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x57, 1,  0,  127, "PAT LFO FMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x58, 1,  0,  127, "PAT LFO AMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x59, 1,  0,   95, "AC1 CONTROLLER NUMBER",  16, NULL,     NULL,     NULL,     NULL     },
	{ 0x5a, 1, 40,   88, "AC1 PITCH CONTROL",      64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x5b, 1,  0,  127, "AC1 FILTER CONTROL",     64, getv9450, getu9450, NULL,     NULL     },
	{ 0x5c, 1,  0,  127, "AC1 AMPLITUDE CONTROL",  64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x5d, 1,  0,  127, "AC1 LFO PMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5e, 1,  0,  127, "AC1 LFO FMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5f, 1,  0,  127, "AC1 LFO AMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x60, 1,  0,   95, "AC2 CONTROLLER NUMBER",  17, NULL,     NULL,     NULL,     NULL     },
	{ 0x61, 1, 40,   88, "AC2 PITCH CONTROL",      64, getv0x40, getu0x40, NULL,     unit_sem },
	{ 0x62, 1,  0,  127, "AC2 FILTER CONTROL",     64, getv9450, getu9450, NULL,     NULL     },
	{ 0x63, 1,  0,  127, "AC2 AMPLITUDE CONTROL",  64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x64, 1,  0,  127, "AC2 LFO PMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x65, 1,  0,  127, "AC2 LFO FMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x66, 1,  0,  127, "AC2 LFO AMOD DEPTH",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x67, 1,  0,    1, "PORTAMENTO SWITCH",       0, NULL,     NULL,     getsonff, NULL     },
	{ 0x68, 1,  0,  127, "PORTAMENTO TIME",         0, NULL,     NULL,     NULL,     NULL     },
	{ 0x69, 1,  0,  127, "PITCH EG INITIAL LEVEL", 64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x6a, 1,  0,  127, "PITCH EG ATTACK TIME",   64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x6b, 1,  0,  127, "PITCH EG RELEASE LEVEL", 64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x6c, 1,  0,  127, "PITCH EG RELEASE TIME",  64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x6d, 1,  1,  127, "VELOCITY LIMIT LOW",      1, NULL,     NULL,     NULL,     NULL     },
	{ 0x6e, 1,  1,  127, "VELOCITY LIMIT HIGH",   127, NULL,     NULL,     NULL,     NULL     }
};


// DRUM SETUP
// Address: 0x3n <note> <id>   n=0,1 (drumset)
//
static
XGParamItem DRUMSETUPParamTab[] =
{	//id size min   max   name                    def  getv      getu      gets      unit
	{ 0x00, 1,  0,  127, "PITCH COARSE",           64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x01, 1,  0,  127, "PITCH FINE",             64, getv0x40, getu0x40, NULL,     unit_cen },
	{ 0x02, 1,  0,  127, "LEVEL",                   0, NULL,     NULL,     NULL,     NULL     }, // depend on the note
	{ 0x03, 1,  0,  127, "ALTERNATE GROUP",         0, NULL,     NULL,     NULL,     NULL     }, // depend on the note (0=OFF)
	{ 0x04, 1,  0,  127, "PAN",                     0, getv0x40, getu0x40, NULL,     NULL     }, // depend on the note (0=random)
	{ 0x05, 1,  0,  127, "REVERB SEND",             0, NULL,     NULL,     NULL,     NULL     }, // depend on the note
	{ 0x06, 1,  0,  127, "CHORUS SEND",             0, NULL,     NULL,     NULL,     NULL     }, // depend on the note
	{ 0x07, 1,  0,  127, "VARIATION SEND",        127, NULL,     NULL,     NULL,     NULL     },
	{ 0x08, 1,  0,    1, "KEY ASSIGN",              0, NULL,     NULL,     getskeya, NULL     },
	{ 0x09, 1,  0,    1, "Rcv NOTE OFF",            0, NULL,     NULL,     getsonff, NULL     }, // depend on the note
	{ 0x0a, 1,  0,    1, "Rcv NOTE ON",             1, NULL,     NULL,     getsonff, NULL     },
	{ 0x0b, 1,  0,  127, "FILTER CUTOFF FREQ",     64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x0c, 1,  0,  127, "FILTER RESONANCE",       64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x0d, 1,  0,  127, "EG ATTACK RATE",         64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x0e, 1,  0,  127, "EG DECAY1 RATE",         64, getv0x40, getu0x40, NULL,     NULL     },
	{ 0x0f, 1,  0,  127, "EG DECAY2 RATE",         64, getv0x40, getu0x40, NULL,     NULL     } 
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
	{ 0x00, 8, 32,  127, "VOICE NAME",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x0b, 1,  1,    3, "ELEMENT SWITCH",          0, NULL,     NULL,     NULL,     NULL     }, // 1=Element 1 on, 2=Element 2 on, 3=Element 1 and 2 on
	{ 0x0c, 1,  0,  127, "VOICE LEVEL",             0, NULL,     NULL,     NULL,     NULL     },
	// [Element 1]
	{ 0x3d, 2,  0,16383, "WAVE NUMBER",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x3f, 1,  0,  127, "NOTE LIMIT LOW",          0, NULL,     NULL,     NULL,     NULL     },
	{ 0x40, 1,  0,  127, "NOTE LIMIT HIGH",         0, NULL,     NULL,     NULL,     NULL     },
	{ 0x41, 1,  0,  127, "VELOCITY LIMIT LOW",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x42, 1,  0,  127, "VELOCITY LIMIT HIGH",     0, NULL,     NULL,     NULL,     NULL     },
	{ 0x43, 1,  0,    1, "FILTER EG VEL CURVE",     0, NULL,     NULL,     NULL,     NULL     },
	{ 0x44, 1,  0,    2, "LFO WAVE SELECT",         0, NULL,     NULL,     getswave, NULL     },
	{ 0x45, 1,  0,    1, "LFO PHASE INITIALIZE",    0, NULL,     NULL,     getsonff, NULL     },
	{ 0x46, 1,  0,   63, "LFO SPEED",               0, NULL,     NULL,     NULL,     NULL     },
	{ 0x47, 1,  0,  127, "LFO DELAY",               0, NULL,     NULL,     NULL,     NULL     },
	{ 0x48, 1,  0,  127, "LFO FADE TIME",           0, NULL,     NULL,     NULL,     NULL     },
	{ 0x49, 1,  0,   63, "LFO PMD DEPTH",           0, NULL,     NULL,     NULL,     NULL     },
	{ 0x4a, 1,  0,   15, "LFO CMD DEPTH",           0, NULL,     NULL,     NULL,     NULL     },
	{ 0x4b, 1,  0,   31, "LFO AMD DEPTH",           0, NULL,     NULL,     NULL,     NULL     },
	{ 0x4c, 1, 32,   96, "NOTE SHIFT",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x4d, 1, 14,  114, "DETUNE",                  0, NULL,     NULL,     NULL,     NULL     },
	{ 0x4e, 1,  0,    5, "PITCH SCALING",           0, NULL,     NULL,     getspscl, NULL     },
	{ 0x4f, 1,  0,  127, "PITCH SC CENTER NOTE",    0, NULL,     NULL,     NULL,     NULL     },
	{ 0x50, 1,  0,    3, "PITCH EG DEPTH",          0, NULL,     NULL,     getspdph, NULL     },
	{ 0x51, 1, 57,   71, "VEL PEG LEVEL SENS",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x52, 1, 57,   71, "VEL PEG RATE SENS",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x53, 1, 57,   71, "PEG RATE SCALING",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x54, 1,  0,  127, "PEG RATE SC CENTER NOTE", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x55, 1,  0,   63, "PEG RATE 1",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x56, 1,  0,   63, "PEG RATE 2",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x57, 1,  0,   63, "PEG RATE 3",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x58, 1,  0,   63, "PEG RATE 4",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x59, 1,  0,  127, "PEG LEVEL 0",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5a, 1,  0,  127, "PEG LEVEL 1",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5b, 1,  0,  127, "PEG LEVEL 2",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5c, 1,  0,  127, "PEG LEVEL 3",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5d, 1,  0,  127, "PEG LEVEL 4",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5e, 1,  0,   63, "FILTER RESONANCE",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x5f, 1,  0,    7, "VELOCITY SENSITIVITY",    0, NULL,     NULL,     NULL,     NULL     },
	{ 0x60, 1,  0,  127, "CUTOFF FREQUENCY",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x61, 1,  0,  127, "CUTOFF SC BREAK POINT 1", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x62, 1,  0,  127, "CUTOFF SC BREAK POINT 2", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x63, 1,  0,  127, "CUTOFF SC BREAK POINT 3", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x64, 1,  0,  127, "CUTOFF SC BREAK POINT 4", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x65, 1,  0,  127, "CUTOFF SC OFFSET 1",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x66, 1,  0,  127, "CUTOFF SC OFFSET 2",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x67, 1,  0,  127, "CUTOFF SC OFFSET 3",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x68, 1,  0,  127, "CUTOFF SC OFFSET 4",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x69, 1, 57,   71, "VEL FEG LEVEL SENS",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6a, 1, 57,   71, "VEL FEG RATE SENS",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6b, 1, 57,   71, "FEG RATE SCALING",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6c, 1,  0,  127, "FEG RATE SC CENTER NOTE", 0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6d, 1,  0,   63, "FEG RATE 1",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6e, 1,  0,   63, "FEG RATE 2",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x6f, 1,  0,   63, "FEG RATE 3",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x70, 1,  0,   63, "FEG RATE 4",              0, NULL,     NULL,     NULL,     NULL     },
	{ 0x71, 1,  0,  127, "FEG LEVEL 0",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x72, 1,  0,  127, "FEG LEVEL 1",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x73, 1,  0,  127, "FEG LEVEL 2",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x74, 1,  0,  127, "FEG LEVEL 3",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x75, 1,  0,  127, "FEG LEVEL 4",             0, NULL,     NULL,     NULL,     NULL     },
	{ 0x76, 1,  0,  127, "ELEMENT LEVEL",           0, NULL,     NULL,     NULL,     NULL     },
	{ 0x77, 1,  0,  127, "LEVEL SC BREAK POINT 1",  0, NULL,     NULL,     NULL,     NULL     },
	{ 0x78, 1,  0,  127, "LEVEL SC BREAK POINT 2",  0, NULL,     NULL,     NULL,     NULL     },
	{ 0x79, 1,  0,  127, "LEVEL SC BREAK POINT 3",  0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7a, 1,  0,  127, "LEVEL SC BREAK POINT 4",  0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7b, 1,  0,  127, "LEVEL SC OFFSET 1",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7c, 1,  0,  127, "LEVEL SC OFFSET 2",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7d, 1,  0,  127, "LEVEL SC OFFSET 3",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7e, 1,  0,  127, "LEVEL SC OFFSET 4",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x7f, 1,  0,    6, "VELOCITY CURVE",          0, NULL,     NULL,     NULL,     NULL     },
	{ 0x80, 1,  0,   15, "PAN",                     0, NULL,     NULL,     NULL,     NULL     }, // 0=Left..14=Right, 15=Scaling
	{ 0x81, 1, 57,   71, "AEG RATE SCALING",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x82, 1,  0,  127, "AEG SC CENTER NOTE",      0, NULL,     NULL,     NULL,     NULL     },
	{ 0x83, 1,  0,   15, "AEG KEY ON DELAY",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x84, 1,  0,  127, "AEG ATTACK RATE",         0, NULL,     NULL,     NULL,     NULL     },
	{ 0x85, 1,  0,  127, "AEG DECAY 1 RATE",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x86, 1,  0,  127, "AEG DECAY 2 RATE",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x87, 1,  0,  127, "AEG RELEASE RATE",        0, NULL,     NULL,     NULL,     NULL     },
	{ 0x88, 1,  0,  127, "AEG DECAY 1 LEVEL",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x89, 1,  0,  127, "AEG DECAY 2 LEVEL",       0, NULL,     NULL,     NULL,     NULL     },
	{ 0x8a, 2,  0,16383, "ADDRESS OFFSET",          0, NULL,     NULL,     NULL,     NULL     },
	{ 0x8c, 1, 57,   71, "RESONANCE SENSITIVITY",   0, NULL,     NULL,     NULL,     NULL     }
	// [Element 2]
	// ...
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

	return NULL;
}


const XGEffectItem *REVERBEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		REVERBEffectTab, TSIZE(REVERBEffectTab));
}

const XGEffectItem *CHORUSEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		CHORUSEffectTab, TSIZE(CHORUSEffectTab));
}

const XGEffectItem *VARIATIONEffectItem ( unsigned short etype )
{
	return XGEffectItem_find(etype,
		VARIATIONEffectTab, TSIZE(VARIATIONEffectTab));
}


//-------------------------------------------------------------------------
// XG Effect default helpers.

static inline
unsigned short XGEffectDefault_find (
	unsigned short etype, unsigned char index,
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


unsigned short REVERBEffectDefault (
	unsigned short etype, unsigned char index )
{
	return XGEffectDefault_find(etype, index,
		REVERBEffectTab, TSIZE(REVERBEffectTab),
		REVERBDefaultTab);
}

unsigned short CHORUSEffectDefault (
	unsigned short etype, unsigned char index )
{
	return XGEffectDefault_find(etype, index,
		CHORUSEffectTab, TSIZE(CHORUSEffectTab),
		CHORUSDefaultTab);
}

unsigned short VARIATIONEffectDefault (
	unsigned short etype, unsigned char index )
{
	return XGEffectDefault_find(etype, index,
		VARIATIONEffectTab, TSIZE(VARIATIONEffectTab),
		VARIATIONDefaultTab);
}


//-------------------------------------------------------------------------
// XG Parameter table helpers.

static inline
const XGParamItem *XGParamItem_find (
	unsigned char id, XGParamItem items[], unsigned short nitems )
{
	unsigned short i;

	for (i = 0; i < nitems; ++i) {
		XGParamItem *item = &items[i];
		if (item->id == id)
			return item;
	}

	return NULL;
}


const XGParamItem *SYSTEMParamItem ( unsigned char id )
{
	return XGParamItem_find(id, SYSTEMParamTab, TSIZE(SYSTEMParamTab));
}

const XGParamItem *EFFECTParamItem ( unsigned char id )
{
	return XGParamItem_find(id, EFFECTParamTab, TSIZE(EFFECTParamTab));
}

const XGParamItem *MULTIPARTParamItem ( unsigned char id )
{
	return XGParamItem_find(id, MULTIPARTParamTab, TSIZE(MULTIPARTParamTab));
}

const XGParamItem *DRUMSETUPParamItem ( unsigned char id )
{
	return XGParamItem_find(id, DRUMSETUPParamTab, TSIZE(DRUMSETUPParamTab));
}


//-------------------------------------------------------------------------
// class XGParam - XG Generic parameter descriptor.

// Constructor.
XGParam::XGParam ( unsigned char high, unsigned char mid, unsigned char low )
	: m_param(NULL), m_high(high), m_mid(mid), m_low(low), m_value(0)
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
}


// Address acessors.
unsigned char XGParam::high (void) const
{
	return m_high;
}

unsigned char XGParam::mid (void) const
{
	return m_mid;
}

unsigned char XGParam::low (void) const
{
	return m_low;
}


// Number of bytes needed to encode subject.
unsigned char XGParam::size (void) const
{
	return (m_param ? m_param->size : 0);
}


// Virtual accessors.
const char *XGParam::name (void) const
{
	return (m_param ? m_param->name : NULL);
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
	return (m_param && m_param->gets ? m_param->gets(u) : NULL);
}

const char *XGParam::unit (void) const
{
	return (m_param && m_param->unit ? m_param->unit() : NULL);
}


// Value accessors.
void XGParam::setValue ( unsigned short c )
{
	m_value = c;
}

unsigned short XGParam::value (void) const
{
	return m_value;
}


// Decode param value from raw data.
unsigned short XGParam::valueFromData ( unsigned char *data ) const
{
	unsigned short bits = 4;
	if (m_high == 0x02 && m_mid == 0x01)
		bits += 3;
	unsigned short ret = 0;
	unsigned short n = size();
	for (unsigned short i = 0; i < n; ++i)
		ret += (data[i] << (bits * (n - i - 1)));

	return ret;
}


//-------------------------------------------------------------------------
// class XGEffectParam - XG Effect parameter descriptor.

// Constructor.
XGEffectParam::XGEffectParam (
	unsigned char high, unsigned char mid, unsigned char low,
	unsigned short etype) : XGParam(high, mid, low),
		m_etype(etype), m_eparam(NULL)
{
	if (m_param && m_param->name == NULL) {
		const XGEffectItem *effect = NULL;
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
	if (m_param && m_param->name == NULL) {
		switch (m_param->min) {
			case 0: return REVERBEffectDefault(m_etype, m_param->max);
			case 1: return CHORUSEffectDefault(m_etype, m_param->max);
			case 2:	return VARIATIONEffectDefault(m_etype, m_param->max);
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
// class XGParamMap - XG Parameter mapper.
//

// Constructor.
XGParamMap::XGParamMap (void)
	: m_key_param(NULL), m_key(0)
{
}


// Destructor.
XGParamMap::~XGParamMap (void)
{
	XGParamMap::const_iterator iter = XGParamMap::constBegin();
	for (; iter != XGParamMap::constEnd(); ++iter)
		delete iter.value();
}


// Append method.
void XGParamMap::add_param ( XGParam *param, unsigned short key )
{
	XGParamSet *paramset = find_paramset(param->low());
	paramset->insert(key, param);
}


// Map finders.
XGParam *XGParamMap::find_param ( unsigned short id )
{
	XGParamSet *paramset = find_paramset(id);
	if (paramset == NULL)
		return NULL;

	unsigned short key = current_key();
	return (paramset->contains(key) ? paramset->value(key) : NULL);
}


// Key param accessors.
void XGParamMap::set_key_param ( XGParam *param )
{
	m_key_param = param;
}

XGParam *XGParamMap::key_param (void) const
{
	return m_key_param;
}


// Key value accessors.
void XGParamMap::set_current_key ( unsigned short key )
{
	if (m_key_param)
		m_key_param->setValue(key);

	m_key = key;
}

unsigned short XGParamMap::current_key () const
{
	return (m_key_param ? m_key_param->value() : m_key);
}


// Param set/factory method.
XGParamSet *XGParamMap::find_paramset ( unsigned short id )
{
	XGParamSet *paramset = NULL;

	if (XGParamMap::contains(id)) {
		paramset = XGParamMap::value(id);
	} else {
		paramset = new XGParamSet();
		XGParamMap::insert(id, paramset);
	}

	return paramset;
}


//-------------------------------------------------------------------------
// class XGParamMasterMap - XG Parameter master state database.
//
// Pseudo-singleton reference.
XGParamMasterMap *XGParamMasterMap::g_pParamMasterMap = NULL;

// Pseudo-singleton accessor (static).
XGParamMasterMap *XGParamMasterMap::getInstance (void)
{
	return g_pParamMasterMap;
}


// Constructor.
XGParamMasterMap::XGParamMasterMap (void)
{
	unsigned short i, j, k;

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
				XGEffectItem  *eitem  = &REVERBEffectTab[j];
				unsigned short etype  = (eitem->msb << 7) + eitem->lsb;
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
				XGEffectItem  *eitem  = &CHORUSEffectTab[j];
				unsigned short etype  = (eitem->msb << 7) + eitem->lsb;
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
				XGEffectItem  *eitem  = &VARIATIONEffectTab[j];
				unsigned short etype  = (eitem->msb << 7) + eitem->lsb;
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
	for (i = 0; i < TSIZE(MULTIPARTParamTab); ++i) {
		XGParamItem *item = &MULTIPARTParamTab[i];
		for (j = 0; j < 16; ++j) {
			XGParam *param = new XGParam(0x08, j, item->id);
			XGParamMasterMap::add_param(param);
			MULTIPART.add_param(param, j);
		}
	}

	// XG DRUM SETUP...
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

	// Pseudo-singleton set.
	g_pParamMasterMap = this;
}


// Destructor.
XGParamMasterMap::~XGParamMasterMap (void)
{
	// Pseudo-singleton reset.
	g_pParamMasterMap = NULL;

	XGParamMasterMap::const_iterator iter = XGParamMasterMap::constBegin();
	for (; iter != XGParamMasterMap::constEnd(); ++iter)
		delete iter.value();
}


// Master append method.
void XGParamMasterMap::add_param ( XGParam *param )
{
	XGParamMasterMap::insertMulti(XGParamKey(param), param);
}


// Master map finders.
XGParam *XGParamMasterMap::find_param (
	unsigned char high, unsigned char mid, unsigned char low ) const
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
	XGParamMasterMap::const_iterator iter = XGParamMasterMap::constFind(key);
	for (; iter != XGParamMasterMap::constEnd() && iter.key() == key; ++iter) {
		XGParam *param = iter.value();
		if (key.high() == 0x02 && key.mid() == 0x01 && etype > 0) {
			XGEffectParam *eparam
				= static_cast<XGEffectParam *> (param);
			if (eparam->etype() == etype)
				return param;
		}
		else return param;
	}

	return NULL;
}


// end of XGParam.cpp
