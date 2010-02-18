#include "logger.h"
#include <iostream>

#if defined(WIN32)
#include <windows.h>

DWORD WINAPI Fun1Proc (LPVOID lpParameter) {
	LOG(INFO) << "hello there";
	return 0;
}
#endif

void* mythread(void* ptr){
	InitLogging("thread.log", FILE_AND_CONSOLE, AppendToTheOldFile);
	LOG(INFO) <<"Thread started";
	return 0;
}

int main() {
//	InitializeLogFile();
	InitLogging("Hello.log", FILE_AND_CONSOLE, AppendToTheOldFile);
//	COMPACT_LOG_INFO;
//	COMPACT_LOG_WARNING;
	LOG(INFO) << "you sucker" << " ass hole";
	LOG(WARNING) << "just ss";
	LOG(ERR) << "error ss";
	LOG(FATAL) << "piecefull";
	const int i = 9;
	LOG_IF(INFO, i > 8) << "bastard";
	LOG_IF(WARNING, i > 10) << "houzi";

	for(int i=0;i<5;i++){
		pthread_t MyThread;
		pthread_create(&MyThread, 0, &mythread, 0);
 //Commenting this in doesn't solve the problem
	}
#if defined(WIN32)
	HANDLE hThread1;
	hThread1 = CreateThread(NULL, 0, Fun1Proc, NULL, 0, NULL);
	CloseHandle(hThread1);
#endif
	sleep(5);
	return 0;
}