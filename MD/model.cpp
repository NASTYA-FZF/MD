#include "pch.h"
#include "model.h"
using namespace std;

crystall::crystall()
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
}
