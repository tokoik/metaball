// アプリケーション本体
#include "GgApplication.h"

// 乱数
#include <random>

// 点
typedef std::array<GLfloat, 4> Point;

// 点群
typedef std::vector<Point> Points;

// 点群の生成
static void generatePoints(Points &points, int count)
{
  // ハードウェア乱数で種を作る
  std::random_device seed;

  // メルセンヌツイスター法による乱数の系列を設定する
  std::mt19937 rn(seed());

  // 一様実数分布
  // [0, 2) の値の範囲で等確率に実数を生成する
  std::uniform_real_distribution<GLfloat> uniform(0.0f, 2.0f);

  // 正規分布
  // 平均 0、標準偏差 0.25 で分布させる
  std::normal_distribution<GLfloat> normal(0.0f, 0.5f);

  // 原点中心に直径方向に正規分布する点群を発生する
  while (--count >= 0)
  {
    // 緯度方向
    const GLfloat cp(uniform(rn) - 1.0f);
    const GLfloat sp(sqrt(1.0f - cp * cp));

    // 経度方向
    const GLfloat t(3.1415927f * uniform(rn));
    const GLfloat ct(cos(t)), st(sin(t));

    // 中心からの距離
    const GLfloat r(normal(rn));

    // 点の位置
    const Point p{ r * sp * ct, r * sp * st, r * cp, 1.0f };

    // 点を追加する
    points.emplace_back(p);
  }
}

//
// アプリケーションの実行
//
void GgApplication::run()
{
  // ウィンドウを作成する
  Window window("metaball");

  // 背景色を指定する
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // 点のサイズを指定する
  glPointSize(128.0f);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  // 加算を有効にする
  //glBlendFunc(GL_ONE, GL_ZERO); // 上書き（デフォルト）
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 通常のアルファブレンデング
  glBlendFunc(GL_ONE, GL_ONE); // 加算
  //glBlendColor(0.01f, 0.01f, 0.01f, 0.0f);
  //glBlendFunc(GL_CONSTANT_COLOR, GL_ONE); // 加算
  glBlendEquation(GL_FUNC_ADD);
  glEnable(GL_BLEND);

  // 点群を生成する
  Points points;
  generatePoints(points, 100);

  // 点群の頂点配列オブジェクト
  const GgPoints cloud(points.data(), points.size());

  // 点を描くシェーダ
  const GgPointShader shader("point.vert", "point.frag");

  // 点の大きさ
  const GLint sizeLoc(glGetUniformLocation(shader.get(), "size"));
  
  // 視点の位置
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // ウィンドウが開いている間繰り返す
  while (window.shouldClose() == GL_FALSE)
  {
    // 投影変換行列
    const GgMatrix mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 9.0f));

    // ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // シェーダの指定
    shader.use();

    // 点の大きさを設定する
    glUniform1f(sizeLoc, window.getHeight() * 0.1f);

    // 変換行列の設定
    shader.loadMatrix(mp, mv * window.getLeftTrackball());

    // 描画
    cloud.draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
