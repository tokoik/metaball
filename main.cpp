// �A�v���P�[�V�����{��
#include "GgApplication.h"

//
// ���C���v���O����
//
int main()
{
  try
  {
    // �A�v���P�[�V�����{��
    GgApplication app;

    // �A�v���P�[�V���������s����
    app.run();
  }
  catch (const std::runtime_error &e)
  {
    // �G���[���b�Z�[�W��\������
    std::cerr << e.what() << "\n\n[Type enter key] ";

    // Enter �L�[�̓��͂�҂�
    std::cin.get();

    // �u���O�������I������
    exit(EXIT_FAILURE);
  }
}
