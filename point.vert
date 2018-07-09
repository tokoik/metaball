#version 410 core

//
// point.vert
//
//   ���^�{�[���̕`��
//

// �X���C�X�̈ʒu
uniform float zSlice;

// ���^�{�[���̔��a
uniform float radius;

// �t���[���o�b�t�@�̃T�C�Y
uniform vec2 size;

// ���f���r���[�ϊ��s��
uniform mat4 mv;

// ���e�ϊ��s��
uniform mat4 mp;

// ���[�J�����W�n�̒��_�ʒu
layout (location = 0) in vec4 pv;

// ���^�{�[���̒��S�ƃX���C�X�Ƃ̋����̃��^�{�[���̔��a�ɑ΂��銄��
out float d;

void main()
{
  // ���^�{�[���̒��S�̎��_���W�n�̈ʒu�����߂�
  vec4 p = mv * pv;

  // ���^�{�[���̒��S�̂̃X�N���[�����W�l�����߂�
  gl_Position = mp * p;

	// ���^�{�[���̒��S�ƃX���C�X�Ƃ̋����̃��^�{�[���̔��a�ɑ΂��銄�������߂�
	d = (zSlice - p.z) / radius;

	// ���^�{�[���̒f�ʂ̑傫���i���^�{�[�����K���X���C�X�ƌ�������Ȃ� 1.0 - d * d �͕��ɂȂ�Ȃ��j
	gl_PointSize = -size.y * 2.0 * radius * sqrt(1.0 - d * d) * mp[1][1] / zSlice;
}
