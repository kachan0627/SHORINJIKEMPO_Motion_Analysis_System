// DIALOG1.cpp : 実装ファイル
//

#include "stdafx.h"
#include "DIALOG1.h"
#include "afxdialogex.h"


// CDIALOG1 ダイアログ

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


// CDIALOG1 メッセージ ハンドラー


void CDIALOG1::OnBnClickedButton5()
{
	MessageBox(_T("ボタン１が押されました。"));
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}

BOOL CDIALOG1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニュー項目をシステム メニューへ追加します
	// TODO: 特別な初期化を行う時はこの場所に追加してください。

	// ボタンオブジェクトの参照を得る
	


	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}


void CDIALOG1::OnBnClickedButton6()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}
