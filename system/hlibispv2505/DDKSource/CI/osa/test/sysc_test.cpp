#include "systemc.h"

extern "C" int test_main();

class test_top_level : public sc_module
{
public:
	test_top_level(sc_module_name name)
		: sc_module(name)
	{
		SC_THREAD(test_run);
	}

	SC_HAS_PROCESS(test_top_level);

	void test_run()
	{
		test_main();
	}
};


int sc_main(int argc, char** argv)
{
	test_top_level top_level("test_top_level_module");

	sc_start();

	return 0;
}