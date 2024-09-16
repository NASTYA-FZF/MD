#pragma once
#include<math.h>
#include <vector>

//������������� ����� ��� ��� ���������, ���������, ����� ���������������� ������� ��� x y �������� �� ������� �����
typedef std::vector<double> vec2D;

//����� ����� ������ (�� �.�.�. ���������� � ��) �� ����� �� ��������� ����?
static double m = 39.948 / (6.02 * 10e26);
//����������� ���������� ����� �������� ������ (�� ����� ������� �����, ���� ��������� ������)
static double r0 = 0.382 * 10e-9;
//������� ������������� ��� �� �������
static double D = 0.0103 * 1.6 * 10e-19;
//�������� b � ��������������� ������� (���. 1, ���. 23 ���������)
static double b = sqrt(3) * r0 / 2;
//��� ���� ������ �� Lx Ly ��� ��� Lx ��� 20 �, Ly ��� 20 b
static int L = 20;

//���������, �������� ���������� �� ������
struct atom
{
	vec2D coord;
	vec2D speed;

	//������� ��������� ����� (������ ������������ �����)
	atom(double x, double y);
};

//�����, ������� ����� ������������ �������� ������
class crystall
{
	//������� ������
	std::vector<atom> setka;

public:
	//�����������, � ������� �������� ���������� ������
	crystall();

	//���������� ����� ��������� ���� ������ (��� ������������)
	std::vector<std::vector<double>> GetPos();
};