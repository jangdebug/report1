#include <stdio.h>
#include <string.h>
#include "longest.h"

char line[MAXLINE];
char longest[MAXLINE];

int main()
{
	int len;
	int max;
	max = 0;
	while (mygets(line, sizeof(line)) != NULL) {
		len = strlen(line);
		if (len > max){
			max = len;
			copy(line, longest);
			}
	}
	if (max > 0)
		printf("%s", longest);
	return 0;
}

