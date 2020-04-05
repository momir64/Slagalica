#include <windows.h>
#include <VersionHelpers.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <conio.h>
#include <codecvt>
#include <iomanip>
#include <thread>
#include <random>
#include <string>
#include <locale>
#include <mutex>
#include <io.h>
#include "resource.h"

using namespace std;

HANDLE ConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE ConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);
mutex mtx;
bool kraj = false, isteklovreme = false, mute = false, interupt = false;
char *recnik;
DWORD recnikSize;
wstring slova(12, NULL);
wstring svasloval(L"ABVOGDUĐEŽZIJKULQAMNOWPIRSETAĆFUHCIČXOŠE");
wstring svaslovalm(L"abvogduđežzijkulqamnowpirsetaćfuhcičxoše");
wstring svaslovam(L"абвогдуђежзијкулљамноњпирсетаћфухцичџоше");
wstring svaslova(L"АБВОГДУЂЕЖЗИЈКУЛЉАМНОЊПИРСЕТАЋФУХЦИЧЏОШЕ");
//                 А  О	 У Е  И  У  А  О  И  Е А  У  И  О Е 
//				   А	   Е  И		   О 	      У
//					  О          У  А	     Е	     И
//						 У				  И    А	    О Е
//	ODNOS SAMOGLASNICI/SUGLASINICI = 37.5%

void UcitajKlik(int &, int &);
void gotoxy(int, int);
void SakriKursor();
void Mute();
void CentrirajKonzolu();
void PodesiKonzolu();
void OdstampajKonzolu();
bool ProveriRec(wstring);
void tajmer();
void engleska(wchar_t &);
wstring FaleSlova(wstring);
void PrintSredina(wstring);
void Slika(int);
void Muzika();
bool UcitajRecnik();
BOOL WINAPI EXIT(DWORD);

