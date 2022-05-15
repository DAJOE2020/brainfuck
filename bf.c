#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long min(long n1, long n2) {
	if (n1 < n2) return n1;
	return n2;
}

long max(long n1, long n2) {
	if (n1 > n2) return n1;
	return n2;
}

unsigned char cmpstr(char* str1, char* str2) {
	int len = strlen(str1);
	if (len != strlen(str2)) return 0;
	for (int i=0;i<len;i++) if (str1[i] != str2[i]) return 0;
	return 1;
}

int main(int argc, char* argv[]) {
	int tapesize = 256;
	char* fileName;
	char* helpStr[4] = {
		"Usage: bf <file_name.bf>",
		"Options:",
		"	--help     shows this text",
		"	--tapesize sets tape length"
	};

	// get filename
	if (argc < 2) {
		printf("Please enter a file\n");
		return(-1);
	}
	fileName = argv[1];

	// loop thru arguments
	char* arg;
	for (int i=1;i<argc;i++) {
		arg = argv[i];

		// check for tapesize argument
		if ((cmpstr(arg,"--tapesize") || cmpstr(arg, "-ts")) && i != argc-1) {
			tapesize = atoi(argv[i+1]);
			i++;
		}

		// check for help argument
		else if (cmpstr(arg,"--help") || cmpstr(arg,"-h")) {
			size_t hlen = sizeof(helpStr)/sizeof(helpStr[0]);
			for (int j=0;j<hlen;j++) printf("%s\n",helpStr[j]);
			return 0;
		}
	}

	// read file to buffer
	FILE* prgm = fopen(fileName,"r");
	if (prgm == NULL) {
		printf("There was an error reading the file\n");
		return(-1);
	}

	fseek(prgm, 0L, SEEK_END);
	long pz = ftell(prgm);
	rewind(prgm);

	char* buff = (char*)malloc(pz*sizeof(char));
	fread(buff, sizeof(char), pz, prgm);

	// check for bracket pairs
	long* pairs = (long*)malloc(pz*sizeof(long));
	long* stackBuff = (long*)malloc(pz*sizeof(long));
	long counter = 0;
	long j;
	char c;

	for (int i=0;i<pz;i++) {
		c = buff[i];

		if (c == '[') {
			stackBuff[counter++] = (long)i;
		}

		if (c == ']') {
			if (counter == 0) {
				printf("unbalanced brackets; too many ]");
				return(-1);
			}
			j = stackBuff[--counter];
			pairs[i] = j;
			pairs[j] = i;
		}
	}

	if (counter != 0) {
		printf("unbalanced brackets; too many [");
		return(-1);
	}
	free(stackBuff);

	// init tape
	char tape[tapesize];
	for (int i=0;i<tapesize;i++) tape[i] = 0;
	int ptr = 0;
	int maxptr = 0;
	char debug = 0;
	char valid = 0;
	char strDump[1024];

	// loop through program
	for (int i=0;i<pz;i++) {
		c = buff[i];
		valid = 1;
		
		switch(c) {
			case '+':
				tape[ptr]++;
				break;
			case '-':
				tape[ptr]--;
				break;
			case '.':
				printf("%c",tape[ptr]);
				break;
			case ',':
				fgets(strDump, 1024, stdin);
				tape[ptr] = strDump[0];
				break;
			case '<':
				ptr--;
				break;
			case '>':
				ptr++;
				break;
			case '[':
				if (tape[ptr] == 0) i = pairs[i];
				break;
			case ']':
				if (tape[ptr] != 0) i = pairs[i];
				break;
			case '*':
				debug = !debug;
				valid = debug;
				break;
			default:
				valid = 0;
		}

		if (ptr > maxptr) maxptr = ptr;

		if (debug && valid) {
			printf("tape: {");
			for (j=0;j<=maxptr;j++) {
				if (j == ptr) printf("[%d]",tape[j]);
				else printf("%d",tape[j]);
				if (j < maxptr) printf(", ");
			}
			printf("}\n");
			printf("command: %c\n",c);
			fgets(strDump, 1024, stdin);
		}
	}

	// free memory
	free(buff);
	free(pairs);
	fclose(prgm);

	return 0;
}
