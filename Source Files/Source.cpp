#include <iostream>
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#include <random>
#include <chrono>
using namespace std;
using namespace chrono;

HDC console = GetDC(GetConsoleWindow());
HDC hdc = CreateCompatibleDC(NULL);
HBITMAP bmap = (HBITMAP)LoadImage(NULL, _T("ConsoleMinesweeperSpriteSheet.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
enum state {WIN, LOSS, NONE};

void ShowConsoleCursor(bool flag);
bool KeyIsDown(char key, bool pressed, bool held);
double Wait(double WaitTime);
double GetTimeSince(double StartTime);
double GetTime();

class MineSweeper {
private:
	bool FirstInput = false;
	state GameState = NONE;
	int** MapTop;
	int** MapBottom;
	int NumberOfMines;
	int Dimensions;
	int InputX = 0;
	int InputY = 0;
	int Flags = 0;
	double SecondTS = 0;
	int Time = 0;
	int indent;
	const int GSS = 16;
	random_device generator;

	const int SpritePositions[41][2]{
		0,0, // Unchecked Tile / 1
		16,0, // Smile Tile / 2
		0,16, // Regular 0 / 3
		16,16, // Regular 1 / 4
		32,16, // Regular 2 / 5
		48,16, // Regular 3 / 6
		64,16, // Regular 4 / 7
		80,16, // Regular 5 / 8
		96,16, // Regular 6 / 9
		112,16, // Regular 7 / 10
		128,16, // Regular 8 / 11
		144,16, // Regular 9 / 12

		0,64, // Frown Tile / 13
		16,64, // Cool Tile / 14
		128,0, // Chosen Mine Tile / 15
		48,32, // R / 16 ````
		64,32, // D / 17 ````
		80,32, // S / 18 ````
		96,32, // : / 19 ````

		32,0, // Blank Tile / 20
		48,0, // Flag Tile / 21
		64,0, // Selection Unchecked Tile / 22
		80,0, // Selection Blank Tile / 23
		96,0, // Selection Flag Tile / 24
		112,0, // Mine Tile / 25

		0,32, // Tile 1 / 26
		16,32, // Tile 2 / 27
		32,32, // Tile 3 / 28
		48,32, // Tile 4 / 29
		64,32, // Tile 5 / 30
		80,32, // Tile 6 / 31
		96,32, // Tile 7 / 32 
		112,32, // Tile 8 / 33 

		0,48, // Selected Tile 1 / 34
		16,48, // Selected Tile 2 / 35
		32,48, // Selected Tile 3 / 36
		48,48, // Selected Tile 4 / 37
		64,48, // Selected Tile 5 / 38
		80,48, // Selected Tile 6 / 39
		96,48, // Selected Tile 7 / 40 
		112,48, // Selected Tile 8 / 41 
	};
public:
	bool EXIT_MS = false;
	void SetConsoleSize();
	void DrawBoard() {
		int FlagsLocated = 0;
		int UnopenedTiles = 0;
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapTop[y][x] == 0)
					if (y == InputY && x == InputX)
						DrawSprite(GetSelAlt(MapBottom[y][x]), indent + x * GSS, 32 + GSS + y * GSS);
					else
						DrawSprite(MapBottom[y][x], indent + x * GSS, 32 + GSS + y * GSS);
				else
					if (y == InputY && x == InputX)
						DrawSprite(GetSelAlt(MapTop[y][x]) , indent + x * GSS, 32 + GSS + y * GSS);
					else
						DrawSprite(MapTop[y][x], indent + x * GSS, 32 + GSS + y * GSS);
				switch (GameState) {
				case NONE:
					DrawSprite(2, indent + (Dimensions * GSS / 2) - (GSS / 2), GSS);
					break;
				case WIN:
					DrawSprite(14, indent + (Dimensions * GSS / 2) - (GSS / 2), GSS);
					break;
				case LOSS:
					DrawSprite(13, indent + (Dimensions * GSS / 2) - (GSS / 2), GSS);
					break;
				}
				if (MapTop[y][x] == 21 || MapTop[y][x] == 24)
					FlagsLocated++;
				if (MapTop[y][x] == 1 || MapTop[y][x] == 21 || MapTop[y][x] == 23 || MapTop[y][x] == 24)
					UnopenedTiles++;
			}
		}
		if (UnopenedTiles == NumberOfMines)
			GameState = WIN;
		Flags = FlagsLocated;
		if (Flags > NumberOfMines)
			Flags = NumberOfMines;
		DrawNumber(NumberOfMines - Flags, -4);
		DrawNumber(Time, 2);
	}
	void DrawNumber(int num, int xPos) {
		int Digits[3] = { 0,0,0 };
		if (num < 1000) {
			Digits[0] = num / 100;
			Digits[1] = (num - Digits[0]) / 10;
			Digits[2] = num % 10;
		}
		else
			int Digits[3] = { 1,1,1 };
		DrawSprite(Digits[0] + 3, indent + (Dimensions * GSS / 2) - (GSS / 2) + (xPos * GSS), GSS);
		DrawSprite(Digits[1] + 3, indent + (Dimensions * GSS / 2) - (GSS / 2) + ((xPos + 1) * GSS), GSS);
		DrawSprite(Digits[2] + 3, indent + (Dimensions * GSS / 2) - (GSS / 2) + ((xPos + 2) * GSS), GSS);
	}
	void InputBoard() {
		if (GameState == NONE) {
			if (KeyIsDown('A', true, false))
				InputX--;
			if (KeyIsDown('D', true, false))
				InputX++;
			if (KeyIsDown('W', true, false))
				InputY--;
			if (KeyIsDown('S', true, false))
				InputY++;
			if (KeyIsDown(13, true, false)) {
				FirstInput = true;
				MapTop[InputY][InputX] = 0;
				if (MapBottom[InputY][InputX] == 20) {
					ClearAdjacentTiles(InputX, InputY);
					ClearAdjacentNumber();
				}
			}
			if (KeyIsDown('F', true, false) && MapTop[InputY][InputX] != 0 ) {
				FirstInput = true;
				if (MapTop[InputY][InputX] != 21 && MapTop[InputY][InputX] != 0 && Flags != NumberOfMines)
					MapTop[InputY][InputX] = 21;
				else
					MapTop[InputY][InputX] = 1;
			}
		}
	}
	void LogicBoard() {
		// Set Bounds
		if (GameState == NONE) {
			if (InputX < 0)
				InputX = 0;
			else if (InputX > Dimensions - 1)
				InputX = Dimensions - 1;
			if (InputY < 0)
				InputY = 0;
			else if (InputY > Dimensions - 1)
				InputY = Dimensions - 1;
			if (CheckForLoss() && GameState != WIN)
				GameState = LOSS;
		}
		if (GameState == LOSS || GameState == WIN) {
			ClearTopMap();
			if (GameState == LOSS)
				MapBottom[InputY][InputX] = 15;
			DrawBoard();
			EXIT_MS = true;
		}
		if (GetTimeSince(SecondTS) > 1.0 && FirstInput && GameState == NONE) {
			Time++;
			SecondTS = GetTime();
		}
		if (Time >= 999)
			Time = 999;
	}
	MineSweeper() { 
		NumberOfMines = 10; Dimensions = 12; MapTop = new int* [Dimensions];  MapBottom = new int* [Dimensions];
		for (int i = 0; i < Dimensions; i++) {
			MapTop[i] = new int[Dimensions];
			MapBottom[i] = new int[Dimensions];
		}
		SetConsoleSize();
	};
	MineSweeper(int n, int d) {
		NumberOfMines = n; Dimensions = d; MapTop = new int* [Dimensions];  MapBottom = new int* [Dimensions];
		indent = (15 - (Dimensions / 2)) * 16;
		for (int i = 0; i < Dimensions; i++) {
			MapTop[i] = new int[Dimensions];
			MapBottom[i] = new int[Dimensions];
		}
		SetConsoleSize();
	};
	~MineSweeper() {
		for (int i = 0; i < Dimensions; i++) {
			delete[] MapTop[i];
			delete[] MapBottom[i];
		}
		delete[] MapTop; delete[] MapBottom;
	}
	int CheckForLoss() {
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapTop[y][x] == 0)
					if (MapBottom[y][x] == 25)
						return 1;
			}
		}
		return 0;
	}
	void ClearTopMap() {
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				MapTop[y][x] = 0;
			}
		}
	}
	void ClearAdjacentNumber() {
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapTop[y][x] != 0 && MapBottom[y][x] != 20 && MapBottom[y][x] != 25) {
					if (y - 1 >= 0 && MapTop[y - 1][x] == 0 && MapBottom[y - 1][x] == 20)
						MapTop[y][x] = 0;
					if (y + 1 < Dimensions && MapTop[y + 1][x] == 0 && MapBottom[y + 1][x] == 20)
						MapTop[y][x] = 0;
					if (x - 1 >= 0 && MapTop[y][x - 1] == 0 && MapBottom[y][x - 1] == 20)
						MapTop[y][x] = 0;
					if (x + 1 < Dimensions && MapTop[y][x + 1] == 0 && MapBottom[y][x + 1] == 20)
						MapTop[y][x] = 0;
					if (x + 1 < Dimensions && y - 1 >= 0 && MapTop[y - 1][x + 1] == 0 && MapBottom[y - 1][x + 1] == 20)
						MapTop[y][x] = 0;
					if (x + 1 < Dimensions && y + 1 < Dimensions && MapTop[y + 1][x + 1] == 0 && MapBottom[y + 1][x + 1] == 20)
						MapTop[y][x] = 0;
					if (x - 1 >= 0 && y - 1 >= 0 && MapTop[y - 1][x - 1] == 0 && MapBottom[y - 1][x - 1] == 20)
						MapTop[y][x] = 0;
					if (x - 1 >= 0 && y + 1 < Dimensions && MapTop[y + 1][x - 1] == 0 && MapBottom[y + 1][x - 1] == 20)
						MapTop[y][x] = 0;
				}
			}
		}
	}
	void ClearAdjacentTiles(int x, int y) {
		MapTop[y][x] = 0;
		if (y - 1 >= 0 && MapBottom[y - 1][x] == 20 && MapTop[y - 1][x] != 0) {
			ClearAdjacentTiles(x, y - 1);
		}
		if (y + 1 < Dimensions && MapBottom[y + 1][x] == 20 && MapTop[y + 1][x] != 0) {
			ClearAdjacentTiles(x, y + 1);
		}
		if (x - 1 >= 0 && MapBottom[y][x - 1] == 20 && MapTop[y][x - 1] != 0) {
			ClearAdjacentTiles(x - 1, y);
		}
		if (x + 1 < Dimensions && MapBottom[y][x + 1] == 20 && MapTop[y][x + 1] != 0) {
			ClearAdjacentTiles(x + 1, y);
		}
	}
	void InitializeBoard() {
		uniform_int_distribution<int> distribution(1, NumberOfMines * 2);
		SecondTS = GetTime();
		while (MineSweep() != NumberOfMines) {
			for (int y = 0; y < Dimensions; y++) {
				for (int x = 0; x < Dimensions; x++) {
					// Set up front visuals
					MapTop[y][x] = 1;
					// Randomize back
					if (distribution(generator) == 1 && MineSweep() != NumberOfMines) {
						MapBottom[y][x] = 25;
					}
					else if (MapBottom[y][x] < 25 || MapBottom[y][x] > 33)
						MapBottom[y][x] = 20;
				}
			}
		}
		SetupNumTiles();
	}
	int MineSweep() {
		int Mines = 0;
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapBottom[y][x] == 25)
					Mines++;
			}
		}
		return Mines;
	}
	void DrawSprite(int ArrPos, int x, int y) {
		SelectObject(hdc, bmap);
		BitBlt(console, x, y, GSS, GSS, hdc, SpritePositions[ArrPos - 1][0], SpritePositions[ArrPos - 1][1], SRCCOPY);
		DeleteObject(bmap);
	}
	void SetupNumTiles() {
		int MinesNearby = 0;
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (CheckTilePosition(x, y - 1) == 25)
					MinesNearby++;
				if (CheckTilePosition(x + 1, y - 1) == 25)
					MinesNearby++;
				if (CheckTilePosition(x + 1, y) == 25)
					MinesNearby++;
				if (CheckTilePosition(x + 1, y + 1) == 25)
					MinesNearby++;
				if (CheckTilePosition(x, y + 1) == 25)
					MinesNearby++;
				if (CheckTilePosition(x - 1, y + 1) == 25)
					MinesNearby++;
				if (CheckTilePosition(x - 1, y) == 25)
					MinesNearby++;
				if (CheckTilePosition(x - 1, y - 1) == 25)
					MinesNearby++;
				if (MinesNearby > 0 && MapBottom[y][x] != 25)
					MapBottom[y][x] = 25 + MinesNearby;
				MinesNearby = 0;
			}
		}
	}
	int CheckTilePosition(int x, int y) {
		if (x >= 0 && x < Dimensions && y >= 0 && y < Dimensions) {
			return MapBottom[y][x];
		}
		else
			return 0;
	}
	void ClearInputBuffer() {
		if (KeyIsDown(13, true, false))
			NULL;
		if (KeyIsDown('F', true, false))
			NULL;
		if (KeyIsDown('W', true, false))
			NULL;
		if (KeyIsDown('A', true, false))
			NULL;
		if (KeyIsDown('S', true, false))
			NULL;
		if (KeyIsDown('D', true, false))
			NULL;
	}
	int GetSelAlt(int spriteID) {
		switch (spriteID) {
		case 1:
			return 22;
		case 20:
			return 23;
		case 21:
			return 24;
		case 25:
			return 25;
		case 26:
			return 34;
		case 27:
			return 35;
		case 28:
			return 36;
		case 29:
			return 37;
		case 30:
			return 38;
		case 31:
			return 39;
		case 32:
			return 40;
		case 33:
			return 41;
		case 15:
			return 15;
		default:
			return 1;
		}
	}
};
////////////////////////////////////////////////////////////
int main() {
	system("Title Console Minesweeper");
	bool EXIT_PROGRAM = 0;
	do {
		MineSweeper* Board = new MineSweeper(8, 10);
		Board->InitializeBoard();
		Board->ClearInputBuffer();
		do {
			Board->DrawBoard();
			Board->InputBoard();
			Board->LogicBoard();
			ShowConsoleCursor(false);
			while (GetConsoleWindow() != GetForegroundWindow()) {}
		} while (!Board->EXIT_MS);
		KeyIsDown(NULL, true, true);
		Sleep(2000);
		delete Board;
	} while (!EXIT_PROGRAM);
	return 0;
}
////////////////////////////////////////////////////////////
void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

