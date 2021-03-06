﻿#include "pch.h"     // удалите это если у вас не Visual Studio
#include <iostream>
#include <fstream>
#include <iomanip>   // для setprecision и fixed
#include <windows.h> // для задержки Sleep(40), в миллисекундах
#include <ctime>     // для генератора случайных чисел
#include <conio.h>
#include <stdio.h>
using namespace std;


const char simv_wall = char(219); // '▓' стена
const char simv_floor = char(32); // ' ' пол
const char simv_exit = char(88);  // 'X' выход
const char simv_kust = char(176); // '░' куст
const char simv_rock = char(249); // '.' камушек
const char simv_proxod = char(249); // '>' проход
const char simv_movwall = char(219); // '>' проход
const int skinamount = 7;
const int pageamount = 10;
const char hero_skins[skinamount] = { 'A', 'B', 'C','I','@','?','H' };   // 'A' 'B' 'C' герой
const char simv_check = char(46);  // '.' проверка
bool dangers[6] = { false,false,false,false,false,false };
struct Hero
{
public:
	int x, y, hp;
	int skinnumber;

	char name, memory;
	Hero()
	{
		x = 2;
		y = 2;

		skinnumber = 0;
		name = hero_skins[skinnumber];
		memory = simv_floor;
	}
};
void changefontsize(int a)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(fontInfo);
	GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);
	wcscpy_s(fontInfo.FaceName, L"Courier New");
	fontInfo.dwFontSize.Y = a;
	SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);
}
void changebuffersize(int a, int b)
{
	HANDLE out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD crd;
	crd.X = a;
	crd.Y = b;
	SMALL_RECT src = { 0, 0, crd.X - 1, crd.Y - 1 };
	SetConsoleWindowInfo(out_handle, true, &src);
	SetConsoleScreenBufferSize(out_handle, crd);
}
class Level
{
public:
	int width, height;
	bool areyoubig;
	int exitx, exity;
	Hero hero;
	char **pole;
	int lvlnumber;

