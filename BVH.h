/**
***  BVH Player
***  BVH動作ファイルの読み込み・描画クラス
***  Copyright (c) 2004-2017, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http://opensource.org/licenses/mit-license.php
**/


#ifndef  _BVH_H_
#define  _BVH_H_
#define ROOT_2_INV 0.70710678118654752440
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include "quat.h"
#include "Calc.h"
#include"RotationMatrix.h"
#include "Motionbool.h"
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color3.h>
using namespace  std;



//
//  BVH形式のモーションデータ
//
class  BVH
{
  public:
	/*  内部用構造体  */

	// チャンネルの種類
	enum  ChannelEnum
	{
		X_ROTATION, Y_ROTATION, Z_ROTATION,
		X_POSITION, Y_POSITION, Z_POSITION
	};
	struct  Joint;

	// チャンネル情報
	struct  Channel
	{
		// 対応関節
		Joint *              joint;
		
		// チャンネルの種類
		ChannelEnum          type;

		// チャンネル番号
		int                  index;
	};

	// 関節情報
	struct  Joint
	{
		// 関節名
		string               name;
		// 関節番号
		int                  index;

		// 関節階層（親関節）
		Joint *              parent;
		// 関節階層（子関節）
		vector< Joint * >    children;

		// 接続位置
		double               offset[3];

		// 末端位置情報を持つかどうかのフラグ
		bool                 has_site;
		// 末端位置
		double               site[3];

		// 回転軸
		vector< Channel * >  channels;
		//フレーム分割で用いる関節かどうかのフラグ
		bool				 p_flag;
		//各関節位置ベクトル 追加　
		PositionVector 	     offset_vector;		 
	};

	

  private://privateに戻す!!
	// ロードが成功したかどうかのフラグ
	bool                     is_load_success;

	/*  ファイルの情報  */
	string                   file_name;   // ファイル名
	string                   out_file_name;   // ファイル名
	string                   motion_name; // 動作名

	/*  階層構造の情報  */
	int                      num_channel; // チャンネル数
	vector< Channel * >      channels;    // チャンネル情報 [チャンネル番号]
	vector< Joint * >        joints;      // 関節情報 [パーツ番号]
	map< string, Joint * >   joint_index; // 関節名から関節情報へのインデックス

	/*  モーションデータの情報  */
	int                      num_frame;   // フレーム数
	double                   interval;    // フレーム間の時間間隔
	double *                 motion;      // [フレーム番号][チャンネル番号]
	float *					 sita_box;    //フレーム間角速度格納
	float *					 sita_box2;   //初期位置から角度格納
	float*					 sita_box3;	  //５要素のクォータニオンｗ
	float *					 sita_box4;	  //５要素の回転角を検出
	float *					 sita_boxq;   //クォータニオンｗ
	float *					 sita_boxq2;
	float *					 sita_sum;	  //ラジアン合計
	int *					 sflame_box_spine;  //静止フレーム保存
	int *					 sflame_box_leftarm;  //静止フレーム保存
	int *					 sflame_box_rightarm;  //静止フレーム保存
	int *					 sflame_box_leftleg;  //静止フレーム保存
	int *					 sflame_box_rightleg;  //静止フレーム保存
	int *					 s30flame_box_spine;//３０フレーム以上保存
	int *					 s30flame_box_leftarm;//３０フレーム以上保存
	int *					 s30flame_box_rightarm;//３０フレーム以上保存
	int *					 s30flame_box_leftleg;//３０フレーム以上保存
	int *					 s30flame_box_rightleg;//３０フレーム以上保存
	int *					 sflame_box_sum;
	int *					 sum5jointflame_box;//すべての関節で静止しているフレーム
	vector<int>				 sum5jointflame_box10;//すべての関節で静止しているフレーム
	int 					 num_spine;  //静止フレーム数
	int 					 num_leftarm;  //静止フレーム数
	int 					 num_rightarm;  //静止フレーム数
	int 					 num_leftleg;  //静止フレーム数
	int 					 num_rightleg;  //静止フレーム数
	int 					 num_spine30;  //静止フレーム数(30以上)
	int 					 num_leftarm30;  //静止フレーム数
	int 					 num_rightarm30;  //静止フレーム数
	int 					 num_leftleg30;  //静止フレーム数
	int 					 num_rightleg30;  //静止フレーム数
	int						 num_sum;
	int					     num_sum5jointflame;
	int						  num_sum5jointflame10;

