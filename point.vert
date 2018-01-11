#version 410 core

//
// point.vert
//
//   点を描くシェーダ
//

// 変換行列
uniform mat4 mv;                                      // モデルビュー変換行列
uniform mat4 mp;                                      // 投影変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                     // ローカル座標系の頂点位置

void main(void)
{
  // 座標計算
  vec4 p = mv * pv;                                   // 視点座標系の頂点の位置

  // 頂点のスクリーン座標値
  gl_Position = mp * p;
}
