// �A�v���P�[�V�����{��
#include "GgApplication.h"

// ����
#include <random>

// �_
typedef std::array<GLfloat, 4> Point;

// �_�Q
typedef std::vector<Point> Points;

// ���̐�
constexpr int sphereCount(1000);

// ���̔��a
constexpr GLfloat sphereRadius(0.3f);

// ��p
constexpr GLfloat fovy(1.0f);

// �O���ʂƌ���ʂ̈ʒu
constexpr GLfloat zNear(4.0f), zFar(6.0f);

// FBO �̃T�C�Y
constexpr GLsizei fboWidth(512), fboHeight(512);

// �X���C�X�̐�
constexpr int slices(100);

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
  std::normal_distribution<GLfloat> normal(0.0f, 1.0f);

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
  Window window("metaball", fboWidth, fboHeight);

  //
  // �t���[���o�b�t�@�I�u�W�F�N�g
  //

  // �J���[�o�b�t�@�Ɏg���e�N�X�`��
  GLuint cb;
  glGenTextures(1, &cb);
  glBindTexture(GL_TEXTURE_2D, cb);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboWidth, fboHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // �t���[���o�b�t�@�I�u�W�F�N�g
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cb, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // ��`��`���V�F�[�_
  const GLuint rectangleShader(ggLoadShader("rectangle.vert", "rectangle.frag"));

  // �e�N�X�`��
  const GLint imageLoc(glGetUniformLocation(rectangleShader, "image"));

  // 臒l
  const GLint thresholdLoc(glGetUniformLocation(rectangleShader, "threshold"));

  //
  // �_�Q
  //

  // �_�Q�𐶐�����
  Points points;
  generatePoints(points, sphereCount);

  // �_�Q�̒��_�z��I�u�W�F�N�g
  const GgPoints cloud(points.data(), points.size());

  // �_��`���V�F�[�_
  const GgPointShader pointShader("point.vert", "point.frag");

  // �X���C�X�̈ʒu
  const GLint zsliceLoc(glGetUniformLocation(pointShader.get(), "zslice"));

  // ���̔��a
  const GLint radiusLoc(glGetUniformLocation(pointShader.get(), "radius"));

  // �t���[���o�b�t�@�̃T�C�Y
  const GLint sizeLoc(glGetUniformLocation(pointShader.get(), "size"));

  // �_�̃T�C�Y�̓V�F�[�_����ύX����
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  //
  // OpenGL �̐ݒ�
  //

  /*
  ** �����_�����O���ʂ̃u�����h
  **
  **   glBlendFunc(GL_ONE, GL_ZERO);                       // �㏑���i�f�t�H���g�j
  **   glBlendFunc(GL_ZERO, GL_ONE);                       // �`���Ȃ�
  **   glBlendFunc(GL_ONE, GL_ONE);                        // ���Z
  **   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // �ʏ�̃A���t�@�u�����f���O
  **   glBlendColor(0.01f, 0.01f, 0.01f, 0.0f);            // ���Z����萔
  **   glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);             // �萔�����Z
  */

  // �t���[���o�b�t�@�ɉ��Z����
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);

  // �f�v�X�o�b�t�@�͎g�����ǃf�v�X�e�X�g�͂��Ȃ�
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // �w�i�F���w�肷��
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // �t���[���o�b�t�@�I�u�W�F�N�g�̏����l
  constexpr GLfloat zero[] = { 0.0f, 0.0f, 0.f, 0.0f };

  // �r���[�ϊ��s��
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (window.shouldClose() == GL_FALSE)
  {
    // ���f���r���[�ϊ��s��
    const GgMatrix mw(mv * window.getLeftTrackball());

    // �E�B���h�E����������
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = slices; --i >= 0;)
    {
      // FBO �ɕ`��
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glViewport(0, 0, fboWidth, fboHeight);

      // �t���[���o�b�t�@�ւ̉��Z��L���ɂ���
      glEnable(GL_BLEND);

      // ����`���V�F�[�_���w�肷��
      pointShader.use();

      // �t���[���o�b�t�@�̃T�C�Y��ݒ肷��
      glUniform2f(sizeLoc, static_cast<GLfloat>(window.getWidth()), static_cast<GLfloat>(window.getHeight()));

      // ���e�ϊ��s��
      const GgMatrix mp(ggPerspective(fovy, 1.0f, zNear, zFar));

      // �ϊ��s���ݒ肷��
      pointShader.loadMatrix(mp, mw);

      // �X���C�X�̈ʒu
      const GLfloat zclip(static_cast<GLfloat>(i * 2 + 1) / static_cast<GLfloat>(slices) - 1.0f);
      const GLfloat zslice(mp.get(14) / (zclip * mp.get(11) - mp.get(10)));

      // �X���C�X�̈ʒu��ݒ肷��
      glUniform1f(zsliceLoc, zslice);

      // ���̔��a��ݒ肷��
      glUniform1f(radiusLoc, sphereRadius);

      // �t���[���o�b�t�@����������
      glClearBufferfv(GL_COLOR, 0, zero);

      // �`��
      cloud.draw();

      // �ʏ�̃t���[���o�b�t�@�ɕ`��
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, window.getWidth(), window.getHeight());

      // �t���[���o�b�t�@�ւ̉��Z�𖳌��ɂ���
      glDisable(GL_BLEND);

      // ��`��`���V�F�[�_��I������
      glUseProgram(rectangleShader);

      // �e�N�X�`�������蓖�Ă�
      glBindTexture(GL_TEXTURE_2D, cb);
      glActiveTexture(GL_TEXTURE0);
      glUniform1i(imageLoc, 0);

      // 臒l��ݒ肷��
      glUniform1f(thresholdLoc, window.getWheel() * 0.1f + 1.0f);

      // ��`��`�悷��
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
