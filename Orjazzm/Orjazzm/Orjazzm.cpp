#include "pch.h"     // удалите это если у вас не Visual Studio
#include <iostream>
#include <iomanip>   // для setprecision и fixed
#include <windows.h> // для задержки Sleep(40), в миллисекундах
#include <ctime>     // для генератора случайных чисел
using namespace std;

const char simv_wall = char(219); // '▓' стена
const char simv_floor = char(32); // ' ' проход
const char simv_exit = char(88);  // 'X' выход
const char simv_hero = char(65);  // 'A' герой
const char simv_check = char(46);  // '.' проверка
struct Hero
{
	int x, y, hp;
	char name, memory;
	Hero()
	{
		x = 2;
		y = 2;
		name = simv_hero;
		memory = simv_floor;
	}
};

class Level
{
public:
	int width, height;
	int exitx, exity;
	Hero hero;
	char **pole;
	int lvlnumber;
	void Create()
	{
		srand((int)time(0));
		pole = new char*[height];
		for (int z = 0; z < width; z++)pole[z] = new char[width];
		hero.memory = simv_floor;
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				pole[i][j] = simv_floor;
				if (rand() % 3 == 1) pole[i][j] = simv_wall;
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
	}

	void Show(int steps, double score)
	{
		Sleep(80); cout << endl;
		system("cls");
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
				cout << pole[i][j] << pole[i][j];
			if (i == 0) cout << "           Level: " << lvlnumber;
			if (i == 2) cout << setprecision(3) << fixed << "           SCORE: " << score;
			if (i == 6) cout << "           Steps: " << steps;
			if (i == width - 2) cout << "     Press 0 to end game ";
			cout << endl;
		}
	}
	Level(Hero h, int number, int steps, double score)
	{
		hero = h;
		lvlnumber = number;
		height = 10 + number / 3;
		width = 10 + number / 3;
		Create();

		Show(steps, score);
	}
	bool PlaceFinish(char **f)
	{
		bool flag = false;
		//тут должна быть проверка на достижимостть финиша, но пока я не знаю как это сделать 
		//ни гугл, ни обычный волновой тут не помогут, т.к. у меня зациклено поле 
		//так что ожидаемо в среду я это сделаю, пока нет ни времени, ни желания

		int nwas, mwas;
		for (int n = 0; n < width; n++)
		{
			for (int m = 0; m < height; m++)
			{
				if (f[n][m] == simv_exit) { nwas = n; mwas = m; }
			}
		}
		for (int i = 0; i < (int)height*sqrt(width); i++)
		{
			for (int n = 0; n < width; n++)
			{
				for (int m = 0; m < height; m++)
				{
					if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m != 0 && m != height - 1)
					{
						if (f[n + 1][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == 0 && m != 0 && m != height - 1)
					{
						if (f[n + 1][m] == simv_hero || f[width - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m != 0 && m != height - 1)
					{
						if (f[0][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m == 0)
					{
						if (f[n + 1][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][height - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][height - 1] != simv_wall) f[n][height - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n != 0 && n != width - 1 && m == height - 1)
					{
						if (f[n + 1][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][0] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					// доделать обработку углов, сократить код и сделать его читаемым 
				}
			}
		}
		for (int n = 0; n < width; n++)
		{
			for (int m = 0; m < height; m++)
			{
				if (f[n][m] == simv_exit) f[n][m] = simv_floor;
			}
		}
		if (flag) f[nwas][mwas] = simv_exit;
		return flag;
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


int main()
{
	Hero A;
	int xnew, ynew, n, m;
	int steps = 0;
	int lvlnumber = 0;
	double score = 0;
	char direction = 'a';
	changefontsize(20);
	while (direction != '0')
	{
		Level lvl(A, lvlnumber, steps, score);
		m = 10 + lvlnumber / 3;
		n = 10 + lvlnumber / 3;
		// передвижение
		while (direction != '0' && lvl.hero.memory != simv_exit)
		{
			lvl.pole[lvl.hero.x][lvl.hero.y] = lvl.hero.memory;
			cin >> direction;
			ynew = lvl.hero.y; xnew = lvl.hero.x;
			switch (direction)
			{
			case 'w':xnew--; break;
			case 'a':ynew--; break;
			case 's':xnew++; break;
			case 'd':ynew++; break;
			}
			xnew = xnew % n; if (xnew == -1)xnew = n - 1;
			ynew = ynew % m; if (ynew == -1)ynew = m - 1;
			if (lvl.pole[xnew][ynew] != simv_wall)
			{
				lvl.hero.x = xnew; lvl.hero.y = ynew;
				lvl.hero.memory = lvl.pole[xnew][ynew];
			}
			lvl.pole[lvl.hero.x][lvl.hero.y] = lvl.hero.name;
			if (direction != '0')
			{
				steps++;
				score = lvlnumber * (1 + (double)lvlnumber / 100.0) - (double)steps / 10.0;            //(1 + (double)lvlnumber )*5.0*(double)lvlnumber / ((double)steps * (1 + (double)steps / 100.0));
			}
			lvl.Show(steps, score);
		}
		lvlnumber++;
		lvl.Show(steps, score);
	}
	return 0;
}