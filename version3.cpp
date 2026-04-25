#include <windows.h>
#include <stdio.h>

const int a = 15;
const int b = 30;
const int c = 40;
const int d = 13;

const int ww = c * 2 + b * (a - 1);
const int wh = c * 2 + b * (a - 1);

int q[a][a] = { 0 };
int p = 1;
int over = 0;

int history[a * a][2];
int step = 0;

int getxy(int pos, int off, int sz, int max) {
	for (int i = 0; i < max; i++) {
		int center = off + i * sz;
		if (pos > center - sz / 2 && pos < center + sz / 2)return i;
	}
	return -1;
}

int check(int x, int y, int player) {
	int dx[4] = { 1,0,1,1 };
	int dy[4] = { 0,1,1,-1 };

	for (int i = 0; i < 4; i++) {
		int cnt = 1;
		for (int s = 1; s <= 4; s++) {
			int nx = x + dx[i] * s;
			int ny = y + dy[i] * s;
			if (nx < 0 || nx >= a || ny < 0 || ny >= a)break;
			if (q[nx][ny] == player)cnt++;
			else break;
		}
		for (int s = 1; s <= 4; s++) {
			int nx = x - dx[i] * s;
			int ny = y - dy[i] * s;
			if (nx < 0 || nx >= a || ny < 0 || ny >= a)break;
			if (q[nx][ny] == player)cnt++;
			else break;
		}
		if (cnt >= 5)return 1;
	}
	return 0;
}

int checkfull() {
	for (int i = 0; i < a; i++)
		for (int j = 0; j < a; j++)
			if (q[i][j] == 0)return 0;
	return 1;
}

void drawq(HDC hdc, int x, int y, int player) {
	int cx = c + x * b;
	int cy = c + y * b;
	HBRUSH brush = CreateSolidBrush(player == 1 ? RGB(0, 0, 0) : RGB(255, 255, 255));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	Ellipse(hdc, cx - d, cy - d, cx + d, cy + d);
	SelectObject(hdc, oldbrush);
	DeleteObject(brush);
	if (player == 2) {
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HPEN oldpen = (HPEN)SelectObject(hdc, pen);
		Ellipse(hdc, cx - d, cy - d, cx + d, cy + d);
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
	}
}

void draw(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	RECT rect;
	GetClientRect(hwnd, &rect);
	HBRUSH bg = CreateSolidBrush(RGB(222, 184, 135));
	FillRect(hdc, &rect, bg);
	DeleteObject(bg);
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN oldpen = (HPEN)SelectObject(hdc, pen);
	for (int i = 0; i < a; i++) {
		int pos = c + i * b;
		MoveToEx(hdc, c, pos, NULL);
		LineTo(hdc, c + (a - 1) * b, pos);
		MoveToEx(hdc, pos, c, NULL);
		LineTo(hdc, pos, c + (a - 1) * b);
	}
	int star[5][2] = { {3,3},{11,3},{7,7},{3,11},{11,11} };
	for (int i = 0; i < 5; i++) {
		int sx = c + star[i][0] * b;
		int sy = c + star[i][1] * b;
		HBRUSH sbrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, sbrush);
		Ellipse(hdc, sx - 4, sy - 4, sx + 4, sy + 4);
		SelectObject(hdc, oldbrush);
		DeleteObject(sbrush);
	}
	SelectObject(hdc, oldpen);
	DeleteObject(pen);
	for (int i = 0; i < a; i++)
		for (int j = 0; j < a; j++)
			if (q[i][j] != 0)
				drawq(hdc, i, j, q[i][j]);
	char txt[100];
	if (over == 0)
		sprintf_s(txt, "%s方 (空格重开 ESC退出)", p == 1 ? "黑" : "白");
	else if (over == 1)
		sprintf_s(txt, "黑棋胜利！(空格重开 ESC退出)");
	else if (over == 2)
		sprintf_s(txt, "白棋胜利！(空格重开 ESC退出)");
	else
		sprintf_s(txt, "平局！(空格重开 ESC退出)");
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkColor(hdc, RGB(222, 184, 135));
	TextOut(hdc, 10, 10, txt, strlen(txt));
	EndPaint(hwnd, &ps);
}

void put(HWND hwnd, int mx, int my) {
	if (over != 0)return;
	int gx = getxy(mx, c, b, a);
	int gy = getxy(my, c, b, a);
	if (gx == -1 || gy == -1)return;
	if (q[gx][gy] != 0)return;
	q[gx][gy] = p;
	history[step][0] = gx;
	history[step][1] = gy;
	step++;
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
	if (check(gx, gy, p)) {
		over = p;
		MessageBox(hwnd, p == 1 ? "黑棋胜利！" : "白棋胜利！", "游戏结束", MB_OK);
		return;
	}
	if (checkfull()) {
		over = 3;
		MessageBox(hwnd, "平局！", "游戏结束", MB_OK);
		return;
	}
	p = (p == 1) ? 2 : 1;
}

void undo() {
	if (step == 0) return;
	step--;
	int x = history[step][0];
	int y = history[step][1];
	q[x][y] = 0;
	p = (p == 1) ? 2 : 1;
	if (over != 0) over = 0;
}

void reset() {
	for (int i = 0; i < a; i++)
		for (int j = 0; j < a; j++)
			q[i][j] = 0;
	p = 1;
	over = 0;
	step = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_PAINT:
		draw(hwnd);
		return 0;
	case WM_LBUTTONDOWN:
		put(hwnd, LOWORD(lp), HIWORD(lp));
		return 0;
	case WM_KEYDOWN:
		if (wp == VK_SPACE) {
			reset();
			InvalidateRect(hwnd, NULL, TRUE);
		}
		else if (wp == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		else if (wp == VK_BACK) {
			undo();
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int n) {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = h;
	wc.lpszClassName = "Gomoku";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(0, "Gomoku", "五子棋", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, ww, wh,
		NULL, NULL, h, NULL);
	ShowWindow(hwnd, n);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}