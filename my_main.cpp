#include "fairy_tail.hpp"
#include <locale.h>

/*Для решения поставленной задачи создан класс Solution, метод Solve проверяет, возможна ли встреча, 
метод Show выводит всю известную информацию о местности, Addition_Research позволяет Ивану и Елене
погулять вместе, дополнительно исследуя окрестности в пределах допустимого количества ходов

Подробнее каждый метод описан при реализации*/

class Solution {
private: 
	Fairyland world; 

	//карта местности глазами Ивана
	//Иван изначально стоит в центре, размер карты выбран таким образом, чтобы при любых перемещениях
	//не было выхода за пределы массивы
	// ? - не исследованная клетка
	// # - не проходимая клетка
	// . - проходимая, но еще не посещенная клетка
	// d - проходимая посещенная клетка, в которую Иван пришел из нижней
	// r - проходимая посещенная клетка, в которую Иван пришел из правой
	// l - проходимая посещенная клетка, в которую Иван пришел из левой
	// u - проходимая посещенная клетка, в которую Иван пришел из верхней
	// @ - исходное положение Ивана
	// & - исходное положение Василисы
	char** Ivan; 
	int size = 21;

	//границы исследованной территории (в координатах карты Ивана)
	int left_border = size; 
	int right_border = -1; 
	int up_border = size; 
	int down_border = -1; 

	//исходное положение Елены и текущее положение Ивана на карте Ивана
	//x - строка, y - столбец
	int Elena_x = -1; 
	int Elena_y = -1;
	int Ivan_curX = -1; 
	int Ivan_curY = -1;

	// символ, который должен стоять на месте исходного положения Елены, в зависимости от того, 
	// из какой клетки Иван туда пришел
	char insteadElena;

public: 
	//конструктор по умолчания
	//вся карта не исследована, Иван стоит в центре
	Solution() {
		Ivan = new char* [size]; 
		for (int i = 0; i < size; i++) {
			Ivan[i] = new char[size];
			for (int j = 0; j < size; j++) {
				Ivan[i][j] = '?'; 
			}
		}
		Ivan[size / 2][size / 2] = '@';
		insteadElena = -1;
	}

	//проверяет возможность встречи
	//Иван ходит по карте, Елена стоит на месте
	//это позволяет избежать ситуации, при которой персонажи будут уходить друг от друга
	bool Solve() {
		Ivan_curX = size / 2; 
		Ivan_curY = size / 2;

		//рекурсивный метод хода Ивана, возвращающий true при возможности встречи
		return Search();
	}

	//вывод одной из возможных карт местности в поток
	bool Show(std::ostream& cout) {
		//если граница не определена, то о карте еще ничего не известно
		if (right_border == -1) return false;
		
		//иначе выбираем из карты Ивана участок, который может соответстовать карте местности
		int i_start, i_finish, j_start, j_finish; 
		
		if (right_border - left_border == 11) {
			i_start = left_border + 1; 
			i_finish = right_border - 1; 
		}
		else {
			i_start = left_border; 
			i_finish = left_border + 9;
		}

		if (down_border - up_border == 11) {
			j_start = up_border + 1;
			j_finish = down_border - 1;
		}
		else {
			j_start = up_border;
			j_finish = up_border + 9;
		}

		//обозначаем исходное положение Елены
		if (Elena_x != -1) Ivan[Elena_x][Elena_y] = '&';

		for (int i = i_start; i <= i_finish; i++) {
			for (int j = j_start; j <= j_finish; j++) {
				if (Ivan[i][j] > 'a' && Ivan[i][j] < 'z') cout << '.'; 
				else cout << Ivan[i][j];
			}
			cout << std::endl;
		}

		return true;
	}

	//дополнительное исследование карты местности
	bool Additional_Research() {
		//если встречи не было, Иван и так уже исследовал все, что можно было
		//исследование Елены будет бесполезно, так как нет возможности определить, 
		//как соотносятся карты Елены и Ивана
		if (Elena_x == -1) return false;

		//для дальнейшего исследования удобнее, чтобы на карте не было лишних знаков, 
		//поэтому исходную позицию Елены заменяем знаком, указывающим направление, из которого пришел Иван
		Ivan[Elena_x][Elena_y] = insteadElena; 

		//рекурсивный метод совместной прогулки Ивана и Елены
		Research_Together();

		return true;
	}

