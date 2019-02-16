#include<windows.h>

#define VARIABLES_APPEND	1
#define VARIABLES_RESET		2
#define VARIABLES_NULL		0

#define BUFSIZE 4096

DWORD WINAPI EnumEnvironmentVariables();
DWORD WINAPI ChangeEnviromentVariables(LPTSTR szName, LPTSTR szNewValue, DWORD dwFlag);