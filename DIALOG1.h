#pragma once


// CDIALOG1 ダイアログ

#include "resource.h"

class CDIALOG1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDIALOG1)

public:
	CDIALOG1(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDIALOG1();

// ダイアログ データ
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton5();

	afx_msg void OnBnClickedButton6();
};
