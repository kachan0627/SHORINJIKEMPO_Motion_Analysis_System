#pragma once

#include "resource.h"
// CTestDialog �_�C�A���O

class CTestDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CTestDialog();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TESTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
};
