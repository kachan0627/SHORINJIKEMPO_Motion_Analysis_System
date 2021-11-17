/**
***  キャラクタアニメーションのための人体モデルの表現・基本処理 ライブラリ・サンプルプログラム
***  Copyright (c) 2015-, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http://opensource.org/licenses/mit-license.php
**/


#ifndef  _SIMPLE_HUMAN_H_
#define  _SIMPLE_HUMAN_H_


//
//  行列・ベクトルの表現には vecmath C++ライブラリ（http://objectclub.jp/download/vecmath1）を使用
//
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color4.h>

// STL（Standard Template Library）を使用
#include <vector>
#include <string>
using namespace  std;

// プロトタイプ宣言
struct  Segment;
struct  Joint;
struct  Skeleton;
struct  Posture;



//
//  人体モデルの体節を表す構造体
//
struct  Segment
{
	// 体節番号・名前
	int                  index;
	string               name;

	// 接続関節
	vector< Joint * >    joints;

	// 各関節の接続位置（体節のローカル座標系）
	vector< Point3f >    joint_positions;

	// 体節の末端位置
	bool                 has_site;
	Point3f              site_position;
};


//
//  人体モデルの関節を表す構造体
//
struct  Joint
{
	// 関節番号・名前
	int                  index;
	string               name;

	// 接続体節
	Segment *            segments[2];
};


//
//  人体モデルの骨格を表す構造体
//
struct  Skeleton
{
	// 関節数
	int                  num_segments;

	// 関節の配列 [関節番号]
	Segment **           segments;

	// 体節数
	int                  num_joints;

	// 体節の配列 [体節番号]
	Joint **             joints;


	// コンストラクタ・デストラクタ
	Skeleton();
	Skeleton(int s, int j);
	~Skeleton();
};


//
//  人体モデルの姿勢を表す構造体
//
struct  Posture
{
	// 骨格モデル
	Skeleton *           body;

	// ルートの位置
	Point3f              root_pos;

	// ルートの向き（回転行列表現）
	Matrix3f             root_ori;

	// 各関節の相対回転（回転行列表現）[関節番号]
	Matrix3f *           joint_rotations;


	// コンストラクタ・デストラクタ
	Posture();
	Posture(Skeleton * b);
	Posture(const Posture & p);
	Posture &operator=(const Posture & p);
	~Posture();

	// 初期化
	void  Init(Skeleton * b);
};


//
//  動作を表す構造体
//
struct  Motion
{
	// 骨格モデル
	Skeleton *           body;

	// フレーム数
	int                  num_frames;

	// フレーム間の時間間隔
	float                interval;

	// 全フレームの姿勢 [フレーム番号]
	Posture *            frames;

	// 動作名
	string               name;


	// コンストラクタ・デストラクタ
	Motion();
	Motion(Skeleton * b, int num);
	Motion(const Motion & m);
	Motion &operator=(const Motion & m);
	~Motion();

	// 初期化
	void  Init(Skeleton * b, int num);

	// 動作の長さを取得
	float  GetDuration() const { return  num_frames * interval; }

	// 姿勢を取得
	Posture *  GetFrame(int no) const;
	Posture *  GetFrameTime(float time) const;
	void  GetPosture(float time, Posture & p) const;
	void  GetPosture(int no, Posture & p) const;
};


//
//  キーフレーム動作を表す構造体
//
struct  KeyframeMotion
{
	// 骨格モデル
	Skeleton *           body;

	// キーフレーム数
	int                  num_keyframes;

	// 各キー時刻の配列 ［キーフレーム番号］ 
	float  *             key_times;

	// 各キー姿勢の配列 ［キーフレーム番号］ 
	Posture *            key_poses;


	// コンストラクタ・デストラクタ
	KeyframeMotion();
	KeyframeMotion(Skeleton * b, int num);
	KeyframeMotion(const KeyframeMotion & m);
	KeyframeMotion &operator=(const KeyframeMotion & m);
	~KeyframeMotion();

	// 初期化
	void  Init(Skeleton * b, int num);
	void  Init(Skeleton * b, int num, const float * times, const Posture * poses);

	// 動作の長さを取得
	float  GetDuration() const;

	// 姿勢を取得
	void  GetPosture(float time, Posture & p) const;
};


//
//  人体モデルの骨格・姿勢・動作の基本処理
//

// 姿勢の初期化
void  InitPosture(Posture & posture, Skeleton * body = NULL);

// BVH動作から骨格モデルを生成
Skeleton *  CoustructBVHSkeleton(class BVH * bvh);

// BVH動作から動作データ（＋骨格モデル）を生成
Motion *  CoustructBVHMotion(class BVH * bvh, Skeleton * bvh_body = NULL);

// BVH動作から姿勢を取得
void  GetBVHPosture(const class BVH * bvh, int frame_no, Posture & posture);

// 順運動学計算
void  ForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array);

// 順運動学計算
void  ForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array);

// 姿勢の描画（スティックフィギュアで描画）
void  DrawPosture(const Posture & posture);

// 姿勢の影の描画（スティックフィギュアで描画）
void  DrawPostureShadow(const Posture & posture, const Vector3f & light_dir, const Color4f & color);

// 姿勢補間（２つの姿勢を補間）
void  PostureInterpolation(const Posture & p0, const Posture & p1, float ratio, Posture & p);


/*
// ※ 以下の基本処理は、レポート課題で作成

// 変換行列の水平向き（方位角）成分を計算
float  ComputeOrientation( const Matrix3f & ori );

// ２つの姿勢の位置・向きを合わせるための変換行列を計算
//（next_frame の位置・向きを、prev_frame の位置向きに合わせるための変換行列 trans_mat を計算）
void  ComputeConnectionTransformation( const Matrix4f & prev_frame, const Matrix4f & next_frame, Matrix4f & trans_mat );

// 姿勢の位置・向きに変換行列を適用
void  TransformPosture( const Matrix4f & trans, Posture & posture );

// 末端関節から支点関節へのパス（関節の配列と各関節における末端関節の方向）を探索
void  FindJointPath( const Skeleton * body, int base_joint_no, int ee_joint_no, vector< int > & joint_path, vector< int > & joint_path_signs );

// Inverse Kinematics 計算（CCD法）
void  ApplyInverseKinematicsCCD( Posture & posture, int base_joint_no, int ee_joint_no, Point3f ee_joint_position );

*/


#endif // _SIMPLE_HUMAN_H_
