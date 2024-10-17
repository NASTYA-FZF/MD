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
	//цикл по вертикали (<= потому что я думаю, что на второй границе тоже должны быть частицы)
	for (int y = 0; y < L; y++)
	{
		//цикл по горизонтали (<= по той же причине)
		for (int x = 0; x < L; x++)
		{
			//если остаток от деления на 2 будет 1, то нечетные ряды (ограничение по х нужно, чтобы не вылезло за прямоугольник)
			if (y % 2 == 1 && x < L)
				setka.push_back(atom(0.5 * r0 + x * r0, y * b));
			if (y % 2 == 0)
				setka.push_back(atom(x * r0, y * b)); //четные ряды
		}
	}

	N_atom = setka.size();
}

void crystall::SetStartSpeed(double T)
{
	double v0 = calc_v0(T); //модуль скорости 
	double sum_vx = 0, sum_vy = 0; //вычисляем суммы импульсов по проекциям
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

	//ControlSpeed(); //проверка в отладке значения суммарного импульма
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
	r = sqrt(r);
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

	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			setka[i].speed[j] *= bet;
		}
	}
	LeaveCriticalSection(&cs_setka);
}

int sign(double dx)
{
	return dx >= 0 ? 1 : -1;
}

crystall::crystall(double _T, double d_t, int iter_norm, int it)
{
	InitializeCriticalSection(&cs_setka);
	InitializeCriticalSection(&cs_calcP);
	InitializeCriticalSection(&cs_calcPVirial);
	InitializeCriticalSection(&cs_calcT);
	InitializeCriticalSection(&cs_H);
	InitializeCriticalSection(&cs_R2);
	T = _T;
	sum_V2 = 0;
	bet = 0;
	p = vector<double>(2, 0);
	virial = 0;
	SetStartCoord();
	SetStartSpeed(T);
	delta_t = d_t * 2 * 1e-12;
	S = iter_norm;
	iter_calc = it;
}

crystall::~crystall()
{
	DeleteCriticalSection(&cs_setka);
	DeleteCriticalSection(&cs_calcP);
	DeleteCriticalSection(&cs_calcPVirial);
	DeleteCriticalSection(&cs_calcT);
	DeleteCriticalSection(&cs_H);
	DeleteCriticalSection(&cs_R2);
}

std::vector<std::vector<double>> crystall::GetPos()
{
	int size = setka.size();
	vector<vector<double>> res(size);

	for (int i = 0; i < size; i++)
		res[i].insert(res[i].begin(), setka[i].coord.begin(), setka[i].coord.end()); //копируем координаты атомов

	return res;
}

void crystall::len_jons()
{
	double r06 = p6(r0);
	double dx, dy, rPow2, r8, r6, r;
	double pered_dx;
	double summaX = 0;
	double summaY = 0;
	double rdiff2 = (r1 - r2) * (r1 - r2);
	double rdiffr;
	double rdiffr2;
	double Kr;
	double mnog = 12. * D * r06;

	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		setka[i].Fk_cur[0] = setka[i].Fk_cur[1] = 0;
	}
	LeaveCriticalSection(&cs_setka);

	for (int at = 0; at < N_atom; at++)
	{
		for (int i = at + 1; i < N_atom; i++)
		{
			EnterCriticalSection(&cs_setka);
			dx = setka[at].coord[0] - setka[i].coord[0];
			dy = setka[at].coord[1] - setka[i].coord[1];
			LeaveCriticalSection(&cs_setka);

			if (abs(dx) > 0.5 * L * r0)
				dx -= sign(dx) * L * r0;
			if (abs(dy) > 0.5 * L * r0)
				dy -= sign(dy) * L * r0;

			rPow2 = p2(dx) + p2(dy);
			r = sqrt(rPow2);
			r6 = rPow2 * rPow2 * rPow2;
			r8 = rPow2 * rPow2 * rPow2 * rPow2;

			if (r > r2) continue;
			if (r > r1)
			{
				rdiffr = r - r1;
				rdiffr2 = rdiffr * rdiffr;
				Kr = (1 - rdiffr2 / rdiff2) * (1 - rdiffr2 / rdiff2);
			}
			else
				Kr = 1;

			pered_dx = Kr * (r06 / r6 - 1) / r8;

			summaX = pered_dx * dx;
			summaY = pered_dx * dy;

			EnterCriticalSection(&cs_setka);
			setka[at].Fk_cur[0] += mnog * summaX;
			setka[at].Fk_cur[1] += mnog * summaY;
			setka[i].Fk_cur[0] -= mnog * summaX;
			setka[i].Fk_cur[1] -= mnog * summaY;
			LeaveCriticalSection(&cs_setka);
		}
	}
}

