#version 410 core

//
// point.frag
//
//   �_��`���V�F�[�_
//

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  //vec2 c = gl_PointCoord * 2.0 - 1.0;	  // ���W�l�� [0, 1] �� [-1, 1] �ɕϊ�����
	//float d = 1.0 - length(c);					  // 1 - ���S����̋����̓��
	//if (d < 0.0) discard;                 // �~�̊O�̓t���O�����g���̂Ă�
	//float e = d * d * (3.0 - 2.0 * d);    // Hermite ���
  
  fc = vec4(0.1 * smoothstep(1.0, 0.0, length(gl_PointCoord * 2.0 - 1.0)));  // ���F��`������
}
