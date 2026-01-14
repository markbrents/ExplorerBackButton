### The Idea
You know how in a web browser, you can click the right mouse button and there is an option in the menu to go back to the previous page? I thought it would be cool if you could do that in Windows File Explorer too. (Not very useful, but fun.)

### The Plan
To accomplish this, we need two main things: 
1.	We need to add a menu item to the right-click menu in Explorer. Clicking this should run a program. 
2.	We need to create this program and have it talk to Explorer and get it to go back to its previous location. 

### The Program
Let’s start with the program. 

We need a way to find the instance of File Explorer that our program launched from. Since there could be several of them, we have to find the right one. My initial attempt was to enumerate all the Explorer windows and find the one that is the foreground window. 

Turns out this was way over thinking it. When our program runs, it will be a console program and won’t have a window. This means the File Explorer window that launched us will be the foreground window. And that means it will have focus. We just need to send keys to the foreground window.

### Keys
So, what keys do we send to it? Turns out that if you press Alt + Left Arrow, Explorer will go back to the previous folder. We simply need to figure out how to send this combination. 

My initial attempts involved trying to use the Windows API SendMessage to send keypress messages. I then found out I could just use the keybd_event to accomplish the task.

I was able to get it to work using keybd_event. Hoever, it turns out that the keybd_event call is depreciated. (Lesson: double-check everything AI tells you.) I should have used SendInput. So, one rewrite later and we have: 
```
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
```
