#version 410 core

//
// ��`�̕`��
//

// �e�N�X�`�����W
out vec2 texcoord;

void main()
{
  texcoord = vec2(gl_VertexID & 1, gl_VertexID >> 1);
	gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);
}
