// TestDialog.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TestDialog.h"
#include "afxdialogex.h"


// CTestDialog �_�C�A���O

IMPLEMENT_DYNAMIC(CTestDialog, CDialogEx)

CTestDialog::CTestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDialog::IDD, pParent)
{

}

CTestDialog::~CTestDialog()
{
}

void CTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestDialog, CDialogEx)
END_MESSAGE_MAP()


// CTestDialog ���b�Z�[�W �n���h���[
