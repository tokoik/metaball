// �A�v���P�[�V�����{��
#include "GgApplication.h"

// ����
#include <random>

#define SORT 1
#define TIME 0

// �_
typedef std::array<GLfloat, 3> Point;

// �_�Q
typedef std::vector<Point> Points;

// ���̐�
constexpr int sphereCount(10000);

// ���̔��a
constexpr GLfloat sphereRadius(0.2f);

// ��p
constexpr GLfloat fovy(1.0f);

// �O���ʂƌ���ʂ̈ʒu
constexpr GLfloat zNear(3.0f), zFar(7.0f);

// FBO �̃T�C�Y
constexpr GLsizei fboWidth(128), fboHeight(128);

// �X���C�X�̐�
constexpr int slices(100);

// �_�Q�̐���
static void generatePoints(Points &points, int count)
{
  // �n�[�h�E�F�A�����Ŏ�����
  std::random_device seed;

  // �����Z���k�c�C�X�^�[�@�ɂ�闐���̌n���ݒ肷��
  //std::mt19937 rn(seed());
  std::mt19937 rn(12345);

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
    const Point p{ r * sp * ct, r * sp * st, r * cp };

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
  Window window("metaball", 512, 512);

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
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // �_�Q�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof (Point), points.data(), GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

#if SORT
  // �_�Q�̃C���f�b�N�X�̒��_�o�b�t�@�I�u�W�F�N�g
  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, points.size() * sizeof (GLuint), NULL, GL_STREAM_DRAW);
#endif

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

  // �f�v�X�o�b�t�@�͎g��Ȃ�
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // �w�i�F�̃A���t�@�l�� 0 �ɂ���
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // �t���[���o�b�t�@�I�u�W�F�N�g�̏����l
  constexpr GLfloat zero[] = { 0.0f, 0.0f, 0.f, 0.0f };

  // �r���[�ϊ��s��
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // ���e�ϊ��s��
  const GgMatrix mp(ggPerspective(fovy, 1.0f, zNear, zFar));

  // �o�P�b�g
  std::vector<GLuint> bucket[slices];

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (window.shouldClose() == GL_FALSE)
  {
#if TIME
    glfwSetTime(0.0);
#endif

    // ���f���r���[�ϊ��s��
    const GgMatrix mw(mv * window.getLeftTrackball());

#if SORT
    // �o�P�b�g�̃N���A
    for (int i = 0; i < slices; ++i) bucket[i].clear();

    // ���ׂĂ̋��̒��S�ɂ���
    for (size_t i = 0; i < points.size(); ++i)
    {
      // ���̒��S�̎��_���W�n�� z �l
      const GLfloat zw(mw.get(2) * points[i][0] + mw.get(6) * points[i][1]
        + mw.get(10) * points[i][2] + mw.get(14));

      // ���̑O�[�̎��_���W�n�� z �l
      const GLfloat zwf(zw + sphereRadius);

      // ���̑O�[�̃X�N���[�����W�n�ɂ����� z �l
      const GLfloat zsf((mp.get(10) * zwf + mp.get(14)) / (mp.get(11) * zwf));

      // ���̑O�[�̈ʒu�ɂ�����o�P�b�g�ԍ�
      int bf(static_cast<int>(ceil((zsf * 0.5f + 0.5f) * slices - 0.5f)));
      if (bf < 0) bf = 0;

      // ���̌�[�̎��_���W�n�� z �l
      const GLfloat zwr(zw - sphereRadius);

      // ���̌�[�̃X�N���[�����W�n�ɂ����� z �l
      const GLfloat zsr((mp.get(10) * zwr + mp.get(14)) / (mp.get(11) * zwr));

      // ���̌�[�̈ʒu�ɂ�����o�P�b�g�ԍ�
      int br(static_cast<int>(floor((zsr * 0.5f + 0.5f) * slices - 0.5f)));
      if (br >= slices) br = slices - 1;

      // �o�P�b�g�\�[�g
      for (int b = bf; b < br; ++b) bucket[b].emplace_back(static_cast<GLuint>(i));
    }
#endif

#if TIME
    const double time_sort(glfwGetTime());
#endif

    // �E�B���h�E����������
    glClear(GL_COLOR_BUFFER_BIT);

    // �e�X���C�X�ɂ���
    for (int i = slices; --i >= 0;)
    {
      // �X�N���[�����W�n�ɂ�����X���C�X�̈ʒu
      const GLfloat zclip(static_cast<GLfloat>(i * 2 + 1) / static_cast<GLfloat>(slices) - 1.0f);

      // ���_���W�n�̃X���C�X�̈ʒu
      const GLfloat zslice(mp.get(14) / (zclip * mp.get(11) - mp.get(10)));

      // FBO �ɕ`��
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glViewport(0, 0, fboWidth, fboHeight);

      // �t���[���o�b�t�@�ւ̉��Z��L���ɂ���
      glEnable(GL_BLEND);

      // ����`���V�F�[�_���w�肷��
      pointShader.use();

      // �t���[���o�b�t�@�̃T�C�Y��ݒ肷��
      glUniform2f(sizeLoc, static_cast<GLfloat>(fboWidth), static_cast<GLfloat>(fboHeight));

      // �ϊ��s���ݒ肷��
      pointShader.loadMatrix(mp, mw);

      // �X���C�X�̈ʒu��ݒ肷��
      glUniform1f(zsliceLoc, zslice);

      // ���̔��a��ݒ肷��
      glUniform1f(radiusLoc, sphereRadius);

      // �t���[���o�b�t�@����������
      glClearBufferfv(GL_COLOR, 0, zero);

      // �`��
      glBindVertexArray(vao);
#if SORT
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bucket[i].size() * sizeof (GLuint), bucket[i].data());
      glDrawElements(GL_POINTS, static_cast<GLsizei>(bucket[i].size()), GL_UNSIGNED_INT, NULL);
#else
      glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
#endif

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

#if TIME
    std::cerr << "sort:" << time_sort << ", draw:" << glfwGetTime() - time_sort << '\n';
#endif
  }
}
