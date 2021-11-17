

#include <fstream>
#include <string.h>
#include "BVH.h"
#include "quat.h"
#include "Calc.h"

//CCalcBVH *   calcbvh = NULL;
// コントラクタ
BVH::BVH()
{
	motion = NULL;
	sita_box = NULL;
	sita_box2 = NULL;
	sita_box3 = NULL;
	sita_box4 = NULL;
	sita_boxq = NULL;
	sita_boxq2 = NULL;
	sflame_box_spine = NULL;  
	sflame_box_leftarm = NULL;  
	sflame_box_rightarm = NULL; 
	sflame_box_leftleg = NULL;  
	sflame_box_rightleg = NULL; 
	s30flame_box_spine = NULL;
	s30flame_box_leftarm = NULL;
	s30flame_box_rightarm = NULL;
	s30flame_box_leftleg = NULL;
	s30flame_box_rightleg = NULL;
	sita_sum=NULL;
	sflame_box_sum = NULL;
	sum5jointflame_box = NULL;
	Clear();
}

// コントラクタ
BVH::BVH( const char * bvh_file_name )
{
	motion = NULL;
	sita_box = NULL;
	sita_box2 = NULL;
	sita_box3 = NULL;
	sita_box4 = NULL;
	sita_boxq = NULL;
	sita_boxq2 = NULL;
	sflame_box_spine = NULL;
	sflame_box_leftarm = NULL;
	sflame_box_rightarm = NULL;
	sflame_box_leftleg = NULL;
	sflame_box_rightleg = NULL;
	s30flame_box_spine = NULL;
	s30flame_box_leftarm = NULL;
	s30flame_box_rightarm = NULL;
	s30flame_box_leftleg = NULL;
	s30flame_box_rightleg = NULL;
	sita_sum = NULL;
	sflame_box_sum = NULL;
	sum5jointflame_box = NULL;
	
	Clear();

	Load( bvh_file_name );
	//Load_site();
}

// デストラクタ
BVH::~BVH()
{

	Clear();
}


// 全情報のクリア
void  BVH::Clear()
{
	int  i;
	for ( i=0; i<channels.size(); i++ )
		delete  channels[ i ];
	for ( i=0; i<joints.size(); i++ )
		delete  joints[ i ];
	for (i = 0; i<flame_vectors.size(); i++)
		delete flame_vectors[i];   
	for (i = 0; i<r_motions.size(); i++)
		delete r_motions[i];
	for (i = 0; i<rdash_motions.size(); i++)
		delete rdash_motions[i];
	for (i = 0; i<par_vectors.size(); i++)
		delete par_vectors[i];
	/*for (i = 0; i<pardash_vectors.size(); i++)
		delete pardash_vectors[i];
	for (i = 0; i<q_motions.size(); i++)
		delete q_motions[i];
	for (i = 0; i<rq_motions.size(); i++)
		delete rq_motions[i];
	for (i = 0; i< rqdash_motions.size(); i++)
		delete  rqdash_motions[i];
	for (i = 0; i< rq_s_motions.size(); i++)
		delete  rq_s_motions[i];
	for (i = 0; i< rqdash_s_motions.size(); i++)
		delete  rqdash_s_motions[i];
	for (i = 0; i< select_motions.size(); i++)
		delete  select_motions[i];
	for (i = 0; i< sum5joint_bools.size(); i++)
		delete  sum5joint_bools[i];
	for (i = 0; i< sum5jointflame_box10.size(); i++)
		delete  &sum5jointflame_box10[i];*/
	if ( motion != NULL )
		delete  motion;
	if (sita_box != NULL)
		delete  sita_box;
	if (sita_box2 != NULL)
		delete  sita_box2;
	if (sita_box3 != NULL)
		delete  sita_box3;
	if (sita_box4 != NULL)
		delete  sita_box4;
	if (sita_boxq != NULL)
		delete  sita_boxq;
	if (sita_boxq2 != NULL)
		delete  sita_boxq2;

	if (sflame_box_spine != NULL)
		delete  sflame_box_spine;
	if (sflame_box_leftarm != NULL)
		delete  sflame_box_leftarm;
	if (sflame_box_rightarm != NULL)
		delete  sflame_box_rightarm;
	if (sflame_box_leftleg != NULL)
		delete  sflame_box_leftleg;
	if (sflame_box_rightleg != NULL)
		delete  sflame_box_rightleg;

	if (s30flame_box_spine != NULL)
		delete  s30flame_box_spine;
	if (s30flame_box_leftarm != NULL)
		delete  s30flame_box_leftarm;
	if (s30flame_box_rightarm != NULL)
		delete  s30flame_box_rightarm;
	if (s30flame_box_leftleg != NULL)
		delete  s30flame_box_leftleg;
	if (s30flame_box_rightleg != NULL)
		delete  s30flame_box_rightleg;

	if (sita_sum != NULL)
		delete  sita_sum;
	if (sflame_box_sum != NULL)
		delete  sflame_box_sum;
	if (sum5jointflame_box != NULL)
		delete  sum5jointflame_box;
	
	is_load_success = false;
	
	file_name = "";
	motion_name = "";
	
	num_channel = 0;
	channels.clear();
	joints.clear();
	flame_vectors.clear();
	r_motions.clear();
	rdash_motions.clear();
	pardash_vectors.clear();
	joint_index.clear();
	par_vectors.clear();
	q_motions.clear();
	rq_motions.clear();
	rqdash_motions.clear();
	rq_s_motions.clear();
	rqdash_s_motions.clear();
	select_motions.clear();
	sum5joint_bools.clear();
	sum5jointflame_box10.clear();
	num_frame = 0;
	interval = 0.0;
	motion = NULL;
	sita_box = NULL;
	sita_box2 = NULL;
	sita_box3 = NULL;
	sita_box4 = NULL;
	sita_boxq = NULL;
	sita_boxq2 = NULL;
	sflame_box_spine = NULL;
	sflame_box_leftarm = NULL;
	sflame_box_rightarm = NULL;
	sflame_box_leftleg = NULL;
	sflame_box_rightleg = NULL;
	s30flame_box_spine = NULL;
	s30flame_box_leftarm = NULL;
	s30flame_box_rightarm = NULL;
	s30flame_box_leftleg = NULL;
	s30flame_box_rightleg = NULL;
	sita_sum = NULL;
	sflame_box_sum = NULL;
	sum5jointflame_box = NULL;
	num_spine=0;  //静止フレーム数
	num_leftarm=0;  //静止フレーム数
	num_rightarm=0;  //静止フレーム数
	num_leftleg=0;  //静止フレーム数
	num_rightleg=0;
	num_spine30 = 0;  //静止フレーム数
	num_leftarm30 = 0;  //静止フレーム数
	num_rightarm30 = 0;  //静止フレーム数
	num_leftleg30 = 0;  //静止フレーム数
	num_rightleg30 = 0;
	num_sum5jointflame = 0;
	num_sum5jointflame10 = 0;
}



