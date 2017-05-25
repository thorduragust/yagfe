#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "paths.h"

#define arr_length(a) (sizeof(a)/sizeof(a)[0])

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int int64;
typedef unsigned long int uint64;

extern int8 **environ;

#define allocateMemory(size, type) (type *)malloc(size * sizeof(type))

//stilla IFS og PATH í eitthvað nothæft, TZ geymd
//NOTE: IFS og TZ eru ekki skilgreindar á mac
void sanitizeEnvironmentVariables() {
	int8 *sanitizedVars[] = {
		"IFS= \t\n",
		"PATH=" _PATH_STDPATH,
	};

	int8 **newEnvironment;
	size_t newLength = 0;

	//NOTE: +1 fyrir TZ og +1 fyrir 0
	newLength = arr_length(sanitizedVars) + 2;
	newEnvironment = allocateMemory(newLength, int8 *);

	for(uint32 i = 0; i < arr_length(sanitizedVars); i++) {
		newEnvironment[i] = allocateMemory(strlen(sanitizedVars[i]) + 1, int8); //+1 fyrir null term streng(held það þurfi að vera svoleiðis)
		size_t stringIndex = 0;

		for(; sanitizedVars[i][stringIndex] != 0; stringIndex++) {
			newEnvironment[i][stringIndex] = sanitizedVars[i][stringIndex];
		}

		newEnvironment[i][stringIndex] = 0;
	}

	size_t timeZoneVarLength = 3 + strlen((getenv("TZ") == NULL) ? "" : getenv("TZ"));//3 = strlen("TZ=")
	int8 timeZoneVar[timeZoneVarLength+1];
	if(timeZoneVarLength > 3) {

	}
	timeZoneVar[timeZoneVarLength] = 0;
	printf("%d\n", arr_length(timeZoneVar));

	sprintf(timeZoneVar, "TZ=%s", (timeZoneVarLength > 3) ? getenv("TZ") : "");
	newEnvironment[newLength-2] = allocateMemory(timeZoneVarLength + 1, int8);
	newEnvironment[newLength-2][timeZoneVarLength] = 0;

	for(uint32 i = 0; timeZoneVar[i] != 0; i++) {
		newEnvironment[newLength-2][i] = timeZoneVar[i];
	}

	newEnvironment[newLength-1] = 0;

	environ = newEnvironment;
}

int main() {
	pid_t childProcess = fork();
	
	if(childProcess == 0) {
		printf("this is the child\n");
		sanitizeEnvironmentVariables();
	}else {
		printf("this is the parent\n");
	}

	for(int i = 0; environ[i] != 0; i++) {
		printf("%s\n", environ[i]);
	}

	return 0;
}