#include <windows.h>
extern "C" __declspec(dllexport) void CALLBACK RunSim(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) { MessageBoxA(NULL, "DLL Works!", "Test", MB_OK); }
