

#include "HugeInteger.h"
#include <cstring>
#include <string>

using namespace std;

// ctor converts a long long into a HugeInteger
HugeInteger::HugeInteger( long long value )
{
	// set all MaxDigit digits to zero to start
	this->negative = false;
	if (value < 0LL){ // 0LL is constant literal 0 of type long long
		this->negative = true;
		value = - value; // make the value positive	                
	}
	
	this->hugeInt = "";
	
	// convert individual digits of input value into a HugeInteger
	for( size_t j = 0; j < hugeInt.max_size() && value != 0LL; j++ )
	{
		this->hugeInt += i2c(value % 10);
		value /= 10;
	}
	
	// no empty string
	if (this->hugeInt == "")
		this->hugeInt = "0";

	// test to make sure that HugeInteger was able to contain value

	if (value != 0LL){
		*this = 0LL; // set to -0, to signal overflow
	    this->negative = true; //   Possibly should increase value assigned
	}                          //   to MaxDigit to fix this problem.
}

// converts string into a HugeInteger object
HugeInteger::HugeInteger( const char *str )
{
	this->input( str ); //See HugeInteger::input() method below
}

// converts string into HugeInteger and then invokes
//    HugeInteger::operator +=(const HugeInteger & )
HugeInteger HugeInteger::operator +( const char *str ) const
{
	HugeInteger temp = *this;
	return temp +=( HugeInteger(str) );
}

// converts long long into HugeInteger and then invokes
//    HugeInteger::operator +=(const HugeInteger & )
HugeInteger HugeInteger::operator +( long long value ) const
{
	HugeInteger temp = *this;
	return temp +=( HugeInteger( value ) );
}

// Adds into the HugeInteger pointed to by the "this" pointer 
//   the HugeInteger op.
//   Then the calculated result is returned
HugeInteger & HugeInteger::operator +=( const HugeInteger &op )
{
	// if the signs of the 2 numbers are opposites, we need to do subtraction
	//     remember that x + y  ==  x - (-y)
	if ((this->negative && !(op.negative)) || (!(this->negative) && op.negative)){
		return this->operator -=(-op);
	}

	// NOTE: From here on, we know the two operands are the same sign
	unsigned short digit;
	string result = ""; // initialize result;
	unsigned short carry = 0;
	
	for (unsigned int i = 0; i < this->hugeInt.size() || i < op.hugeInt.size() ; ++i )
	{
		digit = carry;
		
		if (i < this->hugeInt.size())
			digit += c2i(this->hugeInt[i]);
		
		if (i < op.hugeInt.size())
			digit += c2i(op.hugeInt[i]);
		
		if (digit > 9 ){
			digit -= 10;
			carry = 1;
		}
		else {
			carry = 0;
		}
		
		result += i2c( digit ); // concatenate digit to result
	}

	this->hugeInt = result; 
	// test for overflow
	if (carry == 1){
		if (this->hugeInt.size() < this->hugeInt.max_size())
			this->hugeInt += '1';
		else
		{
			*this = 0LL; // just set to -0 (LL is for type long long)
			this->negative = true;  // to signal that an overflow occurred
		}
	}

	return *this;
}

// converts string into HugeInteger and then invokes
//    HugeInteger::operator -=(const HugeInteger & )
HugeInteger HugeInteger::operator -( const char *str ) const
{
	HugeInteger temp = *this;
	return temp -=( HugeInteger( str ) );
}

// converts long long into HugeInteger and then invokes
//    HugeInteger::operator -=(const HugeInteger & )
HugeInteger HugeInteger::operator -( long long value ) const
{
	HugeInteger temp = *this;
	return temp -=( HugeInteger( value ) );
}

// Subracts from the HugeInteger pointed to by the "this" pointer
//   the HugeInteger op
//   Then the calculated value is returned.
HugeInteger & HugeInteger::operator -=( const HugeInteger &op )
{
	// if the signs of the 2 numbers are opposites, we need to do addition
	//     remember that x - y  ==  x + (-y)
	if ((this->negative && !(op.negative)) || (!(this->negative) && op.negative)){
		return this->operator +=(-op);
	}

	// NOTE: From here on, we know the two operands are the same sign

	string bigger, smaller; // used to make code easier to understand

	if ((*this) == op){ // Are the values equal ?
		*this = 0LL;             // then just return 0
		this->negative = false;
		return *this;

	} else if ( (~op) < (~(*this))){ // is magnitude of LHS > RHS

		bigger = this->hugeInt;
		smaller = op.hugeInt;

	} else { // magnitude of RHS > LHS

		smaller = this->hugeInt;
		bigger = op.hugeInt;
		*this = -(*this); // result needs to be negated
	}

	// subtract smaller (in magnitude) from biggger (in magnitude)
	short borrow = 0;
	short top, bottom, result;
	this->hugeInt = ""; // initialize result
	
	for (unsigned int i = 0; i < bigger.size() ; ++i )
	{
		top = c2i(bigger[i]);
		if (i < smaller.size())
			bottom = c2i(smaller[i]);
		else
			bottom = 0;

		if (borrow == 1)
		{
			top -= 1;
			borrow = 0;
		}

		result  = top - bottom;
		if ( result < 0) //if true, we need to borrow
		{
			this->hugeInt += i2c(result + 10);
			borrow = 1;
		}
		else
		{
			this->hugeInt += i2c(result);
		}
	}
	
	// delete leading zeroes in high digits
	normalize();

	return *this;
}

// Computes the negation of a HugeInteger
HugeInteger HugeInteger::operator -(void)const
{
	HugeInteger temp = *this;
	temp.negative = !temp.negative;
	return temp;
}

