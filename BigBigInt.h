/*
 * Name:    BigBigInt.hpp
 * Purpose: Support the bigbigint class
 *
 * Author:  Richard Andrasek
 * Date:    25-Sep-2007
 *
 */

#ifndef __Andrasek_BigBigInt_hpp__
#define __Andrasek_BigBigInt_hpp__

//-----------------------------------------------------------------------------
//                            Required Includes
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <limits.h>

//-----------------------------------------------------------------------------
//                              Base Defines
//-----------------------------------------------------------------------------

#if defined(WIN32)
    #define D_LONG __int64
#else
    #define D_LONG double long
    //Note:  64-bit linux defines "long" as 64 bits.
    //       I'm not sure how this would play out there.
    //       However, this essentially needs to be a 64-bit integer.
#endif

//  The base type 
//  Requirements:
//  1)  Must be unsigned (u_char, u_short, u_int, u_long, etc.)
//  2)  Must not be a double long/__int64/etc.  Needs to be 32-bits at most!
//  
#define BBI_BASE_TYPE unsigned int
#define BBI_BASE_MAX  0xFFFFFFFFUL

//  The minimum size is 32 bytes.
//  NOTE: Make sure that this is at least as large as
//  an INT.
//  (MIN_SIZE * sizeof(BASE_TYPE) = minimum # bytes)
#define BBI_MIN_SIZE  2


//-----------------------------------------------------------------------------
//                          BigBigInt Class
//-----------------------------------------------------------------------------


class bigbigint 
{


// ----
public:
// ----


//
//  CON/DESTRUCTORS
//
    // constructors
    bigbigint();
    bigbigint(long size);

    // copy constructors
    bigbigint(bigbigint const& copy);

    // destructor
    virtual ~bigbigint();

//
//  FUNCTIONS
//
    long length();
    bigbigint * copy(bigbigint *item_to_copy);  // "=" also works.  ;)


//
//  OPERATOR OVERLOADS
//
    // Assignment
    bigbigint operator =(const bigbigint &NewVal);
	bigbigint operator =(int);
	bigbigint operator =(unsigned int);
	bigbigint operator =(long);
	bigbigint operator =(unsigned long);
	bigbigint operator =(short);
	bigbigint operator =(unsigned short);
	bigbigint operator =(char);
	bigbigint operator =(unsigned char);
    bigbigint operator =(float);
    bigbigint operator =(double);

    // Casting
    operator int() const;
    operator unsigned int() const;
    operator long() const;
    operator unsigned long() const;
    operator short() const;
    operator unsigned short() const;
    operator char() const;
    operator unsigned char() const;
    operator float() const;
    operator double() const;

    // Addition
	bigbigint operator +(bigbigint);
    bigbigint operator +(int);
    bigbigint operator +(unsigned int);
    bigbigint operator +(long);
    bigbigint operator +(unsigned long);
    bigbigint operator +(short);
    bigbigint operator +(unsigned short);
    bigbigint operator +(char);
    bigbigint operator +(unsigned char);
    bigbigint operator + (float);
    bigbigint operator + (double);
    static bigbigint _perform_integral_adding(
        bigbigint * menuend,
        void* plus_var, 
        unsigned long plus_size,
        bool is_signed);

    // Addition/Assignment
    bigbigint operator +=(bigbigint);
    bigbigint operator +=(int);
    bigbigint operator +=(unsigned int);
    bigbigint operator +=(long);
    bigbigint operator +=(unsigned long);
    bigbigint operator +=(short);
    bigbigint operator +=(unsigned short);
    bigbigint operator +=(char);
    bigbigint operator +=(unsigned char);
    bigbigint operator +=(float);
    bigbigint operator +=(double);

    // Subtraction
    bigbigint operator -(bigbigint);
    bigbigint operator -(int);
    bigbigint operator -(unsigned int);
    bigbigint operator -(long);
    bigbigint operator -(unsigned long);
    bigbigint operator -(short);
    bigbigint operator -(unsigned short);
    bigbigint operator -(char);
    bigbigint operator -(unsigned char);
    bigbigint operator -(float);
    bigbigint operator -(double);
    static bigbigint _perform_integral_subtraction(
        bigbigint *this_val,
        void *Subtrahend,
        unsigned long sub_size,
        bool is_signed);

