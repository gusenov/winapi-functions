#include <iostream>
#include <windows.h>
#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>
#include <wchar.h>

int main(int argc, char** argv)
{
	HRESULT hr = S_OK;
	
	ITaskScheduler* pITS;
	
	// ������������� COM-����������:
	hr = CoInitialize(NULL);
	
	if (SUCCEEDED(hr))
	{
		// ��������� Task Scheduler �������:
		hr = CoCreateInstance(
			CLSID_CTaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskScheduler,
			(void**)&pITS
		);
		
		if (FAILED(hr))
		{
			CoUninitialize();
			return 1;
		}
	}
	else
	{
		return EXIT_FAILURE;
	}

	ITask *pITask;
	LPCWSTR lpcwszTaskName;
	
	// ������������ ������������� �������:
	lpcwszTaskName = L"Test Task";
	
	// ��������� Task-�������:
	hr = pITS->Activate(lpcwszTaskName,
		IID_ITask,
		(IUnknown**)&pITask);
	
	// ������������ ���������� ITaskScheduler:
	pITS->Release();  
	
	if (FAILED(hr))
	{
		wprintf(L"Failed calling ITaskScheduler::Activate: ");
		wprintf(L"error = 0x%x\n",hr);
		
		// For '0x80070002',it means "File not found".
		
		CoUninitialize();
		return EXIT_FAILURE;
	}
	
	LPCWSTR pwszApplicationName = L"C:\\Windows\\System32\\notepad.exe";
	
	// �������� ����� ���������� ��� ������� ���������� � ���������� lpcwszTaskName:
	hr = pITask->SetApplicationName(pwszApplicationName);
	
	if (FAILED(hr))
	{
		wprintf(L"Failed calling ITask::SetApplicationName: ");
		wprintf(L"error = 0x%x\n",hr);
		pITask->Release();
		CoUninitialize();
		return EXIT_FAILURE;
	}

	IPersistFile *pIPersistFile;
	
	hr = pITask->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
	
	// ���������� ��������� ITask:
	pITask->Release();  
	
	// ����� IPersistFile::Save, ��� ��������� ���������������� ������:
	hr = pIPersistFile->Save(NULL, TRUE);
	
	if (FAILED(hr))
	{
		wprintf(L"Failed calling IPersistFile::Save: ");
		wprintf(L"error = 0x%x\n",hr);
		pIPersistFile->Release();
		CoUninitialize();
		return EXIT_FAILURE;
	}
	
	// �������� ��������� IPersistFile:
	pIPersistFile->Release();

	wprintf(L"Set the application name for Test Task.\n");  
	CoUninitialize();

	system("pause");
	return EXIT_SUCCESS;
}

