#include "pch.h"     // удалите это если у вас не Visual Studio
#include <iostream>
#include <fstream>
#include <iomanip>   // для setprecision и fixed
#include <windows.h> // для задержки Sleep(40), в миллисекундах
#include <ctime>     // для генератора случайных чисел
#include <conio.h>
#include <stdio.h>
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
	COORD crd = { a, b };
	SMALL_RECT src = { 0, 0, crd.X - 1, crd.Y - 1 };
	SetConsoleWindowInfo(out_handle, true, &src);
	SetConsoleScreenBufferSize(out_handle, crd);
}
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

	void Show(int steps, double score,double bestscore)
	{
		changebuffersize(4 * width + 10, height + 10);
		cout << endl;
		system("cls");
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
				cout << pole[i][j] << pole[i][j];
			if (i == 0) cout << "           Level: " << lvlnumber;
			if (i == 2) cout << setprecision(3) << fixed << "         SCORE: " << score;
			if (i == 3) cout << setprecision(3) << fixed << "       bestscore: " << bestscore;
			if (i == 6) cout << "           Steps: " << steps;
			if (i == width - 2) cout << "     Press 0 to end game ";
			cout << endl;
		}
	}
	Level(Hero h, int number, int steps, double score,double bestscore)
	{
		hero = h;
		lvlnumber = number;
		height = 10 + number / 3;
		width = 10 + number / 3;
		Create();

		Show(steps, score,bestscore);
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
					else if (f[n][m] == simv_exit && n == 0 && m == 0)
					{
						if (f[n + 1][m] == simv_hero || f[width - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][height - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][height - 1] != simv_wall) f[n][height - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m == height - 1)
					{
						if (f[0][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][0] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == 0 && m == height - 1)
					{
						if (f[n + 1][m] == simv_hero || f[width - 1][m] == simv_hero || f[n][0] == simv_hero || f[n][m - 1] == simv_hero) flag = true;
						else {
							if (f[n + 1][m] != simv_wall) f[n + 1][m] = simv_exit;
							if (f[width - 1][m] != simv_wall) f[width - 1][m] = simv_exit;
							if (f[n][0] != simv_wall) f[n][0] = simv_exit;
							if (f[n][m - 1] != simv_wall) f[n][m - 1] = simv_exit;
						}
					}
					else if (f[n][m] == simv_exit && n == width - 1 && m == 0)
					{
						if (f[0][m] == simv_hero || f[n - 1][m] == simv_hero || f[n][m + 1] == simv_hero || f[n][height - 1] == simv_hero) flag = true;
						else {
							if (f[0][m] != simv_wall) f[0][m] = simv_exit;
							if (f[n - 1][m] != simv_wall) f[n - 1][m] = simv_exit;
							if (f[n][m + 1] != simv_wall) f[n][m + 1] = simv_exit;
							if (f[n][width - 1] != simv_wall) f[n][width - 1] = simv_exit;
						}
					}
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




int main()
{
	ifstream savefile("Save.txt");
	Hero A;
	int xnew, ynew, n, m;
	int steps = 0;
	int lvlnumber = 0;
	double score = 0,bestscore = 0;
	savefile >> bestscore;
	char direction = 'a';
	changefontsize(20);

	char menu;
	cout << endl << endl << endl;
	cout << "  Are you Playing FIRST TIME?" << endl;
	cout << "      y(yes)  /  else(NO)    " << endl;
	cout << "            ";
	cin >> menu;
	if (menu == 'y') {
		system("cls");
		ofstream sf("Save.txt"); sf << 0; 
		cout << endl << endl << endl;
		cout << "        This is YOU --> AA          " << endl;
		cout << "And this is your destination --> XX  " << endl;
		cout << endl;
		cout << "            ";
		Sleep(4000);
		system("cls");
		cout << endl << endl << endl;
		cout << "  Find path ASAP, but remember      " << endl;
		cout << "        EVERY STEP IS PAIN          " << endl;
		cout << endl;
		cout << "            ";
		Sleep(4000);
	}


	while (direction != '0')
	{
		Level lvl(A, lvlnumber, steps, score,bestscore);
		m = 10 + lvlnumber / 3;
		n = 10 + lvlnumber / 3;
		// передвижение
		while (direction != '0' && lvl.hero.memory != simv_exit)
		{
			lvl.pole[lvl.hero.x][lvl.hero.y] = lvl.hero.memory;
			direction = _getch();
			if (direction == 224) 
			{
				int m = _getch();
				switch (m)
				{
				case 72: direction = 'w'; break;
				case 75:direction = 'a'; break;
				case 80:direction = 's'; break;
				case 77:direction = 'd'; break;
				}
			}
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
				//score = lvlnumber * (1 + (double)lvlnumber / 100.0+ (double)lvlnumber / 300.0) - (double)steps / 10.0;       
				score = lvlnumber * (1 + (double)lvlnumber / 100.0) - (double)steps / (10 + 2 * lvlnumber / 3); 
				if (score > bestscore)
				{
					bestscore = score;
					ofstream savefile("Save.txt");
					savefile << bestscore;
					savefile.close();

				}
				//(1 + (double)lvlnumber )*5.0*(double)lvlnumber / ((double)steps * (1 + (double)steps / 100.0));
			}
			lvl.Show(steps, score,bestscore);
		}
		lvlnumber++;
		lvl.Show(steps, score,bestscore);
	}
	return 0;
}