	//получение количества ходов
	int getTurnCount() {
		return world.getTurnCount();
	}

	~Solution() {
		for (int i = 0; i < size; i++)
			delete[]Ivan[i];
		delete[]Ivan;
	}
	
private:
	// расширение границ исследованной территории, 
	// если текущее положение обеспечивает информацию о местности за ними 
	void Borders(int x, int y) {
		if (x - 1 < left_border) left_border = x - 1;
		if (x + 1 > right_border) right_border = x + 1;
		if (y - 1 < up_border) up_border = y - 1;
		if (y + 1 > down_border) down_border = y + 1;
	}

	//Иван осматривается из текущего положения, заполнение еще не исследованных клеток
	void Look_Around(int x, int y) {
		if (Ivan[x - 1][y] == '?') {
			if (!world.canGo(Character::Ivan, Direction::Up)) Ivan[x - 1][y] = '#';
			else Ivan[x - 1][y] = '.';
		}
		if (Ivan[x + 1][y] == '?') {
			if (!world.canGo(Character::Ivan, Direction::Down)) Ivan[x + 1][y] = '#';
			else Ivan[x + 1][y] = '.';
		}
		if (Ivan[x][y - 1] == '?') {
			if (!world.canGo(Character::Ivan, Direction::Left)) Ivan[x][y - 1] = '#';
			else Ivan[x][y - 1] = '.';
		}
		if (Ivan[x][y + 1] == '?') {
			if (!world.canGo(Character::Ivan, Direction::Right)) Ivan[x][y + 1] = '#';
			else Ivan[x][y + 1] = '.';
		}

	}

	//шаги Ивана при поиске Елены
	bool Search() {
		//Иван осматривается
		Look_Around(Ivan_curX, Ivan_curY);
		//при необходимости расширяются границы
		Borders(Ivan_curX, Ivan_curY);

		//Иван пробует пойти вверх
		if (Ivan[Ivan_curX - 1][Ivan_curY] == '.')
		{
			Ivan[Ivan_curX - 1][Ivan_curY] = 'd';
			Ivan_curX--;
			if (world.go(Direction::Up, Direction::Pass)) { //Иван пришел к Елене
				Elena_x = Ivan_curX;
				Elena_y = Ivan_curY;
				insteadElena = 'd';
				return true;
			}
			else {
				if (Search()) return true;
			}
		}

		//Иван пробует пойти вниз
		if (Ivan[Ivan_curX + 1][Ivan_curY] == '.')
		{
			Ivan[Ivan_curX + 1][Ivan_curY] = 'u';
			Ivan_curX++;
			if (world.go(Direction::Down, Direction::Pass)) { //Иван пришел к Елене
				Elena_x = Ivan_curX;
				Elena_y = Ivan_curY;
				insteadElena = 'u';
				return true;
			}
			else {
				if (Search()) return true;
			}
		}

		//Иван пробует пойти влево
		if (Ivan[Ivan_curX][Ivan_curY - 1] == '.')
		{
			Ivan[Ivan_curX][Ivan_curY - 1] = 'r';
			Ivan_curY--;
			if (world.go(Direction::Left, Direction::Pass)) { //Иван пришел к Елене
				Elena_x = Ivan_curX;
				Elena_y = Ivan_curY;
				insteadElena = 'r';
				return true;
			}
			else {
				if (Search()) return true;
			}
		}

		//Иван пробует пойти вправо
		if (Ivan[Ivan_curX][Ivan_curY + 1] == '.')
		{
			Ivan[Ivan_curX][Ivan_curY + 1] = 'l';
			Ivan_curY++;
			if (world.go(Direction::Right, Direction::Pass)) { //Иван пришел к Елене
				Elena_x = Ivan_curX;
				Elena_y = Ivan_curY;
				insteadElena = 'l';
				return true;
			}
			else {
				if (Search()) return true;
			}
		}


		//Если выполнение программы дошло до этой строки и Иван находится в исходной позиции, 
		//то ни один из путей не привел Ивана к Елене, значит, встреча невозможна
		if (Ivan[Ivan_curX][Ivan_curY] == '@') return false;

		//Если ни один из путей не привел Ивана к Елене, но он еще не находится в исходной клетке, 
		//то можно сделать шаг назад (возможно, на нужной развилке Иван свернул не туда)
		if (Ivan[Ivan_curX][Ivan_curY] == 'd') {
			world.go(Direction::Down, Direction::Pass);
			Ivan_curX++;
			if (Search()) return true;
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'u') {
			world.go(Direction::Up, Direction::Pass);
			Ivan_curX--;
			if (Search()) return true;
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'l') {
			world.go(Direction::Left, Direction::Pass);
			Ivan_curY--;
			if (Search()) return true;
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'r') {
			world.go(Direction::Right, Direction::Pass);
			Ivan_curY++;
			if (Search()) return true;
		}

		//Если выполнение программы дошло до этого места, 
		//то ни один из путей не привел Ивана к Елене
		return false;
	}

