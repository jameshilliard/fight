#ifndef	_G711_H
#define _G711_H




 int  g711a_Encode(unsigned char *src,unsigned char *dest,unsigned int srclen,unsigned int *dstlen); 
 int  g711a_Decode( unsigned char *src,unsigned char *dest,unsigned int srclen,unsigned int *dstlen);
 int  g711u_Encode(unsigned char *src,unsigned char *dest,unsigned int srclen,unsigned int *dstlen);
 int  g711u_Decode( unsigned char *src,unsigned char *dest,unsigned int srclen,unsigned int *dstlen);



#endif