int main() {
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)EXIT, TRUE);
	thread muz(Muzika);
	muz.detach();
	Slika(1);
	PodesiKonzolu();
	thread mut(Mute);
	mut.detach();
	if (!UcitajRecnik())
		return -1;

	while (true) {
		kraj = true;
		isteklovreme = false;
		SetConsoleTitleW(L"СЛАГАЛИЦА");
		OdstampajKonzolu();

		while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
		while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);

		mtx.lock();
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		gotoxy(4, 11);
		std::wcout << L"       За бирање слова притисните спејс или ентер...       ";
		SakriKursor();
		mtx.unlock();
		for (int i = 0; i < 12; i++) {
			random_device rnd;
			mt19937 mt(rnd());
			uniform_int_distribution<mt19937::result_type> rang(0, 39);
			int j = rang(mt);
			do {
				slova[i] = svaslova[j];
				mtx.lock();
				SetConsoleTextAttribute(ConsoleOutputHandle, 22);
				gotoxy(11 + i * 4, 3);
				std::wcout << slova[i];
				SakriKursor();
				mtx.unlock();

				Sleep(80);

				mtx.lock();
				SetConsoleTextAttribute(ConsoleOutputHandle, 22);
				gotoxy(11 + i * 4, 3);
				std::wcout << L" ";
				SakriKursor();
				mtx.unlock();

				if (++j > 29)
					j = 0;
			} while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0);
			mtx.lock();
			SetConsoleTextAttribute(ConsoleOutputHandle, 22);
			gotoxy(11 + i * 4, 3);
			std::wcout << slova[i];
			SakriKursor();
			mtx.unlock();
		}

		mtx.lock();
		SetConsoleTextAttribute(ConsoleOutputHandle, 31);
		gotoxy(4, 7);
		std::wcout << L"       _   _   _   _   _   _   _   _   _   _   _   _       ";
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		gotoxy(4, 11);
		std::wcout << L"    Имате 60 секунди, када завршите притисните ентер...    ";
		SakriKursor();
		mtx.unlock();

		thread tm(tajmer);
		kraj = false;
		tm.detach();

		while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);

		wchar_t ch = NULL;
		wstring rec;
		do {
			Sleep(1);
			if (_kbhit()) {
				ch = _getwch();
				if (ch != L'\r' && ch == L'\b' && rec.size()) {
					mtx.lock();
					SetConsoleTextAttribute(ConsoleOutputHandle, 31);
					gotoxy(7 + rec.size() * 4, 7);
					std::wcout << "_";
					SakriKursor();
					mtx.unlock();
					rec.pop_back();
				}
				else if (ch != L'\r') {
					if (rec.size() > 11)
						continue;
					if (svaslovam.find(ch) != string::npos)
						ch = svaslova[svaslovam.find(ch)];
					else if (svasloval.find(ch) != string::npos)
						ch = svaslova[svasloval.find(ch)];
					else if (svaslovalm.find(ch) != string::npos)
						ch = svaslova[svaslovalm.find(ch)];
					else
						engleska(ch);

					if (svaslova.find(ch) != string::npos) {
						rec += ch;
						mtx.lock();
						SetConsoleTextAttribute(ConsoleOutputHandle, 22);
						gotoxy(7 + rec.size() * 4, 7);
						std::wcout << rec.back();
						SakriKursor();
						mtx.unlock();
					}
				}
			}
		} while (!kraj && GetKeyState(VK_RETURN) >= 0);
		if (kraj) {
			mtx.lock();
			SetConsoleTextAttribute(ConsoleOutputHandle, 22);
			gotoxy(4, 11);
			std::wcout << L"              Време за решавање је истекло...              ";
			SakriKursor();
			mtx.unlock();
			isteklovreme = true;
		}
		kraj = true;
		Sleep(50);

		if (!isteklovreme) {
			wstring recr(13, NULL);
			if (!rec.size()) {
				mtx.lock();
				SetConsoleTextAttribute(ConsoleOutputHandle, 22);
				gotoxy(4, 11);
				std::wcout << L"                 Унели сте празно решење...                ";
				SakriKursor();
				mtx.unlock();
			}
			else if (ProveriRec(rec)) {
				size_t i = 2;
				while (recr != rec && recr.size() >= rec.size() && i < recnikSize) {
					recr = L"";
					while ((recnik[i] != 10 || recnik[i + 1] != 0) && i < recnikSize) {
						recr += recnik[i + 1] << 8 | recnik[i];
						i += 2;
					}
					i += 2;
				}

				if (rec != recr) {
					mtx.lock();
					SetConsoleTextAttribute(ConsoleOutputHandle, 22);
					gotoxy(4, 11);
					std::wcout << L"     Ваша реч је неисправна, не налази се у речнику...     ";
					SakriKursor();
					mtx.unlock();
				}
				else {
					mtx.lock();
					SetConsoleTextAttribute(ConsoleOutputHandle, 22);
					gotoxy(4, 11);
					std::wcout << L"         Ваша реч је исправна и садржи " << setw(2) << rec.size() << L" слова...         ";
					SakriKursor();
					mtx.unlock();
				}
			}
			else {
				mtx.lock();
				SetConsoleTextAttribute(ConsoleOutputHandle, 22);
				gotoxy(4, 11);
				std::wcout << L"         Ваша реч је неисправна, фале Вам слова...         ";
				SakriKursor();
				mtx.unlock();
				while (GetKeyState(VK_RETURN) < 0);
				while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
				while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);
				PrintSredina(FaleSlova(rec));
			}
		}

		bool pauza = true;
		thread th([&pauza]() {
			while (GetKeyState(VK_RETURN) < 0);
			while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
			while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);
			pauza = false;
		});
		th.detach();

		wstring recr;
		size_t i = 2;
		do {
			recr = L"";
			while ((recnik[i] != 10 || recnik[i + 1] != 0) && i < recnikSize) {
				recr += recnik[i + 1] << 8 | recnik[i];
				i += 2;
			}
			i += 2;
		} while (!ProveriRec(recr) && i < recnikSize);

		while (pauza)Sleep(50);

		if (!recr.size()) {
			mtx.lock();
			SetConsoleTextAttribute(ConsoleOutputHandle, 22);
			gotoxy(4, 11);
			std::wcout << L"           Компјутер није успео да пронађе реч!            ";
			SakriKursor();
			mtx.unlock();
		}
		else {
			mtx.lock();
			SetConsoleTextAttribute(ConsoleOutputHandle, 22);
			gotoxy(4, 11);
			std::wcout << L"     Компјутер је успео да пронађе реч од " << setw(2) << recr.size() << L" слова...      ";
			SakriKursor();
			mtx.unlock();
			while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);
			while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
			while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);

			wstring tmp;
			int sizex = recr.size();
			for (int i = 0; i < sizex; i++) {
				tmp += wchar_t(recr[i]);
				tmp += L" ";
			}
			PrintSredina(L"То је реч  " + tmp);
		}

		while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
		while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);

		mtx.lock();
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		gotoxy(4, 11);
		std::wcout << L"                      Играј поново...                      ";
		SakriKursor();
		mtx.unlock();

		while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0)Sleep(50);
		while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);
	}

	return 0;
}

