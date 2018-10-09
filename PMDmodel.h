#pragma once
#include <vector>

//頂点データ
struct PMDVerts
{
	float pos[3];				// x, y, z // 座標
	float normal_vec[3];		// nx, ny, nz // 法線ベクトル
	float uv[2];				// u, v // UV座標 // MMD は頂点 UV
	unsigned short boneIdx[2];	// ボーン番号 1、番号 2 // モデル変形(頂点移動)時に影響
	unsigned char  bone_weight;	// ボーン 1 に与える影響度 // min:0 max:100 // ボーン 2 への影響度は、(100 - bone_weight)
	unsigned char edge_flag;	// 0:通常、1:エッジ無効 // エッジ(輪郭)が有効の場合
};	//38バイト(パディングにより、40バイトの構造体になる)

//マテリアルテータ
struct PMDMaterial
{
	float diffuse_color[3];		// dr, dg, db
	float alpha;				// 減衰色の不透明度 
	float specularity;
	float specular_color[3];	// sr, sg, sb // 光沢色 
	float mirror_color[3];		// mr, mg, mb // 環境色(ambient) 
	unsigned char toon_index;	// toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 ・・・ 10.bmp:0x09 
	unsigned char edge_flag;	// 輪郭、影 
	unsigned int face_vert_count; // 面頂点数
	char texture_file_name[20];	//テクスチャファイル名
};	//70バイト(パディングにより、72バイト)

class PMDModel
{
private:
	unsigned int vert_data_size;	//1頂点のサイズ

	std::vector<PMDVerts> verticesData;	//頂点データ

	std::vector<unsigned short> indicesData;	//インデックスデータ

	std::vector<PMDMaterial> materialData;	//マテリアルデータ

public:
	PMDModel(const char* filePath);
	~PMDModel();

	//頂点データの取得
	std::vector<PMDVerts>& GetModelData();

	//インデックスの取得
	std::vector<unsigned short> & GetIndexData();

	std::vector<PMDMaterial>& Materials();
};
