#include <windows.h>
#include <Windowsx.h>
#include <cstdio>
#include <profileapi.h>




#define X_RUN_STEP 100  // ��� "����" ���� �� ��������.
#define Y_RUN_STEP 100  // ��� "����" ���� �� ��������.

BOOL isRunWin = TRUE;  // ���� "����" ����, ���� TRUE - ���� �������.

// �������� ������� ��������� ���� �� �������:
void RunWindow(HWND hWindow, LPARAM lMouseCoor);

// �������� ������� ��� ������ ���������� ��������� � �����:
int print_log(const char* format, ...);
#define printf(format, ...) \
	print_log(format, __VA_ARGS__)

// ��� ��� ��������� �������� ������ ������ ����:
HHOOK hMouseHook;
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// ������ ��� ������ ������� ����� ����� �������� ������ ����, ����� ���������� ������� ������:
long long mouseDownTime[2] = { 0, INT_MAX };
short mouseDownTimeIdx = 0;




// �������� ��� ������� ������� ���������
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// ��� ������ ���� �������� ���������� ����������
char g_szClassName[] = "WindowApplication";

int WINAPI WinMain(HINSTANCE ThisInstance
	, HINSTANCE PreviousInstance
	, LPSTR lpszArgument
	, int CommandShow
	)
{
	HWND FirstWindow; // ���������� ����
	MSG Messages; // ����� ����������� ��� ��������� � ����������
	WNDCLASS WindowClass; // ��������� ������ ��� ����������������� ������ ����
	WindowClass.hInstance = ThisInstance;
	WindowClass.lpszClassName = g_szClassName;
	// ��������� �� ������� ������� ���������
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.lpszMenuName = NULL; // ��� ���������� ����
	// ������������� ������ �� ���������
	WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // ������ ���� - �������
	// ����������� ����� ���� ��� ��������� ������ � ������ ����,
	// � ����� �������� �������� �����
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	// ��� �������������� ������ ��� ������ ����
	WindowClass.cbClsExtra = 0;
	// ��� �������������� ������ ��� ���������� ����
	WindowClass.cbWndExtra = 0;
	// ��� ���� ������������ ���� �� ���������
	WindowClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	// ���������������� ����� ����, � � ������ ������ ��������� ���������
	if (0 == RegisterClass(&WindowClass))
		return EXIT_FAILURE;
	// �������� ����
	FirstWindow = CreateWindow(g_szClassName // ��� ������
		, "���������� � �������" // ��������� ����
		, WS_OVERLAPPEDWINDOW // ������������� ����
		, CW_USEDEFAULT // �� ������ �� ���,
		, CW_USEDEFAULT // ���� ���������� ����
		, 600 // ������ ����
		, 375 // ������ ����
		, HWND_DESKTOP // ��� ���� - ������� �������� �����
		, NULL // ���� �� ���������
		, ThisInstance // ���������� ���������� ���������
		, NULL // ��� ������ ��� �������� ����
		);
	// ������� (��������) ���� �� �����
	ShowWindow(FirstWindow, CommandShow);
	UpdateWindow(FirstWindow);

	// ������������� ���:
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, NULL);

	// ���� ��������� ���������
	while (GetMessage(&Messages, NULL, 0, 0))
	{
		// ���������� ��������� ����������� ������ � ���������� ���������
		TranslateMessage(&Messages);
		// �������� ��������� � ������� ���������
		DispatchMessage(&Messages);
	}

	// ������� ���:
	UnhookWindowsHookEx(hMouseHook);

	// ���������� �������� 0 �� ������� WinMain, ������� ��� �������
	// ����������� �� PostQuitMessage()
	return Messages.wParam;
}

// ��� ������� (������� ������� ���������) ���������� API-�������� DispatchMessage()
//
// FUNCTION: LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM)
//
// PARAMETERS: [in] hWindow - ����� ����
// [in] uMessage - ��������� ��� ����
// [in] wFirstParameter � �������������� ���������� ��� ���������
// [in] lSecondParameter � �������������� ���������� ��� ���������
//
// RETURN VALUE: 0 - �� ���� �������
//
// COMMENTS: �������� ���������� wFirstParameter � lSecondParameter ������� ��
// ��������� uMessage
//
LRESULT CALLBACK WindowProcedure(HWND hWindow
	, UINT uMessage
	, WPARAM wFirstParameter
	, LPARAM lSecondParameter
	)
{
	switch (uMessage) // ��������� ���������
	{
	case WM_CLOSE: // �������� ����
		DestroyWindow(hWindow);
		return 0;
	case WM_DESTROY: // �������� ����
		PostQuitMessage(0); // �������� ��������� WM_QUIT � ������� ���������
		break;
	case WM_NCMOUSEMOVE:  // ���������� ����, ����� ������ ������������ ��� ������������ �������� ����.
	{
		// �������� lSecondParameter ���������� ���������� �������:
		short xPos = GET_X_LPARAM(lSecondParameter);
		short yPos = GET_Y_LPARAM(lSecondParameter);
		printf("��������� WM_NCMOUSEMOVE, ���������� �������: (%d; %d)\n", xPos, yPos);

		if (isRunWin) {  // ���� ���� "����" ���� ������, ��:
			RunWindow(hWindow, lSecondParameter);  // ���� "�����".
		}

		break;
	}
	case WM_NCLBUTTONDBLCLK:
	{
		printf("��������� WM_NCLBUTTONDBLCLK\n");
		break;
	}
	case WM_LBUTTONDBLCLK:
	{
		printf("��������� WM_LBUTTONDBLCLK\n");
		break;
	}
	case WM_CAPTURECHANGED:
	{
		printf("��������� WM_CAPTURECHANGED\n");
		break;
	}
	default: // ��� ���������, � �������� �� �� ����� �����������
		return DefWindowProc(hWindow
			, uMessage
			, wFirstParameter
			, lSecondParameter
			);
	}
	return 0;
}

