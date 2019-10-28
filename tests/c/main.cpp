#include <stdio.h>
#include "test_c_payload.hpp"
int main(int argc, char const *argv[])
{
	printf("Payload size: %lld\n", payload_size);
	printf("Payload: %s\n",payload);
	return 0;
}