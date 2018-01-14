#version 410 core

//
// point.frag
//
//   点を描くシェーダ
//

// 球の中心とスライスとの距離の球の半径に対する割合
in float d;

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main()
{
	// このフラグメントの球の断面上の位置
	vec3 p = vec3(gl_PointCoord * 2.0 - 1.0, d);

	// その球の中心からの距離
	float r = length(p);

	// 球とスライスが交差していなければフラグメントを捨てる
	if (r > 1.0) discard;

	// ベクトルとポテンシャル
	fc = vec4(p / r, smoothstep(1.0, 0.0, r));
}
