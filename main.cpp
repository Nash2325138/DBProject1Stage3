#include <cstdio>
#include <cstdlib>
#include <string>
#include "base_data.hpp"
#include "color.h"

using std::string;

int main(int argc, char const *argv[])
{
	BaseData base;
	const int MAXSIZE = 10000;
	char buffer[MAXSIZE*2];
	if (argc == 1) {
		int pos = 0, num_chars;
		char oneQuery[MAXSIZE];
		char temp[MAXSIZE];

		oneQuery[0] = '\0';
		printf(">> ");
	 	while ( fgets(buffer, MAXSIZE*2, stdin) != NULL ) {
	 		num_chars = 0;
	 		pos = 0;

	 		while( sscanf(buffer+pos, "%[^;]%n", temp, &num_chars) > 0) {
	 			// printf("!\n");
	 			pos += num_chars;
	 			strcat(oneQuery,temp);
		 		if (buffer[pos] == ';') {
		 			// printf("%s\n", oneQuery);
		 			base.Query(string(oneQuery));
		 			oneQuery[0] = '\0';
		 			pos++;
		 			if (buffer[pos] == '\n') {
		 				printf(">> ");
		 				break;
		 			}
		 		} else {
		 			printf("-> ");
		 			break;
		 		}
		 		num_chars = 0;
	 		}
	 		if (num_chars == 0) {
	 			base.Query(string(oneQuery));
		 		oneQuery[0] = '\0';
		 		printf(">> ");
	 		}
	 	}
	} else if (argc > 1) {
		FILE* file;
		for (int i=1; i<argc; i++) {
			if ((file = fopen(argv[i], "r")) == NULL) {
				printErr("\nNo such file: %s\n", argv[i]);
				continue;
			}

			printf(LIGHT_GREEN "\nUsing file: %s\n" NONE, argv[i]);
			while( fscanf(file, " %[^;];", buffer) > 0 ) {
				printf(">> %s\n", buffer);
				base.Query(string(buffer));
			}
			// base.show();
		}
	} else {
		fprintf(stderr, "Invalid use. Please use the following format instead.\n ./<executable> [<filename1> <filename2> ...]");
	}
	return 0;
}