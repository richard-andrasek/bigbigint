// ------------------------------------------------------------
//  BigBigInt.cpp
//
//  Created by Richard Andrasek
//
//  Purpose:
//      This module holds the procedures that make up the
//  bigbigint class.
//
// ------------------------------------------------------------
//  Note about Operator Overload Macros:
// ------------------------------------------------------------
//  The bodies for many of the overloaded operators (=, +, -, etc.)
//  are coded within a macro (one per operator).  The reason
//  for this is that it cleans up the code and prevents repeat
//  coding for each of the base types (int, long, etc.)
//
//  I considered using function templates to implement the
//  overloads for the different base types, however, I wasn't 
//  sure if templates would work with overloading operators.
//  Also, I've never liked the look of the code that uses 
//  templates, so I decided to make an individual function for 
//  each overloaded operator for each type.  That means
//  one function for each base type for each of the
//  different mathematic, comparison, and assignment operators.
//
//  Of course, having individual functions with the same code
//  (e.g. for int and long) was too much, so I stuck the bodies
//  of these functions inside of macros to simplify the code.
//  The macros are defined directly before the actual functions,
//  for readability.
//
//  I'm sure that the inner workings of macros are well known.  
//  If they're not, it'd be wise not to mess with this module
//  in any way.
//
// ------------------------------------------------------------
//  A note about Endianness
// ------------------------------------------------------------
//
//  For readability purposes, the BigBigInt class always uses
//  Big Endian.  Essentially, this means that the larger
//  numbers are stored on the left (like you'd normally see it
//  written).  Little endian, by comparison, stores the numbers
//  in reverse order.  The hexedecimal value of 0x1234 would
//  be stored as 0x3412.  0x12345678 would be 0x78563412.  (The
//  bytes are reversed, not the numbers--i.e. reverse byte order.)
//
//  Because some computer architecture stores them in little
//  endian format (which is the case with all Intel architecture),
//  we have to reverse the bytes in the integer before we store
//  them into our BigBigInt.
//
//  The following #define allows us to declare the architecture
//  as Big Endian, if we need to.  Simply uncomment the define
//  below and add a # sign.
//  
//  define MEM_ARCH_USES_BIG_ENDIAN true
//
//  Note: This must be defined before the BigBigInt.h header
//
//  Note: Only do this if you fully understand the impact.  
//  If you're on a standard Intel machine (which is probably  
//  the case), you don't want to mess with this.
// ------------------------------------------------------------
#include "BigBigInt.h"
#include <stdio.h>
#include <memory.h>


/*******************************************
 *       CONSTRUCTORS / DESTRUCTOR         *
 *******************************************/

bigbigint::bigbigint(bigbigint const& copy)
{
    this->copy((bigbigint*)&copy);
}

bigbigint::bigbigint(long size)
{
    this->_constructor(size);
}

bigbigint::bigbigint()
{
    this->_constructor(0);
}

bigbigint::~bigbigint()
{
    this->_free();
}


void bigbigint::_constructor(long size) 
{
    if(size < BBI_MIN_SIZE) 
        size = BBI_MIN_SIZE;
    // Setup internal vars
    this->_length = size;
    this->_num_bytes = size * sizeof(BBI_BASE_TYPE);

    // Malloc size.
    this->_malloc(this->_num_bytes);
}


/*******************************************
 *           INTERNAL FUNCTIONS            *
 *******************************************/

//
//  Malloc / Free (internal)
//
void bigbigint::_free()
{    
    free((void*)this->_first_byte);
    this->_first_byte = NULL;
    this->_num_bytes = 0;
    this->_length = 0;
}


void bigbigint::_malloc(unsigned long num_bytes)
{    
    this->_first_byte = (unsigned char*)malloc(num_bytes);
    if (this->_first_byte == NULL)
        exit(2);
}

//
//  UpSize (internal utility)
//
void bigbigint::_upsize(unsigned long new_length)
{
    unsigned char* save_ptr;
    unsigned long save_num_bytes, offset;

    save_ptr = (unsigned char*)this->_first_byte;
    save_num_bytes = this->_num_bytes;

    this->_length = new_length;
    this->_num_bytes = new_length * sizeof(BBI_BASE_TYPE);

    this->_malloc(this->_num_bytes);

    offset = this->_num_bytes - save_num_bytes;
    memset(this->_first_byte, 0, offset);
    memcpy(this->_first_byte + offset, save_ptr, save_num_bytes);
    
    free(save_ptr);
}

//
//  Zero Fill (internal utility)
//
void bigbigint::zero_fill(unsigned long num_byts_to_fill)
{
    if (num_byts_to_fill == 0) return;
    memset(this->_first_byte, '\0', num_byts_to_fill);
}

void bigbigint::zero_fill()
{
    memset(this->_first_byte, '\0', this->_num_bytes);
}


/*******************************************
 *           EXTERNAL FUNCTIONS            *
 *******************************************/

//
// Length function
//
long bigbigint::length()
{
    return this->_length;
}

//
// Copy function
//

bigbigint * bigbigint::copy(bigbigint *copy)
{
    // Setup internal vars
    this->_length = copy->_length;
    this->_num_bytes = copy->_num_bytes;
    this->_flags = copy->_flags;

    // Malloc size.
    this->_malloc(this->_num_bytes);

    memcpy((char *)this->_first_byte, 
        (char *)copy->_first_byte, 
        copy->_num_bytes);
    return this;
}

/*

// 
//  Print Function
//
void bigbigint::PrintToBuffer(char **outline)
{

    sprintf(*outline, "%d", this->_first_byte);

}
*/




/*******************************************
 *          OPERATOR OVERLOADING           *
 *******************************************/


// ------------------------------------------
// Function Set:  Operator = Overloading
// ------------------------------------------

//
//  Copying a bigbigint.
bigbigint bigbigint::operator =(const bigbigint &NewVal)
{
    unsigned long offset;

    if (this == &NewVal)  return *this;

    //  If it's just a straight copy, then do it quickly and forget about it.
    //  Otherwise, we need to Truncate / Zero Fill the value
    if (this->_num_bytes == NewVal._num_bytes) {
        memcpy(this->_first_byte, NewVal._first_byte, this->_num_bytes);
    }
    else if (this->_num_bytes < NewVal._num_bytes) {
        this->_free();
        this->copy((bigbigint *)&NewVal);
    }
    else {
        // Set the new value as the last bytes in the array.
        // zero fill the rest.
        offset = this->_num_bytes - NewVal._num_bytes;
        memcpy(((char *)this->_first_byte) + offset, 
            NewVal._first_byte, 
            NewVal._num_bytes);
        this->zero_fill(offset);
    }
    this->_flags = NewVal._flags;

    return *this;
}

//
//  This is the macro used to define the bodies for the overloading
//  of the assignement (=) operator for the integral-type base types
//  (int, long, unsigned int, short, etc.).  This is also used for
//  floating point base types after they have been rounded to an
//  integral-type value.
//
#define ASSIGN_OP_BODY_INT_TYPES(__var_name)   \
    unsigned long offset;           \
                                    \
    this->_flags = 0;               \
    if (__var_name < 0) {           \
        this->_flags |= BBI_NEGATIVE;   \
        __var_name = 0 - __var_name;    \
    }                               \
                                    \
    /* Clear the current value */   \
    this->zero_fill();              \
                                    \
    /* Flip the byte order for copying*/\
    REVERSE_BYTE_ORDER(__var_name); \
                                    \
    /* Copy over the new value in the correct offset.*/ \
    offset = this->_num_bytes - sizeof(__var_name); \
    memcpy( ((char *)this->_first_byte + offset),   \
        &__var_name, sizeof(__var_name));