    // Subtraction/Assignment
    bigbigint operator -=(bigbigint);
    bigbigint operator -=(int);
    bigbigint operator -=(unsigned int);
    bigbigint operator -=(long);
    bigbigint operator -=(unsigned long);
    bigbigint operator -=(short);
    bigbigint operator -=(unsigned short);
    bigbigint operator -=(char);
    bigbigint operator -=(unsigned char);
    bigbigint operator -=(float);
    bigbigint operator -=(double);

    // Multiplication
    bigbigint operator *(const bigbigint &multiplier);
    bigbigint operator *(int);
    bigbigint operator *(unsigned int);
    bigbigint operator *(long);
    bigbigint operator *(unsigned long);
    bigbigint operator *(short);
    bigbigint operator *(unsigned short);
    bigbigint operator *(char);
    bigbigint operator *(unsigned char);
    bigbigint operator *(float);
    bigbigint operator *(double);
    static bigbigint bigbigint::_perform_integral_multiplication(
        bigbigint * multiplicand,
        void *multiplier, 
        unsigned long mult_size,
        bool is_signed);

    // Multiplication/Assignment
    bigbigint operator *=(bigbigint);
    bigbigint operator *=(int);
    bigbigint operator *=(unsigned int);
    bigbigint operator *=(long);
    bigbigint operator *=(unsigned long);
    bigbigint operator *=(short);
    bigbigint operator *=(unsigned short);
    bigbigint operator *=(char);
    bigbigint operator *=(unsigned char);
    bigbigint operator *=(float);
    bigbigint operator *=(double);

    // Division
    bigbigint operator /(bigbigint);
    bigbigint operator /(int);
    bigbigint operator /(unsigned int);
    bigbigint operator /(long);
    bigbigint operator /(unsigned long);
    bigbigint operator /(short);
    bigbigint operator /(unsigned short);
    bigbigint operator /(char);
    bigbigint operator /(unsigned char);
    bigbigint operator /(float);
    bigbigint operator /(double);
    static void _perform_integral_division(
        bigbigint dividend,
        bigbigint divisor,
        bigbigint *quotient,
        bigbigint *remainder);

    // Division/Assignment
    bigbigint operator /=(bigbigint);
    bigbigint operator /=(int);
    bigbigint operator /=(unsigned int);
    bigbigint operator /=(long);
    bigbigint operator /=(unsigned long);
    bigbigint operator /=(short);
    bigbigint operator /=(unsigned short);
    bigbigint operator /=(char);
    bigbigint operator /=(unsigned char);
    bigbigint operator /=(float);
    bigbigint operator /=(double);

    // Comparison:  >
    bool operator >(bigbigint CompVal);
    bool operator >(int);
    bool operator >(unsigned int);
    bool operator >(long);
    bool operator >(unsigned long);
    bool operator >(short);
    bool operator >(unsigned short);
    bool operator >(char);
    bool operator >(unsigned char);
    bool operator >(float);
    bool operator >(double);

    // Comparison:  >=
    bool operator >=(bigbigint CompVal);
    bool operator >=(int);
    bool operator >=(unsigned int);
    bool operator >=(long);
    bool operator >=(unsigned long);
    bool operator >=(short);
    bool operator >=(unsigned short);
    bool operator >=(char);
    bool operator >=(unsigned char);
    bool operator >=(float);
    bool operator >=(double);

    // Comparison:  <
    bool operator <(bigbigint CompVal);
    bool operator <(int);
    bool operator <(unsigned int);
    bool operator <(long);
    bool operator <(unsigned long);
    bool operator <(short);
    bool operator <(unsigned short);
    bool operator <(char);
    bool operator <(unsigned char);
    bool operator <(float);
    bool operator <(double);

    // Comparison:  <=
    bool operator <=(bigbigint CompVal);
    bool operator <=(int);
    bool operator <=(unsigned int);
    bool operator <=(long);
    bool operator <=(unsigned long);
    bool operator <=(short);
    bool operator <=(unsigned short);
    bool operator <=(char);
    bool operator <=(unsigned char);
    bool operator <=(float);
    bool operator <=(double);

    // Comparison:  ==
    bool operator ==(bigbigint CompVal);
    bool operator ==(int);
    bool operator ==(unsigned int);
    bool operator ==(long);
    bool operator ==(unsigned long);
    bool operator ==(short);
    bool operator ==(unsigned short);
    bool operator ==(char);
    bool operator ==(unsigned char);
    bool operator ==(float);
    bool operator ==(double);

