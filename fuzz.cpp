#include "sh.h"


#include <stddef.h> /* NULL size_t */
#include <string.h> /* memcmp */


static unsigned char const* g_in_data;
static size_t g_in_size;
static size_t g_in_cursor;
static unsigned char g_out_data[4 * 1024];
static size_t g_out_cursor;


#define crash() do{ int volatile* volatile ptr; __debugbreak(); ptr = NULL; *ptr = 0; }while(false)
#define test(x) do{ if(!(x)){ crash(); } }while(false)


unsigned int SPR_in(void)
{
	unsigned int r;

	if(g_in_cursor != g_in_size)
	{
		r = g_in_data[g_in_cursor];
		++g_in_cursor;
	}
	else
	{
		r = 0xffff;
	}
	return r;
}

void SPR_out(unsigned char const byte)
{
	test(g_out_cursor != sizeof(g_out_data));
	g_out_data[g_out_cursor] = byte;
	++g_out_cursor;
}


extern "C" int LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
	g_in_data = data;
	g_in_size = size;
	g_in_cursor = 0;
	g_out_cursor = 0;
	SPR_compress();
	test(g_in_cursor == g_in_size);
	SPR_decompress();
	test(g_out_cursor == g_in_size);
	test(memcmp(&g_out_data[0], g_in_data, g_in_size) == 0);
	return 0;
}
