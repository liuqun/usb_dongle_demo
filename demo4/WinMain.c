#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <locale.h>

/**/
extern int main(int argc, const char* argv[]);

/**/
int APIENTRY _tWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    
    if (GetStdHandle(STD_OUTPUT_HANDLE) == 0) // no console, we must be the child process
    {
        MessageBox(0, _T("Hello GUI world!"), _T("Testing..."), 0);
    }

	AllocConsole();
    FILE* stdout_new = NULL;
    FILE* stdin_new = NULL;
	_tfreopen_s(&stdout_new, _T("CONOUT$"), _T("w+t"), stdout);
	_tfreopen_s(&stdin_new, _T("CONIN$"), _T("r+t"), stdin);
	if (!stdout_new || !stdin_new) {
		exit(255);
	}
	_tsetlocale(LC_ALL, _T("chs"));

#ifdef _WINDOWS
	fprintf(stdout_new, "_WINDOWS is defined...\n");
#endif

#ifdef _CONSOLE
	fprintf(stdout_new, "_CONSOLE is defined...\n");
#endif

    return main(1, (const char**)&lpCmdLine);
}