#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <tchar.h>
#include <shobjidl.h> 
#include <atlstr.h>
#include <vector>
#include <locale>
#include <codecvt>
#include "Node.h"
#include "WinZipRaLib.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::wstring filePath;
std::wstring dbpath;
bool isIndex = false;

std::string GetPath(std::string path)
{
    std::string pathUnzip;
    int dotCount = 0;
    for (int i = path.size();; i--)
        if (path[i] == '.')
        {
            dotCount++;
            if (dotCount == 2)
            {
                int size = path.size();
                std::string firstPart = path;
                std::string secondPart = path.erase(0, i);
                secondPart = path.erase(path.size()-4, 4);
                return firstPart.erase(i, size - i) + "UnZIP" + secondPart;
            }
        }

}

int CALLBACK  wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
    MSG msg{};
    HWND hwnd{};
    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"MyAppClass";
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    if (!RegisterClassEx(&wc))
    {
        return EXIT_FAILURE;
    }


    if (hwnd = CreateWindow(wc.lpszClassName, L"Paint", WS_OVERLAPPEDWINDOW, 0, 0, 1000, 500, nullptr, nullptr, wc.hInstance, nullptr);
        hwnd == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int> (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;


    static HWND hEdit;
    static HWND hListBox;
    static HMODULE hLib;


    switch (messg)
    {
    case WM_CREATE:
    {
        HWND hButtonBrowse = CreateWindow(
            L"BUTTON",
            L"Browse",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 0, 150, 50, hWnd, reinterpret_cast<HMENU>(0), nullptr, nullptr
        );
        HWND hButtonFind = CreateWindow(
            L"BUTTON",
            L"ZIP",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 50, 150, 50, hWnd, reinterpret_cast<HMENU>(1), nullptr, nullptr
        );
        HWND hFileBrowse = CreateWindow(
            L"BUTTON",
            L"UNZIP",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 175, 150, 50, hWnd, reinterpret_cast<HMENU>(5), nullptr, nullptr
        );


        SendMessage(GetDlgItem(hWnd, 2), CB_ADDSTRING, 0, (LPARAM)L"Phone");

        SendMessage(GetDlgItem(hWnd, 2), CB_ADDSTRING, 0, (LPARAM)L"Second name");

        SendMessage(GetDlgItem(hWnd, 2), CB_ADDSTRING, 0, (LPARAM)L"Streat");
    }
    return 0;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 0:
        {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
                COINIT_DISABLE_OLE1DDE);
            IFileOpenDialog* pFileOpen;
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
            hr = pFileOpen->Show(NULL);
            IShellItem* pItem;
            LPWSTR pszFilePath;
            hr = pFileOpen->GetResult(&pItem);
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            const wchar_t* file = pszFilePath;
            std::wstring widestr(file);
            filePath = widestr;
            CoTaskMemFree(pszFilePath);
            pItem->Release();
            pFileOpen->Release();
            CoUninitialize();
        }
        break;
        case 1:
        {
            if (filePath.size() == 0)
                return 0;
            
            std::string path(filePath.begin(), filePath.end());

            Zip(path);
        }
        break;
        case 5:
        {
            if (filePath.size() == 0)
                return 0;

            std::string path(filePath.begin(), filePath.end());
            std::string a = GetPath(path);
            UnZip(path, GetPath(path));
        }
        break;
        default:
            break;
        }
    }
    return 0;
    case WM_DESTROY:
    {
        PostQuitMessage(EXIT_SUCCESS);
    }
    return 0;


    default:
    {
        return(DefWindowProc(hWnd, messg, wParam, lParam));
    }
    }
    return 0;
}