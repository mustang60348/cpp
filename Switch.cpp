#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Menu item and button identifiers
#define ID_FILE_EXIT 9001
#define ID_BUTTON_EXIT 9002

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register the window class
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window with a size of 1024x768
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class name
        "Sample Window",                // Window title
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Create a menu
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();

    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, "Exit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hFileMenu, "File");

    SetMenu(hwnd, hMenu);

    // Create the Exit button
    HWND hButton = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Exit",    // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        0, 0, 100, 30,        // Position and size (x, y, width, height)
        hwnd,     // Parent window
        (HMENU)ID_BUTTON_EXIT,       // Button ID
        hInstance,
        NULL);      // Pointer not needed

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void DrawColumnsAndBall(HDC hdc, RECT clientRect)
{
    int numColumns = 8;
    int totalWidth = clientRect.right - clientRect.left;
    int columnWidth = totalWidth / (2 * numColumns + 1); // Each column occupies a fraction of the total width
    int spacing = columnWidth; // Evenly spaced

    HBRUSH hBrushColumn = CreateSolidBrush(RGB(105, 105, 105)); // Dark grey color for columns
    HBRUSH hBrushBall = CreateSolidBrush(RGB(0, 0, 255)); // Blue color for ball

    // Calculate the bottom position of the columns, 50 pixels from the bottom of the client area
    int columnBottom = clientRect.bottom - 50;

    // Draw the columns
    for (int i = 0; i < numColumns; i++)
    {
        int x = clientRect.left + spacing * (i + 1) + columnWidth * i;
        RECT columnRect = { x, clientRect.top + 100, x + columnWidth, columnBottom };
        FillRect(hdc, &columnRect, hBrushColumn);
    }

    // Draw the ball above the first space
    int ballX = clientRect.left ; // X position of the ball centered over the first space
    int ballY = clientRect.top + 50; // Y position of the ball (above the first space)
    Ellipse(hdc, ballX, ballY - columnWidth, ballX + columnWidth, ballY);

    // Clean up
    DeleteObject(hBrushColumn);
    DeleteObject(hBrushBall);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
        {
            // Position the Exit button in the bottom right corner
            RECT rect;
            GetClientRect(hwnd, &rect);

            HWND hButton = GetDlgItem(hwnd, ID_BUTTON_EXIT);
            SetWindowPos(hButton, NULL,
                         rect.right - 110, rect.bottom - 40, 100, 30,
                         SWP_NOZORDER | SWP_NOACTIVATE);

            // Invalidate the window to trigger a repaint
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_FILE_EXIT:
                case ID_BUTTON_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear the background
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            // Get client area size
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);

            // Draw the columns and the ball
            DrawColumnsAndBall(hdc, clientRect);

            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
