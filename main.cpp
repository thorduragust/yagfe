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
	/*int8 pathVar[strlen("PATH=") + strlen(getenv("PATH")) + 1];
	strcpy(pathVar, "PATH=");
	strcat(pathVar, getenv("PATH"));*/
	
	int8 *sanitizedVars[] = {
		"IFS= \t\n",
		"PATH=" _PATH_STDPATH //það er mögulega eitthvað sniðugra á mac þar sem path er aðeins öðruvísi
		//pathVar
	};

	int8 **newEnvironment;
	size_t newLength = 0;

	bool timeZoneDefined = (getenv("TZ") != NULL);

	newLength = arr_length(sanitizedVars) + 1;
	if(timeZoneDefined) newLength++;

	newEnvironment = allocateMemory(newLength, int8 *);
	newEnvironment[newLength-1] = NULL;

	for(uint32 i = 0; i < arr_length(sanitizedVars); i++) {
		newEnvironment[i] = allocateMemory(strlen(sanitizedVars[i]) + 1, int8); //+1 fyrir null term streng(held það þurfi að vera svoleiðis)
		size_t stringIndex = 0;

		for(; sanitizedVars[i][stringIndex] != 0; stringIndex++) {
			newEnvironment[i][stringIndex] = sanitizedVars[i][stringIndex];
		}

		newEnvironment[i][stringIndex] = NULL;
	}

	if(timeZoneDefined) {
		size_t timeZoneVarLength = 3 + strlen(getenv("TZ"));//3 = strlen("TZ=")
		int8 timeZoneVar[timeZoneVarLength+1];

		timeZoneVar[timeZoneVarLength] = 0;

		sprintf(timeZoneVar, "TZ=%s", getenv("TZ"));
		newEnvironment[newLength-2] = allocateMemory(timeZoneVarLength + 1, int8);
		newEnvironment[newLength-2][timeZoneVarLength] = 0;

		memcpy(timeZoneVar, newEnvironment[newLength-2], timeZoneVarLength);
	}

	environ = newEnvironment;
}

int main() {
	pid_t childProcess = fork();
	
	printf("%d\n", childProcess);

	if(childProcess == 0) {
		printf("this is the child\n");
		sanitizeEnvironmentVariables();
		//execlp("lldb", "", "testprog", NULL);
		execl("testprog", "pizza", NULL); //testprog mun prenta út pizza
	}else {
		printf("this is the parent\n");
		int status;
		waitpid(childProcess, &status, 0);
		printf("status: %d\n", WIFEXITED(status));
	}

	return 0;
}