void crystall::verle_coord()
{
	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			setka[i].Fk_prev[j] = setka[i].Fk_cur[j];
			setka[i].coord[j] += setka[i].speed[j] * delta_t + setka[i].Fk_prev[j] * p2(delta_t) / (2. * m);

			if (setka[i].coord[j] < 0)
			{
				p[j] -= m * setka[i].speed[j];
				setka[i].coord[j] += L * r0;
			}
			else
			{
				if (j == 0 && setka[i].coord[j] > L * r0)
				{
					p[j] += m * setka[i].speed[j];
					setka[i].coord[j] -= L * r0;
				}
				else
					if (j == 1 && setka[i].coord[j] > L * b)
					{
						p[j] += m * setka[i].speed[j];
						setka[i].coord[j] -= L * r0;
					}
			}
		}
	}
	LeaveCriticalSection(&cs_setka);
}

void crystall::verle_V()
{
	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			virial += setka[i].coord[0] * setka[i].Fk_cur[0] + setka[i].coord[1] * setka[i].Fk_cur[1];
			setka[i].speed[j] += (setka[i].Fk_cur[j] + setka[i].Fk_prev[j]) * delta_t / (2. * m);

			ek_iter += m * p2(setka[i].speed[j]) / 2;
			ek_T += m * p2(setka[i].speed[j]) / 2;
			ek += m * p2(setka[i].speed[j]) / 2;
			sum_V2 += m * p2(setka[i].speed[j]) / S;
		}
	}
	LeaveCriticalSection(&cs_setka);
}

void crystall::OneIterationVerle(int iter)
{
	if (iter == 1)
	{
		len_jons();
		ep = ek = 0;
	}

	ek_T = 0;

	verle_coord();
	len_jons();
	verle_V();
	calc_Ep();

	if (iter % 100 == 0)
	{
		energyEk.push_back(0.01 * ek / eV);
		energyV.push_back(0.01 * ep / eV);
		energyE.push_back(energyEk.back() + energyV.back());
		ep = ek = 0;
	}

	if (iter % S == 0)
	{
		perenormirovka();
	}

	if (iter == iter_calc)
	{
		R2_iter = R2t = ep_iter = ek_iter = p[0] = p[1] = virial = 0;
		RememberCoord();
	}

	if (iter > iter_calc)
	{
		razn_iter = iter - iter_calc;
		EnterCriticalSection(&cs_calcP);
		calcP = SredP(razn_iter);
		H = (ep_iter + ek_iter) / razn_iter + calcP * p2(L) * r0 * b;
		LeaveCriticalSection(&cs_calcP);
		EnterCriticalSection(&cs_calcPVirial);
		calcPVirial = pVirial(razn_iter);
		LeaveCriticalSection(&cs_calcPVirial);

		//R2_iter += CalcR2();
		EnterCriticalSection(&cs_R2);
		R2t = CalcR2();
		LeaveCriticalSection(&cs_R2);
	}
	EnterCriticalSection(&cs_calcT);
	calcT = ek_T / (k_B * N_atom);
	LeaveCriticalSection(&cs_calcT);
}

void crystall::printEnergy(string fileName)
{
	ofstream file_out(fileName);
	string enE, enEk, enEp;
	for (int i = 0; i < energyE.size(); i++)
	{
		enE = to_string(energyE[i]);
		enEk = to_string(energyEk[i]);
		enEp = to_string(energyV[i]);

		replace(enE.begin(), enE.end(), '.', ',');
		replace(enEk.begin(), enEk.end(), '.', ',');
		replace(enEp.begin(), enEp.end(), '.', ',');

		file_out << (i + 1) * 100 << "\t" << enEk << "\t" << enEp << "\t" << enE << endl;
	}

	file_out.close();
}

