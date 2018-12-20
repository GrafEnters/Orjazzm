#include "pch.h" // удалите это если у вас не Visual Studio
#include <iostream>
#include <iomanip>  //для setprecision
#include <windows.h> // для задержки Sleep(40); 40 - колво миллисекунд
#include <ctime> // для генератора случайных чисел
using namespace std;

const char simv_wall = char(219); // '▓' стена
const char simv_floor = char(32); // ' ' проход
const char simv_exit = char(88);  // 'X' выход
const char simv_hero = char(65);  // 'A' герой

struct Hero
{
	int x, y, hp;
	char name, memory;
};

class Field
{
public:
	int n, m;
	Hero h;
	char **pole;
	int lvlnumber;
	bool exist = false;
	void Create()
	{
		exist = true;
		srand((int)time(0));
		pole = new char*[m];
		for (int z = 0; z < n; z++)pole[z] = new char[n];
		h.memory = simv_floor;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				pole[i][j] = simv_floor;
				if (rand() % 3 == 1) pole[i][j] = simv_wall;
			}
		}
		char p = simv_hero;
		while (p == simv_hero)
			p = pole[(rand() % n)][(rand() % m)];
		pole[(rand() % n)][(rand() % m)] = simv_exit;
	}
	void Show(int steps, double score)
	{
		if (exist) {
			Sleep(80); cout << endl;
			system("cls");
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < m; j++)
					cout << pole[i][j] << pole[i][j];
				if (i == 0) cout << "           Level: " << lvlnumber;
				if (i == 2) cout << setprecision(3) << fixed << "           SCORE: " << score;
				if (i == 6) cout << "           Steps: " << steps;				
				if (i == n - 2) cout << "     Press 0 to end game ";
				cout << endl;
			}
		}
	}
	Field(int number, int steps, double score)
	{
		srand((int)time(0));
		lvlnumber = number;
		m = 10 + number / 3;
		n = 10 + number / 3;
		h.x = 2;
		h.y = 2;
		h.name = simv_hero;
		h.memory = simv_floor;
		Create();
		pole[h.x][h.y] = h.name;
		Show(steps, score);
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
	int steps = 0;
	double score = 0;
	changefontsize(20);
	int xnew = 2, ynew = 2, n = 10, m = 10, lvlnumber = 0;
	Hero A;
	A.name = simv_hero;
	A.memory = simv_floor;
	A.x = 2;
	A.y = 2;
	char direction = 'a';


	while (direction != '0')
	{
		Field b(lvlnumber, steps, score);
		m = 10 + lvlnumber / 3;
		n = 10 + lvlnumber / 3;
		// передвижение
		while (direction != '0' && b.h.memory != simv_exit)
		{
			cin >> direction;
			b.pole[b.h.x][b.h.y] = b.h.memory;
			switch (direction)
			{
			case 'w':
				xnew = b.h.x - 1; xnew = xnew % n; if (xnew == -1)xnew = n - 1;
				if (b.pole[(xnew)][b.h.y] != simv_wall) {
					b.h.x = xnew;
					b.h.memory = b.pole[b.h.x][b.h.y];
				}
				break;
			case 'a':
				ynew = b.h.y - 1; ynew = ynew % m; if (ynew == -1)ynew = m - 1;
				if (b.pole[(b.h.x)][ynew] != simv_wall) {
					b.h.y = ynew;
					b.h.memory = b.pole[b.h.x][b.h.y];
				}
				break;
			case 's':
				xnew = b.h.x + 1; xnew = xnew % n; if (xnew == -1)xnew = n - 1;
				if (b.pole[(xnew)][b.h.y] != simv_wall) {
					b.h.x = xnew;
					b.h.memory = b.pole[b.h.x][b.h.y];
				}
				break;
			case 'd':
				ynew = b.h.y + 1; ynew = ynew % m;
				if (b.pole[(b.h.x)][ynew] != simv_wall) {
					b.h.y = ynew;
					b.h.memory = b.pole[b.h.x][b.h.y];
				}
				break;
			}
			if (b.exist) b.pole[b.h.x][b.h.y] = b.h.name;
			steps++; if (direction == '0') steps--;
			score = (1 + (double)steps / 10.0)*5.0*(double)lvlnumber / ((double)steps * (1 + (double)steps / 100.0));
			b.Show(steps, score);
		}
		lvlnumber++;
		score = (1 + (double)steps / 10.0)*5.0*(double)lvlnumber / (double)steps;
		b.Show(steps, score);
	}
	return 0;
}