#include <cstdio>
#include <cstdlib>
#include <string>
#include "base_data.hpp"

using std::string;

int main(int argc, char const *argv[])
{
	BaseData base;
	char buffer[10000];
	if (argc == 1) {
		// printf(">> ");
	 	// while ( scanf("%[^;]", buffer) );
	} else if (argc > 1) {
		for (int i=1; i<argc; i++) {
			FILE* file = fopen(argv[i], "r");
			printf("\nUsing file: %s\n", argv[i]);
			while( fscanf(file, " %[^;];", buffer) > 0 ) {
				// printf(">> %s\n", buffer);
				base.Query(string(buffer));
			}
		}
	} else {
		fprintf(stderr, "Invalid use. Please use the following format instead.\n ./<executable> [<filename1> <filename2> ...]");
	}
	return 0;
}