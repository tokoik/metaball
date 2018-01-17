#version 410 core

//
// ��`�̕`��
//

// ����
uniform vec4 lamb = vec4(0.1, 0.1, 0.1, 1.0);
uniform vec4 ldiff = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 lspec = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 lpos = vec4(3.0, 4.0, 5.0, 0.0);

// �ގ�
uniform vec4 kamb = vec4(0.2, 0.5, 0.7, 1.0);
uniform vec4 kdiff = vec4(0.2, 0.5, 0.7, 0.0);
uniform vec4 kspec = vec4(0.3, 0.3, 0.3, 0.0);
float kshi = 50.0;

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

  // �A�e�t��
	vec3 v = -vec3(0.0, 0.0, 1.0);
	vec3 l = normalize(lpos.xyz);
	vec3 n = normalize(mat3(mn) * c.xyz);
	vec3 h = normalize(l - v);
  vec4 idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

	// �F
	fc = idiff + ispec;
	//fc = vec4(n * 0.5 + 0.5, 1.0);
}