    // Comparison:  !=
    bool operator !=(bigbigint CompVal);
    bool operator !=(int);
    bool operator !=(unsigned int);
    bool operator !=(long);
    bool operator !=(unsigned long);
    bool operator !=(short);
    bool operator !=(unsigned short);
    bool operator !=(char);
    bool operator !=(unsigned char);
    bool operator !=(float);
    bool operator !=(double);

    // Bitwise Left Shift Operators
    bigbigint operator <<(int);
    bigbigint operator <<(unsigned int);
    bigbigint operator <<(long);
    bigbigint operator <<(unsigned long);
    bigbigint operator <<(short);
    bigbigint operator <<(unsigned short);
    bigbigint operator <<(char);
    bigbigint operator <<(unsigned char);

    // Bitwise Left Shift / Assignment
    bigbigint operator <<=(int);
    bigbigint operator <<=(unsigned int);
    bigbigint operator <<=(long);
    bigbigint operator <<=(unsigned long);
    bigbigint operator <<=(short);
    bigbigint operator <<=(unsigned short);
    bigbigint operator <<=(char);
    bigbigint operator <<=(unsigned char);

    // Bitwise Right Shift Operators
    bigbigint operator >>(int);
    bigbigint operator >>(unsigned int);
    bigbigint operator >>(long);
    bigbigint operator >>(unsigned long);
    bigbigint operator >>(short);
    bigbigint operator >>(unsigned short);
    bigbigint operator >>(char);
    bigbigint operator >>(unsigned char);

    // Bitwise Right Shift / Assignment
    bigbigint operator >>=(int);
    bigbigint operator >>=(unsigned int);
    bigbigint operator >>=(long);
    bigbigint operator >>=(unsigned long);
    bigbigint operator >>=(short);
    bigbigint operator >>=(unsigned short);
    bigbigint operator >>=(char);
    bigbigint operator >>=(unsigned char);

    // Bitwise Or Operator
    bigbigint operator |(bigbigint);
    bigbigint operator |(int);
    bigbigint operator |(unsigned int);
    bigbigint operator |(long);
    bigbigint operator |(unsigned long);
    bigbigint operator |(short);
    bigbigint operator |(unsigned short);
    bigbigint operator |(char);
    bigbigint operator |(unsigned char);
    bigbigint operator |(float);
    bigbigint operator |(double);

    bigbigint operator |=(bigbigint);
    bigbigint operator |=(int);
    bigbigint operator |=(unsigned int);
    bigbigint operator |=(long);
    bigbigint operator |=(unsigned long);
    bigbigint operator |=(short);
    bigbigint operator |=(unsigned short);
    bigbigint operator |=(char);
    bigbigint operator |=(unsigned char);
    bigbigint operator |=(float);
    bigbigint operator |=(double);

    // Unary Operators
    bigbigint operator -();
    bigbigint operator --(void);   //Prefix
    bigbigint operator ++(void);   
    bigbigint operator --(int);    //Postfix
    bigbigint operator ++(int);   
    bool operator !();

/*
    -----------
    TODO
    -----------

    %
    %=
    &


*/


// -----
private:
// -----


//
//  VARIABLES
//
    unsigned char * _first_byte;
    unsigned long _num_bytes;
    unsigned long _length;
    unsigned char _flags;

    // Flags for the _flags value...
    #define BBI_NEGATIVE 0x01
    #define IS_NEGATIVE(__value)  (__value & BBI_NEGATIVE)

//
//  FUNCTIONS
//
    void _constructor(long size);
    void _free();
    void _malloc(unsigned long num_bytes);
    void _upsize(unsigned long new_length);
    void zero_fill(unsigned long num_byts_to_fill);
    void zero_fill();
};


//-----------------------------------------------------------------------------
//                 Non-Member Functions (operator overloads)
//-----------------------------------------------------------------------------