void UcitajKlik(int &x, int &y) {
	while (true) {
		DWORD tmp;
		INPUT_RECORD input;
		SetConsoleMode(ConsoleInputHandle, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
		do {
			Sleep(50);
			GetNumberOfConsoleInputEvents(ConsoleInputHandle, &tmp);
			if (interupt) { interupt = false; x = -1; y = -1; return; }
		} while (!tmp);
		ReadConsoleInputW(ConsoleInputHandle, &input, 1, &tmp);
		if (!(input.Event.MouseEvent.dwButtonState & 1)) {
			while (true) {
				do {
					Sleep(50);
					GetNumberOfConsoleInputEvents(ConsoleInputHandle, &tmp);
					if (interupt) { interupt = false; x = -1; y = -1; return; }
				} while (!tmp);
				ReadConsoleInputW(ConsoleInputHandle, &input, 1, &tmp);
				if (input.Event.MouseEvent.dwButtonState & 1) {
					while (true) {
						do {
							Sleep(50);
							GetNumberOfConsoleInputEvents(ConsoleInputHandle, &tmp);
							if (interupt) { interupt = false; x = -1; y = -1; return; }
						} while (!tmp);
						ReadConsoleInputW(ConsoleInputHandle, &input, 1, &tmp);
						if (!(input.Event.MouseEvent.dwButtonState & 1)) {
							x = input.Event.MouseEvent.dwMousePosition.X;
							y = input.Event.MouseEvent.dwMousePosition.Y;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void SakriKursor() {
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(ConsoleOutputHandle, &info);
}

void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(ConsoleOutputHandle, coord);
}

void Mute() {
	int x, y;
	while (true) {
		do
			UcitajKlik(x, y);
		while ((x != 65 || y != 16) && !mute);
		mute = true;

		mtx.lock();
		gotoxy(65, 16);
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		std::wcout << L"♪";
		SakriKursor();
		mtx.unlock();

		do
			UcitajKlik(x, y);
		while (x != 65 || y != 16);
		mute = false;

		mtx.lock();
		gotoxy(65, 16);
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		std::wcout << L"♫";
		SakriKursor();
		mtx.unlock();
	}
}

void CentrirajKonzolu() {
	RECT rectClient, rectWindow;
	HWND hWnd = GetConsoleWindow();
	GetClientRect(hWnd, &rectClient);
	GetWindowRect(hWnd, &rectWindow);
	int posx, posy;
	posx = GetSystemMetrics(SM_CXSCREEN) / 2 - (rectWindow.right - rectWindow.left) / 2;
	posy = GetSystemMetrics(SM_CYSCREEN) / 2 - (rectWindow.bottom - rectWindow.top) / 2;

	SetWindowPos(GetConsoleWindow(), HWND_TOP, posx, posy, 0, 0, SWP_NOSIZE);
}

void PodesiKonzolu() {
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);

	_CONSOLE_SCREEN_BUFFER_INFOEX info;
	info.cbSize = sizeof(info);
	GetConsoleScreenBufferInfoEx(ConsoleOutputHandle, &info);
	info.dwSize = { 67, 18 };
	info.ColorTable[1] = RGB(4, 18, 93);
	info.ColorTable[5] = RGB(255, 209, 41);
	info.ColorTable[6] = RGB(238, 238, 79);
	info.ColorTable[14] = RGB(249, 241, 165);
	info.ColorTable[15] = RGB(255, 255, 255);
	SetConsoleScreenBufferInfoEx(ConsoleOutputHandle, &info);

	CONSOLE_FONT_INFOEX infof = { 0 };
	infof.cbSize = sizeof(infof);
	infof.dwFontSize.Y = 38;
	infof.FontWeight = FW_NORMAL;
	wcscpy_s(infof.FaceName, 32, L"Consolas");
	SetCurrentConsoleFontEx(ConsoleOutputHandle, NULL, &infof);

	DWORD tmp;
	FillConsoleOutputAttribute(ConsoleOutputHandle, BACKGROUND_BLUE, 67 * 18, { 0, 0 }, &tmp);

	CentrirajKonzolu();
	CentrirajKonzolu();
}

void OdstampajKonzolu() {
	mtx.lock();
	SetConsoleTextAttribute(ConsoleOutputHandle, 31);
	gotoxy(0, 0);  std::wcout << L"                                                                   ";
	gotoxy(0, 1);  std::wcout << L"   ╔═══════════════════════════════════════════════════════════╗   ";
	gotoxy(0, 2);  std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 3);  std::wcout << L"   ║       _   _   _   _   _   _   _   _   _   _   _   _       ║   ";
	gotoxy(0, 4);  std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 5);  std::wcout << L"   ╠═══════════════════════════════════════════════════════════╣   ";
	gotoxy(0, 6);  std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 7);  std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 8);  std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 9);  std::wcout << L"   ╠═══════════════════════════════════════════════════════════╣   ";
	gotoxy(0, 10); std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 11); std::wcout << L"   ║      ";
	SetConsoleTextAttribute(ConsoleOutputHandle, 22);
	;                         std::wcout << L"Да бисте започели притисните спејс или ентер...";
	SetConsoleTextAttribute(ConsoleOutputHandle, 31);                       std::wcout << L"      ║   ";
	gotoxy(0, 12); std::wcout << L"   ║                                                           ║   ";
	gotoxy(0, 13); std::wcout << L"   ╚═══════════════════════════════════════════════════════════╝   ";
	gotoxy(0, 14); std::wcout << L"                                                                   ";
	gotoxy(0, 15); std::wcout << L"    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░    ";
	SetConsoleTextAttribute(ConsoleOutputHandle, 22);
	gotoxy(0, 16);
	if (mute)      std::wcout << L"                                                                 ♪ ";
	else           std::wcout << L"                                                                 ♫ ";
	gotoxy(0, 17); std::wcout << L"                                                                  ";
	SakriKursor();
	mtx.unlock();

		// █▓▒░
}

