#include "featureDownsample.h"
/*----------------------------------------------------------------------*/
/*									*/
/*	featureDownsample.c						*/
/*									*/
/*	consist of ds_filter1.c ds_filter2.c ds_compand.c		*/
/*	MM changes: ANSI, mode to initialize sij, corrected bug		*/
/*----------------------------------------------------------------------*/
/* #define DEBUG      0	*/			/* Debug switch (0,1)	*/
#define MINZ     -32768				/* Minimaler Wert	*/
#define MAXZ      32767				/* Maximaler Wert	*/

/*----------------------------------------------------------------------*/
/* F I L T E R 1							*/
/*									*/
/* Diese Funktion filtert einen 16-Bit-Abtastwert			*/
/* Der Filter besteht aus einem Butterworth Tiefpass 8. Ordnung		*/
/* Der Filter ist realisiert als Serienschaltung von Subsystemen	*/
/* zweiter Ordnung in Direktform II nach Oppenheim & Schafer, p. 152	*/
/*----------------------------------------------------------------------*/
/* Input-Parameter:	x	- Abtastwert				*/
/* Funktionswert:	y	- gefilterter Abtastwert		*/
/*----------------------------------------------------------------------*/
/* Autor:		Michael Wagner					*/
/* Version:		09.06.92					*/
/*----------------------------------------------------------------------*/
/* Konstanten:								*/
/*----------------------------------------------------------------------*/
#define alpha11  0.39244409772263710856		/* Tiefpass Stufe 1	*/
#define alpha12 -0.68109612990589940562
#define alpha21  0.30313253523119448873		/* Tiefpass Stufe 2	*/
#define alpha22 -0.29851597917491151657
#define alpha31  0.25816519206972698974		/* Tiefpass Stufe 3	*/
#define alpha32 -0.10589127925059130253
#define alpha41  0.23897939720667044083		/* Tiefpass Stufe 4	*/
#define alpha42 -0.02370590385414955703
#define gain1    0.00333318931584549675

/*----------------------------------------------------------------------*/
/* Programm:								*/
/*----------------------------------------------------------------------*/

short int ds_filter1(short int x, int mode)
{
   /*-------------------------------------------------------------------*/
   /* Variable								*/
   /*-------------------------------------------------------------------*/

   double s,y;
   static double s11,s12,s21,s22,s31,s32,s41,s42;

   if (!mode) {
      s11 = 0.0; s12 = 0.0;
      s21 = 0.0; s22 = 0.0;
      s31 = 0.0; s32 = 0.0;
      s41 = 0.0; s42 = 0.0;
   }
   /*-------------------------------------------------------------------*/
   /* Tiefpass 8. Ordnung = 4 Stufen 2. Ordnung in Serie		*/
   /*									*/
   /*                        gain * (1 + z**(-1))**N               	*/
   /* H(z) = ------------------------------------------------------	*/
   /*        prod(k=1,nhalf)(1 - alpha1k*z**(-1) - alpha2k*z**(-2))	*/
   /*									*/
   /* alpha11=  0.39244409772263710856 alpha12= -0.68109612990589940562	*/
   /* alpha21=  0.30313253523119448873 alpha22= -0.29851597917491151657	*/
   /* alpha31=  0.25816519206972698974 alpha32= -0.10589127925059130253	*/
   /* alpha41=  0.23897939720667044083 alpha42= -0.02370590385414955703	*/
   /* beta.1 =  2.0                    beta.2=   1.0			*/
   /* gain1=    0.00333318931584549675					*/
   /*-------------------------------------------------------------------*/

   /*-------------------------------------------------------------------*/
   /* Stufe 1								*/
   /*-------------------------------------------------------------------*/
   s   = gain1 * x + alpha11 * s11 + alpha12 * s12;
   y   = s + 2.0 * s11 + s12;
   s12 = s11;
   s11 = s;

#if DEBUG
   /* printf("Stage 1: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Stufe 2								*/
   /*-------------------------------------------------------------------*/
   s   = y + alpha21 * s21 + alpha22 * s22;
   y   = s + 2.0 * s21 + s22;
   s22 = s21;
   s21 = s;

#if DEBUG
   /* printf("Stage 2: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Stufe 3								*/
   /*-------------------------------------------------------------------*/
   s   = y + alpha31 * s31 + alpha32 * s32;
   y   = s + 2.0 * s31 + s32;
   s32 = s31;
   s31 = s;

#if DEBUG
   /* printf("Stage 3: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Stufe 4								*/
   /*-------------------------------------------------------------------*/
   s   = y + alpha41 * s41 + alpha42 * s42;
   y   = s + 2.0 * s41 + s42;
   s42 = s41;
   s41 = s;

#if DEBUG
   /* printf("Stage 4: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Begrenzung des Funktionswertes auf 12 Bits			*/
   /*-------------------------------------------------------------------*/
   if (y > MAXZ)
      return MAXZ;
   else if (y < MINZ)
      return MINZ;
   else
      return (short int)y;
}