//
//  BVHファイルのロード
//
void  BVH::Load_site(){
	int cnt = 0;
	enum  ChannelEnum    c1, c2, c3, tmp;
	end_site.resize(5);
	c1 = GetJoint(0)->channels[0]->type;
	c2 = GetJoint(0)->channels[1]->type;
	c3 = GetJoint(0)->channels[2]->type;
	for (int i = 0; i < joint_index.size(); i++){
		if (GetJoint(i)->has_site == true){
			if (c1 == 3){
				end_site[cnt].x = GetJoint(i)->site[0];
				if (c2 == 4){
					end_site[cnt].y = GetJoint(i)->site[1];
					end_site[cnt].z = GetJoint(i)->site[2];
				}
				else{
					end_site[cnt].y = GetJoint(i)->site[2];
					end_site[cnt].z = GetJoint(i)->site[1];
				}
			}
			if (c1 == 4){
				end_site[cnt].y = GetJoint(i)->site[0];
				if (c2 == 5){
					end_site[cnt].z = GetJoint(i)->site[1];
					end_site[cnt].x = GetJoint(i)->site[2];
				}
				else{
					end_site[cnt].z = GetJoint(i)->site[2];
					end_site[cnt].x = GetJoint(i)->site[1];
				}
			}
			if (c1 == 5){
				end_site[cnt].z = GetJoint(i)->site[0];
				if (c2 == 3){
					end_site[cnt].x = GetJoint(i)->site[1];
					end_site[cnt].y = GetJoint(i)->site[2];
				}
				else{
					end_site[cnt].x = GetJoint(i)->site[2];
					end_site[cnt].y = GetJoint(i)->site[1];
				}
			}

			cnt++;
		}
	}
}
void  BVH::Load( const char * bvh_file_name )
{
	// ファイル読み込みのバッファサイズ
	#define  BUFFER_LENGTH  1024*32

	ifstream  file;
	char      line[ BUFFER_LENGTH ];
	char *    token;
	char      separater[] = " :,\t";
	vector< Joint * >   joint_stack;
	Joint *   joint = NULL;
	Channel *  channel = NULL;//追加　木構造が格納されている（名前，オイラー角）
	Joint *   new_joint = NULL;//木構造が格納されている（名前，オフセット）
	PositionVector *  flame_vector = NULL;
	RotationMatrix *  r_motion = NULL;
	bool      is_site = false;
	bool	  is_p_flag = false;
	double    x, y ,z;
	double    px, py, pz;//フレーム分割で用いるroot関節のoffsetを格納する
	quat	  tmp_quat;
	int       i, j;

	// 初期化
	Clear();

	// ファイルの情報（ファイル名・動作名）の設定
	file_name = bvh_file_name;
	const char *  mn_first = bvh_file_name;
	const char *  mn_last = bvh_file_name + strlen( bvh_file_name );
	if ( strrchr( bvh_file_name, '\\' ) != NULL )
		mn_first = strrchr( bvh_file_name, '\\' ) + 1;
	else if ( strrchr( bvh_file_name, '/' ) != NULL )
		mn_first = strrchr( bvh_file_name, '/' ) + 1;
	if ( strrchr( bvh_file_name, '.' ) != NULL )
		mn_last = strrchr( bvh_file_name, '.' );
	if ( mn_last < mn_first )
		mn_last = bvh_file_name + strlen( bvh_file_name );
	motion_name.assign( mn_first, mn_last );

	// ファイルのオープン
	file.open( bvh_file_name, ios::in );
	if ( file.is_open() == 0 )  return; // ファイルが開けなかったら終了

	// 階層情報の読み込み
	while ( ! file.eof() )
	{
		// ファイルの最後まできてしまったら異常終了
		if ( file.eof() )  
			goto bvh_error;

		// １行読み込み、先頭の単語を取得
		file.getline( line, BUFFER_LENGTH );
		token = strtok( line, separater );

		// 空行の場合は次の行へ
		if ( token == NULL )  continue;

		// 関節ブロックの開始
		if ( strcmp( token, "{" ) == 0 )
		{
			// 現在の関節をスタックに積む
			joint_stack.push_back( joint );
			joint = new_joint;
			continue;
		}
		// 関節ブロックの終了
		if ( strcmp( token, "}" ) == 0 )
		{
			// 現在の関節をスタックから取り出す
			joint = joint_stack.back();
			joint_stack.pop_back();
			is_site = false;
			is_p_flag = true;
			continue;
		}

		// 関節情報の開始
		if ( ( strcmp( token, "ROOT" ) == 0 ) ||
		     ( strcmp( token, "JOINT" ) == 0 ) )
		{
			// 関節データの作成
			new_joint = new Joint();
			new_joint->index = joints.size();
			new_joint->parent = joint;
			new_joint->has_site = false;
			new_joint->offset[0] = 0.0;  new_joint->offset[1] = 0.0;  new_joint->offset[2] = 0.0;
			new_joint->site[0] = 0.0;  new_joint->site[1] = 0.0;  new_joint->site[2] = 0.0;
			new_joint->offset_vector.CreatePV(0.0, 0.0, 0.0);
			if (is_p_flag){
				new_joint->p_flag = true;//end_siteの後の関節をフレーム分割に用いる．追加
				is_p_flag = false;
			}
			else{
				new_joint->p_flag = false;//フレーム分割に用いないフラグを全てfalseにする．追加
			}
			joints.push_back( new_joint );//jointsに新たな要素を末尾に追加している．重要！
			if ( joint )
				joint->children.push_back( new_joint );

			// 関節名の読み込み
			token = strtok( NULL, "" );
			while ( *token == ' ' )  token ++;
			new_joint->name = token;

			// インデックスへ追加
			joint_index[ new_joint->name ] = new_joint;
			continue;
		}

		// 末端情報の開始
		if ( ( strcmp( token, "End" ) == 0 ) )
		{
			new_joint = joint;
			is_site = true;
			continue;
		}

		// 関節のオフセット or 末端位置の情報
		if ( strcmp( token, "OFFSET" ) == 0 )
		{
			// 座標値を読み込み
			token = strtok( NULL, separater );
			x = token ? atof( token ) : 0.0;
			token = strtok( NULL, separater );
			y = token ? atof( token ) : 0.0;
			token = strtok( NULL, separater );
			z = token ? atof( token ) : 0.0;
			
			// 関節のオフセットに座標値を設定
			if ( is_site )
			{
				joint->has_site = true;
				joint->site[0] = x;
				joint->site[1] = y;
				joint->site[2] = z;
			}
			else
			// 末端位置に座標値を設定
			{
				joint->offset[0] = x;
				joint->offset[1] = y;
				joint->offset[2] = z;
				if (joint->parent == NULL){
					joint->offset_vector.CreatePV(x, y, z);
				}
				else{
					float ox, oy, oz;//オフセット値から位置ベクトル生成
					ox = x - joint->parent->offset_vector.V.x;//親関節のオフセット値を用いて引き算を行う．
					oy = y - joint->parent->offset_vector.V.y;
					oz = z - joint->parent->offset_vector.V.z;
					joint->offset_vector.CreatePV(ox, oy, oz);
				}
			}
			if (joint->parent == NULL){
				px = x;
				py = y;
				pz = z;
			}
			else{
				if (joint->parent->offset[0] == px&&joint->parent->offset[1] == py&&joint->parent->offset[2] == pz){
					if (joint->offset[0] < joint->offset[1] && joint->offset[2] < joint->offset[1]){
						joint->p_flag = true;
					}
				}
			}
			continue;
		}

		// 関節のチャンネル情報
		if ( strcmp( token, "CHANNELS" ) == 0 )
		{
			// チャンネル数を読み込み
			token = strtok( NULL, separater );
			joint->channels.resize( token ? atoi( token ) : 0 );

			// チャンネル情報を読み込み
			for ( i=0; i<joint->channels.size(); i++ )
			{
				// チャンネルの作成
			    channel = new Channel();//変更前　Channel *channel = new Channel()
				channel->joint = joint;
				channel->index = channels.size();
				channels.push_back( channel );
				joint->channels[ i ] = channel;

				// チャンネルの種類の判定
				token = strtok( NULL, separater );
				if ( strcmp( token, "Xrotation" ) == 0 )
					channel->type = X_ROTATION;
				else if ( strcmp( token, "Yrotation" ) == 0 )
					channel->type = Y_ROTATION;
				else if ( strcmp( token, "Zrotation" ) == 0 )
					channel->type = Z_ROTATION;
				else if ( strcmp( token, "Xposition" ) == 0 )
					channel->type = X_POSITION;
				else if ( strcmp( token, "Yposition" ) == 0 )
					channel->type = Y_POSITION;
				else if ( strcmp( token, "Zposition" ) == 0 )
					channel->type = Z_POSITION;
			}
		}
		//フレーム分割に用いる関節のフラグをtrueにする

		// Motionデータのセクションへ移る
		if ( strcmp( token, "MOTION" ) == 0 )
			break;
	}


	// モーション情報の読み込み
	file.getline( line, BUFFER_LENGTH );
	token = strtok( line, separater );
	if ( strcmp( token, "Frames" ) != 0 )  
		goto bvh_error;
	token = strtok( NULL, separater );
	if ( token == NULL )  
		goto bvh_error;
	num_frame = atoi( token );

	file.getline( line, BUFFER_LENGTH );
	token = strtok( line, ":" );
	if ( strcmp( token, "Frame Time" ) != 0 )  
		goto bvh_error;
	token = strtok( NULL, separater );
	if ( token == NULL )  
		goto bvh_error;
	interval = atof( token );

	num_channel = channels.size();
	motion = new double[ num_frame * num_channel ];
	//q_motion = new quat[num_frame * num_channel];
	//r_motion = new RotationMatrix();
	//flame_vector = new PositionVector[num_frame];
	float p1x, p1y, p1z;
	int cnt = 0;
	int k = 0;
	// モーションデータの読み込み
	for ( i=0; i<num_frame; i++ )
	{
		file.getline( line, BUFFER_LENGTH );
		token = strtok( line, separater );
		k = 0;
		for ( j=0; j<num_channel; j++ )
		{
			if ( token == NULL )
				goto bvh_error;
			motion[ i*num_channel + j ] = atof( token );
			cnt++;
			if (cnt == 1){
				p1z = motion[i*num_channel + j];
			}
			else if (cnt == 2){
				p1x = motion[i*num_channel + j];
			}
			else{
				p1y = motion[i*num_channel + j];
			}
			if (cnt == 3){
				if (j>2){
					r_motion = new RotationMatrix();
					r_motion->rRotationMatrix(p1x, p1y, p1z);//回転行列生成
					r_motions.push_back(r_motion);
					//quat_assign(q_motion+i*(num_channel-3)/3+k, 0.0, p1x, p1y, p1z);//ここがミスってる．回転行列rを作らないといけない．
					k++;
				}
				else{
					flame_vector = new PositionVector();
					flame_vector->CreatePV(p1z, p1x, p1y);
					flame_vectors.push_back(flame_vector);
				}
				
				cnt = 0;
			}
			token = strtok( NULL, separater );
		}
	}

	// ファイルのクローズ
	file.close();

	// ロードの成功
	is_load_success = true;
	
	return;

bvh_error:
	file.close();
}