	//совместная прогулка для исследования территории
	void Research_Together() {
		//Иван осматривается
		Look_Around(Ivan_curX, Ivan_curY);
		//при необходимости расширяются границы
		Borders(Ivan_curX, Ivan_curY);

		//Если возможно, герои идут вверх
		if (Ivan[Ivan_curX - 1][Ivan_curY] == '.' && world.getTurnCount() < 249)
		{
			Ivan[Ivan_curX - 1][Ivan_curY] = 'd';
			world.go(Direction::Up, Direction::Up);
			Ivan_curX--;
			Research_Together();
		}

		//Если возможно, герои идут вниз
		if (Ivan[Ivan_curX + 1][Ivan_curY] == '.' && world.getTurnCount() < 249)
		{
			Ivan[Ivan_curX + 1][Ivan_curY] = 'u';
			world.go(Direction::Down, Direction::Down);
			Ivan_curX++;
			Research_Together();
		}

		//Если возможно, герои идут влево
		if (Ivan[Ivan_curX][Ivan_curY - 1] == '.' && world.getTurnCount() < 249)
		{
			Ivan[Ivan_curX][Ivan_curY - 1] = 'r';
			world.go(Direction::Left, Direction::Left);
			Ivan_curY--;
			Research_Together();
		}

		//Если возможно, герои идут вправо
		if (Ivan[Ivan_curX][Ivan_curY + 1] == '.' && world.getTurnCount() < 249)
		{
			Ivan[Ivan_curX][Ivan_curY + 1] = 'l';
			world.go(Direction::Right, Direction::Right);
			Ivan_curY++;
			Research_Together();
		}


		//Если впереди все обследовано, герои возвращаются в предыдущую клетку
		if (Ivan[Ivan_curX][Ivan_curY] == 'd' && world.getTurnCount() < 249) {
			world.go(Direction::Down, Direction::Down);
			Ivan_curX++;
			Research_Together();
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'u' && world.getTurnCount() < 249) {
			world.go(Direction::Up, Direction::Up);
			Ivan_curX--;
			Research_Together();
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'l' && world.getTurnCount() < 249) {
			world.go(Direction::Left, Direction::Left);
			Ivan_curY--;
			Research_Together();
		}
		else if (Ivan[Ivan_curX][Ivan_curY] == 'r' && world.getTurnCount() < 249) {
			world.go(Direction::Right, Direction::Right);
			Ivan_curY++;
			Research_Together();
		}
	}
};


int main() {
	setlocale(LC_ALL, "Russian");
	Solution solution;

	if (solution.Solve())
		std::cout << "Встреча произошла. Потрачено ходов: " << solution.getTurnCount() << std::endl;
	else
		std::cout << "Встреча невозможна. Потрачено ходов: " << solution.getTurnCount() << std::endl;

	solution.Show(std::cout);

	if (!solution.Additional_Research())
		std::cout << "Дополнительное исследование местности невозможно, так как встреча не произошла" << std::endl;
	else
	{
		std::cout << "Проведены дополнительные исследования. Всего потрачено ходов: " << solution.getTurnCount() << std::endl;
		solution.Show(std::cout);
	}
}
