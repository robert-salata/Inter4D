#include "rational_number.h"

RationalNumber::RationalNumber(Integer numerator, Integer denominator) : numerator(numerator), denominator(denominator)
{
	if (denominator < 0)
	{
		this->numerator = -numerator;
		this->denominator = -denominator;
	}

	Integer divident = Integer::GCD(numerator, denominator);
	this->numerator /= divident;
	this->denominator /= divident;
}

RationalNumber::RationalNumber(Integer number) : numerator(number), denominator(1)
{

}

RationalNumber::RationalNumber(double value) : RationalNumber((int)(value * 1000), 1000) //lazy solution
{

}


double RationalNumber::evaluate() const
{
	return (double)numerator.evaluate() / denominator.evaluate();
}

RationalNumber RationalNumber::operator + (const RationalNumber& number) const
{
	return RationalNumber(numerator*number.denominator + number.numerator*denominator, denominator*number.denominator);
}

RationalNumber RationalNumber::operator - (const RationalNumber& number) const
{
	return RationalNumber(numerator*number.denominator - number.numerator*denominator, denominator*number.denominator);
}

RationalNumber RationalNumber::operator * (const RationalNumber& number) const
{
	return RationalNumber(numerator*number.numerator, denominator*number.denominator);
}

RationalNumber RationalNumber::operator / (const RationalNumber& number) const
{
	return RationalNumber(numerator*number.denominator, denominator*number.numerator);
}

RationalNumber RationalNumber::operator - () const
{
	return RationalNumber(-numerator, denominator);
}

RationalNumber RationalNumber::operator += (const RationalNumber& number)
{
	RationalNumber result = *this + number;
	this->numerator = result.numerator;
	this->denominator = result.denominator;
	return result;
}

RationalNumber RationalNumber::operator -= (const RationalNumber& number)
{
	RationalNumber result = *this - number;
	this->numerator = result.numerator;
	this->denominator = result.denominator;
	return result;
}

RationalNumber RationalNumber::operator *= (const RationalNumber& number)
{
	RationalNumber result = *this * number;
	this->numerator = result.numerator;
	this->denominator = result.denominator;
	return result;
}

RationalNumber RationalNumber::operator /= (const RationalNumber& number)
{
	RationalNumber result = *this / number;
	this->numerator = result.numerator;
	this->denominator = result.denominator;
	return result;
}

bool RationalNumber::operator == (const RationalNumber& number) const
{
	return (numerator == 0 && number.numerator == 0) || numerator == number.numerator && denominator == number.denominator;
}

bool RationalNumber::operator < (const RationalNumber& number) const
{
	return numerator*number.denominator < number.numerator*denominator;
}

bool RationalNumber::operator > (const RationalNumber& number) const
{
	return numerator*number.denominator > number.numerator*denominator;
}

bool RationalNumber::operator <= (const RationalNumber& number) const
{
	return *this < number || *this == number;
}

bool RationalNumber::operator >= (const RationalNumber& number) const
{
	return *this > number || *this == number;
}

bool RationalNumber::operator != (const RationalNumber& number) const
{
	return !(*this == number);
}

RationalNumber operator / (Integer number1, Integer number2)
{
	return RationalNumber(number1, number2);
}

RationalNumber abs(RationalNumber number)
{
	return RationalNumber(abs(number.numerator), number.denominator);
}