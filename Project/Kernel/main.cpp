#include "Kernel.h"
#include <stdio.h>

#ifdef  WIN32
#include "CDumper.h"
class Dumper{
public:
	Dumper(){
		CDumper::GetInstance();
	}
};

static Dumper dumper;

#endif //  WIN32

void test1() {
	int *p;
	p = (int*)0x100;
	*p = 0; //写0x100地址, 这个是非法的
}

int main( int argc, char **argv )
{
	Kernel kernel;
    if ( kernel.Ready() )
    {
        if (kernel.Initialize(argc, argv))
        {
            kernel.Loop();
            kernel.Destroy();
        }
    }
    return 0;
}