double crystall::calc_Ep()
{
	double r06 = p6(r0);
	double r012 = r06 * r06;
	double r12;
	double r6;
	double dx;
	double dy;
	double rPow2;
	double r;
	double rdiffr;
	double rdiffr2;
	double rdiff2 = (r1 - r2) * (r1 - r2);
	double Kr;

	for (int i = 0; i < N_atom; i++)
	{
		for (int j = i + 1; j < N_atom; j++)
		{
			EnterCriticalSection(&cs_setka);
			dx = setka[i].coord[0] - setka[j].coord[0];
			dy = setka[i].coord[1] - setka[j].coord[1];
			LeaveCriticalSection(&cs_setka);

			if (abs(dx) > 0.5 * L * r0)
				dx -= sign(dx) * L * r0;
			if (abs(dy) > 0.5 * L * r0)
				dy -= sign(dy) * L * r0;

			rPow2 = p2(dx) + p2(dy);
			r = sqrt(rPow2);
			r6 = rPow2 * rPow2 * rPow2;
			r12 = r6 * r6;

			if (r > r2) continue;
			if (r > r1)
			{
				rdiffr = r - r1;
				rdiffr2 = rdiffr * rdiffr;
				Kr = (1 - rdiffr2 / rdiff2) * (1 - rdiffr2 / rdiff2);
			}
			else
				Kr = 1;

			ep += Kr * D * (r012 / r12 - 2. * r06 / r6);
			ep_iter += Kr * D * (r012 / r12 - 2. * r06 / r6);
		}
	}

	return ep;
}

std::vector<double> crystall::PKF(std::string fileName)
{
	vector<double> my_pkf(100);
	int pkf_atom = 0;
	double k = 1.;
	int L4 = L / 4;
	int L34 = 3 * L / 4;
	double part_r0 = 0.05;
	double dr = part_r0 * r0;
	double r = 0;
	int num_kol = 0;
	double rmax = 5 * r0;

	for (int i = 0; i < N_atom; i++)
	{
		EnterCriticalSection(&cs_setka);
		if (setka[i].coord[0] < L4 * r0 || setka[i].coord[0] > L34 * r0) continue;
		if (setka[i].coord[1] < L4 * b || setka[i].coord[1] > L34 * b) continue;
		LeaveCriticalSection(&cs_setka);

		for (int j = 0; j < N_atom; j++)
		{
			if (i == j) continue;

			EnterCriticalSection(&cs_setka);
			r = sqrt(p2(setka[i].coord[0] - setka[j].coord[0]) + p2(setka[i].coord[1] - setka[j].coord[1]));
			LeaveCriticalSection(&cs_setka);
			if (r > rmax) continue;
			pkf_atom++;
			num_kol = floor(r / dr);
			my_pkf[num_kol] += k / (2. * M_PI * (num_kol + 1) * part_r0);
		}
	}
	
	/*for (int i = 0; i < my_pkf.size(); i++)
		my_pkf[i] /= pkf_atom;*/

	return my_pkf;
}

void crystall::printPKF(std::string fileName)
{
	auto _pkf = PKF("null");
	double dr = 0.05;
	ofstream file_out(fileName);
	string str_pkf, str_dr;
	for (int i = 0; i < _pkf.size(); i++)
	{
		str_pkf = to_string(_pkf[i]);
		str_dr = to_string((i + 1) * dr);

		replace(str_pkf.begin(), str_pkf.end(), '.', ',');
		replace(str_dr.begin(), str_dr.end(), '.', ',');

		file_out << str_dr << "\t" << str_pkf << endl;
	}

	file_out.close();
}

atom::atom(double x, double y)
{
	coord.insert(coord.begin(), { x, y });
	speed.insert(speed.begin(), { 0, 0 });
	Fk_cur = vector<double>(2, 0);
	Fk_prev = vector<double>(2, 0);
}

double crystall::SredP(int iter)
{
	return (p[0] / (L * r0) + p[1] / (L * b)) / (iter * delta_t);
}

double crystall::pVirial(int iter)
{
	return (N_atom * k_B * T) / (L * L * r0 * b) + 0.5 * virial / (L * L * r0 * b * iter);
}

void crystall::RememberCoord()
{
	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		setka[i].x1 = setka[i].coord[0];
		setka[i].y1 = setka[i].coord[1];
	}
	LeaveCriticalSection(&cs_setka);
}

double crystall::CalcR2()
{
	double res = 0;

	EnterCriticalSection(&cs_setka);
	for (int i = 0; i < N_atom; i++)
	{
		res += p2(setka[i].coord[0] - setka[i].x1) + p2(setka[i].coord[1] - setka[i].y1);
	}
	LeaveCriticalSection(&cs_setka);

	return res / N_atom;
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