//  <type> + [bigbigint]
bigbigint operator +(const int &PlusVal, bigbigint &ValB);
bigbigint operator +(const unsigned int &PlusVal, bigbigint &ValB);
bigbigint operator +(const long &PlusVal, bigbigint &ValB);
bigbigint operator +(const unsigned long &PlusVal, bigbigint &ValB);
bigbigint operator +(const short &PlusVal, bigbigint &ValB);
bigbigint operator +(const unsigned short &PlusVal, bigbigint &ValB);
bigbigint operator +(const char &PlusVal, bigbigint &ValB);
bigbigint operator +(const unsigned char &PlusVal, bigbigint &ValB);
bigbigint operator +(const float &PlusVal, bigbigint &ValB);
bigbigint operator +(const double &PlusVal, bigbigint &ValB);

//  <type> += [bigbigint]
int operator +=(int &PlusVal, bigbigint &ValB);
unsigned int operator +=(unsigned int& PlusVal, bigbigint &ValB);
long operator +=(long& PlusVal, bigbigint &ValB);
unsigned long operator +=(unsigned long &PlusVal, bigbigint &ValB);
short operator +=(short &PlusVal, bigbigint &ValB);
unsigned short operator +=(unsigned short &PlusVal, bigbigint &ValB);
char operator +=(char &PlusVal, bigbigint &ValB);
unsigned char operator +=(unsigned char &PlusVal, bigbigint &ValB);
float operator +=(float& PlusVal, bigbigint &ValB);
double operator +=(double &PlusVal, bigbigint &ValB);

//  <type> - [bigbigint]
bigbigint operator -(const int &SubVal, bigbigint &MyVal);
bigbigint operator -(const unsigned int &SubVal, bigbigint& MyVal);
bigbigint operator -(const long &SubVal, bigbigint &MyVal);
bigbigint operator -(const unsigned long &SubVal, bigbigint &MyVal);
bigbigint operator -(const short &SubVal, bigbigint& MyVal);
bigbigint operator -(const unsigned short &SubVal, bigbigint &MyVal);
bigbigint operator -(const char &SubVal, bigbigint &MyVal);
bigbigint operator -(const unsigned char &SubVal, bigbigint &MyVal);
bigbigint operator -(const float &SubVal, bigbigint &MyVal);
bigbigint operator -(const double &SubVal,  bigbigint& MyVal);

//  <type> -= [bigbigint]
int operator -=(int& PlusVal, bigbigint &ValB);
unsigned int operator -=(unsigned int &PlusVal, bigbigint &ValB);
long operator -=(long &PlusVal, bigbigint &ValB);
unsigned long operator -=(unsigned long &PlusVal, bigbigint &ValB);
short operator -=(short& PlusVal, bigbigint &ValB);
unsigned short operator -=(unsigned short &PlusVal, bigbigint &ValB);
char operator -=(char &PlusVal, bigbigint &ValB);
unsigned char operator -=(unsigned char& PlusVal, bigbigint &ValB);
float operator -=(float& PlusVal, bigbigint &ValB);
double operator -=(double& PlusVal, bigbigint &ValB);

//  <type> * [bigbigint]
bigbigint operator *(const int &SubVal, const bigbigint &MyVal);
bigbigint operator *(const unsigned int &SubVal, const bigbigint& MyVal);
bigbigint operator *(const long &SubVal, const bigbigint& MyVal);
bigbigint operator *(const unsigned long &SubVal, const bigbigint &MyVal);
bigbigint operator *(const short &SubVal, const bigbigint& MyVal);
bigbigint operator *(const unsigned short &SubVal, const bigbigint& MyVal);
bigbigint operator *(const char &SubVal, const bigbigint& MyVal);
bigbigint operator *(const unsigned char &SubVal, const bigbigint &MyVal);
bigbigint operator *(const float &SubVal, const bigbigint& MyVal);
bigbigint operator *(const double &SubVal, const bigbigint& MyVal);

//  <type> *= [bigbigint]
int operator *=( int &PlusVal, bigbigint &ValB);
unsigned int operator *=( unsigned int &PlusVal, bigbigint &ValB);
long operator *=( long& PlusVal, bigbigint &ValB);
unsigned long operator *=( unsigned long& PlusVal, bigbigint &ValB);
short operator *=( short& PlusVal, bigbigint &ValB);
unsigned short operator *=( unsigned short vPlusVal, bigbigint &ValB);
char operator *=( char &PlusVal, bigbigint &ValB);
unsigned char operator *=( unsigned char &PlusVal, bigbigint &ValB);
float operator *=( float &PlusVal, bigbigint &ValB);
double operator *=( double &PlusVal, bigbigint &ValB);

