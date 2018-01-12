#version 410 core

//
// point.frag
//
//   点を描くシェーダ
//

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
  //vec2 c = gl_PointCoord * 2.0 - 1.0;	  // 座標値を [0, 1] → [-1, 1] に変換する
	//float d = 1.0 - length(c);					  // 1 - 中心からの距離の二乗
	//if (d < 0.0) discard;                 // 円の外はフラグメントを捨てる
	//float e = d * d * (3.0 - 2.0 * d);    // Hermite 補間
  
  fc = vec4(0.1 * smoothstep(1.0, 0.0, length(gl_PointCoord * 2.0 - 1.0)));  // 白色を描くだけ
}
