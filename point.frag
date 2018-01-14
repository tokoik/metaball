#version 410 core

//
// point.frag
//
//   �_��`���V�F�[�_
//

// ���̒��S�ƃX���C�X�Ƃ̋����̋��̔��a�ɑ΂��銄��
in float d;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main()
{
	// ���̃t���O�����g�̋��̒f�ʏ�̈ʒu
	vec3 p = vec3(gl_PointCoord * 2.0 - 1.0, d);

	// ���̋��̒��S����̋���
	float r = length(p);

	// ���ƃX���C�X���������Ă��Ȃ���΃t���O�����g���̂Ă�
	if (r > 1.0) discard;

	// �x�N�g���ƃ|�e���V����
	fc = vec4(p / r, smoothstep(1.0, 0.0, r));
}
