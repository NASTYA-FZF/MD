#pragma once
#include <gdiplus.h>
#include <vector>

// MultikMD

class MultikMD : public CStatic
{
	DECLARE_DYNAMIC(MultikMD)

public:
	MultikMD();
	virtual ~MultikMD();

protected:
	DECLARE_MESSAGE_MAP()
public:
	//надо для запуска gdi+
	UINT_PTR token;
	//коориднаты атомов
	std::vector<std::vector<double>> atoms;
	//радиус атомов, максимумы и минимумы изображения
	double r_atom, maxX, maxY, minX, minY;
	//параметры трансформации изображения
	double scaleX, scaleY, tranX, tranY;
	//флаг первого запуска, так как параметры трансформации достаточно 1 раз посчитать
	bool start;
	//вызывается для перерисовки
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	//вычисление параметров масштабирования по ширине, высоте области вывода
	void CalcMas(double width_wnd, double height_wnd);
};


