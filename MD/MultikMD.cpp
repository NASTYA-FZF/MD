// MultikMD.cpp: файл реализации
//

#include "pch.h"
#include "MD.h"
#include "MultikMD.h"
using namespace std;
using namespace Gdiplus;

// MultikMD

IMPLEMENT_DYNAMIC(MultikMD, CStatic)

MultikMD::MultikMD()
{
	GdiplusStartupInput input;
	Status s;
	s = GdiplusStartup(&token, &input, NULL);
	if (s != Ok) MessageBox(L"s != Ok", L"Error!");
	minX = minY = 0; //всегда будут 0, так как у нас область (расчетная ячейка) не двигается
	start = true; //флаг первого запуска
}

MultikMD::~MultikMD()
{
	GdiplusShutdown(token);
}


BEGIN_MESSAGE_MAP(MultikMD, CStatic)
END_MESSAGE_MAP()



// Обработчики сообщений MultikMD




void MultikMD::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Добавьте исходный код для отображения указанного элемента

	if (start)
	{
		start = false; //больше первого запуска не будет
		CalcMas((double)lpDrawItemStruct->rcItem.right - 2. * tranX, (double)lpDrawItemStruct->rcItem.bottom - 2. * tranY);
		return;
	}

	//если пустой вектор, то выходим от греха подальше
	if (atoms.empty())
	{
		MessageBox(L"Нет атомов для отрисовки!", L"Ошибка!");
		return;
	}

	//получаем дескриптор окна (вроде так называется)
	Graphics wnd(lpDrawItemStruct->hDC);
	//заводим буфер для рисования, чтобы не сразу на экран рисовалось
	Bitmap buffer(lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &wnd);
	//заводим эту штуку для буфера
	Graphics draw_in_buffer(&buffer);

	//матрица трансформаций (преобразований) пока не работает, хрен пойми почему
	Matrix matr;
	//сдвиг небольшой, чтобы атом крайний был не в углу
	matr.Translate(tranX, tranY);
	//масштабироавние под область вывода
	matr.Scale(scaleX, scaleY);
	//применение матрицы
	draw_in_buffer.SetTransform(&matr);

	//заливка в белый
	draw_in_buffer.Clear(Color::White);

	//цвет для атомов
	SolidBrush b_atom(Color::Orange);

	//рисуем атомы
	for (int i = 0; i < atoms.size(); i++)
		draw_in_buffer.FillEllipse(&b_atom, (REAL)(atoms[i][0] - r_atom), (REAL)(atoms[i][1] - r_atom), (REAL)(2. * r_atom), (REAL)(2. * r_atom));

	//выводи из буфера
	wnd.DrawImage(&buffer, 0, 0, 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, UnitPixel);
}

void MultikMD::CalcMas(double width_wnd, double height_wnd)
{
	//вычисляем параметры масштабирования
	scaleX = width_wnd / (maxX - minX);
	scaleY = height_wnd / (maxY - minY);
}
