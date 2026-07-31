### The Idea

Web browsers have had a **Back** button forever. You can even right-click on a page and choose **Back** from the context menu.

That got me wondering...

Wouldn't it be fun if Windows File Explorer had a similar option? Right-click in a folder, click **Back**, and Explorer would navigate to the previous folder.

Is it particularly useful? Probably not.

Is it a fun little Windows hack? Absolutely.

---

### The Plan

To make this work, we need two things:

1. Add a custom item to File Explorer's right-click menu.
2. Have that menu item launch a program that tells Explorer to navigate back.

The first part is just a Registry change.

The second part turned out to be more interesting.

---

## Writing the Program

The biggest challenge was figuring out **which File Explorer window** launched our program.

Since users can have several Explorer windows open, my first thought was to enumerate every Explorer window and find the one that currently had focus.

That turned out to be completely unnecessary.

Our utility is a simple console application and never creates a window of its own. When it starts, the File Explorer window that launched it is still the foreground window. That means we can simply retrieve the foreground window and send the appropriate keyboard shortcut to it.

Much simpler.

---

## Sending the Shortcut

So what keyboard shortcut makes Explorer go back?

**Alt + Left Arrow**

The next question was how to send that key combination.

My first attempt used the Windows API `SendMessage()` to simulate key presses. After a little searching, I discovered `keybd_event()`, which worked perfectly.

Unfortunately, I later learned that `keybd_event()` has been deprecated. The modern approach is to use `SendInput()` instead.

> **Lesson learned:** Always verify what AI tells you before shipping code.

Here's the final version: 
```cpp
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

## Adding the Context Menu

Now we just need to tell File Explorer to run our program when the user clicks a menu item.

> **Warning:** Editing the Windows Registry incorrectly can cause problems. Proceed carefully.

1. Run **Regedit**.
2. Navigate to:

   ```
   HKEY_CURRENT_USER\Software\Classes\Directory\Background\shell
   ```

3. Right-click **shell** and choose **New → Key**.
4. Name the new key **BackFolder** (or whatever text you want displayed in the menu).
5. Right-click **BackFolder** and create another key named **command**.
6. Select the **command** key.
7. Double-click **(Default)** in the right pane.
8. Enter the full path to your executable, enclosing it in quotes if the path contains spaces.

For example:

```
"C:\Tools\BackFolder.exe"
```

That's it. No reboot is required.

---
## The Result

Open File Explorer and browse through several folders so you have some navigation history.

Now right-click in an empty area of the folder background.

You should see your new **BackFolder** command in the context menu.

Click it, and Explorer immediately navigates back to the previous folder—the same as if you had pressed **Alt + Left Arrow**.

It's a tiny utility, but it's a fun example of how easily you can extend File Explorer with a little Registry editing and a few lines of Win32 code.
That's it. No reboot is required.

---