//
//  BVH骨格・姿勢の描画関数
//

#include <math.h>
#include <gl/glut.h>


// 指定フレームの姿勢を描画
void  BVH::RenderFigure( int frame_no, float scale )
{
	// BVH骨格・姿勢を指定して描画
	RenderFigure( joints[ 0 ], motion + frame_no * num_channel, scale );//motionはdouble型のアドレス，frame_noは現在のフレーム，num_channelは1フレームに入っているデータ数，チャンネル数（６６個）
}


// 指定されたBVH骨格・姿勢を描画（クラス関数）
void  BVH::RenderFigure( const Joint * joint, const double * data, float scale )
{
	glPushMatrix();

	// ルート関節の場合は平行移動を適用
	if ( joint->parent == NULL )
	{
		glTranslatef( data[ 0 ] * scale, data[ 1 ] * scale, data[ 2 ] * scale );
	}
	// 子関節の場合は親関節からの平行移動を適用
	else
	{
		glTranslatef( joint->offset[ 0 ] * scale, joint->offset[ 1 ] * scale, joint->offset[ 2 ] * scale );
	}

	// 親関節からの回転を適用（ルート関節の場合はワールド座標からの回転）
	int  i, j;
	for ( i=0; i<joint->channels.size(); i++ )
	{
		Channel *  channel = joint->channels[ i ];
		if ( channel->type == X_ROTATION )
			glRotatef( data[ channel->index ], 1.0f, 0.0f, 0.0f );
		else if ( channel->type == Y_ROTATION )
			glRotatef( data[ channel->index ], 0.0f, 1.0f, 0.0f );
		else if ( channel->type == Z_ROTATION )
			glRotatef( data[ channel->index ], 0.0f, 0.0f, 1.0f );
	}

	// リンクを描画
	// 関節座標系の原点から末端点へのリンクを描画
	if ( joint->children.size() == 0 )
	{
		RenderBone( 0.0f, 0.0f, 0.0f, joint->site[ 0 ] * scale, joint->site[ 1 ] * scale, joint->site[ 2 ] * scale );
	}
	// 関節座標系の原点から次の関節への接続位置へのリンクを描画
	if ( joint->children.size() == 1 )
	{
		Joint *  child = joint->children[ 0 ];
		RenderBone( 0.0f, 0.0f, 0.0f, child->offset[ 0 ] * scale, child->offset[ 1 ] * scale, child->offset[ 2 ] * scale );
	}
	// 全関節への接続位置への中心点から各関節への接続位置へ円柱を描画
	if ( joint->children.size() > 1 )
	{
		// 原点と全関節への接続位置への中心点を計算
		float  center[ 3 ] = { 0.0f, 0.0f, 0.0f };
		for ( i=0; i<joint->children.size(); i++ )
		{
			Joint *  child = joint->children[ i ];
			center[ 0 ] += child->offset[ 0 ];
			center[ 1 ] += child->offset[ 1 ];
			center[ 2 ] += child->offset[ 2 ];
		}
		center[ 0 ] /= joint->children.size() + 1;
		center[ 1 ] /= joint->children.size() + 1;
		center[ 2 ] /= joint->children.size() + 1;

		// 原点から中心点へのリンクを描画
		RenderBone(	0.0f, 0.0f, 0.0f, center[ 0 ] * scale, center[ 1 ] * scale, center[ 2 ] * scale );

		// 中心点から次の関節への接続位置へのリンクを描画
		for ( i=0; i<joint->children.size(); i++ )
		{
			Joint *  child = joint->children[ i ];
			RenderBone(	center[ 0 ] * scale, center[ 1 ] * scale, center[ 2 ] * scale,
				child->offset[ 0 ] * scale, child->offset[ 1 ] * scale, child->offset[ 2 ] * scale );
		}
	}

	// 子関節に対して再帰呼び出し
	for ( i=0; i<joint->children.size(); i++ )
	{
		RenderFigure( joint->children[ i ], data, scale );
	}

	glPopMatrix();
}


