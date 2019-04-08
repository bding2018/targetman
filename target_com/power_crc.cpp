/*---------------------------------------------------------------------------
  CRC 校验计算类
      该类可用于计算8位、16位和32位 CRC 校验。在类的初始化函数中指明要计算
      的CRC的位长、多项式、运算顺序、初始值和屏蔽值。
      Reverse为真时表示是右移
---------------------------------------------------------------------------*/
#include "power_crc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
UBYTE4 Crc::bits [BITSPERBYTE * 4] =
 { 0x00000001UL, 0x00000002UL, 0x00000004UL, 0x00000008UL,
   0x00000010UL, 0x00000020UL, 0x00000040UL, 0x00000080UL,
   0x00000100UL, 0x00000200UL, 0x00000400UL, 0x00000800UL,
   0x00001000UL, 0x00002000UL, 0x00004000UL, 0x00008000UL,
   0x00010000UL, 0x00020000UL, 0x00040000UL, 0x00080000UL,
   0x00100000UL, 0x00200000UL, 0x00400000UL, 0x00800000UL,
   0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL,
   0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
 } ;
//---------------------------------------------------------------------------
Crc::Crc (unsigned int BitCount,  
		  unsigned long Polynominal,
          bool Reverse,
		  unsigned long Initial, 
		  unsigned long FinalMask)	
// BitCount    : CRC Size
// Polynominal : CRC Polynomial
// Reverse     : Reversed (means shift right)
// Initial     : Initial CRC Register Value
// FinalMask   : Final CRC XOR Value
{
  BITCOUNT    = BitCount;   //校验码的位数
  POLYNOMINAL = Polynominal;//生成多项式
  INITIAL     = Initial;    //注册码
  FINALMASK   = FinalMask;  //掩码
  // Mask needed to mask off redundent bits
  mask = ((1UL << (BITCOUNT - 1)) - 1UL)  | (1UL << (BITCOUNT - 1)) ;
  REVERSE = Reverse;        // Before set this property
                            // mask must be calculated
}
//---------------------------------------------------------------------------
void Crc::SetReverse(bool reverse)
{
  FREVERSE = reverse;
  if (reverse)  {
    for (unsigned int ii = 0 ; ii < MAXBYTEVALUES ; ++ ii)
      values [ii] = ReverseTableEntry (ii) ;
  }
  else  {
    for (unsigned int ii = 0 ; ii < MAXBYTEVALUES ; ++ ii)
      values [ii] = ForwardTableEntry (ii) ;
  }

}
//---------------------------------------------------------------------------
/************************************************************************/
/* 函数名称: Reverse (UBYTE4 value)                                     

/* 参    数: 1.UBYTE4 value - 需要首尾互调的数

/* 返 回 值: 返回首尾互调后的数值

/* 功    能: 将一个数值首尾互调，如 1010 -> 0101 

/* 作    者:ebaoqigai@sina.com

/* 修    改: version		  purpose			 date
             1.0              create             2014.09.12
/************************************************************************/
BYTE4 Crc::Reverse (UBYTE4 value)
{
  unsigned long result = 0 ;

  for (unsigned int jj = 0 ; jj < BITCOUNT ; ++ jj)
  {
    if ((value & bits [jj]) != 0)
      result |= bits [BITCOUNT - jj - 1] ;
  }
  return result ;
}
//---------------------------------------------------------------------------
/************************************************************************/
/* 函数名称: ForwardTableEntry (unsigned int entryindex)                                     

/* 参    数: 1.unsigned int entryindex - 需要首尾互调的数

/* 返 回 值: 返回首尾互调后的数值

/* 功    能: 将一个数值首尾互调，如 1010 -> 0101 

/* 作    者:ebaoqigai@sina.com

/* 修    改: version		  purpose			 date
             1.0              create             2014.09.12
/************************************************************************/
UBYTE4 Crc::ForwardTableEntry (unsigned int entryindex)
//  This function creates a CRC table entry for a non-reversed
//  CRC function.
//
//  Parameters:
//
//  entryindex: The index of the CRC table entry.
//
//  Return Value:
//
//  The value for the specified CRC table entry.
//
{

  UBYTE4 result = entryindex << (BITCOUNT - BITSPERBYTE) ;
  for (unsigned int ii = 0 ; ii < BITSPERBYTE ; ++ ii)
  {
    if ((result & bits [BITCOUNT - 1]) == 0)
      result <<= 1 ;
    else
      result = (result << 1) ^ POLYNOMINAL ;
  }
  result &= mask ;
  return result ;
}
//---------------------------------------------------------------------------
UBYTE4 Crc::ReverseTableEntry (unsigned int entryindex)
//    This function creates a CRC table entry for a reversed
//
//    CRC function.
//
//    Parameters:
//
//    entryindex: The index of the CRC table entry.
//
//    Return Value:
//
//    The value for the specified CRC table entry.
//
{
  UBYTE4 result = entryindex ;
  for (unsigned int ii = 0 ; ii < BITSPERBYTE ; ++ ii)
  {
    if ((result & 1) == 0)
      result >>= 1 ;
    else
      result = (result >> 1) ^ Reverse (POLYNOMINAL) ;
  }
  result &=  mask ;
  return result ;
}
//---------------------------------------------------------------------------
void Crc::reset (void)
{
  crc_register = INITIAL ;
}
//---------------------------------------------------------------------------
UBYTE4 Crc::value (void) const
{
  UBYTE4 result = crc_register ^ FINALMASK ;
  result &= mask ;
  return result ;
}
//---------------------------------------------------------------------------
void Crc::update (const char *buffer, unsigned int length)
//    This function updates the value of the CRC register based upon
//    the contents of a buffer.
//
//  Parameters:
//
//    buffer: The input buffer
//    length: The length of the input buffer.
//
{
  // The process for updating depends upon whether or not we are using
  // the reversed CRC form.

  if (REVERSE)
  {
    for (unsigned int ii = 0 ; ii < length ; ++ ii)
    {
      crc_register = values [(crc_register ^ buffer [ii]) & 0xFF]
                   ^ (crc_register >> 8) ;
    }
  }
  else
  {
    for (unsigned int ii = 0 ; ii < length ; ++ ii)
    {
      UBYTE4 index = ((crc_register >> (BITCOUNT - BITSPERBYTE)) ^ buffer [ii]) ;
      crc_register = values [index & 0xFF]
                   ^ (crc_register << BITSPERBYTE) ;
    }
  }
  return ;
}
//---------------------------------------------------------------------------
UBYTE4 Crc::getcrc(const char *buffer, unsigned int length)
{
  reset();
  update(buffer,length);
  return value();
}