bool KeyIsDown(char key, bool pressed, bool held) {
	int keyState = GetAsyncKeyState(static_cast<int>(key));
	return (pressed && (keyState & 1)) || (held && (keyState & 0xA000));
}

void MineSweeper::SetConsoleSize() {
	HWND console = GetConsoleWindow();
	HMONITOR monitor = MonitorFromWindow(console, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(monitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// Physical Width/Height
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;

	double horzScale = ((double)cxPhysical / (double)cxLogical);
	double vertScale = ((double)cyPhysical / (double)cyLogical);

	SetWindowPos(console, HWND_TOP, 0, 0, double(480) / horzScale + 4, double(480) / vertScale, SWP_NOMOVE); // Resize without moving where the console window was placed

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	COORD new_size = {
		info.srWindow.Right - info.srWindow.Left + 1,
		info.srWindow.Bottom - info.srWindow.Top + 1
	};
	SetConsoleScreenBufferSize(handle, new_size);
}

double GetTime() {
	return time_point_cast<nanoseconds>(high_resolution_clock::now()).time_since_epoch().count() / 1e9;
}

double GetTimeSince(double startTime) {
	return time_point_cast<nanoseconds>(high_resolution_clock::now()).time_since_epoch().count() / 1e9 - startTime;
}

double Wait(double waitTime) {
	double startTime = GetTime();

	while (waitTime > GetTimeSince(startTime)) {}

	return GetTimeSince(startTime + waitTime);
}