// BVH骨格の１本のリンクを描画（クラス関数）
void  BVH::RenderBone( float x0, float y0, float z0, float x1, float y1, float z1 )
{
	// 与えられた２点を結ぶ円柱を描画

	// 円柱の２端点の情報を原点・向き・長さの情報に変換
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;
	GLdouble  bone_length = sqrt( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z );

	// 描画パラメタの設定
	static GLUquadricObj *  quad_obj = NULL;
	if ( quad_obj == NULL )
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle( quad_obj, GLU_FILL );
	gluQuadricNormals( quad_obj, GLU_SMOOTH );

	glPushMatrix();

	// 平行移動を設定
	glTranslated( x0, y0, z0 );

	// 以下、円柱の回転を表す行列を計算

	// ｚ軸を単位ベクトルに正規化
	double  length;
	length = sqrt( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z );
	if ( length < 0.0001 ) { 
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;

	// 基準とするｙ軸の向きを設定
	GLdouble  up_x, up_y, up_z;
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;

	// ｚ軸とｙ軸の外積からｘ軸の向きを計算
	double  side_x, side_y, side_z;
	side_x = up_y * dir_z - up_z * dir_y;
	side_y = up_z * dir_x - up_x * dir_z;
	side_z = up_x * dir_y - up_y * dir_x;

	// ｘ軸を単位ベクトルに正規化
	length = sqrt( side_x*side_x + side_y*side_y + side_z*side_z );
	if ( length < 0.0001 ) {
		side_x = 1.0; side_y = 0.0; side_z = 0.0;  length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;

	// ｚ軸とｘ軸の外積からｙ軸の向きを計算
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;

	// 回転行列を設定
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
	                    up_x,   up_y,   up_z,   0.0,
	                    dir_x,  dir_y,  dir_z,  0.0,
	                    0.0,    0.0,    0.0,    1.0 };
	glMultMatrixd( m );

	// 円柱の設定
	GLdouble radius= 0.01; // 円柱の太さ
	GLdouble slices = 8.0; // 円柱の放射状の細分数（デフォルト12）
	GLdouble stack = 3.0;  // 円柱の輪切りの細分数（デフォルト１）

	// 円柱を描画
	gluCylinder( quad_obj, radius, radius, bone_length, slices, stack ); 

	glPopMatrix();
}

//フレーム分割
void BVH::FramePartition(vector <int>& framecom){
	//Joint *  p_joints=NULL;
	//double * p_motion = NULL;
	//PartitionJointDetection(joints[0]);//フレーム分割を行うために用いる関節情報を算出する
	RotationMatrix * a;
	RotationMatrix *par_vector = NULL;
	RotationMatrix *pardash_vector = NULL;
	quat* q_motion = NULL;
	quat* rq_motion = NULL;
	quat* rqdash_motion = NULL;
	quat* rq_motion_before = NULL;
	quat* rq_motion_after = NULL;
	quat* rq_motion_axis = NULL;
	quat* rq_motion_rad = NULL;
	/////////////////////////////////////////////////
	quat* rq_s_motion = NULL;
	quat* rqdash_s_motion = NULL;
	quat* rq_motion_before_s = NULL;
	quat* rq_motion_axis_s = NULL;
	quat* rq_motion_rad_s = NULL;
	//quat* rq_motion_after_s = NULL;
	RotationMatrix * rdash_motion = NULL;
	quat* select_motion = NULL;
	Motionbool*  sum5joint_bool = NULL;
	float s, t, rq_motion_before_norm, rq_motion_after_norm, sita, rq_motion_before_s_norm,sita2,ts;
	int i = 0, j = 0, k = 0, l = 0, cnt = 0,cntq=0, m = 0,n=0;
	int b = ((num_channel - 3) / 3);
	float ssf = 0.0;
	int ss = 0;
	sita_box = new float[num_frame *( (num_channel-3)/3)];
	sita_boxq = new float[num_frame *((num_channel - 3) / 3)];
	sita_boxq2 = new float[num_frame *((num_channel - 3) / 3)];
	sita_box2 = new float[num_frame *((num_channel - 3) / 3)];
	for (i = 0; i < num_frame; i++)
	{

		for (j = 0; j < b; j++)
		{
			if (j != 0){
				par_vector = new RotationMatrix();
				par_vector->MatrixMultiplication(joints[j]->offset_vector, r_motions[i*(num_channel - 3) / 3 + (joints[j]->parent->index)]->r_Vector);//回転後のベクトル生成
				par_vectors.push_back(par_vector);
			}
		}
	}

	for (i = 0; i < b - 1; i++)
	{

		for (j = 0; j < num_frame; j++)
		{
				/*q_motion = new quat();
				quat_assign(q_motion, 0.0, par_vectors[i*((num_channel - 3) / 3 - 1) + j]->r_Vector.V.x, par_vectors[i*((num_channel - 3) / 3 - 1) + j]->r_Vector.V.y, par_vectors[i*((num_channel - 3) / 3 - 1) + j]->r_Vector.V.z);
				q_motions.push_back(q_motion);*/
			pardash_vector = new RotationMatrix();
			pardash_vector = par_vectors[j*((num_channel - 3) / 3 - 1) + i];
			pardash_vectors.push_back(pardash_vector);
		}
	}

	for (i = 0; i < b - 1; i++)
	{
		//rq_motion_before_s = new quat();//各関節の１フレーム目の回転後位置ベクトルを用いてクォータニオンを生成＝rq_motion_before_s
		//quat_assign(rq_motion_before_s, 0.0, pardash_vectors[i*(num_frame - 1)]->r_Vector.V.x, pardash_vectors[i*(num_frame - 1)]->r_Vector.V.y, pardash_vectors[i*(num_frame - 1)]->r_Vector.V.z);
		//rq_motion_before_s_norm = quat_norm(rq_motion_before_s);
		for (j = 0; j < num_frame-1; j++)
		{

			rq_motion_before = new quat();//iフレーム目の回転後位置ベクトルを用いてクォータニオンを生成＝rq_motion_before
			quat_assign(rq_motion_before, 0.0, pardash_vectors[j + i*num_frame]->r_Vector.V.x, pardash_vectors[j + i*num_frame]->r_Vector.V.y, pardash_vectors[j + i*num_frame]->r_Vector.V.z);
			rq_motion_before_norm = quat_norm(rq_motion_before);
			rq_motion_after = new quat();//i+1フレーム目の回転後位置ベクトルを用いてクォータニオンを生成＝rq_motion_after
			quat_assign(rq_motion_after, 0.0, pardash_vectors[j + i*num_frame + 1]->r_Vector.V.x, pardash_vectors[j + i*num_frame + 1]->r_Vector.V.y, pardash_vectors[j + i*num_frame + 1]->r_Vector.V.z);
			rq_motion_after_norm = quat_norm(rq_motion_after);
			rq_motion_axis = new quat();//rq_motion_beforeとrq_motion_afterを用いて外積計算し，回転軸を決定する．
			quat_mul(rq_motion_axis, rq_motion_before, rq_motion_after);
			rq_motion_axis->w = 0.0;//回転軸にw=回転量は必要ない
			s = quat_norm(rq_motion_axis);
			quat_div_real(rq_motion_axis, rq_motion_axis, s);//回転軸ベクトルをノルムで割ってあげることで単位ベクトルに変換する
			rq_motion_rad = new quat();
			quat_sub(rq_motion_rad, rq_motion_before, rq_motion_after);//rq_motion_beforeとrq_motion_afterを引き算し，ベクトルを生成する．
			t = quat_norm(rq_motion_rad) / (2.0 * rq_motion_before_norm * ROOT_2_INV);//rq_motion_beforeとrq_motion_afterの二つのベクトルが通る円の半径はrq_motion_beforeとrq_motion_afterのノルムと同値なのでその値で割り算を行い単位円状の回転量を求める
			sita = AngleOf2Vector(rq_motion_before, rq_motion_after);
			sita_box[k] = sita;
			if (t > 1.0) t = 1.0;
			sita_boxq[k] = cos(sita / 2);
			ssf = sita_boxq[k];
			ssf = ssf * 100;
			ssf = ssf + 0.5;
			ss = ssf;
			ssf = ss;
			sita_boxq2[k] = ssf / 100;
			k++;
			rq_motion = new quat();//フレーム間のクォータニオン生成
			rqdash_motion = new quat();
			quat_assign(rq_motion, cos(asin(t)), rq_motion_axis->x * t, rq_motion_axis->y * t, rq_motion_axis->z * t);
			quat_assign(rqdash_motion, cos(sita / 2), rq_motion_axis->x * sin(sita / 2), rq_motion_axis->y *  sin(sita / 2), rq_motion_axis->z *  sin(sita / 2));
			rqdash_motions.push_back(rqdash_motion);
			rq_motions.push_back(rq_motion);
			rdash_motion = new RotationMatrix();//確認用
			create_rotation_matrix(rdash_motion, rq_motion);
			rdash_motions.push_back(rdash_motion);
	
		}
	}

	
	
	for (i = 0; i < b - 1; i++){
		if (joints[i]->parent){
			if (joints[i]->p_flag){
				if (joints[i]->offset[0] == 0.0&&joints[i]->offset[2] == 0.0){
					for (j = 0; j < num_frame - 1; j++){
						select_motion = new quat();
						select_motion = rq_motions[j + (i - 1)*num_frame];
						select_motions.push_back(select_motion);

					}

				}
			}
			else if (joints[i]->parent->p_flag){
				if (joints[i]->parent->offset[0] != 0.0||joints[i]->parent->offset[2] != 0.0){
					for (j = 0; j < num_frame - 1; j++){
						select_motion = new quat();
						select_motion = rq_motions[j + (i - 1)*num_frame];
						select_motions.push_back(select_motion);

					}
				}
			}
		}
		
	}
	//////////////////////////////////5要素のクォータニオンw/////
	sita_box3 = new float[select_motions.size()];
	for (i = 0; i < b - 1; i++){
		if (joints[i]->parent){
			if (joints[i]->p_flag){
				if (joints[i]->offset[0] == 0.0&&joints[i]->offset[2] == 0.0){
					cntq++;
					for (j = 0; j < num_frame - 1; j++){
						sita_box3[m] = sita_boxq2[j + (i - 1)*(num_frame - 1)];
						m++;
					}

				}
			}
			else if (joints[i]->parent->p_flag){
				if (joints[i]->parent->offset[0] != 0.0 || joints[i]->parent->offset[2] != 0.0){
					cntq++;
					for (j = 0; j < num_frame - 1; j++){
						sita_box3[m] = sita_boxq2[j + (i - 1)*(num_frame - 1)];
						m++;
					}
				}
			}
		}
		
	}
	/////////////////////////////////////////5要素の回転角/////////////
	sita_box4 = new float[select_motions.size()];
	for (i = 0; i < b - 1; i++){
		if (joints[i]->parent){
			if (joints[i]->p_flag){
				if (joints[i]->offset[0] == 0.0&&joints[i]->offset[2] == 0.0){
					cnt++;
					for (j = 0; j < num_frame - 1; j++){
						sita_box4[n] = sita_box[j + (i - 1)*(num_frame - 1)];//変更
						n++;
					}

				}
			}
			else if (joints[i]->parent->p_flag){
				if (joints[i]->parent->offset[0] != 0.0 || joints[i]->parent->offset[2] != 0.0){
					cnt++;
					for (j = 0; j < num_frame - 1; j++){
						sita_box4[n] = sita_box[j + (i - 1)*(num_frame - 1)];//変更q2->なし
						n++;
					}
				}
			}
		}

	}
	//////////////////////////////////////////////////////////////////
	/*回転角の足し合わせ*/
	sita_sum = new float[num_frame - 1];
	for (i = 0; i < num_frame - 1; i++){
		sita_sum[i] = 0.0;
		for (j = 0; j < 5; j++){
			sita_sum[i] = sita_sum[i]+sita_box4[j*(num_frame - 1) + i];
		}

		sita_sum[i] = sita_sum[i] / 5.0;

	}

	int msm = 0;
	bool fps_flag = false;
	sflame_box_sum = new int[num_frame - 1];
		fps_flag = false;
		for (j = 0; j < num_frame - 2; j++){//全体のフレーム-1まで探索
			if (sita_sum[j] < 0.2){
				if (!fps_flag){
					if (sita_sum[j+1] < 0.2){
						fps_flag = true;
						sflame_box_sum[msm] = j;
						msm++;
					}
				}
			}
			else{
				if (fps_flag){
					sflame_box_sum[msm] = j - 1;
					msm++;
					fps_flag = false;
				}
			}
		}

	num_sum = msm;
	//////////////////////////////////////////////////////////////////
	/*フレーム分割候補*/

	int ms = 0, mla = 0, mra = 0, mll = 0, mrl = 0;
	bool fp_flag = false;
	sflame_box_spine = new int[num_frame - 1];
	sflame_box_leftarm = new int[num_frame - 1];
	sflame_box_rightarm = new int[num_frame - 1];
	sflame_box_leftleg = new int[num_frame - 1];
	sflame_box_rightleg = new int[num_frame - 1];
	for (i = 0; i < cntq; i++){
		fp_flag = false;
		for (j = 0; j < num_frame - 1; j++){//全体のフレーム-1まで探索
			
				
					
					if (i == 0){
						if (sita_box3[j + i*(num_frame - 1)] >= 0.99){//動いていないフレーム間を取得する範囲設定（１．００を動いていないとする）
							if (!fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] >= 1.00&&sita_box3[j + i*(num_frame - 1) + 2] >= 1.00){//３フレーム間動いてない場合動いてないとする
									fp_flag = true;
									sflame_box_spine[ms] = j;
									ms++;
								//}

							}
						}
						else{
							if (fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] < 1.00||sita_box3[j + i*(num_frame - 1) + 2] < 1.00){
									sflame_box_spine[ms] = j - 1;
									ms++;
									fp_flag = false;
								//}
							}
						}
					}
					else if (i == 1){
						if (sita_box3[j + i*(num_frame - 1)] >= 0.99){
							if (!fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] >= 0.99&&sita_box3[j + i*(num_frame - 1) + 2] >= 0.99){
									fp_flag = true;
									sflame_box_leftarm[mla] = j;
									mla++;
								//}
							}
						}
						else{
							if (fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] < 0.99||sita_box3[j + i*(num_frame - 1) + 2] < 0.99){
									sflame_box_leftarm[mla] = j - 1;
									mla++;
									fp_flag = false;
								//}
							}
						}
					}
					else if (i == 2){
						if (sita_box3[j + i*(num_frame - 1)] >= 0.99){
							if (!fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] >= 0.99&&sita_box3[j + i*(num_frame - 1) + 2] >= 0.99){
									fp_flag = true;
									sflame_box_rightarm[mra] = j;
									mra++;
								//}
							}
						}
						else{
							if (fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] < 0.99||sita_box3[j + i*(num_frame - 1) + 2] < 0.99){
									sflame_box_rightarm[mra] = j - 1;
									mra++;
									fp_flag = false;
								//}
							}
						}
					}
					else if (i == 3){
						if (sita_box3[j + i*(num_frame - 1)] >= 0.99){
							if (!fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] >= 0.99&&sita_box3[j + i*(num_frame - 1) + 2] >= 0.99){
									fp_flag = true;
									sflame_box_leftleg[mll] = j;
									mll++;
								//}
							}
						}
						else{
							if (fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] < 0.99||sita_box3[j + i*(num_frame - 1) + 2] < 0.99){
									sflame_box_leftleg[mll] = j - 1;
									mll++;
									fp_flag = false;
								//}
							}
						}
					}
					else{
						if (sita_box3[j + i*(num_frame - 1)] >= 0.99){
							if (!fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] >= 0.99&&sita_box3[j + i*(num_frame - 1) + 2] >= 0.99){
									fp_flag = true;
									sflame_box_rightleg[mrl] = j;
									mrl++;
								//}
							}
						}
						else{
							if (fp_flag){
								//if (sita_box3[j + i*(num_frame - 1) + 1] < 0.99||sita_box3[j + i*(num_frame - 1) + 2] < 0.99){
									sflame_box_rightleg[mrl] = j - 1;
									mrl++;
									fp_flag = false;
								//}
							}
						}
					}
			
			
			
		}
		if (i == 0){
			if (fp_flag){
				sflame_box_spine[ms] = j;
				ms++;
				fp_flag = false;
			}
		}
		else if (i == 1){
			if (fp_flag){
				sflame_box_leftarm[mla] = j;
				mla++;
				fp_flag = false;
			}
		}
		else if (i == 2){
			if (fp_flag){
				sflame_box_rightarm[mra] = j;
				mra++;
				fp_flag = false;
			}
		}
		else if (i == 3){
			if (fp_flag){
				sflame_box_leftleg[mll] = j;
				mll++;
				fp_flag = false;
			}
		}
		else{
			if (fp_flag){
				sflame_box_rightleg[mrl] = j;
				mrl++;
				fp_flag = false;
			}
		}
	}

	num_spine = ms;
	num_leftarm = mla;
	num_rightarm = mra;
	num_leftleg = mll;
	num_rightleg = mrl;
	//////////////////////////////////////////ノイズ除去////////////////////////////
	/*ms = 0, mla = 0, mra = 0, mll = 0, mrl = 0;
	s30flame_box_spine = new int[num_spine];
	s30flame_box_leftarm = new int[num_leftarm];
	s30flame_box_rightarm = new int[num_rightarm];
	s30flame_box_leftleg = new int[num_leftleg];
	s30flame_box_rightleg = new int[num_rightleg];
	for (i = 0; i < num_spine; i=i+2){
		if ((sflame_box_spine[i + 1] - sflame_box_spine[i])>10){
			s30flame_box_spine[ms] = sflame_box_spine[i];
			ms++;
			s30flame_box_spine[ms] = sflame_box_spine[i + 1];
			ms++;
		}
	}
	for (i = 0; i < num_leftarm; i = i + 2){
		if ((sflame_box_leftarm[i + 1] - sflame_box_leftarm[i])>10){
			s30flame_box_leftarm[mla] = sflame_box_leftarm[i];
			mla++;
			s30flame_box_leftarm[mla] = sflame_box_leftarm[i + 1];
			mla++;
		}
	}
	for (i = 0; i < num_rightarm; i = i + 2){
		if ((sflame_box_rightarm[i + 1] - sflame_box_rightarm[i])>10){
			s30flame_box_rightarm[mra] = sflame_box_rightarm[i];
			mra++;
			s30flame_box_rightarm[mra] = sflame_box_rightarm[i + 1];
			mra++;
		}
	}
	for (i = 0; i < num_leftleg; i = i + 2){
		if ((sflame_box_leftleg[i + 1] - sflame_box_leftleg[i])>10){
			s30flame_box_leftleg[mll] = sflame_box_leftleg[i];
			mll++;
			s30flame_box_leftleg[mll] = sflame_box_leftleg[i + 1];
			mll++;
		}
	}
	for (i = 0; i < num_rightleg; i = i + 2){
		if ((sflame_box_rightleg[i + 1] - sflame_box_rightleg[i])>10){
			s30flame_box_rightleg[mrl] = sflame_box_rightleg[i];
			mrl++;
			s30flame_box_rightleg[mrl] = sflame_box_rightleg[i + 1];
			mrl++;
		}
	}*/
	num_spine30 = ms;
	num_leftarm30 = mla;
	num_rightarm30 = mra;
	num_leftleg30 = mll;
	num_rightleg30 = mrl;
	//////////////////////////////////////////５つの情報を１つに集約////////////////////////////
	/*for (i = 0; i < num_frame-1; i++){
		sum5joint_bool = new Motionbool();
		sum5joint_bool->allfalse();
		sum5joint_bools.push_back(sum5joint_bool);
	}
	for (i = 0; i < num_spine30; i = i + 2){
		for (j = s30flame_box_spine[i]; j < s30flame_box_spine[i + 1];j++){
			sum5joint_bools[j]->spinetrue();
		}
	}
	for (i = 0; i < num_leftarm30; i = i + 2){
		for (j = s30flame_box_leftarm[i]; j < s30flame_box_leftarm[i + 1]; j++){
			sum5joint_bools[j]->leftarmtrue();
		}
	}
	for (i = 0; i < num_rightarm30; i = i + 2){
		for (j = s30flame_box_rightarm[i]; j < s30flame_box_rightarm[i + 1]; j++){
			sum5joint_bools[j]->rightarmtrue();
		}
	}
	for (i = 0; i < num_leftleg30; i = i + 2){
		for (j = s30flame_box_leftleg[i]; j < s30flame_box_leftleg[i + 1]; j++){
			sum5joint_bools[j]->leftlegtrue();
		}
	}
	for (i = 0; i < num_rightleg30; i = i + 2){
		for (j = s30flame_box_rightleg[i]; j < s30flame_box_rightleg[i + 1]; j++){
			sum5joint_bools[j]->rightlegtrue();
		}
	}*/
	for (i = 0; i < num_frame - 1; i++){
		sum5joint_bool = new Motionbool();
		sum5joint_bool->allfalse();
		sum5joint_bools.push_back(sum5joint_bool);
	}
	for (i = 0; i < num_spine30; i = i + 2){
		for (j = sflame_box_spine[i]; j < sflame_box_spine[i + 1]; j++){
			sum5joint_bools[j]->spinetrue();
		}
	}
	for (i = 0; i < num_leftarm30; i = i + 2){
		for (j = sflame_box_leftarm[i]; j < sflame_box_leftarm[i + 1]; j++){
			sum5joint_bools[j]->leftarmtrue();
		}
	}
	for (i = 0; i < num_rightarm30; i = i + 2){
		for (j = sflame_box_rightarm[i]; j < sflame_box_rightarm[i + 1]; j++){
			sum5joint_bools[j]->rightarmtrue();
		}
	}
	for (i = 0; i < num_leftleg30; i = i + 2){
		for (j = sflame_box_leftleg[i]; j < sflame_box_leftleg[i + 1]; j++){
			sum5joint_bools[j]->leftlegtrue();
		}
	}
	for (i = 0; i < num_rightleg30; i = i + 2){
		for (j = sflame_box_rightleg[i]; j < sflame_box_rightleg[i + 1]; j++){
			sum5joint_bools[j]->rightlegtrue();
		}
	}
	/////////////////////////////////////////すべてがtrueの場合を見つける///////////////////////////////
	sum5jointflame_box = new int[num_frame - 1];
	fp_flag = false;
	ms = 0;
	for (i = 0; i < num_frame - 1; i++){
		
		if (!fp_flag){
			if (sum5joint_bools[i]->check5joint_bool()){
				fp_flag = true;
				sum5jointflame_box[ms] = i;
				ms++;
			}

		}
		else{
			if (!sum5joint_bools[i]->check5joint_bool()){
				fp_flag = false;
				sum5jointflame_box[ms] = i;
				ms++;
			}
		}
	}
	if (fp_flag){
		sum5jointflame_box[ms] = i;
		ms++;
		fp_flag = false;
	}
	num_sum5jointflame = ms;
	framecom.resize(ms);
	for (int i = 0; i < num_sum5jointflame;i++){
		framecom[i] = sum5jointflame_box[i];
	}
	//a = sum5jointflame_box;
	/////////////////////////////////////ノイズ除去///////////////////////////
	//sum5jointflame_box10 = new int[num_sum5jointflame];
	/*ms = 0;
	for (i = 0; i < num_sum5jointflame; i = i + 2){
		if ((sum5jointflame_box[i + 1] - sum5jointflame_box[i])>10){
			sum5jointflame_box10.push_back(sum5jointflame_box[i]);
			ms++;
			sum5jointflame_box10.push_back(sum5jointflame_box[i+1]);
			ms++;
		}
	}
	num_sum5jointflame10 = ms;*/
	////////////////////////11個になるまで減らす///////////////////////////////
	//int frame_number = 0;
	//int frame_sub = 0;
	//while(sum5jointflame_box10.size() != 22){
	//	frame_number = 1;
	//	frame_sub = sum5jointflame_box10[2] - sum5jointflame_box10[1];
	//	for (i = 3; i < sum5jointflame_box10.size()-1; i = i + 2){

	//		if (frame_sub >= sum5jointflame_box10[i + 1] - sum5jointflame_box10[i]){
	//			frame_number = i;
	//			frame_sub = sum5jointflame_box10[i + 1] - sum5jointflame_box10[i];
	//		}
	//		
	//	}
	//	sum5jointflame_box10.erase(sum5jointflame_box10.begin()+frame_number);
	//	sum5jointflame_box10.erase(sum5jointflame_box10.begin() + frame_number);
	//}


}