bool ProveriRec(wstring rec) {
	wstring tmp = slova;
	int i = 0, velicina = rec.size();
	bool postoji;
	while (i < velicina) {
		postoji = false;
		for (int j = 0; j < 12; j++)
			if (rec[i] == tmp[j]) {
				tmp[j] = NULL;
				postoji = true;
				i++;
				break;
			}
		if (!postoji)
			return false;
	}
	return true;
}

void tajmer() {
	if (!kraj) {
		int sekunde = 1;
		mtx.lock();
		gotoxy(4, 15);
		SetConsoleTextAttribute(ConsoleOutputHandle, 31);
		std::wcout << L"█";
		SakriKursor();
		mtx.unlock();
		if (kraj)
			return;
		Sleep(1017);
		if (kraj)
			return;
		do {
			mtx.lock();
			gotoxy(sekunde + 3, 15);
			SetConsoleTextAttribute(ConsoleOutputHandle, 30);
			std::wcout << L"▓";
			SetConsoleTextAttribute(ConsoleOutputHandle, 31);
			std::wcout << L"█";
			SakriKursor();
			mtx.unlock();
			if (kraj)
				return;
			Sleep(1017);
			if (kraj)
				return;
			sekunde++;
		} while (sekunde < 59 && !kraj);
	}
	kraj = true;
}