/*----------------------------------------------------------------------*/
/* F I L T E R 2							*/
/*									*/
/* Diese Funktion filtert einen 16-Bit-Abtastwert                       */
/* Der Filter besteht aus einem Butterworth Hochpass 4. Ordnung		*/
/* Der Filter ist realisiert als Serienschaltung von Subsystemen	*/
/* zweiter Ordnung in Direktform II nach Oppenheim & Schafer, p. 152	*/
/*----------------------------------------------------------------------*/
/* Input-Parameter:	x	- Abtastwert				*/
/* Funktionswert:	y	- gefilterter Abtastwert		*/
/*----------------------------------------------------------------------*/
/* Autor:		Michael Wagner					*/
/* Version:		09.06.92					*/
/*----------------------------------------------------------------------*/
/* Konstanten:								*/
/*----------------------------------------------------------------------*/
#define alpha51  1.90064656380712770911		/* Hochpass Stufe 1	*/
#define alpha52 -0.91391293369153381310
#define alpha61  1.79158769677778417773		/* Hochpass Stufe 2	*/
#define alpha62 -0.80409284398316283049
#define gain5    0.85724608479567454555

/*----------------------------------------------------------------------*/
/* Programm:								*/
/*----------------------------------------------------------------------*/

short int ds_filter2(short int x, int mode)
{
   /*-------------------------------------------------------------------*/
   /* Variable								*/
   /*-------------------------------------------------------------------*/

   double s,y;
   static double s51,s52,s61,s62;

   if (!mode) {
      s51 = 0.0; s52 = 0.0;
      s61 = 0.0; s62 = 0.0;
   }
   /*-------------------------------------------------------------------*/
   /* Hochpass 4. Ordnung = 2 Stufen 2. Ordnung in Serie		*/
   /*									*/
   /*                        gain * (1 - z**(-1))**N               	*/
   /* H(z) = ------------------------------------------------------	*/
   /*        prod(k=1,nhalf)(1 - alpha1k*z**(-1) - alpha2k*z**(-2))	*/
   /*									*/
   /* alpha51=  1.90064656380712770911 alpha52= -0.91391293369153381310	*/
   /* alpha61=  1.79158769677778417773 alpha62= -0.80409284398316283049	*/
   /* beta.1 = -2.0                    beta.2=   1.0			*/
   /* gain5     0.85724608479567454555					*/
   /*-------------------------------------------------------------------*/

   /*-------------------------------------------------------------------*/
   /* Stufe 1                                                           */
   /*-------------------------------------------------------------------*/
   s   = gain5 * x + alpha51 * s51 + alpha52 * s52;
   y   = s - 2.0 * s51 + s52;
   s52 = s51;
   s51 = s;

#if DEBUG
   /* printf("Stage 5: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Stufe 2                                                           */
   /*-------------------------------------------------------------------*/
   s   = y + alpha61 * s61 + alpha62 * s62;
   y   = s - 2.0 * s61 + s62;
   s62 = s61;
   s61 = s;

#if DEBUG
   /* printf("Stage 6: s=%lf y=%lf\n",s,y); */
#endif

   /*-------------------------------------------------------------------*/
   /* Begrenzung des Funktionswertes auf 12 Bits                        */
   /*-------------------------------------------------------------------*/
   if (y > MAXZ)
      return MAXZ;
   else if (y < MINZ)
      return MINZ;
   else
      return (short int)y;
}
/*----------------------------------------------------------------------*/
/* C O M P A N D							*/
/*									*/
/* Diese Funktion komprimiert einen 16-Bit-Abtastwert (lin-PCM)		*/
/* zu einem 8-Bit-Abtastwert (log-PCM/A-law) und re-expandiert		*/
/* den gefundenen Wert zurueck zu einem 16-Bit-Abtastwert (lin-PCM).	*/
/*----------------------------------------------------------------------*/
/* Input-Parameter:     x       - Abtastwert -32768 <= x <= +32767	*/
/* Funktionswert:       y       - Abtastwert -32768 <= y <= +32767	*/
/*----------------------------------------------------------------------*/
/* Autor:               Michael Wagner					*/
/* Version:             22.06.92					*/
/*----------------------------------------------------------------------*/
/* Konstanten:								*/
/*----------------------------------------------------------------------*/
#define XMAX2 16384
#define XMAX4  8192
#define XMAX8  4096
#define XMAX16 2048
#define XMAX32 1024
#define XMAX64  512
/*----------------------------------------------------------------------*/
/* Programm:								*/
/*----------------------------------------------------------------------*/
short int ds_compand(short int x)
{
   unsigned xx, yy;		/* Zwischenwerte			*/
   short int sign;		/* Vorzeichenspeicher			*/

   /*-------------------------------------------------------------------*/
   /* Vorzeichenentzug							*/
   /*-------------------------------------------------------------------*/

   if (x > 0) {
      xx   = x;
      sign = 1;
   }
   else {
      xx   = -x;
      sign = -1;
   }

   /*-------------------------------------------------------------------*/
   /* Kompression nach A-Law in 7 linearen Bereichen (+6 negativen)	*/
   /*-------------------------------------------------------------------*/

   if (xx >= XMAX2)
      yy = 112 + (unsigned)((float)(xx-XMAX2) / XMAX2 * 16 + 0.5);
   else if (xx >= XMAX4)
      yy = 96 + (unsigned)((float)(xx-XMAX4) / XMAX4 * 16 + 0.5);
   else if (xx >= XMAX8)
      yy = 80 + (unsigned)((float)(xx-XMAX8) / XMAX8 * 16 + 0.5);
   else if (xx >= XMAX16)
      yy = 64 + (unsigned)((float)(xx-XMAX16) / XMAX16 * 16 + 0.5);
   else if (xx >= XMAX32)
      yy = 48 + (unsigned)((float)(xx-XMAX32) / XMAX32 * 16 + 0.5);
   else if (xx >= XMAX64)
      yy = 32 + (unsigned)((float)(xx-XMAX64) / XMAX64 * 16 + 0.5);
   else
      yy = (unsigned)((float)xx / XMAX64 * 32 + 0.5);

   /*-------------------------------------------------------------------*/
   /* Vorsicht: yy=128 ist fuer positive x nicht erlaubt!		*/
   /*-------------------------------------------------------------------*/

   if (sign==1 && yy==128)
      yy = 127;

#if DEBUG
   /* printf("COMPAND: yy=%d\n",sign*yy); */
#endif

   /*-------------------------------------------------------------------*/
   /* Re-Expansion zu linear-PCM					*/
   /*-------------------------------------------------------------------*/

   if (yy >= 112)
      xx = XMAX2 + (yy - 112) * XMAX2 / 16;
   else if (yy >= 96)
      xx = XMAX4 + (yy - 96) * XMAX4 / 16;
   else if (yy >= 80)
      xx = XMAX8 + (yy - 80) * XMAX8 / 16;
   else if (yy >= 64)
      xx = XMAX16 + (yy - 64) * XMAX16 / 16;
   else if (yy >= 48)
      xx = XMAX32 + (yy - 48) * XMAX32 / 16;
   else if (yy >= 32)
      xx = XMAX64 + (yy - 32) * XMAX64 / 16;
   else
      xx = yy * XMAX64 / 32;

   /*-------------------------------------------------------------------*/
   /* Vorzeichenrueckgabe						*/
   /*-------------------------------------------------------------------*/

   if (sign == 1)
      return (short int)xx;
   else
      return -(short int)(xx);
}