	Level(Hero h, int number, int steps, double score, double bestscore)
	{
		areyoubig = false;
		srand(int(time(0)));
		hero = h;
		lvlnumber = number;
		height = 10 + number / 3;
		width = 10 + number / 3;
		if (dangers[1] && rand() % 100 + lvlnumber > 85) // шанс появления огромного уровня 
		{
			areyoubig = true;
			height *= 2;
			width *= 2;
		}
		Create();

		Show(steps, score, bestscore, areyoubig, 1);
	}
	void Create()
	{
		srand((int)time(0));
		pole = new char*[height];
		for (int z = 0; z < width; z++)pole[z] = new char[width];
		hero.memory = simv_floor;
		for (int i = 0; i < width; i++) // placing walls
			for (int j = 0; j < height; j++)
			{
				pole[i][j] = simv_floor;
				if (rand() % 3 == 1) pole[i][j] = simv_wall;
			}

		for (int i = 1; i < width - 1; i++) // closing 1 cell holes
			for (int j = 1; j < height - 1; j++)
				if (pole[i + 1][j] == simv_wall && pole[i - 1][j] == simv_wall && pole[i][j + 1] == simv_wall && pole[i - 1][j] == simv_wall)pole[i][j] = simv_wall;
		
		if (areyoubig) // adding additional walls
		{
			for (int i = 0; i < width; i++)
			{
				if (pole[i][height / 2] == simv_wall || pole[i][height / 2 - 1] == simv_wall)
				{
					pole[i][height / 2] = simv_wall; pole[i][height / 2 - 1] = simv_wall;
				}
				if (pole[i][0] == simv_wall || pole[i][height - 1] == simv_wall)
				{
					pole[i][0] = simv_wall; pole[i][height - 1] = simv_wall;
				}
			}
			for (int j = 0; j < height; j++)
			{
				if (pole[width / 2][j] == simv_wall || pole[width / 2 - 1][j] == simv_wall)
				{
					pole[width / 2][j] = simv_wall; pole[width / 2 - 1][j] = simv_wall;
				}
				if (pole[0][j] == simv_wall || pole[width - 1][j] == simv_wall)
				{
					pole[0][j] = simv_wall; pole[width - 1][j] = simv_wall;
				}
			}
		}

		pole[hero.x][hero.y] = hero.name;

		bool proverka = false;
		while (!proverka)
		{
			char p = simv_floor;
			srand((int)time(0));
			while (p != simv_wall)
			{
				exitx = rand() % width;
				exity = rand() % height;
				p = pole[exitx][exity];
			}
			pole[exitx][exity] = simv_exit;
			proverka = PlaceFinish(pole);
		}
		if (dangers[0] && rand() % 3 == 1) // placing kusts (fuck bushes)
			for (int i = 1; i < width - 1; i++)
				for (int j = 1; j < height - 1; j++)
				{
					if (rand() % 4 == 1 && pole[i][j] == simv_floor) pole[i][j] = simv_kust;
					if (rand() % 2 == 1 && pole[i][j] == simv_floor && (pole[i + 1][j] == simv_kust || pole[i - 1][j] == simv_kust || pole[i][j + 1] == simv_kust && pole[i][j - 1] == simv_kust))pole[i][j] = simv_kust;
				}
		if (dangers[2] && rand() % 4 == 0) // placing rocks
			for (int i = 0; i < width; i++)
				for (int j = 0; j < height; j++)
					if (rand() % 9 == 1 && pole[i][j] == simv_floor)pole[i][j] = simv_rock;

		if (rand() % 3 == 1)
		{
			for (int i = 1; i < width - 2; i++) //placing moving walls
				for (int j = 1; j < height - 2; j++)
				{
					if (pole[i][j] == simv_wall) {
						int x = rand() % 4; 
						srand((int)time(0));
						if (x == 1 && rand() % 9 == 1 && pole[i][j] != simv_exit && pole[i + 1][j] != simv_exit && pole[i + 2][j] != simv_exit) { pole[i][j] = '1'; pole[i + 1][j] = '2'; pole[i + 2][j] = '3'; }
						if (x == 2 && rand() % 5 == 1 && pole[i][j] != simv_exit && pole[i][j + 1] != simv_exit && pole[i][j + 2] != simv_exit) { pole[i][j] = '1'; pole[i][j + 1] = '2'; pole[i][j + 2] = '3'; }
						if (x == 3 && rand() % 5 == 1 && pole[i][j] != simv_exit && pole[i + 1][j] != simv_exit) { pole[i][j] = '1'; pole[i + 1][j] = '2'; }
						if (x == 0 && rand() % 5 == 1 && pole[i][j] != simv_exit && pole[i][j + 1] != simv_exit) { pole[i][j] = '1'; pole[i][j + 1] = '2'; }
					}
				}
		}

	}

