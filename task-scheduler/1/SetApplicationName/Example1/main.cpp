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
	
	// Инициализация COM-библиотеки:
	hr = CoInitialize(NULL);
	
	if (SUCCEEDED(hr))
	{
		// Получения Task Scheduler объекта:
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
	
	// Наименование СУЩЕСТВУЮЩЕГО задания:
	lpcwszTaskName = L"Test Task";
	
	// Получение Task-объекта:
	hr = pITS->Activate(lpcwszTaskName,
		IID_ITask,
		(IUnknown**)&pITask);
	
	// Освобождение интерфейса ITaskScheduler:
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
	
	// Указание имени приложения для задания указанного в переменной lpcwszTaskName:
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
	
	// Освободить интерфейс ITask:
	pITask->Release();  
	
	// Вызов IPersistFile::Save, что сохранить модифицированную задачу:
	hr = pIPersistFile->Save(NULL, TRUE);
	
	if (FAILED(hr))
	{
		wprintf(L"Failed calling IPersistFile::Save: ");
		wprintf(L"error = 0x%x\n",hr);
		pIPersistFile->Release();
		CoUninitialize();
		return EXIT_FAILURE;
	}
	
	// Осводить интерфейс IPersistFile:
	pIPersistFile->Release();

	wprintf(L"Set the application name for Test Task.\n");  
	CoUninitialize();

	system("pause");
	return EXIT_SUCCESS;
}

