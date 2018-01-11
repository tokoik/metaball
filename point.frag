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
  // 画素の色
  fc = vec4(1.0, 1.0, 1.0, 0.0);
}
