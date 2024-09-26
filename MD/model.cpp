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
	for (int y = 0; y <= L; y++)
	{
		//цикл по горизонтали (<= по той же причине)
		for (int x = 0; x <= L; x++)
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
	int num_atom = setka.size(); //число атомов

	for (int i = 0; i < num_atom; i++)
	{
		setka[i].SetSpeed(calc_vx(v0, random), calc_vy(v0, random)); //устанавливаем скорость
		sum_vx += setka[i].speed[0];
		sum_vy += setka[i].speed[1];
	}

	sum_vx /= num_atom;
	sum_vy /= num_atom;

	for (int i = 0; i < num_atom; i++)
	{
		setka[i].Minus_dv(sum_vx, sum_vy); //вычитаем вычисленные поправки
	}

	ControlSpeed(); //проверка в отладке значени€ суммарного импульма
}

void crystall::ControlSpeed()
{
	double sum_v = 0;
	for (int i = 0; i < setka.size(); i++)
	{
		sum_v += setka[i].speed[0] + setka[i].speed[1]; //суммарный импульс
	}
	sum_v /= setka.size();
}

double crystall::Kr(double r)
{
	if (r > r1)
		return p2(1 - p2((r - r1) / (r1 - r2)));
	if (r <= r1)
		return 1.;
	return 0;
}

int sign(double dx)
{
	return dx >= 0 ? 1 : -1;
}

crystall::crystall(double T)
{
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
	double mnog = -12 * D * p6(r0);

	for (int i = 0; i < N_atom; i++)
	{
		if (i != num_atom)
		{
			r = setka[num_atom].rass2_atom(setka[i].coord);

			if (r <= r2)
			{
				sum += mnog * Kr(r) * (p6(r0) / p3(r) - 1) * (setka[num_atom].coord[coord] - setka[i].coord[coord]) / p4(r);
			}
		}
	}
	return sum;
}

double crystall::len_jons_x(int num_atom)
{
	double sum = 0;

	for (int i = 0; i < setka.size(); i++)
	{
		if (i != num_atom)
		{
			double r = (setka[num_atom].coord[0] - setka[i].coord[0]) * (setka[num_atom].coord[0] - setka[i].coord[0]) +
				(setka[num_atom].coord[1] - setka[i].coord[1]) * (setka[num_atom].coord[1] - setka[i].coord[1]);

			double a1 = r0 * r0 * r0 * r0 * r0 * r0;
			double a2 = a1 / r / r / r;
			double a3 = a2 - 1;
			double a4 = a3 * (setka[num_atom].coord[0] - setka[i].coord[0]);
			double a5 = a4 / r / r / r / r;

			//sum += (pow(r0, 6) / r / r / r - 1) * (setka[num_atom].coord[0] - setka[i].coord[0]) / r / r / r / r;
			sum += (a2 - 1) * (setka[num_atom].coord[0] - setka[i].coord[0]) / r / r / r / r;
		}

	}

	return -12 * D * r0 * r0 * r0 * r0 * r0 * r0 * sum;
}

double crystall::len_jons_y(int num_atom)
{
	double sum = 0;

	for (int i = 0; i < setka.size(); i++)
	{
		if (i != num_atom)
		{
			double r = (setka[num_atom].coord[0] - setka[i].coord[0]) * (setka[num_atom].coord[0] - setka[i].coord[0]) +
				(setka[num_atom].coord[1] - setka[i].coord[1]) * (setka[num_atom].coord[1] - setka[i].coord[1]);

			sum += (pow(r0, 6) / pow(r, 3) - 1) * (setka[num_atom].coord[1] - setka[i].coord[1]) / pow(r, 4);
		}

	}

	return -12 * D * sum;
}

void crystall::verle_coord()
{
	ofstream my_out("vne.txt", ios::app);
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
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
				my_out << i << "\t" << j << "\t" << setka[i].coord[j] << endl;
			}
		}
	}
}

double crystall::verle_x(int num_atom)
{
	for (int i = 0; i < setka.size(); i++)
	{
		setka[i].Fk[0] = -len_jons_x(i);
		setka[i].coord[0] += setka[i].speed[0] * delta_t + setka[i].Fk[0] / (2 * m) * delta_t * delta_t;
	}
	return 0;
}

double crystall::verle_y(int num_atom)
{
	for (int i = 0; i < setka.size(); i++)
	{
		setka[i].Fk[1] = -len_jons_y(i);
		setka[i].coord[1] += setka[i].speed[1] * delta_t + setka[i].Fk[1] / (2 * m) * delta_t * delta_t;
	}
	return 0;
}


void crystall::verle_V()
{
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			setka[i].speed[j] += (-len_jons(i, j) + setka[i].Fk[j]) * delta_t / (2. * m);
		}
	}
}
//F_k - с предыдущего шага, его нужно записывать куда-то 
double crystall::verle_Vx(double F_k,int num_atom)
{
	for (int i = 0; i < setka.size(); i++)
	{
		setka[i].speed[0] += (-len_jons_x(i) + setka[i].Fk[0]) / (2 * m) * delta_t;
	}
	return 0;
}

double crystall::verle_Vy(double F_k,int num_atom)
{
	for (int i = 0; i < setka.size(); i++)
	{
		setka[i].speed[1] += (-len_jons_y(i) + setka[i].Fk[1]) / (2 * m) * delta_t;
	}
	return 0;
}

void crystall::OneIterationVerle(int iter)
{
	verle_coord();
	verle_V();
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

	if (dx > L * r0 / 2)
		dx -= sign(dx) * L;
	if (dy > L * r0 / 2)
		dy -= sign(dy) * L;

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