bigbigint bigbigint::operator = (int NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (unsigned int NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (long NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (unsigned long NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (short NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (unsigned short NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (char NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (unsigned char NewVal)
{
    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (float NewFloat)
{
    D_LONG NewVal;

    NewVal = (D_LONG)NewFloat;

    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

bigbigint bigbigint::operator = (double NewDouble)
{
    D_LONG NewVal;

    NewVal = (D_LONG)NewDouble;

    ASSIGN_OP_BODY_INT_TYPES(NewVal);
    return *this;
}

// ------------------------------------------
// Function Set:  Operator <type> Overloading
// ------------------------------------------
#define CAST_OPERATOR_FUNCTION(__type)  \
    bigbigint::operator __type() const  \
    {                           \
        __type tmp_var;         \
                                \
        tmp_var = (__type) (this->_first_byte + (this->_num_bytes - sizeof(__type))); \
                                \
        if(IS_NEGATIVE(this->_flags)) {     \
            tmp_var = 0 - tmp_var;          \
        }                       \
        return tmp_var;       \
    }       

CAST_OPERATOR_FUNCTION(int);
CAST_OPERATOR_FUNCTION(unsigned int);
CAST_OPERATOR_FUNCTION(long);
CAST_OPERATOR_FUNCTION(unsigned long);
CAST_OPERATOR_FUNCTION(short);
CAST_OPERATOR_FUNCTION(unsigned short);
CAST_OPERATOR_FUNCTION(char);
CAST_OPERATOR_FUNCTION(unsigned char);

bigbigint::operator float() const  
{                                  
    long tmp_var;
    tmp_var = ((long)(this->_first_byte + (this->_num_bytes - sizeof(long))));
    return (float) tmp_var;  
}       
bigbigint::operator double() const  
{                                   
    long tmp_var;
    tmp_var = ((long)(this->_first_byte + (this->_num_bytes - sizeof(long))));
    return (double) tmp_var;
}       


// ------------------------------------------
// Function Set:  Operator + Overloading
// ------------------------------------------
bigbigint bigbigint::operator +(bigbigint PlusVal)
{
    unsigned char *this_val, *plus_val, *tval_val;
    bigbigint tVal(MAX(this->_length, PlusVal._length));
    long result;


    // Is there a single negative value?  If so, just subtract the
    // negative from the positive.
    // @RLA -- this needs to be (*this < 0)
    if(IS_NEGATIVE(this->_flags) && !IS_NEGATIVE(PlusVal._flags))
    {
        *this = PlusVal - (-*this);
        return *this;
    }
    else if(!IS_NEGATIVE(this->_flags) && IS_NEGATIVE(PlusVal._flags))
    {
        *this = *this - (-PlusVal);
        return *this;
    }

    tVal = 0;
    tVal._flags = this->_flags;

    // start by pointing to the last byte in the number (big endian, remember)
    this_val = (((unsigned char*) this->_first_byte) + this->_num_bytes - 1);
    plus_val = (((unsigned char*) PlusVal._first_byte) + PlusVal._num_bytes - 1);
    tval_val = (((unsigned char*) tVal._first_byte) + tVal._num_bytes - 1);

    result = 0;
    //
    // For each byte in each of the string of numbers, add the Plus and This
    while ((void*)this_val >= (void*)this->_first_byte 
        && (void*)plus_val >= (void*)PlusVal._first_byte) 
    {
        result += (long)*this_val + (long)*plus_val;

        *tval_val = (unsigned char)(result & UCHAR_MAX);
        if (result > UCHAR_MAX) {
            result >>= sizeof(char) * 8;
        }
        else {
            result = 0;
        }
        plus_val--;
        this_val--;
        tval_val--;
    }

//  Do we have something we need to carry over still?
    if (result > 0) {
        tVal._upsize(tVal._length + 1);
        *((unsigned char*)tVal._first_byte + (sizeof(BBI_BASE_TYPE) - 1)) =
                (char)(result & UCHAR_MAX);

    }

	return tVal;
}



bigbigint bigbigint::_perform_integral_adding(
        bigbigint * menuend,
        void* plus_var, 
        unsigned long plus_size,
        bool is_signed)
{
    bigbigint tVal(menuend->_length);
    unsigned char *p_menuend_val, *p_plus_val, *p_outp_val;  
    D_LONG add_value, result;

    tVal = *menuend;

    //
    // First off, we need to take the data and shove it into
    // a format we can read.  For this purpose, we just stick
    // everything in an __int64/double long so that the
    // machine can read it as it was meant to be.
#if defined(MEM_ARCH_USES_BIG_ENDIAN) 
    if((char*)plus_var >= 0  || is_signed == false)  
#else
    if((*((char*)plus_var + (plus_size-1)) >= 0) || is_signed == false)  
#endif
    {
        add_value = 0;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)add_value + (sizeof(add_value) - plus_size)),
                plus_var, plus_size);
        #else
            memcpy(&add_value, plus_var, plus_size);
        #endif
    }
    else
    {
        add_value = -1;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)add_value + (sizeof(add_value) - plus_size)),
                plus_var, plus_size);
        #else
            memcpy(&add_value, plus_var, plus_size);
        #endif

        // note: if p_plus_val is negative, so is *menuend 
        // since we're copying the flags from *menuend to the output_value
        // the output will automatically be negative (so we just need to add the
        // positive parts of the numbers (-5) + (-3) = (-8)
        add_value = 0 - add_value;
    }

    /* Flip the byte order for copying*/                        
    REVERSE_BYTE_ORDER(add_value);

    /* start by pointing to the last byte in the number (big endian) */ 
    p_menuend_val = (((unsigned char*) menuend->_first_byte) +      
                   menuend->_num_bytes - 1);                       
    p_outp_val = (((unsigned char*) tVal._first_byte) +   
                   tVal._num_bytes - 1);                    
    p_plus_val = (((unsigned char*) &add_value) +            
                   sizeof(add_value) - 1);                     

    result = 0; 
 
    /* For each byte in each of the string of numbers, */
    /* add the Plus and Menuend */
    while ((void*)p_menuend_val >= (void*)menuend->_first_byte)
    {
        if ((void*)p_plus_val >= (void*)&add_value)
        {
            result += (long)*p_menuend_val + 
                      (long)*p_plus_val;
            p_plus_val--;
        } else {
            if(result == 0)
            { break; }

            result += (long)*p_plus_val;
        }

        *p_outp_val = (char)(result & UCHAR_MAX);
        if (result > UCHAR_MAX) {
            result >>= sizeof(char); 
        }  
        else { 
            result = 0;
        } 
        p_menuend_val--; 
        p_outp_val--;
    }

    /* Do we have something we need to carry over still? */     
    if (result > 0) {
        tVal._upsize(tVal._length + 1);
        *((char*)tVal._first_byte + (sizeof(BBI_BASE_TYPE) - 1)) =
                (char)(result & UCHAR_MAX);

    }
    return tVal;
}

//
//      ------------------
//          @RLA -- THis needs to be integrated into the functions below
//          (via a macro, no doubt)
//      ------------------
//

/*          @RLA  -- uncomment!  (once the operator- is done)
    // Is there a single negative value?  If so, just subtract the
    // negative from the positive.
    // @RLA -- this needs to be (*this < 0)

    if(IS_NEGATIVE(this->_flags) && PlusVal > 0)
    {
        *this = PlusVal - *this;
        return *this;
    }
    else if(!IS_NEGATIVE(this->_flags) && PlusVal < 0)
    {
        *this = *this - PlusVal;
        return *this;
    }
*/

#define ADD_OPERATOR_MEMBER_FUNCTION(__type, __is_signed)    \
    bigbigint bigbigint::operator +(__type PlusVal) \
    {                                               \
	    return (this->_perform_integral_adding(     \
                this, (void*)&PlusVal, sizeof(PlusVal), __is_signed)); \
    }


ADD_OPERATOR_MEMBER_FUNCTION(int, true);
ADD_OPERATOR_MEMBER_FUNCTION(unsigned int, false);
ADD_OPERATOR_MEMBER_FUNCTION(long, true);
ADD_OPERATOR_MEMBER_FUNCTION(unsigned long, false);
ADD_OPERATOR_MEMBER_FUNCTION(short, true);
ADD_OPERATOR_MEMBER_FUNCTION(unsigned short, false);
ADD_OPERATOR_MEMBER_FUNCTION(char, true);
ADD_OPERATOR_MEMBER_FUNCTION(unsigned char, false);

bigbigint bigbigint::operator + (float PlusFloat)
{
    D_LONG PlusVal;
    PlusVal = (D_LONG)PlusFloat;

	return (_perform_integral_adding(this, (void*)&PlusVal, sizeof(PlusVal), true));
}


bigbigint bigbigint::operator + (double PlusDouble)
{
    D_LONG PlusVal;
    PlusVal = (D_LONG)PlusDouble;

	return (_perform_integral_adding(this, (void*)&PlusVal, sizeof(PlusVal), true));
}

// 
//  Same as above, but non-member functions for adding bigbigint's to <type>
//

#define ADD_OPERATOR_NON_MEMBER_FUNCTION(__type, __is_signed)    \
    bigbigint operator +(const __type &PlusVal, bigbigint &ValB)    \
    {                                               \
        return (bigbigint::_perform_integral_adding(    \
                &ValB, (void*)&PlusVal,             \
                sizeof(PlusVal),__is_signed));      \
    }

ADD_OPERATOR_NON_MEMBER_FUNCTION(int, true);
ADD_OPERATOR_NON_MEMBER_FUNCTION(unsigned int, false);
ADD_OPERATOR_NON_MEMBER_FUNCTION(long, true);
ADD_OPERATOR_NON_MEMBER_FUNCTION(unsigned long, false);
ADD_OPERATOR_NON_MEMBER_FUNCTION(short, true);
ADD_OPERATOR_NON_MEMBER_FUNCTION(unsigned short, false);
ADD_OPERATOR_NON_MEMBER_FUNCTION(char, true);
ADD_OPERATOR_NON_MEMBER_FUNCTION(unsigned char, false);

bigbigint operator +(const float &PlusFloat, bigbigint &ValB)
{
    D_LONG PlusVal;
    PlusVal = (D_LONG)PlusFloat;

    return (bigbigint::_perform_integral_adding(
            &ValB, (void*)&PlusVal, sizeof(PlusVal), true));
}

bigbigint operator +(const double &PlusDouble, bigbigint &ValB)
{
    D_LONG PlusVal;
    PlusVal = (D_LONG)PlusDouble;

    return (bigbigint::_perform_integral_adding(
                &ValB, (void*)&PlusVal, sizeof(PlusVal), true));
}


// ------------------------------------------
// Function Set:  Operator += Overloading
// ------------------------------------------
#define PLUS_EQ_OPERATOR_MEMBER_FUNCTION(__type)    \
    bigbigint bigbigint::operator +=(__type PlusVal) \
    {                   \
        *this = *this + PlusVal; \
        return *this;       \
    }

PLUS_EQ_OPERATOR_MEMBER_FUNCTION(bigbigint);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(int);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(long);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(short);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(char);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(float);
PLUS_EQ_OPERATOR_MEMBER_FUNCTION(double);


//
//  The += for non-member functions
//
#define PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type)    \
    __type operator +=(__type &PlusVal, bigbigint &ValB) \
    {           \
        PlusVal = PlusVal + (__type)ValB;   \
        return PlusVal; \
    }

PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(int);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(long);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(short);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(char);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(float);
PLUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(double);


// ------------------------------------------
// Function Set:  Operator - Overloading
// ------------------------------------------

//
//  Unary operator- Overload
//
bigbigint bigbigint::operator -()
{
    bigbigint tVal(this->_length);
    tVal = *this;
    tVal._flags ^= BBI_NEGATIVE;
    return tVal;
}

//
//  Binary operator- Overload
//
/*
    Subtraction algorithm:
        a) Take the 1's compliment of the subtrahend
        b) Add it to the menuend
        c) If there's a carry past the end of the number,
            add it to the result (it will be 1)
        d) If there's not a carry, take the 1's compliment of the result
            and flip the sign
*/
bigbigint bigbigint::operator -(bigbigint Subtrahend)
{
    unsigned char *p_tVal, *p_this;
    bigbigint tVal(MAX(this->_length, Subtrahend._length));
    long result;

    /*
     * Is there a single negative value?  If so, just add the
     * negative to the positive.
     */
    if(!IS_NEGATIVE(this->_flags) && IS_NEGATIVE(Subtrahend._flags) )
    {   // 5 - (-3) = 5+3
        tVal = *this + (-Subtrahend);
        return tVal;
    }
    else if(IS_NEGATIVE(this->_flags) && IS_NEGATIVE(Subtrahend._flags) )
    {   // -5 - (-3) = 3-5
        tVal = (-Subtrahend) - (-*this);
        return tVal;
    }
    else if(IS_NEGATIVE(this->_flags) && !IS_NEGATIVE(Subtrahend._flags) )
    {   // -5 - (3) = -5 + -3
        tVal = *this + (-Subtrahend);
        return tVal;
    }
    //  Positive && positive:  Process normal
    //  e.g.  5 - 3  

    tVal = Subtrahend;
    tVal._flags = this->_flags;

    // Set up the Pointers
    p_tVal = (((unsigned char*) tVal._first_byte) + tVal._num_bytes - 1);
    p_this = (((unsigned char*) this->_first_byte) + this->_num_bytes - 1);

    // Step 1:  Perform a 1's compliment of the subtrahend
    ONES_COMPLEMENT(tVal);

    result = 0;
    //
    // Step 2:  Add the Subtrahend to the Menuend
    while ((void*)p_this >= (void*)this->_first_byte 
        && (void*)p_tVal >= (void*)tVal._first_byte) 
    {
        result += (long)*p_this + (long)*p_tVal;

        *p_tVal = (unsigned char)(result & UCHAR_MAX);
        if (result > UCHAR_MAX) {
            result >>= sizeof(unsigned char) * 8;
        }
        else {
            result = 0;
        }
        p_tVal--;
        p_this--;
    }

    //
    // Step 3:  Handle the Carry
    //
    //     3a: If we have no carry: do a 1's compliment && flip the NEG flag
    //     3b: If we have do have one: add it to the result.
    if (result <= 0) {
        ONES_COMPLEMENT(tVal);

        if(IS_NEGATIVE(tVal._flags)) {
            tVal._flags &= ~BBI_NEGATIVE;
        }
        else {
            tVal._flags |= BBI_NEGATIVE;
        }
    }
    else {

        // Re-Set up the tVal Pointer
        p_tVal = (((unsigned char*) tVal._first_byte) + tVal._num_bytes - 1);

        // And add the remainder to the final number
        while ((void*)p_tVal >= (void*)tVal._first_byte) 
        {
            result += (long)*p_tVal;

            *p_tVal = (unsigned char)(result & UCHAR_MAX);
            if (result <= UCHAR_MAX) {
                break;
            }
            result >>= sizeof(unsigned char) * 8;
            p_tVal--;
        }
    }

	return tVal;
}



bigbigint bigbigint::_perform_integral_subtraction(
        bigbigint *this_val,
        void *Subtrahend,
        unsigned long sub_size,
        bool is_signed)
{
    unsigned char *p_tVal, *p_this;
    D_LONG result;
    long sub_value;
    bigbigint tVal(MAX(this_val->_num_bytes, sub_size));

    //
    // First off, we need to take the data and shove it into
    // a format we can read.  For this purpose, we just stick
    // everything in an __int64/double long so that the
    // machine can read it as it was meant to be.
#if defined(MEM_ARCH_USES_BIG_ENDIAN) 
    if((char*)Subtrahend >= 0  || is_signed == false)  
#else
    if((*((char*)Subtrahend + (sub_size-1)) >= 0) || is_signed == false)
#endif
    {
        sub_value = 0;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)sub_value + (sizeof(sub_value) - sub_size)),
                Subtrahend, sub_size);
        #else
            memcpy(&sub_value, Subtrahend, sub_size);
        #endif
    }
    else
    {
        sub_value = -1;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)sub_value + (sizeof(sub_value) - sub_size)),
                Subtrahend, sub_size);
        #else
            memcpy(&sub_value, Subtrahend, sub_size);
        #endif
    }

    /*
     * Is there a single negative value?  If so, just add the
     * negative to the positive.
     */
    if(!IS_NEGATIVE(this_val->_flags) && sub_value<0 )
    {   // 5 - (-3) = 5+3
        tVal = *this_val + (-sub_value);
        return tVal;
    }
    else if(IS_NEGATIVE(this_val->_flags) && sub_value<0 )
    {   // -5 - (-3) = 3-5
        tVal = (-sub_value) - ( -*this_val);
        return tVal;
    }
    else if(IS_NEGATIVE(this_val->_flags) && sub_value>0 )
    {   // -5 - (3) = -5 + -3
        tVal = *this_val + (-sub_value);
        return tVal;
    }
    // else:  Positive && Positive (process normally)
    //        e.g. 5-3  

    tVal = sub_value;
    tVal._flags = this_val->_flags;

    // Set up the Pointers
    p_tVal = (((unsigned char*) tVal._first_byte) + tVal._num_bytes - 1);
    p_this = (((unsigned char*) this_val->_first_byte) + this_val->_num_bytes - 1);

    // Step 1:  Perform a 1's compliment of the subtrahend
    ONES_COMPLEMENT(tVal);

    result = 0;
    //
    // Step 2:  Add the Subtrahend to the Menuend
    while ((void*)p_this >= (void*)this_val->_first_byte 
        && (void*)p_tVal >= (void*)tVal._first_byte) 
    {
        result += (long)*p_this + (long)*p_tVal;

        *p_tVal = (unsigned char)(result & UCHAR_MAX);
        if (result > UCHAR_MAX) {
            result >>= sizeof(unsigned char) * 8;
        }
        else {
            result = 0;
        }
        p_tVal--;
        p_this--;
    }

    //
    // Step 3:  Handle the Carry
    //
    //     3a: If we have no carry: do a 1's compliment && flip the NEG flag
    //     3b: If we have do have one: add it to the result.
    if (result <= 0) {
        ONES_COMPLEMENT(tVal);

        if(IS_NEGATIVE(tVal._flags)) {
            tVal._flags &= ~BBI_NEGATIVE;
        }
        else {
            tVal._flags |= BBI_NEGATIVE;
        }
    }
    else {

        // Re-Set up the tVal Pointer
        p_tVal = (((unsigned char*) tVal._first_byte) + tVal._num_bytes - 1);

        // And add the remainder to the final number
        while ((void*)p_tVal >= (void*)tVal._first_byte) 
        {
            result += (long)*p_tVal;

            *p_tVal = (unsigned char)(result & UCHAR_MAX);
            if (result <= UCHAR_MAX) {
                break;
            }
            result >>= sizeof(unsigned char) * 8;
            p_tVal--;
        }
    }
    return tVal;
}


