#include <iostream>
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#include <random>
#include <chrono>
using namespace std;
using namespace chrono;
bool EXIT_PROGRAM = false;

HDC console = GetDC(GetConsoleWindow());
HDC hdc = CreateCompatibleDC(NULL);
HBITMAP bmap = (HBITMAP)LoadImage(NULL, _T("ConsoleMinesweeperSpriteSheet.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
enum state {WIN, LOSS, UNDC};
enum input {UP, DOWN, START, NONE};

double FPS = 1.0 / 60.0;
double timer = 0, dt = 0;

void ShowConsoleCursor(bool flag);
void SetConsoleSize();
bool KeyIsDown(char key, bool pressed, bool held);
double Wait(double WaitTime);
double GetTimeSince(double StartTime);
double GetTime();

bool EXIT_MENU = false;
int SelDimensions = 0;
int SelMines = 0;

class Menu {
private:
	input PlayerInput = NONE;
	const int GSS = 16;
	int os;
	const int SpritePositions[19][2]{
		0,80, // A / 1
		16,80, // B / 2
		32,80, // D / 3
		48,80, // E / 4
		64,80, // G / 5
		80,80, // I / 6
		96,80, // M / 7
		128,80, // P / 8
		144,80, // R / 9
		0,96, // T / 10
		16,96, // X / 11
		64,96, // S / 12
		80,96, // W / 13
		32,96, // Left Bracket / 14
		48,96, // Right BRacket / 15
		112,80, // N / 16

		0,64, // Frown Tile / 17
		16,0, // Smile Tile / 18

		96,96, // Empty Sprite / 19
	};
public:
	Menu() {
		os = 1;
	}
	void DrawMenu() {
		DrawSprite(18, (20 / 2) - 2, 7);
		DrawSprite(7, (20 / 2) - 1 , 7);
		DrawSprite(6, (20 / 2), 7);
		DrawSprite(16, (20 / 2) + 1, 7);
		DrawSprite(4, (20 / 2) + 2, 7);

		DrawSprite(12, (20 / 2) + 5, 7);
		DrawSprite(13, (20 / 2) + 6, 7);
		DrawSprite(4, (20 / 2) + 7, 7);
		DrawSprite(4, (20 / 2) + 8, 7);
		DrawSprite(8, (20 / 2) + 9, 7);
		DrawSprite(4, (20 / 2) + 10, 7);
		DrawSprite(9, (20 / 2) + 11, 7);
		DrawSprite(17, (20 / 2) + 12, 7);

		if (os == 1) {
			DrawSprite(14, (20 / 2), 11);
			DrawSprite(15, (20 / 2) + 9, 11);
		}
		else {
			DrawSprite(19, (20 / 2), 11);
			DrawSprite(19, (20 / 2) + 9, 11);
		}
		DrawSprite(2, (20 / 2) + 1, 11);
		DrawSprite(4, (20 / 2) + 2, 11);
		DrawSprite(5, (20 / 2) + 3, 11);
		DrawSprite(6, (20 / 2) + 4, 11);
		DrawSprite(16, (20 / 2) + 5, 11);
		DrawSprite(16, (20 / 2) + 6, 11);
		DrawSprite(4, (20 / 2) + 7, 11);
		DrawSprite(9, (20 / 2) + 8, 11);

		if (os == 2) {
			DrawSprite(14, (20 / 2) - 2, 13);
			DrawSprite(15, (20 / 2) + 11, 13);
		}
		else {
			DrawSprite(19, (20 / 2) - 2, 13);
			DrawSprite(19, (20 / 2) + 11, 13);
		}
		DrawSprite(6, (20 / 2) - 1, 13);
		DrawSprite(16, (20 / 2), 13);
		DrawSprite(10, (20 / 2) + 1, 13);
		DrawSprite(4, (20 / 2) + 2, 13);
		DrawSprite(9, (20 / 2) + 3, 13);
		DrawSprite(7, (20 / 2) + 4, 13);
		DrawSprite(4, (20 / 2) + 5, 13);
		DrawSprite(3, (20 / 2) + 6, 13);
		DrawSprite(6, (20 / 2) + 7, 13);
		DrawSprite(1, (20 / 2) + 8, 13);
		DrawSprite(10, (20 / 2) + 9, 13);
		DrawSprite(4, (20 / 2) + 10, 13);

		if (os == 3) {
			DrawSprite(14, (20 / 2) + 1, 15);
			DrawSprite(15, (20 / 2) + 8, 15);
		}
		else {
			DrawSprite(19, (20 / 2) + 1, 15);
			DrawSprite(19, (20 / 2) + 8, 15);
		}
		DrawSprite(4, (20 / 2) + 2, 15);
		DrawSprite(11, (20 / 2) + 3, 15);
		DrawSprite(8, (20 / 2) + 4, 15);
		DrawSprite(4, (20 / 2) + 5, 15);
		DrawSprite(9, (20 / 2) + 6, 15);
		DrawSprite(10, (20 / 2) + 7, 15);

		if (os == 4) {
			DrawSprite(14, (20 / 2) + 2, 17);
			DrawSprite(15, (20 / 2) + 7, 17);
		}
		else {
			DrawSprite(19, (20 / 2) + 2, 17);
			DrawSprite(19, (20 / 2) + 7, 17);
		}
		DrawSprite(4, (20 / 2) + 3, 17);
		DrawSprite(11, (20 / 2) + 4, 17);
		DrawSprite(6, (20 / 2) + 5, 17);
		DrawSprite(10, (20 / 2) + 6, 17);

	}
	void InputMenu() {
		PlayerInput = NONE;
		if (KeyIsDown('W', true, false) || KeyIsDown(38, true, false))
			PlayerInput = UP;
		if (KeyIsDown('S', true, false) || KeyIsDown(40, true, false))
			PlayerInput = DOWN;
		if (KeyIsDown(13, true, false))
			PlayerInput = START;
	}
	void LogicMenu() {
		switch (PlayerInput) {
		case UP:
			os--;
			break;
		case DOWN:
			os++;
			break;
		case START:
			switch (os) {
			case 1:
				SelDimensions = 10;
				SelMines = 8;
				EXIT_MENU = true;
				break;
			case 2:
				SelDimensions = 15;
				SelMines = 40;
				EXIT_MENU = true;
				break;
			case 3:
				SelDimensions = 20;
				SelMines = 66;
				EXIT_MENU = true;
				break;
			case 4:
				SelDimensions = 20;
				SelMines = 1;
				EXIT_PROGRAM = true;
				EXIT_MENU = true;
				break;
			}
			break;
		}
		if (os > 4)
			os = 4;
		if (os < 1)
			os = 1;
	}
	void DrawSprite(int ArrPos, int x, int y) {
		SelectObject(hdc, bmap);
		BitBlt(console, x * GSS, y * GSS, GSS, GSS, hdc, SpritePositions[ArrPos - 1][0], SpritePositions[ArrPos - 1][1], SRCCOPY);
		DeleteObject(bmap);
	}
};

class MineSweeper {
private:
	bool FirstInput = false;
	state GameState = UNDC;
	int** MapTop;
	int** MapBottom;
	int NumberOfMines;
	int Dimensions;
	int InputX;
	int InputY;
	int Flags = 0;
	double SecondTS = 0;
	int Time = 0;
	int indent;
	const int GSS = 16;
	random_device generator;

	const int SpritePositions[43][2]{
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
		48,64, // Vertical Border Tile / 16
		32,64, // Horizontal Border Tile / 17
		64,64, // Top Left Border Tile / 18
		80,64, // Bottom Right Border Tile / 19

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

		96,64, // Top Right Border Tile / 42
		112,64, // Bottom Left Border Tile / 43
	};
public:
	bool EXIT_MS = false;
	bool ForceReset = false;
	void DrawBoard() {
		int FlagsLocated = 0;
		int UnopenedTiles = 0;
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapTop[y][x] == 0)
					if (y == InputY && x == InputX)
						DrawSprite(GetSelAlt(MapBottom[y][x]), indent + x * GSS, 48 + GSS + y * GSS);
					else
						DrawSprite(MapBottom[y][x], indent + x * GSS, 48 + GSS + y * GSS);
				else
					if (y == InputY && x == InputX)
						DrawSprite(GetSelAlt(MapTop[y][x]) , indent + x * GSS, 48 + GSS + y * GSS);
					else
						DrawSprite(MapTop[y][x], indent + x * GSS, 48 + GSS + y * GSS);
				switch (GameState) {
				case UNDC:
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
		DrawBorder();
		if (UnopenedTiles == NumberOfMines)
			GameState = WIN;
		Flags = FlagsLocated;
		if (Flags > NumberOfMines)
			Flags = NumberOfMines;
		DrawNumber(NumberOfMines - Flags, -4);
		DrawNumber(Time, 2);
	}
	void DrawBorder() {
		for (int y = 0; y < Dimensions + 2; y++) {
			for (int x = 0; x < Dimensions + 2; x++) {
				if ((x == 0 || x == Dimensions + 1) && (y > 0 && y < Dimensions + 1))
					DrawSprite(16, indent - GSS + x * GSS, 32 + GSS + y * GSS);
				if ((y == 0 || y == Dimensions + 1) && (x > 0 && x < Dimensions + 1))
					DrawSprite(17, indent - GSS + x * GSS, 32 + GSS + y * GSS);
				if (x == 0 && y == 0)
					DrawSprite(18, indent - GSS + x * GSS, 32 + GSS + y * GSS);
				if (x == Dimensions + 1 && y == Dimensions + 1)
					DrawSprite(19, indent - GSS + x * GSS, 32 + GSS + y * GSS);
				if (x == Dimensions + 1 && y == 0)
					DrawSprite(42, indent - GSS + x * GSS, 32 + GSS + y * GSS);
				if (x == 0 && y == Dimensions + 1)
					DrawSprite(43, indent - GSS + x * GSS, 32 + GSS + y * GSS);
			}
		}
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
		if (GameState == UNDC) {
			if (KeyIsDown('A', true, false) || KeyIsDown(37, true, false))
				InputX--;
			if (KeyIsDown('D', true, false) || KeyIsDown(39, true, false))
				InputX++;
			if (KeyIsDown('W', true, false) || KeyIsDown(38, true, false))
				InputY--;
			if (KeyIsDown('S', true, false) || KeyIsDown(40, true, false))
				InputY++;
			if (KeyIsDown(27, true, false)) {
				EXIT_MENU = false;
				EXIT_MS = true;
				ForceReset = true;
			}
			if (KeyIsDown('R', true, false)) {
				EXIT_MS = true;
				ForceReset = true;
			}
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
		if (GameState == UNDC) {
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
			RevealMines();
			if (GameState == LOSS)
				MapBottom[InputY][InputX] = 15;
			DrawBoard();
			EXIT_MS = true;
		}
		if (GetTimeSince(SecondTS) > 1.0 && FirstInput && GameState == UNDC) {
			Time++;
			SecondTS = GetTime();
		}
		if (Time >= 999)
			Time = 999;
	}
	MineSweeper() { 
		NumberOfMines = 10; Dimensions = 12; MapTop = new int* [Dimensions];  MapBottom = new int* [Dimensions];
		indent = (15 - (Dimensions / 2)) * 16;
		for (int i = 0; i < Dimensions; i++) {
			MapTop[i] = new int[Dimensions];
			MapBottom[i] = new int[Dimensions];
		}
		InputX = Dimensions / 2;
		InputY = Dimensions / 2;
	};
	MineSweeper(int n, int d) {
		NumberOfMines = n; Dimensions = d; MapTop = new int* [Dimensions];  MapBottom = new int* [Dimensions];
		indent = (15 - (Dimensions / 2)) * 16;
		for (int i = 0; i < Dimensions; i++) {
			MapTop[i] = new int[Dimensions];
			MapBottom[i] = new int[Dimensions];
		}
		InputX = Dimensions / 2;
		InputY = Dimensions / 2;
		if (NumberOfMines < 2)
			ForceReset = true;
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
	void RevealMines() {
		for (int y = 0; y < Dimensions; y++) {
			for (int x = 0; x < Dimensions; x++) {
				if (MapBottom[y][x] == 25) {
					MapTop[y][x] = 0;
				}
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
		if (KeyIsDown(13, true, true))
			NULL;
		if (KeyIsDown('F', true, true))
			NULL;
		if (KeyIsDown('W', true, true) || KeyIsDown(38, true, false))
			NULL;
		if (KeyIsDown('A', true, true) || KeyIsDown(40, true, false))
			NULL;
		if (KeyIsDown('S', true, true) || KeyIsDown(37, true, false))
			NULL;
		if (KeyIsDown('D', true, true) || KeyIsDown(39, true, false))
			NULL;
		if (KeyIsDown('R', true, true))
			NULL;
		if (KeyIsDown(27, true, true))
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
	system("color 70");
	SetConsoleSize();
	ShowConsoleCursor(false);
	do {
		Menu* Main = new Menu;
		do {
			Main->DrawMenu();
			Main->InputMenu();
			Main->LogicMenu();
			while (GetConsoleWindow() != GetForegroundWindow()) {}
		} while (!EXIT_MENU && (timer += (dt = FPS + Wait(FPS))));
		MineSweeper* Board = new MineSweeper(SelMines, SelDimensions);
		delete Main;
		system("CLS");
		Board->InitializeBoard();
		Board->ClearInputBuffer();
		while (!Board->EXIT_MS && !EXIT_PROGRAM && (timer += (dt = FPS + Wait(FPS)))) {
			Board->DrawBoard();
			Board->InputBoard();
			Board->LogicBoard();
			while (GetConsoleWindow() != GetForegroundWindow()) {}
		}
		if (!Board->ForceReset)
			Wait(2.5);
		else if (!Board->EXIT_MS)
			EXIT_MENU = true;
		delete Board;
		system("CLS");
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
void SetConsoleSize() {
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

	SetWindowPos(console, HWND_TOP, 0, 0, double(490.0) / horzScale + 4, double(480.0) / vertScale, SWP_NOMOVE); // Resize without moving where the console window was placed

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
