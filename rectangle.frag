#version 410 core

//
// 矩形の描画
//

// 法線ベクトルの変換行列
uniform mat4 mn;

// テクスチャ
uniform sampler2D image;

// 閾値
uniform float threshold;

// テクスチャ座標
in vec2 texcoord;

// フラグメントの色
layout (location = 0) out vec4 fc;

void main()
{
  // テクスチャをサンプリングする
	vec4 c = texture(image, texcoord);

	// ポテンシャルが閾値未満ならフラグメントを捨てる
	if (c.w < threshold) discard;

/*
	// 座標計算
  vec4 p = mv * pv;                                   // 視点座標系の頂点の位置
  vec3 v = normalize(p.xyz);                          // 視線ベクトル
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);  // 光線ベクトル
  vec3 n = normalize((mn * nv).xyz);                  // 法線ベクトル
  vec3 h = normalize(l - v);                          // 中間ベクトル

  // 陰影計算
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;
*/

	// 色
	fc = vec4(normalize(c.xyz) * 0.5 + 0.5, 1.0);
}
