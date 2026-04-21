#include <windows.h>
#include <stdio.h>

#define N 15
#define SIZE 30
#define OFFSET 40
#define R 13

int board[N][N] = { 0 };
int turn = 1;
int over = 0;

int getPos(int mouse, int offset, int size) {
    for (int i = 0; i < N; i++) {
        int center = offset + i * size;
        if (mouse > center - size / 2 && mouse < center + size / 2)
            return i;
    }
    return -1;
}

int checkWin(int x, int y, int player) {
    int dx[4] = { 1, 0, 1, 1 };
    int dy[4] = { 0, 1, 1, -1 };

    for (int d = 0; d < 4; d++) {
        int count = 1;

        for (int s = 1; s <= 4; s++) {
            int nx = x + dx[d] * s;
            int ny = y + dy[d] * s;
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) break;
            if (board[nx][ny] == player) count++;
            else break;
        }

        for (int s = 1; s <= 4; s++) {
            int nx = x - dx[d] * s;
            int ny = y - dy[d] * s;
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) break;
            if (board[nx][ny] == player) count++;
            else break;
        }

        if (count >= 5) return 1;
    }
    return 0;
}

int full() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == 0) return 0;
    return 1;
}

void drawPiece(HDC hdc, int x, int y, int player) {
    int cx = OFFSET + x * SIZE;
    int cy = OFFSET + y * SIZE;

    HBRUSH br = CreateSolidBrush(player == 1 ? RGB(0, 0, 0) : RGB(255, 255, 255));
    SelectObject(hdc, br);
    Ellipse(hdc, cx - R, cy - R, cx + R, cy + R);
    DeleteObject(br);
}

void drawBoard(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, pen);

    for (int i = 0; i < N; i++) {
        int pos = OFFSET + i * SIZE;
        MoveToEx(hdc, OFFSET, pos, NULL);
        LineTo(hdc, OFFSET + (N - 1) * SIZE, pos);

        MoveToEx(hdc, pos, OFFSET, NULL);
        LineTo(hdc, pos, OFFSET + (N - 1) * SIZE);
    }

    DeleteObject(pen);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j])
                drawPiece(hdc, i, j, board[i][j]);

    char text[64];
    if (over == 0)
        sprintf_s(text, "%s »ØºÏ", turn == 1 ? "ºÚ" : "°×");
    else if (over == 1)
        sprintf_s(text, "ºÚÆåÊ¤");
    else if (over == 2)
        sprintf_s(text, "°×ÆåÊ¤");
    else
        sprintf_s(text, "Æ½¾Ö");

    TextOut(hdc, 10, 10, text, strlen(text));

    EndPaint(hwnd, &ps);
}

void place(HWND hwnd, int mx, int my) {
    if (over) return;

    int gx = getPos(mx, OFFSET, SIZE);
    int gy = getPos(my, OFFSET, SIZE);
    if (gx < 0 || gy < 0) return;
    if (board[gx][gy]) return;

    board[gx][gy] = turn;

    if (checkWin(gx, gy, turn)) {
        over = turn;
        MessageBox(hwnd, turn == 1 ? "ºÚÆåÊ¤" : "°×ÆåÊ¤", "½áÊø", MB_OK);
    }
    else if (full()) {
        over = 3;
        MessageBox(hwnd, "Æ½¾Ö", "½áÊø", MB_OK);
    }
    else {
        turn = (turn == 1) ? 2 : 1;
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

void resetGame() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = 0;
    turn = 1;
    over = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT:
        drawBoard(hwnd);
        return 0;
    case WM_LBUTTONDOWN:
        place(hwnd, LOWORD(lp), HIWORD(lp));
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_SPACE) {
            resetGame();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (wp == VK_ESCAPE) {
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "OldGomoku";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "OldGomoku",
        "Îå×ÓÆå",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        OFFSET * 2 + SIZE * (N - 1),
        OFFSET * 2 + SIZE * (N - 1),
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}