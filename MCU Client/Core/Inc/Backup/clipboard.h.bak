#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#ifdef _WIN32 // use Win API for windows clipboard management
    #include <windows.h>
    
    void set_clipboard_content(const char* buffer)
    {
        const size_t buffer_len = strlen(buffer) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, buffer_len);
        memcpy(GlobalLock(hMem), buffer, buffer_len);
        GlobalUnlock(hMem);
        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
    }

    void get_clipboard_content(char** buffer)
    {
        if (!OpenClipboard(NULL))
            return;
        
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == NULL)
            return;
        
        *buffer = strdup((char*)hData);

        CloseClipboard();
    }

// end _WIN32

// workaround so I can compile properly on linux and re-build on windows in STM32CubeIDE
#else
    void set_clipboard_content(const char* buffer) {}
    void get_clipboard_content(char** buffer) {}

#endif

#endif // CLIPBOARD_H