void BVH::create_rotation_matrix(RotationMatrix* m, const quat *q){
	/*(xa,xb,xc)
	(ya,yb,yc)
	(za,zb,zc)*/
	m->r_Vector.VM.xa = 1.0 - 2.0 * (q->y * q->y + q->z * q->z); m->r_Vector.VM.xb = 2.0 * (q->x * q->y - q->z * q->w); m->r_Vector.VM.xc = 2.0 * (q->z * q->x + q->w * q->y);
	m->r_Vector.VM.ya = 2.0 * (q->x * q->y + q->z * q->w); m->r_Vector.VM.yb = 1.0 - 2.0 * (q->z * q->z + q->x * q->x); m->r_Vector.VM.yc = 2.0 * (q->y * q->z - q->w * q->x);
	m->r_Vector.VM.za = 2.0 * (q->z * q->x - q->w * q->y); m->r_Vector.VM.zb = 2.0 * (q->y * q->z + q->x * q->w); m->r_Vector.VM.zc = 1.0 - 2.0 * (q->y * q->y + q->x * q->x);


}
float BVH::AngleOf2Vector(quat* b,quat* a){
	float b_length = 0.0;
	b_length = get_vector_length(b);//ベクトルbとaの長さを計算する
	float a_length = 0.0;
	a_length = get_vector_length(a);
	float cos_sita = 0.0;
	cos_sita = dot_product(b, a) / (b_length*a_length);//内積とベクトル長さを使ってcosθを求める
	if (cos_sita > 1.0)cos_sita = 1.0;
	return acos(cos_sita);//cosθからθを求める
}

