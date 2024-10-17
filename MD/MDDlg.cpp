
// MDDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "MD.h"
#include "MDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CMDDlg



CMDDlg::CMDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MD_DIALOG, pParent)
	, T(100)
	, delta_tau(0.005)
	, iter_norm(10)
	, iter_calc(15000)
	, iter_maximum(30000)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVOLUTION, pic_evolution);
	DDX_Control(pDX, IDC_TEMP, text_temp);
	DDX_Control(pDX, IDC_ITERATION, text_iteration);
	DDX_Control(pDX, IDC_P, text_p);
	DDX_Control(pDX, IDC_PVIRIAL, text_virial);
	DDX_Control(pDX, IDC_H, text_H);
	DDX_Text(pDX, IDC_EDIT1, T);
	DDX_Text(pDX, IDC_EDIT2, delta_tau);
	DDX_Text(pDX, IDC_EDIT3, iter_norm);
	DDX_Text(pDX, IDC_EDIT4, iter_calc);
	DDX_Text(pDX, IDC_EDIT5, iter_maximum);
	DDX_Control(pDX, IDC_R2, text_R2);
}

BEGIN_MESSAGE_MAP(CMDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMDDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PAUSE, &CMDDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_STOP, &CMDDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// Обработчики сообщений CMDDlg

BOOL CMDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	SetParamMult(); //при запуске расчитываем все параметры рисовальщика
	InitializeCriticalSection(&cs);
	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CMDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CMDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMDDlg::SetParamMult()
{
	pic_evolution.maxY = L * b; 
	pic_evolution.maxX = L * r0;
	pic_evolution.r_atom = r0 * 0.5; //можно менять число, чтобы подобрать радиус нарисованных точек
	pic_evolution.otstup = 15; //подобрать 
}



void CMDDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData();
	argon = crystall(T, delta_tau, iter_norm, iter_calc);
	str.Format(L"Температура: %.f", T);
	text_temp.SetWindowTextW(str);
	
	flag_stop = false;
	flag_sleep = false;
	my_thread = CreateThread(NULL, NULL, MyThreadFunction, this, NULL, NULL);
	my_timer = SetTimer(123, 26, NULL);
}

void CMDDlg::OnCancel()
{
	// TODO: добавьте специализированный код или вызов базового класса
	DeleteCriticalSection(&cs);
	flag_stop = true;
	KillTimer(my_timer);
	Sleep(1000);
	CDialogEx::OnCancel();
}

DWORD __stdcall MyThreadFunction(LPVOID lpParam)
{
	CMDDlg* my_process = (CMDDlg*)lpParam;
	my_process->iter = 1;

	while (my_process->iter <= my_process->iter_maximum && !my_process->flag_stop)
	{
		if (my_process->flag_sleep)
			continue;
		Sleep(10);
		my_process->argon.OneIterationVerle(my_process->iter);
		EnterCriticalSection(&my_process->cs);
		my_process->iter++;
		LeaveCriticalSection(&my_process->cs);
	}
	my_process->KillTimer(my_process->my_timer);
	my_process->argon.printEnergy("energy.txt");
	my_process->argon.printPKF("pkf.txt");
	return 0;
}


void CMDDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	EnterCriticalSection(&argon.cs_setka);
	pic_evolution.atoms = argon.GetPos(); //считываем по кнопке координаты атомов
	LeaveCriticalSection(&argon.cs_setka);

	EnterCriticalSection(&cs);
	str.Format(L"Итерация: %d/%d", iter, iter_maximum);
	LeaveCriticalSection(&cs);

	EnterCriticalSection(&argon.cs_calcP);
	str_p.Format(L"Давление (методом потока): %f", argon.calcP);
	str_H.Format(L"Энтальпия: %E", argon.H);
	LeaveCriticalSection(&argon.cs_calcP);

	EnterCriticalSection(&argon.cs_calcPVirial);
	str_virial.Format(L"Давление(методом вириала): %f", argon.calcPVirial);
	LeaveCriticalSection(&argon.cs_calcPVirial);

	EnterCriticalSection(&argon.cs_calcT);
	str_T.Format(L"Температура: %.f", argon.calcT);
	LeaveCriticalSection(&argon.cs_calcT);

	EnterCriticalSection(&argon.cs_R2);
	str_R2.Format(L"Средний квадрат смещения: %E", argon.R2t);
	LeaveCriticalSection(&argon.cs_R2);

	text_iteration.SetWindowTextW(str);
	text_p.SetWindowTextW(str_p);
	text_virial.SetWindowTextW(str_virial);
	text_temp.SetWindowTextW(str_T);
	text_H.SetWindowTextW(str_H);
	text_R2.SetWindowTextW(str_R2);
	Invalidate(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


void CMDDlg::OnBnClickedPause()
{
	// TODO: добавьте свой код обработчика уведомлений
	flag_sleep = !flag_sleep;
}


void CMDDlg::OnBnClickedStop()
{
	// TODO: добавьте свой код обработчика уведомлений
	flag_stop = true;
	KillTimer(my_timer);
}
