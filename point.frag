#version 410 core

//
// point.frag
//
//   ���^�{�[���̕`��
//

// ���^�{�[���̒��S�ƃX���C�X�Ƃ̋����̃��^�{�[���̔��a�ɑ΂��銄��
in float d;

// �t���O�����g�̐F
layout (location = 0) out vec4 fc;

void main()
{
	// ���̃t���O�����g�̃��^�{�[���̒f�ʏ�̈ʒu
	vec3 q = vec3(gl_PointCoord * vec2(2.0, -2.0) - vec2(1.0, -1.0), d);

	// ���̃��^�{�[���̒��S����̋���
	float r = length(q);

	// ���^�{�[���ƃX���C�X���������Ă��Ȃ���΃t���O�����g���̂Ă�
	if (r > 1.0) discard;

	// ���z�ƔZ�x
	fc = vec4((1.0 - r) * 6.0 * q, smoothstep(1.0, 0.0, r));
}
