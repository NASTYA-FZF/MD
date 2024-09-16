
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
}

BEGIN_MESSAGE_MAP(CMDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMDDlg::OnBnClickedOk)
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
	pic_evolution.r_atom = r0 * 0.3; //можно менять число, чтобы подобрать радиус нарисованных точек
	pic_evolution.otstup = 10; //подобрать (еще раз посмотреть)
}



void CMDDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	pic_evolution.atoms = argon.GetPos(); //считываем по кнопке координаты атомов
	Invalidate(FALSE);
}
