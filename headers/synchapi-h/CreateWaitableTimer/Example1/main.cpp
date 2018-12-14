#include <iostream>
#include <windows.h>

DWORD 
CALLBACK  
ShutDownProc(LPVOID p)  
{  
    PHANDLE timer = (PHANDLE) p;  
  
    WaitForSingleObject(*timer, INFINITE);  
    CloseHandle(*timer);  
    SetThreadExecutionState(ES_DISPLAY_REQUIRED);   
  
    return 0;  
}

static BOOL WINAPI  
_EnableShutDownPriv()  
{  
    BOOL ret = FALSE;  
    HANDLE process = GetCurrentProcess();  
    HANDLE token;  
  
    if(!OpenProcessToken(process  
                        , TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY  
                        , &token))  
        return FALSE;  
  
    LUID luid;  
    LookupPrivilegeValue(NULL, "SeShutdownPrivilege", &luid);  
  
    TOKEN_PRIVILEGES priv;  
    priv.PrivilegeCount = 1;  
    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
    priv.Privileges[0].Luid = luid;  
    ret = AdjustTokenPrivileges(token, FALSE, &priv, 0, NULL, NULL);  
    CloseHandle(token);  
  
    return TRUE;  
}  

BOOL 
WINAPI  
HibernateAndReboot(int secs)  
{  
    if(!_EnableShutDownPriv())  
        return FALSE;  
  
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, "MyWaitableTimer");  
    if(timer == NULL)  
        return FALSE;  
  
    __int64 nanoSecs;  
    LARGE_INTEGER due;  
  
    nanoSecs = -secs * 1000 * 1000 * 10;  
    due.LowPart = (DWORD) (nanoSecs & 0xFFFFFFFF);  
    due.HighPart = (LONG) (nanoSecs >> 32);  
  
    if(!SetWaitableTimer(timer, &due, 0, 0, 0, TRUE))  
        return FALSE;  
  
    if(GetLastError() == ERROR_NOT_SUPPORTED)  
        return FALSE;  
  
    HANDLE thread = CreateThread(NULL, 0, ShutDownProc, &timer, 0, NULL);  
    if(!thread)  
    {  
        CloseHandle(timer);  
        return FALSE;  
    }  
  
    CloseHandle(thread);  
    SetSystemPowerState(FALSE, FALSE);  
    return TRUE;  
} 

int main(int argc, char** argv) {
	int secs;
	std::cout << "secs>";
	std::cin >> secs;
	
	system("pause");
	
	HibernateAndReboot(secs);
	
	return 0;
}