#define SUBTRACT_OPERATOR_MEMBER_FUNCTION(__type,__is_signed)  \
    bigbigint bigbigint::operator -(__type Subtrahend)  \
    {                                                   \
        return (_perform_integral_subtraction(          \
                    this,                               \
                    (void*)&Subtrahend,                 \
                    sizeof(Subtrahend),                 \
                    __is_signed));                      \
    }

SUBTRACT_OPERATOR_MEMBER_FUNCTION(int, true);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(unsigned int, false);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(long, true);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(unsigned long, false);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(short, true);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(unsigned short, false);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(char, true);
SUBTRACT_OPERATOR_MEMBER_FUNCTION(unsigned char, false);

bigbigint bigbigint::operator -(float Subtrahend)  
{
    D_LONG tmp_val;
    tmp_val = (D_LONG)Subtrahend;
    return (_perform_integral_subtraction(
                this, (void*)&tmp_val, sizeof(tmp_val), true));
}
bigbigint bigbigint::operator -(double Subtrahend)  
{
    D_LONG tmp_val;
    tmp_val = (D_LONG)Subtrahend;
    return (_perform_integral_subtraction(
                this, (void*)&tmp_val, sizeof(tmp_val), true));
}


//
//  Non-Member Functions
//
#define SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(__type,__is_signed)    \
    bigbigint operator -(const __type &SubVal, bigbigint &Subtrahend)    \
    {                                               \
        return bigbigint::_perform_integral_subtraction(        \
                &Subtrahend, (void*)&SubVal, sizeof(SubVal),    \
                __is_signed);                                   \
    }


SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(int, true);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(unsigned int, false);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(long, true);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(unsigned long, false);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(short, true);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(unsigned short, false);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(char, true);
SUBTRACT_OPERATOR_NON_MEMBER_FUNCTION(unsigned char, false);

bigbigint operator -(const float &SubVal, bigbigint &Subtrahend)
{
    D_LONG tmp_val;
    tmp_val = (D_LONG)SubVal;
    return bigbigint::_perform_integral_subtraction(
            &Subtrahend, (void*)&tmp_val, sizeof(tmp_val), true);
}
bigbigint operator -(const double &SubVal, bigbigint &Subtrahend)
{
    D_LONG tmp_val;
    tmp_val = (D_LONG)SubVal;
    return bigbigint::_perform_integral_subtraction(
            &Subtrahend, (void*)&tmp_val, sizeof(tmp_val), true);
}



// ------------------------------------------
// Function Set:  Operator -= Overloading
// ------------------------------------------

#define MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(__type)    \
bigbigint bigbigint::operator -=(__type SubVal)              \
{                                                       \
    *this = *this - SubVal;                             \
    return *this;       \
}

MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(bigbigint);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(int);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned int);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(long);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned long);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(short);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned short);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(char);
MINUS_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned char);


//
//  Non-Member Functions
//
#define MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type)    \
    __type operator -=(__type &SubVal, bigbigint &ValB) \
    {               \
        SubVal = SubVal - (__type)ValB; \
        return SubVal; \
    }

MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(int);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(long);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(short);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(char);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(float);
MINUS_EQ_OPERATOR_NON_MEMBER_FUNCTION(double);


// ------------------------------------------
// Function Set:  Operator --/++ Overloading
// ------------------------------------------
bigbigint bigbigint::operator --(void)   //Prefix
{
    *this = *this - 1;
    return *this;
}

bigbigint bigbigint::operator ++(void)   
{
    *this = *this + 1;
    return *this;
}

bigbigint bigbigint::operator --(int)   //Postfix
{
    *this = *this - 1;
    return *this;
}

bigbigint bigbigint::operator ++(int)   
{
    *this = *this + 1;
    return *this;
}


// ------------------------------------------
// Function Set:  Operator * Overloading
// ------------------------------------------