float BVH::get_vector_length(quat* v){

	return  pow((v->x * v->x) + (v->y * v->y) + (v->z * v->z), 0.5);
}
double BVH::dot_product(quat* vl, quat* vr){

	return vl->x * vr->x + vl->y * vr->y + vl->z * vr->z;
}
#include<iostream> //入出力ライブラリ
#include<fstream> 
using namespace std;
void BVH::out_of_file(){
	int i, j = 0;

	//fcalcbvh->rdash_motions[0]->r_Vector;

		ofstream ofs("quat.csv"); //ファイル出力ストリーム
	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
		for (i = 0; i < ((num_channel - 3) / 3) -1; i++)
		{

			for (j = 0; j < num_frame-1 ; j++)
			{
				ofs << sita_box[i*(num_frame-1) +j] ;
				ofs << ",";
			}
			ofs<< endl;
		}
		//ofs << "21,22,23,24" << endl;
		//ofs << "31,32,33,34" << endl;

		return ;
	


}
void BVH::out_of_file4(){
	int i, j = 0;

	//fcalcbvh->rdash_motions[0]->r_Vector;

	ofstream ofs("quat4.csv"); //ファイル出力ストリーム
	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
	for (i = 0; i < ((num_channel - 3) / 3) - 1; i++)
	{

		for (j = 0; j < num_frame - 1; j++)
		{
			ofs << sita_boxq[i*(num_frame - 1) + j];
			ofs << ",";
		}
		ofs << endl;
	}
	//ofs << "21,22,23,24" << endl;
	//ofs << "31,32,33,34" << endl;

	return;



}
void BVH::out_of_file5(){
	int i, j = 0;

	//fcalcbvh->rdash_motions[0]->r_Vector;

	ofstream ofs("quat5.csv"); //ファイル出力ストリーム
	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
	for (i = 0; i < ((num_channel - 3) / 3) - 1; i++)
	{

		for (j = 0; j < num_frame - 1; j++)
		{
			ofs << sita_boxq2[i*(num_frame - 1) + j];
			ofs << ",";
		}
		ofs << endl;
	}
	//ofs << "21,22,23,24" << endl;
	//ofs << "31,32,33,34" << endl;

	return;



}

