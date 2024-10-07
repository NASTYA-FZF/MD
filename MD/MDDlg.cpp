
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
	double T = 20;
	argon = crystall(T);
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

	while (my_process->iter < 20000 && !my_process->flag_stop)
	{
		if (my_process->flag_sleep)
			Sleep(10);
		Sleep(10);
		EnterCriticalSection(&my_process->cs);
		my_process->argon.OneIterationVerle(my_process->iter);
		my_process->p = my_process->argon.SredP(my_process->iter);
		my_process->iter++;
		LeaveCriticalSection(&my_process->cs);
	}
	my_process->argon.printEnergy("energy.txt");
	my_process->argon.printPKF("pkf.txt");
	return 0;
}


void CMDDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	EnterCriticalSection(&cs);
	pic_evolution.atoms = argon.GetPos(); //считываем по кнопке координаты атомов
	str.Format(L"Итерация: %d/20 000", iter);
	str_p.Format(L"Давление (методом потока): %f", p);
	LeaveCriticalSection(&cs);
	text_iteration.SetWindowTextW(str);
	text_p.SetWindowTextW(str_p);
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