//
//  Multiplication algorithm
//  
//  Note (to self): I am considering upgrading this to the
//  Toom-Cook divide-and-conquer algorithm.  This will 
//  increase the speed of multiplication for larger numbers 
//  (e.g. 50+ bytes each).  However, it will decrease the
//  speed for smaller numbers (int/double/etc.).  Still, 
//  it would be benefitial to pursue this in future releases.
//                  e.g.
//      if( this->length >= MIN_TOOM_LEN &&
//          that->length >= MIN_TOOM_LEN) 
//              return this->toom_multiple(that);
//
bigbigint bigbigint::operator *(const bigbigint &multiplier)
{
    bigbigint tVal(this->_length + multiplier._length);
    //bigbigint tmp_val(this->_length + multiplier._length);
    unsigned D_LONG result;
    BBI_BASE_TYPE *p_this, *p_mult, *p_tval;
    BBI_BASE_TYPE *p_this_lim, *p_mult_lim;
    BBI_BASE_TYPE byte_1, byte_2;
    int i;
    unsigned long this_length;

    tVal = 0;
    //
    //  Set up the pointers.
    //  Make p_this point to the larger of the two numbers
    if( this->_length >= multiplier._length) {
        p_this = ((BBI_BASE_TYPE*)this->_first_byte) + (this->_length-1);
        p_this_lim = ((BBI_BASE_TYPE*)this->_first_byte);
        p_mult = ((BBI_BASE_TYPE*)multiplier._first_byte) + (multiplier._length - 1);
        p_mult_lim = ((BBI_BASE_TYPE*)multiplier._first_byte);
        this_length = this->_length;
    }
    else {
        p_mult = ((BBI_BASE_TYPE*)this->_first_byte) + (this->_length-1);
        p_mult_lim = ((BBI_BASE_TYPE*)this->_first_byte);
        p_this = ((BBI_BASE_TYPE*)multiplier._first_byte) + (multiplier._length - 1);
        p_this_lim = ((BBI_BASE_TYPE*)multiplier._first_byte);
        this_length = multiplier._length;
    }

    i = 1;
    while(p_mult >= p_mult_lim) {

        p_tval = ((BBI_BASE_TYPE*)tVal._first_byte) + (tVal._length - i);
        p_this = p_this_lim + (this_length - 1);
        result = 0;
        while (p_this >= p_this_lim) {
            byte_1 = *p_mult;
            REVERSE_BYTE_ORDER(byte_1);
            byte_2 = *p_this;
            REVERSE_BYTE_ORDER(byte_2);

            result += (D_LONG)(byte_1) * (D_LONG)(byte_2);
            result += *p_tval;
            *p_tval = (BBI_BASE_TYPE)(result & BBI_BASE_MAX);
            result >>= (8 * sizeof(BBI_BASE_TYPE));

            p_tval--;
            p_this--;
        }
        p_mult--;
        i++;
    }
    
    if((IS_NEGATIVE(this->_flags) && !IS_NEGATIVE(multiplier._flags)) ||
        (!IS_NEGATIVE(this->_flags) && IS_NEGATIVE(multiplier._flags))) 
    {
        tVal._flags |= BBI_NEGATIVE;
    }

    return tVal;
}


bigbigint bigbigint::_perform_integral_multiplication(
        bigbigint * multiplicand,
        void *multiplier, 
        unsigned long mult_size,
        bool is_signed)
{
    bigbigint tVal(multiplicand->_length + 1);

    unsigned D_LONG result;
    D_LONG mul_value;
    BBI_BASE_TYPE *p_this, *p_mult, *p_tval;
    BBI_BASE_TYPE *p_this_lim, *p_mult_lim;
    BBI_BASE_TYPE byte_1, byte_2;
    int i;
    unsigned long this_length;
    unsigned char save_flags;

    tVal = 0;
    //
    // First off, we need to take the data and shove it into
    // a format we can read.  For this purpose, we just stick
    // everything in an __int64/double long so that the
    // machine can read it as it was meant to be.
#if defined(MEM_ARCH_USES_BIG_ENDIAN) 
    if((char*)multiplier >= 0 || is_signed == false)  
#else
    if((*((char*)multiplier + (mult_size-1)) >= 0) || is_signed == false)
#endif
    {
        mul_value = 0;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)mul_value + (sizeof(mul_value) - mult_size)),
                multiplier, mult_size);
        #else
            memcpy(&mul_value, multiplier, mult_size);
        #endif
        if(IS_NEGATIVE(multiplicand->_flags)) {
            tVal._flags |= BBI_NEGATIVE;
        }
    }
    else
    {
        mul_value = -1;
        #if defined(MEM_ARCH_USES_BIG_ENDIAN) 
            memcpy((char*)mul_value + (sizeof(mul_value) - mult_size)),
                multiplier, mult_size);
        #else
            memcpy(&mul_value, multiplier, mult_size);
        #endif
        if(!IS_NEGATIVE(multiplicand->_flags)) {
            tVal._flags |= BBI_NEGATIVE;
        }
        mul_value = 0-mul_value;
    }

    // Just a bit of a performance enhancement.
    if(mul_value == 1) { 
        save_flags = tVal._flags;
        tVal = *multiplicand;
        tVal._flags = save_flags;
        return tVal;
    }
    if(mul_value == 0) {
        return tVal;
    }

    //
    //  Set up the pointers.
    p_this = ((BBI_BASE_TYPE*)multiplicand->_first_byte) + (multiplicand->_length-1);
    p_this_lim = ((BBI_BASE_TYPE*)multiplicand->_first_byte);
    this_length = multiplicand->_length;
    p_mult = ((BBI_BASE_TYPE*)&mul_value) + (sizeof(mul_value) - 1);
    p_mult_lim = ((BBI_BASE_TYPE*)&mul_value);

    i = 1;
    while(p_mult >= p_mult_lim) {

        p_tval = ((BBI_BASE_TYPE*)tVal._first_byte) + (tVal._length - i);
        p_this = p_this_lim + (this_length - 1);
        result = 0;
        while (p_this >= p_this_lim) {
            byte_1 = *p_mult;
            REVERSE_BYTE_ORDER(byte_1);
            byte_2 = *p_this;
            REVERSE_BYTE_ORDER(byte_2);

            result += (D_LONG)(byte_1) * (D_LONG)(byte_2);
            result += *p_tval;
            *p_tval = (BBI_BASE_TYPE)(result & BBI_BASE_MAX);
            result >>= (8 * sizeof(BBI_BASE_TYPE));

            p_tval--;
            p_this--;
        }
        p_mult--;
        i++;
    }

    return tVal;
}


#define MULTIPLY_OPERATOR_MEMBER_FUNCTION(__type,__is_signed)  \
    bigbigint bigbigint::operator *(__type Multiplier)  \
    {                                                   \
        return (_perform_integral_multiplication(       \
                    this,                               \
                    (void*)&Multiplier,                 \
                    sizeof(Multiplier),                 \
                    __is_signed));                      \
    }

MULTIPLY_OPERATOR_MEMBER_FUNCTION(int, true);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(unsigned int, false);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(long, true);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(unsigned long, false);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(short, true);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(unsigned short, false);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(char, true);
MULTIPLY_OPERATOR_MEMBER_FUNCTION(unsigned char, false);

bigbigint bigbigint::operator *(float Multiplier)  
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    D_LONG tmp_val;
    tmp_val = (D_LONG)Multiplier;
    return (_perform_integral_multiplication(
                this, (void*)&tmp_val, sizeof(tmp_val), true));
}
bigbigint bigbigint::operator *(double Multiplier)  
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    D_LONG tmp_val;
    tmp_val = (D_LONG)Multiplier;
    return (_perform_integral_multiplication(
                this, (void*)&tmp_val, sizeof(tmp_val), true));
}

//
//  Non-Member functions
//
#define MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(__type,__is_signed)  \
    bigbigint operator *(const __type &PlusVal, bigbigint &ValB)  \
    {                                                   \
        return (bigbigint::_perform_integral_multiplication(   \
                    &ValB,                              \
                    (void*)&PlusVal,                    \
                    sizeof(PlusVal),                    \
                    __is_signed));                      \
    }

MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(int, true);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(unsigned int, false);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(long, true);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(unsigned long, false);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(short, true);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(unsigned short, false);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(char, true);
MULTIPLY_OPERATOR_NON_MEMBER_FUNCTION(unsigned char, false);

