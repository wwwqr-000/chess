#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

//Use -luser32 and -lgdi32

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>

#include "classes/dimensions.hpp"
#include "classes/particle.hpp"

LRESULT CALLBACK winProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassName[] = _T("wwwqrAppChess");

HWND hwnd;
bool active = true;
std::vector<std::thread> threads;
std::vector<particle> particles;
int tickTime;

void free() {
    active = false;
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void tick() {
    while (active) {
        InvalidateRect(hwnd, NULL, FALSE);
        ++tickTime;
        Sleep(7);//For ~144Hz
    }
}

int randInt(int min_num, int max_num) {
    return min_num + (rand() % max_num);
}

void threadSetup() {
    threads.emplace_back([]{ tick(); });
}

void createParticleTest() {
    for (int i = 0; i < 10; i++) {
        particles.emplace_back(particle(point2(randInt(0, 1920 / 2), randInt(0, 5)), point3(255, 255, 255), 5, 40, 1, 1, point2(1, 1)));
    }
}

void drawScreen(HDC &hdcMem, int width, int height) {
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    std::vector<COLORREF> pixels(width * height);

    if (tickTime % 10 == 0) {
        createParticleTest();
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            pixels[y * width + x] = RGB(0, 0, 0);
        }
    }

    for (auto& partcl : particles) {
        bool fallen = partcl.getFallen();
        if (tickTime % partcl.getDirExecTime() > 10 && !fallen) {
            partcl.getDirExecTime() = randInt(0, 100);
        }
        if (tickTime % partcl.getDirChangeTime() == 0 && !fallen) {
            partcl.getDirection() = randInt(0, 3);
        }

        if (tickTime % partcl.getDirExecTime() == 0 && !fallen) {
            int dir = partcl.getDirection();
            switch (dir) {
                case 0:
                    partcl.getPos().x_i--;
                break;
                case 1:
                    partcl.getPos().x_i++;
                break;
            }
        }

        if (partcl.getPos().y_i >= 500) {
            partcl.getFallen() = true;
        }
        else {
            partcl.getPos().y_i++;
        }
        for (int x = partcl.getPos().x_i; x < partcl.getPos().x_i + partcl.getBox().x_i; x++) {
            for (int y = partcl.getPos().y_i; y < partcl.getPos().y_i + partcl.getBox().y_i; y++) {
                point3 rgb = partcl.getRGB();
                pixels[y * width + x] = RGB(rgb.x_i, rgb.y_i, rgb.z_i);
            }
        }
    }

    SetDIBitsToDevice(hdcMem, 0, 0, width, height, 0, 0, 0, height, pixels.data(), &bmi, DIB_RGB_COLORS);
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    MSG msgs;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = winProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl)) { return 0; }
    hwnd = CreateWindowEx(0, szClassName, _T("Chess"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1920 / 2, 1080 / 2, HWND_DESKTOP, NULL, hThisInstance, NULL);
    //Initialize area external functions
    std::srand((unsigned) time(NULL));//Set random seed
    createParticleTest();
    threadSetup();
    //
    ShowWindow(hwnd, nCmdShow);

    while (GetMessage(&msgs, NULL, 0, 0)) {
        TranslateMessage(&msgs);
        DispatchMessage(&msgs);
    }

    return msgs.wParam;
}

LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC hdcMem = NULL;
    static HBITMAP hbmMem = NULL;
    static HBITMAP hbmOld = NULL;
    static HDC hdcWin = NULL;
    static RECT rect;
    static PAINTSTRUCT ps;

    switch (msg) {
        case WM_CREATE:
            GetClientRect(hwnd, &rect);
            hdcWin = GetDC(hwnd);
            hdcMem = CreateCompatibleDC(hdcWin);
            hbmMem = CreateCompatibleBitmap(hdcWin, rect.right, rect.bottom);
            hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
            ReleaseDC(hwnd, hdcWin);
            return 0;

        case WM_PAINT:
            hdcWin = BeginPaint(hwnd, &ps);
            FillRect(hdcMem, &rect, (HBRUSH)(COLOR_WINDOW + 1));
            drawScreen(hdcMem, rect.right, rect.bottom);
            BitBlt(hdcWin, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            free();
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
