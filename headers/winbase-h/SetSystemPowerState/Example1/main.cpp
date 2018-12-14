#include <iostream>
#include <windows.h>

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

int main(int argc, char** argv) {
    if(!_EnableShutDownPriv())  
        return FALSE; 
	
	SetSystemPowerState(FALSE, FALSE);
	
	system("pause");
	return 0;
}
