
// MDDlg.h: файл заголовка
//

#pragma once
#include "model.h"
#include "MultikMD.h"

// Диалоговое окно CMDDlg
class CMDDlg : public CDialogEx
{
// Создание
public:
	CMDDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//рисовальщик
	MultikMD pic_evolution;
	//объект нашего класса
	crystall argon;
	//установка параметров рисовальщика (максимумов, минимумов, радиуса)
	void SetParamMult();
	afx_msg void OnBnClickedOk();
};
