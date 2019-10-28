#include <stdio.h>
#include "build/test_cpp_payload.hpp"
int main(int argc, char const *argv[])
{
	printf("Payload size: %lld\n", payload.size());
	printf("Payload: %s\n",payload.data());
	return 0;
}