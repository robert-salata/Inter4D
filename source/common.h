#ifndef common_header
#define common_header

#include <string>

#define clip(val,min,max) (val<min?min:val>max?max:val)
#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define SafeDelete(a) {if(a){delete a; a=NULL;}}

namespace common
{
	template<typename T> void swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}
}
#endif