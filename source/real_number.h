#ifndef Real_number_class
#define Real_number_class

//#include "rational_number.h"
#include <math.h>
#include <cmath>

class RealNumber
{
public:
	//RealNumber(RationalNumber number) : rational_value(number) { }
	RealNumber(double value) : rational_value(value) { }
	RealNumber() : rational_value(0) { }

	double evaluate() const { return rational_value; }

	RealNumber operator + (const RealNumber& number) const { return rational_value + number.rational_value; }
	RealNumber operator - (const RealNumber& number) const { return rational_value - number.rational_value; }
	RealNumber operator * (const RealNumber& number) const { return rational_value * number.rational_value; }
	RealNumber operator / (const RealNumber& number) const { return rational_value / number.rational_value; }
	RealNumber operator - () const { return -rational_value; }

	RealNumber operator += (const RealNumber& number) { return rational_value += number.rational_value; }
	RealNumber operator -= (const RealNumber& number) { return rational_value -= number.rational_value; }
	RealNumber operator *= (const RealNumber& number) { return rational_value *= number.rational_value; }
	RealNumber operator /= (const RealNumber& number) { return rational_value /= number.rational_value; }

	bool operator == (const RealNumber& number) const { return abs(rational_value - number.rational_value) < epsilon; }
	bool operator < (const RealNumber& number) const { return rational_value < number.rational_value; }
	bool operator > (const RealNumber& number) const { return rational_value > number.rational_value; }
	bool operator <= (const RealNumber& number) const {	return rational_value < number.rational_value || *this == number; }
	bool operator >= (const RealNumber& number) const { return rational_value > number.rational_value || *this == number; }
	bool operator != (const RealNumber& number) const { return !(*this == number); }

private:
	static const double epsilon;
	double rational_value;
};

inline RealNumber operator + (double number1, const RealNumber& number2) { return RealNumber(number1) + number2; }
inline RealNumber operator - (double number1, const RealNumber& number2) { return RealNumber(number1) - number2; }
inline RealNumber operator * (double number1, const RealNumber& number2) { return RealNumber(number1) * number2; }
inline RealNumber operator / (double number1, const RealNumber& number2) { return RealNumber(number1) / number2; }

inline bool operator == (double number1, const RealNumber& number2) { return RealNumber(number1) == number2; }
inline bool operator < (double number1, const RealNumber& number2) { return RealNumber(number1) < number2; }
inline bool operator > (double number1, const RealNumber& number2) { return RealNumber(number1) > number2; }
inline bool operator <= (double number1, const RealNumber& number2) { return RealNumber(number1) <= number2; }
inline bool operator >= (double number1, const RealNumber& number2) { return RealNumber(number1) >= number2; }
inline bool operator != (double number1, const RealNumber& number2) { return RealNumber(number1) != number2; }

inline RealNumber abs(RealNumber number) { return abs(number.evaluate()); }
inline RealNumber sin(RealNumber number) { return sin(number.evaluate()); }
inline RealNumber cos(RealNumber number) { return cos(number.evaluate()); }
inline RealNumber tan(RealNumber number) { return cos(number.evaluate()); }
inline RealNumber asin(RealNumber number) { return asin(number.evaluate()); }
inline RealNumber acos(RealNumber number) { return acos(number.evaluate()); }
inline RealNumber atan(RealNumber number) { return atan(number.evaluate()); }
inline RealNumber sqrt(RealNumber number) { return sqrt(number.evaluate()); }

#endif
