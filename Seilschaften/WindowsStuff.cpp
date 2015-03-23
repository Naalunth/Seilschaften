#include "WindowsStuff.h"
#include "Seilschaften.h"

void AddWindowsConsole()
{
	AllocConsole();
	_setmode(_fileno(stdout), _O_U16TEXT);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AddWindowsConsole();
	main();
}