void engleska(wchar_t &ch) {
	if (ch == L'[')
		ch = L'Ш';
	else if (ch == L']')
		ch = L'Ђ';
	else if (ch == L';')
		ch = L'Ч';
	else if (ch == L'\'')
		ch = L'Ћ';
	else if (ch == L'\\')
		ch = L'Ж';
	else if (ch == L'y')
		ch = L'З';
	else if (ch == L'Y')
		ch = L'З';
}

wstring FaleSlova(wstring rec) {
	wstring tmp = slova, rtr;
	int i = 0, velicina = rec.size();
	bool postoji;
	while (i < velicina) {
		postoji = false;
		for (int j = 0; j < 12; j++)
			if (rec[i] == tmp[j]) {
				tmp[j] = NULL;
				postoji = true;
				break;
			}
		if (!postoji) {
			rtr += rec[i];
			rtr += L" ";
		}
		i++;
	}

	return rtr;
}

void PrintSredina(wstring str) {
	mtx.lock();
	SetConsoleTextAttribute(ConsoleOutputHandle, 22);
	gotoxy(4, 11);
	int size = str.size();
	for (int i = 0; i < (59 - size) / 2; i++)
		std::wcout << L" ";
	std::wcout << str;
	for (int i = 0; i < (59 - size) / 2 + (59 - size) % 2; i++)
		std::wcout << L" ";
	SakriKursor();
	mtx.unlock();
}

void Slika(int br) {
	_setmode(_fileno(stdout), _O_TEXT);
	_setmode(_fileno(stdin), _O_TEXT);

	if (br == 1) {
		SetConsoleTitleW(L"СЛАГАЛИЦА");
		HWND consoleWindow = GetConsoleWindow();
		SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX & ~WS_VSCROLL);
		SetConsoleMode(ConsoleInputHandle, ENABLE_EXTENDED_FLAGS);
	}

	_CONSOLE_SCREEN_BUFFER_INFOEX info;
	info.cbSize = sizeof(info);
	GetConsoleScreenBufferInfoEx(ConsoleOutputHandle, &info);
	info.dwSize = { 501, 143 };
	info.ColorTable[1] = RGB(4, 18, 93);
	info.ColorTable[5] = RGB(255, 209, 41);
	info.ColorTable[6] = RGB(238, 238, 79);
	info.ColorTable[14] = RGB(249, 241, 165);
	info.ColorTable[15] = RGB(255, 255, 255);
	SetConsoleScreenBufferInfoEx(ConsoleOutputHandle, &info);

	CONSOLE_FONT_INFOEX infof = { 0 };
	infof.cbSize = sizeof(infof);
	infof.dwFontSize.Y = 5;
	infof.FontWeight = FW_NORMAL;
	wcscpy_s(infof.FaceName, 32, L"Consolas");
	SetCurrentConsoleFontEx(ConsoleOutputHandle, NULL, &infof);

	DWORD tmp;
	FillConsoleOutputAttribute(ConsoleOutputHandle, BACKGROUND_BLUE, 501 * 143, { 0, 0 }, &tmp);
	system("MODE CON COLS=501 LINES=143");

	if (br == 1) {
		CentrirajKonzolu();
		CentrirajKonzolu();
	}

	string slika;
	HRSRC hRes = NULL;
	if (br == 1)
		hRes = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_IMG1), MAKEINTRESOURCEW(IMG));
	else if (br == 2)
		hRes = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_IMG2), MAKEINTRESOURCEW(IMG));
	if (hRes != NULL) {
		HGLOBAL hData = LoadResource(0, hRes);
		if (hData != NULL) {
			DWORD dataSize = SizeofResource(0, hRes);
			char *data = (char *)LockResource(hData);
			slika.assign(data, dataSize);

			SetConsoleTextAttribute(ConsoleOutputHandle, 21);
			gotoxy(0, 0);
			cout << slika;
			SakriKursor();

			if (br == 1) {
				clock_t start = clock();
				while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0 && (clock() - start) / CLOCKS_PER_SEC < 8)Sleep(50);
				while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0)Sleep(50);
			}
			else if (br == 2)
				Sleep(1111);
		}
	}
}

