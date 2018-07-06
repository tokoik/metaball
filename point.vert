#version 410 core

//
// point.vert
//
//   メタボールの描画
//

// スライスの位置
uniform float zSlice;

// メタボールの半径
uniform float radius;

// フレームバッファのサイズ
uniform vec2 size;

// モデルビュー変換行列
uniform mat4 mv;

// 投影変換行列
uniform mat4 mp;

// ローカル座標系の頂点位置
layout (location = 0) in vec4 pv;

// メタボールの中心とスライスとの距離のメタボールの半径に対する割合
out float d;

void main()
{
  // メタボールの中心の視点座標系の位置を求める
  vec4 p = mv * pv;

  // メタボールの中心ののスクリーン座標値を求める
  gl_Position = mp * p;

	// メタボールの中心とスライスとの距離のメタボールの半径に対する割合を求める
	d = (zSlice - p.z) / radius;

	// メタボールの断面の大きさ（メタボールが必ずスライスと交差するなら 1.0 - d * d は負にならない）
	gl_PointSize = -size.y * 2.0 * radius * sqrt(1.0 - d * d) * mp[1][1] / zSlice;
}
