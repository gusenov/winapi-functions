#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <TlHelp32.h>
#include <psapi.h>

int main(int argc, char** argv) {

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
         DWORD errorCode = GetLastError();
         return EXIT_FAILURE;
    }
    
    PROCESSENTRY32 ProcEntry;
    ProcEntry.dwSize = sizeof(PROCESSENTRY32);
    
    BOOL bResult = Process32First(hSnapShot, &ProcEntry);
    if (bResult == FALSE)
    {
         DWORD errorCode = GetLastError();
         CloseHandle(hSnapShot);
         return EXIT_FAILURE;
    }
    
    while (bResult == TRUE)
    {  
	    HANDLE hProc = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			ProcEntry.th32ProcessID
		);
		
		if (hProc != NULL)
	    {	
			// Имя образа.
			printf("ProcEntry.szExeFile = %s\n", ProcEntry.szExeFile);
			
			// Cчетчик потоков.
			printf("ProcEntry.cntThreads = %lu\n", ProcEntry.cntThreads);
			
			// ИД процесса.
			printf("ProcEntry.th32ProcessID = %lu\n", ProcEntry.th32ProcessID);
			
			// The identifier of the process that created this process (its parent process).
			printf("ProcEntry.th32ParentProcessID = %lu\n", ProcEntry.th32ParentProcessID);
			
			// The base priority of any threads created by this process.
			printf("ProcEntry.pcPriClassBase = %ld\n", ProcEntry.pcPriClassBase);

			// This member is no longer used and is always set to zero.
			
			//printf("ProcEntry.dwSize = %lu\n", ProcEntry.dwSize);
			//printf("ProcEntry.dwFlags = %lu\n", ProcEntry.dwFlags);
			//printf("ProcEntry.th32ModuleID = %lu\n", ProcEntry.th32ModuleID);
			//printf("ProcEntry.cntUsage = %lu\n", ProcEntry.cntUsage);
			//printf("ProcEntry.th32DefaultHeapID = %Ix\n", ProcEntry.th32DefaultHeapID);
			
			PROCESS_MEMORY_COUNTERS mc;
			mc.cb = sizeof(mc);
		
			SIZE_T sz = 0;	
			if (GetProcessMemoryInfo(hProc, &mc, sizeof(mc)) != 0)
			{
				sz = mc.WorkingSetSize / 1024;
				
				// Память.
				printf("sz = %lu\n", sz);
			}
			
			DWORD pri = GetPriorityClass(hProc);
	
			printf("pri = ");
			switch (pri) {
				case REALTIME_PRIORITY_CLASS: // Реального времени.
					printf("REALTIME_PRIORITY_CLASS");
					break;
				case HIGH_PRIORITY_CLASS: // Высокий.
					printf("HIGH_PRIORITY_CLASS");
					break;
				case ABOVE_NORMAL_PRIORITY_CLASS: // Выше среднего.
					printf("ABOVE_NORMAL_PRIORITY_CLASS");
					break;
				case NORMAL_PRIORITY_CLASS: // Средний.
					printf("NORMAL_PRIORITY_CLASS");
					break;
				case BELOW_NORMAL_PRIORITY_CLASS: // Ниже среднего.
					printf("BELOW_NORMAL_PRIORITY_CLASS");
					break;
				case IDLE_PRIORITY_CLASS: // Низкий.
					printf("IDLE_PRIORITY_CLASS");
					break;
			}
			printf("\n");
			
			printf("\n");
				
			CloseHandle(hProc);
		}
	
		bResult = Process32Next(hSnapShot, &ProcEntry);	            
    }
    
    CloseHandle(hSnapShot);

	system("pause");
	
	return EXIT_SUCCESS;

}

