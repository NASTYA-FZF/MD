#include "pch.h"
#define _USE_MATH_DEFINES
#define random ((double)rand() / RAND_MAX)
#define p2(x) ((x) * (x))
#define p6(x) ((x) * (x) * (x) * (x) * (x) * (x))
#define p3(x) ((x) * (x) * (x))
#define p4(x) ((x) * (x) * (x) * (x))
#include "model.h"
using namespace std;

void crystall::SetStartCoord()
{
	//цикл по вертикали (<= потому что € думаю, что на второй границе тоже должны быть частицы)
	for (int y = 0; y < L; y++)
	{
		//цикл по горизонтали (<= по той же причине)
		for (int x = 0; x < L; x++)
		{
			//если остаток от делени€ на 2 будет 1, то нечетные р€ды (ограничение по х нужно, чтобы не вылезло за пр€моугольник)
			if (y % 2 == 1 && x < L)
				setka.push_back(atom(0.5 * r0 + x * r0, y * b));
			if (y % 2 == 0)
				setka.push_back(atom(x * r0, y * b)); //четные р€ды
		}
	}

	N_atom = setka.size();
}

void crystall::SetStartSpeed(double T)
{
	double v0 = calc_v0(T); //модуль скорости 
	double sum_vx = 0, sum_vy = 0; //вычисл€ем суммы импульсов по проекци€м
	//int num_atom = setka.size(); //число атомов

	for (int i = 0; i < N_atom; i++)
	{
		setka[i].SetSpeed(calc_vx(v0, random), calc_vy(v0, random)); //устанавливаем скорость
		sum_vx += setka[i].speed[0];
		sum_vy += setka[i].speed[1];
	}

	sum_vx /= N_atom;
	sum_vy /= N_atom;

	for (int i = 0; i < N_atom; i++)
	{
		setka[i].Minus_dv(sum_vx, sum_vy); //вычитаем вычисленные поправки
	}

	ControlSpeed(); //проверка в отладке значени€ суммарного импульма
}

void crystall::ControlSpeed()
{
	double sum_vx = 0, sum_vy = 0;
	for (int i = 0; i < N_atom; i++)
	{
		sum_vx += setka[i].speed[0];
		sum_vy += setka[i].speed[1]; //суммарный импульс
	}
	sum_vx /= N_atom;
	sum_vy /= N_atom;
}

double crystall::Kr(double r)
{
	if (r >= r1)
		return p2(1 - p2(r - r1) / p2(r1 - r2));
	if (r < r1)
		return 1.;
	return 0;
}

double crystall::betta()
{
	return sqrt(2. * N_atom * k_B * T / sum_V2);
}

void crystall::perenormirovka()
{
	bet = betta();
	sum_V2 = 0;

	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			setka[i].speed[j] *= bet;
		}
	}
}

int sign(double dx)
{
	return dx >= 0 ? 1 : -1;
}

crystall::crystall(double _T)
{
	T = _T;
	sum_V2 = 0;
	bet = 0;
	SetStartCoord();
	SetStartSpeed(T);
}

std::vector<std::vector<double>> crystall::GetPos()
{
	int size = setka.size();
	vector<vector<double>> res(size);

	for (int i = 0; i < size; i++)
		res[i].insert(res[i].begin(), setka[i].coord.begin(), setka[i].coord.end()); //копируем координаты атомов

	return res;
}

double crystall::len_jons(int num_atom, int coord)
{
	double r, sum = 0.;

	for (int i = 0; i < N_atom; i++)
	{
		if (i != num_atom)
		{
			r = setka[num_atom].rass2_atom(setka[i].coord);

			if (r <= r2)
			{
				sum += Kr(r) * (p6(r0) / p3(r) - 1) * (setka[num_atom].coord[coord] - setka[i].coord[coord]) / p4(r);
				ep += Kr(r) * D * (p2(p6(r0 / r)) - 2. * p6(r0 / r));
			}
		}
	}
	return -12. * D * p6(r0) * sum;
}

void crystall::verle_coord()
{
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (setka[i].Fk[j] == 0) 
				setka[i].Fk[j] = -len_jons(i, j);
			setka[i].coord[j] += setka[i].speed[j] * delta_t + setka[i].Fk[j] * p2(delta_t) / (2. * m);

			if (setka[i].coord[j] < 0)
				setka[i].coord[j] += L * r0;
			else
				if (setka[i].coord[j] > L * r0)
					setka[i].coord[j] -= L * r0;
			
			if (setka[i].coord[j] < 0 || setka[i].coord[j] > L * r0)
			{
				MessageBox(NULL, L"¬не €чейки", L"ќсторожно!", MB_TASKMODAL);
			}
		}
	}
}

void crystall::verle_V()
{
	double Fk;
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Fk = -len_jons(i, j);
			setka[i].speed[j] += (Fk + setka[i].Fk[j]) * delta_t / (2. * m);

			setka[i].Fk[j] = Fk;
			ek += m * p2(setka[i].speed[j]) / 2;
			sum_V2 += m * p2(setka[i].speed[j]) / S;
		}
	}
}

void crystall::OneIterationVerle(int iter)
{
	ep = ek = 0;
	verle_coord();
	verle_V();
	energyEk.push_back(ek / eV);
	energyV.push_back(0.5 * ep / eV);
	energyE.push_back((ek + 0.5 * ep) / eV);

	//if (iter % S == 0)
	//{
	//	perenormirovka();
	//}
}

void crystall::printEnergy(string fileName)
{
	ofstream file_out(fileName);

	for (int i = 0; i < energyE.size(); i++)
	{
		file_out << i + 1 << "\t" << energyEk[i] << "\t" << energyV[i] << "\t" << energyE[i] << endl;
	}

	file_out.close();
}

atom::atom(double x, double y)
{
	coord.insert(coord.begin(), { x, y });
	speed.insert(speed.begin(), { 0, 0 });
	Fk.insert(Fk.begin(), { 0, 0 });
}

void atom::SetSpeed(double vx, double vy)
{
	speed[0] = vx; 
	speed[1] = vy;
}

void atom::Minus_dv(double dvx, double dvy)
{
	speed[0] -= dvx;
	speed[1] -= dvy;
}

double atom::rass2_atom(vec2D coord_atom)
{
	double dx = coord[0] - coord_atom[0], dy = coord[1] - coord_atom[1];

	if (abs(dx) > L * r0 / 2)
		dx -= sign(dx) * L * r0;
	if (abs(dy) > L * r0 / 2)
		dy -= sign(dy) * L * r0;

	return p2(dx) + p2(dy);
}

double calc_v0(double T)
{
	return sqrt(2. * k_B * T / m);
}

double calc_vx(double v0, double r)
{
	return v0 * cos(2. * M_PI * r);
}

double calc_vy(double v0, double r)
{
	return v0 * sin(2. * M_PI * r);
}
