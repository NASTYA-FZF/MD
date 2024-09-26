#pragma once
#include <math.h>
#include <vector>
#include <string>
#include <fstream>

//переименовала такой тип дл€ координат, скоростей, чтобы автоматизировать формулы дл€ x y отдельно не вбивать потом
typedef std::vector<double> vec2D;

//масса атома аргона (из а.е.м. перевеодим в кг) не будет ли хранитьс€ ноль?
static double m = 39.948 / (6.02 * 1e26);
//равновесное рассто€ние между центрами атомов (по факту диаметр атома, если правильно пон€ла) в м
static double r0 = 0.382 * 1e-9;
//глубина потенциальной €мы на графике в ƒж
static double D = 0.0103 * 1.6 * 1e-19;
//величина b в кристаллической решетке (рис. 1, стр. 23 методички) в м
static double b = sqrt(3) * r0 / 2;
//это один размер по Lx Ly так как Lx это 20 а, Ly это 20 b
static int L = 20;
//посто€нна€ больцмана в ƒж /  
static double k_B = 1.38 * 1e-23;

static double delta_t = 0.005 * 2 * 1e-12;

static double r1 = 1.15 * r0;
static double r2 = 1.75 * r0;
static int S = 8;

//структура, хран€ща€ информацию об атомах
struct atom
{
	vec2D coord;
	vec2D speed;
	vec2D Fk;

	//задание координат атома (удобно использовать позже)
	atom(double x, double y);
	//установим скорость
	void SetSpeed(double vx, double vy);
	//вычтем нормировку
	void Minus_dv(double dvx, double dvy);

	double rass2_atom(vec2D coord_atom);
};

//класс, который будет обрабатывать движени€ атомов
class crystall
{
	//решетка атомов
	std::vector<atom> setka;
	int N_atom;
	double sum_V2;
	double T;
	double bet;
	//”становить начальные координаты
	void SetStartCoord();
	//установить начальные скорости по значению температуры
	void SetStartSpeed(double T);
	//проверка суммарного импульса
	void ControlSpeed();

	double Kr(double r);

	double betta();

	void perenormirovka();

public:
	//конструктор, в котором задаютс€ координаты атомов
	crystall() { srand(time(NULL)); sum_V2 = 0; }
	//конструктор, в котором задаютс€ начальные координаты и скорости
	crystall(double _T);

	//возвращает набор координат всех атомов (дл€ рисовальщика)
	std::vector<std::vector<double>> GetPos();

	//Ћен_ƒжонс проекци€ силы по x и y
	double len_jons(int num_atom, int coord);

	//расчЄт координат и скоростей
	void verle_coord();

	void verle_V();

	void OneIterationVerle(int iter);
};

//вычисление модул€ начальной скорости по температуре
double calc_v0(double T);
//вычисление проекции скорости по оси х
double calc_vx(double v0, double r);
//вычисление проекции скорости по оси y
double calc_vy(double v0, double r);

int sign(double dx);