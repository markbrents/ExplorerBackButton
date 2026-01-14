#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <windows.h>
#include <WinUser.h>

int main()
{

	INPUT inputs[4] = {0};
	ZeroMemory(inputs, sizeof(inputs));

    HWND fore = GetForegroundWindow();

	SetForegroundWindow(fore);
	

	// Press ALT 
	
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MENU; // ALT key

    // Press Left Arrow
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_LEFT; // LEFT ARROW key

    //Release Left Arrow
	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_LEFT; // LEFT ARROW key
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // Release ALT
	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_MENU; // ALT key
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    // Send the keys
	SendInput(4, inputs, sizeof(INPUT));

	return 0; 
}