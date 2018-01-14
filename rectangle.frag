#version 410 core

//
// ��`�̕`��
//

// �@���x�N�g���̕ϊ��s��
uniform mat4 mn;

// �e�N�X�`��
uniform sampler2D image;

// 臒l
uniform float threshold;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���O�����g�̐F
layout (location = 0) out vec4 fc;

void main()
{
  // �e�N�X�`�����T���v�����O����
	vec4 c = texture(image, texcoord);

	// �|�e���V������臒l�����Ȃ�t���O�����g���̂Ă�
	if (c.w < threshold) discard;

/*
	// ���W�v�Z
  vec4 p = mv * pv;                                   // ���_���W�n�̒��_�̈ʒu
  vec3 v = normalize(p.xyz);                          // �����x�N�g��
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);  // �����x�N�g��
  vec3 n = normalize((mn * nv).xyz);                  // �@���x�N�g��
  vec3 h = normalize(l - v);                          // ���ԃx�N�g��

  // �A�e�v�Z
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;
*/

	// �F
	fc = vec4(normalize(c.xyz) * 0.5 + 0.5, 1.0);
}
