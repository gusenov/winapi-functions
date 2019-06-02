#include <windows.h>
#include <Windowsx.h>
#include <cstdio>
#include <profileapi.h>




#define X_RUN_STEP 100  // шаг "бега" окна по абсциссе.
#define Y_RUN_STEP 100  // шаг "бега" окна по ординате.

BOOL isRunWin = TRUE;  // флаг "бега" окна, если TRUE - окно убегает.

// Прототип функции «убегания окна от курсора»:
void RunWindow(HWND hWindow, LPARAM lMouseCoor);

// Прототип функции для печати отладочных сообщений в вывод:
int print_log(const char* format, ...);
#define printf(format, ...) \
	print_log(format, __VA_ARGS__)

// Хук для перехвата двойного щелчка кнопки мыши:
HHOOK hMouseHook;
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// Массив для замера времени между двумя щелчками кнопки мыши, чтобы определить двойной щелчок:
long long mouseDownTime[2] = { 0, INT_MAX };
short mouseDownTimeIdx = 0;




// Прототип для главной оконной процедуры
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Имя класса окна делается глобальной переменной
char g_szClassName[] = "WindowApplication";

int WINAPI WinMain(HINSTANCE ThisInstance
	, HINSTANCE PreviousInstance
	, LPSTR lpszArgument
	, int CommandShow
	)
{
	HWND FirstWindow; // Дескриптор окна
	MSG Messages; // Здесь сохраняются все сообщения в приложении
	WNDCLASS WindowClass; // Структура данных для пользовательского класса окна
	WindowClass.hInstance = ThisInstance;
	WindowClass.lpszClassName = g_szClassName;
	// указатель на главную оконную процедуру
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.lpszMenuName = NULL; // Нет системного меню
	// использование иконки по умолчанию
	WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // курсор мыши - стрелка
	// перерисовка всего окна при изменении высоты и ширины окна,
	// а также перехват двойного клика
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	// нет дополнительных байтов для класса окна
	WindowClass.cbClsExtra = 0;
	// нет дополнительных байтов для экземпляра окна
	WindowClass.cbWndExtra = 0;
	// для фона используется цвет по умолчанию
	WindowClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	// зарегистрировать класс окна, а в случае ошибки завершить программу
	if (0 == RegisterClass(&WindowClass))
		return EXIT_FAILURE;
	// создание окна
	FirstWindow = CreateWindow(g_szClassName // Имя класса
		, "Приложение с окошком" // заголовок окна
		, WS_OVERLAPPEDWINDOW // перекрываемое окно
		, CW_USEDEFAULT // ОС решает за нас,
		, CW_USEDEFAULT // куда разместить окно
		, 600 // ширина окна
		, 375 // высота окна
		, HWND_DESKTOP // это окно - потомок рабочего стола
		, NULL // меню не выводится
		, ThisInstance // дескриптор экземпляра программы
		, NULL // без данных для создания окна
		);
	// Вывести (показать) окно на экран
	ShowWindow(FirstWindow, CommandShow);
	UpdateWindow(FirstWindow);

	// Устанавливаем хук:
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, NULL);

	// Цикл обработки сообщений
	while (GetMessage(&Messages, NULL, 0, 0))
	{
		// трансляция сообщений виртуальных клавиш в символьные сообщения
		TranslateMessage(&Messages);
		// отправка сообщений в оконную процедуру
		DispatchMessage(&Messages);
	}

	// Удаляем хук:
	UnhookWindowsHookEx(hMouseHook);

	// Возвратить значение 0 из функции WinMain, которое она получит
	// транзитивно от PostQuitMessage()
	return Messages.wParam;
}

// Эта функция (главная оконная процедура) вызывается API-функцией DispatchMessage()
//
// FUNCTION: LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM)
//
// PARAMETERS: [in] hWindow - хэндл окна
// [in] uMessage - сообщение для окна
// [in] wFirstParameter – дополнительная информация для сообщения
// [in] lSecondParameter – дополнительная информация для сообщения
//
// RETURN VALUE: 0 - во всех случаях
//
// COMMENTS: Значение параметров wFirstParameter и lSecondParameter зависит от
// параметра uMessage
//
LRESULT CALLBACK WindowProcedure(HWND hWindow
	, UINT uMessage
	, WPARAM wFirstParameter
	, LPARAM lSecondParameter
	)
{
	switch (uMessage) // Обработка сообщений
	{
	case WM_CLOSE: // закрытие окна
		DestroyWindow(hWindow);
		return 0;
	case WM_DESTROY: // удаление окна
		PostQuitMessage(0); // отправка сообщения WM_QUIT в очередь сообщений
		break;
	case WM_NCMOUSEMOVE:  // посылается окну, когда курсор перемещается над неклиентской областью окна.
	{
		// Параметр lSecondParameter обозначает координаты курсора:
		short xPos = GET_X_LPARAM(lSecondParameter);
		short yPos = GET_Y_LPARAM(lSecondParameter);
		printf("Сообщение WM_NCMOUSEMOVE, координаты курсора: (%d; %d)\n", xPos, yPos);

		if (isRunWin) {  // если флаг "бега" окна поднят, то:
			RunWindow(hWindow, lSecondParameter);  // окно "бежит".
		}

		break;
	}
	case WM_NCLBUTTONDBLCLK:
	{
		printf("Сообщение WM_NCLBUTTONDBLCLK\n");
		break;
	}
	case WM_LBUTTONDBLCLK:
	{
		printf("Сообщение WM_LBUTTONDBLCLK\n");
		break;
	}
	case WM_CAPTURECHANGED:
	{
		printf("Сообщение WM_CAPTURECHANGED\n");
		break;
	}
	default: // Для сообщений, с которыми мы не хотим связываться
		return DefWindowProc(hWindow
			, uMessage
			, wFirstParameter
			, lSecondParameter
			);
	}
	return 0;
}