// Computes and returns absolute value of operand
HugeInteger HugeInteger::operator ~(void)const
{
	HugeInteger temp = *this;
	temp.negative = false;
	return temp;
}

// Computes if two HugeIntegers are equal
bool HugeInteger::operator ==( const HugeInteger &op )const
{
	if (this->negative != op.negative){  // if not the same sign
		return false;                    //   they are not equal
	}
	
	if (this->hugeInt.size() != op.hugeInt.size())
		return false;
	
	for(unsigned int i = 0; i < hugeInt.size(); ++i )
	{
		if (this->hugeInt[i] != op.hugeInt[i])
		{
			return false;
		}
	}
	return true;
}

// Computes if one HugeInteger is less than another HugeInteger
bool HugeInteger::operator < ( const HugeInteger &op )const
{
	if (this->negative && !(op.negative)){ // if LHS < 0, and RHS is not < 0
		return true;  // then LHS must be less than RHS
	}

	if (!(this->negative) && op.negative){ // if LHS >= 0, and RHS < 0
		return false;  // then LHS must NOT be less than RHS
	}

	// NOTE: From here on, we know the two operands are the same sign
	if (this->hugeInt.size() < op.hugeInt.size())
		return !(this->negative);
	
	if (this->hugeInt.size() > op.hugeInt.size())
		return (this->negative);
	
	for( unsigned int i = hugeInt.size(); i > 0 ; --i )
	{
		if (this->hugeInt[i - 1] < op.hugeInt[i - 1])
		{
			return !(this->negative); // if both numbers are positive - true
									  // if both numbers are negative - false
		}
		else if (this->hugeInt[i - 1] > op.hugeInt[i - 1])
		{
			return (this->negative); // if both numbers are positive - false
									 // if both numbers are negative - true
		}
	}
	
	return false;
}

// Is_Zero operator
bool HugeInteger::operator !()const
{
	for (unsigned int i = 0; i < hugeInt.size(); ++i)
	{
		if ( this->hugeInt[ i ] != '0' )
		{
			return false;
		}
	}
	return true;
}

void HugeInteger::input( const char *str )
{
	// assume positive for now
	this->negative = false;

	// init. to empty string
	this->hugeInt = "";

	unsigned int len = static_cast<int>(strlen( str ));
	unsigned int k = 0;

	// if sign part of string, we need to process
	// if + sign, we ignore since we start with assumption that 
	//     input string represents a positive number
	if ((str[k] == '-') || (str[k] == '+')){ 
		if (str[k] == '-'){ // if negative, set negative member to true
			this->negative = true;
		}

		++k; // go to next char in string "str"
		--len; // length of number is one less
	}

	if (len > hugeInt.max_size()) {  // if true, too many digits
		this->negative = true; // return -0 to signal there
		return;				   // was a problem
	}
		
	for( unsigned int j = k; j < strlen( str ); ++j )
	{
		if (isdigit(str[j])){
			this->hugeInt = str[j] + this->hugeInt; // uppend characters in front of hugeInt
		}
		else  // a problem with the string input !!!
		{
			*this = 0LL; // just set to -0 (LL is for type long long)
			this->negative = true;  // to signal there was a problem
			break;     //   and go home
		}
	}
	
	// delete leading zeroes
	normalize();
}

// overload typecast to double
// I _STRONGLY_ recommend overloading this operator last
//     for the reasons we discussed in class.
HugeInteger::operator double(void)const
{
	double return_val = 0.0;

	// find first non-zero digit
	int i = hugeInt.size() - 1;
	while(i > 0 && hugeInt[i] == '0')
		--i;
	

	// process remaining digits
	for( ; i >=0 ; --i)
	{
		return_val *= 10;
		return_val += static_cast<double>( c2i(this->hugeInt[i] ) );
	}

	// check if we need to make it negative
	if (this->negative){
		return_val = -return_val;
	}

	return return_val;
}

// Pre-increment operator
HugeInteger & HugeInteger::operator ++ ()
{
	*this += 1LL;
	return (*this);
}

// Post-increment operator
HugeInteger HugeInteger::operator ++ (int)
{
	HugeInteger temp = *this;
	*this += 1LL;
	return temp;
}

// Pre-decrement operator
HugeInteger & HugeInteger::operator -- ()
{
	*this -= 1LL;
	return (*this);
}

// Post decrement operator
HugeInteger HugeInteger::operator -- (int)
{
	HugeInteger temp = *this;
	*this -= 1LL;
	return temp;
}

// PLEASE NOTE:  The remainder are are functions, not member functions!!!

// A couple of functions follow below

// overloads the >> operator as a friend function
istream & operator >> (istream & src, HugeInteger & value)
{
	string input_string; 
	                          
	src >> input_string;
	value.input(input_string.c_str());
	return src;
}

// overloads the << operator as a friend function
ostream & operator << (ostream & dest, const HugeInteger & value)
{
	// check if number is negative
	if (value.negative){
		dest << '-';
	}

	// output remaining digits
	for (unsigned int i = value.hugeInt.size(); i > 0; --i)
	{
		dest << value.hugeInt[i - 1];
	}

	return dest;
}

// Delete zeroes in high digits
void HugeInteger::normalize()
{
	unsigned int i = this->hugeInt.size();
	while (i > 1 && this->hugeInt[i - 1] == '0')
	{
		this->hugeInt.erase(i - 1, 1);
		--i;
	}
}

// converts character number to unsigned short
unsigned short c2i(char ch)
{
	return (unsigned short)(ch - '0');
}

// converts unsigned short number to character
char i2c(unsigned short n)
{
	return (char)('0' + n);
}

