#pragma once


// CDIALOG1 �_�C�A���O

#include "resource.h"

class CDIALOG1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDIALOG1)

public:
	CDIALOG1(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDIALOG1();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton5();

	afx_msg void OnBnClickedButton6();
};