//  <type> / [bigbigint]
bigbigint operator /(int &SubVal, bigbigint &MyVal);
bigbigint operator /(unsigned int &SubVal, bigbigint& MyVal);
bigbigint operator /(long& SubVal, bigbigint &MyVal);
bigbigint operator /(unsigned long &SubVal, bigbigint& MyVal);
bigbigint operator /(short &SubVal, bigbigint& MyVal);
bigbigint operator /(unsigned short& SubVal, bigbigint& MyVal);
bigbigint operator /(char &SubVal, bigbigint& MyVal);
bigbigint operator /(unsigned char &SubVal, bigbigint& MyVal);
bigbigint operator /(float &SubVal, bigbigint& MyVal);
bigbigint operator /(double &SubVal, bigbigint &MyVal);

//  <type> /= [bigbigint]
int operator /=( int &PlusVal, bigbigint &ValB);
unsigned int operator /=( unsigned int &PlusVal, bigbigint &ValB);
long operator /=( long& PlusVal, bigbigint &ValB);
unsigned long operator /=( unsigned long& PlusVal, bigbigint &ValB);
short operator /=( short& PlusVal, bigbigint &ValB);
unsigned short operator /=( unsigned short vPlusVal, bigbigint &ValB);
char operator /=( char &PlusVal, bigbigint &ValB);
unsigned char operator /=( unsigned char &PlusVal, bigbigint &ValB);
float operator /=( float &PlusVal, bigbigint &ValB);
double operator /=( double &PlusVal, bigbigint &ValB);

//  <type> > [bigbigint]
bool operator >(const int &SubVal, bigbigint &MyVal);
bool operator >(const unsigned int &SubVal, bigbigint &MyVal);
bool operator >(const long &SubVal, bigbigint &MyVal);
bool operator >(const unsigned long &SubVal, bigbigint &MyVal);
bool operator >(const short &SubVal, bigbigint &MyVal);
bool operator >(const unsigned short &SubVal, bigbigint& MyVal);
bool operator >(const char &SubVal, bigbigint &MyVal);
bool operator >(const unsigned char &SubVal, bigbigint &MyVal);
bool operator >(const float &SubVal, bigbigint& MyVal);
bool operator >(const double &SubVal, bigbigint &MyVal);

//  <type> >= [bigbigint]
bool operator >=(const int &SubVal, bigbigint &MyVal);
bool operator >=(const unsigned int &SubVal, bigbigint& MyVal);
bool operator >=(const long &SubVal, bigbigint &MyVal);
bool operator >=(const unsigned long &SubVal, bigbigint &MyVal);
bool operator >=(const short &SubVal, bigbigint &MyVal);
bool operator >=(const unsigned short &SubVal, bigbigint& MyVal);
bool operator >=(const char &SubVal, bigbigint &MyVal);
bool operator >=(const unsigned char &SubVal, bigbigint &MyVal);
bool operator >=(const float &SubVal, bigbigint &MyVal);
bool operator >=(const double &SubVal, bigbigint &MyVal);

//  <type> < [bigbigint]
bool operator <(const int &SubVal, bigbigint &MyVal);
bool operator <(const unsigned int &SubVal, bigbigint &MyVal);
bool operator <(const long &SubVal, bigbigint& MyVal);
bool operator <(const unsigned long &SubVal, bigbigint &MyVal);
bool operator <(const short &SubVal, bigbigint& MyVal);
bool operator <(const unsigned short &SubVal, bigbigint &MyVal);
bool operator <(const char &SubVal, bigbigint &MyVal);
bool operator <(const unsigned char &SubVal, bigbigint &MyVal);
bool operator <(const float &SubVal, bigbigint& MyVal);
bool operator <(const double &SubVal, bigbigint &MyVal);

//  <type> <= [bigbigint]
bool operator <=(const int &SubVal, bigbigint &MyVal);
bool operator <=(const unsigned int &SubVal, bigbigint &MyVal);
bool operator <=(const long &SubVal, bigbigint& MyVal);
bool operator <=(const unsigned long &SubVal, bigbigint &MyVal);
bool operator <=(const short &SubVal, bigbigint& MyVal);
bool operator <=(const unsigned short &SubVal, bigbigint &MyVal);
bool operator <=(const char &SubVal, bigbigint &MyVal);
bool operator <=(const unsigned char &SubVal, bigbigint &MyVal);
bool operator <=(const float &SubVal, bigbigint& MyVal);
bool operator <=(const double &SubVal, bigbigint &MyVal);

