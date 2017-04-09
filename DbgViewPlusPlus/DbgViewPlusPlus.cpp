// DbgViewPlusPlus.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#pragma 

typedef struct {
	DWORD PID;
	char msg;
}DbgEntry, *pDbgEntry;

void printUsage() {
	printf("DbgViewPlusPlus.exe [-pid 000] [-name partialname.exe]\n\n");
	printf("Provide either the PID or a partial name of the process you want to filter for.\n\n");
	printf("Provide no paramters for no filtering.\n");
}

int main(unsigned int argc, char *argv[])
{
	DWORD tgtPid = -1;
	bool filtering = false;
	std::string givenName;
	if (argc > 3) {
		printf("Too many parameters provided!\n");
		printUsage();
		return -1;
	}
	if (argc > 1) {
		unsigned int idx = 1;
		filtering = true;
		while (idx < argc) {
			std::string tok = std::string(argv[idx]);
			if (tok == "-pid") {
				tgtPid = std::stoul(argv[idx + 1]);
				idx++;
			}
			else if (tok == "-name") {
				givenName = std::string(argv[idx + 1]);
				std::transform(givenName.begin(), givenName.end(), givenName.begin(), ::tolower);
				idx++;
			}
			else {
				printf("Unknown parameter \"%s\"\n.", tok);
				printUsage();
				return -1;
			}
			idx++;
		}
	}
	// create the necessart named objects to make sure we can continue
	HANDLE hBufReady = CreateEvent(NULL, false, false, "DBWIN_BUFFER_READY");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		printf("Another Debug Reader already present.");
		return -1;
	}

	HANDLE hDataReady = CreateEvent(NULL, false, false, "DBWIN_DATA_READY");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		printf("Another Debug Reader already present.");
		return -1;
	}

	if (hBufReady == nullptr || hDataReady == nullptr) {
		printf("Error during setup.");
		return -1;
	}

	HANDLE hDbgFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, "DBWIN_BUFFER");
	pDbgEntry dbgBuf = static_cast<pDbgEntry>(MapViewOfFile(hDbgFile, FILE_MAP_ALL_ACCESS, 0, 0, 4096));

	HANDLE waitHandles[2];
	waitHandles[0] = hDataReady;
	DWORD waitCount = 1;

	// Set the ready event to tell other processes that we're listening
	SetEvent(hBufReady);
	bool keepListening = true;
	while (keepListening) {
		DWORD reason = WaitForMultipleObjects(waitCount, waitHandles, false, INFINITE);

		switch (reason) {
		case WAIT_OBJECT_0:
			if (filtering && tgtPid == -1) {
				// we need to compare the given name with the name of the process
				HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, false, dbgBuf->PID);
				if (hProc != NULL) {
					DWORD bufSize = 256;
					char imageName[256];
					memset(imageName, 0, bufSize);
					DWORD ret = QueryFullProcessImageName(hProc, 0, imageName, &bufSize);
					if (ret > 0) {
						std::string procName = std::string(imageName);
						std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
						if (procName.find(givenName) != -1) {
							tgtPid = dbgBuf->PID;
							waitHandles[1] = hProc; // stop listening when the target process dies
							waitCount++;
						}
					}
				}
			}

			if (filtering && tgtPid == dbgBuf->PID)
				printf("[%d] %s\n", dbgBuf->PID, &(dbgBuf->msg));
			memset(static_cast<void *>(dbgBuf), 0, 4096);
			SetEvent(hBufReady);
			break;
		case WAIT_OBJECT_0 + 1:
			keepListening = false;
			CloseHandle(waitHandles[1]);
			break;
		}
		
	}
	
	UnmapViewOfFile(dbgBuf);
	CloseHandle(hDbgFile);
	CloseHandle(hDataReady);
	CloseHandle(hBufReady);

    return 0;
}

