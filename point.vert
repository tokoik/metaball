#version 410 core

//
// point.vert
//
//   点を描くシェーダ
//

// スライスの位置
uniform float zSlice;

// 球の半径
uniform float radius;

// フレームバッファのサイズ
uniform vec2 size;

// 変換行列
uniform mat4 mv;                                      // モデルビュー変換行列
uniform mat4 mp;                                      // 投影変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                     // ローカル座標系の頂点位置

// 球の中心とスライスとの距離の球の半径に対する割合
out float d;

void main()
{
  // 球の中心の視点座標系の位置を求める
  vec4 p = mv * pv;

	// 球の中心とスライスとの距離の球の半径に対する割合を求める
	d = (zSlice - p.z) / radius;

  // 球の中心ののスクリーン座標値を求める
  gl_Position = mp * p;

	// 球の断面の大きさ（球が必ずスライスと交差するなら 1.0 - d * d は負にならない）
	gl_PointSize = size.y * radius * sqrt(1.0 - d * d) * mp[1][1] / gl_Position.w;
}