//  <type> == [bigbigint]
bool operator ==(const int &SubVal, bigbigint& MyVal);
bool operator ==(const unsigned int &SubVal, bigbigint& MyVal);
bool operator ==(const long &SubVal, bigbigint &MyVal);
bool operator ==(const unsigned long &SubVal, bigbigint& MyVal);
bool operator ==(const short &SubVal, bigbigint& MyVal);
bool operator ==(const unsigned short &SubVal, bigbigint &MyVal);
bool operator ==(const char &SubVal, bigbigint &MyVal);
bool operator ==(const unsigned char &SubVal, bigbigint &MyVal);
bool operator ==(const float &SubVal, bigbigint &MyVal);
bool operator ==(const double &SubVal, bigbigint &MyVal);

//  <type> != [bigbigint]
bool operator !=(const int &SubVal, bigbigint &MyVal);
bool operator !=(const unsigned int &SubVal, bigbigint &MyVal);
bool operator !=(const long &SubVal, bigbigint &MyVal);
bool operator !=(const unsigned long &SubVal, bigbigint& MyVal);
bool operator !=(const short &SubVal, bigbigint &MyVal);
bool operator !=(const unsigned short &SubVal, bigbigint& MyVal);
bool operator !=(const char &SubVal, bigbigint& MyVal);
bool operator !=(const unsigned char &SubVal, bigbigint &MyVal);
bool operator !=(const float &SubVal, bigbigint& MyVal);
bool operator !=(const double &SubVal, bigbigint &MyVal);

//  <type> | [bigbigint]
bigbigint operator |(int &SubVal,  bigbigint& MyVal);
bigbigint operator |(unsigned int &SubVal,  bigbigint& MyVal);
bigbigint operator |(long &SubVal,  bigbigint &MyVal);
bigbigint operator |(unsigned long &SubVal,  bigbigint &MyVal);
bigbigint operator |(short &SubVal,  bigbigint& MyVal);
bigbigint operator |(unsigned short &SubVal,  bigbigint &MyVal);
bigbigint operator |(char &SubVal,  bigbigint &MyVal);
bigbigint operator |(unsigned char &SubVal,  bigbigint &MyVal);
bigbigint operator |(float &SubVal,  bigbigint &MyVal);
bigbigint operator |(double &SubVal,  bigbigint vMyVal);



//-----------------------------------------------------------------------------
//                              Macros
//-----------------------------------------------------------------------------
//
//  This reverses the byte order of a value. 
//  The three XOR_Equal operations do the actual
//  flip-flop of the bytes.  As long as it's a type
//  on which we can do a SIZEOF(), this will work
#if !defined(MEM_ARCH_USES_BIG_ENDIAN)
    #define REVERSE_BYTE_ORDER(__value)         \
        {           \
            unsigned char *__b1, *__b2;   \
            __b1 = (unsigned char*)&__value; \
            __b2 = ((unsigned char*)&__value) + sizeof(__value) - 1;   \
            while(__b1 < __b2) {      \
                *__b1 ^= *__b2;       \
                *__b2 ^= *__b1;       \
                *__b1 ^= *__b2;       \
                __b1++; __b2--;}      \
        }
#else
    #define REVERSE_BYTE_ORDER(__value)
#endif


//
//  This performs a basic 1's compliment (bitwise NOT)
#define ONES_COMPLEMENT(__big_int_value)    \
    {                                       \
        unsigned long _i;                   \
        for(_i = 0; _i < __big_int_value._length; _i++) {               \
            ((BBI_BASE_TYPE*)__big_int_value._first_byte)[_i] =         \
                    ~((BBI_BASE_TYPE*)__big_int_value._first_byte)[_i]; \
        }                                   \
    }

//
//  Basic MIN and MAX macros
#ifndef MIN
#define MIN(val1, val2) ((val1) < (val2) ? (val1) : (val2))
#endif

#ifndef MAX
#define MAX(val1, val2) ((val1) > (val2) ? (val1) : (val2))
#endif

#ifndef ABS
    #define ABS(x)  ((x) < 0 ? -(x) : (x))
#endif

#endif