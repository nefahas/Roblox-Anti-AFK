#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <iostream>
#include <thread>

// nepaha ;-;

bool HasWindowTitle(HWND hwnd) {
	wchar_t title[256];

	GetWindowTextW(hwnd, title, 256);

	return (std::wstring(title) == L"Roblox");
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	std::vector<HWND>* hwnds = reinterpret_cast<std::vector<HWND>*>(lParam);

	if (!IsWindowVisible(hwnd)) {
		return TRUE;
	}

	if (HasWindowTitle(hwnd)) {
		hwnds->push_back(hwnd);
	}

	return TRUE;
}

void SendSpaceToWindow(HWND hwnd) {
	INPUT inputs[2] = {};

	// use wscan and scancodes bc simulated no hardware keypresses are ignored by roblox
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_SPACE;
	inputs[0].ki.wScan = MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC);
	inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_SPACE;
	inputs[1].ki.wScan = MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC);
	inputs[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

	SendInput(1, &inputs[0], sizeof(INPUT));
	std::this_thread::sleep_for(std::chrono::milliseconds(2));
	SendInput(1, &inputs[1], sizeof(INPUT));
}

int main() {
	std::atomic<bool> keepRun(true);

	auto start = std::chrono::steady_clock::now() - std::chrono::minutes(5);

	while (keepRun) {
		std::vector<HWND> windows;

		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

		if (windows.empty()) {
			keepRun = false;

			std::cout << "roblox not open";

			return 1;
		}

		for (HWND hwnd : windows) {
			ShowWindow(hwnd, SW_RESTORE);

			std::this_thread::sleep_for(std::chrono::milliseconds(250));

			SendSpaceToWindow(hwnd);

			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			ShowWindow(hwnd, SW_MINIMIZE);
		}

		auto end = std::chrono::steady_clock::now();

		auto elapsed = (std::chrono::duration_cast<std::chrono::minutes>(end - start));

		if (elapsed < std::chrono::minutes(5)) {
			std::this_thread::sleep_for(std::chrono::minutes(5) - elapsed);
		}

		start = std::chrono::steady_clock::now();
	}

	return 0;
}