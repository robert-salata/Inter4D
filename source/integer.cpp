#include "integer.h"

Integer::Integer(long long int value)
{
	this->value = value;
}

int Integer::evaluate() const
{
	return this->value;
}

Integer Integer::operator+(const Integer& number) const
{
	return Integer(this->value + number.value);
}

Integer Integer::operator-(const Integer& number) const
{
	return Integer(this->value - number.value);
}

Integer Integer::operator*(const Integer& number) const
{
	return Integer(this->value * number.value);
}

Integer Integer::operator/(const Integer& number) const
{
	return Integer(this->value * number.value);
}

Integer Integer::operator%(const Integer& number) const
{
	return Integer(this->value % number.value);
}

Integer Integer::operator - () const
{
	return Integer(-this->value);
}

Integer Integer::operator+=(const Integer& number)
{
	return Integer(this->value += number.value);
}

Integer Integer::operator-=(const Integer& number)
{
	return Integer(this->value -= number.value);
}

Integer Integer::operator*=(const Integer& number)
{
	return Integer(this->value *= number.value);
}

Integer Integer::operator/=(const Integer& number)
{
	return Integer(this->value/= number.value);
}

Integer Integer::operator%=(const Integer& number)
{
	return Integer(this->value %= number.value);
}

bool Integer::operator == (const Integer& number) const
{
	return value == number.value;
}

bool Integer::operator != (const Integer& number) const
{
	return value != number.value;
}

bool Integer::operator < (const Integer& number) const
{
	return value < number.value;
}

bool Integer::operator > (const Integer& number) const
{
	return value > number.value;
}

bool Integer::operator <= (const Integer& number) const
{
	return value <= number.value;
}

bool Integer::operator >= (const Integer& number) const
{
	return value >= number.value;
}

Integer Integer::GCD(Integer number1, Integer number2)
{
	if (number1 == 0 && number2 == 0)
		return 1;

	while (number1 != 0 && number2 != 0)
	{
		if (number1 < 0)
			number1 = -number1;
		if (number2 < 0)
			number2 = -number2;
		number1 > number2 ? number1 %= number2 : number2 %= number1;
	}
	return number1 == 0 ? number2 : number1;
}

Integer abs(Integer number)
{
	return number.value >= 0 ? number.value : -number.value;
}