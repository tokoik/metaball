#version 410 core
#extension GL_ARB_shading_language_420pack : enable

//
// ��`�̕`��
//

// �ގ�
uniform vec4 kamb = vec4(0.2, 0.5, 0.7, 1.0);
uniform vec4 kdiff = vec4(0.2, 0.5, 0.7, 0.0);
uniform vec4 kspec = vec4(0.3, 0.3, 0.3, 0.0);
float kshi = 50.0;

// ����
layout (std140, binding = 0) uniform Light
{
  vec4 lamb;
  vec4 ldiff;
  vec4 lspec;
  vec4 lpos;
};

// �@���x�N�g���̕ϊ��s��
uniform mat4 mn;

// �e�N�X�`��
uniform sampler2D image[2];

// 臒l
uniform float threshold;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���O�����g�̐F
layout (location = 0) out vec4 fc;

void main()
{
  // ���݂̃X���C�X�̃e�N�X�`�����T���v�����O����
	vec4 c = texture(image[0], texcoord);

	// �|�e���V������臒l�����Ȃ�t���O�����g���̂Ă�
  float w1 = c.w - threshold;
	if (w1 <= 0.0) discard;

  // �ЂƂO�̃X���C�X�̃e�N�X�`�����T���v�����O����
	vec4 d = texture(image[1], texcoord);

  // �ЂƂO�̃X���C�X�̃|�e���V�����̕����傫����΃t���O�����g���̂Ă�
  float w2 = c.w - d.w;
  if (w2 <= 0.0) discard;

  // �|�e���V������臒l�ƈ�v����ʒu
  float t = w1 / w2;

  // �A�e�t��
	vec3 v = -vec3(0.0, 0.0, 1.0);
	vec3 l = normalize(vec3(lpos));
	vec3 n = normalize(mat3(mn) * mix(vec3(c), vec3(d), t));
	vec3 h = normalize(l - v);
  vec4 idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

	// �F
	fc = idiff + ispec;
	//fc = vec4(n * 0.5 + 0.5, 1.0);
}
