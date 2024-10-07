#pragma once
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

//������������� ����� ��� ��� ���������, ���������, ����� ���������������� ������� ��� x y �������� �� ������� �����
typedef std::vector<double> vec2D;

//����� ����� ������ (�� �.�.�. ���������� � ��) �� ����� �� ��������� ����?
static double m = 66.354 * 1e-27;
//����������� ���������� ����� �������� ������ (�� ����� ������� �����, ���� ��������� ������) � �
static double r0 = 0.382 * 1e-9;
//������� ������������� ��� �� ������� � ��
static double eV = 1.6 * 1e-19;
static double D = 0.0103 * eV;
//�������� b � ��������������� ������� (���. 1, ���. 23 ���������) � �
static double b = sqrt(3) * r0 / 2;
//��� ���� ������ �� Lx Ly ��� ��� Lx ��� 20 �, Ly ��� 20 b
static int L = 20;
//���������� ��������� � �� / �
static double k_B = 1.38 * 1e-23;

static double delta_t = 0.005 * 2 * 1e-12;

static double r1 = 1.15 * r0;
static double r2 = 1.75 * r0;
static int S = 8;

//���������, �������� ���������� �� ������
struct atom
{
	vec2D coord;
	vec2D speed;
	vec2D Fk_prev;
	vec2D Fk_cur;

	//������� ��������� ����� (������ ������������ �����)
	atom(double x, double y);
	//��������� ��������
	void SetSpeed(double vx, double vy);
	//������ ����������
	void Minus_dv(double dvx, double dvy);

	double rass2_atom(vec2D coord_atom);
};

//�����, ������� ����� ������������ �������� ������
class crystall
{
	//������� ������
	std::vector<atom> setka;
	int N_atom;
	double sum_V2;
	double T;
	double bet, ek, ep;
	vec2D p;
	double virial;

	std::vector<double> energyE, energyV, energyEk;
	//���������� ��������� ����������
	void SetStartCoord();
	//���������� ��������� �������� �� �������� �����������
	void SetStartSpeed(double T);
	//�������� ���������� ��������
	void ControlSpeed();

	double Kr(double r);

	double betta();

	void perenormirovka();

public:
	//�����������, � ������� �������� ���������� ������
	crystall() { srand(time(NULL)); sum_V2 = 0; N_atom = setka.size(); }
	//�����������, � ������� �������� ��������� ���������� � ��������
	crystall(double _T);

	//���������� ����� ��������� ���� ������ (��� ������������)
	std::vector<std::vector<double>> GetPos();

	//���_����� �������� ���� �� x � y
	void len_jons();

	//������ ��������� � ���������
	void verle_coord();

	void verle_V();

	void OneIterationVerle(int iter);

	void printEnergy(std::string fileName);

	double calc_Ep();

	std::vector<double> PKF(std::string fileName);

	void printPKF(std::string fileName);

	double SredP(int iter);

	double pVirial(int iter);
};

//���������� ������ ��������� �������� �� �����������
double calc_v0(double T);
//���������� �������� �������� �� ��� �
double calc_vx(double v0, double r);
//���������� �������� �������� �� ��� y
double calc_vy(double v0, double r);

int sign(double dx);