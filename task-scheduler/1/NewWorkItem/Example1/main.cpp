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
		hr = CoCreateInstance(CLSID_CTaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskScheduler,
			(void**)&pITS);
		if (FAILED(hr))
		{
			CoUninitialize();
			return EXIT_FAILURE;
		}
	}
	else
	{
		return EXIT_FAILURE;
	}

	LPCWSTR pwszTaskName;
	ITask *pITask;
	IPersistFile *pIPersistFile;
	pwszTaskName = L"Test Task";
	
	hr = pITS->NewWorkItem(
		pwszTaskName,         // наименование задания.
		CLSID_CTask,          // идентификатор класса.
		IID_ITask,            // идентификатор интерфейса.
		(IUnknown**)&pITask   // адрес задачи.
	);
	
	// Освобождение интерфейса ITaskScheduler:
	pITS->Release();
	
	if (FAILED(hr))
	{
		CoUninitialize();
		fprintf(stderr, "Failed calling NewWorkItem, error = 0x%x\n",hr);
		
		// if you get error 0x80070050 it means that a task with that name already exists, 
		// delete the .job file or change the name.
		
		return EXIT_FAILURE;
	}
	
	// Получить указатель на IPersistFile:
	hr = pITask->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
	
	pITask->Release();
	if (FAILED(hr))
	{
		CoUninitialize();
		fprintf(stderr, "Failed calling QueryInterface, error = 0x%x\n", hr);
		return EXIT_FAILURE;
	}
  
  	// Вызов IPersistFile::Save, чтобы сохранить новую задачу на диск:
	hr = pIPersistFile->Save(NULL, TRUE);

	pIPersistFile->Release();
	if (FAILED(hr))
	{
		CoUninitialize();
		fprintf(stderr, "Failed calling Save, error = 0x%x\n",hr);
		return EXIT_FAILURE;
	}
	
	CoUninitialize();
	printf("Created task.\n");
	
	system("pause");
	return EXIT_SUCCESS;
}

