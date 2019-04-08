//---------------------------------------------------------------------------
#ifndef CRCClassH
#define CRCClassH
//---------------------------------------------------------------------------
typedef char BYTE1 ;
typedef unsigned char UBYTE1 ;
typedef short BYTE2 ;
typedef unsigned short UBYTE2 ;
typedef int BYTE4 ;
typedef unsigned int UBYTE4 ;
typedef float        FBYTE4;

const int MAXBYTEVALUES = 256;
const int BITSPERBYTE   = 8 ;
const int BUFFER_SIZE   = 512;

#include <memory.h>
#include "time.h"
class Crc
{
public:
    Crc ( unsigned int  BitCount,   //生成的校验码位数
          unsigned long Polynominal,//多项式系数
          bool Reverse,             //取反
          unsigned long Initial,    //初始位
          unsigned long FinalMask) ;//掩码
    ~Crc () {}
    BYTE4   Reverse (UBYTE4 value);
    UBYTE4 value (void) const ;
    void   reset (void) ;
    void   update (const char *buffer, unsigned int length) ;
    UBYTE4 getcrc(const char *buffer, unsigned int length) ;
	bool GetReverse(void)
	{
		return FREVERSE;
	};
    _declspec( property( get=GetReverse, put=SetReverse ) ) bool REVERSE;

private:
    unsigned int    BITCOUNT;
    unsigned long   POLYNOMINAL;
    bool            FREVERSE;
    unsigned long   INITIAL;
    unsigned long   FINALMASK;
    unsigned long   mask;          
    static UBYTE4   bits [];//BITSPERBYTE * 4];
    UBYTE4 values   [MAXBYTEVALUES] ;
    UBYTE4 crc_register ;
    void   SetReverse(bool reverse);
    UBYTE4 ForwardTableEntry (unsigned int entryindex);
    UBYTE4 ReverseTableEntry (unsigned int entryindex);
} ;

//---------------------------------------------------------------------------
#endif
