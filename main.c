//
// bin_to_c - convert binary data into c-compatible data tables
//
// Written by Larry Bank
// Copyright (c) 2009 BitBank Software, Inc.
// Change history
// 2/1/09 - Started the project
// 11/6/15 - converted to Linux
// 7/29/20 - tailored to Arduino FLASH data output
//
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
FILE * ihandle;
void MakeC(unsigned char *, int, int);
void GetLeafName(char *fname, char *leaf);
void FixName(char *name);

int main(int argc, char *argv[])
{
int iSize, iData;
unsigned char *p;
char szLeaf[256];

   if (argc != 2)
      {
      printf("bin_to_c Copyright (c) 2013 BitBank Software, Inc.\n"); 
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
   p = (unsigned char *)malloc(0x10000); // allocate 64k to play with
   GetLeafName(argv[1], szLeaf);
   printf("//\n// %s\n//\n", szLeaf); // comment header with filename
   FixName(szLeaf); // remove unusable characters
   printf("const uint8_t %s[] PROGMEM = {", szLeaf); // start of data array
   while (iSize)
   {
      iData = fread(p, 1, 0x10000, ihandle); // try to read 64k
      MakeC(p, iData, iSize == iData); // create the output data
      iSize -= iData;
   }
   free(p);
   fclose(ihandle);
   printf("};\n"); // final closing brace
   return 0;
} /* main() */
//
// Generate C hex characters from each byte of file data
//
void MakeC(unsigned char *p, int iLen, int bLast)
{
int i, j, iCount;
char szTemp[256], szOut[256];

   iCount = 0;
   for (i=0; i<iLen>>4; i++) // do lines of 16 bytes
   {
      strcpy(szOut, "\t");
      for (j=0; j<16; j++)
      {
         if (iCount == iLen-1) // last one, skip the comma
            sprintf(szTemp, "0x%02x", p[(i*16)+j]);
         else
            sprintf(szTemp, "0x%02x,", p[(i*16)+j]);
         strcat(szOut, szTemp);
         iCount++;
      }
      if (!bLast || iCount != iLen)
         strcat(szOut, "\n");
      printf("%s",szOut);
   }
   p += (iLen & 0xfff0); // point to last section
   if (iLen & 0xf) // any remaining characters?
      {
      strcpy(szOut, "\t");
      for (j=0; j<(iLen & 0xf); j++)
      {
         if (iCount == iLen-1)
            sprintf(szTemp, "0x%02x", p[j]);
         else
            sprintf(szTemp, "0x%02x,", p[j]);
         strcat(szOut, szTemp);
         iCount++;
      }
      if (!bLast)
         strcat(szOut, "\n");
      printf("%s",szOut);
      }
} /* MakeC() */
//
// Make sure the name can be used in C/C++ as a variable
// replace invalid characters and make sure it starts with a letter
//
void FixName(char *name)
{
char c, *d, *s, szTemp[256];
int i, iLen;

   iLen = strlen(name);
   d = szTemp;
   s = name;
   if (s[0] >= '0' && s[0] <= '9') // starts with a digit
      *d++ = '_'; // Insert an underscore
   for (i=0; i<iLen; i++)
   {
      c = *s++;
      // these characters can't be in a variable name
      if (c < ' ' || (c >= '!' && c < '0') || (c > 'Z' && c < 'a'))
         c = '_'; // convert all to an underscore
      *d++ = c;
   }
   *d++ = 0;
   strcpy(name, szTemp);
} /* FixName() */
//
// Trim off the leaf name from a fully
// formed file pathname
//
void GetLeafName(char *fname, char *leaf)
{
int i, iLen;
   
   iLen = strlen(fname);
   for (i=iLen-1; i>=0; i--)
      {
      if (fname[i] == '\\' || fname[i] == '/') // Windows or Linux
         break;
      }
   strcpy(leaf, &fname[i+1]);
   // remove the filename extension
   iLen = strlen(leaf); 
   for (i=iLen-1; i>=0; i--)
   {
      if (leaf[i] == '.')
      {
         leaf[i] = 0;
         break;
      }
   }
} /* GetLeafName() */

