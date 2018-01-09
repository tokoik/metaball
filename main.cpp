// アプリケーション本体
#include "GgApplication.h"

//
// メインプログラム
//
int main()
{
  try
  {
    // アプリケーション本体
    GgApplication app;

    // アプリケーションを実行する
    app.run();
  }
  catch (const std::runtime_error &e)
  {
    // エラーメッセージを表示する
    std::cerr << e.what() << "\n\n[Type enter key] ";

    // Enter キーの入力を待つ
    std::cin.get();

    // ブログラムを終了する
    exit(EXIT_FAILURE);
  }
}
