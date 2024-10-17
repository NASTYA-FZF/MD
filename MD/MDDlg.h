﻿
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
	int iter;
	double p;
	double p_virial;
	afx_msg void OnBnClickedOk();
	CRITICAL_SECTION cs;
	HANDLE my_thread;
	bool flag_sleep, flag_stop;
	UINT_PTR my_timer;
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	CStatic text_temp;
	CString str;
	CString str_p;
	CString str_virial;
	CString str_T;
	CString str_H;
	CString str_R2;
	CStatic text_iteration;
	CStatic text_p;
	CStatic text_virial;
	CStatic text_H;
	double T;
	double delta_tau;
	int iter_norm;
	int iter_calc;
	int iter_maximum;
	CStatic text_R2;
};

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
