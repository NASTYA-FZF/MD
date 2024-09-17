#include "pch.h"
#include "model.h"
using namespace std;

crystall::crystall()
{
	//цикл по вертикали (<= потому что я думаю, что на второй границе тоже должны быть частицы)
	for (int y = 0; y <= L; y++)
	{
		//цикл по горизонтали (<= по той же причине)
		for (int x = 0; x <= L; x++)
		{
			//если остаток от деления на 2 будет 1, то нечетные ряды (ограничение по х нужно, чтобы не вылезло за прямоугольник)
			if (y % 2 == 1 && x < L)
				setka.push_back(atom(0.5 * r0 + x * r0, y * b));
			if (y % 2 == 0)
				setka.push_back(atom(x * r0, y * b)); //четные ряды
		}
	}
}

std::vector<std::vector<double>> crystall::GetPos()
{
	int size = setka.size();
	vector<vector<double>> res(size);

	for (int i = 0; i < size; i++)
		res[i].insert(res[i].begin(), setka[i].coord.begin(), setka[i].coord.end()); //копируем координаты атомов

	return res;
}

atom::atom(double x, double y)
{
	coord.insert(coord.begin(), { x, y });
}
