#include <windows.h>
#include <vector>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define BLOCK_COUNT 14
#define BLOCK_HEIGHT 30 // Restored original height for square shape
#define BLOCK_SPACING 20  // Space between blocks
#define SIDE_MARGIN 30    // Space at the beginning and end of the row
#define BORDER_THICKNESS 25 // Thickness of the grey border
#define BALL_SIZE 20 // Size of the blue ball
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define TIMER_ID 1
#define FRAME_DELAY 10 // Milliseconds between frames
#define ROW_SPACING 15 // Space between rows

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct Block {
    RECT rect;
    bool visible;
    COLORREF color;
};

std::vector<Block> blocks;

struct Ball {
    int x, y;
    int dx, dy;
};

Ball ball = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 5, 5};

struct Paddle {
    int x, y;
    int width, height;
};

Paddle paddle = {WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - PADDLE_HEIGHT - 30, PADDLE_WIDTH, PADDLE_HEIGHT};

void InitBlocks() {
    int totalSpacing = BLOCK_SPACING * (BLOCK_COUNT - 1);
    int availableWidth = WINDOW_WIDTH - (2 * SIDE_MARGIN) - (2 * BORDER_THICKNESS) - totalSpacing;
    int blockWidth = availableWidth / BLOCK_COUNT; // Restored original width calculation for square shape

    COLORREF colors[6] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 165, 0), RGB(128, 0, 128) };

    for (int row = 0; row < 6; ++row) {
        int yOffset = BORDER_THICKNESS + row * (BLOCK_HEIGHT + ROW_SPACING);
        for (int i = 0; i < BLOCK_COUNT; ++i) {
            int x = SIDE_MARGIN + i * (blockWidth + BLOCK_SPACING) + BORDER_THICKNESS;
            RECT rect = { x, yOffset, x + blockWidth, yOffset + BLOCK_HEIGHT };
            blocks.push_back({ rect, true, colors[row] });
        }
    }
}

void MoveBall(HWND hwnd) {
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Bounce off walls
    if (ball.x <= BORDER_THICKNESS || ball.x + BALL_SIZE >= WINDOW_WIDTH - BORDER_THICKNESS) {
        ball.dx = -ball.dx;
    }
    if (ball.y <= BORDER_THICKNESS) {
        ball.dy = -ball.dy;
    }

    // Bounce off paddle
    if (ball.y + BALL_SIZE >= paddle.y && ball.x + BALL_SIZE >= paddle.x && ball.x <= paddle.x + paddle.width) {
        ball.dy = -ball.dy;
    }

    // Check collision with blocks
    for (auto& block : blocks) {
        if (block.visible && ball.x < block.rect.right && ball.x + BALL_SIZE > block.rect.left &&
            ball.y < block.rect.bottom && ball.y + BALL_SIZE > block.rect.top) {
            block.visible = false; // Hide the block
            ball.dy = -ball.dy; // Reverse ball direction
            RECT invalidRect = block.rect;
            InvalidateRect(hwnd, &invalidRect, TRUE); // Invalidate the area of the block to erase it
            break;
        }
    }

    // Reset if ball goes below the paddle
    if (ball.y + BALL_SIZE >= WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = WINDOW_HEIGHT / 2;
        ball.dy = -ball.dy;
    }

    // Invalidate only the area where the ball moves for smoother rendering
    RECT ballRect = { ball.x - abs(ball.dx), ball.y - abs(ball.dy), ball.x + BALL_SIZE + abs(ball.dx), ball.y + BALL_SIZE + abs(ball.dy) };
    InvalidateRect(hwnd, &ballRect, FALSE);
}

void DrawBlocks(HDC hdc) {
    for (const auto& block : blocks) {
        if (block.visible) {
            HBRUSH hBrush = CreateSolidBrush(block.color);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            HPEN hPen = CreatePen(PS_SOLID, 1, block.color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            // Draw the block with rounded corners
            int cornerRadius = 10; // Adjust the corner radius as needed
            RoundRect(hdc, block.rect.left, block.rect.top, block.rect.right, block.rect.bottom, cornerRadius, cornerRadius);

            // Clean up
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    }
}


void DrawBall(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);
    Ellipse(hdc, ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void DrawPaddle(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    RECT paddleRect = {paddle.x, paddle.y, paddle.x + paddle.width, paddle.y + paddle.height};
    FillRect(hdc, &paddleRect, hBrush);
    DeleteObject(hBrush);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Colored Blocks Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    InitBlocks();

    // Set a timer for smooth frame updates
    SetTimer(hwnd, TIMER_ID, FRAME_DELAY, NULL);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_TIMER: {
            MoveBall(hwnd);
        }
        return 0;

        case WM_MOUSEMOVE: {
            int mouseX = LOWORD(lParam);
            paddle.x = mouseX - paddle.width / 2;

            // Keep paddle within the window bounds
            if (paddle.x < BORDER_THICKNESS) {
                paddle.x = BORDER_THICKNESS;
            } else if (paddle.x + paddle.width > WINDOW_WIDTH - BORDER_THICKNESS) {
                paddle.x = WINDOW_WIDTH - BORDER_THICKNESS - paddle.width;
            }

            // Invalidate the paddle area for smoother rendering
            RECT paddleRect = { paddle.x - 5, paddle.y - 5, paddle.x + paddle.width + 5, paddle.y + paddle.height + 5 };
            InvalidateRect(hwnd, &paddleRect, FALSE);
        }
        return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Set up double buffering
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

            // Set the color to grey for the border
            HBRUSH hBorderBrush = CreateSolidBrush(RGB(169, 169, 169));

            // Draw the grey border around the play area (filled)
            RECT borderRect = { 
                0, 
                0, 
                WINDOW_WIDTH, 
                WINDOW_HEIGHT 
            };
            FillRect(hdcMem, &borderRect, hBorderBrush);

            // Draw the blocks
            DrawBlocks(hdcMem);

            // Draw the ball
            DrawBall(hdcMem);

            // Draw the paddle
            DrawPaddle(hdcMem);

            // Transfer the off-screen buffer to the screen
            BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);

            // Clean up
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            DeleteObject(hBorderBrush);
            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}