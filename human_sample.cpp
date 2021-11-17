
#ifdef  WIN32
#include <windows.h>
#endif


// GLUT を使用
#include <gl/freeglut.h>
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color3.h>

// STL を使用
#include <vector>
#include <string>

#include<iostream>
#include <fstream>
#include <tchar.h>
#include <wingdi.h>
#include <locale.h>
#include <stdio.h>
#include "resource.h"

using namespace std;

// 標準算術関数・定数の定義
#define  _USE_MATH_DEFINES
#include <math.h>

#include <string.h>

// ライブラリ・クラス定義の読み込み
#include "SimpleHuman.h"
#include "SimpleHumanGLUT.h"
#include "BVH.h"
#include <algorithm>
#define M 5      // 予測曲線の次数
#define	SpineThreshold  19
#define	Spine1Threshold  18
#define	NeckThreshold  22
#define	ShoulderThreshold  51
#define	ArmThreshold  34
#define	ForeArmThreshold 40
#define	UplegThreshold  29
#define	LegThreshold  50
#define	FootThreshold  51
#define Alpha 0.55
#define Beta 0.45

// シンボル定義及びマクロ
#define WINDOW_WIDTH    80
#define WINDOW_HEIGHT   60

//  インスタンス（グローバル変数）
HINSTANCE hInst;

////  ウィンドウプロシージャのコールバック関数
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgWndProc(HWND, UINT, WPARAM, LPARAM);

//LRESULT CALLBACK eMainWindowProc(
//	HWND   hwnd   // handle to window
//	, UINT   uMsg   // message identifier
//	, WPARAM wParam // first message parameter
//	, LPARAM lParam // second message parameter
//	);
// BVHファイルの位置情報に適用するスケーリング比率（デフォルトでは cm→m への変換）
const float  bvh_scale = 0.01f;
///////////////////////////////////////////////////////////////////////////////
//
//  動作再生アプリケーション
//


//
//  動作再生アプリケーションクラス
//
class  MotionPlaybackApp : public GLUTBaseApp
{
protected:
	// キャラクタ情報

	// キャラクタの骨格
	Skeleton * body;
	Skeleton * body_a;
	Skeleton * body_m;
	// キャラクタの姿勢
	Posture *  curr_posture;
	Posture *  curr_posture_a;
	Posture *  curr_posture_m;

protected:
	// 動作再生のための変数

	// 動作データ
	Motion *   motion;
	Motion *   motion_a;
	Motion *   motion_m;
	// アニメーション再生中かどうかを表すフラグ
	bool       on_animation;

	// アニメーションの再生時間
	float      animation_time;
	float      animation_time_m;
	float      animation_time_a;
	// アニメーションの再生速度
	float      animation_speed;
	float      animation_speed_m;
	float      animation_speed_a;
	// 現在の表示フレーム番号
	int        frame_no;
	int        frame_no_m;
	int        frame_no_a;
	vector<int> framecom;
	vector<Point3f> end_site_master;
	vector<Point3f> end_site;
public:
	// コンストラクタ
	MotionPlaybackApp();

	
	// デストラクタ
	virtual ~MotionPlaybackApp();

public:
	// イベント処理

	//  初期化
	virtual void  Initialize();

	//  開始・リセット
	virtual void  Start();
	virtual void  Start(int a, int b,int c,int d);
	//  画面描画
	virtual void  Display();

	// キーボードのキー押下
	virtual void  Keyboard(unsigned char key, int mx, int my);

	// アニメーション処理
	virtual void  Animation(float delta);
	virtual void  Animation(float delta,int a,int b,int c,int d);
	virtual void  AnimationStartPoint(int a);
	virtual void  AnimationStartPoint_m(int a);
	virtual void  AnimationStartPoint_a(int a);
public:
	// 補助処理

	// BVH動作ファイルの読み込み、骨格・姿勢の初期化
	void  LoadBVH(const char * file_name);

	// ファイルダイアログを表示してBVH動作ファイルを選択・読み込み
	void  OpenNewBVH();

	// BVH動作ファイルの読み込み、骨格・姿勢の初期化
	void  LoadBVHMASTER(const char * file_name);

	// ファイルダイアログを表示してBVH動作ファイルを選択・読み込み
	void  OpenNewBVHMASTER();
	// BVH動作ファイルの読み込み、骨格・姿勢の初期化
	void  LoadBVHADVICE(const char * file_name);

	// ファイルダイアログを表示してBVH動作ファイルを選択・読み込み
	void  OpenNewBVHADVICE();
};


//
//  コンストラクタ
//
MotionPlaybackApp::MotionPlaybackApp()
{
	app_name = "Motion Playback";
	body = NULL;
	body_m = NULL;
	body_a= NULL;
	curr_posture = NULL;
	curr_posture_m = NULL;
	curr_posture_a = NULL;
	motion = NULL;
	motion_m = NULL;
	motion_a = NULL;
	on_animation = true;
	animation_time = 0.0f;
	animation_time_m = 0.0f;
	animation_time_a = 0.0f;
	animation_speed = 1.0f;
	animation_speed_m = 1.0f;
	animation_speed_a = 1.0f;
	frame_no = 0; frame_no_m = 0; frame_no_a = 0;
}


//
//  デストラクタ
//
MotionPlaybackApp::~MotionPlaybackApp()
{
	if (motion)
		delete  motion;
	if (curr_posture)
		delete  curr_posture;
	if (body)
		delete  body;
	if (motion_m)
		delete  motion_m;
	if (curr_posture_m)
		delete  curr_posture_m;
	if (body_m)
		delete  body_m;
	if (motion_a)
		delete  motion_a;
	if (curr_posture_a)
		delete  curr_posture_a;
	if (body_a)
		delete  body_a;
}


//
//  初期化
//
void  MotionPlaybackApp::Initialize()
{
	GLUTBaseApp::Initialize();

	// サンプルBVH動作データを読み込み
	LoadBVHMASTER("motion_data\\master_data.bvh");
	LoadBVHADVICE("motion_data\\master_data_advice.bvh");
	if (!motion_m){
		MessageBox(NULL, "'m'ボタンを押してマスターデータを選択してください", "警告", MB_OK | MB_ICONINFORMATION);
	}
}


//
//  開始・リセット
//
void  MotionPlaybackApp::Start()
{
	GLUTBaseApp::Start();

	on_animation = true;
	animation_time = 0.0f;
	frame_no = 0;

	Animation(0.0f);
}

void  MotionPlaybackApp::Start(int a,int b,int c,int d)
{
	GLUTBaseApp::Start();

	on_animation = true;
	animation_time = 0.0f;
	frame_no = a;

	Animation(0.0f,a,b,c,d);
}

//
//  画面描画
//
void  MotionPlaybackApp::Display()
{
	GLUTBaseApp::Display();

	// キャラクタを描画
	if (curr_posture)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		DrawPosture(*curr_posture);
		DrawPostureShadow(*curr_posture, shadow_dir, shadow_color);
	}

	// 現在のモード、時間・フレーム番号を表示
	DrawTextInformation(0, "Motion Playback");
	char  message[64];
	if (motion)
		sprintf(message, "%.2f (%d)", animation_time, frame_no);
	else
		sprintf(message, "Press 'L' key to Load a BVH file");
	DrawTextInformation(1, message);
}


//
//  キーボードのキー押下
//
void  MotionPlaybackApp::Keyboard(unsigned char key, int mx, int my)
{
	GLUTBaseApp::Keyboard(key, mx, my);
	GLUTBaseApp::Keyboard2(key, mx, my);
	GLUTBaseApp::Keyboard3(key, mx, my);
	// s キーでアニメーションの停止・再開
	if (key == 's'){
		on_animation = !on_animation;
		animation_speed = 1.0f;
		animation_speed_m = 1.0f;
		animation_speed_a = 1.0f;
	}
	// w キーでアニメーションの再生速度を変更
	if (key == 'w'){
		animation_speed = (animation_speed == 1.0f) ? 0.1f : 1.0f;
		animation_speed_m = (animation_speed_m == 1.0f) ? 0.1f : 1.0f;
		animation_speed_a = (animation_speed_m == 1.0f) ? 0.1f : 1.0f;
	}
	// n キーで次のフレーム
	if ((key == 'n') && !on_animation && motion)
	{
		on_animation = true;
		Animation(motion->interval);
		on_animation = false;
	}

	// p キーで前のフレーム
	if ((key == 'p') && !on_animation && motion && (frame_no > 0))
	{
		on_animation = true;
		Animation(-motion->interval);
		on_animation = false;
	}

	// l キーで再生動作の変更
	if (key == 'l')
	{
		// ファイルダイアログを表示してBVHファイルを選択・読み込み
		OpenNewBVH();
		animation_speed_m = 1.0f;
		animation_time_m = 0.0f;
		animation_speed = 1.0f;
		animation_speed_a = 1.0f;
		animation_time_a = 0.0f;
		MessageBox(NULL, "'c'ボタンを押すと動作評価が始まります", "手順", MB_OK | MB_ICONINFORMATION);
		
	}

	if (key == 'm')
	{
		// ファイルダイアログを表示してBVHファイルを選択・読み込み
		OpenNewBVHMASTER();
		animation_speed = 1.0f;
		animation_speed_m = 1.0f;
		animation_speed_a = 1.0f;
		animation_time = 0.0f;
	}

	

}


//
//  アニメーション処理
//
void  MotionPlaybackApp::Animation(float delta)
{
	// アニメーション再生中でなければ終了
	if (!on_animation)
		return;

	// 動作データが読み込まれていなければ終了
	if (!motion_m)
		return;
	/*if (!motion_m)
		return;*/

	animation_time_m += delta * animation_speed_m;
	if (animation_time_m > motion_m->GetDuration())//最後のフレームの場合
		animation_time_m -= motion_m->GetDuration();

	// 現在のフレーム番号を計算
	frame_no_m = animation_time_m / motion_m->interval;

	// 動作データから現在時刻の姿勢を取得
	motion_m->GetPosture(animation_time_m, *curr_posture_m);

	if (!motion)
		return;
	// 時間を進める
	animation_time += delta * animation_speed;
	if (animation_time > motion->GetDuration())//最後のフレームの場合
		animation_time -= motion->GetDuration();

	// 現在のフレーム番号を計算
	frame_no = animation_time / motion->interval;

	// 動作データから現在時刻の姿勢を取得
	motion->GetPosture(animation_time, *curr_posture);
	


	// 動作データが読み込まれていなければ終了
	if (!motion_a)
		return;
	/*if (!motion_m)
	return;*/

	animation_time_a += delta * animation_speed_a;
	if (animation_time_a > motion_a->GetDuration())//最後のフレームの場合
		animation_time_a -= motion_a->GetDuration();

	// 現在のフレーム番号を計算
	frame_no_a = animation_time_a / motion_a->interval;

	// 動作データから現在時刻の姿勢を取得
	motion_a->GetPosture(animation_time_a, *curr_posture_a);

}

void  MotionPlaybackApp::Animation(float delta,int a,int b,int c,int d)
{
	// アニメーション再生中でなければ終了
	if (!on_animation)
		return;
	// 動作データが読み込まれていなければ終了
	if (!motion_m)
		return;
	/*if (!motion_m)
	return;*/
	animation_speed_m = 1.0f;
	// 時間を進める
	animation_time_m += delta * animation_speed_m;
	if (animation_time_m > motion_m->interval*d)//最後のフレームの場合
		AnimationStartPoint_m(c);
	// 現在のフレーム番号を計算
	frame_no_m = (animation_time_m) / motion_m->interval;

	// 動作データから現在時刻の姿勢を取得
	motion_m->GetPosture(animation_time_m, *curr_posture_m);
	//motion_m->GetPosture(animation_time, *curr_posture_m);
	// 動作データが読み込まれていなければ終了
	if (!motion)
		return;
	/*if (!motion_m)
	return;*/
	animation_speed = 1.0f;
	// 時間を進める
	animation_time += delta * animation_speed ;
	if (animation_time > motion->interval*b)//最後のフレームの場合
		AnimationStartPoint(a);
	// 現在のフレーム番号を計算
	frame_no = (animation_time)/ motion->interval;

	// 動作データから現在時刻の姿勢を取得
	motion->GetPosture(animation_time, *curr_posture);
	//motion_m->GetPosture(animation_time, *curr_posture_m);
}
void  MotionPlaybackApp::AnimationStartPoint(int a)
{	
	if (!motion)
		return;
	animation_time = motion->interval*a;
}
void  MotionPlaybackApp::AnimationStartPoint_m(int a)
{
	if (!motion_m)
		return;
	animation_time_m = motion_m->interval*a;
}
void  MotionPlaybackApp::AnimationStartPoint_a(int a)
{
	if (!motion_a)
		return;
	animation_time_a = motion_a->interval*a;
}
//
//  BVH動作ファイルの読み込み、骨格・姿勢の初期化
//
void  MotionPlaybackApp::LoadBVH(const char * file_name)
{
	// BVH動作データを読み込み
	BVH *  bvh = new BVH(file_name);

	// 読み込みに失敗したら終了
	if (!bvh->IsLoadSuccess())
	{
		delete  bvh;
		bvh = NULL;
		body = NULL;
		return;
	}

	// BVH動作から骨格モデルと動作データをを生成
	motion = CoustructBVHMotion(bvh);
	if (!motion)
		return;
	body = motion->body;
	//end_site.resize(5);
	bvh->FramePartition(framecom);//フレーム分割
	//for (int i = 0; i < 5; i++){
	//	end_site[i] = bvh->end_site[i];
	//	end_site[i].scale(bvh_scale);
	//}
	delete  bvh;
	//bvh->FramePartition();//フレーム分割
	// 姿勢の初期化
	if (curr_posture)
		delete  curr_posture;
	curr_posture = new Posture();
	InitPosture(*curr_posture, body);

	// 再生開始
	Start();
}


//
//  ファイルダイアログを表示してBVH動作ファイルを選択・読み込み
//
void  MotionPlaybackApp::OpenNewBVH()
{
#ifdef  WIN32
	const int  file_name_len = 256;
	char  file_name[file_name_len] = "";

	// ファイルダイアログの設定
	OPENFILENAME	open_file;
	memset(&open_file, 0, sizeof(OPENFILENAME));
	open_file.lStructSize = sizeof(OPENFILENAME);
	open_file.hwndOwner = NULL;
	open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
	open_file.nFilterIndex = 1;
	open_file.lpstrFile = file_name;
	open_file.nMaxFile = file_name_len;
	open_file.lpstrTitle = "Select a BVH file";
	open_file.lpstrDefExt = "bvh";
	open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	// ファイルダイアログを表示
	BOOL  ret = GetOpenFileName(&open_file);

	// ファイルが指定されたら新しい動作を設定
	if (ret)
	{
		// BVH動作データの読み込み、骨格・姿勢の初期化
		LoadBVH(file_name);
		
		// 動作再生の開始
		Start();
	}
#endif // WIN32
}

//
//  BVH動作ファイルの読み込み、骨格・姿勢の初期化
//
void  MotionPlaybackApp::LoadBVHMASTER(const char * file_name)
{
	// BVH動作データを読み込み
	BVH *  bvh_m = new BVH(file_name);

	// 読み込みに失敗したら終了
	if (!bvh_m->IsLoadSuccess())
	{
		delete  bvh_m;
		bvh_m = NULL;
		body_m = NULL;
		return;
	}

	// BVH動作から骨格モデルと動作データをを生成
	motion_m = CoustructBVHMotion(bvh_m);
	if (!motion_m)
		return;
	body_m = motion_m->body;
	/*end_site_master.resize(5);
	for (int i = 0; i < 5; i++){
		end_site_master[i] = bvh_m->end_site[i];
		end_site_master[i].scale(bvh_scale);
	}*/
	delete  bvh_m;
	//bvh->FramePartition();//フレーム分割
	// 姿勢の初期化
	if (curr_posture_m)
		delete  curr_posture_m;
	curr_posture_m = new Posture();
	InitPosture(*curr_posture_m, body_m);

	// 再生開始
	//Start();
}


//
//  ファイルダイアログを表示してBVH動作ファイルを選択・読み込み
//
void  MotionPlaybackApp::OpenNewBVHMASTER()
{
#ifdef  WIN32
	const int  file_name_len = 256;
	char  file_name[file_name_len] = "";

	// ファイルダイアログの設定
	OPENFILENAME	open_file;
	memset(&open_file, 0, sizeof(OPENFILENAME));
	open_file.lStructSize = sizeof(OPENFILENAME);
	open_file.hwndOwner = NULL;
	open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
	open_file.nFilterIndex = 1;
	open_file.lpstrFile = file_name;
	open_file.nMaxFile = file_name_len;
	open_file.lpstrTitle = "Select a BVH file";
	open_file.lpstrDefExt = "bvh";
	open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	// ファイルダイアログを表示
	BOOL  ret = GetOpenFileName(&open_file);

	// ファイルが指定されたら新しい動作を設定
	if (ret)
	{
		// BVH動作データの読み込み、骨格・姿勢の初期化
		LoadBVHMASTER(file_name);

		// 動作再生の開始
		//Start();
	}
#endif // WIN32
}

void  MotionPlaybackApp::LoadBVHADVICE(const char * file_name)
{
	// BVH動作データを読み込み
	BVH *  bvh_a = new BVH(file_name);

	// 読み込みに失敗したら終了
	if (!bvh_a->IsLoadSuccess())
	{
		delete  bvh_a;
		bvh_a = NULL;
		body_a = NULL;
		return;
	}

	// BVH動作から骨格モデルと動作データをを生成
	motion_a = CoustructBVHMotion(bvh_a);
	if (!motion_a)
		return;
	body_a = motion_a->body;
	/*end_site_master.resize(5);
	for (int i = 0; i < 5; i++){
		end_site_master[i] = bvh_a->end_site[i];
		end_site_master[i].scale(bvh_scale);
	}*/
	delete  bvh_a;
	//bvh->FramePartition();//フレーム分割
	// 姿勢の初期化
	if (curr_posture_a)
		delete  curr_posture_a;
	curr_posture_a = new Posture();
	InitPosture(*curr_posture_a, body_a);

	// 再生開始
	//Start();
}
void  MotionPlaybackApp::OpenNewBVHADVICE()
{
#ifdef  WIN32
	const int  file_name_len = 256;
	char  file_name[file_name_len] = "";

	// ファイルダイアログの設定
	OPENFILENAME	open_file;
	memset(&open_file, 0, sizeof(OPENFILENAME));
	open_file.lStructSize = sizeof(OPENFILENAME);
	open_file.hwndOwner = NULL;
	open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
	open_file.nFilterIndex = 1;
	open_file.lpstrFile = file_name;
	open_file.nMaxFile = file_name_len;
	open_file.lpstrTitle = "Select a BVH file";
	open_file.lpstrDefExt = "bvh";
	open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	// ファイルダイアログを表示
	BOOL  ret = GetOpenFileName(&open_file);

	// ファイルが指定されたら新しい動作を設定
	if (ret)
	{
		// BVH動作データの読み込み、骨格・姿勢の初期化
		LoadBVHADVICE(file_name);

		// 動作再生の開始
		//Start();
	}
#endif // WIN32
}
///////////////////////////////////////////////////////////////////////////////
//
//  キーフレーム動作再生アプリケーション
//


//
//  キーフレーム動作再生アプリケーションクラス
//

///////////////////////////////////////////////////////////////////////////////
//
//  順運動学計算アプリケーション
//


//
//  順運動学計算アプリケーションクラス
// （動作再生アプリケーションに順運動学計算を追加）
//
class  ForwardKinematicsApp : public MotionPlaybackApp
{
protected:
	// 順運動学計算結果の変数

	// 全体節の位置・向き（座標系）
	vector< Matrix4f >      segment_frames;
	// 全体節の位置・向き（座標系）全フレーム
	vector< Matrix4f >      segment_frames_all;
	vector< Matrix4f >      segment_frames_all2;
	// 全体節の位置・向き（座標系）全フレーム master
	vector< Matrix4f >      segment_frames_all_master;
	// 全関節の位置
	vector< Point3f >       joint_positions;
	// 全関節の位置全フレーム版
	vector<Point3f>			joint_positions_all;
	vector<Point3f>			joint_positions_all2;
	// 全関節の位置全フレーム版 master
	vector<Point3f>			joint_positions_all_master;	
	// 身体の移動位置全フレーム版
	vector<Point3f>			motion_positions_all;
	vector<Point3f>			motion_positions_all2;
	// 身体の移動位置全フレーム版
	vector<Point3f>			motion_positions_all_master;
	//末端情報全フレーム
	vector<Point3f>			motion_end_all_master;
	vector<Point3f>			motion_end_all;
	//静止部分類似度
	vector<float>				cosine_similarity_Stationary;
	//静止部分類似度(パーセンテージ)
	vector<float>				cosine_similarity_Stationary_percentage;
	//静止部分類似度(パーセンテージ)総合評価
	vector<float>				cosine_similarity_Stationary_percentageAll;
	//動作部分類似度
	vector<float>				cosine_similarity_Active;
	//動作部分類似度(パーセンテージ)
	vector<float>				cosine_similarity_Active_percentage;
	//フレーム分割点
	vector<int>					framepoint;
	vector<int> MasterFramePartition;
	//点数
	vector<float> COSs_Evalue;
	vector<float> acc_Evalue;
	vector<float> Aacc_Evalue;
	vector<float> DTW_Evalue;
	vector<float> COSa_Evalue;
	vector<float> score;
	//修正箇所
	vector<int> j_number_acc;
	vector<int>j_number_angle;
	vector<int>g_number_acc;
	vector<int>g_number_angle;
	vector<int> k_number_acc;
	vector<int>k_number_angle;
	vector<int>u_number_acc;
	vector<int>u_number_angle;
	vector<int>d_number_acc;
	vector<int>d_number_angle;
	vector<int>h_number_acc;
	vector<int>h_number_angle;
	vector<int>ke_number_acc;
	vector<int>ke_number_angle;

	vector<int>j_number_anglep;
	vector<int>g_number_anglep;
	vector<int>k_number_anglep;
	vector<int>u_number_anglep;
	vector<int>d_number_anglep;
	vector<int>h_number_anglep;
	vector<int>ke_number_anglep;

	vector<Vector3f>j_number_anglej;
	vector<Vector3f>g_number_anglej;
	vector<Vector3f>k_number_anglej;
	vector<Vector3f>u_number_anglej;
	vector<Vector3f>d_number_anglej;
	vector<Vector3f>h_number_anglej;
	vector<Vector3f>ke_number_anglej;

	bool j_flag = false;
	bool g_flag = false;
	bool k_flag = false;
	bool u_flag = false;
	bool d_flag = false;
	bool h_flag = false;
	bool ke_flag = false;


	float total_score = 0.0;
	
	//訂正方向
	vector<Vector3f>  joint_direction_s;
	vector<Vector3f>  joint_direction_a;
	bool						frameflag=false;
	int							endframe = 0;
	int							startframe = 0;
	int							endframe_m = 0;
	int							startframe_m = 0;
	int							endframe_a = 0;
	int							startframe_a = 0;
public:
	// コンストラクタ
	ForwardKinematicsApp();
	


public:
	// イベント処理

	//  開始・リセット
	virtual void  Start();
	virtual void  Start(int a, int b,int c,int d);
	//  画面描画
	virtual void  Display();
	virtual void  Display2();
	virtual void  Display3();
	virtual void  Display4();
	// アニメーション処理
	virtual void  Animation(float delta);

	virtual void Keyboard(unsigned char key, int mx, int my);

	virtual void MotionComparisonApp();
};


// 補助処理（グローバル関数）のプロトタイプ宣言

// 順運動学計算
void  MyForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array);

// 順運動学計算のための反復計算（ルート体節から末端体節に向かって繰り返し再帰呼び出し）
void  MyForwardKinematicsIteration(const Segment *  segment, const Segment * prev_segment, const Posture & posture,
	Matrix4f * seg_frame_array, Point3f * joi_pos_array = NULL);

//動作比較
void  MotionComparison(vector<float> & cos_Stationary, vector<float> & cos_Active, vector<int>& framePoint, vector<int>& Active_masterframe, vector<int>& Active_frame, vector< Point3f> & joi_pos_array, vector< Point3f> & joi_pos_array_master);

// 順運動学計算全フレーム版
void  AllMyForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motion, vector< Point3f > &  motion_positions_array);
void  AllMyForwardKinematics2(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motion, Motion & motion2, vector< Point3f > &  motion_positions_array);
void  AllMyForwardKinematics_end(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motion, vector< Point3f > &  motion_positions_array, vector< Point3f > &  end_site_set, vector< Point3f > &  end_site_array);

// 順運動学計算のための反復計算（ルート体節から末端体節に向かって繰り返し再帰呼び出し）
void AllMyForwardKinematicsIteration(const int i, const int joint_num, const Segment *  segment, const Segment * prev_segment, const Posture & posture,	Matrix4f * seg_frame_array, Point3f * joi_pos_array = NULL);
void AllMyForwardKinematicsIteration_end(const int i, const int joint_num, const Segment *  segment, const Segment * prev_segment, const Posture & posture, Matrix4f * seg_frame_array, Point3f * joi_pos_array, int& cnt, vector< Point3f > &  end_site_set, Point3f * end_site_array = NULL);





//左手のベクトル生成
void CreateleftArmForeArmvector(const Posture & posture, vector< Point3f > &  joi_pos_array, Vector3f* left_ArmForeArm_array = NULL);
void CreatelefthandForeArmvector(const Posture & posture, vector< Point3f > &  joi_pos_array, Vector3f* left_handForeArm_array = NULL);


//コサイン類似度
float CosineSimilarity(Vector3f vector1, Vector3f vector2);
//パーセンテージ化
void  Percentage(vector<float> & cos_s, float OriginalNumber, vector<float> & out_parcent);
void  PercentageAll(vector<float> & cos_s, float OriginalNumber, vector<float> & out_parcent);

//フレーム分割点
void FramePartitionApp(const Posture & posture, vector< Point3f> & joi_pos_array, vector< Point3f> & master_joi_pos_array, vector<int>& frameCandidate, vector<int>& framePoint, vector< Point3f > &  motion_positions_array, vector< Point3f > &  master_motion_positions_array,float interval);

void out_of_filehyoukachi(vector< float> & cosine_similarity_Stationary, vector< float> & cosine_similarity_Active, vector< float> & acc, vector< float> & A_acc, vector< float> & DTW, vector<float>& score, float & total_score);

void out_of_file_nejire(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector< Matrix4f > & seg_frame_array, int frame_before, int frame_after,float interval);
void out_of_file_nejiremaster(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector< Matrix4f > & seg_frame_array, int frame_before, int frame_after, float interval);
void out_of_filejoi2acceleration(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm);
void out_of_filejoi2acceleration2(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm);

void out_of_filejoi2accelerationmaster(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm);
void out_of_filejoi2accelerationmaster2(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm);
float ED(Point3f a, Point3f b);
float DTW(vector<Point3f> , vector<Point3f>, int );
void orbit(vector< Point3f>  joi_pos_array, vector<Point3f>& a, int start, int end, int joint_number);
void Evaluation_acceleration(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<int> frame_section, vector<float>& acc, vector<float>& A_acc, vector<int>& acc_frame, vector<int>& Angular_acc_frame, float intervalm);
void Evaluation_spineVariation(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<int> frame_section, vector<float>& SD_point);
void Evaluation_DTW(vector< Point3f> & joi_pos_array_m, vector< Point3f> & joi_pos_array, vector<int> frame_section_m, vector<int> frame_section, vector<float>& DTW_point);
void Evaluation_acc_value(vector<float>& m_acc, vector<float>& m_A_acc, vector<float>& acc, vector<float>& A_acc, vector<float>& E_accvalue, vector<float>& E_Aaccvalue);
void Evaluation_SD_value(vector<float>& m_SD,  vector<float>& SD, vector<float>& E_SDvalue);
void Evaluation_DTW_value(vector<float>& DTW, vector<float>& E_DTWvalue, float  weight);
void Evaluation_angle(vector<float> & cos_s, vector<float> & cos_a, vector<float> & E_cos_s, vector<float> & E_cos_a);
void ComprehensiveEvaluation(vector<float> & score, float & total_score, vector< float> & cosine_similarity_Stationary, vector< float> & cosine_similarity_Active, vector< float> & acc, vector< float> & A_acc, vector< float> & DTW);
void Modification_place(vector<Vector3f> & joint_direction_c, vector<Vector3f> & joint_direction_a, vector<int>& framePoint, vector<int>& Active_masterframe, vector<int>& Active_frame, vector< Point3f> & joi_pos_array, vector< Point3f> & joi_pos_array_master);
void CreateSentence();
void FindFixesPoint(vector<float>cosine_similarity_Stationary, vector<float>cosine_similarity_Active, vector<Vector3f> & joint_direction_s, vector<Vector3f> & joint_direction_a, vector<float> COSs_Evalue, vector<float> acc_Evalue, vector<float> Aacc_Evalue, vector<float> DTW_Evalue, vector<float> COSa_Evalue,
	vector<int>& j_number_acc, vector<int>&j_number_angle, vector<int>&g_number_acc, vector<int>&g_number_angle, vector<int>&k_number_acc, vector<int>&k_number_angle, vector<int>&u_number_acc, vector<int>&u_number_angle, vector<int>&d_number_acc, vector<int>&d_number_angle, vector<int>&h_number_acc, vector<int>&h_number_angle, vector<int>&ke_number_acc, vector<int>&ke_number_angle,
	vector<int>&j_number_anglep,vector<int>&g_number_anglep,vector<int>&k_number_anglep,vector<int>&u_number_anglep,vector<int>&d_number_anglep,vector<int>&h_number_anglep,vector<int>&ke_number_anglep,
	vector<Vector3f>&j_number_anglej, vector<Vector3f>&g_number_anglej, vector<Vector3f>&k_number_anglej, vector<Vector3f>&u_number_anglej, vector<Vector3f>&d_number_anglej, vector<Vector3f>&h_number_anglej, vector<Vector3f>&ke_number_anglej);


//
//  コンストラクタ
//
ForwardKinematicsApp::ForwardKinematicsApp()
{
	app_name = "shorinjikempotrainingsystem ";
}


//
//  開始・リセット
//
void  ForwardKinematicsApp::Start()
{
	MotionPlaybackApp::Start();

	if (!body)
		return;

	// 配列初期化
	segment_frames.resize(body->num_segments);
	joint_positions.resize(body->num_joints);

	ForwardKinematics(*curr_posture, segment_frames, joint_positions);

}

void  ForwardKinematicsApp::Start(int a,int b,int c,int d)
{
	MotionPlaybackApp::Start(a,b,c,d);

	if (!body)
		return;

	// 配列初期化
	segment_frames.resize(body->num_segments);
	joint_positions.resize(body->num_joints);

	ForwardKinematics(*curr_posture, segment_frames, joint_positions);

}

//
//  画面描画
//
void  ForwardKinematicsApp::Display()
{
	GLUTBaseApp::Display();

	// キャラクタを描画
	if (curr_posture_m)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		DrawPosture(*curr_posture_m);
		DrawPostureShadow(*curr_posture_m, shadow_dir, shadow_color);
	}
	// 現在のモード、時間・フレーム番号を表示
	DrawTextInformation(0, " ");
	char  message[64];
	if (motion)
		sprintf(message, "%.2f (%d)", animation_time_m, frame_no_m);
	else
		sprintf(message, "Press 'L' key to Load a BVH file");
	DrawTextInformation(1, message);
}