	void Show(int steps, double score, double bestscore, bool areyoubig, int currentstate)
	{
		if (!areyoubig)changebuffersize(4 * width + 10, height + 10); else changebuffersize((4 * width + 10) / 2, (height + 10) / 2);
		cout << endl;
		system("cls");
		if (areyoubig) {
			int inach = 0, jnach = 0;
			if (hero.x >= width / 2 && hero.y <= height / 2) { inach = width / 2; jnach = 0; }
			if (hero.x <= width / 2 && hero.y >= height / 2) { inach = 0; jnach = height / 2; }
			if (hero.x >= width / 2 && hero.y >= height / 2) { inach = width / 2; jnach = height / 2; }

			for (int i = inach; i < inach + width / 2; i++)
			{
				for (int j = jnach; j < jnach + height / 2; j++) {
					if (pole[i][j] == simv_exit&& lvlnumber== 75) cout << char(206) << char(205);
					else if (pole[i][j] == '1' || pole[i][j] == '2' || pole[i][j] == '3')
					{
						if (currentstate == pole[i][j] - '0') cout << simv_movwall << simv_movwall;
						else  cout << simv_floor << simv_floor;
					}
					
					else cout << pole[i][j] << pole[i][j];
					
				}

				if (i == inach + 0) cout << "           Level: " << lvlnumber;
				if (i == inach + 2) cout << setprecision(3) << fixed << "         SCORE: " << score;
				if (i == inach + 3) cout << setprecision(3) << fixed << "       bestscore: " << bestscore;
				if (i == inach + 6) cout << "           Steps: " << steps;
				if (i == inach + width / 2 - 2) cout << "     Press 0 to end game ";
				cout << endl;
			}
		}
		else
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++) {
					if (pole[i][j] == simv_exit && lvlnumber == 75) cout << char(206) << char(205);
					else if (pole[i][j] == '1' || pole[i][j] == '2' || pole[i][j] == '3')
					{
						if (currentstate == pole[i][j] - '0') cout << simv_movwall << simv_movwall;
						else  cout << simv_floor << simv_floor;
					}

					else cout << pole[i][j] << pole[i][j];
				}
					if (i == 0) cout << "           Level: " << lvlnumber;
					if (i == 2) cout << setprecision(3) << fixed << "         SCORE: " << score;
					if (i == 3) cout << setprecision(3) << fixed << "       bestscore: " << bestscore;
					if (i == 6) cout << "           Steps: " << steps;
					if (i == width - 2) cout << "     Press 0 to end game ";
					cout << endl;
				
			}
	}

	bool PlaceFinish(char **f)
	{
		bool flag = false;
		int nwas, mwas;
		for (int n = 0; n < width; n++)
			for (int m = 0; m < height; m++)
				if (f[n][m] == simv_exit) { nwas = n; mwas = m; }
		for (int i = 0; i < (int)height*sqrt(width); i++)
			for (int n = 0; n < width; n++)
				for (int m = 0; m < height; m++)
				{
					if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m != 0 && m != height - 1)
					{
						if (f[n + 1][m] == hero.name || f[n - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == 0 && m != 0 && m != height - 1)
					{
						if (f[n + 1][m] == hero.name || f[width - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m != 0 && m != height - 1)
					{
						if (f[0][m] == hero.name || f[n - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m == 0)
					{
						if (f[n + 1][m] == hero.name || f[n - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][height - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][height - 1] != simv_wall) f[n][height - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m == height - 1)
					{
						if (f[n + 1][m] == hero.name || f[n - 1][m] == hero.name || f[n][0] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == 0 && m == 0)
					{
						if (f[n + 1][m] == hero.name || f[width - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][height - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][height - 1] != simv_wall) f[n][height - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m == height - 1)
					{
						if (f[0][m] == hero.name || f[n - 1][m] == hero.name || f[n][0] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == 0 && m == height - 1)
					{
						if (f[n + 1][m] == hero.name || f[width - 1][m] == hero.name || f[n][0] == hero.name || f[n][m - 1] == hero.name) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m == 0)
					{
						if (f[0][m] == hero.name || f[n - 1][m] == hero.name || f[n][m + 1] == hero.name || f[n][height - 1] == hero.name) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][width - 1] != simv_wall) f[n][width - 1] = simv_exit;
						}
					}
				}
		for (int n = 0; n < width; n++)
			for (int m = 0; m < height; m++)
				if (f[n][m] == simv_exit) f[n][m] = simv_floor;
		if (flag) f[nwas][mwas] = simv_exit;
		return flag;
	}
};




int main()
{
	ifstream savefile("Save.txt");
	changefontsize(20);
	Hero A;
	int wallsspeed = 3;
	int currentstate = 1, inverter = 1;;
	bool goodending = false;
	int bonus = 0;
	int steps = 0;
	int lvlnumber = 0;
	bool crystalfound = false;
	double score = 0, bestscore = 0;
	bool skins[skinamount] = { true,false,false,false,false,false,false };
	bool newpage = false, newskin = false;
	savefile >> bestscore >> skins[5] >> crystalfound;
	char firsttime;

	cout << endl << endl << endl;
	cout << "  Are you playing FIRST TIME?" << endl;
	cout << "      y(yes)  /  else(NO)    " << endl;
	cout << "            ";

	cin >> firsttime; Sleep(100);

	if (firsttime == 'y') {
		ofstream sf("Save.txt"); sf << 0 << " " << false << " " << false; bestscore = 0; skins[5] = false; crystalfound = false; sf.close();
	}


	string pos[skinamount] = { "  ","  ","  ","  ", "  ","  " };

	if (bestscore >= 5) { dangers[0] = true; }
	if (bestscore >= 15) { skins[1] = true; dangers[1] = true; }
	if (bestscore >= 25) { skins[2] = true; dangers[2] = true; }
	if (bestscore >= 50) { skins[3] = true; dangers[3] = true; }
	if (bestscore >= 75 && crystalfound) { skins[4] = true; dangers[4] = true; }
	if (bestscore >= 100 ) { dangers[5] = true; }
	if (bestscore >= 125 && crystalfound) { skins[6] = true;}
	char direction = '0';
	while (direction != '!')
	{
		switch (direction)
		{
		case '0':// main menu
		{
			system("cls");
			cout
				<< "           ORJAZZM          " << endl
				<< "                            " << endl
				<< "      <1> Start Game        " << endl
				<< "      <2> Story   ";
			if (newpage) cout << "++";
			cout << endl
				<< "      <3> Skins   ";
			if (newskin) cout << "++";
			cout << endl
				<< "                            " << endl
				<< "         <0> Quit           " << endl
				<< endl
				<< "             ";
			cin >> direction;
			if (direction == '!') direction = 'e';
			if (direction == '0') direction = '!';
			if (direction == '?')
			{
				skins[5] = true;
				ofstream sf("Save.txt"); sf << bestscore << " " << skins[5];
				sf.close();
				newskin = true;
			}
			break;
		}
		case '1': // game
		{
			int xnew, ynew, n, m;
			direction = 'e';
			if (firsttime == 'y')
			{
				firsttime = 'n';
				system("cls");
				cout << endl << endl << endl;
				cout << "         This is YOU --> AA          " << endl;
				cout << " And this is your destination --> XX  " << endl;
				cout << endl;
				cout << "             ";
				Sleep(2000);
				system("cls");
				cout << endl << endl << endl;
				cout << "   Find path ASAP, but remember      " << endl;
				cout << "         EVERY STEP IS PAIN          " << endl;
				cout << endl;
				cout << "             ";
				Sleep(3000);
			}
			int wallwait = wallsspeed;
			while (direction != '0')
			{
				Level lvl(A, lvlnumber, steps, score, bestscore);
				m = lvl.width;
				n = lvl.height;

				// передвижение
				while (direction != '0' && lvl.hero.memory != simv_exit)
				{
					lvl.pole[lvl.hero.x][lvl.hero.y] = lvl.hero.memory;
					direction = _getch();
					ynew = lvl.hero.y; xnew = lvl.hero.x;
					switch (direction)
					{
					case 'w':xnew--; break;
					case 'a':ynew--; break;
					case 's':xnew++; break;
					case 'd':ynew++; break;
					}

					wallwait--;
					if (wallwait == 0) {
						wallwait = wallsspeed;
						currentstate += inverter;
						if (currentstate == 1 || currentstate == 3) inverter *= -1;
					}
					xnew = xnew % n; if (xnew == -1)xnew = n - 1;
					ynew = ynew % m; if (ynew == -1)ynew = m - 1;
					if (lvl.pole[xnew][ynew] != simv_wall && currentstate != lvl.pole[xnew][ynew] - '0')
					{
						lvl.hero.x = xnew; lvl.hero.y = ynew;
						lvl.hero.memory = lvl.pole[xnew][ynew];
					}
					if (lvl.hero.memory != simv_kust)  lvl.pole[lvl.hero.x][lvl.hero.y] = lvl.hero.name;
					if (lvl.hero.memory == simv_rock) bonus -= 1;
					if (direction != '0')
					{
						steps++;
						score = lvlnumber * (1 + (double)lvlnumber / 100.0) - (double)steps / (10 + 2 * lvlnumber / 3) + bonus;

						if (bestscore < 5 && score >= 5) { dangers[0] = true; newpage = true; }
						if (bestscore < 10 && score >= 10) { skins[1] = true; newskin = true; dangers[1] = true; newpage = true; }
						if (bestscore < 25 && score >= 25) { skins[2] = true; newskin = true; }
						if (bestscore < 50 && score >= 50) { skins[3] = true; newskin = true; }
						if (bestscore < 100 && score >= 100) { skins[4] = true; newskin = true; }
						if (score > bestscore)
						{
							bestscore = score;
							ofstream sf("Save.txt");
							sf << bestscore << " " << skins[5] << " " << crystalfound;
							sf.close();
						}
					}
					lvl.Show(steps, score, bestscore, lvl.areyoubig, currentstate);
				}
				if (lvlnumber == 1 && !crystalfound) {
					crystalfound = true;
					ofstream sf("Save.txt");
					sf << bestscore << " " << skins[5]<<" " << crystalfound;
					dangers[4] = true;
					sf.close();
				}
				lvlnumber++;
				if (lvl.areyoubig)
				{
					bonus += 3;
					score = lvlnumber * (1 + (double)lvlnumber / 100.0) - (double)steps / (10 + 2 * lvlnumber / 3) + bonus;
				}
				lvl.Show(steps, score, bestscore, lvl.areyoubig, currentstate);
			}
			break;
		}
		case '2': // story pages
		{
			newpage = false;
			int page = 1;
			while (direction != '0')
			{
				system("cls");
				switch (page)
				{
				case 1:
					cout
						<< "           Page 1/" << pageamount << endl
						<< "                                " << endl
						<< "    Hate those dumb guards.     " << endl
						<< "    I WILL rule this kingdom.   " << endl
						<< "    Good way or another.        " << endl
						<< "       <use WASD to move>       " << endl
						<< "           <0> Back          <d>" << endl
						<< endl
						<< "             ";
					break;
				case 2:
					cout
						<< "           Page 2/" << pageamount << endl
						<< "                                " << endl
						<< "    Hope, gossips do not lie    " << endl
						<< "    At the bottom of city sewers" << endl
						<< "    Lies old graveyard.         " << endl
						<< "    Time to meet my old friend. " << endl
						<< "<a>        <0> Back          <d>" << endl
						<< endl
						<< "             ";
					break;
				case 3:
					if (dangers[0]) {
						cout
							<< "           Page 3/" << pageamount << endl
							<< "                                " << endl
							<< "     Dense VINES appears        " << endl
							<< "     More and more often.       " << endl
							<< "     How long am I wandering    " << endl
							<< "     through this MAZES ?       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else
					{
						cout
							<< "           Page 3/" << pageamount << endl
							<< "                                " << endl
							<< "     ????? ????? ???????        " << endl
							<< "     ???? ??? ???? ??????       " << endl
							<< "     ??? ???? ?? ? ?????????    " << endl
							<< "     ??????? ???? ????? ?       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 4:
					if (dangers[1]) {
						cout
							<< "           Page 4/" << pageamount << endl
							<< "                                " << endl
							<< "     Rooms are getting BIGGER,  " << endl
							<< "     finding exit is much       " << endl
							<< "     harder than before.        " << endl
							<< "     Crypts are close.          " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 4/" << pageamount << endl
							<< "                                " << endl
							<< "     ????? ??? ??????? ???????  " << endl
							<< "     ? ?????? ??? ?????         " << endl
							<< "     ??? ??? ??? ?????????      " << endl
							<< "     ? ???? ????? ??? ?????     " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 5:
					if (dangers[2]) {
						cout
							<< "           Page 5/" << pageamount << endl
							<< "                                " << endl
							<< "     Becoming raw-boned, pale.  " << endl
							<< "     Skin now white as snow.    " << endl
							<< "     Why I'm not starving?      " << endl
							<< "     I hope it'll end soon.     " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 5/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 6:
					if (dangers[2]) {
						cout
							<< "           Page 6/" << pageamount << endl
							<< "                                " << endl
							<< "     ROCKS? Who leave rocks?    " << endl
							<< "     Oh, it's bones. Need to    " << endl
							<< "     find NECROMANCER. Fast.    " << endl
							<< "     I am prepared, am I?       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 6/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 7:
					if (dangers[3]) {
						cout
							<< "           Page 7/" << pageamount << endl
							<< "                                " << endl
							<< "     Of course I am, it's my    " << endl
							<< "     DESTINY. I was born to     " << endl
							<< "     Ru.. to Rual .. was born   " << endl
							<< "     to ...    SERVE.           " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 7/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 8:
					if (dangers[3]) {
						cout
							<< "           Page 8/" << pageamount << endl
							<< "                                " << endl
							<< "      I can't let MASTER wait.  " << endl
							<< "      Walls MOVE, but dead moves" << endl
							<< "      faster. the ARMY OF DEAD  " << endl
							<< "      calls me.         " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 8/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 9:
					if (dangers[4]) {
						cout
							<< "           Page 9/" << pageamount << endl
							<< "                                " << endl
							<< "    Thanks god, i find that     " << endl
							<< "    shiny sword. My mind     " << endl
							<< "    is pured, and i know why    " << endl
							<< "    I am truly here...          " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 9/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back          <d>" << endl
							<< endl
							<< "             ";
					}
					break;
				case 10:
					if (A.name == skins[6])
					{
						cout
							<< "           Page 10/" << pageamount << endl
							<< "                                " << endl
							<< "      I killed him. In these    " << endl
							<< "      mazes i found only evil   " << endl
							<< "      and madness. I can't      " << endl
							<< "      fight any more, I REST.   " << endl
							<< "<a>        <0> Back             " << endl
							<< endl
							<< "             ";
					}
					else if (dangers[5])
					{
						cout
							<< "           Page 10/" << pageamount << endl
							<< "                                " << endl
							<< "      I am a new SKELETON KING  " << endl
							<< "      And no mortal can         " << endl
							<< "      stop my army.             " << endl
							<< "      MUA - HA - HA - HAAAAA    " << endl
							<< "<a>        <0> Back          ???" << endl
							<< endl
							<< "             ";
					}
					else {
						cout
							<< "           Page 10/" << pageamount << endl
							<< "                                " << endl
							<< "     ?????? ??? ????? ??????    " << endl
							<< "     ??? ????? ??? ??????       " << endl
							<< "     ???? ?? ???? ??????????    " << endl
							<< "     ? ?? ????????  ?? ??       " << endl
							<< "<a>        <0> Back             " << endl
							<< endl
							<< "             ";
					}
					break;
				}
				direction = _getch();
				if (direction == 'a') page--;
				if (direction == 'd') page++;
				if (page < 1) page = 1;
				if (page > pageamount) page = pageamount;
			}
			break;
		}
		case '3': // skins
		{
			int page = 1;
			newskin = false;
			int m = A.skinnumber;
			direction = 'e';
			while (direction != '0')
			{
				pos[A.skinnumber] = "^^";
				system("cls");
				if (page == 1)
				{
					cout
						<< "       Choose your skin      " << endl
						<< "                             " << endl
						<< "     ";
					if (skins[0]) cout << hero_skins[0] << hero_skins[0] << "       "; else cout << "??       ";
					if (skins[1]) cout << hero_skins[1] << hero_skins[1] << "       "; else cout << "??       ";
					if (skins[2]) cout << hero_skins[2] << hero_skins[2] << "       " << endl; else cout << "??       " << endl;
					cout << "     " << pos[0][0] << pos[0][1] << "       " << pos[1][0] << pos[1][1] << "       " << pos[2][0] << pos[2][1] << "       " << endl << endl;
					cout << "     ";
					if (skins[3]) cout << hero_skins[3] << hero_skins[3] << "       "; else cout << "??       ";
					if (skins[4]) cout << hero_skins[4] << hero_skins[4] << "       "; else cout << "??       ";
					if (skins[5]) cout << hero_skins[5] << hero_skins[5] << "       " << endl; else cout << "??       " << endl;
					cout << "     " << pos[3][0] << pos[3][1] << "       " << pos[4][0] << pos[4][1] << "       " << pos[5][0] << pos[5][1] << "       " << endl << endl;
					cout
						<< "                             " << endl
						<< "           <0> Back          " << endl

						<< "             ";
				}
				if (page == 2)
				{
					cout
						<< "       Choose your skin      " << endl
						<< "                             " << endl << endl << endl
						<< "              ";

					if (skins[6]) cout << hero_skins[6] << hero_skins[6] << endl; else cout << "??" << endl;
					cout << "              " << pos[6][0] << pos[6][1] << endl << endl;
					cout
						<< endl
						<< "                             " << endl
						<< "<a>        <0> Back          " << endl
						<< endl
						<< "             ";
				}
				direction = _getch();
				if (direction == 'd')
				{
					pos[m] = "  ";
					m++; if (m > skinamount - 1)m = 0;
					while (!skins[m]) { pos[m] = "  "; m++; if (m > skinamount - 1)m = 0; }
					A.skinnumber = m;
					if (m == 6)page++;
				}
				if (direction == 'a') {
					pos[m] = "  ";
					m--; if (m < 0)m = skinamount - 1;
					while (!skins[m]) { pos[m] = "  "; m--; if (m < 0)m = skinamount - 1; }
					A.skinnumber = m;
					if (m < 6)page = 1;
				}

			}
			A.name = hero_skins[A.skinnumber];
			break;
		}
		default:
			direction = '0';
		}
	}
	return 0;
}