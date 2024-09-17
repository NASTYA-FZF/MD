#include "pch.h"
#define _USE_MATH_DEFINES
#define random ((double)rand() / RAND_MAX)
#include "model.h"
using namespace std;

void crystall::SetStartCoord()
{
	//���� �� ��������� (<= ������ ��� � �����, ��� �� ������ ������� ���� ������ ���� �������)
	for (int y = 0; y <= L; y++)
	{
		//���� �� ����������� (<= �� ��� �� �������)
		for (int x = 0; x <= L; x++)
		{
			//���� ������� �� ������� �� 2 ����� 1, �� �������� ���� (����������� �� � �����, ����� �� ������� �� �������������)
			if (y % 2 == 1 && x < L)
				setka.push_back(atom(0.5 * r0 + x * r0, y * b));
			if (y % 2 == 0)
				setka.push_back(atom(x * r0, y * b)); //������ ����
		}
	}
}

void crystall::SetStartSpeed(double T)
{
	double v0 = calc_v0(T); //������ �������� 
	double sum_vx = 0, sum_vy = 0; //��������� ����� ��������� �� ���������
	int num_atom = setka.size(); //����� ������

	for (int i = 0; i < num_atom; i++)
	{
		setka[i].SetSpeed(calc_vx(v0, random), calc_vy(v0, random)); //������������� ��������
		sum_vx += setka[i].speed[0];
		sum_vy += setka[i].speed[1];
	}

	sum_vx /= num_atom;
	sum_vy /= num_atom;

	for (int i = 0; i < num_atom; i++)
	{
		setka[i].Minus_dv(sum_vx, sum_vy); //�������� ����������� ��������
	}

	ControlSpeed(); //�������� � ������� �������� ���������� ��������
}

void crystall::ControlSpeed()
{
	double sum_v = 0;
	for (int i = 0; i < setka.size(); i++)
	{
		sum_v += setka[i].speed[0] + setka[i].speed[1]; //��������� �������
	}
	sum_v /= setka.size();
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
		res[i].insert(res[i].begin(), setka[i].coord.begin(), setka[i].coord.end()); //�������� ���������� ������

	return res;
}

atom::atom(double x, double y)
{
	coord.insert(coord.begin(), { x, y });
	speed.insert(speed.begin(), { 0, 0 });
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