void  ForwardKinematicsApp::Display2()
{


	GLUTBaseApp::Display2();

	// キャラクタを描画
	if (curr_posture)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		DrawPosture(*curr_posture);
		DrawPostureShadow(*curr_posture, shadow_dir, shadow_color);
	}


	// 現在のモード、時間・フレーム番号を表示
	DrawTextInformation(0, " ");
	char  message[64];
	if (motion)
		sprintf(message, "%.2f (%d)", animation_time, frame_no);
	else
		sprintf(message, "Press 'L' key to Load a BVH file");
	DrawTextInformation(1, message);
}
void  ForwardKinematicsApp::Display3()
{


	GLUTBaseApp::Display3();

	// キャラクタを描画
	if (curr_posture_a)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		DrawPosture(*curr_posture_a);
		DrawPostureShadow(*curr_posture_a, shadow_dir, shadow_color);
	}


	// 現在のモード、時間・フレーム番号を表示
	DrawTextInformation(0, " ");
	char  message[64];
	if (motion)
		sprintf(message, "%.2f (%d)", animation_time_a, frame_no_a);
	else
		sprintf(message, "Press 'L' key to Load a BVH file");
	DrawTextInformation(1, message);
}
void  ForwardKinematicsApp::Display4()
{


	GLUTBaseApp::Display4();

	// キャラクタを描画
	if (curr_posture&&curr_posture_m)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		DrawPosture(*curr_posture);
		DrawPostureShadow(*curr_posture, shadow_dir, shadow_color);
		glColor3f(1.0f, 0.0f, 0.0f);
		DrawPosture(*curr_posture_m);
		DrawPostureShadow(*curr_posture_m, shadow_dir, shadow_color);
	}


	// 現在のモード、時間・フレーム番号を表示
	DrawTextInformation(0, " ");
	char  message[64];
	if (motion)
		sprintf(message, "master %.2f (%d) hikensha %.2f (%d)", animation_time_m, frame_no_m, animation_time, frame_no);
	else
		sprintf(message, "Press 'L' key to Load a BVH file");
	
	DrawTextInformation(1, message);
}
//
//  キーボードのキー押下
//
void  ForwardKinematicsApp::Keyboard(unsigned char key, int mx, int my)
{//関係なし

	// 基底クラスの処理を実行
	MotionPlaybackApp::Keyboard(key, mx, my);
	MotionPlaybackApp::Keyboard2(key, mx, my);
	MotionPlaybackApp::Keyboard3(key, mx, my);
	// d キーで描画設定を変更
	if (key == 'c')
	{
	
		on_animation = !on_animation;
		MotionComparisonApp();
		char score_sentence[1024] = {};
		char f[256] = {};
		char b[256] = {};
		char d[256] = "点\n";
		char e[256] = {};
		int a = 0;
		memset(score_sentence, '\0', 1024);
		for (int i = 0; i < 8; i++){
			if (i != 7){
				a = score[i] * 100;
			}
			else{
				a = total_score * 100;
			}
			memset(b, '\0', 256);
			_itoa(a, b, 10);
			memset(e, '\0', 256);
			if (i == 0){
				char* c = "順突動作";
				strcat(score_sentence, c);
				strcat(e, "'1'キーを押すと順突動作の修正方法を提示します\n\n");
			}
			else if (i == 1){
				char* c = "逆突動作";
				strcat(score_sentence, c);
				strcat(e, "'2'キーを押すと逆突動作の修正方法を提示します\n\n");
			}
			else if (i == 2){
				char* c = "鈎突動作";
				strcat(score_sentence, c);
				strcat(e, "'3'キーを押すと鈎突動作の修正方法を提示します\n\n");
			}
			else if (i == 3){
				char* c = "上受動作";
				strcat(score_sentence, c);
				strcat(e, "'4'キーを押すと上受動作の修正方法を提示します\n\n");
			}
			else if (i == 4){
				char* c = "同時受動作";
				strcat(score_sentence, c);
				strcat(e, "'5'キーを押すと同時受動作の修正方法を提示します\n\n");
			}
			else if (i == 5){
				char* c = "払受動作";
				strcat(score_sentence, c);
				strcat(e, "'6'キーを押すと払受動作の修正方法を提示します\n\n");
			}
			else if (i == 6){
				char* c = "払受動作";
				strcat(score_sentence, c);
				strcat(e, "'7'キーを押すと蹴上動作の修正方法を提示します\n\n");
			}
			else if (i == 7){
				char* c = "合計得点";
				strcat(score_sentence, c);
				strcat(e, "'8'キーを押すと全体の修正方法を提示します\n\n");
			}
			strcat(score_sentence, b);
			strcat(score_sentence, d);
			strcat(score_sentence, e);

		}
		
		MessageBox(NULL, score_sentence, "About", MB_OK | MB_ICONINFORMATION);
	}
	if (j_flag == false && g_flag == false && k_flag == false && u_flag == false && d_flag == false && h_flag == false && ke_flag == false ){
		if (key == '0')
		{
			frameflag = true;
			//Start(framepoint[0], framepoint[1]);
			endframe = framepoint[1];
			startframe = framepoint[0];
			endframe_m = MasterFramePartition[1];
			startframe_m = MasterFramePartition[0];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			MotionPlaybackApp::AnimationStartPoint(startframe);
			animation_speed = 1.0f;
			animation_speed_m = 1.0f;
			

		}
		if (key == '1')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[2];
			startframe = framepoint[1];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[2];
			startframe_m = MasterFramePartition[1];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = true;
			g_flag = false;
			k_flag = false;
			u_flag = false;
			d_flag = false;
			h_flag = false;
			ke_flag = false;
			MessageBox(NULL, "順突が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);

		}
		if (key == '2')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[3];
			startframe = framepoint[2];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[3];
			startframe_m = MasterFramePartition[2];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = true;
			k_flag = false;
			u_flag = false;
			d_flag = false;
			h_flag = false;
			ke_flag = false;
			MessageBox(NULL, "逆突が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
		if (key == '3')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[4];
			startframe = framepoint[3];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[4];
			startframe_m = MasterFramePartition[3];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = false;
			k_flag = true;
			u_flag = false;
			d_flag = false;
			h_flag = false;
			ke_flag = false;
			MessageBox(NULL, "鈎突が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
		if (key == '4')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[5];
			startframe = framepoint[4];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[5];
			startframe_m = MasterFramePartition[4];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = false;
			k_flag = false;
			u_flag = true;
			d_flag = false;
			h_flag = false;
			ke_flag = false;
			MessageBox(NULL, "上受が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
		if (key == '5')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[6];
			startframe = framepoint[5];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[6];
			startframe_m = MasterFramePartition[5];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = false;
			k_flag = false;
			u_flag = false;
			d_flag = true;
			h_flag = false;
			ke_flag = false;
			MessageBox(NULL, "同時受が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
		if (key == '6')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[7];
			startframe = framepoint[6];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[7];
			startframe_m = MasterFramePartition[6];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = false;
			k_flag = false;
			u_flag = false;
			d_flag = false;
			h_flag = true;
			ke_flag = false;
			MessageBox(NULL, "払受が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
		if (key == '7')
		{
			frameflag = true;
			//Start(framepoint[1], framepoint[2]);
			endframe = framepoint[8];
			startframe = framepoint[7];
			MotionPlaybackApp::AnimationStartPoint(startframe);
			endframe_m = MasterFramePartition[8];
			startframe_m = MasterFramePartition[7];
			MotionPlaybackApp::AnimationStartPoint_m(startframe_m);
			j_flag = false;
			g_flag = false;
			k_flag = false;
			u_flag = false;
			d_flag = false;
			h_flag = false;
			ke_flag = true;
			MessageBox(NULL, "蹴上が修正箇所として設定されました．\n修正方法は'1'キー'2'キー'3'キーを押すことで表示できます．", "About", MB_OK | MB_ICONINFORMATION);
		}
	}
	else{
		if (key == '1')
		{
			int i = 0;
			if (j_flag == true){
				if (j_number_acc.size()>=1){
					
					//加速度
					if (j_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "突きを行う際に右手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
				}
				else{
					//角度
					if (j_number_angle[i] == 0){
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 1){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 2){
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 3){
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 4){
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (j_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 6){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (j_number_angle[i] == 7){
						//右手
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 8){
						//左足
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 9){
						//右足
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}//LoadBVHADVICE(const char * file_name);
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if (g_flag == true){
					if (g_number_acc.size() >= 1){
					//加速度
						if (g_number_acc[i] == 0){
						//右手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
						else if (g_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きを行う際に左手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
						else if (g_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
						else if (g_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
						else if (g_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					
				}
				else{
					//角度
					if (g_number_angle[i] == 0){
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 1){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 2){
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 7){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 3){
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 4){
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (g_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 6){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 7){
						//右手
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
				

					else if (g_number_anglep[i] == 7){
						//上腕
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else{
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}


					}

					}
					else if (g_number_angle[i] == 8){
						//左足
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 9){
						//右足
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (k_flag == true){

				if (k_number_acc.size() >= 1){
					//加速度
					if (k_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "突きを行う際に右手で外受をしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 2){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					//角度
					if (k_number_angle[i] == 0){
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 1){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[0].x) == max({ fabsf(k_number_anglej[0].x), fabsf(k_number_anglej[0].y), fabsf(k_number_anglej[0].z) })){
								if (k_number_anglej[0].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘の位置が低いので，左肘を上げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が高いので，左肘を下げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が左に外れているので，左肘を右方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が右に外れているので，左肘を左方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を上げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手の位置が高いので，低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左肘が伸びているので，肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 2){
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 3){
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 4){
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (k_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 6){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (k_number_angle[i] == 7){
						//右手
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 8){
						//左足
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 9){
						//右足
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (u_flag == true){

				if (u_number_acc.size() >= 1){
					//加速度
					if (u_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "左足の移動を早くしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					//角度
					if (u_number_angle[i] == 0){
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 1){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 2){
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 3){
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 4){
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (u_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 6){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}

						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 7){
						//右手
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
						//上腕
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else{
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}


					}



					}
					else if (u_number_angle[i] == 8){
						//左足
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 9){
						//右足
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (d_flag == true){
				if (d_number_acc.size() >= 1){
					//加速度
					if (d_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					//角度
					if (d_number_angle[i] == 0){
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 1){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 2){
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 3){
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 4){
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (d_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[0].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 6){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (d_number_angle[i] == 7){
						//右手
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 8){
						//左足
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 9){
						//右足
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			if (h_flag == true){
				if (h_number_acc.size() >= 1){
					//加速度
					if (h_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					//角度
					if (h_number_angle[i] == 0){
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 1){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 2){
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 3){
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 4){
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (h_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[0].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 6){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (h_number_angle[i] == 7){
						//右手
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}

							}
						}
					}
					else if (h_number_angle[i] == 8){
						//左足
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 9){
						//右足
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (ke_flag == true){
				if (ke_number_acc.size() >= 1){
					//加速度
					if (ke_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "蹴りのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					//角度
					if (ke_number_angle[i] == 0){
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 1){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 2){
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 3){
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 4){
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (ke_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[0].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 6){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 7){
						//右手
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 8){
						//左足
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 9){
						//右足
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "高く蹴りすぎているので低くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "蹴りが低いので高くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に蹴っているので右方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に蹴っているので左方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
		}

		///==============================================================
		///================================================================
		if (key == '2')
		{

			int i = 1;
			if (j_flag == true){
				if (j_number_acc.size() >= 2){

					//加速度
					if (j_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "突きを行う際に右手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
				}
				else{
					if (j_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (j_number_angle[i] == 0){
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 1){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 2){
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 3){
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 4){
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (j_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 6){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (j_number_angle[i] == 7){
						//右手
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 8){
						//左足
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 9){
						//右足
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}//LoadBVHADVICE(const char * file_name);
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (g_flag == true){
				int i = 1;
				if (g_number_acc.size() >= 2){
					//加速度
					if (g_number_acc[i] == 0){
						//右手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きを行う際に左手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (g_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (g_number_angle[i] == 0){
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 1){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 2){
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 7){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 3){
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 4){
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (g_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 6){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 7){
						//右手
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}


						else if (g_number_anglep[i] == 7){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}

					}
					else if (g_number_angle[i] == 8){
						//左足
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 9){
						//右足
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (k_flag == true){
				int i = 1;
				if (k_number_acc.size() >= 2){
					//加速度
					if (k_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "突きを行う際に右手で外受をしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 2){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (k_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (k_number_angle[i] == 0){
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 1){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[0].x) == max({ fabsf(k_number_anglej[0].x), fabsf(k_number_anglej[0].y), fabsf(k_number_anglej[0].z) })){
								if (k_number_anglej[0].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘の位置が低いので，左肘を上げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が高いので，左肘を下げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が左に外れているので，左肘を右方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が右に外れているので，左肘を左方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を上げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手の位置が高いので，低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左肘が伸びているので，肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 2){
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 3){
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 4){
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (k_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 6){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (k_number_angle[i] == 7){
						//右手
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 8){
						//左足
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 9){
						//右足
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (u_flag == true){
				int i = 1;
				if (u_number_acc.size() >= 2){
					//加速度
					if (u_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "左足の移動を早くしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (u_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (u_number_angle[i] == 0){
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 1){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 2){
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 3){
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 4){
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (u_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 6){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}

						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 7){
						//右手
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}



					}
					else if (u_number_angle[i] == 8){
						//左足
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 9){
						//右足
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (d_flag == true){
				int i = 1;
				if (d_number_acc.size() >= 2){
					//加速度
					if (d_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (d_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (d_number_angle[i] == 0){
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 1){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 2){
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 3){
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 4){
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (d_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[0].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 6){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (d_number_angle[i] == 7){
						//右手
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 8){
						//左足
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 9){
						//右足
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			if (h_flag == true){
				int i = 1;
				if (h_number_acc.size() >= 2){
					//加速度
					if (h_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (h_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (h_number_angle[i] == 0){
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 1){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 2){
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 3){
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 4){
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (h_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[0].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 6){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (h_number_angle[i] == 7){
						//右手
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}

							}
						}
					}
					else if (h_number_angle[i] == 8){
						//左足
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 9){
						//右足
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (ke_flag == true){
				int i = 1;
				if (ke_number_acc.size() >= 2){
					//加速度
					if (ke_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "蹴りのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (ke_number_acc.size() == 1){
						i = 0;
					}
					else{
						i = 1;
					}
					//角度
					if (ke_number_angle[i] == 0){
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 1){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 2){
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 3){
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 4){
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (ke_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[0].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 6){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 7){
						//右手
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 8){
						//左足
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 9){
						//右足
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "高く蹴りすぎているので低くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "蹴りが低いので高くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に蹴っているので右方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に蹴っているので左方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
		}

		///==============================================================
		///================================================================

		if (key == '3')
		{

			int i = 2;
			if (j_flag == true){
				if (j_number_acc.size() >= 3){

					//加速度
					if (j_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "突きを行う際に右手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (j_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
				}
				else{
					if (j_number_acc.size() == 1){
						i = 1;
					}
					else if (j_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (j_number_angle[i] == 0){
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 1){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 2){
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 3){
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 4){
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (j_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
							if (j_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 6){
						//左手
						if (j_number_anglep[i] == 3){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 4){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (j_number_angle[i] == 7){
						//右手
						//右手
						if (j_number_anglep[i] == 6){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 7){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (j_number_angle[i] == 8){
						//左足
						//左足
						if (j_number_anglep[i] == 9){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 10){
							//上腕

							if (j_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (j_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (j_number_angle[i] == 9){
						//右足
						//右足
						if (j_number_anglep[i] == 12){
							//肩
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (j_number_anglep[i] == 13){
							//上腕
							if (fabsf(j_number_anglej[i].x) == max({ fabsf(j_number_anglej[i].x), fabsf(j_number_anglej[i].y), fabsf(j_number_anglej[i].z) })){
								if (j_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (j_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}//LoadBVHADVICE(const char * file_name);
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (g_flag == true){
				int i = 2;
				if (g_number_acc.size() >= 3){
					//加速度
					if (g_number_acc[i] == 0){
						//右手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "突きを行う際に左手を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "突きを行う際に右足を前に寄せてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (g_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (g_number_acc.size() == 1){
						i = 1;
					}
					else if (g_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (g_number_angle[i] == 0){
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 1){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 2){
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 7){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 3){
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 4){
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (g_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
							if (g_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 6){
						//左手
						if (g_number_anglep[i] == 3){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 4){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (g_number_angle[i] == 7){
						//右手
						//右手
						if (g_number_anglep[i] == 6){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}


						else if (g_number_anglep[i] == 7){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}

					}
					else if (g_number_angle[i] == 8){
						//左足
						//左足
						if (g_number_anglep[i] == 9){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 10){
							//上腕

							if (g_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (g_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (g_number_angle[i] == 9){
						//右足
						//右足
						if (g_number_anglep[i] == 12){
							//肩
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (g_number_anglep[i] == 13){
							//上腕
							if (fabsf(g_number_anglej[i].x) == max({ fabsf(g_number_anglej[i].x), fabsf(g_number_anglej[i].y), fabsf(g_number_anglej[i].z) })){
								if (g_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (g_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (k_flag == true){
				int i = 2;
				if (k_number_acc.size() >= 3){
					//加速度
					if (k_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "突きを行う前に左足を前に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "突きを行う際に右手で外受をしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 2){
						//左手
						MessageBox(NULL, "突きのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "突きを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (k_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "突きを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (k_number_acc.size() == 1){
						i = 1;
					}
					else if (k_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (k_number_angle[i] == 0){
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 1){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[0].x) == max({ fabsf(k_number_anglej[0].x), fabsf(k_number_anglej[0].y), fabsf(k_number_anglej[0].z) })){
								if (k_number_anglej[0].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘の位置が低いので，左肘を上げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が高いので，左肘を下げてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が左に外れているので，左肘を右方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が右に外れているので，左肘を左方向に構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を上げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手の位置が高いので，低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左肘が伸びているので，肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 2){
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 3){
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 4){
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (k_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
							if (k_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 6){
						//左手
						if (k_number_anglep[i] == 3){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 4){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (k_number_angle[i] == 7){
						//右手
						//右手
						if (k_number_anglep[i] == 6){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 7){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (k_number_angle[i] == 8){
						//左足
						//左足
						if (k_number_anglep[i] == 9){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 10){
							//上腕

							if (k_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (k_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (k_number_angle[i] == 9){
						//右足
						//右足
						if (k_number_anglep[i] == 12){
							//肩
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (k_number_anglep[i] == 13){
							//上腕
							if (fabsf(k_number_anglej[i].x) == max({ fabsf(k_number_anglej[i].x), fabsf(k_number_anglej[i].y), fabsf(k_number_anglej[i].z) })){
								if (k_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (k_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			if (u_flag == true){
				int i = 2;
				if (u_number_acc.size() >= 3){
					//加速度
					if (u_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "左足の移動を早くしてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 1){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 2){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 3){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (u_number_acc[i] == 4){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (u_number_acc.size() == 1){
						i = 1;
					}
					else if (u_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (u_number_angle[i] == 0){
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 1){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 2){
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 3){
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 4){
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (u_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
							if (u_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 6){
						//左手
						if (u_number_anglep[i] == 3){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}

						else if (u_number_anglep[i] == 4){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (u_number_angle[i] == 7){
						//右手
						//右手
						if (u_number_anglep[i] == 6){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 7){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "下方向に突きを行っているので上方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "上方向に突きを行っているので下方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に突きを行っているので右方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に突きを行っているので左方向に突きを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}



					}
					else if (u_number_angle[i] == 8){
						//左足
						//左足
						if (u_number_anglep[i] == 9){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 10){
							//上腕

							if (u_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (u_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (u_number_angle[i] == 9){
						//右足
						//右足
						if (u_number_anglep[i] == 12){
							//肩
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (u_number_anglep[i] == 13){
							//上腕
							if (fabsf(u_number_anglej[i].x) == max({ fabsf(u_number_anglej[i].x), fabsf(u_number_anglej[i].y), fabsf(u_number_anglej[i].z) })){
								if (u_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (u_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (d_flag == true){
				int i = 2;
				if (d_number_acc.size() >= 3){
					//加速度
					if (d_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (d_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (d_number_acc.size() == 1){
						i = 1;
					}
					else if (d_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (d_number_angle[i] == 0){
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 1){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 2){
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 3){
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 4){
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (d_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[0].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
							if (d_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 6){
						//左手
						if (d_number_anglep[i] == 3){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 4){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (d_number_angle[i] == 7){
						//右手
						//右手
						if (d_number_anglep[i] == 6){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 7){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "右手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (d_number_angle[i] == 8){
						//左足
						//左足
						if (d_number_anglep[i] == 9){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 10){
							//上腕

							if (d_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (d_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (d_number_angle[i] == 9){
						//右足
						//右足
						if (d_number_anglep[i] == 12){
							//肩
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (d_number_anglep[i] == 13){
							//上腕
							if (fabsf(d_number_anglej[i].x) == max({ fabsf(d_number_anglej[i].x), fabsf(d_number_anglej[i].y), fabsf(d_number_anglej[i].z) })){
								if (d_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (d_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			if (h_flag == true){
				int i = 2;
				if (h_number_acc.size() >= 3){
					//加速度
					if (h_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (h_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (h_number_acc.size() == 1){
						i = 1;
					}
					else if (h_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (h_number_angle[i] == 0){
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 1){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 2){
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (h_number_angle[i] == 3){
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 4){
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (h_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[0].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
							if (h_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 6){
						//左手
						if (h_number_anglep[i] == 3){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 4){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}


						}
					}
					else if (h_number_angle[i] == 7){
						//右手
						//右手
						if (h_number_anglep[i] == 6){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 7){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "左手が下方向に受けを行っているので上方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が上方向に受けを行っているので下方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左方向に受けを行っているので右方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右方向に受けを行っているので左方向に受けを行ってください", "About", MB_OK | MB_ICONINFORMATION);
								}

							}
						}
					}
					else if (h_number_angle[i] == 8){
						//左足
						//左足
						if (h_number_anglep[i] == 9){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 10){
							//上腕

							if (h_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (h_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (h_number_angle[i] == 9){
						//右足
						//右足
						if (h_number_anglep[i] == 12){
							//肩
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (h_number_anglep[i] == 13){
							//上腕
							if (fabsf(h_number_anglej[i].x) == max({ fabsf(h_number_anglej[i].x), fabsf(h_number_anglej[i].y), fabsf(h_number_anglej[i].z) })){
								if (h_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (h_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (ke_flag == true){
				int i = 2;
				if (ke_number_acc.size() >= 3){
					//加速度
					if (ke_number_acc[i] == 0){
						//左足
						MessageBox(NULL, "受けを行う前に左足を後ろ方向に踏み込んでください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (ke_number_acc[i] == 1){
						//左手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (ke_number_acc[i] == 2){
						//右手
						MessageBox(NULL, "受けのスピードが遅いです", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (ke_number_acc[i] == 3){
						//右足
						MessageBox(NULL, "受けを行う際，右足を引いてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (ke_number_acc[i] == 4){
						//腰
						MessageBox(NULL, "受けを行う前に腰を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}
					else if (ke_number_acc[i] == 5){
						//肩
						MessageBox(NULL, "受けを行う前に肩を回転させてください", "About", MB_OK | MB_ICONINFORMATION);
					}

				}
				else{
					if (ke_number_acc.size() == 1){
						i = 1;
					}
					else if (ke_number_acc.size() == 2){
						i = 0;
					}
					else{
						i = 2;
					}
					//角度
					if (ke_number_angle[i] == 0){
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 1){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 2){
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 3){
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 4){
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右足が左側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右足が右側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
					}
					else if (ke_number_angle[i] == 5){
						//~~~~~~~~~~~~~~~~~~~~~~~動作部分~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						//背中
						if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[0].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
							if (ke_number_anglej[i].x > 0){
								MessageBox(NULL, "姿勢が前に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が後ろに傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
						else{
							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "姿勢が左に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "姿勢が右に傾いていますので，姿勢を正してください", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 6){
						//左手
						if (ke_number_anglep[i] == 3){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 4){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "左肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "左肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 7){
						//右手
						//右手
						if (ke_number_anglep[i] == 6){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肩が下がっていますので，肩に力を入れて位置を高くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肩が上がっていますので，肩の力を抜いて位置を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){

									MessageBox(NULL, "身体が内側を向いているので，身体を外側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{

									MessageBox(NULL, "身体が外側を向いているので，身体を内側に回してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 7){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右脇を閉めすぎているので，力を抜いて大きく構えてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘の位置が身体から離れてしまっているため，脇を閉めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右肘が前に出ているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が後ろに外れているので，身体に近い位置に左肘を構え直してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else{
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "右肘が伸びて腕が下におちているので，力を入れて肘を曲げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右肘が曲がりすぎているので，力を抜いて肘を伸ばして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "右手が左に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右手が右に向いているので，手を正面に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}

						}
					}
					else if (ke_number_angle[i] == 8){
						//左足
						//左足
						if (ke_number_anglep[i] == 9){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){
									MessageBox(NULL, "足幅が狭いので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "足幅が広いので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足が左側にあるので，足幅を狭めて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足が右側にあるので，足幅を広げて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 10){
							//上腕

							if (ke_number_anglej[i].z > 0){
								MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
							}

						}
						else{

							if (ke_number_anglej[i].y > 0){
								MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
							}
							else{
								MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
							}
						}
					}
					else if (ke_number_angle[i] == 9){
						//右足
						//右足
						if (ke_number_anglep[i] == 12){
							//肩
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].x > 0){

									MessageBox(NULL, "高く蹴りすぎているので低くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "蹴りが低いので高くし，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左方向に蹴っているので右方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "右方向に蹴っているので左方向に蹴りを修正し，蹴り方を確認してください", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
						}
						else if (ke_number_anglep[i] == 13){
							//上腕
							if (fabsf(ke_number_anglej[i].x) == max({ fabsf(ke_number_anglej[i].x), fabsf(ke_number_anglej[i].y), fabsf(ke_number_anglej[i].z) })){
								if (ke_number_anglej[i].z > 0){
									MessageBox(NULL, "膝が伸びているので，膝を曲げて姿勢を低くしてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "膝が曲がりすぎているので，膝を伸ばして姿勢を高くして下さい", "About", MB_OK | MB_ICONINFORMATION);
								}
							}
							else{
								if (ke_number_anglej[i].y > 0){
									MessageBox(NULL, "左足首が外側を向いているため，内側に向けてください", "About", MB_OK | MB_ICONINFORMATION);
								}
								else{
									MessageBox(NULL, "左足首が内側を向いているため，外側に向けて下さい", "About", MB_OK | MB_ICONINFORMATION);
								}

							}

						}
					}
				}
			}
		}
		if (key == 'n')
		{
			j_flag = false;
			g_flag = false;
			k_flag = false;
			u_flag = false;
			d_flag = false;
			h_flag = false;
			ke_flag = false;
		}
	}
	
}
	
	


//
//  アニメーション処理
//
void  ForwardKinematicsApp::Animation(float delta)
{
	if (frameflag == false){
		MotionPlaybackApp::Animation(delta);
	}
	else{
		MotionPlaybackApp::Animation(delta, startframe, endframe, startframe_m, endframe_m);
	}
	// アニメーション再生中でなければ終了
	if (!on_animation)
		return;

	if (!curr_posture)
		return;

	// 順運動学計算
	MyForwardKinematics(*curr_posture, segment_frames, joint_positions);
}

void  ForwardKinematicsApp::MotionComparisonApp(){

	// 配列初期化
	float interval = 0.0;
	vector<Point3f> jun_mas;
	vector<Point3f> jun_sub;
	vector<float> important_acc_master;
	vector<float> important_acc;
	vector<int> important_acc_masterframe;
	vector<int> important_accframe;
	vector<float> important_Aacc_master;
	vector<float> important_Aacc;
	vector<int> important_Aacc_masterframe;
	vector<int> important_Aaccframe;
	vector<float> important_DTW;
	vector<int> Active_master_frame;
	vector<int> Active_frame;


	score.resize(7);
	float distance = 0.0;
	segment_frames_all_master.resize((*curr_posture_m).body->num_segments*(*motion_m).num_frames);
	joint_positions_all_master.resize((*curr_posture_m).body->num_joints*(*motion_m).num_frames);
	motion_end_all.resize((*motion).num_frames * 5);
	motion_end_all_master.resize((*motion_m).num_frames * 5);
	// 配列初期化
	segment_frames_all.resize((*curr_posture).body->num_segments*(*motion).num_frames);
	segment_frames_all2.resize((*curr_posture).body->num_segments*(*motion).num_frames);
	joint_positions_all2.resize((*curr_posture).body->num_joints*(*motion).num_frames);
	joint_positions_all.resize((*curr_posture).body->num_joints*(*motion).num_frames);
	//配列初期化
	cosine_similarity_Stationary.resize(105);
	cosine_similarity_Active.resize(105);
	joint_direction_s.resize(105);
	joint_direction_a.resize(105);
	cosine_similarity_Stationary_percentage.resize(105);
	cosine_similarity_Active_percentage.resize(105);
	COSs_Evalue.resize(35);
	COSa_Evalue.resize(35);
	cosine_similarity_Stationary_percentageAll.resize(10);
	motion_positions_all_master.resize((*motion_m).num_frames);
	motion_positions_all.resize((*motion).num_frames);
	motion_positions_all2.resize((*motion).num_frames);
	//フレーム分割点
	framepoint.resize(10);
	MasterFramePartition.resize(10);
	important_acc_master.resize(22);
	important_acc.resize(22);
	important_Aacc_master.resize(12);
	important_Aacc.resize(12);
	important_acc_masterframe.resize(22);
	important_accframe.resize(22);
	important_Aacc_masterframe.resize(12);
	important_Aaccframe.resize(12);
	important_DTW.resize(11);
	Active_master_frame.resize(7);
	Active_frame.resize(7);
	acc_Evalue.resize(22);
	Aacc_Evalue.resize(12);
	DTW_Evalue.resize(11);

	MasterFramePartition = { 0, 200, 350, 440, 579, 749, 868, 969, 1107, 1277 };
	AllMyForwardKinematics(*curr_posture_m, segment_frames_all_master, joint_positions_all_master, *motion_m, motion_positions_all_master);
	AllMyForwardKinematics(*curr_posture, segment_frames_all, joint_positions_all, *motion, motion_positions_all);
	/*AllMyForwardKinematics_end(*curr_posture_m, segment_frames_all_master, joint_positions_all_master, *motion_m, motion_positions_all_master, end_site, motion_end_all_master);
	AllMyForwardKinematics_end(*curr_posture, segment_frames_all, joint_positions_all, *motion, motion_positions_all, end_site, motion_end_all);*/
	//マスターデータの骨格で学習者データを付与してみた
	AllMyForwardKinematics2(*curr_posture, segment_frames_all2, joint_positions_all2, *motion, *motion_m, motion_positions_all2);
		interval = motion->interval;
		FramePartitionApp(body, joint_positions_all, joint_positions_all_master, framecom, framepoint, motion_positions_all, motion_positions_all_master,interval);
		//加速度比較D1α
		interval = motion_m->interval;
		Evaluation_acceleration(joint_positions_all_master, motion_positions_all_master, MasterFramePartition, important_acc_master, important_Aacc_master, important_acc_masterframe, important_Aacc_masterframe, interval);
		interval = motion->interval;
		Evaluation_acceleration(joint_positions_all, motion_positions_all, framepoint, important_acc, important_Aacc, important_accframe, important_Aaccframe, interval);
		Evaluation_acc_value(important_acc_master, important_Aacc_master, important_acc, important_Aacc, acc_Evalue, Aacc_Evalue);
		//軌道比較D1
		Evaluation_DTW(joint_positions_all_master, joint_positions_all2, MasterFramePartition, framepoint,important_DTW);
		Evaluation_DTW_value(important_DTW, DTW_Evalue, 0.02);

		//姿勢比較D2γ
		MotionComparison(cosine_similarity_Stationary, cosine_similarity_Active, framepoint, Active_master_frame, Active_frame, joint_positions_all, joint_positions_all_master);	
		Modification_place(joint_direction_s, joint_direction_a, framepoint, Active_master_frame, Active_frame, joint_positions_all, joint_positions_all_master);
		Evaluation_angle(cosine_similarity_Stationary, cosine_similarity_Active, COSs_Evalue, COSa_Evalue);
		//総合評価D_total=αD1+βD2(α+β=1.0)
		ComprehensiveEvaluation(score,total_score, COSs_Evalue, COSa_Evalue, acc_Evalue, Aacc_Evalue, DTW_Evalue);
		out_of_filehyoukachi(COSs_Evalue, COSa_Evalue, acc_Evalue, Aacc_Evalue, DTW_Evalue,score,total_score);
		//修正方法提示手法	
		FindFixesPoint(cosine_similarity_Stationary, cosine_similarity_Active, joint_direction_s, joint_direction_a, COSs_Evalue, acc_Evalue, Aacc_Evalue, DTW_Evalue, COSa_Evalue, j_number_acc, j_number_angle, g_number_acc, g_number_angle, k_number_acc, k_number_angle, u_number_acc, u_number_angle, d_number_acc, d_number_angle, h_number_acc, h_number_angle, ke_number_acc, ke_number_angle,j_number_anglep, g_number_anglep,k_number_anglep, u_number_anglep, d_number_anglep,h_number_anglep,ke_number_anglep,
			j_number_anglej,g_number_anglej,k_number_anglej, u_number_anglej, d_number_anglej, h_number_anglej, ke_number_anglej);




}

double ipow(double p, int n)
{
	int k;
	double s = 1;

	for (k = 1; k <= n; k++)
		s *= p;

	return s;
}
void CreateSentence(){






}

void FindFixesPoint(vector<float>cosine_similarity_Stationary, vector<float>cosine_similarity_Active, vector<Vector3f> & joint_direction_s, vector<Vector3f> & joint_direction_a, vector<float> COSs_Evalue, vector<float> acc_Evalue, vector<float> Aacc_Evalue, vector<float> DTW_Evalue, vector<float> COSa_Evalue,
	vector<int>& j_number_acc, vector<int>&j_number_angle, vector<int>&g_number_acc, vector<int>&g_number_angle, vector<int>&k_number_acc, vector<int>&k_number_angle, vector<int>&u_number_acc, vector<int>&u_number_angle, vector<int>&d_number_acc, vector<int>&d_number_angle, vector<int>&h_number_acc, vector<int>&h_number_angle, vector<int>&ke_number_acc, vector<int>&ke_number_angle,
	vector<int>&j_number_anglep, vector<int>&g_number_anglep, vector<int>&k_number_anglep, vector<int>&u_number_anglep, vector<int>&d_number_anglep, vector<int>&h_number_anglep, vector<int>&ke_number_anglep,
	vector<Vector3f>&j_number_anglej, vector<Vector3f>&g_number_anglej, vector<Vector3f>&k_number_anglej, vector<Vector3f>&u_number_anglej, vector<Vector3f>&d_number_anglej, vector<Vector3f>&h_number_anglej, vector<Vector3f>&ke_number_anglej){
	int a[14] = { 1, 6, 2, 6, 1, 2, 3, 2, 1, 2, 1, 2, 4, 6 };
	int b[7] = { 4, 3, 3, 3, 4, 4, 1 };
	float Angle = 0.0, Acc = 0.0;
	int cnt1 = 0, cnt2 = 0;
	float score_a = 0.0;
	float score_b = 0.0;
	vector<float> acc_all;
	vector<float> Angle_all;
	vector<float> Angle_p;
	vector<float> Angle_j;
	float min_acc = 11111.0;
	float min_Angle = 11111.0;
	vector<int> min_number_acc;
	vector<int> min_number_Angle;
	vector<int> number_j;
	vector<Vector3f> p;
	for (int i = 0; i < 7; i++){
		while (min_number_acc.size() != 0){
			min_number_acc.pop_back();
		}
		while (min_number_Angle.size() != 0){
			min_number_Angle.pop_back();
		}
		while (Angle_all.size() != 0){
			Angle_all.pop_back();
		}
		while (acc_all.size() != 0){
			acc_all.pop_back();
		}
		for (int j = 0; j < 5; j++){
			//静止姿勢
			Angle += COSs_Evalue[i * 5 + j];
			Angle_all.push_back(COSs_Evalue[i * 5 + j]);
		}
		for (int j = 0; j < 5; j++){
			//動作姿勢
			if (a[i * 2] != j&&a[i * 2 + 1] != j){
				Angle += COSa_Evalue[i * 5 + j];
				Angle_all.push_back(COSa_Evalue[i * 5 + j]);
			}
			else{
				Angle += DTW_Evalue[cnt1];
				Angle_all.push_back(DTW_Evalue[cnt1]);
				cnt1++;
			}
		}
		for (int j = 0; j < b[i]; j++){
			//加速度
			Acc += acc_Evalue[cnt2 + j];
			acc_all.push_back(acc_Evalue[cnt2 + j]);
		}
		cnt2 += b[i];
		for (int j = 0; j < 2; j++){
			//角加速度
			if (i != 6){
				Acc += Aacc_Evalue[i * 2 + j];
				acc_all.push_back(Aacc_Evalue[i * 2 + j]);
			}
		}
		//平均値
		for (int l = 0; l < 3; l++){
			Angle = 0.0f;
			Acc = 0.0f;
			for (int m = 0; m < Angle_all.size(); m++){
				Angle += Angle_all[m];
			}
			for (int m = 0; m < acc_all.size(); m++){
				Acc += acc_all[m];
			}
			Angle /= 10;
			if (i!=6){
				Acc /= (b[i] + 2);
			}
			else{
				Acc /= b[i];
			}
			score_a = Alpha*Angle;
			score_b = Beta*Acc;
			if (score_a>score_b){
				min_number_acc.push_back(0);
				for (int k = 0; k < acc_all.size(); k++){
					if (min_acc>acc_all[k]){
						min_acc = acc_all[k];
						min_number_acc.pop_back();
						min_number_acc.push_back(k);
					}
				}
				acc_all[min_number_acc[min_number_acc.size()-1]] = 1.0;
			}
			else if (score_a < score_b){
				min_number_Angle.push_back(0);
				for (int k = 0; k < Angle_all.size(); k++){
					if (min_Angle>Angle_all[k]){
						min_Angle = Angle_all[k];
						min_number_Angle.pop_back();
						min_number_Angle.push_back(k);
					}
				}
				Angle_all[min_number_Angle[min_number_Angle.size()-1]] = 1.0;
			}
			else{
				min_number_acc.push_back(0);
				for (int k = 0; k < acc_all.size(); k++){
					if (min_acc>acc_all[k]){
						min_acc = acc_all[k];
						min_number_acc.pop_back();
						min_number_acc.push_back(k);
					}
				}
					min_number_Angle.push_back(0);
					for (int k = 0; k < Angle_all.size(); k++){
						if (min_Angle>Angle_all[k]){
							min_Angle = Angle_all[k];
							min_number_Angle.pop_back();
							min_number_Angle.push_back(k);
						}
					}
						if (Alpha*min_Angle > Beta*min_acc){
							min_number_Angle.pop_back();
							acc_all[min_number_acc[min_number_acc.size()-1]] = 1.0;

						}
						else{
							min_number_acc.pop_back();
							Angle_all[min_number_Angle[min_number_Angle.size()-1]] = 1.0;
						}
					
				
			}
			min_acc = 1111.0;
			min_Angle = 1111.0;

		}
		for (int o = 0; o < min_number_Angle.size(); o++){
			if (min_number_Angle[o] < 5){
				if (cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i] == min({ cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i], cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i + 1], cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i + 2] })){
					number_j.push_back(min_number_Angle[o] * 3);
					p.push_back(joint_direction_s[min_number_Angle[o] * 3 + 15 * i]);
				}
				else if (cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i + 1] == min({ cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i], cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i + 1], cosine_similarity_Stationary[min_number_Angle[o] * 3 + 15 * i + 2] })){
					number_j.push_back(min_number_Angle[o] * 3 + 1);
					p.push_back(joint_direction_s[min_number_Angle[o] * 3 + 15 * i + 1]);
				}
				else{
					number_j.push_back(min_number_Angle[o] * 3 + 2);
					p.push_back(joint_direction_s[min_number_Angle[o] * 3 + 15 * i + 2]);
				}
			}
			else{
				if (cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i] == min({ cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i], cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i + 1], cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i + 2] })){
					number_j.push_back((min_number_Angle[o] - 5) * 3);
					p.push_back(joint_direction_s[(min_number_Angle[o] - 5) * 3 + 15 * i]);
				}
				else if (cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i + 1] == min({ cosine_similarity_Active[(min_number_Angle[o] - 5) * 3 + 15 * i], cosine_similarity_Active[(min_number_Angle[o] - 5 )* 3 + 15 * i + 1], cosine_similarity_Active[(min_number_Angle[o] - 5 )* 3 + 15 * i + 2] })){
					number_j.push_back((min_number_Angle[o] - 5) * 3 + 1);
					p.push_back(joint_direction_s[(min_number_Angle[o] - 5) * 3 + 15 * i + 1]);
				}
				else{
					number_j.push_back((min_number_Angle[o] - 5) * 3 + 2);
					p.push_back(joint_direction_s[(min_number_Angle[o] - 5) * 3 + 15 * i + 2]);
				}
			}
		}
			if (i == 0){
				copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(j_number_acc));
				copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(j_number_angle));
				copy(number_j.begin(), number_j.end(), back_inserter(j_number_anglep));
				copy(p.begin(), p.end(), back_inserter(j_number_anglej));
			}
		else if (i == 1){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(g_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(g_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(g_number_anglep));
			copy(p.begin(), p.end(), back_inserter(g_number_anglej));
		}
		else if (i == 2){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(k_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(k_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(k_number_anglep));
			copy(p.begin(), p.end(), back_inserter(k_number_anglej));
		}
		else if (i == 3){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(u_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(u_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(u_number_anglep));
			copy(p.begin(), p.end(), back_inserter(u_number_anglej));
		}
		else if (i == 4){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(d_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(d_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(d_number_anglep));
			copy(p.begin(), p.end(), back_inserter(d_number_anglej));
		}
		else if (i == 5){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(h_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(h_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(h_number_anglep));
			copy(p.begin(), p.end(), back_inserter(h_number_anglej));
		}
		else if (i == 6){
			copy(min_number_acc.begin(), min_number_acc.end(), back_inserter(ke_number_acc));
			copy(min_number_Angle.begin(), min_number_Angle.end(), back_inserter(ke_number_angle));
			copy(number_j.begin(), number_j.end(), back_inserter(ke_number_anglep));
			copy(p.begin(), p.end(), back_inserter(ke_number_anglej));
		}

	}





}
void Modification_place(vector<Vector3f> & joint_direction_c, vector<Vector3f> & joint_direction_a, vector<int>& framePoint, vector<int>& Active_masterframe, vector<int>& Active_frame, vector< Point3f> & joi_pos_array, vector< Point3f> & joi_pos_array_master){
	vector<Vector3f> joint_vector;
	vector<Vector3f> joint_vector2;
	Vector3f* r1 = new Vector3f();
	Vector3f* r2 = new Vector3f();
	Vector3f* r1_dash = new Vector3f();
	Vector3f* r2_dash = new Vector3f();
	Vector3f* r_cross = new Vector3f();
	Matrix3f* Rx = new Matrix3f();
	Matrix3f* Ry = new Matrix3f();
	Quat4f* r_q = new Quat4f();
	Quat4f* r_p = new Quat4f();
	Quat4f* r_quat = new Quat4f();
	Quat4f* r_conjugate = new Quat4f();
	Vector3f* z_axis = new Vector3f(0.0, 0.0, 1.0);
	//マスターデータのフレーム分割点0-後ろ下がり200-順突362-逆突き460-鍵突579-上受749-同時受868-払い受け969-蹴り1107-後ろ下がり1277-構え
	int MasterFramePartition[10] = { 0, 200, 362, 460, 579, 749, 868, 969, 1107, 1277 };
	//仮
	int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	int cnt = 0, cnt2 = 0, cnt3 = 0;
	int a[16] = { 7, 2, 11, 2, 7, 2, 11, 2, 11, 2, 11, 2, 19, 0, 3, 0 };
	float b = 0.0;
	float zz = 0.0;
	float angle_r1 = 0.0;
	float angle_r2 = 0.0;
	float tmp = 0.0;
	//~~~~~~~~~比較フレーム取得~~~~~~~~~~~~



	for (int i = 0; i < 7; i++){

		for (int j = 0; j < 20; j++){
			if (j % 4 != 3){
				if (j == 13 || j == 17){
					r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
					r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
					r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
					r1_dash->x = joi_pos_array[framePoint[i + 2] * 20 + j - 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
					r1_dash->y = joi_pos_array[framePoint[i + 2] * 20 + j - 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
					r1_dash->z = joi_pos_array[framePoint[i + 2] * 20 + j - 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
					r2->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
					r2->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
					r2->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
					r2_dash->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
					r2_dash->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
					r2_dash->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
					angle_r1 = CosineSimilarity(*r1, *r1_dash);
					angle_r2 = CosineSimilarity(*r2, *r2_dash);
						joint_direction_c[cnt2].z=angle_r2-angle_r1;
						cnt2++;
				
					cnt++;
					angle_r1 = 0.0;
					angle_r2 = 0.0;
				}
				else{
					r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
					r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
					r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
					r2->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
					r2->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
					r2->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
					
						//前後判定（Xの値が前の場合マイナス後ろの場合プラス）左右判定（Zの値が左の場合マイナス右の場合プラス）
					r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
					r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
					r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
					r1->x = 0.0;
						//r1->x = 0.0;
						r1->normalize();
						r_cross->cross(*r1, *z_axis);
						if (r_cross->x < 0.0){
							//前側にある
							Rx->rotX(-1 * r1->angle(*z_axis));
						}
						else{
							//後側にある
							Rx->rotX(r1->angle(*z_axis));
						}
						tmp = r1->angle(*z_axis);
						tmp = z_axis->angle(*r1);
						r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
						r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
						r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
						Rx->transform(r1);
						r_cross->cross(*r1, *z_axis);
						if (r_cross->y < 0.0){
							//左側にある
							Ry->rotY(-1 * r1->angle(*z_axis));
						}
						else{
							//右側にある
							Ry->rotY(r1->angle(*z_axis));
						}
						tmp = r1->angle(*z_axis);
						tmp = z_axis->angle(*r1);
						r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
						r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
						r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
						r1->normalize();
						r2->normalize();
						Rx->transform(r1);
						Rx->transform(r2);
						Ry->transform(r1);
						Ry->transform(r2);
						joint_direction_c[cnt2].cross(*z_axis, *r2);
						cnt2++;
					
				}
			}
		}
		cnt = 0;

		for (int j = 0; j < 20; j++){
			if (j % 4 != 3){
				if (j == 13 || j == 17){
					r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					r1_dash->x = joi_pos_array[(Active_frame[i]) * 20 + j - 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1_dash->y = joi_pos_array[(Active_frame[i]) * 20 + j - 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1_dash->z = joi_pos_array[(Active_frame[i]) * 20 + j - 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					r2->x = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					r2->y = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					r2->z = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					r2_dash->x = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					r2_dash->y = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					r2_dash->z = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					angle_r1 = CosineSimilarity(*r1, *r1_dash);
					angle_r2 = CosineSimilarity(*r2, *r2_dash);
					joint_direction_a[cnt3].z = angle_r2 - angle_r1;
					cnt3++;
					cnt++;
					angle_r1 = 0.0;
					angle_r2 = 0.0;
				}
				else{
					r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					r2->x = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					r2->y = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					r2->z = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					//前後判定（Xの値が前の場合マイナス後ろの場合プラス）左右判定（Zの値が左の場合マイナス右の場合プラス）
					r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					r1->x = 0.0;
					//r1->x = 0.0;
					r1->normalize();
					r_cross->cross(*r1, *z_axis);
					if (r_cross->x < 0.0){
						//前側にある
						Rx->rotX(-1 * r1->angle(*z_axis));
					}
					else{
						//後側にある
						Rx->rotX(r1->angle(*z_axis));
					}
					tmp = r1->angle(*z_axis);
					tmp = z_axis->angle(*r1);
					r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					Rx->transform(r1);
					r_cross->cross(*r1, *z_axis);
					if (r_cross->y < 0.0){
						//左側にある
						Ry->rotY(-1 * r1->angle(*z_axis));
					}
					else{
						//右側にある
						Ry->rotY(r1->angle(*z_axis));
					}
					tmp = r1->angle(*z_axis);
					tmp = z_axis->angle(*r1);
					r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					r1->normalize();
					r2->normalize();
					Rx->transform(r1);
					Rx->transform(r2);
					Ry->transform(r1);
					Ry->transform(r2);
					joint_direction_a[cnt3].cross(*z_axis, *r2);
					cnt3++;
				}
			}

		}


	}

}
void ComprehensiveEvaluation(vector<float> & score, float & total_score, vector< float> & cosine_similarity_Stationary, vector< float> & cosine_similarity_Active, vector< float> & acc, vector< float> & A_acc, vector< float> & DTW){
	int a[14] = {1,6,2,6,1,2,3,2,1,2,1,2,4,6 };
	int b[7] = {4,3,3,3,4,4,1};
	float Angle = 0.0, Acc = 0.0;
	int cnt1 = 0,cnt2=0;
	for (int i = 0; i < 7; i++){
		for (int j = 0; j < 5; j++){
			//静止姿勢
			Angle += cosine_similarity_Stationary[i * 5 + j];
		}
		for (int j = 0; j < 5; j++){
			//動作姿勢
			if (a[i * 2] != j&&a[i * 2 + 1] != j){
				Angle += cosine_similarity_Active[i * 5 + j];
			}
			else{
				Angle += DTW[cnt1];
				cnt1++;
			}
		}
		for (int j = 0; j < b[i]; j++){
			//加速度
			Acc += acc[cnt2 + j];
		}
		cnt2 += b[i];
		for (int j = 0; j < 2; j++){
			//角加速度
			if (i != 6)
			Acc += A_acc[i * 2 + j];
		}
		//平均値
		Angle /= 10;
		if (i!=6){
				Acc /= (b[i] + 2);
			}
			else{
				Acc /= b[i];
			}
		score[i] = Alpha*Angle + Beta*Acc;
		total_score += score[i];
		Angle = 0.0;
		Acc = 0.0;
	}
	total_score /= 7;
}
void Evaluation_angle(vector<float> & cos_s, vector<float> & cos_a, vector<float> & E_cos_s, vector<float> & E_cos_a){
	int cnt=0;
	for (int i = 0; i < cos_s.size(); i++){
		//cos_s[i] = (cos_s[i] + 1.0) / 2.0;
		if (i % 15 == 0){
			if (cos_s[i]>SpineThreshold*M_PI/180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= SpineThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>SpineThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= SpineThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 1){
			if (cos_s[i]>Spine1Threshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= Spine1Threshold*M_PI / 180.0;
			}

			if (cos_a[i]>Spine1Threshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= Spine1Threshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 2){
			if (cos_s[i]>NeckThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= NeckThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>NeckThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= NeckThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 3 || i % 15 == 6){
			if (cos_s[i]>ShoulderThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= ShoulderThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>ShoulderThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= ShoulderThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 4 || i % 15 == 7){
			if (cos_s[i]>ArmThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= ArmThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>ArmThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= ArmThreshold*M_PI / 180.0;
			}

		}	
		if (i % 15 == 5 || i % 15 == 8){
			if (cos_s[i]>ForeArmThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= ForeArmThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>ForeArmThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= ForeArmThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 9 || i % 15 == 12){
			if (cos_s[i]>UplegThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= UplegThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>UplegThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= UplegThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 10 || i % 15 == 13){
			if (cos_s[i]>LegThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= LegThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>LegThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= LegThreshold*M_PI / 180.0;
			}

		}
		if (i % 15 == 11 || i % 15 == 14){
			if (cos_s[i]>FootThreshold*M_PI / 180.0){
				//cos_s[i] = M_PI/2;
				cos_s[i] = 1;
			}
			else{
				cos_s[i] /= FootThreshold*M_PI / 180.0;
			}

			if (cos_a[i]>FootThreshold*M_PI / 180.0){
				//cos_a[i] = M_PI/2;
				cos_a[i] = 1;
			}
			else{
				cos_a[i] /= FootThreshold*M_PI / 180.0;
			}

		}
			cos_s[i] = 1 - cos_s[i];
			cos_a[i] = 1 - cos_a[i];
	}

	for (int i = 0; i < E_cos_s.size(); i++){
		E_cos_s[i] = (cos_s[i*3] + cos_s[i*3 + 1] + cos_s[i*3 + 2]) / 3.0;
	}
	for (int i = 0; i < E_cos_a.size(); i++){
		E_cos_a[i] = (cos_a[i*3] + cos_a[i*3 + 1] + cos_a[i*3 + 2]) / 3.0;
	}

}
void Evaluation_acc_value(vector<float>& m_acc, vector<float>& m_A_acc, vector<float>& acc, vector<float>& A_acc, vector<float>& E_accvalue, vector<float>& E_Aaccvalue){
	for (int i = 0; i < m_acc.size(); i++){
		if (fabsf(acc[i]) > fabsf(m_acc[i])){
			E_accvalue[i] = 1.0;
		}
		else{
			E_accvalue[i] = fabsf(acc[i]) / fabsf(m_acc[i]);
		}
	}
	for (int i = 0; i < m_A_acc.size(); i++){
		if (fabsf(A_acc[i]) > fabsf(m_A_acc[i])){
			E_Aaccvalue[i] = 1.0;
		}
		else{
			E_Aaccvalue[i] = fabsf(A_acc[i]) / fabsf(m_A_acc[i]);
		}
	}
}
void Evaluation_SD_value(vector<float>& m_SD, vector<float>& SD, vector<float>& E_SDvalue){
	for (int i = 0; i < m_SD.size(); i++){
		if (SD[i] < m_SD[i]){
			E_SDvalue[i] = 1.0;
		}
		else{
			E_SDvalue[i] = m_SD[i]/ SD[i];
		}
	}

}

void Evaluation_DTW_value(vector<float>& DTW, vector<float>& E_DTWvalue, float  weight){
	for (int i = 0; i < DTW.size(); i++){
		E_DTWvalue[i] = 1.0 / (1 + weight*DTW[i]);
	}
}

float ED(Point3f a, Point3f b){
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)+pow(a.z - b.z, 2));
}

float DTW(vector<Point3f>  ts_a,vector<Point3f>  ts_b,int window){
	//動的伸縮法
	int ts_a_len=0;
	int ts_b_len=0;
	int windowstart = 0;
	int windowend = 0;

	ts_a_len = ts_a.size();
	ts_b_len = ts_b.size();
	vector<vector<float>> cost;
	vector<vector<float>> dist; 

	if (window <= 0){
		window = max(ts_a_len,ts_b_len);
	}

	cost.resize(ts_a_len);
	dist.resize(ts_a_len);
	for (int i = 0; i < ts_a_len;i++){
		cost[i].resize(ts_b_len);
		dist[i].resize(ts_b_len);
	}

	cost[0][0] = fabsf(ED(ts_a[0],ts_b[0]));
	dist[0][0] = fabsf(ED(ts_a[0], ts_b[0]));
	for (int i = 1; i < ts_a_len;i++){
		cost[i][0] = fabsf(ED(ts_a[i], ts_b[0]));
		dist[i][0] = dist[i-1][0]+cost[i][0];
	}
	for (int j = 1; j < ts_b_len; j++){
		cost[0][j] = fabsf(ED(ts_a[0], ts_b[j]));
		dist[0][j] = dist[0][j-1] + cost[0][j];
	}

	for (int i = 1; i < ts_a_len; i++){
		windowstart = max(1,i-window);
		windowend = min(ts_b_len, i + window);
		for (int j = windowstart; j < windowend;j++){
			cost[i][j] = fabsf(ED(ts_a[i], ts_b[j]));

			dist[i][j] = min({ dist[i - 1][j], dist[i][j - 1], dist[i - 1][j - 1] }) + cost[i][j];
		}
	}
	return dist[ts_a_len-1][ts_b_len-1];
}

void calcLeastSquaresMethod(vector<Point3f> r1, vector<int> r2, vector<float> & r3)
{//最小二乗法

	double a[M + 1][M + 2], s[2 * M + 1], t[M + 1];
	int i, j, k;
	double p, d, px;
	// 測定データ
	static double x[] = { -3, -2, -1, 0, 1, 2, 3 };
	static double y[] = { 5, -2, -3, -1, 1, 4, 5 };

	for (i = 0; i <= 2 * M; i++)
		s[i] = 0;
	for (i = 0; i <= M; i++)
		t[i] = 0;


	// s[], t[] 計算
	for (i = 0; i < r1.size(); i++) {
		for (j = 0; j <= 2 * M; j++)
			s[j] += ipow(r2[i], j);
		for (j = 0; j <= M; j++)
			t[j] += ipow(r2[i], j) * r1[i].x;
	}

	// a[][] に s[], t[] 代入
	for (i = 0; i <= M; i++) {
		for (j = 0; j <= M; j++)
			a[i][j] = s[i + j];
		a[i][M + 1] = t[i];
	}

	// 掃き出し
	for (k = 0; k <= M; k++) {
		p = a[k][k];
		for (j = k; j <= M + 1; j++)
			a[k][j] /= p;
		for (i = 0; i <= M; i++) {
			if (i != k) {
				d = a[i][k];
				for (j = k; j <= M + 1; j++)
					a[i][j] -= d * a[k][j];
			}
		}
	}

	// y 値計算＆結果出力
	for (k = 0; k <= M; k++)
		printf("a%d = %10.6f\n", k, a[k][M + 1]);
	printf("    x    y\n");
	for (px = r2[0]; px <= r2[r2.size()-1]; px += 1) {
		p = 0;
		for (k = 0; k <= M; k++)
			p += a[k][M + 1] * ipow(px, k);
		r3[px] = p;
	}
}

void orbit(vector< Point3f>  joi_pos_array, vector<Point3f>& a,int start,int end,int joint_number){
	for (int i = 0; i < end-start; i++){
		a[i] = joi_pos_array[(i + start) * 20 + joint_number] - joi_pos_array[start * 20 + joint_number];

	}

}

void out_of_filehyoukachi(vector< float> & cosine_similarity_Stationary, vector< float> & cosine_similarity_Active, vector< float> & acc, vector< float> & A_acc, vector< float> & DTW, vector<float>& score, float & total_score){
	int i, j = 0;
	int cnt = 0;
	int a[7] = {4,7,10,13,17,21,22};

	score.resize(7);

	ofstream ofs("評価値.csv"); //ファイル出力ストリーム

	ofs << "角度" ",";
	ofs << endl;
	ofs << "背中"  "," "左手"  "," "右手" "," "左足" "," "右足" ",";
	ofs << endl;

	for (i = 0; i < cosine_similarity_Stationary.size(); i++){
		ofs << cosine_similarity_Stationary[i];
		ofs << ",";
		cnt++;
		if (cnt==5){
			ofs << endl;
			cnt = 0;
		}

	}
	cnt = 0;
	ofs << endl;
	ofs << endl;
	ofs << "背中"  "," "左手"  "," "右手" "," "左足" "," "右足" ",";
	ofs << endl;
	for (i = 0; i < cosine_similarity_Active.size(); i++){
		ofs << cosine_similarity_Active[i];
		ofs << ",";

		cnt++;
		if (cnt == 5){
			ofs << endl;
			cnt = 0;
		}

	}
	cnt = 0;
	ofs << endl;
	ofs << endl;
	ofs << "加速度" ",";
	ofs << endl;
	ofs << "左足" "," "左手" "," "右手" "," "右足" ",";
	ofs << endl;
	for (i = 0; i < acc.size(); i++){
		ofs << acc[i];
		ofs << ",";
		if (a[cnt] == i+1){
			cnt++;
			if (cnt == 1){
				ofs << endl;
				ofs << "右手" "," "左手" "," "右足" ",";
				ofs << endl;
			}
			else if (cnt == 2){
				ofs << endl;
				ofs << "左足" "," "右手" "," "左手" ",";
				ofs << endl;
			}
			else if (cnt == 3){
				ofs << endl;
				ofs << "左足" "," "右手" "," "右足" ",";
				ofs << endl;
			}
			else if (cnt ==4){
				ofs << endl;
				ofs << "左足"  "," "左手""," "右手" "," "右足" ",";
				ofs << endl;
			}
			else if (cnt == 5){
				ofs << endl;
				ofs << "左足"  "," "左手""," "右手" "," "右足" ",";
				ofs << endl;
			}
			else if (cnt == 6){
				ofs << endl;
				ofs << "右足" ;
				ofs << endl;
			}
		}

	}
	ofs << endl;
	ofs << endl;
	ofs << "角加速度" ",";
	ofs << endl;
	ofs << "順突腰" ",""順突肩" ",""逆突腰" ",""逆突肩" ",""鈎突腰" ",""鈎突肩" ",""上受腰" ",""上受肩" ",""同時受腰" ",""同時受肩" ",""払受腰" ",""払受肩" ",";
	ofs << endl;
	for (i = 0; i < A_acc.size(); i++){
		ofs << A_acc[i];
		ofs << ",";


	}
	ofs << endl;
	ofs << endl;
	ofs << "軌跡" ",";
	ofs << endl;
	ofs << "順突左手"",""逆突右手" ",""鈎突左手"",""鈎突右手" ",""上受左足" ",""上受右手"",""同時受左手"",""同時受右手"",""払受左手" ",""払受右手" ",""蹴上右足" ",";
	ofs << endl;
	for (i = 0; i < DTW.size(); i++){
		ofs << DTW[i];
		ofs << ",";



	}
	ofs << endl;
	ofs << endl;
	ofs << "総合評価" ",";
	ofs << endl;
	ofs << "順突"",""逆突" ",""鈎突" ",""上受" ",""同時受"",""払受"",""蹴上" ",""総合評価" ",";
	ofs << endl;
	for (i = 0; i < score.size(); i++){
		ofs << score[i];
		ofs << ",";
	}
	ofs <<total_score;
	ofs << ",";

	
	return;



}

void out_of_file4(vector< float> & ruijido1, vector< float> & ruijido2, vector< float> & ruijido3, vector<float> & ruijido4, vector<float> & ruijido5){
	int i, j = 0;

	
	
	ofstream ofs("ruijido.csv"); //ファイル出力ストリーム
	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
	/*for (i = 0; i < ((num_channel - 3) / 3) - 1; i++)
	{

		for (j = 0; j < num_frame - 1; j++)
		{
			ofs << sita_boxq[i*(num_frame - 1) + j];
			ofs << ",";
		}
		ofs << endl;
	}*/
	for (i = 0; i < ruijido1.size(); i++){
		ofs << ruijido1[i];
		ofs << ",";
	}
	ofs << endl;
	for (i = 0; i < ruijido2.size(); i++){
		ofs << ruijido2[i];
		ofs << ",";
	}
	ofs << endl;
	for (i = 0; i < ruijido3.size(); i++){
		ofs << ruijido3[i];
		ofs << ",";
	}
	ofs << endl;
	for (i = 0; i < ruijido4.size(); i++){
		ofs << ruijido4[i];
		ofs << ",";
	}
	ofs << endl;
	for (i = 0; i < ruijido5.size(); i++){
		ofs << ruijido5[i];
		ofs << ",";
	}
	ofs << endl;
	return;



}
void out_of_fileD(vector< double> & D){
	int j = 0;

		ofstream ofs("D.csv"); //ファイル出力ストリーム

		for (j = 0; j < D.size();j++){
			ofs << D[j];
			ofs << ",";
		}
		ofs << endl;
	return;
}
void out_of_fileD1(vector< double> & D){
	int j = 0;

	ofstream ofs("D1.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_fileD2(vector< double> & D){
	int j = 0;

	ofstream ofs("D2.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_fileD3(vector< double> & D){
	int j = 0;

	ofstream ofs("D3.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_fileD4(vector< double> & D){
	int j = 0;

	ofstream ofs("D4.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_fileD5(vector< double> & D){
	int j = 0;

	ofstream ofs("D5.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_fileD6(vector< double> & D){
	int j = 0;

	ofstream ofs("D6.csv"); //ファイル出力ストリーム

	for (j = 0; j < D.size(); j++){
		ofs << D[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}





void simulate_trackball(quat* q, float p1x, float p1y, float p1z, float p2x, float p2y, float p2z){
	quat p1, p2, a, d;
	float s, t;
	double R = 1.0;
	quat_assign(&p1,0.0,p1x,p1y,p1z);
	quat_assign(&p2, 0.0, p2x, p2y, p2z);
	quat_mul(&a,&p1,&p2);
	a.w = 0.0;
	s = quat_norm(&a);
	quat_div_real(&a,&a,s);
	quat_sub(&d,&p1,&p2);
	t = quat_norm(&d)/(2.0*R*ROOT_2_INV);
	if (t > 1.0)t = 1.0;
	quat_assign(q,cos(asin(t)),a.x*t,a.y*t,a.z*t);
}

void out_of_file_nejiremaster(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector< Matrix4f > & seg_frame_array, int frame_before, int frame_after,float interval){
	//角速度を取得
	vector<Point3f> r1;
	vector<float> r2_x;
	vector<float> r2_y;
	vector<float> r2_z;
	vector<float> r3_x;
	vector<float> r3_y;
	vector<float> r3_z;
	vector<float> r4;
	vector<float> r4_r;
	vector<float> r5;
	vector<float> r5_r;
	Vector3f *r4_vec1 = new Vector3f();
	Vector3f *r4_vec2 = new Vector3f();
	vector<quat> q_r4;
	double threshold = 0.001;
	/*vector<Point3f> r3;
	vector<Point3f> r4;
	vector<Point3f> r5;
	vector<Point3f> r6;
	vector<Point3f> r7;
	vector<Point3f> r8;
	vector<Point3f> r9;
	vector<Point3f> r10;
	vector<Point3f> r11;
	vector<Point3f> r12;
	vector<Point3f> r13;
	vector<Point3f> r14;
	vector<Point3f> r15;
	vector<Point3f> r16;
	vector<Point3f> r17;
	vector<Point3f> r18;
	vector<Point3f> r19;
	vector<Point3f> r20;
	vector<Point3f> r21;
	vector<Point3f> r22;
	vector<int> r23;
	vector<float> r24;
	vector<Point3f> r25;
	vector<Point3f> r26;
	vector<Point3f> r27;
	vector<Point3f> r28;frame_after-frame_before*/
	r1.resize(frame_after - frame_before);
	r2_x.resize((frame_after - frame_before));
	r2_y.resize((frame_after - frame_before));
	r2_z.resize((frame_after - frame_before));
	r3_x.resize((frame_after - frame_before)-1);
	r3_y.resize((frame_after - frame_before)-1);
	r3_z.resize((frame_after - frame_before)-1);
	r4.resize((frame_after - frame_before) - 1);
	r4_r.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before) - 1);
	r5_r.resize((frame_after - frame_before) - 1);
	q_r4.resize((frame_after - frame_before) - 1);
	/*r3.resize(joi_pos_array.size() / 21);
	r4.resize(joi_pos_array.size() / 21);
	r5.resize(joi_pos_array.size() / 20);
	r6.resize(joi_pos_array.size() / 20);
	r7.resize(joi_pos_array.size() / 20);
	r8.resize(joi_pos_array.size() / 20);
	r9.resize(joi_pos_array.size() / 20);
	r10.resize(joi_pos_array.size() / 20);
	r11.resize(joi_pos_array.size() / 20);
	r12.resize(joi_pos_array.size() / 20);
	r13.resize(joi_pos_array.size() / 20);
	r14.resize(joi_pos_array.size() / 20);
	r15.resize(joi_pos_array.size() / 20);
	r16.resize(joi_pos_array.size() / 20);
	r17.resize(joi_pos_array.size() / 20);
	r18.resize(joi_pos_array.size() / 20);
	r19.resize(joi_pos_array.size() / 20);
	r20.resize(joi_pos_array.size() / 20);
	r21.resize(joi_pos_array.size() / 20);
	r22.resize(joi_pos_array.size() / 20);
	r23.resize(joi_pos_array.size() / 20);
	r24.resize(joi_pos_array.size() / 20);
	r25.resize(joi_pos_array.size() / 20);
	r26.resize(joi_pos_array.size() / 20);
	r27.resize(joi_pos_array.size() / 20);
	r28.resize(joi_pos_array.size() / 20);*/

	for (int i = 0; i <(frame_after - frame_before)-1; i++){
		r1[i] = joi_pos_array[(i + frame_before) * 20 + 7] - motion_positions_array[i];
		//r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i] - (joi_pos_array[15] - motion_positions_array[0]);

		//r1[i] = asin(seg_frame_array[(i + frame_before) * 21].m21);
		//r2[i] = (asin(seg_frame_array[(i + frame_before) * 21+21].m21) - asin(seg_frame_array[(i + frame_before) * 21].m21))/pow(interval,2);
		//r2_y.[i] = atan2(-seg_frame_array[(i + frame_before) * 21 + 21].m20, seg_frame_array[(i + frame_before) * 21 + 21].m22);
		if (abs(seg_frame_array[(i + frame_before) * 21 + 21].m21 - 1.0) < threshold){ // R(2,1) = sin(x) = 1の時
			r2_x[i] = M_PI / 2;
			r2_y[i] = 0;
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m10, seg_frame_array[(i + frame_before) * 21 + 21].m00);
		}
		else if (abs(seg_frame_array[(i + frame_before) * 21 + 21].m21 + 1.0) < threshold){ // R(2,1) = sin(x) = -1の時
			r2_x[i] = -M_PI / 2;
			r2_y[i] = 0;
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m10, seg_frame_array[(i + frame_before) * 21 + 21].m00);
		}
		else{
			r2_x[i] = asin(seg_frame_array[(i + frame_before) * 21 + 21].m21);
			r2_y[i] = atan2(-seg_frame_array[(i + frame_before) * 21 + 21].m20, seg_frame_array[(i + frame_before) * 21 + 21].m22);
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m01, seg_frame_array[(i + frame_before) * 21 + 21].m11);
		}

		if (abs(seg_frame_array[(i + frame_before) * 21 ].m21 - 1.0) < threshold){ // R(2,1) = sin(x) = 1の時
			r3_x[i] = M_PI / 2;
			r3_y[i] = 0;
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 ].m10, seg_frame_array[(i + frame_before) * 21 ].m00);
		}
		else if (abs(seg_frame_array[(i + frame_before) * 21 ].m21 + 1.0) < threshold){ // R(2,1) = sin(x) = -1の時
			r3_x[i] = -M_PI / 2;
			r3_y[i] = 0;
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21].m10, seg_frame_array[(i + frame_before) * 21 ].m00);
		}
		else{
			r3_x[i] = asin(seg_frame_array[(i + frame_before) * 21].m21);
			r3_y[i] = atan2(-seg_frame_array[(i + frame_before) * 21 ].m20, seg_frame_array[(i + frame_before) * 21 ].m22);
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 ].m01, seg_frame_array[(i + frame_before) * 21].m11);
		}
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 12].x - motion_positions_array[(i + frame_before)].x;
		r4_vec1->y = 0; //joi_pos_array[(i + frame_before) * 20 + 12].y - motion_positions_array[(i + frame_before)].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 12].z - motion_positions_array[(i + frame_before)].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 12].x - motion_positions_array[(i + frame_before + 1)].x;
		r4_vec2->y = 0;// joi_pos_array[(i + frame_before) * 20 + 20 + 12].y - motion_positions_array[(i + frame_before + 1)].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 12].z - motion_positions_array[(i + frame_before + 1)].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r4[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 16].x - motion_positions_array[(i + frame_before)].x;
		r4_vec1->y = 0;// joi_pos_array[(i + frame_before) * 20 + 16].y - motion_positions_array[(i + frame_before)].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 16].z - motion_positions_array[(i + frame_before)].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 16].x - motion_positions_array[(i + frame_before + 1)].x;
		r4_vec2->y = 0;// joi_pos_array[(i + frame_before) * 20 + 20 + 16].y - motion_positions_array[(i + frame_before + 1)].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 16].z - motion_positions_array[(i + frame_before + 1)].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r4_r[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 8].x - joi_pos_array[(i + frame_before) * 20 + 2].x;
		r4_vec1->y = 0;// joi_pos_array[(i + frame_before) * 20 + 8].y - joi_pos_array[(i + frame_before) * 20 + 2].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 8].z - joi_pos_array[(i + frame_before) * 20 + 2].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 8].x - joi_pos_array[(i + frame_before) * 20+20 + 2].x;
		r4_vec2->y = 0;// joi_pos_array[(i + frame_before) * 20 + 20 + 8].y - joi_pos_array[(i + frame_before) * 20 + 20 + 2].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 8].z - joi_pos_array[(i + frame_before) * 20 + 20 + 2].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r5[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 4].x - joi_pos_array[(i + frame_before) * 20 + 2].x;
		r4_vec1->y = 0;// joi_pos_array[(i + frame_before) * 20 + 4].y - joi_pos_array[(i + frame_before) * 20 + 2].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 4].z - joi_pos_array[(i + frame_before) * 20 + 2].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 4].x - joi_pos_array[(i + frame_before) * 20 + 20 + 2].x;
		r4_vec2->y = 0;// joi_pos_array[(i + frame_before) * 20 + 20 + 4].y - joi_pos_array[(i + frame_before) * 20 + 20 + 2].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 4].z - joi_pos_array[(i + frame_before) * 20 + 20 + 2].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r5_r[i] = r4_vec1->angle(*r4_vec2);
		simulate_trackball(&(q_r4[i]), r3_x[i], r3_y[i], r3_z[i], r2_x[i], r2_y[i], r2_z[i]);


		//r2[i] = joi_pos_array[i * 20 + 1] - motion_positions_array[i];
		/*r3[i] = joi_pos_array[i * 20 + 2] - motion_positions_array[i];
		r4[i] = joi_pos_array[i * 20 + 3] - motion_positions_array[i];
		r5[i] = joi_pos_array[i * 20 + 4] - motion_positions_array[i];
		r6[i] = joi_pos_array[i * 20 + 5] - motion_positions_array[i];
		r7[i] = joi_pos_array[i * 20 + 6] - motion_positions_array[i];
		r8[i] = joi_pos_array[i * 20 + 7] - motion_positions_array[i];
		r9[i] = joi_pos_array[i * 20 + 8] - motion_positions_array[i];
		r10[i] = joi_pos_array[i * 20 + 9] - motion_positions_array[i];
		r11[i] = joi_pos_array[i * 20 + 10] - motion_positions_array[i];
		r12[i] = joi_pos_array[i * 20 + 11] - motion_positions_array[i];
		r13[i] = joi_pos_array[i * 20 + 12] - motion_positions_array[i];
		r14[i] = joi_pos_array[i * 20 + 13] - motion_positions_array[i];
		r15[i] = joi_pos_array[i * 20 + 14] - motion_positions_array[i];
		r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i] - (joi_pos_array[15] - motion_positions_array[0]);
		r17[i] = joi_pos_array[i * 20 + 16] - motion_positions_array[i];
		r18[i] = joi_pos_array[i * 20 + 17] - motion_positions_array[i];
		r19[i] = joi_pos_array[i * 20 + 18] - motion_positions_array[i];
		r20[i] = joi_pos_array[i * 20 + 19] - motion_positions_array[i] - (joi_pos_array[19] - motion_positions_array[0]);
		r21[i] = r8[i] - r3[i] - (r8[0] - r3[0]);
		r22[i] = r12[i] - r3[i] - (r12[0] - r3[0]);*/

	}
	/*for (int i = 0; i < r21.size(); i++){
	r23[i] = i;
	}*/

	//calcLeastSquaresMethod( r21, r23,r24);

	ofstream ofs("joinejire_master.csv"); //ファイル出力ストリーム



	for (int j = 0; j < q_r4.size(); j++){
		ofs << q_r4[j].w;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].z;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_x.size(); j++){
		ofs << r2_x[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_y.size(); j++){
		ofs << r2_y[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_z.size(); j++){
		ofs << r2_z[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r4.size(); j++){
		ofs << r4[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r4_r.size(); j++){
		ofs << r4_r[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5_r.size(); j++){
		ofs << r5_r[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	return;
}

void out_of_file_nejire(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector< Matrix4f > & seg_frame_array, int frame_before, int frame_after, float interval){
	//角速度を取得
	vector<Point3f> r1;
	vector<float> r2_x;
	vector<float> r2_y;
	vector<float> r2_z;
	vector<float> r3_x;
	vector<float> r3_y;
	vector<float> r3_z;
	vector<float> r4;
	vector<float> r4_r;
	vector<float> r5;
	vector<float> r5_r;
	Vector3f *r4_vec1 = new Vector3f();
	Vector3f *r4_vec2 = new Vector3f();
	vector<quat> q_r4;
	double threshold = 0.001;
	/*vector<Point3f> r3;
	vector<Point3f> r4;
	vector<Point3f> r5;
	vector<Point3f> r6;
	vector<Point3f> r7;
	vector<Point3f> r8;
	vector<Point3f> r9;
	vector<Point3f> r10;
	vector<Point3f> r11;
	vector<Point3f> r12;
	vector<Point3f> r13;
	vector<Point3f> r14;
	vector<Point3f> r15;
	vector<Point3f> r16;
	vector<Point3f> r17;
	vector<Point3f> r18;
	vector<Point3f> r19;
	vector<Point3f> r20;
	vector<Point3f> r21;
	vector<Point3f> r22;
	vector<int> r23;
	vector<float> r24;
	vector<Point3f> r25;
	vector<Point3f> r26;
	vector<Point3f> r27;
	vector<Point3f> r28;frame_after-frame_before*/
	r1.resize(frame_after - frame_before);
	r2_x.resize((frame_after - frame_before));
	r2_y.resize((frame_after - frame_before));
	r2_z.resize((frame_after - frame_before));
	r3_x.resize((frame_after - frame_before) - 1);
	r3_y.resize((frame_after - frame_before) - 1);
	r3_z.resize((frame_after - frame_before) - 1);
	r4.resize((frame_after - frame_before) - 1);
	r4_r.resize((frame_after - frame_before) - 1);
	q_r4.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before) - 1);
	r5_r.resize((frame_after - frame_before) - 1);
	/*r3.resize(joi_pos_array.size() / 21);
	r4.resize(joi_pos_array.size() / 21);
	r5.resize(joi_pos_array.size() / 20);
	r6.resize(joi_pos_array.size() / 20);
	r7.resize(joi_pos_array.size() / 20);
	r8.resize(joi_pos_array.size() / 20);
	r9.resize(joi_pos_array.size() / 20);
	r10.resize(joi_pos_array.size() / 20);
	r11.resize(joi_pos_array.size() / 20);
	r12.resize(joi_pos_array.size() / 20);
	r13.resize(joi_pos_array.size() / 20);
	r14.resize(joi_pos_array.size() / 20);
	r15.resize(joi_pos_array.size() / 20);
	r16.resize(joi_pos_array.size() / 20);
	r17.resize(joi_pos_array.size() / 20);
	r18.resize(joi_pos_array.size() / 20);
	r19.resize(joi_pos_array.size() / 20);
	r20.resize(joi_pos_array.size() / 20);
	r21.resize(joi_pos_array.size() / 20);
	r22.resize(joi_pos_array.size() / 20);
	r23.resize(joi_pos_array.size() / 20);
	r24.resize(joi_pos_array.size() / 20);
	r25.resize(joi_pos_array.size() / 20);
	r26.resize(joi_pos_array.size() / 20);
	r27.resize(joi_pos_array.size() / 20);
	r28.resize(joi_pos_array.size() / 20);*/

	for (int i = 0; i <(frame_after - frame_before) - 1; i++){
		r1[i] = joi_pos_array[(i + frame_before) * 20 + 7] - motion_positions_array[i];
		//r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i] - (joi_pos_array[15] - motion_positions_array[0]);

		//r1[i] = asin(seg_frame_array[(i + frame_before) * 21].m21);
		//r2[i] = (asin(seg_frame_array[(i + frame_before) * 21+21].m21) - asin(seg_frame_array[(i + frame_before) * 21].m21))/pow(interval,2);
		//r2_y.[i] = atan2(-seg_frame_array[(i + frame_before) * 21 + 21].m20, seg_frame_array[(i + frame_before) * 21 + 21].m22);
		if (abs(seg_frame_array[(i + frame_before) * 21 + 21].m21 - 1.0) < threshold){ // R(2,1) = sin(x) = 1の時
			r2_x[i] = M_PI / 2;
			r2_y[i] = 0;
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m10, seg_frame_array[(i + frame_before) * 21 + 21].m00);
		}
		else if (abs(seg_frame_array[(i + frame_before) * 21 + 21].m21 + 1.0) < threshold){ // R(2,1) = sin(x) = -1の時
			r2_x[i] = -M_PI / 2;
			r2_y[i] = 0;
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m10, seg_frame_array[(i + frame_before) * 21 + 21].m00);
		}
		else{
			r2_x[i] = asin(seg_frame_array[(i + frame_before) * 21 + 21].m21);
			r2_y[i] = atan2(-seg_frame_array[(i + frame_before) * 21 + 21].m20, seg_frame_array[(i + frame_before) * 21 + 21].m22);
			r2_z[i] = atan2(seg_frame_array[(i + frame_before) * 21 + 21].m01, seg_frame_array[(i + frame_before) * 21 + 21].m11);
		}

		if (abs(seg_frame_array[(i + frame_before) * 21].m21 - 1.0) < threshold){ // R(2,1) = sin(x) = 1の時
			r3_x[i] = M_PI / 2;
			r3_y[i] = 0;
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21].m10, seg_frame_array[(i + frame_before) * 21].m00);
		}
		else if (abs(seg_frame_array[(i + frame_before) * 21].m21 + 1.0) < threshold){ // R(2,1) = sin(x) = -1の時
			r3_x[i] = -M_PI / 2;
			r3_y[i] = 0;
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21].m10, seg_frame_array[(i + frame_before) * 21].m00);
		}
		else{
			r3_x[i] = asin(seg_frame_array[(i + frame_before) * 21].m21);
			r3_y[i] = atan2(-seg_frame_array[(i + frame_before) * 21].m20, seg_frame_array[(i + frame_before) * 21].m22);
			r3_z[i] = atan2(seg_frame_array[(i + frame_before) * 21].m01, seg_frame_array[(i + frame_before) * 21].m11);
		}
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 12].x - motion_positions_array[(i + frame_before)].x;
		r4_vec1->y = joi_pos_array[(i + frame_before) * 20 + 12].y - motion_positions_array[(i + frame_before)].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 12].z - motion_positions_array[(i + frame_before)].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 12].x - motion_positions_array[(i + frame_before + 1)].x;
		r4_vec2->y = joi_pos_array[(i + frame_before) * 20 + 20 + 12].y - motion_positions_array[(i + frame_before + 1)].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 12].z - motion_positions_array[(i + frame_before + 1)].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r4[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 16].x - motion_positions_array[(i + frame_before)].x;
		r4_vec1->y = joi_pos_array[(i + frame_before) * 20 + 16].y - motion_positions_array[(i + frame_before)].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 16].z - motion_positions_array[(i + frame_before)].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 16].x - motion_positions_array[(i + frame_before + 1)].x;
		r4_vec2->y = joi_pos_array[(i + frame_before) * 20 + 20 + 16].y - motion_positions_array[(i + frame_before + 1)].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 16].z - motion_positions_array[(i + frame_before + 1)].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r4_r[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 8].x - joi_pos_array[(i + frame_before) * 20 + 2].x;
		r4_vec1->y = joi_pos_array[(i + frame_before) * 20 + 8].y - joi_pos_array[(i + frame_before) * 20 + 2].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 8].z - joi_pos_array[(i + frame_before) * 20 + 2].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 8].x - joi_pos_array[(i + frame_before) * 20 + 20 + 2].x;
		r4_vec2->y = joi_pos_array[(i + frame_before) * 20 + 20 + 8].y - joi_pos_array[(i + frame_before) * 20 + 20 + 2].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 8].z - joi_pos_array[(i + frame_before) * 20 + 20 + 2].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r5[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + frame_before) * 20 + 4].x - joi_pos_array[(i + frame_before) * 20 + 2].x;
		r4_vec1->y = joi_pos_array[(i + frame_before) * 20 + 4].y - joi_pos_array[(i + frame_before) * 20 + 2].y;
		r4_vec1->z = joi_pos_array[(i + frame_before) * 20 + 4].z - joi_pos_array[(i + frame_before) * 20 + 2].z;
		r4_vec2->x = joi_pos_array[(i + frame_before) * 20 + 20 + 4].x - joi_pos_array[(i + frame_before) * 20 + 20 + 2].x;
		r4_vec2->y = joi_pos_array[(i + frame_before) * 20 + 20 + 4].y - joi_pos_array[(i + frame_before) * 20 + 20 + 2].y;
		r4_vec2->z = joi_pos_array[(i + frame_before) * 20 + 20 + 4].z - joi_pos_array[(i + frame_before) * 20 + 20 + 2].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r5_r[i] = r4_vec1->angle(*r4_vec2);
		simulate_trackball(&(q_r4[i]), r3_x[i], r3_y[i], r3_z[i], r2_x[i], r2_y[i], r2_z[i]);


		//r2[i] = joi_pos_array[i * 20 + 1] - motion_positions_array[i];
		/*r3[i] = joi_pos_array[i * 20 + 2] - motion_positions_array[i];
		r4[i] = joi_pos_array[i * 20 + 3] - motion_positions_array[i];
		r5[i] = joi_pos_array[i * 20 + 4] - motion_positions_array[i];
		r6[i] = joi_pos_array[i * 20 + 5] - motion_positions_array[i];
		r7[i] = joi_pos_array[i * 20 + 6] - motion_positions_array[i];
		r8[i] = joi_pos_array[i * 20 + 7] - motion_positions_array[i];
		r9[i] = joi_pos_array[i * 20 + 8] - motion_positions_array[i];
		r10[i] = joi_pos_array[i * 20 + 9] - motion_positions_array[i];
		r11[i] = joi_pos_array[i * 20 + 10] - motion_positions_array[i];
		r12[i] = joi_pos_array[i * 20 + 11] - motion_positions_array[i];
		r13[i] = joi_pos_array[i * 20 + 12] - motion_positions_array[i];
		r14[i] = joi_pos_array[i * 20 + 13] - motion_positions_array[i];
		r15[i] = joi_pos_array[i * 20 + 14] - motion_positions_array[i];
		r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i] - (joi_pos_array[15] - motion_positions_array[0]);
		r17[i] = joi_pos_array[i * 20 + 16] - motion_positions_array[i];
		r18[i] = joi_pos_array[i * 20 + 17] - motion_positions_array[i];
		r19[i] = joi_pos_array[i * 20 + 18] - motion_positions_array[i];
		r20[i] = joi_pos_array[i * 20 + 19] - motion_positions_array[i] - (joi_pos_array[19] - motion_positions_array[0]);
		r21[i] = r8[i] - r3[i] - (r8[0] - r3[0]);
		r22[i] = r12[i] - r3[i] - (r12[0] - r3[0]);*/

	}
	/*for (int i = 0; i < r21.size(); i++){
	r23[i] = i;
	}*/

	//calcLeastSquaresMethod( r21, r23,r24);

	ofstream ofs("joinejire.csv"); //ファイル出力ストリーム



	for (int j = 0; j < q_r4.size(); j++){
		ofs << q_r4[j].w;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].z;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_x.size(); j++){
		ofs << r2_x[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_y.size(); j++){
		ofs << r2_y[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r2_z.size(); j++){
		ofs << r2_z[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r4.size(); j++){
		ofs << r4[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r4_r.size(); j++){
		ofs << r4_r[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5_r.size(); j++){
		ofs << r5_r[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	return;
}

void out_of_filejoi2acceleration(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm){
	vector<Point3f> r1;//左手の加速度
	vector<Point3f> r2;//右手の加速度
	vector<Point3f> r3;//左足の加速度
	vector<Point3f> r4;//右足の加速度
	vector<float> r5;
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();
	float interval = 0.0;
	interval = intervalm;
	r1.resize((frame_after - frame_before) - 1);
	r2.resize((frame_after - frame_before) - 1);
	r3.resize((frame_after - frame_before) - 1);
	r4.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before));
	for (int i = 0; i < r1.size(); i++){
		/*r1[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 7] - motion_positions_array[(i + frame_before) + 1] - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 7] - motion_positions_array[(i + frame_before)] - joi_pos_array[(i + frame_before) * 20 + 3]);
		r2[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 11] - motion_positions_array[(i + frame_before) + 1] - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 11] - motion_positions_array[(i + frame_before)] - joi_pos_array[(i + frame_before) * 20 + 3]);
		r3[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 15] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 15] - motion_positions_array[(i + frame_before)]);
		r4[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 19] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 19] - motion_positions_array[(i + frame_before)]);
*/
		
		r1[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 7]  - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 7] - joi_pos_array[(i + frame_before) * 20 + 3]);
		r2[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 11]  - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 11]  - joi_pos_array[(i + frame_before) * 20 + 3]);
		r3[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 15] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 15] - motion_positions_array[(i + frame_before)]);
		r4[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 19] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 19] - motion_positions_array[(i + frame_before)]);
		
	}
	for (int i = 0; i < r5.size(); i++){
		//背筋の変化
		r5_vec1->x = joi_pos_array[(i + frame_before) * 20 + 2].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec1->y = joi_pos_array[(i + frame_before) * 20 + 2].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec1->z = joi_pos_array[(i + frame_before) * 20 + 2].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5_vec2->x = joi_pos_array[(i + frame_before) * 20 + 0].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec2->y = joi_pos_array[(i + frame_before) * 20 + 0].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec2->z = joi_pos_array[(i + frame_before) * 20 + 0].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5[i] = r5_vec1->angle(*r5_vec2);
	}
	ofstream ofs("out_of_filejoi2acceleration.csv"); //ファイル出力ストリーム
	

	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_filejoi2acceleration2(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm){
	vector<float> r1;//左手の加速度
	vector<float> r2;//右手の加速度
	vector<float> r3;//左足の加速度
	vector<float> r4;//右足の加速度
	vector<float> r5;
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();
	float interval = 0.0;
	interval = intervalm;
	r1.resize((frame_after - frame_before) - 1);
	r2.resize((frame_after - frame_before) - 1);
	r3.resize((frame_after - frame_before) - 1);
	r4.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before));
	for (int i = 0; i < r1.size(); i++){
		/*r1[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 7] - motion_positions_array[(i + frame_before) + 1] - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 7] - motion_positions_array[(i + frame_before)] - joi_pos_array[(i + frame_before) * 20 + 3]);
		r2[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 11] - motion_positions_array[(i + frame_before) + 1] - joi_pos_array[(i + frame_before) * 20 + 20 + 3]) - (joi_pos_array[(i + frame_before) * 20 + 11] - motion_positions_array[(i + frame_before)] - joi_pos_array[(i + frame_before) * 20 + 3]);
		r3[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 15] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 15] - motion_positions_array[(i + frame_before)]);
		r4[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 19] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 19] - motion_positions_array[(i + frame_before)]);
		*/

		r1[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 7], joi_pos_array[(i + frame_before) * 20 + 7]);
		r2[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 11], joi_pos_array[(i + frame_before) * 20 + 11]);
		r3[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 15], joi_pos_array[(i + frame_before) * 20 + 15]);
		r4[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 19], joi_pos_array[(i + frame_before) * 20 + 19]);

	}
	for (int i = 0; i < r5.size(); i++){
		//背筋の変化
		r5_vec1->x = joi_pos_array[(i + frame_before) * 20 + 2].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec1->y = joi_pos_array[(i + frame_before) * 20 + 2].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec1->z = joi_pos_array[(i + frame_before) * 20 + 2].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5_vec2->x = joi_pos_array[(i + frame_before) * 20 + 0].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec2->y = joi_pos_array[(i + frame_before) * 20 + 0].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec2->z = joi_pos_array[(i + frame_before) * 20 + 0].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5[i] = r5_vec1->angle(*r5_vec2);
	}
	ofstream ofs("out_of_filejoi2acceleration2.csv"); //ファイル出力ストリーム


	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void out_of_filejoi2accelerationmaster(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm){
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	vector<float> r5;
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();
	float interval = 0.0;
	interval = intervalm;
	r1.resize((frame_after - frame_before) - 1);
	r2.resize((frame_after - frame_before) - 1);
	r3.resize((frame_after - frame_before) - 1);
	r4.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before));
	for (int i = 0; i < r1.size(); i++){
		r1[i] = joi_pos_array[(i + frame_before) * 20 + 20 + 7]   - joi_pos_array[(i + frame_before) * 20 + 7];
		r2[i] = joi_pos_array[(i + frame_before) * 20 + 20 + 11] - joi_pos_array[(i + frame_before) * 20 + 11];
		/*r3[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 15] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 15] - motion_positions_array[(i + frame_before)]);
		r4[i] = (joi_pos_array[(i + frame_before) * 20 + 20 + 19] - motion_positions_array[(i + frame_before) + 1]) - (joi_pos_array[(i + frame_before) * 20 + 19] - motion_positions_array[(i + frame_before)]);*/
		r3[i] = joi_pos_array[(i + frame_before) * 20 + 20 + 15] - joi_pos_array[(i + frame_before) * 20 + 15];
		r4[i] = joi_pos_array[(i + frame_before) * 20 + 20 + 19] - joi_pos_array[(i + frame_before) * 20 + 19];

		}
	for (int i = 0; i < r5.size(); i++){
		r5_vec1->x = joi_pos_array[(i + frame_before) * 20 + 2].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec1->y = joi_pos_array[(i + frame_before) * 20 + 2].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec1->z = joi_pos_array[(i + frame_before) * 20 + 2].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5_vec2->x = joi_pos_array[(i + frame_before) * 20 + 0].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec2->y = joi_pos_array[(i + frame_before) * 20 + 0].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec2->z = joi_pos_array[(i + frame_before) * 20 + 0].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5[i] = r5_vec1->angle(*r5_vec2);
	}
	ofstream ofs("out_of_filejoi2acceleration_master.csv"); //ファイル出力ストリーム
	/*for (int j = 0; j < r1.size(); j++){
	ofs << (r1[j] + r2[j] + r3[j] + r4[j]) / 4.0;
	ofs << ",";
	}
	ofs << endl;*/

	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}

void out_of_filejoi2accelerationmaster2(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int frame_before, int frame_after, float intervalm){
	vector<float> r1;
	vector<float> r2;
	vector<float> r3;
	vector<float> r4;
	vector<float> r5;
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();
	float interval = 0.0;
	interval = intervalm;
	r1.resize((frame_after - frame_before) - 1);
	r2.resize((frame_after - frame_before) - 1);
	r3.resize((frame_after - frame_before) - 1);
	r4.resize((frame_after - frame_before) - 1);
	r5.resize((frame_after - frame_before));
	for (int i = 0; i < r1.size(); i++){
		r2[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 11], joi_pos_array[(i + frame_before) * 20 + 11]);
		r3[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 15], joi_pos_array[(i + frame_before) * 20 + 15]);
		r4[i] = ED(joi_pos_array[(i + frame_before) * 20 + 20 + 19], joi_pos_array[(i + frame_before) * 20 + 19]);

	}
	for (int i = 0; i < r5.size(); i++){
		r5_vec1->x = joi_pos_array[(i + frame_before) * 20 + 2].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec1->y = joi_pos_array[(i + frame_before) * 20 + 2].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec1->z = joi_pos_array[(i + frame_before) * 20 + 2].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5_vec2->x = joi_pos_array[(i + frame_before) * 20 + 0].x - joi_pos_array[(i + frame_before) * 20 + 1].x;
		r5_vec2->y = joi_pos_array[(i + frame_before) * 20 + 0].y - joi_pos_array[(i + frame_before) * 20 + 1].y;
		r5_vec2->z = joi_pos_array[(i + frame_before) * 20 + 0].z - joi_pos_array[(i + frame_before) * 20 + 1].z;
		r5[i] = r5_vec1->angle(*r5_vec2);
	}
	ofstream ofs("out_of_filejoi2acceleration_master2.csv"); //ファイル出力ストリーム
	/*for (int j = 0; j < r1.size(); j++){
	ofs << (r1[j] + r2[j] + r3[j] + r4[j]) / 4.0;
	ofs << ",";
	}
	ofs << endl;*/

	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j] / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	
	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void section_jointcreate(vector< Point3f> & joi_pos_array, vector<Point3f>& r1, vector<Point3f>& r2, vector<Point3f>& r3, vector<Point3f>& r4, int start_frame, int end_frame, float interval){
	
	for (int i = 0; i < r1.size(); i++){
		r1[i] = joi_pos_array[(i + start_frame) * 20 + 20 + 7] - joi_pos_array[(i + start_frame) * 20 + 7];
		r2[i] = joi_pos_array[(i + start_frame) * 20 + 20 + 11] - joi_pos_array[(i + start_frame) * 20 + 11];
		r3[i] = joi_pos_array[(i + start_frame) * 20 + 20 + 15] - joi_pos_array[(i + start_frame) * 20 + 15];
		r4[i] = joi_pos_array[(i + start_frame) * 20 + 20 + 19] - joi_pos_array[(i + start_frame) * 20 + 19];
		r1[i].x = r1[i].x / pow(interval, 2);
		r1[i].y = r1[i].y / pow(interval, 2);
		r1[i].z = r1[i].z / pow(interval, 2);
		r2[i].x = r2[i].x / pow(interval, 2);
		r2[i].y = r2[i].y / pow(interval, 2);
		r2[i].z = r2[i].z / pow(interval, 2);
		r3[i].x = r3[i].x / pow(interval, 2);
		r3[i].y = r3[i].y / pow(interval, 2);
		r3[i].z = r3[i].z / pow(interval, 2);
		r4[i].x = r4[i].x / pow(interval, 2);
		r4[i].y = r4[i].y / pow(interval, 2);
		r4[i].z = r4[i].z / pow(interval, 2);
	}
}
void section_nejire_jointcreate(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<float>& r5, vector<float>& r6, int start_frame, int end_frame, float interval){
	Vector3f *r4_vec1 = new Vector3f();
	Vector3f *r4_vec2 = new Vector3f();

	for (int i = 0; i < r5.size(); i++){
		r4_vec1->x = joi_pos_array[(i + start_frame) * 20 + 12].x - motion_positions_array[(i + start_frame)].x;
		r4_vec1->y = 0;// joi_pos_array[(i + start_frame) * 20 + 12].y - motion_positions_array[(i + start_frame)].y;
		r4_vec1->z = joi_pos_array[(i + start_frame) * 20 + 12].z - motion_positions_array[(i + start_frame)].z;
		r4_vec2->x = joi_pos_array[(i + start_frame) * 20 + 20 + 12].x - motion_positions_array[(i + start_frame + 1)].x;
		r4_vec2->y = 0;// joi_pos_array[(i + start_frame) * 20 + 20 + 12].y - motion_positions_array[(i + start_frame + 1)].y;
		r4_vec2->z = joi_pos_array[(i + start_frame) * 20 + 20 + 12].z - motion_positions_array[(i + start_frame + 1)].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r5[i] = r4_vec1->angle(*r4_vec2);
		r4_vec1->x = joi_pos_array[(i + start_frame) * 20 + 8].x - joi_pos_array[(i + start_frame) * 20 + 2].x;
		r4_vec1->y = 0;// joi_pos_array[(i + start_frame) * 20 + 8].y - joi_pos_array[(i + start_frame) * 20 + 2].y;
		r4_vec1->z = joi_pos_array[(i + start_frame) * 20 + 8].z - joi_pos_array[(i + start_frame) * 20 + 2].z;
		r4_vec2->x = joi_pos_array[(i + start_frame) * 20 + 20 + 8].x - joi_pos_array[(i + start_frame) * 20 + 20 + 2].x;
		r4_vec2->y = 0;// joi_pos_array[(i + start_frame) * 20 + 20 + 8].y - joi_pos_array[(i + start_frame) * 20 + 20 + 2].y;
		r4_vec2->z = joi_pos_array[(i + start_frame) * 20 + 20 + 8].z - joi_pos_array[(i + start_frame) * 20 + 20 + 2].z;
		r4_vec1->normalize();
		r4_vec2->normalize();
		r6[i] = r4_vec1->angle(*r4_vec2);

		r5[i] = r5[i] / pow(interval, 2);
		r6[i] = r6[i] / pow(interval, 2);
	
	}
}
void Evaluation_acceleration(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<int> frame_section, vector<float>& acc, vector<float>& Angular_acc, vector<int>& acc_frame, vector<int>& Angular_acc_frame, float intervalm){
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	vector<float> r5;
	vector<float> r6;
	float last_acc = 0.0;
	int last_frame = 0;
	float first_acc = 0.0;
	int first_frame = 0;
	float mid1_acc = 0.0;
	int mid1_frame = 0;
	float mid2_acc = 0.0;
	int mid2_frame = 0;
	float hip_acc = 0.0;
	int hip_frame = 0;
	float shoulder_acc = 0.0;
	int shoulder_frame = 0;
	//順突き動作
	r1.resize((frame_section[2] - frame_section[1]) - 1);
	r2.resize((frame_section[2] - frame_section[1]) - 1);
	r3.resize((frame_section[2] - frame_section[1]) - 1);
	r4.resize((frame_section[2] - frame_section[1]) - 1);
	r5.resize((frame_section[2] - frame_section[1]) - 1);
	r6.resize((frame_section[2] - frame_section[1]) - 1);
	section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[1], frame_section[2], intervalm);
	section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[1], frame_section[2], intervalm);		
		for (int i = 0; i < r1.size(); i++){
			if (last_acc < r1[i].z){
				last_acc = r1[i].z;
				last_frame = i;
			}
		}		
		for (int i = 0; i < last_frame; i++){
			if (first_acc < r3[i].z){
				first_acc = r3[i].z;
				first_frame = i;
			}
		}

		for (int i = 0; i < last_frame; i++){
			if (hip_acc < r5[i]){
				hip_acc = r5[i];
				hip_frame = i;
			}
		}
		for (int i = hip_frame; i < last_frame; i++){
			if (shoulder_acc < r6[i]){
				shoulder_acc = r6[i];
				shoulder_frame = i;
			}
		}

		acc[0] = r3[first_frame].z;//左足
		acc[1] = r1[last_frame].z;//左手
		acc[2] = r2[last_frame].z;//右手
		acc[3] = r4[last_frame].z;//右足
		acc_frame[0] = first_frame + frame_section[1];
		acc_frame[1] = last_frame + frame_section[1];
		acc_frame[2] = last_frame + frame_section[1];
		acc_frame[3] = last_frame + frame_section[1];
		Angular_acc[0] = hip_acc;
		Angular_acc[1] = shoulder_acc;
		Angular_acc_frame[0] = hip_frame + frame_section[1];
		Angular_acc_frame[1] = shoulder_frame + frame_section[1];
//逆突き動作
		 last_acc = 0.0;
		 last_frame = 0;
		 first_acc = 0.0;
		 first_frame = 0;
		 hip_acc = 0.0;
		 hip_frame = 0;
		 shoulder_acc = 0.0;
		 shoulder_frame = 0;
		r1.resize((frame_section[3] - frame_section[2]) - 1);
		r2.resize((frame_section[3] - frame_section[2]) - 1);
		r3.resize((frame_section[3] - frame_section[2]) - 1);
		r4.resize((frame_section[3] - frame_section[2]) - 1);
		r5.resize((frame_section[3] - frame_section[2]) - 1);
		r6.resize((frame_section[3] - frame_section[2]) - 1);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[2], frame_section[3], intervalm);
		section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[2], frame_section[3], intervalm);
		for (int i = 0; i < r2.size(); i++){
			if (first_acc < r2[i].z){
				first_acc = r2[i].z;
				first_frame = i;
			}
		}
		for (int i = 0; i < first_frame; i++){
			if (hip_acc < r5[i]){
				hip_acc = r5[i];
				hip_frame = i;
			}
		}
		for (int i = hip_frame; i < first_frame; i++){
			if (shoulder_acc < r6[i]){
				shoulder_acc = r6[i];
				shoulder_frame = i;
			}
		}
		acc[4] = r2[first_frame].z;//右手
		acc[5] = r1[first_frame].z;//左手
		acc[6] = r4[first_frame].z;//右足
		acc_frame[4] = first_frame + frame_section[2];
		acc_frame[5] = first_frame + frame_section[2];
		acc_frame[6] = first_frame + frame_section[2];
		Angular_acc[2] = hip_acc;
		Angular_acc[3] = shoulder_acc;
		Angular_acc_frame[2] = hip_frame + frame_section[2];
		Angular_acc_frame[3] = shoulder_frame + frame_section[2];
//鈎突き動作
		last_acc = 0.0;
		last_frame = 0;
		first_acc = 0.0;
		first_frame = 0;
		hip_acc = 0.0;
		hip_frame = 0;
		shoulder_acc = 0.0;
		shoulder_frame = 0;
		r1.resize((frame_section[4] - frame_section[3]) - 1);
		r2.resize((frame_section[4] - frame_section[3]) - 1);
		r3.resize((frame_section[4] - frame_section[3]) - 1);
		r4.resize((frame_section[4] - frame_section[3]) - 1);
		r5.resize((frame_section[4] - frame_section[3]) - 1);
		r6.resize((frame_section[4] - frame_section[3]) - 1);
		section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[3], frame_section[4], intervalm);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[3], frame_section[4], intervalm);
		for (int i = 0; i < r1.size(); i++){
			if (last_acc > r1[i].x){
				last_acc = r1[i].x;
				last_frame = i;
			}
		}
		for (int i = 0; i < last_frame; i++){
			if (mid1_acc > r2[i].x){
				mid1_acc = r2[i].x;
				mid1_frame = i;
			}
		}
		for (int i = 0; i < mid1_frame; i++){
			if (first_acc < r3[i].z){
				first_acc = r3[i].z;
				first_frame = i;
			}
		}

		for (int i = 0; i < last_frame; i++){
			if (hip_acc < r5[i]){
				hip_acc = r5[i];
				hip_frame = i;
			}
		}
		for (int i = hip_frame; i < last_frame; i++){
			if (shoulder_acc < r6[i]){
				shoulder_acc = r6[i];
				shoulder_frame = i;
			}
		}
		acc[7] = r3[first_frame].z;//左足
		acc[8] = r2[mid1_frame].z;//右手
		acc[9] = r1[last_frame].z;//左手
		acc_frame[7] = first_frame + frame_section[3];
		acc_frame[8] = mid1_frame + frame_section[3];
		acc_frame[9] = last_frame + frame_section[3];
		Angular_acc[4] = hip_acc;
		Angular_acc[5] = shoulder_acc;
		Angular_acc_frame[4] = hip_frame + frame_section[3];
		Angular_acc_frame[5] = shoulder_frame + frame_section[3];
//上受け動作
		last_acc = 0.0;
		last_frame = 0;
		first_acc = 0.0;
		first_frame = 0;
		mid1_acc = 0.0;
		mid1_frame = 0;
		hip_acc = 0.0;
		hip_frame = 0;
		shoulder_acc = 0.0;
		shoulder_frame = 0;
		r1.resize((frame_section[5] - frame_section[4]) - 1);
		r2.resize((frame_section[5] - frame_section[4]) - 1);
		r3.resize((frame_section[5] - frame_section[4]) - 1);
		r4.resize((frame_section[5] - frame_section[4]) - 1);
		r5.resize((frame_section[5] - frame_section[4]) - 1);
		r6.resize((frame_section[5] - frame_section[4]) - 1);
		section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[4], frame_section[5], intervalm);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[4], frame_section[5], intervalm);
		for (int i = 0; i < r3.size(); i++){
			
			if (first_acc > r3[i].z){
				first_acc = r3[i].z;
				first_frame = i;
			}
		}

		for (int i = first_frame; i < r2.size(); i++){
			if (mid1_acc < r2[i].y){
				mid1_acc = r2[i].y;
				mid1_frame = i;
			}
		}
		for (int i = mid1_frame; i < r2.size(); i++){
			if (last_acc > r4[i].z){
				last_acc = r4[i].z;
				last_frame = i;
			}
		}

		for (int i = 0; i < mid1_frame; i++){
			if (hip_acc < r5[i]){
				hip_acc = r5[i];
				hip_frame = i;
			}
		}
		for (int i = hip_frame; i < mid1_frame; i++){
			if (shoulder_acc < r6[i]){
				shoulder_acc = r6[i];
				shoulder_frame = i;
			}
		}
		acc[10] = first_acc;//左足
		acc[11] = mid1_acc;//右手
		acc[12] = last_acc;//右足
		acc_frame[10] = first_frame + frame_section[4];
		acc_frame[11] = mid1_frame + frame_section[4];
		acc_frame[12] = last_frame + frame_section[4];
		float a = r4[mid1_frame].z;
		Angular_acc[6] = hip_acc;
		Angular_acc[7] = shoulder_acc;
		Angular_acc_frame[6] = hip_frame + frame_section[4];
		Angular_acc_frame[7] = shoulder_frame + frame_section[4];
//同時受け動作
		last_acc = 0.0;
		last_frame = 0;
		first_acc = 0.0;
		first_frame = 0;
		mid1_acc = 0.0;
		mid1_frame = 0;
		hip_acc = 0.0;
		hip_frame = 0;
		shoulder_acc = 0.0;
		shoulder_frame = 0;
		r1.resize((frame_section[6] - frame_section[5]) - 1);
		r2.resize((frame_section[6] - frame_section[5]) - 1);
		r3.resize((frame_section[6] - frame_section[5]) - 1);
		r4.resize((frame_section[6] - frame_section[5]) - 1);
		r5.resize((frame_section[6] - frame_section[5]) - 1);
		r6.resize((frame_section[6] - frame_section[5]) - 1);
		section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[5], frame_section[6], intervalm);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[5], frame_section[6], intervalm);
		for (int i = 0; i < r1.size(); i++){
			if (mid1_acc > r1[i].x){
				mid1_acc = r1[i].x;
				mid1_frame = i;
			}
		}
		for (int i = 0; i < r1.size(); i++){
			if (mid2_acc < r2[i].x){
				mid2_acc = r2[i].x;
				mid2_frame = i;
			}
		}
		if (mid1_frame < mid2_frame){
			for (int i = 0; i < mid1_frame; i++){

				if (first_acc < r3[i].x){
					first_acc = r3[i].x;
					first_frame = i;
				}
			}

			for (int i = mid1_frame; i < r2.size(); i++){
				if (last_acc > r4[i].z){
					last_acc = r4[i].z;
					last_frame = i;
				}
			}

			for (int i = 0; i < mid1_frame; i++){
				if (hip_acc < r5[i]){
					hip_acc = r5[i];
					hip_frame = i;
				}
			}
			for (int i = hip_frame; i < mid1_frame; i++){
				if (shoulder_acc < r6[i]){
					shoulder_acc = r6[i];
					shoulder_frame = i;
				}
			}
		}
		else{
			for (int i = 0; i < mid2_frame; i++){

				if (first_acc < r3[i].x){
					first_acc = r3[i].x;
					first_frame = i;
				}
			}

			for (int i = mid2_frame; i < r2.size(); i++){
				if (last_acc > r4[i].z){
					last_acc = r4[i].z;
					last_frame = i;
				}
			}

			for (int i = 0; i < mid2_frame; i++){
				if (hip_acc < r5[i]){
					hip_acc = r5[i];
					hip_frame = i;
				}
			}
			for (int i = hip_frame; i < mid2_frame; i++){
				if (shoulder_acc < r6[i]){
					shoulder_acc = r6[i];
					shoulder_frame = i;
				}
			}
		}
		acc[13] = first_acc;//左足
		acc[14] = mid1_acc;//左手
		acc[15] = mid2_acc;//右手
		acc[16] = last_acc;//右足
		acc_frame[13] = first_frame + frame_section[5];
		acc_frame[14] = mid1_frame + frame_section[5];
		acc_frame[15] = mid2_frame + frame_section[5];
		acc_frame[16] = last_frame + frame_section[5];
		float b = r4[mid2_frame].z;
		Angular_acc[8] = hip_acc;
		Angular_acc[9] = shoulder_acc;
		Angular_acc_frame[8] = hip_frame + frame_section[5];
		Angular_acc_frame[9] = shoulder_frame + frame_section[5];
//払い受け動作
		last_acc = 0.0;
		last_frame = 0;
		first_acc = 0.0;
		first_frame = 0;
		mid1_acc = 0.0;
		mid1_frame = 0;
		mid2_acc = 0.0;
		mid2_frame = 0;
		hip_acc = 0.0;
		hip_frame = 0;
		shoulder_acc = 0.0;
		shoulder_frame = 0;
		r1.resize((frame_section[7] - frame_section[6]) - 1);
		r2.resize((frame_section[7] - frame_section[6]) - 1);
		r3.resize((frame_section[7] - frame_section[6]) - 1);
		r4.resize((frame_section[7] - frame_section[6]) - 1);
		r5.resize((frame_section[7] - frame_section[6]) - 1);
		r6.resize((frame_section[7] - frame_section[6]) - 1);
		section_nejire_jointcreate(joi_pos_array, motion_positions_array, r5, r6, frame_section[6], frame_section[7], intervalm);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[6], frame_section[7], intervalm);
		for (int i = 0; i < r1.size(); i++){
			if (mid1_acc < r1[i].x){
				mid1_acc = r1[i].x;
				mid1_frame = i;
			}
		}
		for (int i = 0; i < r1.size(); i++){
			if (mid2_acc > r2[i].x){
				mid2_acc = r2[i].x;
				mid2_frame = i;
			}
		}
		for (int i = 0; i < mid1_frame; i++){

			if (first_acc < r3[i].x){
				first_acc = r3[i].x;
				first_frame = i;
			}
		}

		for (int i = mid1_frame; i < r2.size(); i++){
			if (last_acc > r4[i].z){
				last_acc = r4[i].z;
				last_frame = i;
			}
		}
		for (int i = 0; i < mid1_frame; i++){
			if (hip_acc < r5[i]){
				hip_acc = r5[i];
				hip_frame = i;
			}
		}
		for (int i = hip_frame; i < mid1_frame; i++){
			if (shoulder_acc < r6[i]){
				shoulder_acc = r6[i];
				shoulder_frame = i;
			}
		}
		acc[17] = first_acc;//左足
		acc[18] = mid1_acc;//左手
		acc[19] = mid2_acc;//右手
		acc[20] = last_acc;//右足
		acc_frame[17] = first_frame + frame_section[6];
		acc_frame[18] = mid1_frame + frame_section[6];
		acc_frame[19] = mid2_frame + frame_section[6];
		acc_frame[20] = last_frame + frame_section[6];
		float c = r4[mid2_frame].z;
		Angular_acc[10] = hip_acc;
		Angular_acc[11] = shoulder_acc;
		Angular_acc_frame[10] = hip_frame + frame_section[6];
		Angular_acc_frame[11] = shoulder_frame + frame_section[6];
//蹴り上げ動作
		last_acc = 0.0;
		last_frame = 0;
		first_acc = 0.0;
		first_frame = 0;
		mid1_acc = 0.0;
		mid1_frame = 0;
		mid2_acc = 0.0;
		mid2_frame = 0;
		r1.resize((frame_section[8] - frame_section[7]) - 1);
		r2.resize((frame_section[8] - frame_section[7]) - 1);
		r3.resize((frame_section[8] - frame_section[7]) - 1);
		r4.resize((frame_section[8] - frame_section[7]) - 1);
		section_jointcreate(joi_pos_array, r1, r2, r3, r4, frame_section[7], frame_section[8], intervalm);

		for (int i = 0; i < r4.size(); i++){

			if (first_acc < r4[i].y){
				first_acc = r4[i].y;
				first_frame = i;
			}
		}

		acc[21] = first_acc;//右足
		acc_frame[21] = first_frame + frame_section[7];
}
void section_spine_create(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<float>& r1, int start_frame, int end_frame){
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();

	for (int i = 0; i < r1.size(); i++){
		r5_vec1->x = 0;
		r5_vec1->y = 1;
		r5_vec1->z = 0;
		r5_vec2->x = joi_pos_array[(i + start_frame) * 20 + 3].x - motion_positions_array[(i + start_frame)].x;
		r5_vec2->y = joi_pos_array[(i + start_frame) * 20 + 3].y - motion_positions_array[(i + start_frame)].y;
		r5_vec2->z = joi_pos_array[(i + start_frame) * 20 + 3].z - motion_positions_array[(i + start_frame)].z;
		r5_vec1->normalize();
		r5_vec2->normalize();
		r1[i] = r5_vec1->angle(*r5_vec2);
	}
}
void section_spine_create2(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<float>& r1, int start_frame, int end_frame){
	Vector3f *r5_vec1 = new Vector3f();
	Vector3f *r5_vec2 = new Vector3f();

	for (int i = 0; i < r1.size(); i++){
		r5_vec1->x = joi_pos_array[(i + start_frame) * 20 + 1].x - joi_pos_array[(i + start_frame) * 20 + 0].x;
		r5_vec1->y = joi_pos_array[(i + start_frame) * 20 + 1].y - joi_pos_array[(i + start_frame) * 20 + 0].y;
		r5_vec1->z = joi_pos_array[(i + start_frame) * 20 + 1].z - joi_pos_array[(i + start_frame) * 20 + 0].z;
		r5_vec2->x = motion_positions_array[(i + start_frame) ].x - joi_pos_array[(i + start_frame) * 20 + 0].x;
		r5_vec2->y = motion_positions_array[(i + start_frame) ].y - joi_pos_array[(i + start_frame) * 20 + 0].y;
		r5_vec2->z = motion_positions_array[(i + start_frame) ].z - joi_pos_array[(i + start_frame) * 20 + 0].z;
		r5_vec1->normalize();
		r5_vec2->normalize();
		r1[i] = r5_vec1->angle(*r5_vec2);
	}
}
void calcSD_point(vector<float> r1, float& SD_point){
	float ave = 0.0;
	float SD = 0.0;
	for (int i = 0; i < r1.size(); i++){
		ave = ave + r1[i];
	}
	ave = ave / r1.size();

	for (int i = 0; i < r1.size(); i++){
		SD = SD + pow(r1[i]-ave,2);
	}
	SD = SD / r1.size();
	SD_point = sqrt(SD);

}
void Evaluation_spineVariation(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, vector<int> frame_section, vector<float>& SD_point){
	vector<float> r1;
	float ave = 0.0;
	for (int i = 0; i < 7; i++){
		ave = 0.0;
		r1.resize((frame_section[i+2] - frame_section[i+1]) - 1);
		section_spine_create(joi_pos_array, motion_positions_array, r1, frame_section[i + 1], frame_section[i + 2]);
		//section_spine_create2(joi_pos_array, motion_positions_array, r1, frame_section[i + 1], frame_section[i + 2]);
		calcSD_point( r1, SD_point[i]);
		/*for (int j = 0; j < r1.size(); j++){
			ave = ave + r1[j];
		}
		ave = ave / r1.size();
		SD_point[i] = ave;*/
	}
}

void Evaluation_DTW(vector< Point3f> & joi_pos_array_m, vector< Point3f> & joi_pos_array, vector<int> frame_section_m, vector<int> frame_section, vector<float>& DTW_point){
	int a[11] = {7,11,7,11,15,11,7,11,7,11,19};
	int cnt = 0;
	vector<Point3f>mas;
	vector<Point3f>sub;
	for (int i = 0; i < 7; i++){
	
			mas.resize(frame_section_m[i + 2] - frame_section_m[i + 1]);
			sub.resize(frame_section[i + 2] - frame_section[i + 1]);
			orbit(joi_pos_array_m, mas, frame_section_m[i + 1], frame_section_m[i + 2], a[cnt]);
			orbit(joi_pos_array, sub, frame_section[i + 1], frame_section[i + 2], a[cnt]);
			DTW_point[cnt] = DTW(mas, sub, 0);
			cnt++;
			if (i == 2 || i == 3 || i == 4 || i == 5){
			mas.resize(frame_section_m[i + 2] - frame_section_m[i + 1]);
			sub.resize(frame_section[i + 2] - frame_section[i + 1]);
			orbit(joi_pos_array_m, mas, frame_section_m[i + 1], frame_section_m[i + 2], a[cnt]);
			orbit(joi_pos_array, sub, frame_section[i + 1], frame_section[i + 2], a[cnt]);
			DTW_point[cnt] = DTW(mas, sub, 0);
			cnt++;
		}
		
		
	}
}

void out_of_filejoi3Dmoveclearkubite(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array){
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	vector<Point3f> r5;
	vector<Point3f> r6;
	vector<Point3f> r7;
	vector<Point3f> r8;
	vector<Point3f> r9;
	vector<Point3f> r10;
	vector<Point3f> r11;
	vector<Point3f> r12;
	vector<Point3f> r13;
	vector<Point3f> r14;
	vector<Point3f> r15;
	vector<Point3f> r16;
	vector<Point3f> r17;
	vector<Point3f> r18;
	vector<Point3f> r19;
	vector<Point3f> r20;
	vector<Point3f> r21;
	vector<Point3f> r22;
	vector<int> r23;
	vector<float> r24;
	vector<Point3f> r25;
	vector<Point3f> r26;
	vector<Point3f> r27;
	vector<Point3f> r28;
	r1.resize(joi_pos_array.size() / 20);
	r2.resize(joi_pos_array.size() / 20);
	r3.resize(joi_pos_array.size() / 20);
	r4.resize(joi_pos_array.size() / 20);
	r5.resize(joi_pos_array.size() / 20);
	r6.resize(joi_pos_array.size() / 20);
	r7.resize(joi_pos_array.size() / 20);
	r8.resize(joi_pos_array.size() / 20);
	r9.resize(joi_pos_array.size() / 20);
	r10.resize(joi_pos_array.size() / 20);
	r11.resize(joi_pos_array.size() / 20);
	r12.resize(joi_pos_array.size() / 20);
	r13.resize(joi_pos_array.size() / 20);
	r14.resize(joi_pos_array.size() / 20);
	r15.resize(joi_pos_array.size() / 20);
	r16.resize(joi_pos_array.size() / 20);
	r17.resize(joi_pos_array.size() / 20);
	r18.resize(joi_pos_array.size() / 20);
	r19.resize(joi_pos_array.size() / 20);
	r20.resize(joi_pos_array.size() / 20);
	r21.resize(joi_pos_array.size() / 20);
	r22.resize(joi_pos_array.size() / 20);
	r23.resize(joi_pos_array.size() / 20);
	r24.resize(joi_pos_array.size() / 20);
	r25.resize(joi_pos_array.size() / 20);
	r26.resize(joi_pos_array.size() / 20);
	r27.resize(joi_pos_array.size() / 20);
	r28.resize(joi_pos_array.size() / 20);

	for (int i = 0; i < joi_pos_array.size() / 20; i++){
		r1[i] = joi_pos_array[i * 20] - motion_positions_array[i] ;
		r2[i] = joi_pos_array[i * 20 + 1] - motion_positions_array[i] ;
		r3[i] = joi_pos_array[i * 20 + 2] - motion_positions_array[i] ;
		r4[i] = joi_pos_array[i * 20 + 3] - motion_positions_array[i] ;
		r5[i] = joi_pos_array[i * 20 + 4] - motion_positions_array[i];
		r6[i] = joi_pos_array[i * 20 + 5] - motion_positions_array[i] ;
		r7[i] = joi_pos_array[i * 20 + 6] - motion_positions_array[i] ;
		r8[i] = joi_pos_array[i * 20 + 7] - motion_positions_array[i] ;
		r9[i] = joi_pos_array[i * 20 + 8] - motion_positions_array[i];
		r10[i] = joi_pos_array[i * 20 + 9] - motion_positions_array[i] ;
		r11[i] = joi_pos_array[i * 20 + 10] - motion_positions_array[i];
		r12[i] = joi_pos_array[i * 20 + 11] - motion_positions_array[i] ;
		r13[i] = joi_pos_array[i * 20 + 12] - motion_positions_array[i] ;
		r14[i] = joi_pos_array[i * 20 + 13] - motion_positions_array[i] ;
		r15[i] = joi_pos_array[i * 20 + 14] - motion_positions_array[i] ;
		r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i] - (joi_pos_array[15] - motion_positions_array[0]);
		r17[i] = joi_pos_array[i * 20 + 16] - motion_positions_array[i] ;
		r18[i] = joi_pos_array[i * 20 + 17] - motion_positions_array[i] ;
		r19[i] = joi_pos_array[i * 20 + 18] - motion_positions_array[i] ;
		r20[i] = joi_pos_array[i * 20 + 19] - motion_positions_array[i] - (joi_pos_array[19] - motion_positions_array[0]);
		r21[i] = r8[i] - r3[i] - (r8[0] - r3[0]);
		r22[i] = r12[i] - r3[i] - (r12[0] - r3[0]);

	}
	/*for (int i = 0; i < r21.size(); i++){
		r23[i] = i;
	}*/

	//calcLeastSquaresMethod( r21, r23,r24);

	ofstream ofs("joi3Dmoveclearkubite.csv"); //ファイル出力ストリーム


	
	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].z;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].z;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16.size(); j++){
		ofs << r16[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16.size(); j++){
		ofs << r16[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16.size(); j++){
		ofs << r16[j].z;
		ofs << ",";
	}
	ofs << endl;



	for (int j = 0; j < r20.size(); j++){
		ofs << r20[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r20.size(); j++){
		ofs << r20[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r20.size(); j++){
		ofs << r20[j].z;
		ofs << ",";
	}
	ofs << endl;



	return;
}
void out_of_filejoi3Dmoveclearkatate(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array){
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	vector<Point3f> r5;
	vector<Point3f> r6;
	vector<Point3f> r7;
	vector<Point3f> r8;
	vector<Point3f> r9;
	vector<Point3f> r10;
	vector<Point3f> r11;
	vector<Point3f> r12;
	vector<Point3f> r13;
	vector<Point3f> r14;
	vector<Point3f> r15;
	vector<Point3f> r16;
	vector<Point3f> r17;
	vector<Point3f> r18;
	vector<Point3f> r19;
	vector<Point3f> r20;
	vector<Point3f> r21;
	vector<Point3f> r22;
	vector<int> r23;
	vector<float> r24;
	vector<Point3f> r25;
	vector<Point3f> r26;
	vector<Point3f> r27;
	vector<Point3f> r28;
	/*r1.resize(joi_pos_array.size() / 20);
	r2.resize(joi_pos_array.size() / 20);
	r3.resize(joi_pos_array.size() / 20);
	r4.resize(joi_pos_array.size() / 20);
	r5.resize(joi_pos_array.size() / 20);
	r6.resize(joi_pos_array.size() / 20);
	r7.resize(joi_pos_array.size() / 20);
	r8.resize(joi_pos_array.size() / 20);
	r9.resize(joi_pos_array.size() / 20);
	r10.resize(joi_pos_array.size() / 20);
	r11.resize(joi_pos_array.size() / 20);
	r12.resize(joi_pos_array.size() / 20);
	r13.resize(joi_pos_array.size() / 20);
	r14.resize(joi_pos_array.size() / 20);
	r15.resize(joi_pos_array.size() / 20);
	r16.resize(joi_pos_array.size() / 20);
	r17.resize(joi_pos_array.size() / 20);
	r18.resize(joi_pos_array.size() / 20);
	r19.resize(joi_pos_array.size() / 20);
	r20.resize(joi_pos_array.size() / 20);
	r21.resize(joi_pos_array.size() / 20);
	r22.resize(joi_pos_array.size() / 20);
	r23.resize(joi_pos_array.size() / 20);
	r24.resize(joi_pos_array.size() / 20);
	r25.resize(joi_pos_array.size() / 20);
	r26.resize(joi_pos_array.size() / 20);
	r27.resize(joi_pos_array.size() / 20);
	r28.resize(joi_pos_array.size() / 20);*/
	r1.resize(joi_pos_array.size() / 200);
	r2.resize(joi_pos_array.size() / 200);
	r3.resize(joi_pos_array.size() / 200);
	r4.resize(joi_pos_array.size() / 200);
	r5.resize(joi_pos_array.size() / 200);
	r6.resize(joi_pos_array.size() / 200);
	r7.resize(joi_pos_array.size() / 200);
	r8.resize(joi_pos_array.size() / 200);
	r9.resize(joi_pos_array.size() / 200);
	r10.resize(joi_pos_array.size() / 200);
	r11.resize(joi_pos_array.size() / 200);
	r12.resize(joi_pos_array.size() / 200);
	r13.resize(joi_pos_array.size() / 200);
	r14.resize(joi_pos_array.size() / 200);
	r15.resize(joi_pos_array.size() / 200);
	r16.resize(joi_pos_array.size() / 200);
	r17.resize(joi_pos_array.size() / 200);
	r18.resize(joi_pos_array.size() / 200);
	r19.resize(joi_pos_array.size() / 200);
	r20.resize(joi_pos_array.size() / 200);
	r21.resize(joi_pos_array.size() / 200);
	r22.resize(joi_pos_array.size() / 200);
	r23.resize(joi_pos_array.size() / 200);
	r24.resize(joi_pos_array.size() / 200);
	r25.resize(joi_pos_array.size() / 200);
	r26.resize(joi_pos_array.size() / 200);
	r27.resize(joi_pos_array.size() / 200);
	r28.resize(joi_pos_array.size() / 200);
	for (int i = 0; i < (joi_pos_array.size() / 200); i++){
		r1[i] = joi_pos_array[i * 200];
		r2[i] = joi_pos_array[i * 200 + 1];
		r3[i] = joi_pos_array[i * 200 + 2];
		r4[i] = joi_pos_array[i * 200 + 3];
		r5[i] = joi_pos_array[i * 200 + 4];
		r6[i] = joi_pos_array[i * 200 + 5];
		r7[i] = joi_pos_array[i * 200 + 6];
		r8[i] = joi_pos_array[i * 200 + 7];
		r9[i] = joi_pos_array[i * 200 + 8];
		r10[i] = joi_pos_array[i * 200 + 9];
		r11[i] = joi_pos_array[i * 200 + 10] ;
		r12[i] = joi_pos_array[i * 200 + 11];
		r13[i] = joi_pos_array[i * 200 + 12];
		r14[i] = joi_pos_array[i * 200 + 13];
		r15[i] = joi_pos_array[i * 200 + 14];
		r16[i] = joi_pos_array[i * 200 + 15];
		r17[i] = joi_pos_array[i * 200 + 16];
		r18[i] = joi_pos_array[i * 200 + 17];
		r19[i] = joi_pos_array[i * 200 + 18];
		r20[i] = joi_pos_array[i * 200 + 19];
		r21[i] = r8[i] - r5[i] - (r8[0] - r5[0]);
		r22[i] = r12[i] - r9[i] - (r12[0] - r9[0]);
		r25[i] = r16[i] - r13[i] - (r16[0] - r13[0]);
		r26[i] = r20[i] - r17[i] - (r20[0] - r17[0]);
		/*r21[i] = r8[i]  ;
		r22[i] = r12[i]  ;
		r25[i] = r16[i] ;
		r26[i] = r20[i] ;*/
	}
	/*for (int i = 0; i < r21.size(); i++){
	r23[i] = i;
	}*/

	//calcLeastSquaresMethod( r21, r23,r24);

	ofstream ofs("joi3Dmoveclearkatate.csv"); //ファイル出力ストリーム



	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r21.size(); j++){
		ofs << r21[j].z;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r22.size(); j++){
		ofs << r22[j].z;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16.size(); j++){
		ofs << r25[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16.size(); j++){
		ofs << r25[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16.size(); j++){
		ofs << r25[j].z;
		ofs << ",";
	}
	ofs << endl;



	for (int j = 0; j < r20.size(); j++){
		ofs << r26[j].x;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r20.size(); j++){
		ofs << r26[j].y;
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r20.size(); j++){
		ofs << r26[j].z;
		ofs << ",";
	}
	ofs << endl;



	return;
}
void ave(Point3f head_origin_ave, Point3f spine_set, Point3f & spine_ave, Point3f neck_set, Point3f & neck_ave, Point3f head_set, Point3f & head_ave, vector< Point3f> & master_joi_pos_array, const Posture & posture, int j,int a,int b,int c,int d){
	//頭座標設定
	head_origin_ave.set(master_joi_pos_array[a + j*posture.body->num_joints]);
	//肩座標引き算
	spine_set.set(master_joi_pos_array[b + j*posture.body->num_joints]);
	spine_set.sub(head_origin_ave);
	spine_ave.add(spine_set);
	//肩座標引き算
	neck_set.set(master_joi_pos_array[c + j*posture.body->num_joints]);
	neck_set.sub(head_origin_ave);
	neck_ave.add(neck_set);
	//肩座標引き算
	head_set.set(master_joi_pos_array[d + j*posture.body->num_joints]);
	head_set.sub(head_origin_ave);
	head_ave.add(head_set);

}

void initpoint(Point3f & head_origin_ave,  Point3f & spine_ave,  Point3f & neck_ave, Point3f & head_ave){
	head_origin_ave.set(0.0, 0.0, 0.0);
	spine_ave.set(0.0, 0.0, 0.0);
	neck_ave.set(0.0, 0.0, 0.0);
	head_ave.set(0.0, 0.0, 0.0);
	

}

void avecalc(Point3f spine_ave, float & spine_ave_x, float & spine_ave_y, float & spine_ave_z,int a){
	//平均値計算　頭
	spine_ave_x = spine_ave.x / a;
	spine_ave_y = spine_ave.y / a;
	spine_ave_z = spine_ave.z / a;

}

void sumSD(vector< Point3f> & master_joi_pos_array, const Posture & posture, Point3f head_origin, Point3f spine_set, float spine_ave_x, float spine_ave_y, float spine_ave_z, float & spine_SD_xm, float & spine_SD_ym, float & spine_SD_zm, int j ,int a){

	spine_set.set(master_joi_pos_array[a + j*posture.body->num_joints]);
	spine_set.sub(head_origin);
	spine_SD_xm = spine_SD_xm + pow((spine_set.x - spine_ave_x), 2);
	spine_SD_ym = spine_SD_ym + pow((spine_set.y - spine_ave_y), 2);
	spine_SD_zm = spine_SD_zm + pow((spine_set.z - spine_ave_z), 2);

}

void Analogy(float spine_SD_xm, float spine_SD_ym, float spine_SD_zm, float  spine_SD_x, float  spine_SD_y, float  spine_SD_z, float & DoS1h){
	float DoS1htemp = 0.0;
	float DoS2htemp = 0.0;
	float DoS3htemp = 0.0;
	DoS1h = 0.0;
	if (spine_SD_x > spine_SD_xm){
		DoS1htemp = spine_SD_xm / spine_SD_x;
	}
	else{
		DoS1htemp = spine_SD_x / spine_SD_xm;
	}

	if (spine_SD_y > spine_SD_ym){
		DoS2htemp = spine_SD_ym / spine_SD_y;
	}
	else{
		DoS2htemp = spine_SD_y / spine_SD_ym;
	}

	if (spine_SD_z > spine_SD_zm){
		DoS3htemp = spine_SD_zm / spine_SD_z;
	}
	else{
		DoS3htemp = spine_SD_z / spine_SD_zm;
	}
	DoS1h = (DoS1htemp + DoS2htemp + DoS3htemp) / 3.0;

}

void EuclideanDistance(vector<Point3f> master_motion, vector<Point3f> subject_motion,double& D){
	double SUM_x=0.0;
	double SUM_y = 0.0;
	double SUM_z = 0.0;
	double SUM_d = 0.0;
	for (int i = 0; i < 20; i++){
		SUM_x = pow(master_motion[i].x - subject_motion[i].x, 2);
		SUM_y = pow(master_motion[i].y - subject_motion[i].y, 2);
		SUM_z = pow(master_motion[i].z - subject_motion[i].z, 2);
		SUM_d = SUM_d + sqrt(SUM_x+SUM_y+SUM_z);
	}
	D = SUM_d;
}
void ED(Point3f master_motion, Point3f subject_motion, float& D){
	float SUM_x = 0.0;
	float SUM_y = 0.0;
	float SUM_z = 0.0;
	float SUM_d = 0.0;

		SUM_x = pow(master_motion.x - subject_motion.x, 2);
		SUM_y = pow(master_motion.y - subject_motion.y, 2);
		SUM_z = pow(master_motion.z - subject_motion.z, 2);
		SUM_d = sqrt(SUM_x + SUM_y + SUM_z);
	
	D = SUM_d;
}

void out_of_filejoi2(vector< Point3f> & joi_pos_array){
	vector<float> r1;
	vector<float> r2;
	vector<float> r3;
	vector<float> r4;
	vector<float> r5;
	vector<float> r6;
	vector<float> r7;
	vector<float> r8;
	vector<float> r9;
	vector<float> r10;
	vector<float> r11;
	vector<float> r12;
	vector<float> r13;
	vector<float> r14;
	vector<float> r15;
	vector<float> r16;
	vector<float> r17;
	vector<float> r18;
	vector<float> r19;
	vector<float> r20;
	r1.resize((joi_pos_array.size() / 20)-1);
	r2.resize((joi_pos_array.size() / 20) - 1);
	r3.resize((joi_pos_array.size() / 20) - 1);
	r4.resize((joi_pos_array.size() / 20) - 1);
	r5.resize((joi_pos_array.size() / 20) - 1);
	r6.resize((joi_pos_array.size() / 20) - 1);
	r7.resize((joi_pos_array.size() / 20) - 1);
	r8.resize((joi_pos_array.size() / 20) - 1);
	r9.resize((joi_pos_array.size() / 20) - 1);
	r10.resize((joi_pos_array.size() / 20) - 1);
	r11.resize((joi_pos_array.size() / 20) - 1);
	r12.resize((joi_pos_array.size() / 20) - 1);
	r13.resize((joi_pos_array.size() / 20) - 1);
	r14.resize((joi_pos_array.size() / 20) - 1);
	r15.resize((joi_pos_array.size() / 20) - 1);
	r16.resize((joi_pos_array.size() / 20) - 1);
	r17.resize((joi_pos_array.size() / 20) - 1);
	r18.resize((joi_pos_array.size() / 20) - 1);
	r19.resize((joi_pos_array.size() / 20) - 1);
	r20.resize((joi_pos_array.size() / 20) - 1);
	for (int i = 0; i < (joi_pos_array.size() / 20)-1; i++){
		ED(joi_pos_array[i * 20], joi_pos_array[i * 20+20], r1[i]);
		ED(joi_pos_array[i * 20 + 1], joi_pos_array[i * 20 + 20 + 1], r2[i]);
		ED(joi_pos_array[i * 20 + 2], joi_pos_array[i * 20 + 20 + 2], r3[i]);
		ED(joi_pos_array[i * 20 + 3], joi_pos_array[i * 20 + 20 + 3], r4[i]);
		ED(joi_pos_array[i * 20 + 4], joi_pos_array[i * 20 + 20 + 4], r5[i]);
		ED(joi_pos_array[i * 20 + 5], joi_pos_array[i * 20 + 20 + 5], r6[i]);
		ED(joi_pos_array[i * 20 + 6], joi_pos_array[i * 20 + 20 + 6], r7[i]);
		ED(joi_pos_array[i * 20 + 7], joi_pos_array[i * 20 + 20 + 7], r8[i]);
		ED(joi_pos_array[i * 20 + 8], joi_pos_array[i * 20 + 20 + 8], r9[i]);
		ED(joi_pos_array[i * 20 + 9], joi_pos_array[i * 20 + 20 + 9], r10[i]);
		ED(joi_pos_array[i * 20 + 10], joi_pos_array[i * 20 + 20 + 10], r11[i]);
		ED(joi_pos_array[i * 20 + 11], joi_pos_array[i * 20 + 20 + 11], r12[i]);
		ED(joi_pos_array[i * 20 + 12], joi_pos_array[i * 20 + 20 + 12], r13[i]);
		ED(joi_pos_array[i * 20 + 13], joi_pos_array[i * 20 + 20 + 13], r14[i]);
		ED(joi_pos_array[i * 20 + 14], joi_pos_array[i * 20 + 20 + 14], r15[i]);
		ED(joi_pos_array[i * 20 + 15], joi_pos_array[i * 20 + 20 + 15], r16[i]);
		ED(joi_pos_array[i * 20 + 16], joi_pos_array[i * 20 + 20 + 16], r17[i]);
		ED(joi_pos_array[i * 20 + 17], joi_pos_array[i * 20 + 20 + 17], r18[i]);
		ED(joi_pos_array[i * 20 + 18], joi_pos_array[i * 20 + 20 + 18], r19[i]);
		ED(joi_pos_array[i * 20 + 19], joi_pos_array[i * 20 + 20 + 19], r20[i]);

	}

	ofstream ofs("joi2.csv"); //ファイル出力ストリーム
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j];
		ofs << ",";
	}
	ofs << endl;
	
	for (int j = 0; j < r2.size(); j++){
		ofs << r2[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r3.size(); j++){
		ofs << r3[j];
		ofs << ",";
	}
	ofs << endl;


	for (int j = 0; j < r4.size(); j++){
		ofs << r4[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r5.size(); j++){
		ofs << r5[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r6.size(); j++){
		ofs << r6[j];
		ofs << ",";
	}
	ofs << endl;

	
	for (int j = 0; j < r7.size(); j++){
		ofs << r7[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r8.size(); j++){
		ofs << r8[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r9.size(); j++){
		ofs << r9[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r10.size(); j++){
		ofs << r10[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r11.size(); j++){
		ofs << r11[j];
		ofs << ",";
	}
	ofs << endl;


	for (int j = 0; j < r12.size(); j++){
		ofs << r12[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r13.size(); j++){
		ofs << r13[j];
		ofs << ",";
	}
	ofs << endl;


	for (int j = 0; j < r14.size(); j++){
		ofs << r14[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r15.size(); j++){
		ofs << r15[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16.size(); j++){
		ofs << r16[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r17.size(); j++){
		ofs << r17[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r18.size(); j++){
		ofs << r18[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r19.size(); j++){
		ofs << r19[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20.size(); j++){
		ofs << r20[j];
		ofs << ",";
	}
	ofs << endl;



	return;
}
void out_of_filejoi2ave(vector< Point3f> & joi_pos_array){
	vector<float> r1;
	vector<float> r2;
	vector<float> r3;
	vector<float> r4;
	vector<float> r5;
	vector<float> r6;
	vector<float> r7;
	vector<float> r8;
	vector<float> r9;
	vector<float> r10;
	vector<float> r11;
	vector<float> r12;
	vector<float> r13;
	vector<float> r14;
	vector<float> r15;
	vector<float> r16;
	vector<float> r17;
	vector<float> r18;
	vector<float> r19;
	vector<float> r20;
	r1.resize((joi_pos_array.size() / 20) - 1);
	r2.resize((joi_pos_array.size() / 20) - 1);
	r3.resize((joi_pos_array.size() / 20) - 1);
	r4.resize((joi_pos_array.size() / 20) - 1);
	r5.resize((joi_pos_array.size() / 20) - 1);
	r6.resize((joi_pos_array.size() / 20) - 1);
	r7.resize((joi_pos_array.size() / 20) - 1);
	r8.resize((joi_pos_array.size() / 20) - 1);
	r9.resize((joi_pos_array.size() / 20) - 1);
	r10.resize((joi_pos_array.size() / 20) - 1);
	r11.resize((joi_pos_array.size() / 20) - 1);
	r12.resize((joi_pos_array.size() / 20) - 1);
	r13.resize((joi_pos_array.size() / 20) - 1);
	r14.resize((joi_pos_array.size() / 20) - 1);
	r15.resize((joi_pos_array.size() / 20) - 1);
	r16.resize((joi_pos_array.size() / 20) - 1);
	r17.resize((joi_pos_array.size() / 20) - 1);
	r18.resize((joi_pos_array.size() / 20) - 1);
	r19.resize((joi_pos_array.size() / 20) - 1);
	r20.resize((joi_pos_array.size() / 20) - 1);
	for (int i = 0; i < (joi_pos_array.size() / 20) - 1; i++){
		ED(joi_pos_array[i * 20], joi_pos_array[i * 20 + 20], r1[i]);
		ED(joi_pos_array[i * 20 + 1], joi_pos_array[i * 20 + 20 + 1], r2[i]);
		ED(joi_pos_array[i * 20 + 2], joi_pos_array[i * 20 + 20 + 2], r3[i]);
		ED(joi_pos_array[i * 20 + 3], joi_pos_array[i * 20 + 20 + 3], r4[i]);
		ED(joi_pos_array[i * 20 + 4], joi_pos_array[i * 20 + 20 + 4], r5[i]);
		ED(joi_pos_array[i * 20 + 5], joi_pos_array[i * 20 + 20 + 5], r6[i]);
		ED(joi_pos_array[i * 20 + 6], joi_pos_array[i * 20 + 20 + 6], r7[i]);
		ED(joi_pos_array[i * 20 + 7], joi_pos_array[i * 20 + 20 + 7], r8[i]);
		ED(joi_pos_array[i * 20 + 8], joi_pos_array[i * 20 + 20 + 8], r9[i]);
		ED(joi_pos_array[i * 20 + 9], joi_pos_array[i * 20 + 20 + 9], r10[i]);
		ED(joi_pos_array[i * 20 + 10], joi_pos_array[i * 20 + 20 + 10], r11[i]);
		ED(joi_pos_array[i * 20 + 11], joi_pos_array[i * 20 + 20 + 11], r12[i]);
		ED(joi_pos_array[i * 20 + 12], joi_pos_array[i * 20 + 20 + 12], r13[i]);
		ED(joi_pos_array[i * 20 + 13], joi_pos_array[i * 20 + 20 + 13], r14[i]);
		ED(joi_pos_array[i * 20 + 14], joi_pos_array[i * 20 + 20 + 14], r15[i]);
		ED(joi_pos_array[i * 20 + 15], joi_pos_array[i * 20 + 20 + 15], r16[i]);
		ED(joi_pos_array[i * 20 + 16], joi_pos_array[i * 20 + 20 + 16], r17[i]);
		ED(joi_pos_array[i * 20 + 17], joi_pos_array[i * 20 + 20 + 17], r18[i]);
		ED(joi_pos_array[i * 20 + 18], joi_pos_array[i * 20 + 20 + 18], r19[i]);
		ED(joi_pos_array[i * 20 + 19], joi_pos_array[i * 20 + 20 + 19], r20[i]);

	}

	ofstream ofs("joiave.csv"); //ファイル出力ストリーム
	for (int j = 0; j < r1.size(); j++){
		ofs << (r1[j] + r2[j] + r3[j] + r4[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r5.size(); j++){
		ofs << (r5[j] + r6[j] + r7[j] + r8[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r9.size(); j++){
		ofs << (r9[j] + r10[j] + r11[j] + r12[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r13.size(); j++){
		ofs << (r13[j] + r14[j] + r15[j] + r16[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;


	for (int j = 0; j < r17.size(); j++){
		ofs << (r17[j] + r18[j] + r19[j] + r20[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;

	return;
}

void out_of_filejoi2avemoveclear(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array){
	vector<float> r1;
	vector<float> r2;
	vector<float> r3;
	vector<float> r4;
	vector<float> r5;
	vector<float> r6;
	vector<float> r7;
	vector<float> r8;
	vector<float> r9;
	vector<float> r10;
	vector<float> r11;
	vector<float> r12;
	vector<float> r13;
	vector<float> r14;
	vector<float> r15;
	vector<float> r16;
	vector<float> r17;
	vector<float> r18;
	vector<float> r19;
	vector<float> r20;
	r1.resize((joi_pos_array.size() / 20) - 1);
	r2.resize((joi_pos_array.size() / 20) - 1);
	r3.resize((joi_pos_array.size() / 20) - 1);
	r4.resize((joi_pos_array.size() / 20) - 1);
	r5.resize((joi_pos_array.size() / 20) - 1);
	r6.resize((joi_pos_array.size() / 20) - 1);
	r7.resize((joi_pos_array.size() / 20) - 1);
	r8.resize((joi_pos_array.size() / 20) - 1);
	r9.resize((joi_pos_array.size() / 20) - 1);
	r10.resize((joi_pos_array.size() / 20) - 1);
	r11.resize((joi_pos_array.size() / 20) - 1);
	r12.resize((joi_pos_array.size() / 20) - 1);
	r13.resize((joi_pos_array.size() / 20) - 1);
	r14.resize((joi_pos_array.size() / 20) - 1);
	r15.resize((joi_pos_array.size() / 20) - 1);
	r16.resize((joi_pos_array.size() / 20) - 1);
	r17.resize((joi_pos_array.size() / 20) - 1);
	r18.resize((joi_pos_array.size() / 20) - 1);
	r19.resize((joi_pos_array.size() / 20) - 1);
	r20.resize((joi_pos_array.size() / 20) - 1);
	for (int i = 0; i < (joi_pos_array.size() / 20) - 1; i++){
		//胴体
		ED(joi_pos_array[i * 20] - motion_positions_array[i], joi_pos_array[i * 20 + 20] - motion_positions_array[i+1], r1[i]);
		ED(joi_pos_array[i * 20 + 1] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 1] - motion_positions_array[i + 1], r2[i]);
		ED(joi_pos_array[i * 20 + 2] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 2] - motion_positions_array[i + 1], r3[i]);
		ED(joi_pos_array[i * 20 + 3] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 3] - motion_positions_array[i + 1], r4[i]);
		//左手
		ED(joi_pos_array[i * 20 + 4] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 4] - motion_positions_array[i + 1], r5[i]);
		ED(joi_pos_array[i * 20 + 5] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 5] - motion_positions_array[i + 1], r6[i]);
		ED(joi_pos_array[i * 20 + 6] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 6] - motion_positions_array[i + 1], r7[i]);
		ED(joi_pos_array[i * 20 + 7] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 7] - motion_positions_array[i + 1], r8[i]);
		//右手
		ED(joi_pos_array[i * 20 + 8] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 8] - motion_positions_array[i + 1], r9[i]);
		ED(joi_pos_array[i * 20 + 9] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 9] - motion_positions_array[i + 1], r10[i]);
		ED(joi_pos_array[i * 20 + 10] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 10] - motion_positions_array[i + 1], r11[i]);
		ED(joi_pos_array[i * 20 + 11] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 11] - motion_positions_array[i + 1], r12[i]);
		//左足
		ED(joi_pos_array[i * 20 + 12] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 12] - motion_positions_array[i + 1], r13[i]);
		ED(joi_pos_array[i * 20 + 13] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 13] - motion_positions_array[i + 1], r14[i]);
		ED(joi_pos_array[i * 20 + 14] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 14] - motion_positions_array[i + 1], r15[i]);
		ED(joi_pos_array[i * 20 + 15] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 15] - motion_positions_array[i + 1], r16[i]);
		//右足
		ED(joi_pos_array[i * 20 + 16] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 16] - motion_positions_array[i + 1], r17[i]);
		ED(joi_pos_array[i * 20 + 17] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 17] - motion_positions_array[i + 1], r18[i]);
		ED(joi_pos_array[i * 20 + 18] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 18] - motion_positions_array[i + 1], r19[i]);
		ED(joi_pos_array[i * 20 + 19] - motion_positions_array[i], joi_pos_array[i * 20 + 20 + 19] - motion_positions_array[i + 1], r20[i]);

	}

	ofstream ofs("joiavemoveclear.csv"); //ファイル出力ストリーム
	/*for (int j = 0; j < r1.size(); j++){
		ofs << (r1[j] + r2[j] + r3[j] + r4[j]) / 4.0;
		ofs << ",";
	}
	ofs << endl;*/

	for (int j = 0; j < r5.size(); j++){
		ofs <<  r8[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r9.size(); j++){
		ofs << r12[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r13.size(); j++){
		ofs << r16[j];
		ofs << ",";
	}
	ofs << endl;


	for (int j = 0; j < r17.size(); j++){
		ofs << r20[j];
		ofs << ",";
	}
	ofs << endl;

	return;
}

void out_of_filejoi2moveaccelerationclear(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array,float intervalm){
	//10フレーム間隔の加速度（重要）
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	float interval = 0.0;
	interval = intervalm * 10;
	r1.resize((joi_pos_array.size() / 20)-10);
	r2.resize((joi_pos_array.size() / 20) - 10);
	r3.resize((joi_pos_array.size() / 20) - 10);
	r4.resize((joi_pos_array.size() / 20) - 10);
	for (int i = 0; i < (joi_pos_array.size() / 20)-10 ; i++){
		/*r1[i] = (joi_pos_array[i * 200 + 200 + 7] - motion_positions_array[i*10 + 10] - joi_pos_array[i * 200 + 200 + 3]) - (joi_pos_array[i * 200 + 7] - motion_positions_array[i*10] - joi_pos_array[i * 200  + 3]);
		r2[i] = (joi_pos_array[i * 200 + 200 + 11] - motion_positions_array[i*10 + 10] - joi_pos_array[i * 200 + 200 + 3]) - (joi_pos_array[i * 200 + 11] - motion_positions_array[i*10] - joi_pos_array[i * 200 + 3]);
		r3[i] = (joi_pos_array[i * 200 + 200 + 15] - motion_positions_array[i*10 + 10]) - (joi_pos_array[i * 200 + 15] - motion_positions_array[i*10]);
		r4[i] = (joi_pos_array[i * 200 + 200 + 19] - motion_positions_array[i*10 + 10]) - (joi_pos_array[i * 200 + 19] - motion_positions_array[i*10]);*/
		
		//r1[i] = (joi_pos_array[i * 20 + 200 + 7] - motion_positions_array[i + 10] - joi_pos_array[i * 20 + 200 + 3]) - (joi_pos_array[i * 20 + 7] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		//r2[i] = (joi_pos_array[i * 20 + 200 + 11] - motion_positions_array[i + 10] - joi_pos_array[i * 20 + 200 + 3]) - (joi_pos_array[i * 20 + 11] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		//r3[i] = (joi_pos_array[i * 20 + 200 + 15] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 15] - motion_positions_array[i]);
		//r4[i] = (joi_pos_array[i * 20 + 200 + 19] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 19] - motion_positions_array[i]);
		r1[i] = joi_pos_array[i * 20 + 200 + 7] - joi_pos_array[i * 20 + 7];
		r2[i] = joi_pos_array[i * 20 + 200 + 11] - joi_pos_array[i * 20 + 11];
		r3[i] = joi_pos_array[i * 20 + 200 + 15] - joi_pos_array[i * 20 + 15];
		r4[i] = joi_pos_array[i * 20 + 200 + 19] - joi_pos_array[i * 20 + 19];
	}

	ofstream ofs("joiavemoveaccelerationclear2.csv"); //ファイル出力ストリーム
	/*for (int j = 0; j < r1.size(); j++){
	ofs << (r1[j] + r2[j] + r3[j] + r4[j]) / 4.0;
	ofs << ",";
	}
	ofs << endl;*/

	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r1[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r2[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r3[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].x / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].y / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r1.size(); j++){
		ofs << r4[j].z / pow(interval, 2);
		ofs << ",";
	}
	ofs << endl;

	return;
}

void acceleration_detection(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int* SubjectKeyFrame, float intervalm){


	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	float a = 0.0;
	float a_dash = 0.0;
	int a_number = 0;
	float interval = 0.0;
	int hirakisagari = 0;
	interval = intervalm * 10;
	r1.resize((joi_pos_array.size() / 20) - 10);
	r2.resize((joi_pos_array.size() / 20)  - 10);
	r3.resize((joi_pos_array.size() / 20)  - 10);
	r4.resize((joi_pos_array.size() / 20)  - 10);
	for (int i = 0; i <r1.size(); i++){
		/*r1[i] = (joi_pos_array[i * 20 + 200 + 7] - motion_positions_array[i + 10] - joi_pos_array[i * 20 + 200 + 3]) - (joi_pos_array[i * 20 + 7] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		r2[i] = (joi_pos_array[i * 20 + 200 + 11] - motion_positions_array[i + 10] - joi_pos_array[i * 20 + 200 + 3]) - (joi_pos_array[i * 20 + 11] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		r3[i] = (joi_pos_array[i * 20 + 200 + 15] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 15] - motion_positions_array[i]);
		r4[i] = (joi_pos_array[i * 20 + 200 + 19] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 19] - motion_positions_array[i]);
		*/
		r1[i] = joi_pos_array[i  * 20 + 200 + 7] - joi_pos_array[i  * 20 + 7];
		r2[i] = joi_pos_array[i  * 20 + 200 + 11] - joi_pos_array[i  * 20 + 11];
		r3[i] = joi_pos_array[i  * 20 + 200 + 15] - joi_pos_array[i * 20 + 15];
		r4[i] = joi_pos_array[i  * 20 + 200 + 19] - joi_pos_array[i  * 20 + 19];
		r1[i].x = r1[i].x / pow(interval, 2);
		r1[i].y = r1[i].y / pow(interval, 2);
		r1[i].z = r1[i].z / pow(interval, 2);
		r2[i].x = r2[i].x / pow(interval, 2);
		r2[i].y = r2[i].y / pow(interval, 2);
		r2[i].z = r2[i].z / pow(interval, 2);
		r3[i].x = r3[i].x / pow(interval, 2);
		r3[i].y = r3[i].y / pow(interval, 2);
		r3[i].z = r3[i].z / pow(interval, 2);
		r4[i].x = r4[i].x / pow(interval, 2);
		r4[i].y = r4[i].y / pow(interval, 2);
		r4[i].z = r4[i].z / pow(interval, 2);
		/*r1[i] = (joi_pos_array[i * 20 + 20 + 7] - motion_positions_array[i + 10] - joi_pos_array[i * 20 + 20 + 3]) - (joi_pos_array[i * 20 + 7] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		r2[i] = (joi_pos_array[i * 20 + 20 + 11] - motion_positions_array[i+ 10] - joi_pos_array[i * 20 + 20 + 3]) - (joi_pos_array[i * 20 + 11] - motion_positions_array[i] - joi_pos_array[i * 20 + 3]);
		r3[i] = (joi_pos_array[i * 20 + 20 + 15] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 15] - motion_positions_array[i]);
		r4[i] = (joi_pos_array[i * 20 + 20 + 19] - motion_positions_array[i + 10]) - (joi_pos_array[i * 20 + 19] - motion_positions_array[i]);*/
	}

	//蹴り上げ検出
	for (int j = 0; j < r4.size(); j++){
		a = r4[j].y;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[13] = a_number;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	for (int j = SubjectKeyFrame[13]; j < SubjectKeyFrame[13]+100; j++){
		a = r4[j].y;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[14] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	//右脚下がり検出
	for (int j = SubjectKeyFrame[14] + 50; j < r4.size(); j++){
		a = r4[j].z;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[15] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;

	//左脚下がり上受け検出
	for (int j = 0; j < SubjectKeyFrame[13]-50; j++){
		a=r3[j].z;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[7] = a_number;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;

	for (int j = SubjectKeyFrame[7] - 100; j < SubjectKeyFrame[7] + 100; j++){
		a = r2[j].y;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}

	SubjectKeyFrame[8] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	//引手まで
	for (int j = SubjectKeyFrame[8]; j < SubjectKeyFrame[8] + 100; j++){
		a = r2[j].y;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}

	SubjectKeyFrame[8] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	/*if (SubjectKeyFrame[7] > SubjectKeyFrame[8]){
		int a1 = 0;
		a1 = SubjectKeyFrame[7];
		SubjectKeyFrame[7] = SubjectKeyFrame[8];
		SubjectKeyFrame[8] = a1;
	}*/
	//同時受け検出
	for (int j = SubjectKeyFrame[8]+50; j < SubjectKeyFrame[13]-50; j++){
		a = r1[j].x;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[9] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;
	for (int j = SubjectKeyFrame[9]- 100; j < SubjectKeyFrame[9]+100; j++){
		a = r2[j].x;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[10] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;

	if (SubjectKeyFrame[9] > SubjectKeyFrame[10]){
		int a1 = 0;
		a1 = SubjectKeyFrame[10];
		SubjectKeyFrame[10] = SubjectKeyFrame[9];
		SubjectKeyFrame[9] = a1;
	}
	//払い受け検出
	for (int j = SubjectKeyFrame[10]+ 50; j < SubjectKeyFrame[13]; j++){
		a = r1[j].x;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[11] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;

	for (int j = SubjectKeyFrame[11] - 100; j <SubjectKeyFrame[11] + 100; j++){
		a = r2[j].x;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[12] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	if (SubjectKeyFrame[11] > SubjectKeyFrame[12]){
		int a1 = 0;
		a1 = SubjectKeyFrame[12];
		SubjectKeyFrame[12] = SubjectKeyFrame[11];
		SubjectKeyFrame[11] = a1;
	}
	//開きさがり
	for (int j = 0; j <SubjectKeyFrame[7]  -50; j++){
		a = r4[j].z;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[0] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	//鈎突き
	for (int j = SubjectKeyFrame[0] + 50; j <SubjectKeyFrame[7]  - 50; j++){
		a = r1[j].x;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[6] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;
	for (int j = SubjectKeyFrame[6]-100; j <SubjectKeyFrame[6]+ 100; j++){
		a = r2[j].y;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[5] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;
	if (SubjectKeyFrame[5] > SubjectKeyFrame[6]){
		int a1 = 0;
		a1 = SubjectKeyFrame[6];
		SubjectKeyFrame[6] = SubjectKeyFrame[5];
		SubjectKeyFrame[5] = a1;
	}
	//逆突き
	for (int j = SubjectKeyFrame[0]+ 50; j <SubjectKeyFrame[5] - 50; j++){
		a = r2[j].z;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[3] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	for (int j = SubjectKeyFrame[3] ; j <SubjectKeyFrame[3] + 50; j++){
		a = r2[j].z;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[4] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;
	//順突き
	for (int j = SubjectKeyFrame[0] + 50; j <SubjectKeyFrame[3]  - 50; j++){
		a = r1[j].z;
		if (a>a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[1] = a_number ;
	a = 0.0;
	a_dash = 1000.0;
	a_number = 0;
	for (int j = SubjectKeyFrame[1]; j <SubjectKeyFrame[1]+ 50; j++){
		a = r1[j].z;
		if (a<a_dash){
			a_dash = a;
			a_number = j;
		}
	}
	SubjectKeyFrame[2] = a_number ;
	a = 0.0;
	a_dash = 0.0;
	a_number = 0;

	return;
}

void hand_move_detection(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int* SubjectKeyFrame){
	vector<Point3f> r1;
	vector<Point3f> r2;
	vector<Point3f> r3;
	vector<Point3f> r4;
	vector<Point3f> r5;
	vector<Point3f> r6;
	vector<Point3f> r7;
	vector<Point3f> r8;
	vector<Point3f> r9;
	vector<Point3f> r10;
	vector<Point3f> r11;
	vector<Point3f> r12;
	vector<Point3f> r13;
	vector<Point3f> r14;
	vector<Point3f> r15;
	vector<Point3f> r16;
	vector<Point3f> r17;
	vector<Point3f> r18;
	vector<Point3f> r19;
	vector<Point3f> r20;
	float a = 0.0;
	int parameter = 50;
	int a_number = 0;
	r1.resize(joi_pos_array.size() / 20);
	r2.resize(joi_pos_array.size() / 20);
	r3.resize(joi_pos_array.size() / 20);
	r4.resize(joi_pos_array.size() / 20);
	r5.resize(joi_pos_array.size() / 20);
	r6.resize(joi_pos_array.size() / 20);
	r7.resize(joi_pos_array.size() / 20);
	r8.resize(joi_pos_array.size() / 20);
	r9.resize(joi_pos_array.size() / 20);
	r10.resize(joi_pos_array.size() / 20);
	r11.resize(joi_pos_array.size() / 20);
	r12.resize(joi_pos_array.size() / 20);
	r13.resize(joi_pos_array.size() / 20);
	r14.resize(joi_pos_array.size() / 20);
	r15.resize(joi_pos_array.size() / 20);
	r16.resize(joi_pos_array.size() / 20);
	r17.resize(joi_pos_array.size() / 20);
	r18.resize(joi_pos_array.size() / 20);
	r19.resize(joi_pos_array.size() / 20);
	r20.resize(joi_pos_array.size() / 20);
	for (int i = 0; i < joi_pos_array.size() / 20; i++){
		r1[i] = joi_pos_array[i * 20] - motion_positions_array[i];
		r2[i] = joi_pos_array[i * 20 + 1] - motion_positions_array[i];
		r3[i] = joi_pos_array[i * 20 + 2] - motion_positions_array[i];
		r4[i] = joi_pos_array[i * 20 + 3] - motion_positions_array[i];
		r5[i] = joi_pos_array[i * 20 + 4] - motion_positions_array[i];
		r6[i] = joi_pos_array[i * 20 + 5] - motion_positions_array[i];
		r7[i] = joi_pos_array[i * 20 + 6] - motion_positions_array[i];
		r8[i] = joi_pos_array[i * 20 + 7] - motion_positions_array[i];
		r9[i] = joi_pos_array[i * 20 + 8] - motion_positions_array[i];
		r10[i] = joi_pos_array[i * 20 + 9] - motion_positions_array[i];
		r11[i] = joi_pos_array[i * 20 + 10] - motion_positions_array[i];
		r12[i] = joi_pos_array[i * 20 + 11] - motion_positions_array[i];
		r13[i] = joi_pos_array[i * 20 + 12] - motion_positions_array[i];
		r14[i] = joi_pos_array[i * 20 + 13] - motion_positions_array[i];
		r15[i] = joi_pos_array[i * 20 + 14] - motion_positions_array[i];
		r16[i] = joi_pos_array[i * 20 + 15] - motion_positions_array[i];
		r17[i] = joi_pos_array[i * 20 + 16] - motion_positions_array[i];
		r18[i] = joi_pos_array[i * 20 + 17] - motion_positions_array[i];
		r19[i] = joi_pos_array[i * 20 + 18] - motion_positions_array[i];
		r20[i] = joi_pos_array[i * 20 + 19] - motion_positions_array[i];
	}
	//上受け検出
	for (int i = SubjectKeyFrame[4] - parameter; i < SubjectKeyFrame[4] + parameter+150; i++){
		if (r12[i].y>a){
			a = r12[i].y;
			a_number = i;
		}
	}
	SubjectKeyFrame[5] = a_number;
	a = 10000.0;
	a_number = 0;


	//鈎突き検出
	for (int i = 0; i < SubjectKeyFrame[4] - parameter; i++){
		if (r8[i].x<a){
			a = r8[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[3] = a_number;
	a = 0.0;
	a_number = 0;

	for (int i = 0; i < SubjectKeyFrame[4] - parameter; i++){
		if (r8[i].y>a){
			a = r8[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[2] = a_number;
	a = 0.0;
	a_number = 0;
	//逆突き検出
	for (int i = 0; i < SubjectKeyFrame[2] - parameter; i++){
		if (r12[i].z>a){
			a = r12[i].z;
			a_number = i;
		}
	}
	SubjectKeyFrame[1] = a_number;
	a = 0.0;
	a_number = 0;

	//順突き検出
	for (int i = 0; i < SubjectKeyFrame[1] - parameter; i++){
		if (r8[i].z>a){
			a = r8[i].z;
			a_number = i;
		}
	}
	SubjectKeyFrame[0] = a_number;
	a = 1000.0;
	a_number = 0;
	//払い受け検出
	for (int i = SubjectKeyFrame[5] + parameter; i < SubjectKeyFrame[10] - parameter; i++){
		if (r12[i].x<a){
			a = r12[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[9] = a_number;
	a = 0.0;
	a_number = 0;
	for (int i = SubjectKeyFrame[5] + parameter; i < SubjectKeyFrame[10] - parameter; i++){
		if (r8[i].x>a){
			a = r8[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[8] = a_number;
	a = 10000.0;
	a_number = 0;
	//同時受け検出
	for (int i = SubjectKeyFrame[5] + parameter; i < SubjectKeyFrame[8] - parameter; i++){
		if (r8[i].x<a){
			a = r8[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[7] = a_number;
	a = 0.0;
	a_number = 0;

	for (int i = SubjectKeyFrame[5] + parameter; i < SubjectKeyFrame[8] - parameter; i++){
		if (r12[i].x>a){
			a = r12[i].x;
			a_number = i;
		}
	}
	SubjectKeyFrame[6] = a_number;
	a = 0.0;
	a_number = 0;
	return;
}

void Double_hand_move_detection(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int* SubjectKeyFrame){
	//移動方向（１フレームごと）
	vector<float> r8_x;
	vector<float> r8_y;
	vector<float> r8_z;

	vector<float> r12_x;
	vector<float> r12_y;
	vector<float> r12_z;

	vector<float> r16_x;
	vector<float> r16_y;
	vector<float> r16_z;

	vector<float> r20_x;
	vector<float> r20_y;
	vector<float> r20_z;

	float a = 0.0;
	int parameter = 50;
	int a_number = 0;
	r8_x.resize(joi_pos_array.size() / 20);
	r8_y.resize(joi_pos_array.size() / 20);
	r8_z.resize(joi_pos_array.size() / 20);

	r12_x.resize(joi_pos_array.size() / 20);
	r12_y.resize(joi_pos_array.size() / 20);
	r12_z.resize(joi_pos_array.size() / 20);

	r16_x.resize(joi_pos_array.size() / 20);
	r16_y.resize(joi_pos_array.size() / 20);
	r16_z.resize(joi_pos_array.size() / 20);

	r20_x.resize(joi_pos_array.size() / 20);
	r20_y.resize(joi_pos_array.size() / 20);
	r20_z.resize(joi_pos_array.size() / 20);
	for (int i = 0; i < joi_pos_array.size() / 20-1; i++){

		r8_x[i] = (joi_pos_array[i * 20+20 + 7].x - motion_positions_array[i].x)-(joi_pos_array[i * 20 + 7].x - motion_positions_array[i+1].x);
		r8_y[i] = (joi_pos_array[i * 20 + 20 + 7].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 7].y - motion_positions_array[i+1].y);
		r8_z[i] = (joi_pos_array[i * 20 + 20 + 7].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 7].z - motion_positions_array[i+1].z);
		
		r12_x[i] = (joi_pos_array[i * 20 + 20 + 11].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 11].x - motion_positions_array[i+1].x);
		r12_y[i] = (joi_pos_array[i * 20 + 20 + 11].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 11].y - motion_positions_array[i+1].y);
		r12_z[i] = (joi_pos_array[i * 20 + 20 + 11].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 11].z - motion_positions_array[i+1].z);

		r16_x[i] = (joi_pos_array[i * 20 + 20 + 15].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 15].x - motion_positions_array[i+1].x);
		r16_y[i] = (joi_pos_array[i * 20 + 20 + 15].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 15].y - motion_positions_array[i+1].y);
		r16_z[i] = (joi_pos_array[i * 20 + 20 + 15].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 15].z - motion_positions_array[i+1].z);

		r20_x[i] = (joi_pos_array[i * 20 + 20 + 19].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 19].x - motion_positions_array[i+1].x);
		r20_y[i] = (joi_pos_array[i * 20 + 20 + 19].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 19].y - motion_positions_array[i+1].y);
		r20_z[i] = (joi_pos_array[i * 20 + 20 + 19].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 19].z - motion_positions_array[i+1].z);
	
	}
	ofstream ofs("xyzmove.csv"); //ファイル出力ストリーム
	for (int j = 0; j < r8_x.size(); j++){
		ofs << r8_x[j] ;
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r8_y.size(); j++){
		ofs << r8_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r8_z.size(); j++){
		ofs << r8_z[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_x.size(); j++){
		ofs << r12_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_y.size(); j++){
		ofs << r12_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_z.size(); j++){
		ofs << r12_z[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16_x.size(); j++){
		ofs << r16_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16_y.size(); j++){
		ofs << r16_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16_z.size(); j++){
		ofs << r16_z[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_x.size(); j++){
		ofs << r20_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_y.size(); j++){
		ofs << r20_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_z.size(); j++){
		ofs << r20_z[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}

void Double_hand_move_detection_Absolute_value(vector< Point3f> & joi_pos_array, vector< Point3f > &  motion_positions_array, int* SubjectKeyFrame){
	//ｘ・ｙ・ｚの移動量両手両脚先
	vector<float> r8_x;
	vector<float> r8_y;
	vector<float> r8_z;

	vector<float> r12_x;
	vector<float> r12_y;
	vector<float> r12_z;

	vector<float> r16_x;
	vector<float> r16_y;
	vector<float> r16_z;

	vector<float> r20_x;
	vector<float> r20_y;
	vector<float> r20_z;

	float a = 0.0;
	int parameter = 50;
	int a_number = 0;
	r8_x.resize(joi_pos_array.size() / 20);
	r8_y.resize(joi_pos_array.size() / 20);
	r8_z.resize(joi_pos_array.size() / 20);

	r12_x.resize(joi_pos_array.size() / 20);
	r12_y.resize(joi_pos_array.size() / 20);
	r12_z.resize(joi_pos_array.size() / 20);

	r16_x.resize(joi_pos_array.size() / 20);
	r16_y.resize(joi_pos_array.size() / 20);
	r16_z.resize(joi_pos_array.size() / 20);

	r20_x.resize(joi_pos_array.size() / 20);
	r20_y.resize(joi_pos_array.size() / 20);
	r20_z.resize(joi_pos_array.size() / 20);
	for (int i = 0; i < joi_pos_array.size() / 20 - 1; i++){

		r8_x[i] = fabsf((joi_pos_array[i * 20 + 20 + 7].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 7].x - motion_positions_array[i + 1].x));
		r8_y[i] = fabsf((joi_pos_array[i * 20 + 20 + 7].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 7].y - motion_positions_array[i + 1].y));
		r8_z[i] = fabsf((joi_pos_array[i * 20 + 20 + 7].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 7].z - motion_positions_array[i + 1].z));

		r12_x[i] = fabsf((joi_pos_array[i * 20 + 20 + 11].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 11].x - motion_positions_array[i + 1].x));
		r12_y[i] = fabsf((joi_pos_array[i * 20 + 20 + 11].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 11].y - motion_positions_array[i + 1].y));
		r12_z[i] = fabsf((joi_pos_array[i * 20 + 20 + 11].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 11].z - motion_positions_array[i + 1].z));

		r16_x[i] = fabsf((joi_pos_array[i * 20 + 20 + 15].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 15].x - motion_positions_array[i + 1].x));
		r16_y[i] = fabsf((joi_pos_array[i * 20 + 20 + 15].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 15].y - motion_positions_array[i + 1].y));
		r16_z[i] = fabsf((joi_pos_array[i * 20 + 20 + 15].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 15].z - motion_positions_array[i + 1].z));

		r20_x[i] = fabsf((joi_pos_array[i * 20 + 20 + 19].x - motion_positions_array[i].x) - (joi_pos_array[i * 20 + 19].x - motion_positions_array[i + 1].x));
		r20_y[i] = fabsf((joi_pos_array[i * 20 + 20 + 19].y - motion_positions_array[i].y) - (joi_pos_array[i * 20 + 19].y - motion_positions_array[i + 1].y));
		r20_z[i] = fabsf((joi_pos_array[i * 20 + 20 + 19].z - motion_positions_array[i].z) - (joi_pos_array[i * 20 + 19].z - motion_positions_array[i + 1].z));

	}
	ofstream ofs("xyzmoveAbsolute_value.csv"); //ファイル出力ストリーム
	for (int j = 0; j < r8_x.size(); j++){
		ofs << r8_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r8_y.size(); j++){
		ofs << r8_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r8_z.size(); j++){
		ofs << r8_z[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_x.size(); j++){
		ofs << r12_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_y.size(); j++){
		ofs << r12_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r12_z.size(); j++){
		ofs << r12_z[j];
		ofs << ",";
	}
	ofs << endl;
	for (int j = 0; j < r16_x.size(); j++){
		ofs << r16_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16_y.size(); j++){
		ofs << r16_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r16_z.size(); j++){
		ofs << r16_z[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_x.size(); j++){
		ofs << r20_x[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_y.size(); j++){
		ofs << r20_y[j];
		ofs << ",";
	}
	ofs << endl;

	for (int j = 0; j < r20_z.size(); j++){
		ofs << r20_z[j];
		ofs << ",";
	}
	ofs << endl;
	return;
}


void CreateKeyFrame(int* MasterKeyFrame, int* SubjectKeyFrame, vector< Point3f> & joi_pos_array, vector< Point3f> & master_joi_pos_array, vector< Point3f > &  motion_positions_array, vector< Point3f > &  master_motion_positions_array,float interval){
	vector<Point3f> master_motion;
	master_motion.resize(20);
	vector<Point3f> subject_motion;
	subject_motion.resize(20);
	double D=0.0;
	double D1 = 100000.0;
	vector<double> D_Sum;
	D_Sum.resize(motion_positions_array.size());

	acceleration_detection(joi_pos_array, motion_positions_array, SubjectKeyFrame, interval);
	out_of_filejoi2moveaccelerationclear(joi_pos_array, motion_positions_array, interval);
}

void FramePartitionApp(const Posture & posture, vector< Point3f> & joi_pos_array, vector< Point3f> & master_joi_pos_array, vector<int>& frameCandidate, vector<int>& framePoint, vector< Point3f > &  motion_positions_array, vector< Point3f > &  master_motion_positions_array,float interval){
	//マスターデータのフレーム分割点0-後ろ下がり200-順突362-逆突き460-鍵突579-上受749-同時受868-払い受け969-蹴り1107-後ろ下がり1277-構え
	int MasterFramePartition[10] = { 0, 200, 362, 460, 579, 749, 868, 969, 1107, 1277 };
	int MasterKeyFrame[7] = {323,425,548,704,824,935,1046};
	int SubjectKeyFrame[16] = {};
	//int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	framePoint[0] = 0;
	framePoint[9] = 0;
	int point = 0;
	int point2 = 0;
	int point3 = 0;
	int point4 = 0;
	int point5 = 0;
	int point6 = 0;
	int point7 = 0;
	int point8 = 0;
	int point9 = 0;
	int point10 = 0;
	
	vector<float> ruijido1;
	vector<float> ruijido2;
	vector<float> ruijido3;
	vector<float> ruijido4;
	vector<float> ruijido5;
	//頭
	Point3f  head_origin;
	Point3f   spine;
	Point3f  neck;
	Point3f  head;
	//右手
	Point3f  right_origin;
	Point3f   right_Arm;
	Point3f  right_ForeArm;
	Point3f  right_Hand;
	//左手
	Point3f  left_origin;
	Point3f   left_Arm;
	Point3f  left_ForeArm;
	Point3f  left_Hand;
	//右手
	Point3f  right_f_origin;
	Point3f   right_Leg;
	Point3f  right_Foot;
	Point3f  right_ToeBase;
	//右手
	Point3f  left_f_origin;
	Point3f   left_Leg;
	Point3f  left_Foot;
	Point3f  left_ToeBase;

	//頭
	Point3f  head_origin_ave;
	Point3f   spine_ave;
	Point3f  neck_ave;
	Point3f  head_ave;
	//右手
	Point3f  right_origin_ave;
	Point3f   right_Arm_ave;
	Point3f   right_ForeArm_ave;
	Point3f   right_Hand_ave;
	//左手
	Point3f  left_origin_ave ;
	Point3f   left_Arm_ave ;
	Point3f   left_ForeArm_ave;
	Point3f   left_Hand_ave;
	//右足
	Point3f  right_f_origin_ave;
	Point3f   right_Leg_ave;
	Point3f   right_Foot_ave;
	Point3f   right_ToeBase_ave;
	//左足
	Point3f  left_f_origin_ave;
	Point3f   left_Leg_ave;
	Point3f   left_Foot_ave;
	Point3f   left_ToeBase_ave;

	//頭
	Point3f  head_origin_set;
	Point3f   spine_set;
	Point3f  neck_set;
	Point3f  head_set;
	//右手
	Point3f   right_Arm_set;
	Point3f   right_ForeArm_set;
	Point3f   right_Hand_set;
	//左手
	Point3f   left_Arm_set;
	Point3f   left_ForeArm_set;
	Point3f   left_Hand_set;
	//右足
	Point3f   right_Leg_set;
	Point3f   right_Foot_set;
	Point3f   right_ToeBase_set;
	//左足
	Point3f   left_Leg_set;
	Point3f   left_Foot_set;
	Point3f   left_ToeBase_set;

	//頭
	float spine_ave_x = 0.0;
	float spine_ave_y = 0.0;
	float spine_ave_z = 0.0;
	float neck_ave_x = 0.0;
	float neck_ave_y = 0.0;
	float neck_ave_z = 0.0;
	float head_ave_x = 0.0;
	float head_ave_y = 0.0;
	float head_ave_z = 0.0;
	//右手
	float right_Arm_ave_x = 0.0;
	float right_Arm_ave_y = 0.0;
	float right_Arm_ave_z = 0.0;
	float right_ForeArm_ave_x = 0.0;
	float right_ForeArm_ave_y = 0.0;
	float right_ForeArm_ave_z = 0.0;
	float right_Hand_ave_x = 0.0;
	float right_Hand_ave_y = 0.0;
	float right_Hand_ave_z = 0.0;
	//左手
	float left_Arm_ave_x = 0.0;
	float left_Arm_ave_y = 0.0;
	float left_Arm_ave_z = 0.0;
	float left_ForeArm_ave_x = 0.0;
	float left_ForeArm_ave_y = 0.0;
	float left_ForeArm_ave_z = 0.0;
	float left_Hand_ave_x = 0.0;
	float left_Hand_ave_y = 0.0;
	float left_Hand_ave_z = 0.0;
	//右足
	float right_Leg_ave_x = 0.0;
	float right_Leg_ave_y = 0.0;
	float right_Leg_ave_z = 0.0;
	float right_Foot_ave_x = 0.0;
	float right_Foot_ave_y = 0.0;
	float right_Foot_ave_z = 0.0;
	float right_ToeBase_ave_x = 0.0;
	float right_ToeBase_ave_y = 0.0;
	float right_ToeBase_ave_z = 0.0;
	//左足
	float left_Leg_ave_x = 0.0;
	float left_Leg_ave_y = 0.0;
	float left_Leg_ave_z = 0.0;
	float left_Foot_ave_x = 0.0;
	float left_Foot_ave_y = 0.0;
	float left_Foot_ave_z = 0.0;
	float left_ToeBase_ave_x = 0.0;
	float left_ToeBase_ave_y = 0.0;
	float left_ToeBase_ave_z = 0.0;


	//頭
	float spine_SD_xm[8] = {};
	float spine_SD_ym[8] = {};
	float spine_SD_zm[8] = {};
	float neck_SD_xm[8] = {};
	float neck_SD_ym[8] = {};
	float neck_SD_zm[8] = {};
	float head_SD_xm[8] = {};
	float head_SD_ym[8] = {};
	float head_SD_zm[8] = {};
	//左手
	float left_Arm_SD_xm[8] = {};
	float left_Arm_SD_ym[8] = {};
	float left_Arm_SD_zm[8] = {};
	float left_ForeArm_SD_xm[8] = {};
	float left_ForeArm_SD_ym[8] = {};
	float left_ForeArm_SD_zm[8] = {};
	float left_Hand_SD_xm[8] = {};
	float left_Hand_SD_ym[8] = {};
	float left_Hand_SD_zm[8] = {};
	//右手
	float right_Arm_SD_xm[8] = {};
	float right_Arm_SD_ym[8] = {};
	float right_Arm_SD_zm[8] = {};
	float right_ForeArm_SD_xm[8] = {};
	float right_ForeArm_SD_ym[8] = {};
	float right_ForeArm_SD_zm[8] = {};
	float right_Hand_SD_xm[8] = {};
	float right_Hand_SD_ym[8] = {};
	float right_Hand_SD_zm[8] = {};
	//左足
	float left_Leg_SD_xm[8] = {};
	float left_Leg_SD_ym[8] = {};
	float left_Leg_SD_zm[8] = {};
	float left_Foot_SD_xm[8] = {};
	float left_Foot_SD_ym[8] = {};
	float left_Foot_SD_zm[8] = {};
	float left_ToeBase_SD_xm[8] = {};
	float left_ToeBase_SD_ym[8] = {};
	float left_ToeBase_SD_zm[8] = {};
	//右足
	float right_Leg_SD_xm[8] = {};
	float right_Leg_SD_ym[8] = {};
	float right_Leg_SD_zm[8] = {};
	float right_Foot_SD_xm[8] = {};
	float right_Foot_SD_ym[8] = {};
	float right_Foot_SD_zm[8] = {};
	float right_ToeBase_SD_xm[8] = {};
	float right_ToeBase_SD_ym[8] = {};
	float right_ToeBase_SD_zm[8] = {};

	//頭
	float spine_SD_x = 0.0;
	float spine_SD_y = 0.0;
	float spine_SD_z = 0.0;
	float neck_SD_x = 0.0;
	float neck_SD_y = 0.0;
	float neck_SD_z = 0.0;
	float head_SD_x = 0.0;
	float head_SD_y = 0.0;
	float head_SD_z = 0.0;
	//右手
	float right_Arm_SD_x = 0.0;
	float right_Arm_SD_y = 0.0;
	float right_Arm_SD_z = 0.0;
	float right_ForeArm_SD_x = 0.0;
	float right_ForeArm_SD_y = 0.0;
	float right_ForeArm_SD_z = 0.0;
	float right_Hand_SD_x = 0.0;
	float right_Hand_SD_y = 0.0;
	float right_Hand_SD_z = 0.0;
	//左手
	float left_Arm_SD_x = 0.0;
	float left_Arm_SD_y = 0.0;
	float left_Arm_SD_z = 0.0;
	float left_ForeArm_SD_x = 0.0;
	float left_ForeArm_SD_y = 0.0;
	float left_ForeArm_SD_z = 0.0;
	float left_Hand_SD_x = 0.0;
	float left_Hand_SD_y = 0.0;
	float left_Hand_SD_z = 0.0;
	//左足
	float left_Leg_SD_x = 0.0;
	float left_Leg_SD_y = 0.0;
	float left_Leg_SD_z = 0.0;
	float left_Foot_SD_x = 0.0;
	float left_Foot_SD_y = 0.0;
	float left_Foot_SD_z = 0.0;
	float left_ToeBase_SD_x = 0.0;
	float left_ToeBase_SD_y = 0.0;
	float left_ToeBase_SD_z = 0.0;
	//右足
	float right_Leg_SD_x = 0.0;
	float right_Leg_SD_y = 0.0;
	float right_Leg_SD_z = 0.0;
	float right_Foot_SD_x = 0.0;
	float right_Foot_SD_y = 0.0;
	float right_Foot_SD_z = 0.0;
	float right_ToeBase_SD_x = 0.0;
	float right_ToeBase_SD_y = 0.0;
	float right_ToeBase_SD_z = 0.0;

	//頭
	float DoS1h = 0.0;
	float DoS2h = 0.0;
	float DoS3h = 0.0;
	float DoS4h = 0.0;
	//右手
	float DoS1rh = 0.0;
	float DoS2rh = 0.0;
	float DoS3rh = 0.0;
	float DoS4rh = 0.0;
	//左手
	float DoS1lh = 0.0;
	float DoS2lh = 0.0;
	float DoS3lh = 0.0;
	float DoS4lh = 0.0;
	//右足
	float DoS1rf = 0.0;
	float DoS2rf = 0.0;
	float DoS3rf = 0.0;
	float DoS4rf = 0.0;
	//左足
	float DoS1lf = 0.0;
	float DoS2lf = 0.0;
	float DoS3lf = 0.0;
	float DoS4lf = 0.0;

	float DoS4 = 0.0;
	float DoS5 = 0.0;
	float DoS6 = 0.0;
	float DoS7 = 0.0;
	float ruiji[8] = {};
	//頭
	float DoS1htemp = 0.0;
	float DoS2htemp = 0.0;
	float DoS3htemp = 0.0;
	//右手
	float DoS1rhtemp = 0.0;
	float DoS2rhtemp = 0.0;
	float DoS3rhtemp = 0.0;
	//左手
	float DoS1lhtemp = 0.0;
	float DoS2lhtemp = 0.0;
	float DoS3lhtemp = 0.0;
	//右足
	float DoS1rftemp = 0.0;
	float DoS2rftemp = 0.0;
	float DoS3rftemp = 0.0;
	//左足
	float DoS1lftemp = 0.0;
	float DoS2lftemp = 0.0;
	float DoS3lftemp = 0.0;

	float head_SD_ave = 0.0;
	float rh_SD_ave = 0.0;
	float lh_SD_ave = 0.0;
	float rf_SD_ave = 0.0;
	float lf_SD_ave = 0.0;
	int parameter = 20;
	int parameter2 = 20;
	ruijido1.resize(frameCandidate.size());
	ruijido2.resize(frameCandidate.size());
	ruijido3.resize(frameCandidate.size());
	ruijido4.resize(frameCandidate.size());
	ruijido5.resize(frameCandidate.size());
	
	CreateKeyFrame(MasterKeyFrame, SubjectKeyFrame, joi_pos_array, master_joi_pos_array,motion_positions_array, master_motion_positions_array,interval);

//後ろ下がり右脚
//マスターデータ
	for (int cnt=0; cnt < 8;cnt++){
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int j = MasterFramePartition[cnt]; j < MasterFramePartition[cnt+1]; j++){
			//頭
			ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, master_joi_pos_array, posture, j, 0, 1, 2, 3);
			//右肩座標設定
			ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, master_joi_pos_array, posture, j, 8, 9, 10, 11);
			//左肩座標設定
			ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, master_joi_pos_array, posture, j, 4, 5, 6, 7);
			//右腰座標設定
			ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, master_joi_pos_array, posture, j, 16, 17, 18, 19);
			//左腰座標設定
			ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, master_joi_pos_array, posture, j, 12, 13, 14, 15);
		}
		//平均値計算　頭
		avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		//平均値計算　左手
		avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		//平均値計算　右手
		avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		//平均値計算　右足
		avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		//平均値計算　左足
		avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);
		avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]);

		for (int j = MasterFramePartition[cnt]; j < MasterFramePartition[cnt + 1]; j++){
			//頭
			head_origin.set(master_joi_pos_array[0 + j*posture.body->num_joints]);
			sumSD(master_joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_xm[cnt], spine_SD_ym[cnt], spine_SD_zm[cnt], j, 1);
			sumSD(master_joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_xm[cnt], neck_SD_ym[cnt], neck_SD_zm[cnt], j, 2);
			sumSD(master_joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_xm[cnt], head_SD_ym[cnt], head_SD_zm[cnt], j, 3);
			//右手
			right_origin.set(master_joi_pos_array[8 + j*posture.body->num_joints]);
			sumSD(master_joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_xm[cnt], right_Arm_SD_ym[cnt], right_Arm_SD_zm[cnt], j, 9);
			sumSD(master_joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_xm[cnt], right_ForeArm_SD_ym[cnt], right_ForeArm_SD_zm[cnt], j, 10);
			sumSD(master_joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_xm[cnt], right_Hand_SD_ym[cnt], right_Hand_SD_zm[cnt], j, 11);
			//左手
			left_origin.set(master_joi_pos_array[4 + j*posture.body->num_joints]);
			sumSD(master_joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_xm[cnt], left_Arm_SD_ym[cnt], left_Arm_SD_zm[cnt], j, 5);
			sumSD(master_joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_xm[cnt], left_ForeArm_SD_ym[cnt], left_ForeArm_SD_zm[cnt], j, 6);
			sumSD(master_joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_xm[cnt], left_Hand_SD_ym[cnt], left_Hand_SD_zm[cnt], j, 7);
			//右足
			right_f_origin.set(master_joi_pos_array[16 + j*posture.body->num_joints]);
			sumSD(master_joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_xm[cnt], right_Leg_SD_ym[cnt], right_Leg_SD_zm[cnt], j, 17);
			sumSD(master_joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_xm[cnt], right_Foot_SD_ym[cnt], right_Foot_SD_zm[cnt], j, 18);
			sumSD(master_joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_xm[cnt], right_ToeBase_SD_ym[cnt], right_ToeBase_SD_zm[cnt], j, 19);

			//左足
			left_f_origin.set(master_joi_pos_array[12 + j*posture.body->num_joints]);
			sumSD(master_joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_xm[cnt], left_Leg_SD_ym[cnt], left_Leg_SD_zm[cnt], j, 13);
			sumSD(master_joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_xm[cnt], left_Foot_SD_ym[cnt], left_Foot_SD_zm[cnt], j, 14);
			sumSD(master_joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_xm[cnt], left_ToeBase_SD_ym[cnt], left_ToeBase_SD_zm[cnt], j, 15);

		}
		//頭標準偏差
		spine_SD_xm[cnt] = sqrt(spine_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		spine_SD_ym[cnt] = sqrt(spine_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		spine_SD_zm[cnt] = sqrt(spine_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		neck_SD_xm[cnt] = sqrt(neck_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		neck_SD_ym[cnt] = sqrt(neck_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		neck_SD_zm[cnt] = sqrt(neck_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		head_SD_xm[cnt] = sqrt(head_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		head_SD_ym[cnt] = sqrt(head_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		head_SD_zm[cnt] = sqrt(head_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		//右手標準偏差
		right_Arm_SD_xm[cnt] = sqrt(right_Arm_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Arm_SD_ym[cnt] = sqrt(right_Arm_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Arm_SD_zm[cnt] = sqrt(right_Arm_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ForeArm_SD_xm[cnt] = sqrt(right_ForeArm_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ForeArm_SD_ym[cnt] = sqrt(right_ForeArm_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ForeArm_SD_zm[cnt] = sqrt(right_ForeArm_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Hand_SD_xm[cnt] = sqrt(right_Hand_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Hand_SD_ym[cnt] = sqrt(right_Hand_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Hand_SD_zm[cnt] = sqrt(right_Hand_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		//左手標準偏差
		left_Arm_SD_xm[cnt] = sqrt(left_Arm_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Arm_SD_ym[cnt] = sqrt(left_Arm_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Arm_SD_zm[cnt] = sqrt(left_Arm_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ForeArm_SD_xm[cnt] = sqrt(left_ForeArm_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ForeArm_SD_ym[cnt] = sqrt(left_ForeArm_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ForeArm_SD_zm[cnt] = sqrt(left_ForeArm_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Hand_SD_xm[cnt] = sqrt(left_Hand_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Hand_SD_ym[cnt] = sqrt(left_Hand_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Hand_SD_zm[cnt] = sqrt(left_Hand_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		//右足標準偏差
		right_Leg_SD_xm[cnt] = sqrt(right_Leg_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Leg_SD_ym[cnt] = sqrt(right_Leg_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Leg_SD_zm[cnt] = sqrt(right_Leg_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Foot_SD_xm[cnt] = sqrt(right_Foot_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Foot_SD_ym[cnt] = sqrt(right_Foot_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_Foot_SD_zm[cnt] = sqrt(right_Foot_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ToeBase_SD_xm[cnt] = sqrt(right_ToeBase_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ToeBase_SD_ym[cnt] = sqrt(right_ToeBase_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		right_ToeBase_SD_zm[cnt] = sqrt(right_ToeBase_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		//右足標準偏差
		left_Leg_SD_xm[cnt] = sqrt(left_Leg_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Leg_SD_ym[cnt] = sqrt(left_Leg_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Leg_SD_zm[cnt] = sqrt(left_Leg_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Foot_SD_xm[cnt] = sqrt(left_Foot_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Foot_SD_ym[cnt] = sqrt(left_Foot_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_Foot_SD_zm[cnt] = sqrt(left_Foot_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ToeBase_SD_xm[cnt] = sqrt(left_ToeBase_SD_xm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ToeBase_SD_ym[cnt] = sqrt(left_ToeBase_SD_ym[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));
		left_ToeBase_SD_zm[cnt] = sqrt(left_ToeBase_SD_zm[cnt] / (MasterFramePartition[cnt + 1] - MasterFramePartition[cnt]));

	}
			/*head_SD_ave = (spine_SD_xm + spine_SD_ym + spine_SD_zm + neck_SD_xm + neck_SD_ym + neck_SD_zm + head_SD_xm + head_SD_ym + head_SD_zm)/9.0;
			rh_SD_ave = (right_Arm_SD_xm + right_Arm_SD_ym + right_Arm_SD_zm + right_ForeArm_SD_xm + right_ForeArm_SD_ym + right_ForeArm_SD_zm + right_Hand_SD_xm + right_Hand_SD_ym + right_Hand_SD_zm) /9.0;
			lh_SD_ave = (left_Arm_SD_xm + left_Arm_SD_ym + left_Arm_SD_zm + left_ForeArm_SD_xm + left_ForeArm_SD_ym + left_ForeArm_SD_zm + left_Hand_SD_xm + left_Hand_SD_ym + left_Hand_SD_zm) / 9.0;
			rf_SD_ave = (right_Leg_SD_xm + right_Leg_SD_ym + right_Leg_SD_zm + right_Foot_SD_xm + right_Foot_SD_ym + right_Foot_SD_zm + right_ToeBase_SD_xm + right_ToeBase_SD_ym + right_ToeBase_SD_zm)/9.0;
			lf_SD_ave = (left_Leg_SD_xm + left_Leg_SD_ym + left_Leg_SD_zm + left_Foot_SD_xm + left_Foot_SD_ym + left_Foot_SD_zm + left_ToeBase_SD_xm + left_ToeBase_SD_ym + left_ToeBase_SD_zm) / 9.0;
*/
	for (int cnt = 0; framePoint[1] == 0 || framePoint[2] == 0 || framePoint[3] == 0 || framePoint[4] == 0 || framePoint[5] == 0 || framePoint[6] == 0 || framePoint[7] == 0 || framePoint[8] == 0; cnt++){
		parameter = parameter - cnt;
		parameter2 = parameter2 - cnt;
		if (framePoint[1] == 0){
			//初心者データint KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
			//頭
			initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
			//右手
			initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
			//左手
			initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
			//右足
			initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
			//左足
			initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);


			for (int i = 0; i < frameCandidate.size(); i++){

				//if (frameCandidate[i] - 0 < 250 && frameCandidate[i] - 0 > 50){
				if (SubjectKeyFrame[0] + parameter < frameCandidate[i] && SubjectKeyFrame[1] - parameter > frameCandidate[i]){
					//頭
					initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
					//右手
					initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
					//左手
					initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
					//右足
					initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
					//左足
					initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

					for (int j = 0; j < frameCandidate[i]; j++){
						//頭
						ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
						//右肩座標設定
						ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
						//左肩座標設定
						ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
						//右腰座標設定
						ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
						//左腰座標設定
						ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);
					}
					//平均値計算　頭
					avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, frameCandidate[i]);
					avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, frameCandidate[i]);
					avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, frameCandidate[i]);
					//平均値計算　左手
					avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, frameCandidate[i]);
					avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, frameCandidate[i]);
					avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, frameCandidate[i]);
					//平均値計算　右手
					avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, frameCandidate[i]);
					avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, frameCandidate[i]);
					avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, frameCandidate[i]);
					//平均値計算　右足
					avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, frameCandidate[i]);
					avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, frameCandidate[i]);
					avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, frameCandidate[i]);
					//平均値計算　左足
					avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, frameCandidate[i]);
					avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, frameCandidate[i]);
					avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, frameCandidate[i]);
					//頭
					spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
					//右手
					right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
					//左手
					left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
					//左足
					left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
					//右足
					right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;

					for (int j = 0; j < frameCandidate[i]; j++){
						//頭
						head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
						sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
						sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
						sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
						//右手
						right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
						sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
						sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
						sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
						//左手
						left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
						sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
						sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
						sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
						//右足
						right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
						sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
						sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
						sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

						//左足
						left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
						sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
						sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
						sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);
					}

					//頭標準偏差
					spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i]));
					spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i]));
					spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i]));
					neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i]));
					neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i]));
					neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i]));
					head_SD_x = sqrt(head_SD_x / (frameCandidate[i]));
					head_SD_y = sqrt(head_SD_y / (frameCandidate[i]));
					head_SD_z = sqrt(head_SD_z / (frameCandidate[i]));
					//右手標準偏差
					right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i]));
					right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i]));
					right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i]));
					right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i]));
					right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i]));
					right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i]));
					right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i]));
					right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i]));
					right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i]));
					//左手標準偏差
					left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i]));
					left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i]));
					left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i]));
					left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i]));
					left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i]));
					left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i]));
					left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i]));
					left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i]));
					left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i]));
					//右足標準偏差
					right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i]));
					right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i]));
					right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i]));
					right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i]));
					right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i]));
					right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i]));
					right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i]));
					right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i]));
					right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i]));
					//右足標準偏差
					left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i]));
					left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i]));
					left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i]));
					left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i]));
					left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i]));
					left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i]));
					left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i]));
					left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i]));
					left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i]));

					//頭
					//類似度
					Analogy(spine_SD_xm[0], spine_SD_ym[0], spine_SD_zm[0], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
					Analogy(neck_SD_xm[0], neck_SD_ym[0], neck_SD_zm[0], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
					Analogy(head_SD_xm[0], head_SD_ym[0], head_SD_zm[0], head_SD_x, head_SD_y, head_SD_z, DoS3h);
					DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;


					//右手
					//類似度
					Analogy(right_Arm_SD_xm[0], right_Arm_SD_ym[0], right_Arm_SD_zm[0], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
					Analogy(right_ForeArm_SD_xm[0], right_ForeArm_SD_ym[0], right_ForeArm_SD_zm[0], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
					Analogy(right_Hand_SD_xm[0], right_Hand_SD_ym[0], right_Hand_SD_zm[0], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
					DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;

					//左手
					//類似度
					Analogy(left_Arm_SD_xm[0], left_Arm_SD_ym[0], left_Arm_SD_zm[0], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
					Analogy(left_ForeArm_SD_xm[0], left_ForeArm_SD_ym[0], left_ForeArm_SD_zm[0], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
					Analogy(left_Hand_SD_xm[0], left_Hand_SD_ym[0], left_Hand_SD_zm[0], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
					DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;

					//右足
					//類似度
					Analogy(right_Leg_SD_xm[0], right_Leg_SD_ym[0], right_Leg_SD_zm[0], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
					Analogy(right_Foot_SD_xm[0], right_Foot_SD_ym[0], right_Foot_SD_zm[0], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
					Analogy(right_ToeBase_SD_xm[0], right_ToeBase_SD_ym[0], right_ToeBase_SD_zm[0], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
					DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;

					//左足
					//類似度
					Analogy(left_Leg_SD_xm[0], left_Leg_SD_ym[0], left_Leg_SD_zm[0], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
					Analogy(left_Foot_SD_xm[0], left_Foot_SD_ym[0], left_Foot_SD_zm[0], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
					Analogy(left_ToeBase_SD_xm[0], left_ToeBase_SD_ym[0], left_ToeBase_SD_zm[0], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
					DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;
					/*	DoS4lf = DoS4lf *(1 + lf_SD_ave);
						DoS4h = DoS4h*(1 + head_SD_ave);
						DoS4rh = DoS4rh *(1 + rh_SD_ave);
						DoS4lh = DoS4lh *(1 + lh_SD_ave);
						DoS4rf = DoS4rf *(1 + rf_SD_ave);*/
					DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

					if (DoS4 > DoS5){
						//if (DoS4rf > DoS6){
						ruijido1[i] = DoS4h;
						ruijido2[i] = DoS4rh;
						ruijido3[i] = DoS4lh;
						ruijido4[i] = DoS4rf;
						ruijido5[i] = DoS4lf;
						DoS5 = DoS4;
						DoS6 = DoS4rf;
						framePoint[1] = (frameCandidate[i]);
						point = i;
						//}
					}
					/*else if (framePoint[1] != 0){

						break;
						}*/
				}
				//}
			}
		}
	ruiji[0] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
//順突左手int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	if (framePoint[2] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;



		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[1] < 200 && frameCandidate[i] - framePoint[1]>50){
			if (SubjectKeyFrame[2] + parameter < frameCandidate[i] && SubjectKeyFrame[3] - parameter > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[1]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);

				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[1]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[1]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[1]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[1]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[1]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[1]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;


				for (int j = framePoint[1]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[1]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[1]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[1]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[1]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[1]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[1]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[1]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[1]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[1]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[1]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[1]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[1]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[1]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[1]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[1]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[1]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[1]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[1]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[1]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[1]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[1]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[1]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[1]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[1]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[1]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[1]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[1]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[1]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[1]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[1]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[1]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[1]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[1]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[1]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[1]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[1]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[1]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[1]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[1]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[1]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[1]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[1]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[1]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[1]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[1]));
				//頭
				//類似度
				Analogy(spine_SD_xm[1], spine_SD_ym[1], spine_SD_zm[1], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[1], neck_SD_ym[1], neck_SD_zm[1], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[1], head_SD_ym[1], head_SD_zm[1], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;

				//右手
				//類似度
				Analogy(right_Arm_SD_xm[1], right_Arm_SD_ym[1], right_Arm_SD_zm[1], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[1], right_ForeArm_SD_ym[1], right_ForeArm_SD_zm[1], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[1], right_Hand_SD_ym[1], right_Hand_SD_zm[1], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;

				//左手
				//類似度
				Analogy(left_Arm_SD_xm[1], left_Arm_SD_ym[1], left_Arm_SD_zm[1], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[1], left_ForeArm_SD_ym[1], left_ForeArm_SD_zm[1], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[1], left_Hand_SD_ym[1], left_Hand_SD_zm[1], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;

				//右足
				//類似度
				Analogy(right_Leg_SD_xm[1], right_Leg_SD_ym[1], right_Leg_SD_zm[1], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[1], right_Foot_SD_ym[1], right_Foot_SD_zm[1], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[1], right_ToeBase_SD_ym[1], right_ToeBase_SD_zm[1], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;

				//左足
				//類似度
				Analogy(left_Leg_SD_xm[1], left_Leg_SD_ym[1], left_Leg_SD_zm[1], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[1], left_Foot_SD_ym[1], left_Foot_SD_zm[1], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[1], left_ToeBase_SD_ym[1], left_ToeBase_SD_zm[1], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4lh > DoS6){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4lh;
					framePoint[2] = (frameCandidate[i]);
					point2 = i;
					//}
				}
				/*else if (framePoint[2]!=0){
					break;
					}*/
			}
			//}

		}
	}
	ruiji[1] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
//逆突右手int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	if (framePoint[3] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);



		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point2; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[2]< 200 && frameCandidate[i] - framePoint[2]>50){
			if (SubjectKeyFrame[4] + parameter < frameCandidate[i] && SubjectKeyFrame[5] - parameter > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[2]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);
				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[2]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[2]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[2]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[2]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[2]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[2]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;


				for (int j = framePoint[2]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[2]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[2]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[2]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[2]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[2]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[2]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[2]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[2]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[2]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[2]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[2]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[2]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[2]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[2]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[2]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[2]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[2]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[2]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[2]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[2]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[2]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[2]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[2]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[2]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[2]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[2]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[2]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[2]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[2]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[2]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[2]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[2]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[2]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[2]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[2]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[2]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[2]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[2]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[2]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[2]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[2]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[2]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[2]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[2]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[2]));
				//頭
				//類似度
				Analogy(spine_SD_xm[2], spine_SD_ym[2], spine_SD_zm[2], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[2], neck_SD_ym[2], neck_SD_zm[2], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[2], head_SD_ym[2], head_SD_zm[2], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[2], right_Arm_SD_ym[2], right_Arm_SD_zm[2], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[2], right_ForeArm_SD_ym[2], right_ForeArm_SD_zm[2], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[2], right_Hand_SD_ym[2], right_Hand_SD_zm[2], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[2], left_Arm_SD_ym[2], left_Arm_SD_zm[2], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[2], left_ForeArm_SD_ym[2], left_ForeArm_SD_zm[2], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[2], left_Hand_SD_ym[2], left_Hand_SD_zm[2], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[2], right_Leg_SD_ym[2], right_Leg_SD_zm[2], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[2], right_Foot_SD_ym[2], right_Foot_SD_zm[2], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[2], right_ToeBase_SD_ym[2], right_ToeBase_SD_zm[2], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[2], left_Leg_SD_ym[2], left_Leg_SD_zm[2], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[2], left_Foot_SD_ym[2], left_Foot_SD_zm[2], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[2], left_ToeBase_SD_ym[2], left_ToeBase_SD_zm[2], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rh > DoS6){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4rh;
					framePoint[3] = (frameCandidate[i]);
					point3 = i;

					//}
				}
				/*else if (framePoint[3] != 0){
					break;
					}*/
			}
			//}
		}
	}
	ruiji[2] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
//鈎突右手左手int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };

	if (framePoint[4] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;


		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point3; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[3] < 250 && frameCandidate[i] - framePoint[3]>50){
			if (SubjectKeyFrame[6] + parameter < frameCandidate[i] && SubjectKeyFrame[7] - parameter > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[3]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);


				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[3]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[3]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[3]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[3]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[3]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[3]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;

				for (int j = framePoint[3]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, right_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, right_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[3]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[3]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[3]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[3]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[3]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[3]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[3]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[3]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[3]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[3]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[3]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[3]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[3]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[3]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[3]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[3]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[3]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[3]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[3]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[3]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[3]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[3]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[3]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[3]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[3]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[3]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[3]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[3]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[3]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[3]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[3]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[3]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[3]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[3]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[3]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[3]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[3]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[3]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[3]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[3]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[3]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[3]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[3]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[3]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[3]));
				//頭
				//類似度
				Analogy(spine_SD_xm[3], spine_SD_ym[3], spine_SD_zm[3], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[3], neck_SD_ym[3], neck_SD_zm[3], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[3], head_SD_ym[3], head_SD_zm[3], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[3], right_Arm_SD_ym[3], right_Arm_SD_zm[3], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[3], right_ForeArm_SD_ym[3], right_ForeArm_SD_zm[3], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[3], right_Hand_SD_ym[3], right_Hand_SD_zm[3], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[3], left_Arm_SD_ym[3], left_Arm_SD_zm[3], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[3], left_ForeArm_SD_ym[3], left_ForeArm_SD_zm[3], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[3], left_Hand_SD_ym[3], left_Hand_SD_zm[3], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[3], right_Leg_SD_ym[3], right_Leg_SD_zm[3], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[3], right_Foot_SD_ym[3], right_Foot_SD_zm[3], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[3], right_ToeBase_SD_ym[3], right_ToeBase_SD_zm[3], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[3], left_Leg_SD_ym[3], left_Leg_SD_zm[3], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[3], left_Foot_SD_ym[3], left_Foot_SD_zm[3], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[3], left_ToeBase_SD_ym[3], left_ToeBase_SD_zm[3], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rh > DoS6&&DoS4lh > DoS7){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4rh;
					DoS7 = DoS4lh;
					framePoint[4] = (frameCandidate[i]);
					point4 = i;
					//}
				}
				/*else if (framePoint[4] != 0){
					break;
					}*/
			}
			//}
		}
	}
	ruiji[3] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
	DoS7 = 0.0;
//上受左脚右手int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	if (framePoint[5] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;



		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point4; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[4] < 300 && frameCandidate[i] - framePoint[4]>50){
			if (SubjectKeyFrame[8] + parameter2 < frameCandidate[i] && SubjectKeyFrame[9] - parameter2 > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[4]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);

				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[4]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[4]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[4]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[4]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[4]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[4]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;



				for (int j = framePoint[4]; j < frameCandidate[i]; j++){

					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[4]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[4]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[4]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[4]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[4]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[4]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[4]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[4]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[4]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[4]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[4]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[4]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[4]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[4]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[4]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[4]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[4]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[4]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[4]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[4]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[4]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[4]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[4]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[4]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[4]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[4]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[4]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[4]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[4]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[4]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[4]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[4]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[4]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[4]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[4]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[4]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[4]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[4]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[4]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[4]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[4]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[4]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[4]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[4]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[4]));
				//頭
				//類似度
				Analogy(spine_SD_xm[4], spine_SD_ym[4], spine_SD_zm[4], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[4], neck_SD_ym[4], neck_SD_zm[4], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[4], head_SD_ym[4], head_SD_zm[4], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[4], right_Arm_SD_ym[4], right_Arm_SD_zm[4], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[4], right_ForeArm_SD_ym[4], right_ForeArm_SD_zm[4], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[4], right_Hand_SD_ym[4], right_Hand_SD_zm[4], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[4], left_Arm_SD_ym[4], left_Arm_SD_zm[4], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[4], left_ForeArm_SD_ym[4], left_ForeArm_SD_zm[4], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[4], left_Hand_SD_ym[4], left_Hand_SD_zm[4], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[4], right_Leg_SD_ym[4], right_Leg_SD_zm[4], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[4], right_Foot_SD_ym[4], right_Foot_SD_zm[4], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[4], right_ToeBase_SD_ym[4], right_ToeBase_SD_zm[4], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[4], left_Leg_SD_ym[4], left_Leg_SD_zm[4], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[4], left_Foot_SD_ym[4], left_Foot_SD_zm[4], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[4], left_ToeBase_SD_ym[4], left_ToeBase_SD_zm[4], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rh > DoS7){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4lf;
					DoS7 = DoS4rh;
					framePoint[5] = (frameCandidate[i]);
					point5 = i;
					//	}
				}
				/*else if (framePoint[5] != 0){
					break;
					}*/
			}
			//}
		}
	}
	ruiji[4] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
	DoS7 = 0.0;
//同時受右手左手int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	//頭

	if (framePoint[6] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;

		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point5; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[5] < 200 && frameCandidate[i] - framePoint[5]>50){
			if (SubjectKeyFrame[10] + parameter2 < frameCandidate[i] && SubjectKeyFrame[11] - parameter2 > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[5]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);

				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[5]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[5]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[5]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[5]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[5]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[5]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;

				for (int j = framePoint[5]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[5]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[5]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[5]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[5]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[5]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[5]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[5]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[5]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[5]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[5]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[5]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[5]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[5]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[5]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[5]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[5]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[5]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[5]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[5]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[5]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[5]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[5]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[5]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[5]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[5]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[5]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[5]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[5]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[5]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[5]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[5]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[5]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[5]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[5]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[5]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[5]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[5]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[5]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[5]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[5]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[5]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[5]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[5]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[5]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[5]));
				//類似度
				Analogy(spine_SD_xm[5], spine_SD_ym[5], spine_SD_zm[5], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[5], neck_SD_ym[5], neck_SD_zm[5], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[5], head_SD_ym[5], head_SD_zm[5], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[5], right_Arm_SD_ym[5], right_Arm_SD_zm[5], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[5], right_ForeArm_SD_ym[5], right_ForeArm_SD_zm[5], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[5], right_Hand_SD_ym[5], right_Hand_SD_zm[5], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[5], left_Arm_SD_ym[5], left_Arm_SD_zm[5], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[5], left_ForeArm_SD_ym[5], left_ForeArm_SD_zm[5], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[5], left_Hand_SD_ym[5], left_Hand_SD_zm[5], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[5], right_Leg_SD_ym[5], right_Leg_SD_zm[5], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[5], right_Foot_SD_ym[5], right_Foot_SD_zm[5], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[5], right_ToeBase_SD_ym[5], right_ToeBase_SD_zm[5], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[5], left_Leg_SD_ym[5], left_Leg_SD_zm[5], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[5], left_Foot_SD_ym[5], left_Foot_SD_zm[5], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[5], left_ToeBase_SD_ym[5], left_ToeBase_SD_zm[5], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rh > DoS6&&DoS4lh > DoS7){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4rh;
					DoS7 = DoS4lh;
					framePoint[6] = (frameCandidate[i]);
					point6 = i;
					//}
				}
				/*else if (framePoint[6] !=0){
					break;
					}*/
			}
			//	}
		}
	}
	ruiji[5] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
	DoS7 = 0.0;
//払い受け右手左手
	//頭
	if (framePoint[7] == 0){
		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;

		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point6; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[6] < 250 && frameCandidate[i] - framePoint[6]>50){
			if (SubjectKeyFrame[12] + parameter2 < frameCandidate[i] && SubjectKeyFrame[13] - parameter2 > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[6]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);


				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[6]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[6]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[6]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[6]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[6]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[6]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;

				for (int j = framePoint[6]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[6]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[6]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[6]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[6]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[6]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[6]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[6]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[6]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[6]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[6]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[6]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[6]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[6]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[6]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[6]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[6]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[6]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[6]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[6]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[6]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[6]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[6]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[6]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[6]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[6]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[6]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[6]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[6]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[6]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[6]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[6]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[6]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[6]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[6]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[6]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[6]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[6]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[6]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[6]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[6]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[6]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[6]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[6]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[6]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[6]));
				//頭
				//類似度
				Analogy(spine_SD_xm[6], spine_SD_ym[6], spine_SD_zm[6], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[6], neck_SD_ym[6], neck_SD_zm[6], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[6], head_SD_ym[6], head_SD_zm[6], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[6], right_Arm_SD_ym[6], right_Arm_SD_zm[6], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[6], right_ForeArm_SD_ym[6], right_ForeArm_SD_zm[6], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[6], right_Hand_SD_ym[6], right_Hand_SD_zm[6], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[6], left_Arm_SD_ym[6], left_Arm_SD_zm[6], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[6], left_ForeArm_SD_ym[6], left_ForeArm_SD_zm[6], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[6], left_Hand_SD_ym[6], left_Hand_SD_zm[6], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[6], right_Leg_SD_ym[6], right_Leg_SD_zm[6], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[6], right_Foot_SD_ym[6], right_Foot_SD_zm[6], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[6], right_ToeBase_SD_ym[6], right_ToeBase_SD_zm[6], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[6], left_Leg_SD_ym[6], left_Leg_SD_zm[6], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[6], left_Foot_SD_ym[6], left_Foot_SD_zm[6], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[6], left_ToeBase_SD_ym[6], left_ToeBase_SD_zm[6], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rh > DoS6&&DoS4lh > DoS7){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4rh;
					DoS7 = DoS4lh;
					framePoint[7] = (frameCandidate[i]);
					point7 = i;
					//}
					/*else if (framePoint[7] != 0){
						break;
						}*/
				}
			}
			//}
		}
	}
	ruiji[6] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
	DoS7 = 0.0;
//蹴り右脚
	if (framePoint[8] == 0){

		//頭
		spine_SD_x = 0.0;
		spine_SD_y = 0.0;
		spine_SD_z = 0.0;
		neck_SD_x = 0.0;
		neck_SD_y = 0.0;
		neck_SD_z = 0.0;
		head_SD_x = 0.0;
		head_SD_y = 0.0;
		head_SD_z = 0.0;
		//右手
		right_Arm_SD_x = 0.0;
		right_Arm_SD_y = 0.0;
		right_Arm_SD_z = 0.0;
		right_ForeArm_SD_x = 0.0;
		right_ForeArm_SD_y = 0.0;
		right_ForeArm_SD_z = 0.0;
		right_Hand_SD_x = 0.0;
		right_Hand_SD_y = 0.0;
		right_Hand_SD_z = 0.0;
		//左手
		left_Arm_SD_x = 0.0;
		left_Arm_SD_y = 0.0;
		left_Arm_SD_z = 0.0;
		left_ForeArm_SD_x = 0.0;
		left_ForeArm_SD_y = 0.0;
		left_ForeArm_SD_z = 0.0;
		left_Hand_SD_x = 0.0;
		left_Hand_SD_y = 0.0;
		left_Hand_SD_z = 0.0;
		//左足
		left_Leg_SD_x = 0.0;
		left_Leg_SD_y = 0.0;
		left_Leg_SD_z = 0.0;
		left_Foot_SD_x = 0.0;
		left_Foot_SD_y = 0.0;
		left_Foot_SD_z = 0.0;
		left_ToeBase_SD_x = 0.0;
		left_ToeBase_SD_y = 0.0;
		left_ToeBase_SD_z = 0.0;
		//右足
		right_Leg_SD_x = 0.0;
		right_Leg_SD_y = 0.0;
		right_Leg_SD_z = 0.0;
		right_Foot_SD_x = 0.0;
		right_Foot_SD_y = 0.0;
		right_Foot_SD_z = 0.0;
		right_ToeBase_SD_x = 0.0;
		right_ToeBase_SD_y = 0.0;
		right_ToeBase_SD_z = 0.0;


		//初心者データ
		//頭
		initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
		//右手
		initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
		//左手
		initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
		//右足
		initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
		//左足
		initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

		for (int i = point7; i < frameCandidate.size(); i++){

			//if (frameCandidate[i] - framePoint[7] < 250 && frameCandidate[i] - framePoint[7]>50){
			if (SubjectKeyFrame[14] + parameter2 < frameCandidate[i] && SubjectKeyFrame[15] - parameter2 > frameCandidate[i]){
				//頭
				initpoint(head_origin_ave, spine_ave, neck_ave, head_ave);
				//右手
				initpoint(right_origin_ave, right_Arm_ave, right_ForeArm_ave, right_Hand_ave);
				//左手
				initpoint(left_origin_ave, left_Arm_ave, left_ForeArm_ave, left_Hand_ave);
				//右足
				initpoint(right_f_origin_ave, right_Foot_ave, right_Leg_ave, right_ToeBase_ave);
				//左足
				initpoint(left_f_origin_ave, left_Foot_ave, left_Leg_ave, left_ToeBase_ave);

				for (int j = framePoint[7]; j < frameCandidate[i]; j++){
					//頭
					ave(head_origin_ave, spine_set, spine_ave, neck_set, neck_ave, head_set, head_ave, joi_pos_array, posture, j, 0, 1, 2, 3);
					//右肩座標設定
					ave(right_origin_ave, right_Arm_set, right_Arm_ave, right_ForeArm_set, right_ForeArm_ave, right_Hand_set, right_Hand_ave, joi_pos_array, posture, j, 8, 9, 10, 11);
					//左肩座標設定
					ave(left_origin_ave, left_Arm_set, left_Arm_ave, left_ForeArm_set, left_ForeArm_ave, left_Hand_set, left_Hand_ave, joi_pos_array, posture, j, 4, 5, 6, 7);
					//右腰座標設定
					ave(right_f_origin_ave, right_Leg_set, right_Leg_ave, right_Foot_set, right_Foot_ave, right_ToeBase_set, right_ToeBase_ave, joi_pos_array, posture, j, 16, 17, 18, 19);
					//左腰座標設定
					ave(left_f_origin_ave, left_Leg_set, left_Leg_ave, left_Foot_set, left_Foot_ave, left_ToeBase_set, left_ToeBase_ave, joi_pos_array, posture, j, 12, 13, 14, 15);



				}
				//平均値計算　頭
				avecalc(spine_ave, spine_ave_x, spine_ave_y, spine_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(neck_ave, neck_ave_x, neck_ave_y, neck_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(head_ave, head_ave_x, head_ave_y, head_ave_z, (frameCandidate[i] - framePoint[7]));
				//平均値計算　左手
				avecalc(left_Arm_ave, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(left_ForeArm_ave, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(left_Hand_ave, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, (frameCandidate[i] - framePoint[7]));
				//平均値計算　右手
				avecalc(right_Arm_ave, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(right_ForeArm_ave, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(right_Hand_ave, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, (frameCandidate[i] - framePoint[7]));
				//平均値計算　右足
				avecalc(right_Leg_ave, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(right_Foot_ave, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(right_ToeBase_ave, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, (frameCandidate[i] - framePoint[7]));
				//平均値計算　左足
				avecalc(left_Leg_ave, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(left_Foot_ave, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, (frameCandidate[i] - framePoint[7]));
				avecalc(left_ToeBase_ave, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, (frameCandidate[i] - framePoint[7]));

				//頭
				spine_SD_x = 0.0; spine_SD_y = 0.0; spine_SD_z = 0.0; neck_SD_x = 0.0; neck_SD_y = 0.0; neck_SD_z = 0.0; head_SD_x = 0.0; head_SD_y = 0.0; head_SD_z = 0.0;
				//右手
				right_Arm_SD_x = 0.0; right_Arm_SD_y = 0.0; right_Arm_SD_z = 0.0; right_ForeArm_SD_x = 0.0; right_ForeArm_SD_y = 0.0; right_ForeArm_SD_z = 0.0; right_Hand_SD_x = 0.0; right_Hand_SD_y = 0.0; right_Hand_SD_z = 0.0;
				//左手
				left_Arm_SD_x = 0.0; left_Arm_SD_y = 0.0; left_Arm_SD_z = 0.0; left_ForeArm_SD_x = 0.0; left_ForeArm_SD_y = 0.0; left_ForeArm_SD_z = 0.0; left_Hand_SD_x = 0.0; left_Hand_SD_y = 0.0; left_Hand_SD_z = 0.0;
				//左足
				left_Leg_SD_x = 0.0; left_Leg_SD_y = 0.0; left_Leg_SD_z = 0.0; left_Foot_SD_x = 0.0; left_Foot_SD_y = 0.0; left_Foot_SD_z = 0.0; left_ToeBase_SD_x = 0.0; left_ToeBase_SD_y = 0.0; left_ToeBase_SD_z = 0.0;
				//右足
				right_Leg_SD_x = 0.0; right_Leg_SD_y = 0.0; right_Leg_SD_z = 0.0; right_Foot_SD_x = 0.0; right_Foot_SD_y = 0.0; right_Foot_SD_z = 0.0; right_ToeBase_SD_x = 0.0; right_ToeBase_SD_y = 0.0; right_ToeBase_SD_z = 0.0;


				for (int j = framePoint[7]; j < frameCandidate[i]; j++){
					//頭
					head_origin.set(joi_pos_array[0 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, head_origin, spine_set, spine_ave_x, spine_ave_y, spine_ave_z, spine_SD_x, spine_SD_y, spine_SD_z, j, 1);
					sumSD(joi_pos_array, posture, head_origin, neck_set, neck_ave_x, neck_ave_y, neck_ave_z, neck_SD_x, neck_SD_y, neck_SD_z, j, 2);
					sumSD(joi_pos_array, posture, head_origin, head_set, head_ave_x, head_ave_y, head_ave_z, head_SD_x, head_SD_y, head_SD_z, j, 3);
					//右手
					right_origin.set(joi_pos_array[8 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_origin, right_Arm_set, right_Arm_ave_x, right_Arm_ave_y, right_Arm_ave_z, right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, j, 9);
					sumSD(joi_pos_array, posture, right_origin, right_ForeArm_set, right_ForeArm_ave_x, right_ForeArm_ave_y, right_ForeArm_ave_z, right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, j, 10);
					sumSD(joi_pos_array, posture, right_origin, right_Hand_set, right_Hand_ave_x, right_Hand_ave_y, right_Hand_ave_z, right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, j, 11);
					//左手
					left_origin.set(joi_pos_array[4 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_origin, left_Arm_set, left_Arm_ave_x, left_Arm_ave_y, left_Arm_ave_z, left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, j, 5);
					sumSD(joi_pos_array, posture, left_origin, left_ForeArm_set, left_ForeArm_ave_x, left_ForeArm_ave_y, left_ForeArm_ave_z, left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, j, 6);
					sumSD(joi_pos_array, posture, left_origin, left_Hand_set, left_Hand_ave_x, left_Hand_ave_y, left_Hand_ave_z, left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, j, 7);
					//右足
					right_f_origin.set(joi_pos_array[16 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, right_f_origin, right_Leg_set, right_Leg_ave_x, right_Leg_ave_y, right_Leg_ave_z, right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, j, 17);
					sumSD(joi_pos_array, posture, right_f_origin, right_Foot_set, right_Foot_ave_x, right_Foot_ave_y, right_Foot_ave_z, right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, j, 18);
					sumSD(joi_pos_array, posture, right_f_origin, right_ToeBase_set, right_ToeBase_ave_x, right_ToeBase_ave_y, right_ToeBase_ave_z, right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, j, 19);

					//左足
					left_f_origin.set(joi_pos_array[12 + j*posture.body->num_joints]);
					sumSD(joi_pos_array, posture, left_f_origin, left_Leg_set, left_Leg_ave_x, left_Leg_ave_y, left_Leg_ave_z, left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, j, 13);
					sumSD(joi_pos_array, posture, left_f_origin, left_Foot_set, left_Foot_ave_x, left_Foot_ave_y, left_Foot_ave_z, left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, j, 14);
					sumSD(joi_pos_array, posture, left_f_origin, left_ToeBase_set, left_ToeBase_ave_x, left_ToeBase_ave_y, left_ToeBase_ave_z, left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, j, 15);

				}
				//頭標準偏差
				spine_SD_x = sqrt(spine_SD_x / (frameCandidate[i] - framePoint[7]));
				spine_SD_y = sqrt(spine_SD_y / (frameCandidate[i] - framePoint[7]));
				spine_SD_z = sqrt(spine_SD_z / (frameCandidate[i] - framePoint[7]));
				neck_SD_x = sqrt(neck_SD_x / (frameCandidate[i] - framePoint[7]));
				neck_SD_y = sqrt(neck_SD_y / (frameCandidate[i] - framePoint[7]));
				neck_SD_z = sqrt(neck_SD_z / (frameCandidate[i] - framePoint[7]));
				head_SD_x = sqrt(head_SD_x / (frameCandidate[i] - framePoint[7]));
				head_SD_y = sqrt(head_SD_y / (frameCandidate[i] - framePoint[7]));
				head_SD_z = sqrt(head_SD_z / (frameCandidate[i] - framePoint[7]));
				//右手標準偏差
				right_Arm_SD_x = sqrt(right_Arm_SD_x / (frameCandidate[i] - framePoint[7]));
				right_Arm_SD_y = sqrt(right_Arm_SD_y / (frameCandidate[i] - framePoint[7]));
				right_Arm_SD_z = sqrt(right_Arm_SD_z / (frameCandidate[i] - framePoint[7]));
				right_ForeArm_SD_x = sqrt(right_ForeArm_SD_x / (frameCandidate[i] - framePoint[7]));
				right_ForeArm_SD_y = sqrt(right_ForeArm_SD_y / (frameCandidate[i] - framePoint[7]));
				right_ForeArm_SD_z = sqrt(right_ForeArm_SD_z / (frameCandidate[i] - framePoint[7]));
				right_Hand_SD_x = sqrt(right_Hand_SD_x / (frameCandidate[i] - framePoint[7]));
				right_Hand_SD_y = sqrt(right_Hand_SD_y / (frameCandidate[i] - framePoint[7]));
				right_Hand_SD_z = sqrt(right_Hand_SD_z / (frameCandidate[i] - framePoint[7]));
				//左手標準偏差
				left_Arm_SD_x = sqrt(left_Arm_SD_x / (frameCandidate[i] - framePoint[7]));
				left_Arm_SD_y = sqrt(left_Arm_SD_y / (frameCandidate[i] - framePoint[7]));
				left_Arm_SD_z = sqrt(left_Arm_SD_z / (frameCandidate[i] - framePoint[7]));
				left_ForeArm_SD_x = sqrt(left_ForeArm_SD_x / (frameCandidate[i] - framePoint[7]));
				left_ForeArm_SD_y = sqrt(left_ForeArm_SD_y / (frameCandidate[i] - framePoint[7]));
				left_ForeArm_SD_z = sqrt(left_ForeArm_SD_z / (frameCandidate[i] - framePoint[7]));
				left_Hand_SD_x = sqrt(left_Hand_SD_x / (frameCandidate[i] - framePoint[7]));
				left_Hand_SD_y = sqrt(left_Hand_SD_y / (frameCandidate[i] - framePoint[7]));
				left_Hand_SD_z = sqrt(left_Hand_SD_z / (frameCandidate[i] - framePoint[7]));
				//右足標準偏差
				right_Leg_SD_x = sqrt(right_Leg_SD_x / (frameCandidate[i] - framePoint[7]));
				right_Leg_SD_y = sqrt(right_Leg_SD_y / (frameCandidate[i] - framePoint[7]));
				right_Leg_SD_z = sqrt(right_Leg_SD_z / (frameCandidate[i] - framePoint[7]));
				right_Foot_SD_x = sqrt(right_Foot_SD_x / (frameCandidate[i] - framePoint[7]));
				right_Foot_SD_y = sqrt(right_Foot_SD_y / (frameCandidate[i] - framePoint[7]));
				right_Foot_SD_z = sqrt(right_Foot_SD_z / (frameCandidate[i] - framePoint[7]));
				right_ToeBase_SD_x = sqrt(right_ToeBase_SD_x / (frameCandidate[i] - framePoint[7]));
				right_ToeBase_SD_y = sqrt(right_ToeBase_SD_y / (frameCandidate[i] - framePoint[7]));
				right_ToeBase_SD_z = sqrt(right_ToeBase_SD_z / (frameCandidate[i] - framePoint[7]));
				//右足標準偏差
				left_Leg_SD_x = sqrt(left_Leg_SD_x / (frameCandidate[i] - framePoint[7]));
				left_Leg_SD_y = sqrt(left_Leg_SD_y / (frameCandidate[i] - framePoint[7]));
				left_Leg_SD_z = sqrt(left_Leg_SD_z / (frameCandidate[i] - framePoint[7]));
				left_Foot_SD_x = sqrt(left_Foot_SD_x / (frameCandidate[i] - framePoint[7]));
				left_Foot_SD_y = sqrt(left_Foot_SD_y / (frameCandidate[i] - framePoint[7]));
				left_Foot_SD_z = sqrt(left_Foot_SD_z / (frameCandidate[i] - framePoint[7]));
				left_ToeBase_SD_x = sqrt(left_ToeBase_SD_x / (frameCandidate[i] - framePoint[7]));
				left_ToeBase_SD_y = sqrt(left_ToeBase_SD_y / (frameCandidate[i] - framePoint[7]));
				left_ToeBase_SD_z = sqrt(left_ToeBase_SD_z / (frameCandidate[i] - framePoint[7]));
				//頭
				//類似度
				Analogy(spine_SD_xm[7], spine_SD_ym[7], spine_SD_zm[7], spine_SD_x, spine_SD_y, spine_SD_z, DoS1h);
				Analogy(neck_SD_xm[7], neck_SD_ym[7], neck_SD_zm[7], neck_SD_x, neck_SD_y, neck_SD_z, DoS2h);
				Analogy(head_SD_xm[7], head_SD_ym[7], head_SD_zm[7], head_SD_x, head_SD_y, head_SD_z, DoS3h);
				DoS4h = (DoS1h + DoS2h + DoS3h) / 3.0;
				//右手
				//類似度
				Analogy(right_Arm_SD_xm[7], right_Arm_SD_ym[7], right_Arm_SD_zm[7], right_Arm_SD_x, right_Arm_SD_y, right_Arm_SD_z, DoS1rh);
				Analogy(right_ForeArm_SD_xm[7], right_ForeArm_SD_ym[7], right_ForeArm_SD_zm[7], right_ForeArm_SD_x, right_ForeArm_SD_y, right_ForeArm_SD_z, DoS2rh);
				Analogy(right_Hand_SD_xm[7], right_Hand_SD_ym[7], right_Hand_SD_zm[7], right_Hand_SD_x, right_Hand_SD_y, right_Hand_SD_z, DoS3rh);
				DoS4rh = (DoS1rh + DoS2rh + DoS3rh) / 3.0;
				//左手
				//類似度
				Analogy(left_Arm_SD_xm[7], left_Arm_SD_ym[7], left_Arm_SD_zm[7], left_Arm_SD_x, left_Arm_SD_y, left_Arm_SD_z, DoS1lh);
				Analogy(left_ForeArm_SD_xm[7], left_ForeArm_SD_ym[7], left_ForeArm_SD_zm[7], left_ForeArm_SD_x, left_ForeArm_SD_y, left_ForeArm_SD_z, DoS2lh);
				Analogy(left_Hand_SD_xm[7], left_Hand_SD_ym[7], left_Hand_SD_zm[7], left_Hand_SD_x, left_Hand_SD_y, left_Hand_SD_z, DoS3lh);
				DoS4lh = (DoS1lh + DoS2lh + DoS3lh) / 3.0;
				//右足
				//類似度
				Analogy(right_Leg_SD_xm[7], right_Leg_SD_ym[7], right_Leg_SD_zm[7], right_Leg_SD_x, right_Leg_SD_y, right_Leg_SD_z, DoS1rf);
				Analogy(right_Foot_SD_xm[7], right_Foot_SD_ym[7], right_Foot_SD_zm[7], right_Foot_SD_x, right_Foot_SD_y, right_Foot_SD_z, DoS2rf);
				Analogy(right_ToeBase_SD_xm[7], right_ToeBase_SD_ym[7], right_ToeBase_SD_zm[7], right_ToeBase_SD_x, right_ToeBase_SD_y, right_ToeBase_SD_z, DoS3rf);
				DoS4rf = (DoS1rf + DoS2rf + DoS3rf) / 3.0;
				//左足
				//類似度
				Analogy(left_Leg_SD_xm[7], left_Leg_SD_ym[7], left_Leg_SD_zm[7], left_Leg_SD_x, left_Leg_SD_y, left_Leg_SD_z, DoS1lf);
				Analogy(left_Foot_SD_xm[7], left_Foot_SD_ym[7], left_Foot_SD_zm[7], left_Foot_SD_x, left_Foot_SD_y, left_Foot_SD_z, DoS2lf);
				Analogy(left_ToeBase_SD_xm[7], left_ToeBase_SD_ym[7], left_ToeBase_SD_zm[7], left_ToeBase_SD_x, left_ToeBase_SD_y, left_ToeBase_SD_z, DoS3lf);
				DoS4lf = (DoS1lf + DoS2lf + DoS3lf) / 3.0;

				/*DoS4h = DoS4h*(1 + head_SD_ave);
				DoS4rh = DoS4rh *(1 + rh_SD_ave);
				DoS4lh = DoS4lh *(1 + lh_SD_ave);
				DoS4rf = DoS4rf *(1 + rf_SD_ave);
				DoS4lf = DoS4lf *(1 + lf_SD_ave);*/

				DoS4 = (DoS4h + DoS4rh + DoS4lh + DoS4rf + DoS4lf) / 5.0;

				if (DoS4 > DoS5){
					//if (DoS4rf > DoS6){
					ruijido1[i] = DoS4h;
					ruijido2[i] = DoS4rh;
					ruijido3[i] = DoS4lh;
					ruijido4[i] = DoS4rf;
					ruijido5[i] = DoS4lf;
					DoS5 = DoS4;
					DoS6 = DoS4rf;
					framePoint[8] = (frameCandidate[i]);
					point8 = i;
					//}
				}
				/*else if (framePoint[8] !=0){
					break;
					}*/
			}
			//}
		}
	}
	ruiji[7] = DoS5;
	DoS5 = 0.0;
	DoS6 = 0.0;
	out_of_file4(ruijido1, ruijido2, ruijido3, ruijido4, ruijido5);
	
	}
}


//
//  順運動学計算
//
void  MyForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array)
{
	// 配列初期化
	seg_frame_array.resize(posture.body->num_segments);
	joi_pos_array.resize(posture.body->num_joints);

	// ルート体節の位置・向きを設定
	seg_frame_array[0].set(posture.root_ori, posture.root_pos, 1.0f);

	// Forward Kinematics 計算のための反復計算（ルート体節から末端体節に向かって繰り返し計算）
	MyForwardKinematicsIteration(posture.body->segments[0], NULL, posture, &seg_frame_array.front(), &joi_pos_array.front());
}


//
//  Forward Kinematics 計算のための反復計算（ルート体節から末端体節に向かって繰り返し再帰呼び出し）
//
void  MyForwardKinematicsIteration(
	const Segment *  segment, const Segment * prev_segment, const Posture & posture,
	Matrix4f * seg_frame_array, Point3f * joi_pos_array)
{//現在の体節segmentと一つ前の体節prev_cegment＋現在姿勢を入力posture　全体節の位置・向きの配列 seg_frame_arrayと全関節の位置の配列 joi_pos_arrayも計算結果を格納するための引数として渡す
	//seg_frame_arrayとjoi_pos_arrayは描画する際に用いる
	const Skeleton *  body = posture.body;
	Joint *  next_joint;
	Segment *  next_segment;
	Segment *  set_segment;//追加
	Matrix4f  mat;
	Vector3f  trans;
	Matrix4f  rot_mat;
	// 現在の体節に接続している各関節に対して繰り返し
	for (int j = 0; j<segment->joints.size(); j++)
	{
		// 次の関節・次の体節を取得
		next_joint = segment->joints[j];
		if (next_joint->segments[0] != segment)
			next_segment = next_joint->segments[0];
		else
			next_segment = next_joint->segments[1];

		// 前の体節側（ルート体節側）の関節はスキップ
		if (next_segment == prev_segment)
			continue;

		// ※ レポート課題
		// 現在の体節の変換行列を取得
		mat = seg_frame_array[segment->index];

		// 次の関節・体節の変換行列を計算

		// 現在の体節の座標系から、接続関節への座標系への平行移動をかける
		segment->joint_positions[j].get(&trans);
		mat.transform(&trans);
		mat.m03 += trans.x;
		mat.m13 += trans.y;
		mat.m23 += trans.z;
		// 次の関節の位置を設定
		if (joi_pos_array)
			joi_pos_array[next_joint->index].set(mat.m03, mat.m13, mat.m23);

		// 関節の回転行列をかける
		rot_mat.set(posture.joint_rotations[next_joint->index]);
		mat.mul(mat, rot_mat);

		// 関節の座標系から、次の体節の座標系への平行移動をかける
		next_segment->joint_positions[0].get(&trans);
		mat.transform(&trans);
		mat.m03 -= trans.x;
		mat.m13 -= trans.y;
		mat.m23 -= trans.z;
		

		// 次の体節の変換行列を設定
		seg_frame_array[next_segment->index] = mat;


		//set_segment = next_segment;
		////set_segment->joint_positions[0] = mat*set_segment->joint_positions[0];
		//set_segment->joint_positions[0].x = mat.m00*next_segment->joint_positions[0].x + mat.m01*next_segment->joint_positions[0].y + mat.m02*next_segment->joint_positions[0].z + mat.m03*1;
		//set_segment->joint_positions[0].y = mat.m10*next_segment->joint_positions[0].x + mat.m11*next_segment->joint_positions[0].y + mat.m12*next_segment->joint_positions[0].z + mat.m13*1;
		//set_segment->joint_positions[0].z = mat.m20*next_segment->joint_positions[0].x + mat.m21*next_segment->joint_positions[0].y + mat.m22*next_segment->joint_positions[0].z + mat.m23*1;
		//next_segment->joint_positions[0].x = set_segment->joint_positions[0].x;
		//next_segment->joint_positions[0].y = set_segment->joint_positions[0].y;
		//next_segment->joint_positions[0].z = set_segment->joint_positions[0].z;
		//
		///*next_segment->joint_positions[0].x +=  joi_pos_array[0].x;
		//next_segment->joint_positions[0].y += joi_pos_array[0].y;
		//next_segment->joint_positions[0].z += joi_pos_array[0].z;*/

		//mat.m00 = posture.joint_rotations[next_segment->index].m00;
		//mat.m01 = posture.joint_rotations[next_segment->index].m01;
		//mat.m02 = posture.joint_rotations[next_segment->index].m02;
		//mat.m03 = next_segment->joint_positions[0].x;
		//mat.m10 = posture.joint_rotations[next_segment->index].m10;
		//mat.m11 = posture.joint_rotations[next_segment->index].m11;
		//mat.m12 = posture.joint_rotations[next_segment->index].m12;
		//mat.m13 = next_segment->joint_positions[0].y;
		//mat.m20 = posture.joint_rotations[next_segment->index].m20;
		//mat.m21 = posture.joint_rotations[next_segment->index].m21;
		//mat.m22 = posture.joint_rotations[next_segment->index].m22;
		//mat.m23 = next_segment->joint_positions[0].z;
		//mat.m30 = 0;
		//mat.m31 = 0;
		//mat.m32 = 0;
		//mat.m33 = 1;
		//// 次の関節の位置を設定
		//	if ( joi_pos_array )
		//			joi_pos_array[ next_joint->index ] = next_segment->joint_positions[0];

		//////// 次の体節の変換行列を設定
		//	if ( seg_frame_array )
		//		seg_frame_array[next_segment->index] = mat;

		// 次の体節に対して繰り返し（再帰呼び出し）
		MyForwardKinematicsIteration(next_segment, segment, posture, seg_frame_array, joi_pos_array);
	}


}


void  AllMyForwardKinematicsIteration(const int i,const int joint_num,
	const Segment *  segment, const Segment * prev_segment, const Posture & posture,
	Matrix4f * seg_frame_array, Point3f * joi_pos_array )
{//現在の体節segmentと一つ前の体節prev_cegment＋現在姿勢を入力posture　全体節の位置・向きの配列 seg_frame_arrayと全関節の位置の配列 joi_pos_arrayも計算結果を格納するための引数として渡す
	//seg_frame_arrayとjoi_pos_arrayは描画する際に用いる
	const Skeleton *  body = posture.body;
	Joint *  next_joint;
	Segment *  next_segment;
	Segment *  set_segment;//追加
	Matrix4f  mat;
	Vector3f  trans;
	Matrix4f  rot_mat;
	// 現在の体節に接続している各関節に対して繰り返し
	for (int j = 0; j<segment->joints.size(); j++)
	{
		// 次の関節・次の体節を取得
		next_joint = segment->joints[j];
		if (next_joint->segments[0] != segment)
			next_segment = next_joint->segments[0];
		else
			next_segment = next_joint->segments[1];

		// 前の体節側（ルート体節側）の関節はスキップ
		if (next_segment == prev_segment)
			continue;

		// ※ レポート課題
		// 現在の体節の変換行列を取得
		mat = seg_frame_array[segment->index + i*joint_num];

		// 次の関節・体節の変換行列を計算

		// 現在の体節の座標系から、接続関節への座標系への平行移動をかける
		segment->joint_positions[j].get(&trans);
		mat.transform(&trans);
		mat.m03 += trans.x;
		mat.m13 += trans.y;
		mat.m23 += trans.z;
		// 次の関節の位置を設定
		if (joi_pos_array)
			joi_pos_array[next_joint->index + i*joint_num].set(mat.m03, mat.m13, mat.m23);

		// 関節の回転行列をかける
		rot_mat.set(posture.joint_rotations[next_joint->index]);
		mat.mul(mat, rot_mat);

		// 関節の座標系から、次の体節の座標系への平行移動をかける
		next_segment->joint_positions[0].get(&trans);
		mat.transform(&trans);
		mat.m03 -= trans.x;
		mat.m13 -= trans.y;
		mat.m23 -= trans.z;


		// 次の体節の変換行列を設定
		seg_frame_array[next_segment->index+i*joint_num] = mat;


		// 次の体節に対して繰り返し（再帰呼び出し）
		AllMyForwardKinematicsIteration(i, joint_num, next_segment, segment, posture, seg_frame_array, joi_pos_array);
	}
}

void  AllMyForwardKinematicsIteration_end(const int i, const int joint_num,
	const Segment *  segment, const Segment * prev_segment, const Posture & posture,
	Matrix4f * seg_frame_array, Point3f * joi_pos_array, int& cnt, vector< Point3f > &  end_site_set, Point3f * end_site_array)
{//現在の体節segmentと一つ前の体節prev_cegment＋現在姿勢を入力posture　全体節の位置・向きの配列 seg_frame_arrayと全関節の位置の配列 joi_pos_arrayも計算結果を格納するための引数として渡す
	//seg_frame_arrayとjoi_pos_arrayは描画する際に用いる
	const Skeleton *  body = posture.body;
	Joint *  next_joint;
	Segment *  next_segment;
	Segment *  set_segment;//追加
	Matrix4f  mat;
	Vector3f  trans;
	Matrix4f  rot_mat;
	// 現在の体節に接続している各関節に対して繰り返し
	for (int j = 0; j<segment->joints.size(); j++)
	{
		// 次の関節・次の体節を取得
		next_joint = segment->joints[j];
		if (next_joint->segments[0] != segment)
			next_segment = next_joint->segments[0];
		else
			next_segment = next_joint->segments[1];

		// 前の体節側（ルート体節側）の関節はスキップ
		if (next_segment == prev_segment)
			continue;

		// ※ レポート課題
		// 現在の体節の変換行列を取得
		mat = seg_frame_array[segment->index + i*joint_num];

		// 次の関節・体節の変換行列を計算

		// 現在の体節の座標系から、接続関節への座標系への平行移動をかける
		segment->joint_positions[j].get(&trans);
		mat.transform(&trans);
		mat.m03 += trans.x;
		mat.m13 += trans.y;
		mat.m23 += trans.z;
		// 次の関節の位置を設定
		if (joi_pos_array)
			joi_pos_array[next_joint->index + i*joint_num].set(mat.m03, mat.m13, mat.m23);

		// 関節の回転行列をかける
		rot_mat.set(posture.joint_rotations[next_joint->index]);
		mat.mul(mat, rot_mat);

		// 関節の座標系から、次の体節の座標系への平行移動をかける
		next_segment->joint_positions[0].get(&trans);
		mat.transform(&trans);
		mat.m03 -= trans.x;
		mat.m13 -= trans.y;
		mat.m23 -= trans.z;


		// 次の体節の変換行列を設定
		seg_frame_array[next_segment->index + i*joint_num] = mat;


		// 次の体節に対して繰り返し（再帰呼び出し）
		AllMyForwardKinematicsIteration_end(i, joint_num, next_segment, segment, posture, seg_frame_array, joi_pos_array, cnt, end_site_set, end_site_array);
	}
	if (segment->has_site == true){
		// 現在の体節の変換行列を取得
		mat = seg_frame_array[segment->index + i*joint_num];

		// 次の関節・体節の変換行列を計算

		// 現在の体節の座標系から、接続関節への座標系への平行移動をかける
		end_site_set[cnt].get(&trans);
		mat.transform(&trans);
		mat.m03 += trans.x;
		mat.m13 += trans.y;
		mat.m23 += trans.z;
		// 次の関節の位置を設定
		end_site_array[cnt + i*end_site_set.size()].set(mat.m03, mat.m13, mat.m23);
		cnt++;
	}
}


void  AllMyForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motionFK, vector< Point3f > &  motion_positions_array)
{
	Posture * postureFK =NULL;
	Skeleton * body = NULL;
	postureFK = new Posture();
	body = motionFK.body;
	InitPosture(*postureFK, body);
	// 配列初期化
	/*seg_frame_array.resize(posture.body->num_segments*motionFK.num_frames);
	joi_pos_array.resize(posture.body->num_joints*motionFK.num_frames);*/

	// Forward Kinematics 計算のための反復計算（ルート体節から末端体節に向かって繰り返し計算）
	for (int i = 0; i < motionFK.num_frames; i++){
		if (postureFK)
			delete  postureFK;
		postureFK = new Posture();
		InitPosture(*postureFK, body);
		motionFK.GetPosture(i, *postureFK);
		motion_positions_array[i].set((*postureFK).root_pos);
		// ルート体節の位置・向きを設定
		seg_frame_array[i*posture.body->num_joints].set((*postureFK).root_ori, (*postureFK).root_pos, 1.0f);
		AllMyForwardKinematicsIteration(i, posture.body->num_joints, (*postureFK).body->segments[0], NULL, (*postureFK), &seg_frame_array.front(), &joi_pos_array.front());

	}
}
void  AllMyForwardKinematics_end(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motionFK, vector< Point3f > &  motion_positions_array, vector< Point3f > &  end_site_set, vector< Point3f > &  end_site_array)
{
	Posture * postureFK = NULL;
	Skeleton * body = NULL;
	int cnt = 0;
	postureFK = new Posture();
	body = motionFK.body;
	InitPosture(*postureFK, body);
	// 配列初期化
	/*seg_frame_array.resize(posture.body->num_segments*motionFK.num_frames);
	joi_pos_array.resize(posture.body->num_joints*motionFK.num_frames);*/

	// Forward Kinematics 計算のための反復計算（ルート体節から末端体節に向かって繰り返し計算）
	for (int i = 0; i < motionFK.num_frames; i++){
		if (postureFK)
			delete  postureFK;
		postureFK = new Posture();
		InitPosture(*postureFK, body);
		motionFK.GetPosture(i, *postureFK);
		motion_positions_array[i].set((*postureFK).root_pos);
		// ルート体節の位置・向きを設定
		seg_frame_array[i*posture.body->num_joints].set((*postureFK).root_ori, (*postureFK).root_pos, 1.0f);
		AllMyForwardKinematicsIteration_end(i, posture.body->num_joints, (*postureFK).body->segments[0], NULL, (*postureFK), &seg_frame_array.front(), &joi_pos_array.front(), cnt, end_site_set, &end_site_array.front());

		cnt = 0;
	}
}
void  AllMyForwardKinematics2(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array, Motion & motionFK, Motion & motionFK2, vector< Point3f > &  motion_positions_array)
{
	Posture * postureFK = NULL;
	Skeleton * body = NULL;
	postureFK = new Posture();
	body = motionFK2.body;
	InitPosture(*postureFK, body);
	// 配列初期化
	/*seg_frame_array.resize(posture.body->num_segments*motionFK.num_frames);
	joi_pos_array.resize(posture.body->num_joints*motionFK.num_frames);*/

	// Forward Kinematics 計算のための反復計算（ルート体節から末端体節に向かって繰り返し計算）
	for (int i = 0; i < motionFK.num_frames; i++){
		if (postureFK)
			delete  postureFK;
		postureFK = new Posture();
		InitPosture(*postureFK, body);
		motionFK.GetPosture(i, *postureFK);
		motion_positions_array[i].set((*postureFK).root_pos);
		// ルート体節の位置・向きを設定
		seg_frame_array[i*posture.body->num_joints].set((*postureFK).root_ori, (*postureFK).root_pos, 1.0f);
		AllMyForwardKinematicsIteration(i, posture.body->num_joints, (*postureFK).body->segments[0], NULL, (*postureFK), &seg_frame_array.front(), &joi_pos_array.front());

	}
}



//右手のベクトル生成
void CreateVector(vector< Point3f > &  joi_pos_array,vector< Vector3f >& right_hand_array,int a,int b,int start){
	//posture

	for (int i = 0; i < right_hand_array.size(); i++){
		
		right_hand_array[i].sub(joi_pos_array[a + (i + start) * 20], joi_pos_array[b + (i + start) * 20]);
		
	}
}


void MotionComparison(vector<float> & cos_Stationary, vector<float> & cos_Active, vector<int>& framePoint, vector<int>& Active_masterframe, vector<int>& Active_frame, vector< Point3f> & joi_pos_array, vector< Point3f> & joi_pos_array_master)
{

	vector<Vector3f> joint_vector;
	vector<Vector3f> joint_vector2;
	Vector3f* r1 = new Vector3f();
	Vector3f* r2 = new Vector3f();
	Vector3f* r1_dash = new Vector3f();
	Vector3f* r2_dash = new Vector3f();
	//マスターデータのフレーム分割点0-後ろ下がり200-順突362-逆突き460-鍵突579-上受749-同時受868-払い受け969-蹴り1107-後ろ下がり1277-構え
	int MasterFramePartition[10] = { 0, 200, 362, 460, 579, 749, 868, 969, 1107, 1277 };
	//仮
	int KakkinFramePartition[10] = { 0, 200, 346, 486, 640, 854, 999, 1138, 1326, 1555 };
	int cnt = 0;
	int a[16] = {7,2,11,2,7,2,11,2,11,2,11,2,19,0,3,0};
	float b = 0.0;
	float zz = 0.0;
	float angle_r1 = 0.0;
	float angle_r2 = 0.0;
	
	//~~~~~~~~~比較フレーム取得~~~~~~~~~~~~



	for (int i = 0; i < 7; i++){
		joint_vector.resize(framePoint[i + 2] - framePoint[i + 1]);
		CreateVector(joi_pos_array, joint_vector, a[i*2], a[i*2 + 1], framePoint[i + 1]);
		b = 0.0;
		if (i != 6){	
			for (int j = 0; j < joint_vector.size(); j++){

				if (b < joint_vector[j].length()){
					b = joint_vector[j].length();
					Active_frame[i] = j + framePoint[i + 1];
				}
			}
		}
		else{
			b = 100000.0;
			//蹴り上げのみ
			joint_vector2.resize(framePoint[i + 2] - framePoint[i + 1]);
			CreateVector(joi_pos_array, joint_vector2, a[i * 2+2], a[i * 2 + 3], framePoint[i + 1]);
			for (int j = 0; j < joint_vector.size(); j++){
				joint_vector[j].normalize();
				joint_vector2[j].normalize();
				if (b > joint_vector[j].angle(joint_vector2[j])){
					b = joint_vector[j].angle(joint_vector2[j]);
					Active_frame[i] = j + framePoint[i + 1];
				}
			}
		}
		b = 0.0;
		joint_vector.resize(MasterFramePartition[i + 2] - MasterFramePartition[i + 1]);
		CreateVector(joi_pos_array_master, joint_vector, a[i*2], a[i*2 + 1], MasterFramePartition[i + 1]);
		if (i != 6){
			for (int j = 0; j < joint_vector.size(); j++){
				if (b < joint_vector[j].length()){
					b = joint_vector[j].length();
					Active_masterframe[i] = j + MasterFramePartition[i + 1];
				}
			}
		}
		else{
			b = 100000.0;
			//蹴り上げのみ
			joint_vector2.resize(MasterFramePartition[i + 2] - MasterFramePartition[i + 1]);
			CreateVector(joi_pos_array_master, joint_vector2, a[i * 2 + 2], a[i * 2 + 3], MasterFramePartition[i + 1]);
			for (int j = 0; j < joint_vector.size(); j++){
				joint_vector[j].normalize();
				joint_vector2[j].normalize();
				if (b > joint_vector[j].angle(joint_vector2[j])){
					b = joint_vector[j].angle(joint_vector2[j]);
					Active_masterframe[i] = j + MasterFramePartition[i + 1];
				}
			}
		}
		cnt = 0;
		for (int j = 0; j < 20; j++){
		 if (j % 4 != 3){
			 if (j == 13 || j == 17){
				 r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
				 r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
				 r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
				 r1_dash->x = joi_pos_array[framePoint[i + 2] * 20 + j -1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
				 r1_dash->y = joi_pos_array[framePoint[i + 2] * 20 + j -1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
				 r1_dash->z = joi_pos_array[framePoint[i + 2] * 20 + j -1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
				 r2->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
				 r2->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
				 r2->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
				 r2_dash->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
				 r2_dash->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
				 r2_dash->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j - 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
				 angle_r1 = CosineSimilarity(*r1, *r1_dash);
				 angle_r2 = CosineSimilarity(*r2, *r2_dash);
				 if (angle_r1 > angle_r2){
					 cos_Stationary[i * 15 + cnt] = angle_r1-angle_r2;
				 }
				 else{
					 cos_Stationary[i * 15 + cnt] = angle_r2 - angle_r1;
				 }
				 cnt++;
				 angle_r1 = 0.0;
				 angle_r2 = 0.0;
			 }
			 else{
				 r1->x = joi_pos_array[framePoint[i + 2] * 20 + j + 1].x - joi_pos_array[framePoint[i + 2] * 20 + j].x;
				 r1->y = joi_pos_array[framePoint[i + 2] * 20 + j + 1].y - joi_pos_array[framePoint[i + 2] * 20 + j].y;
				 r1->z = joi_pos_array[framePoint[i + 2] * 20 + j + 1].z - joi_pos_array[framePoint[i + 2] * 20 + j].z;
				 r2->x = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].x - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].x;
				 r2->y = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].y - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].y;
				 r2->z = joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j + 1].z - joi_pos_array_master[MasterFramePartition[i + 2] * 20 + j].z;
				 cos_Stationary[i * 15 + cnt] = CosineSimilarity(*r1, *r2);
				 cnt++;
			 }
			}
		}
		cnt = 0;

		for (int j = 0; j < 20; j++){
			 if(j % 4 != 3){
				 if (j == 13 || j == 17){
					 r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					 r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					 r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					 r1_dash->x = joi_pos_array[(Active_frame[i]) * 20 + j - 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					 r1_dash->y = joi_pos_array[(Active_frame[i]) * 20 + j - 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					 r1_dash->z = joi_pos_array[(Active_frame[i]) * 20 + j - 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					 r2->x = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					 r2->y = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					 r2->z = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					 r2_dash->x = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					 r2_dash->y = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					 r2_dash->z = joi_pos_array_master[Active_masterframe[i] * 20 + j - 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					 angle_r1 = CosineSimilarity(*r1, *r1_dash);
					 angle_r2 = CosineSimilarity(*r2, *r2_dash);
					 if (angle_r1 > angle_r2){
						 cos_Active[i * 15 + cnt] = angle_r1 - angle_r2;
					 }
					 else{
						 cos_Active[i * 15 + cnt] = angle_r2 - angle_r1;
					 }
					 cnt++;
					 angle_r1 = 0.0;
					 angle_r2 = 0.0;
				 }
				 else{
					 r1->x = joi_pos_array[(Active_frame[i]) * 20 + j + 1].x - joi_pos_array[(Active_frame[i]) * 20 + j].x;
					 r1->y = joi_pos_array[(Active_frame[i]) * 20 + j + 1].y - joi_pos_array[(Active_frame[i]) * 20 + j].y;
					 r1->z = joi_pos_array[(Active_frame[i]) * 20 + j + 1].z - joi_pos_array[(Active_frame[i]) * 20 + j].z;
					 r2->x = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].x - joi_pos_array_master[Active_masterframe[i] * 20 + j].x;
					 r2->y = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].y - joi_pos_array_master[Active_masterframe[i] * 20 + j].y;
					 r2->z = joi_pos_array_master[Active_masterframe[i] * 20 + j + 1].z - joi_pos_array_master[Active_masterframe[i] * 20 + j].z;
					 cos_Active[i * 15 + cnt] = CosineSimilarity(*r1, *r2);
					 cnt++;
				 }
				}

		}
		

	}

	

}




float CosineSimilarity(Vector3f  vector1, Vector3f vector2){

	vector1.normalize();
	vector2.normalize();
	return acos(vector1.dot(vector2));//アークコサインいれる
}

void  Percentage(vector<float> & cos_s, float OriginalNumber, vector<float> & out_parcent){

	for (int i = 0; i < cos_s.size(); i++){
		out_parcent[i] = ((cos_s[i]+1.0) / OriginalNumber) * 100;
	}

}
void  PercentageAll(vector<float> & cos_s, float OriginalNumber, vector<float> & out_parcent){

	for (int i = 0; i <out_parcent.size(); i++){
		out_parcent[i] = (((cos_s[i*4] + 1.0) + (cos_s[i*4+1] + 1.0)+(cos_s[i*4+2] + 1.0)+(cos_s[i*4+3] + 1.0)) / (OriginalNumber*4)) * 100;
	}

}






///////////////////////////////////////////////////////////////////////////////
//
//  メイン関数
//



  //メイン関数（プログラムはここから開始）
int _tmain
(//seikou
int argc
, _TCHAR* argv[]
)
{
	// 全アプリケーションのリスト
	vector< class GLUTBaseApp * >    applications;

	// 全アプリケーションを登録
	//applications.push_back(new MotionPlaybackApp());
	//applications.push_back(new KeyframeMotionPlaybackApp());
	applications.push_back(new ForwardKinematicsApp());
	/*applications.push_back(new PostureInterpolationApp());
	applications.push_back(new MotionTransitionApp());
	applications.push_back(new MotionInterpolationApp());
	applications.push_back(new InverseKinematicsCCDApp());
	*/
	// 標準出力にユニコードを表示できるようにする
	setlocale(LC_ALL, "Japanese");

	//WNDCLASSEX tWndClass;
	HINSTANCE  hInstance;
	//TCHAR*     cpClassName;
	//TCHAR*     cpWindowName;
	//TCHAR*     cpMenu;
	//HWND       hWnd;
	//MSG        tMsg;


	// アプリケーションインスタンス
	hInstance = ::GetModuleHandle(NULL);

	static TCHAR szWindowClass[] = _T("Sample05");
	static TCHAR szTitle[] = _T("モーダルダイアログのサンプル");

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("RegisterClassExの処理に失敗しました"),
			_T("Sample03"),
			NULL);

		return 1;
	}

	hInst = hInstance; // グローバル変数に値を入れる

	// The parameters to CreateWindow explained:
	// szWindowClass                : アプリケーションの名前
	// szTitle                      : タイトルバーに現れる文字列
	// WS_OVERLAPPEDWINDOW          : 生成するウィンドウのタイプ
	// CW_USEDEFAULT, CW_USEDEFAULT : 最初に置くポジション (x, y)
	// WINDOW_WIDTH, WINDOW_HEIGHT  : 最初のサイズ (幅, 高さ)
	// NULL                         : このウィンドウの親ウィンドウのハンドル
	// NULL                         : メニューバー（このサンプルでは使用せず）
	// hInstance                    : WinMain関数の最初のパラメータ
	// NULL                         : WM_CREATE情報（このアプリケーションでは使用せず）
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	//  ウィンドウが生成できなかった場合
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("ウィンドウ生成に失敗しました!"),
			_T("Sample05"),
			NULL);
		return 1;
	}

	// ウィンドウの表示に必要なパラメータ:
	// hWnd     : CreateWindowの戻り値
	// nCmdShow : WinMainの引数の4番目
	::ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// メインのメッセージループ:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		SimpleHumanGLUTMain(applications, argc, argv);
	}
	return (int)msg.wParam;

}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND     hDialog;
	//ポイント構造体
	POINT    pt;
	switch (message)
	{
		//キーを押した
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			//終了メッセージを発生させる
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
		//ウインドウが生成されたときに1度だけ通過
	case WM_CREATE:
		break;
		//マウス右クリック
	case WM_RBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		//クライアント座標をスクリーン座標へ変換
		ClientToScreen(hWnd, &pt);
		//ポップアップメニューを表示
		TrackPopupMenu(GetSubMenu(GetMenu(hWnd), 0), TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_40001:
			//バージョン(A)

			MessageBox(hWnd, _T("メニューの実装Ver1.0"), _T("バージョン情報"), MB_OK);
			break;
		case ID_40002:
			//終了(X)
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case ID_40003:
			//モーダルダイアログボックスを作成
			hDialog = CreateDialog(
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd,
				(DLGPROC)DlgWndProc);
			//ダイアログボックスを表示
			ShowWindow(hDialog, SW_SHOW);
			break;
		case ID_40004:
			//モーダルダイアログボックスを作成
			hDialog = CreateDialog(
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_TESTDLG),
				hWnd,
				(DLGPROC)DlgWndProc);

			//ダイアログボックスを表示
			ShowWindow(hDialog, SW_SHOW);
			break;
			
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

//--------------------------------------------
// Name:DlgWndProc()
// Desc:ダイアログ用ウィンドウプロシージャ
//--------------------------------------------
BOOL CALLBACK DlgWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HICON hIcon;
	HDC hdc;
	PAINTSTRUCT ps;
	//IDC_BUTTON5->SetIcon(LoadIcon(IDI_ICON6));
	switch (iMsg)
	{
	case WM_INITDIALOG:
		//ダイアログボックスが生成されたとき
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			//モードレスダイアログボックスを破棄
			DestroyWindow(hWnd);
			break;
		case IDC_BUTTON5:
			MessageBox(hWnd, _T("タイトル"), _T("ボタン１が押されました。"), MB_OK);
			break;
		case IDI_ICON6:
			MessageBox(hWnd, _T("タイトル"), _T("ボタン１が押されました。"), MB_OK);
			break;
		/*case WM_CREATE:
			hIcon = LoadIcon(
				((LPCREATESTRUCT)(lParam))->hInstance,
				TEXT("IDI_ICON1")
				);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			DrawIcon(hdc, 10, 10, hIcon);
			EndPaint(hWnd, &ps);
			return 0;*/
		}
			break;
	
	
		return TRUE;
	default:
		return FALSE;
	}
}