bool UcitajRecnik() {
	bool opened = true;
	HRSRC hRes = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_RECNIK1), MAKEINTRESOURCEW(RECNIK));
	if (hRes != NULL) {
		HGLOBAL hData = LoadResource(NULL, hRes);
		if (hData != NULL) {
			recnikSize = SizeofResource(NULL, hRes);
			recnik = (char *)LockResource(hData);
		}
		else
			opened = false;
	}
	else
		opened = false;

	if (!opened) {
		mtx.lock();
		SetConsoleTextAttribute(ConsoleOutputHandle, 22);
		gotoxy(4, 11);
		std::wcout << L"            Грешка приликом отварања речника...            ";
		SakriKursor();
		mtx.unlock();
		while (GetKeyState(VK_SPACE) >= 0 && GetKeyState(VK_RETURN) >= 0);
		while (GetKeyState(VK_SPACE) < 0 || GetKeyState(VK_RETURN) < 0);
		return false;
	}

	return true;
}

#define A_1      550
#define Ax1      580
#define A_2     1100
#define B_1      620
#define B_2     1230
#define C_2      650
#define Cx2      690
#define C_3     1310
#define Cx3     1390
#define D_1      370
#define Dx1      390
#define D_2      730
#define Dx2      780
#define D_3     1470
#define Dx3     1560
#define E_1      410
#define E_2      820
#define E_3     1650
#define F_1      440
#define Fx1      460
#define F_2      870
#define Fx2      920
#define G_1      490
#define G_2      980
#define P_2        0
#define P_4        0
#define P_8        0
#define T_1     2200	//2400
#define T_2    T_1/2	//1200
#define T_4    T_1/4	//600
#define T_8    T_1/8	//300
#define TT2  T_4*4/3 	//800
#define TT4  T_4*2/3	//400
#define TT8    T_4/3	//200

