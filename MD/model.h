#pragma once
#include <math.h>
#include <vector>
#include <string>

//������������� ����� ��� ��� ���������, ���������, ����� ���������������� ������� ��� x y �������� �� ������� �����
typedef std::vector<double> vec2D;

//����� ����� ������ (�� �.�.�. ���������� � ��) �� ����� �� ��������� ����?
static double m = 39.948 / (6.02 * 1e26);
//����������� ���������� ����� �������� ������ (�� ����� ������� �����, ���� ��������� ������) � �
static double r0 = 0.382 * 1e-9;
//������� ������������� ��� �� ������� � ��
static double D = 0.0103 * 1.6 * 1e-19;
//�������� b � ��������������� ������� (���. 1, ���. 23 ���������) � �
static double b = sqrt(3) * r0 / 2;
//��� ���� ������ �� Lx Ly ��� ��� Lx ��� 20 �, Ly ��� 20 b
static int L = 20;
//���������� ��������� � �� / �
static double k_B = 1.38 * 1e-23;

//���������, �������� ���������� �� ������
struct atom
{
	vec2D coord;
	vec2D speed;

	//������� ��������� ����� (������ ������������ �����)
	atom(double x, double y);
	//��������� ��������
	void SetSpeed(double vx, double vy);
	//������ ����������
	void Minus_dv(double dvx, double dvy);
};

//�����, ������� ����� ������������ �������� ������
class crystall
{
	//������� ������
	std::vector<atom> setka;
	//���������� ��������� ����������
	void SetStartCoord();
	//���������� ��������� �������� �� �������� �����������
	void SetStartSpeed(double T);
	//�������� ���������� ��������
	void ControlSpeed();

public:
	//�����������, � ������� �������� ���������� ������
	crystall() {}
	//�����������, � ������� �������� ��������� ���������� � ��������
	crystall(double T);

	//���������� ����� ��������� ���� ������ (��� ������������)
	std::vector<std::vector<double>> GetPos();
};

//���������� ������ ��������� �������� �� �����������
double calc_v0(double T);
//���������� �������� �������� �� ��� �
double calc_vx(double v0, double r);
//���������� �������� �������� �� ��� y
double calc_vy(double v0, double r);