// ������� ��������� ���� �� �������:
void RunWindow(HWND hWindow, LPARAM lMouseCoor)
{
	// ���������� ������� ���� ��� ��� ��������� �� ������������ ������� ����:
	short mousePosX = GET_X_LPARAM(lMouseCoor);
	short mousePosY = GET_Y_LPARAM(lMouseCoor);

	// ���������� � ������� ����:
	RECT win;
	GetWindowRect(hWindow, &win);
	int winWidth = win.right - win.left;  // ������.
	int winHeight = win.bottom - win.top;  // ������.
	int winY = win.top;
	int winX = win.left;

	// ������� ������:
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);  // ������.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // ������.

	// ���� ������ ���� ����� � ������������ ������� ���� �����:
	if (abs(mousePosX - win.left) < abs(win.right - mousePosX))
	{
		// ���� ���� ����� ������:
		if (win.left + winWidth + X_RUN_STEP < screenWidth)
			winX += X_RUN_STEP;  // ���� "�����" ������.
		else  // ���� ����� �����:
			winX -= winWidth;  // ���� "�����" �����.
	}
	else  // ������ ���� ����� � ������������ ������� ���� ������:
	{
		// ���� ���� ����� �����:
		if (win.left - X_RUN_STEP > 0)
			winX -= X_RUN_STEP;  // ���� "�����" �����.
		else  // ���� ����� ������:
			winX += winWidth;  // ���� "�����" ������.
	}

	// ���� ������ ���� ����� � ������������ ������� ���� ������:
	if (abs(mousePosY - win.top) < abs(win.bottom - mousePosY))
	{
		// ���� ���� ����� �����:
		if (win.top + winHeight + Y_RUN_STEP < screenHeight)
			winY += Y_RUN_STEP;  // ���� "�����" ����.
		else  // ���� ����� ������:
			winY -= winHeight;  // ���� "�����" �����.
	}
	else  // ������ ���� ����� � ������������ ������� ���� �����:
	{
		// ���� ���� ����� ������:
		if (win.top - Y_RUN_STEP > 0)
			winY -= Y_RUN_STEP;  // ���� "�����" �����.
		else  // ���� ����� �����:
			winY += winHeight;  // ���� "�����" ����.
	}
	
	// ����������� ���� � ����� ����������:
	MoveWindow(hWindow, winX, winY, winWidth, winHeight, TRUE);
}

// ������� ��������������� ������� �� ������ ����:
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if (pMouseStruct != NULL){
		switch (wParam)
		{
		case WM_LBUTTONDOWN:  // ��������� ������� �� ����� ������ ����:
		{
			printf("WM_LBUTTONDOWN\n");

			// ������� �������, ����� ��������� ������ ����:
			LARGE_INTEGER li;
			::QueryPerformanceCounter(&li);
			mouseDownTime[mouseDownTimeIdx] = li.QuadPart;
			mouseDownTimeIdx = mouseDownTimeIdx == 0 ? 1 : 0;

			::QueryPerformanceFrequency(&li);
			long long frequency = li.QuadPart;

			double elapsedMilliseconds = (abs(mouseDownTime[1] - mouseDownTime[0]) * 1000.0) / frequency;
			printf("����� ����� ����� ���������� ��������: %f\n", elapsedMilliseconds);

			UINT doubleClickTime = GetDoubleClickTime();
			printf("������� ����� �������� ������ ���� � �������������: %d\n", doubleClickTime);

			// ���� ����� ��������� ����� ����� ���������� �������� ���� ������ ��� 
			// ������������� � ������� ����� ��� �������� ������ ����, �� ����. ��� ���.
			// "���" ���� ��� ��������� ������� ���� �� ��� ������������ �������:
			if (elapsedMilliseconds < doubleClickTime)
			{
				isRunWin = !isRunWin;
			}

			break;
		}
		default:
			break;
		}
		// printf("Mouse position X = %d  Mouse Position Y = %d\n", pMouseStruct->pt.x, pMouseStruct->pt.y);
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// ������� ��� ������ ���������� ��������� � �����:
int print_log(const char* format, ...)
{
	static char s_printf_buf[1024];  // �����.
	va_list args;  // ���������.
	va_start(args, format);
	_vsnprintf(s_printf_buf, sizeof(s_printf_buf), format, args);
	va_end(args);
	OutputDebugStringA(s_printf_buf);  // �����.
	return 0;
}
