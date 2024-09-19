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
		//вычисление коэффициентов масштабирования
		CalcMas((double)lpDrawItemStruct->rcItem.right - 2. * otstup, (double)lpDrawItemStruct->rcItem.bottom - 2. * otstup);
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

	//заливка в белый (делаем до преобразований, а то заливка съедет тоже)
	draw_in_buffer.Clear(Color::White);

	//матрица трансформаций (преобразований) пока не работает, хрен пойми почему
	Matrix matr;
	//сдвиг небольшой, чтобы атом крайний был не в углу (не совсем работало)
	matr.Translate(otstup, otstup);
	//масштабироавние под область вывода
	matr.Scale(scaleX, scaleY);

	//цвет для атомов
	SolidBrush b_atom(Color::Orange);
	PointF points[2];
	//рисуем атомы
	for (int i = 0; i < atoms.size(); i++)
	{
		//запоминаем левую верхную точку прямоугольника, ограничивающего эллипс (круг)
		points[0] = PointF((REAL)(atoms[i][0] - r_atom),
			(REAL)(atoms[i][1] - r_atom));
		//запоминаем правую нижнюю точку прямоугольника
		points[1] = PointF((REAL)(atoms[i][0] + r_atom),
			(REAL)(atoms[i][1] + r_atom));
		//преобразуем координаты
		matr.TransformPoints(points, 2);
		//рисуем точку
		draw_in_buffer.FillEllipse(&b_atom, points[0].X,
			points[0].Y, points[1].X - points[0].X, points[1].Y - points[0].Y);
	}

	//выводи из буфера
	wnd.DrawImage(&buffer, 0, 0, 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, UnitPixel);
}

void MultikMD::CalcMas(double width_wnd, double height_wnd)
{
	//вычисляем параметры масштабирования
	scaleX = width_wnd / (maxX - minX);
	scaleY = height_wnd / (maxY - minY);
}
