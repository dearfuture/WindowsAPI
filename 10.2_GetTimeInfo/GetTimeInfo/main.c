#include<windows.h>
#include<stdio.h>

int main(void)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	printf("Now: %d-%d-%d, %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	st.wHour--;

	SetLocalTime(&st);
}