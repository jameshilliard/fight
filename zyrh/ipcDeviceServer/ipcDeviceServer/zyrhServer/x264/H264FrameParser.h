#ifndef _H264_FRAME_PARSER_H_
#define _H264_FRAME_PARSER_H_
#include "stdint.h"
extern "C"
{
#include "x264.h"
#include "x264_config.h"
};
#include "sps.h"

const char* AVCFindStartCodeInternal(const char *p, const char *end);

const char* AVCFindStartCode(const char *p, const char *end);

void AVCParseNalUnits(const char *bufIn, int inSize, char* bufOut, int* outSize);

void ParseH264Nal(const char* nalsbuf, int size, char* outBuf, int& outLen,
    x264_nal_t* spsBuf, x264_nal_t* ppsBuf,x264_nal_t* pSeiBuf,
    bool& isKeyframe);
void CheckH264key(const char* nalsbuf, int size,  bool& isKeyframe);

void ParseHFlvToNal( char* nalsbuf, int size);

void ParseH264NalSpsPps(const char* nalsbuf, int size,char* spsBuf,unsigned int &spsSize, char* ppsBuf,unsigned int &ppsSize);
#endif // _H264_FRAME_PARSER_H_
