#version 410 core

//
// point.frag
//
//   メタボールの描画
//

// メタボールの中心とスライスとの距離のメタボールの半径に対する割合
in float d;

// フラグメントの色
layout (location = 0) out vec4 fc;

void main()
{
	// このフラグメントのメタボールの断面上の位置
	vec3 q = vec3(gl_PointCoord * vec2(2.0, -2.0) - vec2(1.0, -1.0), d);

	// そのメタボールの中心からの距離
	float r = length(q);

	// メタボールとスライスが交差していなければフラグメントを捨てる
	if (r > 1.0) discard;

	// 勾配と濃度
	fc = vec4((1.0 - r) * 6.0 * q, smoothstep(1.0, 0.0, r));
}
