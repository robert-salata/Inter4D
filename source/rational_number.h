#ifndef Rational_number
#define Rational_number

#include "integer.h"

class RationalNumber
{
public:
	RationalNumber(Integer nominator, Integer denominator);
	RationalNumber(Integer number);
	RationalNumber(double value);


	double evaluate() const;
	RationalNumber operator + (const RationalNumber& number) const;
	RationalNumber operator - (const RationalNumber& number) const;
	RationalNumber operator * (const RationalNumber& number) const;
	RationalNumber operator / (const RationalNumber& number) const;
	RationalNumber operator - () const;

	RationalNumber operator += (const RationalNumber& number);
	RationalNumber operator -= (const RationalNumber& number);
	RationalNumber operator *= (const RationalNumber& number);
	RationalNumber operator /= (const RationalNumber& number);

	bool operator == (const RationalNumber& number) const;
	bool operator < (const RationalNumber& number) const;
	bool operator > (const RationalNumber& number) const;
	bool operator <= (const RationalNumber& number) const;
	bool operator >= (const RationalNumber& number) const;
	bool operator != (const RationalNumber& number) const;

	friend RationalNumber abs(RationalNumber number);

	Integer numerator;
	Integer denominator;
};

RationalNumber operator / (Integer number1, Integer number2);
RationalNumber abs(RationalNumber number);

#endif