//
// bin_to_c - convert binary data into c-compatible data tables
// Written by Larry Bank
// Copyright (c) 2009 BitBank Software, Inc.
// Change history
// 2/1/09 - Started the project
// 11/6/15 - converted to Linux
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
FILE * ihandle;
void MakeC(char *, int);

int main(int argc, char *argv[])
{
int iSize, iData;
char *p;

   if (argc != 2)
      {
      printf("bin_to_c Copyright (c) 2013-2015 BitBank Software, Inc.\n"); 
      printf("Usage: bin_to_c <filename>\n");
      printf("output is written to stdout\n"); 
      return 0; // no filename passed
      }
   ihandle = fopen(argv[1],"rb"); // open input file
   if (ihandle == NULL)
      {
      printf("Unable to open file: %s\n", argv[1]);
      return -1; // bad filename passed
      }
   
   fseek(ihandle, 0L, SEEK_END); // get the file size
   iSize = (int)ftell(ihandle);
   fseek(ihandle, 0, SEEK_SET);
   p = (char *)malloc(0x10000); // allocate 64k to play with
   while (iSize)
      {
      iData = fread(p, 1, 0x10000, ihandle); // try to read 64k
      MakeC(p, iData); // create the output data
      iSize -= iData;
      }
   free(p);
   fclose(ihandle);
   return 0;
} /* main() */

void MakeC(char *p, int iLen)
{
int i, j;
char szTemp[256], szOut[256];

   for (i=0; i<(iLen>>4); i++) // do lines of 16 bytes
      {
      strcpy(szOut, "\t");
      for (j=0; j<16; j++)
         {
         sprintf(szTemp, "0x%02x,", p[(i*16)+j]);
         strcat(szOut, szTemp);
         }
      strcat(szOut, "\r\n");
      printf(szOut);
      }
   p += (iLen & 0xfff0); // point to last section
   if (iLen & 0xf) // any remaining characters?
      {
      strcpy(szOut, "\t");
      for (j=0; j<(iLen & 0xf); j++)
         {
         sprintf(szTemp, "0x%02x,", p[j]);
         strcat(szOut, szTemp);
         }
      strcat(szOut, "\r\n");
      printf(szOut);
      }
} /* MakeC() */