// Функция «убегания окна от курсора»:
void RunWindow(HWND hWindow, LPARAM lMouseCoor)
{
	// Координаты курсора мыши при его попадании на неклиентскую область окна:
	short mousePosX = GET_X_LPARAM(lMouseCoor);
	short mousePosY = GET_Y_LPARAM(lMouseCoor);

	// Координаты и размеры окна:
	RECT win;
	GetWindowRect(hWindow, &win);
	int winWidth = win.right - win.left;  // ширина.
	int winHeight = win.bottom - win.top;  // высота.
	int winY = win.top;
	int winX = win.left;

	// Размеры экрана:
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);  // высота.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // ширина.

	// Если курсор мыши зашел в неклиентскую область окна слева:
	if (abs(mousePosX - win.left) < abs(win.right - mousePosX))
	{
		// Если есть место справа:
		if (win.left + winWidth + X_RUN_STEP < screenWidth)
			winX += X_RUN_STEP;  // окно "бежит" вправо.
		else  // есть место слева:
			winX -= winWidth;  // окно "бежит" влево.
	}
	else  // курсор мыши зашел в неклиентскую область окна справа:
	{
		// Если есть место слева:
		if (win.left - X_RUN_STEP > 0)
			winX -= X_RUN_STEP;  // окно "бежит" влево.
		else  // есть место справа:
			winX += winWidth;  // окно "бежит" вправо.
	}

	// Если курсор мыши зашел в неклиентскую область окна сверху:
	if (abs(mousePosY - win.top) < abs(win.bottom - mousePosY))
	{
		// Если есть место внизу:
		if (win.top + winHeight + Y_RUN_STEP < screenHeight)
			winY += Y_RUN_STEP;  // окно "бежит" вниз.
		else  // есть место вверху:
			winY -= winHeight;  // окно "бежит" вверх.
	}
	else  // курсор мыши зашел в неклиентскую область окна снизу:
	{
		// Если есть место вверху:
		if (win.top - Y_RUN_STEP > 0)
			winY -= Y_RUN_STEP;  // окно "бежит" вверх.
		else  // есть место внизу:
			winY += winHeight;  // окно "бежит" вниз.
	}
	
	// Перемещение окна в новые координаты:
	MoveWindow(hWindow, winX, winY, winWidth, winHeight, TRUE);
}

// Функция перехватывающия нажатие на кнопку мыши:
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if (pMouseStruct != NULL){
		switch (wParam)
		{
		case WM_LBUTTONDOWN:  // произошло нажатие на левую кнопку мыши:
		{
			printf("WM_LBUTTONDOWN\n");

			// Отметка времени, когда произошел щелчок мыши:
			LARGE_INTEGER li;
			::QueryPerformanceCounter(&li);
			mouseDownTime[mouseDownTimeIdx] = li.QuadPart;
			mouseDownTimeIdx = mouseDownTimeIdx == 0 ? 1 : 0;

			::QueryPerformanceFrequency(&li);
			long long frequency = li.QuadPart;

			double elapsedMilliseconds = (abs(mouseDownTime[1] - mouseDownTime[0]) * 1000.0) / frequency;
			printf("Время между двумя последними щелчками: %f\n", elapsedMilliseconds);

			UINT doubleClickTime = GetDoubleClickTime();
			printf("Текущее время двойного щелчка мыши в миллисекундах: %d\n", doubleClickTime);

			// Если время прошедшее между двумя последними щелчками мыши меньше чем 
			// установленное в системе время для двойного щелчка мыши, то выкл. или вкл.
			// "бег" окна при попадании курсора мыши на его неклиентскую область:
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

// Функция для печати отладочных сообщений в вывод:
int print_log(const char* format, ...)
{
	static char s_printf_buf[1024];  // буфер.
	va_list args;  // аргументы.
	va_start(args, format);
	_vsnprintf(s_printf_buf, sizeof(s_printf_buf), format, args);
	va_end(args);
	OutputDebugStringA(s_printf_buf);  // вывод.
	return 0;
}
