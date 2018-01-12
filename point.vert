#version 410 core

//
// point.vert
//
//   �_��`���V�F�[�_
//

// �ϊ��s��
uniform mat4 mv;                                      // ���f���r���[�ϊ��s��
uniform mat4 mp;                                      // ���e�ϊ��s��

// �_�̑傫��
uniform float size;

// ���_����
layout (location = 0) in vec4 pv;                     // ���[�J�����W�n�̒��_�ʒu

void main(void)
{
  // ���W�v�Z
  vec4 p = mv * pv;                                   // ���_���W�n�̒��_�̈ʒu

	// �_�̑傫��
	gl_PointSize = size / p.w;

  // ���_�̃X�N���[�����W�l
  gl_Position = mp * p;
}
