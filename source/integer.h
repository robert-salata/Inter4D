#ifndef Integer_class
#define Integer_class

class Integer
{
public:
	Integer(long long int value);

	int evaluate() const;
	Integer operator + (const Integer& number) const;
	Integer operator - (const Integer& number) const;
	Integer operator * (const Integer& number) const;
	Integer operator / (const Integer& number) const;
	Integer operator % (const Integer& number) const;
	Integer operator - () const;

	Integer operator += (const Integer& number);
	Integer operator -= (const Integer& number);
	Integer operator *= (const Integer& number);
	Integer operator /= (const Integer& number);
	Integer operator %= (const Integer& number);

	bool operator == (const Integer& number) const;
	bool operator < (const Integer& number) const;
	bool operator > (const Integer& number) const;
	bool operator <= (const Integer& number) const;
	bool operator >= (const Integer& number) const;
	bool operator != (const Integer& number) const;

	static Integer GCD(Integer number1, Integer number2);
	friend Integer abs(Integer number);

private:
	long long int value;
};

Integer abs(Integer number);

#endif