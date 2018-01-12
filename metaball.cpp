// �A�v���P�[�V�����{��
#include "GgApplication.h"

// ����
#include <random>

// �_
typedef std::array<GLfloat, 4> Point;

// �_�Q
typedef std::vector<Point> Points;

// �_�Q�̐���
static void generatePoints(Points &points, int count)
{
  // �n�[�h�E�F�A�����Ŏ�����
  std::random_device seed;

  // �����Z���k�c�C�X�^�[�@�ɂ�闐���̌n���ݒ肷��
  std::mt19937 rn(seed());

  // ��l�������z
  // [0, 2) �̒l�͈̔͂œ��m���Ɏ����𐶐�����
  std::uniform_real_distribution<GLfloat> uniform(0.0f, 2.0f);

  // ���K���z
  // ���� 0�A�W���΍� 0.25 �ŕ��z������
  std::normal_distribution<GLfloat> normal(0.0f, 0.5f);

  // ���_���S�ɒ��a�����ɐ��K���z����_�Q�𔭐�����
  while (--count >= 0)
  {
    // �ܓx����
    const GLfloat cp(uniform(rn) - 1.0f);
    const GLfloat sp(sqrt(1.0f - cp * cp));

    // �o�x����
    const GLfloat t(3.1415927f * uniform(rn));
    const GLfloat ct(cos(t)), st(sin(t));

    // ���S����̋���
    const GLfloat r(normal(rn));

    // �_�̈ʒu
    const Point p{ r * sp * ct, r * sp * st, r * cp, 1.0f };

    // �_��ǉ�����
    points.emplace_back(p);
  }
}

//
// �A�v���P�[�V�����̎��s
//
void GgApplication::run()
{
  // �E�B���h�E���쐬����
  Window window("metaball");

  // �w�i�F���w�肷��
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // �_�̃T�C�Y���w�肷��
  glPointSize(128.0f);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  // ���Z��L���ɂ���
  //glBlendFunc(GL_ONE, GL_ZERO); // �㏑���i�f�t�H���g�j
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // �ʏ�̃A���t�@�u�����f���O
  glBlendFunc(GL_ONE, GL_ONE); // ���Z
  //glBlendColor(0.01f, 0.01f, 0.01f, 0.0f);
  //glBlendFunc(GL_CONSTANT_COLOR, GL_ONE); // ���Z
  glBlendEquation(GL_FUNC_ADD);
  glEnable(GL_BLEND);

  // �_�Q�𐶐�����
  Points points;
  generatePoints(points, 100);

  // �_�Q�̒��_�z��I�u�W�F�N�g
  const GgPoints cloud(points.data(), points.size());

  // �_��`���V�F�[�_
  const GgPointShader shader("point.vert", "point.frag");

  // �_�̑傫��
  const GLint sizeLoc(glGetUniformLocation(shader.get(), "size"));
  
  // ���_�̈ʒu
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (window.shouldClose() == GL_FALSE)
  {
    // ���e�ϊ��s��
    const GgMatrix mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 9.0f));

    // �E�B���h�E����������
    glClear(GL_COLOR_BUFFER_BIT);

    // �V�F�[�_�̎w��
    shader.use();

    // �_�̑傫����ݒ肷��
    glUniform1f(sizeLoc, window.getHeight() * 0.1f);

    // �ϊ��s��̐ݒ�
    shader.loadMatrix(mp, mv * window.getLeftTrackball());

    // �`��
    cloud.draw();

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
