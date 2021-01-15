#ifndef Timer_class
#define Timer_class

#include <time.h>

class Timer
{
public:
	Timer()
	{
		last = clock();
	}

	void measure()
	{
		double now = clock();
		times.push_back((double)(now - last) / CLOCKS_PER_SEC * 1000);
		last = now;
	}

private:
	double last;
	vector<double> times;
};


#endif