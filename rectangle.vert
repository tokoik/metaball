#version 410 core

//
// ��`�̕`��
//

// �X�N���[�����W�n�ł̉��s���l
uniform float zClip = 0.0;

// �e�N�X�`�����W
out vec2 texcoord;

void main()
{
  texcoord = vec2(gl_VertexID & 1, gl_VertexID >> 1);
	gl_Position = vec4(texcoord * 2.0 - 1.0, zClip, 1.0);
}
