#include "real_number.h"
const double RealNumber::epsilon = 0.000001;


/*#include <math.h>
#include <cmath>

RealNumber::RealNumber(RationalNumber number) : rational_value(number)
{

}

RealNumber::RealNumber(double value) : rational_value(value)
{

}

RealNumber::RealNumber() : rational_value(0)
{

}

inline double RealNumber::evaluate() const
{
	return rational_value;
}

inline RealNumber RealNumber::operator + (const RealNumber& number) const
{
	return rational_value + number.rational_value;
}

inline RealNumber RealNumber::operator - (const RealNumber& number) const
{
	return rational_value - number.rational_value;
}

inline RealNumber RealNumber::operator * (const RealNumber& number) const
{
	return rational_value * number.rational_value;
}

inline RealNumber RealNumber::operator / (const RealNumber& number) const
{
	return rational_value / number.rational_value;
}

inline RealNumber RealNumber::operator - () const
{
	return -rational_value;
}

inline RealNumber RealNumber::operator += (const RealNumber& number)
{
	return rational_value += number.rational_value;
}

inline RealNumber RealNumber::operator -= (const RealNumber& number)
{
	return rational_value -= number.rational_value;
}

inline RealNumber RealNumber::operator *= (const RealNumber& number)
{
	return rational_value *= number.rational_value;
}

inline RealNumber RealNumber::operator /= (const RealNumber& number)
{
	return rational_value /= number.rational_value;
}

inline bool RealNumber::operator == (const RealNumber& number) const
{
	return rational_value == number.rational_value;
}

inline bool RealNumber::operator < (const RealNumber& number) const
{
	return rational_value < number.rational_value;
}

inline bool RealNumber::operator > (const RealNumber& number) const
{
	return rational_value > number.rational_value;
}

inline bool RealNumber::operator <= (const RealNumber& number) const
{
	return rational_value <= number.rational_value;
}

inline bool RealNumber::operator >= (const RealNumber& number) const
{
	return rational_value >= number.rational_value;
}

inline bool RealNumber::operator != (const RealNumber& number) const
{
	return rational_value != number.rational_value;
}

inline RealNumber operator + (double number1, const RealNumber& number2)
{
	return RealNumber(number1) + number2;
}

inline RealNumber operator - (double number1, const RealNumber& number2)
{
	return RealNumber(number1) - number2;
}

inline RealNumber operator * (double number1, const RealNumber& number2)
{
	return RealNumber(number1) * number2;
}

inline RealNumber operator / (double number1, const RealNumber& number2)
{
	return RealNumber(number1) / number2;
}

inline bool operator == (double number1, const RealNumber& number2)
{
	return RealNumber(number1) == number2;
}

inline bool operator < (double number1, const RealNumber& number2)
{
	return RealNumber(number1) < number2;
}

inline bool operator > (double number1, const RealNumber& number2)
{
	return RealNumber(number1) > number2;
}

inline bool operator <= (double number1, const RealNumber& number2)
{
	return RealNumber(number1) <= number2;
}

inline bool operator >= (double number1, const RealNumber& number2)
{
	return RealNumber(number1) >= number2;
}

inline bool operator != (double number1, const RealNumber& number2)
{
	return RealNumber(number1) != number2;
}

inline RealNumber abs(RealNumber number)
{
	return abs(number.rational_value);
}

inline RealNumber sin(RealNumber number)
{
	return sin(number.evaluate());
}

inline RealNumber cos(RealNumber number)
{
	return cos(number.evaluate());
}

inline RealNumber tan(RealNumber number)
{
	return cos(number.evaluate());
}

inline RealNumber asin(RealNumber number)
{
	return asin(number.evaluate());
}

inline RealNumber acos(RealNumber number)
{
	return acos(number.evaluate());
}

inline RealNumber atan(RealNumber number)
{
	return atan(number.evaluate());
}

inline RealNumber sqrt(RealNumber number)
{
	return sqrt(number.evaluate());
}*/