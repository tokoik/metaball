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
  std::normal_distribution<GLfloat> normal(1.0f, 0.25f);

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

  // �_�Q�𐶐�����
  Points points;
  generatePoints(points, 100000);

  // �_�Q�̒��_�z��I�u�W�F�N�g
  const GgPoints cloud(points.data(), points.size());

  // �_��`���V�F�[�_
  const GgPointShader shader("point.vert", "point.frag");

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

    // �ϊ��s��̐ݒ�
    shader.loadMatrix(mp, mv * window.getLeftTrackball());

    // �`��
    cloud.draw();

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
