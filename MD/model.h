#pragma once
#include <math.h>
#include <vector>
#include <string>

//переименовала такой тип для координат, скоростей, чтобы автоматизировать формулы для x y отдельно не вбивать потом
typedef std::vector<double> vec2D;

//масса атома аргона (из а.е.м. перевеодим в кг) не будет ли храниться ноль?
static double m = 39.948 / (6.02 * 1e26);
//равновесное расстояние между центрами атомов (по факту диаметр атома, если правильно поняла) в м
static double r0 = 0.382 * 1e-9;
//глубина потенциальной ямы на графике в Дж
static double D = 0.0103 * 1.6 * 1e-19;
//величина b в кристаллической решетке (рис. 1, стр. 23 методички) в м
static double b = sqrt(3) * r0 / 2;
//это один размер по Lx Ly так как Lx это 20 а, Ly это 20 b
static int L = 20;
//постоянная больцмана в Дж / К
static double k_B = 1.38 * 1e-23;

//структура, хранящая информацию об атомах
struct atom
{
	vec2D coord;
	vec2D speed;

	//задание координат атома (удобно использовать позже)
	atom(double x, double y);
	//установим скорость
	void SetSpeed(double vx, double vy);
	//вычтем нормировку
	void Minus_dv(double dvx, double dvy);
};

//класс, который будет обрабатывать движения атомов
class crystall
{
	//решетка атомов
	std::vector<atom> setka;
	//Установить начальные координаты
	void SetStartCoord();
	//установить начальные скорости по значению температуры
	void SetStartSpeed(double T);
	//проверка суммарного импульса
	void ControlSpeed();

public:
	//конструктор, в котором задаются координаты атомов
	crystall() {}
	//конструктор, в котором задаются начальные координаты и скорости
	crystall(double T);

	//возвращает набор координат всех атомов (для рисовальщика)
	std::vector<std::vector<double>> GetPos();
};

//вычисление модуля начальной скорости по температуре
double calc_v0(double T);
//вычисление проекции скорости по оси х
double calc_vx(double v0, double r);
//вычисление проекции скорости по оси y
double calc_vy(double v0, double r);