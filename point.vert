#version 410 core

//
// point.vert
//
//   �_��`���V�F�[�_
//

// �X���C�X�̈ʒu
uniform float zslice;

// ���̔��a
uniform float radius;

// �t���[���o�b�t�@�̃T�C�Y
uniform vec2 size;

// �ϊ��s��
uniform mat4 mv;                                      // ���f���r���[�ϊ��s��
uniform mat4 mp;                                      // ���e�ϊ��s��

// ���_����
layout (location = 0) in vec4 pv;                     // ���[�J�����W�n�̒��_�ʒu

// ���̒��S�ƃX���C�X�Ƃ̋����̋��̔��a�ɑ΂��銄��
out float d;

void main()
{
  // ���̒��S�̎��_���W�n�̈ʒu�����߂�
  vec4 p = mv * pv;

	// ���̒��S�ƃX���C�X�Ƃ̋����̋��̔��a�ɑ΂��銄�������߂�
	d = (zslice - p.z) / radius;

  // ���̒��S�̂̃X�N���[�����W�l�����߂�
  gl_Position = mp * p;

	// ���̒f�ʂ̑傫���i�����K���X���C�X�ƌ�������Ȃ� 1.0 - d * d �͕��ɂȂ�Ȃ��j
	gl_PointSize = size.y * radius * sqrt(1.0 - d * d) * mp[1][1] / gl_Position.w;
}
