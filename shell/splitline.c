/*
	splitline.c - command reading and parsing functions for smsh
			char	*next_cmd(char *prompt, FILE *fp) - get next command
			char	**shplitline(char *str) - parse a string
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include "smsh.h"
//read next command line for fp
char* next_cmd(char* prompt, FILE* fp) {
// return : dynamically allocated string
//			holding command line
	int pid = getpid();
	char* buf;							// the buffer
	int bufspace = 0;					// total size
	int pos = 0;						// current position
	int c;								// input char

	printf("%s", prompt);				// prompt user
	while ((c = getc(fp)) != EOF) {
		//need space?
		if (pos + 1 >= bufspace) {		// 1 for \n
			if (bufspace == 0) {		// y : 1st time
					buf = emalloc(BUFSIZ);	// 8K in stdio.h
			}
			else // or expand buffer using realloc
					buf = erealloc(buf, bufspace + BUFSIZ);
			bufspace += BUFSIZ;	// update size
		}
		//end of command?
		if (c == '\n') break;
		// no , then add to buffer
		buf[pos++] = c;
	}
	if (c == EOF && pos == 0)		// EOF and no input
		return NULL;				// say so
	buf[pos] = '\0';
	if (!strcmp(buf, "exit")) kill(pid, SIGTERM);
	return buf;
}

/*
	splitline (parse a line into an array of strings)
*/
#define is_delim(x) ((x)==' ' || (x)=='\t')
char** splitline(char *line) {
// purpose: split a line into array of white - space separated tokens
// return : a NULL - terminated array of pointers to copies of the token
//			or NULL if line if no tokens on the line
// action : traverse array, locate strings, make copies
// note : strtok() could work, but we may want to add quotes later
	char *newstr();
	char **args;
	int spots = 0;
	int bufspace = 0;
	int argnum = 0;
	char *cp = line; // copy an input line
	char *start;
	int len;

	if (line == NULL)
			return NULL;
	args = emalloc(BUFSIZ);
	bufspace = BUFSIZ;
	spots = BUFSIZ / sizeof(char*);
	while (*cp != '\0') {
		while(is_delim(*cp))	// skip leading spaces
				cp++;
		if (*cp == '\0')		// quit at end-of-string
				break;
		// make sure the array has room (+1 for NULL)
		if (argnum+1 >= spots) {
				args = erealloc(args, bufspace + BUFSIZ);
				bufspace += BUFSIZ;
				spots += (BUFSIZ / sizeof(char*));
		}
		// mark start, then find end of word
		start = cp;
		len = 1;
		while (*++cp != '\0' && !(is_delim(*cp)))
				len++;
		args[argnum++] = newstr(start, len);
	}
	args[argnum] = NULL;
	return args;
}

// constructor for string
// a string, never NULL
char* newstr(char* s, int l) {
	char* rv = emalloc(l + 1);

	rv[l] = '\0';
	strncpy(rv, s, l);
	return rv;
}

// free the list returned by splitline
// free all strings in list and then
// free the list
void freelist(char** list) {
	char** cp = list;
	while (*cp) {
		free(*cp++);
	}
	free(list);
}

// extended malloc
void* emalloc(size_t n) {
	void* rv = (void*)malloc(n);
	if (rv == NULL) {
		fatal("out of memory", "", 1);
	}
	return rv;
}

void* erealloc(void* p, size_t n) {
	void* rv = realloc(p, n);
	if (rv == NULL) {
		fatal("realloc() failed", "", 1);
	}
	return rv;
}