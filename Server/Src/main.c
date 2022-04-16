#include <windows.h>
#include <stdio.h>

int main()
{
    HANDLE hComm = CreateFile("\\\\.\\COM3",
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error in opening serial port.\n");
    else
        printf("Opening serial port successful.\n");

    DWORD dwBytesRead;
    char data[64];
    while (1)
    {
        ReadFile(hComm, &data, 64, &dwBytesRead, NULL);
        data[dwBytesRead] = '\0';
        printf("data: %s\n", data);
    }

    CloseHandle(hComm);
    return 0;
}