void BVH::out_of_file_spine(){
	int i = 0;

	//fcalcbvh->rdash_motions[0]->r_Vector;

	ofstream ofs("quatspine.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_spine; i++)
	{
			ofs << sflame_box_spine[i];
			ofs << ",";
	}
	ofs << endl;
	return;
}
void BVH::out_of_file_leftarm(){
	int i = 0;
	ofstream ofs("quatleftarm.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_leftarm; i++)
	{
		ofs << sflame_box_leftarm[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void BVH::out_of_file_rightarm(){
	int i = 0;
	ofstream ofs("quatrightarm.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_rightarm; i++)
	{
		ofs << sflame_box_rightarm[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void BVH::out_of_file_leftleg(){
	int i = 0;
	ofstream ofs("quatleftleg.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_leftleg; i++)
	{
		ofs << sflame_box_leftleg[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}
void BVH::out_of_file_rightleg(){
	int i = 0;
	ofstream ofs("quatrightleg.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_rightleg; i++)
	{
		ofs << sflame_box_rightleg[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}

void BVH::out_of_file_sita_sum(){
	int i = 0;
	ofstream ofs("sita_sum.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_frame-1; i++)
	{
		ofs << sita_sum[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}

void BVH::out_of_file_sita_sumframe(){
	int i = 0;
	ofstream ofs("sita_sumframe.csv"); //ファイル出力ストリーム
	for (i = 0; i < num_sum; i++)
	{
		ofs << sflame_box_sum[i];
		ofs << ",";
	}
	ofs << endl;
	return;
}
//void BVH::out_of_file2(){
//	int i, j = 0;
//
//	//fcalcbvh->rdash_motions[0]->r_Vector;
//
//	ofstream ofs("quat2.csv"); //ファイル出力ストリーム
//	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
//	for (i = 0; i < ((num_channel - 3) / 3)-1; i++)
//	{
//
//		for (j = 0; j < num_frame; j++)
//		{
//			ofs << sita_box2[i*(num_frame) + j];
//			ofs << ",";
//		}
//		ofs << endl;
//	}
//	//ofs << "21,22,23,24" << endl;
//	//ofs << "31,32,33,34" << endl;
//
//	return;
//
//
//
//}
void BVH::out_of_file3(){
	int i, j = 0;

	//fcalcbvh->rdash_motions[0]->r_Vector;
	int iMax = 0;
	iMax = select_motions.size() /( num_frame-1);
	ofstream ofs("5_quatw.csv"); //ファイル出力ストリーム
	//	ofs << "11,12,13,14" << endl; //"<<"で流し込むだけ
	for (i = 0; i < iMax; i++)
	{

		for (j = 0; j < num_frame-1; j++)
		{
			ofs << sita_box3[i*(num_frame-1)+j];
			ofs << ",";
		}
		ofs << endl;
	}
	//ofs << "21,22,23,24" << endl;
	//ofs << "31,32,33,34" << endl;

	return;



}
void BVH::out_of_file_5joint30(){
	int i = 0;
	ofstream ofs("5quat30.csv"); //ファイル出力ストリーム
	ofs << "------------------------------------spine------------------------------------------";
	for (i = 0; i < num_spine30; i++)
	{
		ofs << s30flame_box_spine[i];
		ofs << ",";
	}
	ofs << "------------------------------------leftarm------------------------------------------";
	for (i = 0; i < num_leftarm30; i++)
	{
		ofs << s30flame_box_leftarm[i];
		ofs << ",";
	}
	ofs << "------------------------------------rightarm------------------------------------------";
	for (i = 0; i < num_rightarm30; i++)
	{
		ofs << s30flame_box_rightarm[i];
		ofs << ",";
	}
	ofs << "------------------------------------leftleg------------------------------------------";
	for (i = 0; i < num_leftleg30; i++)
	{
		ofs << s30flame_box_leftleg[i];
		ofs << ",";
	}
	ofs << "------------------------------------rightleg------------------------------------------";
	for (i = 0; i < num_rightleg30; i++)
	{
		ofs << s30flame_box_rightleg[i];
		ofs << ",";
	}
	ofs << "------------------------------------all------------------------------------------";
	for (i = 0; i < num_sum5jointflame; i++)
	{
		ofs << sum5jointflame_box[i];
		ofs << ",";
	}
	ofs << "------------------------------------all(noisecansel)------------------------------------------";
	for (i = 0; i < sum5jointflame_box10.size(); i++)
	{
		ofs << sum5jointflame_box10[i];
		ofs << ",";
	}

	ofs << endl;
	return;
}
// End of BVH.cpp
