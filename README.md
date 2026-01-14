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

### The Keys
So, what keys do we send to it? Turns out that if you press Alt + Left Arrow, Explorer will go back to the previous folder. We simply need to figure out how to send this combination. 

My initial attempts involved trying to use the Windows API SendMessage to send keypress messages. I then found out I could just use the keybd_event to accomplish the task.

I was able to get it to work using keybd_event. However, it turns out that the [keybd_event](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_event) call is depreciated.  I should have used [SendInput](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput). (Lesson: double-check everything AI tells you.)

So, one rewrite later and we have: 
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

### The Menu
Now that we’re got a program to run when the menu is clicked, we need to setup the menu. 

1.	Run Regedit. Usual disclaimers apply.
2.	Go to HKEY_CURRENT_USER\Software\Classes\directory\Background\shell
3.	Right-click shell and click New -> Key.
    The name of the key will be the name that shows up in the menu. 
    For our example, we’ll use “BackFolder”
5.	Next, right-click “BackFolder” and select New -> Key to create a sub key. 
    This key must be named ***command***. 
6.	Select ***command***, and on the right pane double-click (Default) to edit it. 
7.	Here is where we need to tell it where our program is that will run when the BackFolder is pressed. Set it to the path where our program is. Be sure to wrap it in quotes if there are spaces in the file path.

### The Result
Let's try it. Open File Explorer and nagivate to a few places so that you have a history. 
Now Right-click in a blank area in the right file pane. 
You should see the “BackFolder” entry. 
Click it and you should be taken back to the previous folder. 


