// DIALOG1.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "DIALOG1.h"
#include "afxdialogex.h"


// CDIALOG1 �_�C�A���O

IMPLEMENT_DYNAMIC(CDIALOG1, CDialogEx)

CDIALOG1::CDIALOG1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDIALOG1::IDD, pParent)	
{
	OnInitDialog();
}

CDIALOG1::~CDIALOG1()
{
}

void CDIALOG1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDIALOG1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON5, &CDIALOG1::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDIALOG1::OnBnClickedButton6)
END_MESSAGE_MAP()


// CDIALOG1 ���b�Z�[�W �n���h���[


void CDIALOG1::OnBnClickedButton5()
{
	MessageBox(_T("�{�^���P��������܂����B"));
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}

BOOL CDIALOG1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���ڂ��V�X�e�� ���j���[�֒ǉ����܂�
	// TODO: ���ʂȏ��������s�����͂��̏ꏊ�ɒǉ����Ă��������B

	// �{�^���I�u�W�F�N�g�̎Q�Ƃ𓾂�
	


	return TRUE;  // TRUE ��Ԃ��ƃR���g���[���ɐݒ肵���t�H�[�J�X�͎����܂���B
}


void CDIALOG1::OnBnClickedButton6()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}