void Muzika() {
	while (true) {
		while (mute)Sleep(50);       Beep(P_4, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(Dx1, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(G_1, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(A_1, TT4);
		while (mute)Sleep(50);       Beep(G_1, TT8 + TT4);                //
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(Dx1, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(G_1, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(G_1, TT4);
		while (mute)Sleep(50);       Beep(A_1, TT8);                      //
		while (mute)Sleep(50);       Beep(D_2, TT4);
		while (mute)Sleep(50);       Beep(P_8, TT8);
		while (mute)Sleep(50);       Beep(Cx2, TT4);
		while (mute)Sleep(50);       Beep(P_8, TT8);
		while (mute)Sleep(50);       Beep(C_2, TT4);
		while (mute)Sleep(50);       Beep(B_1, TT4 + TT8);
		while (mute)Sleep(50);       Beep(A_1, TT8 + TT4);                //
		while (mute)Sleep(50);       Beep(F_1, TT4);
		while (mute)Sleep(50);       Beep(Fx1, TT8);
		while (mute)Sleep(50);       Beep(D_1, T_4 + TT8 + 100);          //
		while (mute)Sleep(50);       Beep(P_4, T_8);
		while (mute)Sleep(50);       Beep(F_1, TT8);
		while (mute)Sleep(50);       Beep(E_1, TT4);
		while (mute)Sleep(50);       Beep(F_1, TT8);
		while (mute)Sleep(50);       Beep(Ax1, TT4);
		while (mute)Sleep(50);       Beep(B_1, TT8);
		while (mute)Sleep(50);       Beep(G_1, TT4);
		while (mute)Sleep(50);       Beep(F_1, TT8);                      //
		while (mute)Sleep(50);       Beep(Cx2, TT4);
		while (mute)Sleep(50);       Beep(D_2, TT8);
		while (mute)Sleep(50);       Beep(B_2, TT4);
		while (mute)Sleep(50);       Beep(G_1, TT8);
		while (mute)Sleep(50);       Beep(G_2, TT4);
		while (mute)Sleep(50);       Beep(Fx2, TT8);
		while (mute)Sleep(50);       Beep(F_2, TT4);
		while (mute)Sleep(50);       Beep(D_2, TT8);                      //
		while (mute)Sleep(50);       Beep(E_2, TT4);
		while (mute)Sleep(50);       Beep(G_2, TT8);
		while (mute)Sleep(50);       Beep(Cx3, TT4);
		while (mute)Sleep(50);       Beep(G_2, TT8);
		while (mute)Sleep(50);       Beep(C_3, TT4);
		while (mute)Sleep(50);       Beep(Fx2, TT8);
		while (mute)Sleep(50);       Beep(B_2, TT4);
		while (mute)Sleep(50);       Beep(F_2, TT8);                      //
		while (mute)Sleep(50);       Beep(G_2, TT4);
		while (mute)Sleep(50);       Beep(E_2, TT8);
		while (mute)Sleep(50);       Beep(Cx2, TT4);
		while (mute)Sleep(50);       Beep(G_2, TT8);
		while (mute)Sleep(50);       Beep(Fx2, TT4);
		while (mute)Sleep(50);       Beep(C_2, TT8);
		while (mute)Sleep(50);       Beep(B_1, TT4);
		while (mute)Sleep(50);       Beep(F_2, TT8);                      //
		while (mute)Sleep(50);       Beep(P_4, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(Dx1, TT4);
		while (mute)Sleep(50);       Beep(E_1, TT8);
		while (mute)Sleep(50);       Beep(Fx1, TT4);
		while (mute)Sleep(50);       Beep(G_1, TT8);
		while (mute)Sleep(50);       Beep(C_2, TT4);
		while (mute)Sleep(50);       Beep(E_2, TT8);                      //
		while (mute)Sleep(50);       Beep(Fx2, TT4);
		while (mute)Sleep(50);       Beep(G_2, TT8);
		while (mute)Sleep(50);       Beep(Dx2, TT4);
		while (mute)Sleep(50);       Beep(E_2, TT8);
		while (mute)Sleep(50);       Beep(G_2, TT4);
		while (mute)Sleep(50);       Beep(A_2, TT8);
		while (mute)Sleep(50);       Beep(C_3, TT4);
		while (mute)Sleep(50);       Beep(D_3, TT8);                      //
		while (mute)Sleep(50);       Beep(E_3, TT4);
		while (mute)Sleep(50);       Beep(P_8, TT8);
		while (mute)Sleep(50);       Beep(E_3, TT4);
		while (mute)Sleep(50);       Beep(Dx3, TT8);
		while (mute)Sleep(50);       Beep(E_3, TT4);
		while (mute)Sleep(50);       Beep(D_3, TT8);
		while (mute)Sleep(50);       Beep(Dx3, T_4);                      //
		while (mute)Sleep(50);       Beep(C_3, TT4);
		while (mute)Sleep(50);       Beep(Fx2, TT8);
		while (mute)Sleep(50);       Beep(G_2, TT4);
		while (mute)Sleep(50);       Beep(Cx2, T_4 + TT8 + 50);           //
		while (mute)Sleep(50);       Beep(C_3, T_8);
		mute = true;
		interupt = true;
	}
}

BOOL WINAPI EXIT(DWORD CEvent) {
	if (CEvent == CTRL_CLOSE_EVENT && IsWindows10OrGreater())
		Slika(2);

	return TRUE;
}