bigbigint operator *(const float Multiplier, bigbigint &ValB)  
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    D_LONG tmp_val;
    tmp_val = (D_LONG)Multiplier;
    return (bigbigint::_perform_integral_multiplication(
                &ValB, (void*)&tmp_val, sizeof(tmp_val), true));
}
bigbigint operator *(double Multiplier, bigbigint &ValB)  
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    D_LONG tmp_val;
    tmp_val = (D_LONG)Multiplier;
    return (bigbigint::_perform_integral_multiplication(
                &ValB, (void*)&tmp_val, sizeof(tmp_val), true));
}


// ------------------------------------------
// Function Set:  Operator *= Overloading
// ------------------------------------------
#define MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(__type)    \
    bigbigint bigbigint::operator *=(__type PlusVal) \
    {                   \
        *this = *this + PlusVal; \
        return *this;           \
    }

MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(bigbigint);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(int);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(long);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(short);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(char);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(float);
MULTIPLY_EQ_OPERATOR_MEMBER_FUNCTION(double);

//
//  The *= for non-member functions
//
#define MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type)    \
    __type operator *=(__type &PlusVal, bigbigint &ValB) \
    {                   \
        PlusVal = (PlusVal * (__type)ValB); \
        return (PlusVal); \
    }

MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(int);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(long);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(short);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(char);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(float);
MULTIPLY_EQ_OPERATOR_NON_MEMBER_FUNCTION(double);



// ------------------------------------------
// Function Set:  Operator / Overloading
// ------------------------------------------

//
//  Note: If we can use shift operators to perform the division
//
//  Note: we pass back the quotient and remainder
//  so that we can use this function for both the divide and mod 
//  functionality (since it performs both functions).
void bigbigint::_perform_integral_division(
        bigbigint dividend,
        bigbigint divisor,
        bigbigint *quotient,
        bigbigint *remainder )
{
    unsigned int next_bit;
    bigbigint sub_val;
    unsigned D_LONG num_bytes, num_bits;

    *remainder = 0;
    *quotient = 0;

    // These are some basic catches to make sure we don't
    // start bit-stomping unless we have to.
    if (divisor == 0)
    {
        exit(199);  // @RLA - Do we want to exit or allow it to continue running?
        return;
    }

    if (divisor > dividend) 
    {
        *remainder = dividend;
        return;
    }

    if (divisor == dividend) 
    {
        *quotient = 1;
        return;
    }

    if (dividend == 1) 
    {
        *quotient = dividend;
        return;
    }

    // need this to keep us from walking off the end of the heap
    if (dividend == 0)
    {
        return;
    }

    //
    // Quick little loop up top to shift off all the zeroes
    // from our dividend so that we can start making meaningful
    // evaluations.
    num_bytes = 0;
    num_bits = 0;

    next_bit = *(dividend._first_byte) & 0x80;
    while (next_bit == 0) {
        // find the next bit.
        dividend = dividend << 1;
        next_bit = *(dividend._first_byte) & 0x80;

        // Count the number of bits/bytes we've shifted off so far
        num_bits++;
        if(num_bits == 8){
            num_bits = 0;
            num_bytes++;
        }
    }

    // Subtract out the divisors from the dividend
    while(num_bytes < dividend._num_bytes) {
        // Get the next bit to subtract, adding the bit to the remainder
        next_bit = (*(dividend._first_byte) & 0x80) >> 8;
        *remainder <<= 1;
        *remainder |= next_bit;

        // try subtracting the divisor from current value (*remainder)
        sub_val = *remainder - divisor;

        // bump these so we can process the next bit
        dividend <<= 1;
        *quotient <<= 1;

        // was there enough of a remainder to count as a divisor?
        if( sub_val < 0 ) 
        {
            // Yes. Subtraction resulted in a positive value.  
            // Add a bit to our quotient and save the subtraction
            // value as our current remainder
            *quotient |= 1;
            *remainder = sub_val;
        }

        // while we're here, go ahead and make sure the quotient has
        // enough bytes in it
        next_bit = (*(quotient->_first_byte) & 0x80);
        if(next_bit != 0) {
            quotient->_upsize(quotient->_length+1);
        }

        // maintain the bit/byte counters.
        num_bits++;
        if(num_bits == 8) {
            num_bits = 0;
            num_bytes++;
        }
    }

    return;
}  


bigbigint bigbigint::operator /(bigbigint divisor)
{
    bigbigint tQuot, tRem;
    this->_perform_integral_division(*this, divisor, &tQuot, &tRem);
    return tQuot;
}

#define BBI_DIVISION_SHIFT_SHORTCUT(__var)  \
    switch (__var)  \
    {   \
        case 2: return (*this>>1);  \
        case 4: return (*this>>2);  \
        case 8: return (*this>>3);  \
        case 16: return(*this>>4);  \
        case 32: return(*this>>5);  \
        case 64: return(*this>>6);  \
        case 128: return(*this>>7); \
        case 256: return(*this>>8); \
        case 512: return(*this>>9); \
        case 1024: return(*this>>10);\
        case 2048: return(*this>>11);\
        case 4096: return(*this>>12);\
        default: break;             \
    }

#define DIVIDE_OPERATOR_MEMBER_FUNCTION(__type) \
bigbigint bigbigint::operator /(__type divisor) \
{                                               \
    BBI_DIVISION_SHIFT_SHORTCUT(divisor);       \
    bigbigint tVal(this->_length);              \
    tVal = divisor;                             \
    return (*this / tVal);                      \
}


DIVIDE_OPERATOR_MEMBER_FUNCTION(int)
DIVIDE_OPERATOR_MEMBER_FUNCTION(unsigned int);
DIVIDE_OPERATOR_MEMBER_FUNCTION(long)
DIVIDE_OPERATOR_MEMBER_FUNCTION(unsigned long);
DIVIDE_OPERATOR_MEMBER_FUNCTION(short)
DIVIDE_OPERATOR_MEMBER_FUNCTION(unsigned short);
DIVIDE_OPERATOR_MEMBER_FUNCTION(char)
DIVIDE_OPERATOR_MEMBER_FUNCTION(unsigned char);

bigbigint bigbigint::operator /(float divisor) 
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    long dl_divisor;
    dl_divisor = (long)divisor;

    BBI_DIVISION_SHIFT_SHORTCUT(dl_divisor);
    bigbigint tVal(this->_length);
    tVal = divisor;
    return (*this / tVal);
}

bigbigint bigbigint::operator /(double divisor) 
{
    //@RLA - This is very, very incorrect
    //  (0.25 * 100 = 25... not 0)
    long dl_divisor;
    dl_divisor = (long)divisor;

    BBI_DIVISION_SHIFT_SHORTCUT(dl_divisor);
    bigbigint tVal(this->_length);
    tVal = divisor;
    return (*this / tVal);
}

#define DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(__type) \
bigbigint operator /(__type &dividend, bigbigint &divisor) \
{                                               \
    bigbigint tVal(divisor.length());           \
    tVal = dividend;                            \
    tVal = tVal / divisor;      \
    return tVal;                \
}


DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(int)
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(long)
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(short)
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(char)
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(float)
DIVIDE_OPERATOR_NON_MEMBER_FUNCTION(double)

// ------------------------------------------
// Function Set:  Operator /= Overloading
// ------------------------------------------
#define DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(__type) \
bigbigint bigbigint::operator /=(__type divisor) \
{                                               \
    *this = *this / divisor;        \
    return (*this);                 \
}


DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(bigbigint)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(int)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(long)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(short)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(char)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(float)
DIVIDE_EQ_OPERATOR_MEMBER_FUNCTION(double)


#define DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type) \
__type operator /=(__type &dividend, bigbigint &divisor) \
{                                               \
    bigbigint tVal(divisor.length());           \
    tVal = dividend;                            \
    tVal = tVal / divisor;      \
    dividend = (__type)tVal;    \
    return dividend;             \
}


DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(int)
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(long)
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(short)
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(char)
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(float)
DIVIDE_EQ_OPERATOR_NON_MEMBER_FUNCTION(double)


// ------------------------------------------
// Function Set:  Operator > Overloading
// ------------------------------------------
bool bigbigint::operator >(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result > 0 ? true : false);
}


#define GREATER_THAN_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator >(__type CompVal)              \
{                                                       \
    int comp_result;                                    \
    if( CompVal < 0 && !IS_NEGATIVE(this->_flags)) {    \
        return true;        \
    }                                                   \
    {                                                   \
        bigbigint tVal(this->_length);                      \
        tVal = CompVal;                                     \
        comp_result = memcmp(this->_first_byte,             \
                      tVal._first_byte, tVal._num_bytes);   \
    }                                                   \
	return (comp_result > 0 ? true : false);            \
}

GREATER_THAN_OPERATOR_MEMBER_FUNCTION(int)
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(unsigned int);
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(long)
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(unsigned long);
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(short)
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(unsigned short);
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(char)
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(unsigned char);
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(float)
GREATER_THAN_OPERATOR_MEMBER_FUNCTION(double)


#define GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator >(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value <= CompVal);                     \
}


GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(int);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(long);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(short);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(char);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(float);
GREATER_THAN_OPERATOR_NON_MEMBER_FUNCTION(double);



// ------------------------------------------
// Function Set:  Operator >= Overloading
// ------------------------------------------
bool bigbigint::operator >=(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result >= 0 ? true : false);
}


#define GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator >=(__type CompVal)             \
{                                                       \
    int comp_result;                                    \
    if( CompVal == 0 && !IS_NEGATIVE(this->_flags)) {   \
        return true;                                    \
    }                                                   \
    {                                                   \
        bigbigint tVal(this->_length);                      \
        tVal = CompVal;                                     \
        comp_result = memcmp(this->_first_byte,             \
                      tVal._first_byte, tVal._num_bytes);   \
    }                                                   \
	return (comp_result >= 0 ? true : false);           \
}

GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(int)
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(long)
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(short)
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(char)
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(float)
GREATER_THAN_EQ_OPERATOR_MEMBER_FUNCTION(double)


#define GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator >=(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value < CompVal);                     \
}


GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(int);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(long);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(short);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(char);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(float);
GREATER_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(double);



// ------------------------------------------
// Function Set:  Operator < Overloading
// ------------------------------------------
bool bigbigint::operator <(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result < 0 ? true : false);
}


#define LESS_THAN_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator <(__type CompVal)              \
{                                                       \
    int comp_result;                                    \
    if( CompVal == 0 ) {                                \
        return (IS_NEGATIVE(this->_flags) != 0);        \
    }                                                   \
    {                                                   \
        bigbigint tVal(this->_length);                  \
        tVal = CompVal;                                 \
        comp_result = memcmp(this->_first_byte,         \
                      tVal._first_byte, tVal._num_bytes);   \
    }                                                   \
	return (comp_result < 0 ? true : false);            \
}

LESS_THAN_OPERATOR_MEMBER_FUNCTION(int)
LESS_THAN_OPERATOR_MEMBER_FUNCTION(unsigned int);
LESS_THAN_OPERATOR_MEMBER_FUNCTION(long)
LESS_THAN_OPERATOR_MEMBER_FUNCTION(unsigned long);
LESS_THAN_OPERATOR_MEMBER_FUNCTION(short)
LESS_THAN_OPERATOR_MEMBER_FUNCTION(unsigned short);
LESS_THAN_OPERATOR_MEMBER_FUNCTION(char)
LESS_THAN_OPERATOR_MEMBER_FUNCTION(unsigned char);
LESS_THAN_OPERATOR_MEMBER_FUNCTION(float)
LESS_THAN_OPERATOR_MEMBER_FUNCTION(double)


#define LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator <(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value >= CompVal);                     \
}


LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(int);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(long);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(short);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(char);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(float);
LESS_THAN_OPERATOR_NON_MEMBER_FUNCTION(double);




// ------------------------------------------
// Function Set:  Operator <= Overloading
// ------------------------------------------
bool bigbigint::operator <=(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result <= 0 ? true : false);
}


#define LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator <=(__type CompVal)              \
{                                                       \
    int comp_result;                                    \
    if( CompVal == 0 && IS_NEGATIVE(this->_flags)) {    \
        return true;                                    \
    }                                                   \
    {                                                   \
        bigbigint tVal(this->_length);                      \
        tVal = CompVal;                                     \
        comp_result = memcmp(this->_first_byte,             \
                      tVal._first_byte, tVal._num_bytes);   \
    }                                                    \
	return (comp_result <= 0 ? true : false);            \
}

LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(int)
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(long)
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(short)
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(char)
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(float)
LESS_THAN_EQ_OPERATOR_MEMBER_FUNCTION(double)


#define LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator <=(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value > CompVal);                     \
}


LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(int);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(long);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(short);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(char);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(float);
LESS_THAN_EQ_OPERATOR_NON_MEMBER_FUNCTION(double);



// ------------------------------------------
// Function Set:  Operator == Overloading
// ------------------------------------------
bool bigbigint::operator ==(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result == 0 ? true : false);
}


#define EQUALITY_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator ==(__type CompVal)              \
{                                                       \
    int comp_result;                                    \
    bigbigint tVal(this->_length);                      \
    tVal = CompVal;                                     \
    comp_result = memcmp(this->_first_byte,             \
                  tVal._first_byte, tVal._num_bytes);   \
	return (comp_result == 0 ? true : false);            \
}

EQUALITY_OPERATOR_MEMBER_FUNCTION(int)
EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned int);
EQUALITY_OPERATOR_MEMBER_FUNCTION(long)
EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned long);
EQUALITY_OPERATOR_MEMBER_FUNCTION(short)
EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned short);
EQUALITY_OPERATOR_MEMBER_FUNCTION(char)
EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned char);
EQUALITY_OPERATOR_MEMBER_FUNCTION(float)
EQUALITY_OPERATOR_MEMBER_FUNCTION(double)


#define EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator ==(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value == CompVal);                     \
}

EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(int);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(long);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(short);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(char);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(float);
EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(double);


// ------------------------------------------
// Function Set:  Operator != Overloading
// ------------------------------------------
bool bigbigint::operator !=(bigbigint CompVal)
{
    int comp_result;
    if(CompVal._length > this->_length) 
    {
        bigbigint tVal(CompVal._length);
        tVal = *this;
        comp_result = memcmp(tVal._first_byte, CompVal._first_byte, CompVal._num_bytes);
    }
    else if(this->_length > CompVal._length) 
    {
        bigbigint tVal(this->_length);
        tVal = CompVal;
        comp_result = memcmp(this->_first_byte, tVal._first_byte, tVal._num_bytes);
    }
    else
    {
        comp_result = memcmp(this->_first_byte, CompVal._first_byte, CompVal._num_bytes);
    }

	return (comp_result != 0 ? true : false);
}


#define NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(__type)   \
bool bigbigint::operator !=(__type CompVal)              \
{                                                       \
    int comp_result;                                    \
    bigbigint tVal(this->_length);                      \
    tVal = CompVal;                                     \
    comp_result = memcmp(this->_first_byte,             \
                  tVal._first_byte, tVal._num_bytes);   \
	return (comp_result != 0 ? true : false);            \
}

NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(int)
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned int);
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(long)
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned long);
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(short)
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned short);
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(char)
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(unsigned char);
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(float)
NON_EQUALITY_OPERATOR_MEMBER_FUNCTION(double)


#define NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(__type)   \
bool operator !=(const __type &CompVal, bigbigint &this_value) \
{                                                       \
	return (this_value != CompVal);                     \
}

NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(int);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(long);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(short);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(char);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(float);
NON_EQUALITY_OPERATOR_NON_MEMBER_FUNCTION(double);




// ------------------------------------------
// Function Set:  Operator ! Overloading
// ------------------------------------------
bool bigbigint::operator !()
{
	return (this == 0 ? true : false);
}



// ------------------------------------------
// Function Set:  Operator << Overloading
// ------------------------------------------

bigbigint bigbigint::operator << (unsigned long Shift)
{
    bigbigint tVal(this->_length);
    unsigned long curr_byte, offset;
    unsigned char *p_store, *p_load, *p_last_byte;
    int shift_value;

    tVal = 0;

    //Always start storing with the first byte
    p_store = tVal._first_byte;

    //Load based on the offset (shift_value)
    offset = 0;
    offset = Shift / 8;
    shift_value = Shift-1 % 8;

    p_load = ((unsigned char*)this->_first_byte) + offset;

    // If no actual shifting will take place, just do a nice little memcpy    
    if(shift_value == 0) {
        memcpy(p_store, p_load, (this->_num_bytes - offset));
        return tVal;
    }

    p_last_byte = (this->_first_byte + this->_num_bytes) - 1;
    while(p_load <= p_last_byte) {
        
        // The idea here is that we grab two bytes
        // so that any bits in the next byte are correctly
        // shifted up to the current byte.
        // (Then shift it down 8 bits so that we can store the byte)
        // 
        // Note: we have to load up each byte separately since
        // the endianness of the architecture may (or may not) flip
        // the two bytes.  thank you intel.
        curr_byte = *p_load;
        curr_byte <<= 8;
        if(p_load < p_last_byte) {
            curr_byte |= *(p_load+1);
        }
        curr_byte <<= shift_value;
        curr_byte >>= 8;
        *p_store = (unsigned char)(curr_byte & 0xFF);

        p_store++;
        p_load++;
    }

    return tVal;
}

#define LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(__type)   \
bigbigint bigbigint::operator << (__type Shift)     \
{                                                   \
    if(Shift < 0) {                                 \
        return (*this >> (unsigned long)(0-Shift)); \
    }                                               \
    return (*this << (unsigned long)Shift);         \
}

LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(int)
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned int);
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(long)
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(short)
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned short);
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(char)
LEFT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned char);

//
// No Non-member functions for this operator
// <int> << <bigbigint> not allowed
//

// ------------------------------------------
// Function Set:  Operator <<= Overloading
// ------------------------------------------
#define LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(__type)   \
bigbigint bigbigint::operator <<= (__type Shift)    \
{                                                   \
    *this = *this << Shift;                         \
    return *this;                                   \
}

LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(int)
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned int);
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(long)
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned long);
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(short)
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned short);
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(char)
LEFT_SHIFT_EQUAL_OPERATOR_MEMBER_FUNCTION(unsigned char);

//
// No Non-member functions for this operator
// <int> <<= <bigbigint> not allowed
//


// ------------------------------------------
// Function Set:  Operator >> Overloading
// ------------------------------------------
//@RLA


bigbigint bigbigint::operator >> (unsigned long Shift)
{
    bigbigint tVal(this->_length);
    unsigned long curr_byte, offset;
    unsigned char *p_store, *p_load, *p_last_byte;
    int shift_value;

    tVal = 0;


    //Load based on the offset (shift_value)
    offset = 0;
    offset = Shift / 8;
    shift_value = Shift-1 % 8;

    // If no actual shifting will take place, just do a nice little memcpy    
    if(shift_value == 0) {
        memcpy(tVal._first_byte + offset,this->_first_byte, 
                (this->_num_bytes - offset));
        return tVal;
    }

    //Set up the pointers
    p_store = tVal._first_byte + offset;
    p_load = this->_first_byte;
    p_last_byte = this->_first_byte + ((this->_num_bytes - offset)-1);

    while(p_load <= p_last_byte) {
        // The idea here is that we grab two bytes
        // so that any bits in the previous byte are correctly
        // shifted down to the current byte.
        // 
        if(p_load == this->_first_byte) {
            curr_byte = 0;
        }
        else
        {
            curr_byte |= *(p_load-1);
            curr_byte <<= 8;
        }
        curr_byte |= *p_load;
        curr_byte >>= shift_value;
        *p_store = (unsigned char)(curr_byte & 0xFF);

        p_store++;
        p_load++;
    }

    return tVal;
}


#define RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(__type)   \
bigbigint bigbigint::operator >> (__type Shift)     \
{                                                   \
    if(Shift < 0) {                                 \
        return (*this << (unsigned long)(0-Shift)); \
    }                                               \
    return (*this >> (unsigned long)Shift);         \
}

RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(int)
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned int);
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(long)
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(short)
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned short);
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(char)
RIGHT_SHIFT_OPERATOR_MEMBER_FUNCTION(unsigned char);

// ------------------------------------------
// Function Set:  Operator >>= Overloading
// ------------------------------------------
#define RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(__type)   \
bigbigint bigbigint::operator >>= (__type Shift)    \
{                                                   \
    *this = *this >> Shift;                         \
    return *this;                                   \
}

RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(int)
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(long)
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(short)
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(char)
RIGHT_SHIFT_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);



// ------------------------------------------
// Function Set:  Operator | Overloading
// ------------------------------------------
bigbigint bigbigint::operator | (bigbigint OrVal)
{
    bigbigint tVal(this->_length);
    unsigned long i;

    tVal = 0;
    for(i = 0; tVal._num_bytes; i++) {
        tVal._first_byte[i] = this->_first_byte[i] | OrVal._first_byte[i];
    }

    return tVal;
}


#define BIT_OR_OPERATOR_MEMBER_FUNCTION(__type)         \
bigbigint bigbigint::operator |(__type OrVal)                \
{                                                       \
    bigbigint tVal(this->_length);                      \
    tVal = OrVal;                                       \
	return (*this | tVal);                              \
}

BIT_OR_OPERATOR_MEMBER_FUNCTION(int)
BIT_OR_OPERATOR_MEMBER_FUNCTION(unsigned int);
BIT_OR_OPERATOR_MEMBER_FUNCTION(long)
BIT_OR_OPERATOR_MEMBER_FUNCTION(unsigned long);
BIT_OR_OPERATOR_MEMBER_FUNCTION(short)
BIT_OR_OPERATOR_MEMBER_FUNCTION(unsigned short);
BIT_OR_OPERATOR_MEMBER_FUNCTION(char)
BIT_OR_OPERATOR_MEMBER_FUNCTION(unsigned char);
BIT_OR_OPERATOR_MEMBER_FUNCTION(float)
BIT_OR_OPERATOR_MEMBER_FUNCTION(double)



#define BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(__type)     \
bigbigint operator |(__type &OrVal, bigbigint &big_val)   \
{                                                       \
    bigbigint tVal(big_val.length());                   \
    tVal = OrVal;                                       \
	return (tVal | big_val);                            \
}

BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(int)
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(unsigned int);
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(long)
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(unsigned long);
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(short)
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(unsigned short);
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(char)
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(unsigned char);
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(float)
BIT_OR_OPERATOR_NON_MEMBER_FUNCTION(double)


// ------------------------------------------
// Function Set:  Operator |= Overloading
// ------------------------------------------
#define BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(__type)      \
bigbigint bigbigint::operator |=(__type OrVal)          \
{                                                       \
    *this = *this | OrVal;                              \
	return (*this);                                     \
}

BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(bigbigint)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(int)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(unsigned int);
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(long)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(unsigned long);
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(short)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(unsigned short);
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(char)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(unsigned char);
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(float)
BIT_OR_EQ_OPERATOR_MEMBER_FUNCTION(double)


// ------------------------------------------
// Function Set:  Operator & Overloading
// ------------------------------------------