	/*フレーム分割用関節*/
	vector<quat *>           q_motions;		  //フレーム間の回転クォータニオン
	vector<quat *>           rq_motions;   	  //フレーム間の回転クォータニオン(ノルム)
	vector<quat *>			 rqdash_motions;  //フレーム間の回転クォータニオン(ベクトル角)
	vector<quat *>           rq_s_motions;   	  //1フレーム目との回転クォータニオン(ノルム)
	vector<quat *>			 rqdash_s_motions;  //1フレーム目との回転クォータニオン(ベクトル角)
	vector<quat *>			 select_motions;
	//PositionVector *	     p_vector;		  //各関節位置ベクトル
	vector< PositionVector *>	     flame_vectors;   //フレームごとの移動ベクトル追加　※１フレーム1データ，総データ数は１,８８５となる．
	vector< RotationMatrix *>	     r_motions;		  //回転行列（異常値）追加※サンプルデータは１フレーム２１データ，１,８８５フレームの場合，総データ数３９,５８５となる．
	vector< RotationMatrix *>	     rdash_motions;		  //クォータニオンを用いて回転行列（異常値）追加※サンプルデータは１フレーム２１データ，１,８８５フレームの場合，総データ数３９,５８５となる．
	vector< RotationMatrix *>	     par_vectors;	  //回転行列変換後の位置ベクトル 追加 ※サンプルデータは１フレーム２0データ(rootはベクトルがないため），１,８８５フレームの場合，総データ数３７,７００となる．
	vector<RotationMatrix *>		 pardash_vectors; //par_vectorの行列を入れ替える20*1885->1885*20に変換

	vector<Motionbool*>				 sum5joint_bools;

  public:
	// コンストラクタ・デストラクタ
	BVH();
	BVH( const char * bvh_file_name );
	~BVH();

	// 全情報のクリア
	void  Clear();

	// BVHファイルのロード
	void  Load_site();
	void  Load( const char * bvh_file_name );
	void loadcalcbvh(int c_num_channel, Channel *  c_channel, Joint *   c_new_joint, int c_num_frame, double c_interval, double * c_motion);
  public:
	/*  データアクセス関数  */

	// ロードが成功したかどうかを取得
	bool  IsLoadSuccess() const { return is_load_success; }

	// ファイルの情報の取得
	const string &  GetFileName() const { return file_name; }
	const string &  GetMotionName() const { return motion_name; }

	// 階層構造の情報の取得
	const int       GetNumJoint() const { return  joints.size(); }
	const Joint *   GetJoint( int no ) const { return  joints[no]; }
	const int       GetNumChannel() const { return  channels.size(); }
	const Channel * GetChannel( int no ) const { return  channels[no]; }

	const Joint *   GetJoint( const string & j ) const  {
		map< string, Joint * >::const_iterator  i = joint_index.find( j );
		return  ( i != joint_index.end() ) ? (*i).second : NULL; }
	const Joint *   GetJoint( const char * j ) const  {
		map< string, Joint * >::const_iterator  i = joint_index.find( j );
		return  ( i != joint_index.end() ) ? (*i).second : NULL; }
	//フレーム分割に関する情報の取得
	
	// モーションデータの情報の取得
	int     GetNumFrame() const { return  num_frame; }
	double  GetInterval() const { return  interval; }
	double  GetMotion( int f, int c ) const { return  motion[ f*num_channel + c ]; }

	// モーションデータの情報の変更
	void  SetMotion( int f, int c, double v ) { motion[ f*num_channel + c ] = v; }
	
  public:
	/*  姿勢の描画関数  */
	
	// 指定フレームの姿勢を描画
	void  RenderFigure( int frame_no, float scale = 0.4f );

	// 指定されたBVH骨格・姿勢を描画（クラス関数）
	static void  RenderFigure( const Joint * root, const double * data, float scale = 0.4f );

	// BVH骨格の１本のリンクを描画（クラス関数）
	static void  RenderBone( float x0, float y0, float z0, float x1, float y1, float z1 );

 public:
	 /*フレーム分割*/
	 void FramePartition(vector <int>& a);
	// void PartitionJointDetection(const Joint *p_root);//void PartitionJointDetection(const Joint *p_root, Joint *   b_joint);//フレーム分割を行うために用いる関節情報を算出する
	 static void create_rotation_matrix(RotationMatrix *m,const quat *q);
	 float AngleOf2Vector(quat*, quat*);
	 float get_vector_length(quat* );
	 double dot_product(quat* vl, quat* vr);
	 void out_of_file();//フレーム間での回転角度
	 void out_of_file2();//1フレーム目に対する回転角度
	 void out_of_file3();//注目関節の回転角度
	 void out_of_file4();//クォータニオンｗ
	 void out_of_file5();//クォータニオンｗを小数点２点以下で四捨五入
	 
	 void out_of_file_spine();
	 void out_of_file_leftarm();
	 void out_of_file_rightarm();
	 void out_of_file_leftleg();
	 void out_of_file_rightleg();
	 void out_of_file_5joint30();
	 void out_of_file_sita_sum();
	 void out_of_file_sita_sumframe();
		
	 vector<Point3f> end_site;
};



#endif // _BVH_H_
