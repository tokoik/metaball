// アプリケーション本体
#include "GgApplication.h"

// 乱数
#include <random>

// メタボールをバケットソートする場合は 1
#define SORT 1

// デプスバッファを使う場合は 1
#define DEPTH 1

// 時間を計測する場合は 1
#define TIME 1

// glFinish() を実行する場合は 1
#define USE_GLFINISH 0

//
// スライス
//

// スライスの数
constexpr int sliceCount(128);

// スライス用の FBO のサイズ
constexpr GLsizei sliceWidth(128), sliceHeight(128);

//
// メタボール
//

// メタボールの数
constexpr int sphereCount(100000);

// メタボールの位置の平均
constexpr GLfloat sphereMean(0.0f);

// メタボールの位置の標準偏差
constexpr GLfloat sphereDeviation(0.5f);

//
// 光源
//

// 光源の材質と位置
constexpr GgSimpleShader::Light lightData =
{
  { 0.1f, 0.1f, 0.1f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 3.0f, 4.0f, 5.0f, 0.0f }
};

//
// カメラ
//

// カメラの位置
constexpr GLfloat cameraPosition[] = { 0.0f, 0.0f, 5.0f };

// 目標点の位置
constexpr GLfloat cameraTarget[] = { 0.0f, 0.0f, 0.0f };

// カメラの上方向のベクトル
constexpr GLfloat cameraUp[] = { 0.0f, 1.0f, 0.0f };

// 画角
constexpr GLfloat cameraFovy(1.0f);

// 前方面と後方面の位置
constexpr GLfloat cameraNear(3.0f), cameraFar(7.0f);

// 背景色
constexpr GLfloat background[] = { 1.0f, 1.0f, 0.9f };

//
// 点群
//

// 点
typedef std::array<GLfloat, 3> Point;

// 点群
typedef std::vector<Point> Points;

// 点群の生成
static void generatePoints(Points &points, int count, GLfloat mean, GLfloat deviation)
{
  // ハードウェア乱数で種を作る
  std::random_device seed;

  // メルセンヌツイスター法による乱数の系列を設定する
  //std::mt19937 rn(seed());
  std::mt19937 rn(12345);

  // 一様実数分布
  // [0, 2) の値の範囲で等確率に実数を生成する
  std::uniform_real_distribution<GLfloat> uniform(0.0f, 2.0f);

  // 正規分布
  // 平均 0、標準偏差 1 で分布させる
  std::normal_distribution<GLfloat> normal(mean, deviation);

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
    const Point p{ r * sp * ct, r * sp * st, r * cp };

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
  Window window("metaball", 512, 512);

  //
  // スライス用のフレームバッファオブジェクト
  //

  // フレームバッファオブジェクト
  GLuint sliceFbo[2];
  glGenFramebuffers(2, sliceFbo);

  // レンダーターゲットに使うテクスチャ
  GLuint sliceColor[2];
  glGenTextures(2, sliceColor);

  // レンダーターゲットの初期値
  static constexpr GLfloat zero[] = { 0.0f, 0.0f, 0.f, 0.0f };

  // レンダーターゲット
  static constexpr GLenum sliceBufs[] = { GL_COLOR_ATTACHMENT0 };

  for (int i = 0; i < 2; ++i)
  {
    // レンダーターゲットのテクスチャを準備する
    glBindTexture(GL_TEXTURE_2D, sliceColor[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sliceWidth, sliceHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // フレームバッファオブジェクトに組み込む
    glBindFramebuffer(GL_FRAMEBUFFER, sliceFbo[i]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sliceColor[i], 0);

    // カラーバッファを消去する
    glDrawBuffers(1, sliceBufs);
    glClearBufferfv(GL_COLOR, 0, zero);
  }

  //
  // 点群
  //

  // 点群を生成する
  Points points;
  generatePoints(points, sphereCount, sphereMean, sphereDeviation);

  // 点群の頂点配列オブジェクト
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // 点群の位置の頂点バッファオブジェクト
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), NULL, GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

#if SORT
  // 点群のインデックスの頂点バッファオブジェクト
  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, points.size() * sizeof(GLuint), NULL, GL_STREAM_DRAW);

  // バケット
  std::vector<GLuint> bucket[sliceCount];
#endif

  // 頂点配列オブジェクトを標準に戻す
  glBindVertexArray(0);

  // 点を描くシェーダ
  const GgPointShader pointShader("point.vert", "point.frag");

  // スライスの位置
  const GLint zSliceLoc(glGetUniformLocation(pointShader.get(), "zSlice"));

  // メタボールの半径
  const GLint radiusLoc(glGetUniformLocation(pointShader.get(), "radius"));

  // フレームバッファのサイズ
  const GLint sizeLoc(glGetUniformLocation(pointShader.get(), "size"));

  // 点のサイズはシェーダから変更する
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  //
  // 等値面表示
  //

  // 矩形を描くシェーダ
  const GLuint rectangleShader(ggLoadShader("rectangle.vert", "rectangle.frag"));

  // uniform block の場所を取得する
  const GLint lightlLoc(glGetUniformBlockIndex(rectangleShader, "Light"));

  // uniform block の場所を 0 番の結合ポイントに結びつける
  glUniformBlockBinding(rectangleShader, lightlLoc, 0);

#if DEPTH
  // スクリーン座標系での奥行き値
  const GLint zClipLoc(glGetUniformLocation(rectangleShader, "zClip"));
#endif

  // 法線変換行列
  const GLint mnLoc(glGetUniformLocation(rectangleShader, "mn"));

  // テクスチャ
  const GLint imageLoc(glGetUniformLocation(rectangleShader, "image"));

  // 閾値
  const GLint thresholdLoc(glGetUniformLocation(rectangleShader, "threshold"));

  //
  // 合成する図形
  //

  // 図形描画用のシェーダ
  GgSimpleShader shader("simple.vert", "simple.frag");

  // 図形描画用の光源
  GgSimpleShader::LightBuffer light(lightData);

  //
  // OpenGL の設定
  //

  /*
  ** レンダリング結果のブレンド
  **
  **   glBlendFunc(GL_ONE, GL_ZERO);                       // 上書き（デフォルト）
  **   glBlendFunc(GL_ZERO, GL_ONE);                       // 描かない
  **   glBlendFunc(GL_ONE, GL_ONE);                        // 加算
  **   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // 通常のアルファブレンデング
  **   glBlendColor(0.01f, 0.01f, 0.01f, 0.0f);            // 加算する定数
  **   glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);             // 定数を加算
  */

  // フレームバッファに加算する
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);

#if !DEPTH
  // デプスバッファは使わない
  glDisable(GL_DEPTH_TEST);
#endif

  // 背景色のアルファ値は 0 にする
  glClearColor(background[0], background[1], background[2], 0.0f);

  //
  // 描画の設定
  //

  // ビュー変換行列
  const GgMatrix mv(ggLookat(cameraPosition, cameraTarget, cameraUp));

  // 法線変換行列
  const GgMatrix mn(mv.normal());

  // 投影変換行列
  const GgMatrix mp(ggPerspective(cameraFovy, 1.0f, cameraNear, cameraFar));

  // 図形データ
  //const GgSimpleObj object("AC_1038.obj", shader);
  const GgSimpleObj object("bunny.obj", shader, true);
  //const GgSimpleObj object("box.obj", shader);

  //
  // データの転送
  //

  // メタボールの中心の位置の位置のデータを送信する
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(Point), points.data());

  //
  // 描画
  //

#if TIME
  unsigned int frame(0);
  double accum_sort(0.0), accum_total(0.0);
#endif

  // ウィンドウが開いている間繰り返す
  while (window.shouldClose() == GL_FALSE)
  {
#if TIME
    // 時計をリセット
    glfwSetTime(0.0);
#endif

    // モデルビュー変換行列
    const GgMatrix mw(mv * window.getTrackball(0));

    // メタボールの半径
    const GLfloat sphereRadius(window.getWheel(0) * 0.01f + 0.2f);

#if TIME
    // 前処理（バケットソート）の処理時間
    const double time_sort_start(glfwGetTime());
#endif

#if SORT
    //
    // バケットソート
    //

    // バケットのクリア
    for (int bucketNum = 0; bucketNum < sliceCount; ++bucketNum)
      bucket[bucketNum].clear();

    // すべてのメタボールの中心について
    for (size_t pointNum = 0; pointNum < points.size(); ++pointNum)
    {
      // メタボールの中心の視点座標系の z 値
      const GLfloat zw(mw.get(2) * points[pointNum][0] + mw.get(6) * points[pointNum][1]
        + mw.get(10) * points[pointNum][2] + mw.get(14));

      // メタボールの前端のスクリーン座標系における z 値
      const GLfloat zsf(mp.get(10) + mp.get(14) / (zw + sphereRadius));

      // zsf の符号を反転してメタボールの前端の位置におけるバケット番号を求める
      int bucketFront(static_cast<int>(ceil((0.5f - zsf * 0.5f) * sliceCount - 0.5f)));
      if (bucketFront < 0) bucketFront = 0;

      // メタボールの後端のスクリーン座標系における z 値
      const GLfloat zsr(mp.get(10) + mp.get(14) / (zw - sphereRadius));

      // zsr の符号を反転してメタボールの後端の位置におけるバケット番号を求める
      int bucketBack(static_cast<int>(floor((0.5f - zsr * 0.5f) * sliceCount - 0.5f)));
      if (bucketBack >= sliceCount) bucketBack = sliceCount - 1;

      // バケットソート
      for (int bucketNum = bucketFront; bucketNum <= bucketBack; ++bucketNum)
        bucket[bucketNum].emplace_back(static_cast<GLuint>(pointNum));
    }
#endif

#if TIME
    // 前処理（バケットソート）の処理時間
    const double time_sort(glfwGetTime() - time_sort_start);
#endif

#if DEPTH
    // 標示用のカラーバッファとデプスバッファを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    // 標示用のカラーバッファを消去する
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    // テクスチャを割り当てる
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sliceColor[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sliceColor[1]);

    // 描画するスライス
    int f(0);

    // 各スライスについて
    for (int sliceNum = 0; sliceNum < sliceCount; ++sliceNum)
    {
      // スクリーン座標系におけるスライスの位置
      const GLfloat zClip(static_cast<GLfloat>(sliceNum * 2 + 1) / static_cast<GLfloat>(sliceCount) - 1.0f);

      // 視点座標系のスライスの位置
      const GLfloat zSlice(mp.get(14) / (zClip * mp.get(11) - mp.get(10)));

      // FBO に描く
      glBindFramebuffer(GL_FRAMEBUFFER, sliceFbo[f]);
      glDrawBuffers(1, sliceBufs);
      glClearBufferfv(GL_COLOR, 0, zero);

      // ビューポートをスライス用の FBO のサイズに設定する
      glViewport(0, 0, sliceWidth, sliceHeight);

      // メタボールを描くシェーダを指定する
      pointShader.use();

      // フレームバッファのサイズを設定する
      glUniform2f(sizeLoc, static_cast<GLfloat>(sliceWidth), static_cast<GLfloat>(sliceHeight));

      // 変換行列を設定する
      pointShader.loadMatrix(mp, mw);

      // スライスの位置を設定する
      glUniform1f(zSliceLoc, zSlice);

      // メタボールの半径を設定する
      glUniform1f(radiusLoc, sphereRadius);

#if DEPTH
      // メタボールの描画時にはデプスバッファは使わない
      glDisable(GL_DEPTH_TEST);
#endif

      // カラーバッファへの加算を有効にする
      glEnable(GL_BLEND);

      // 描画
      glBindVertexArray(vao);
#if SORT
      // バケットソートするときはインデックスを使って描画する
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bucket[sliceNum].size() * sizeof(GLuint), bucket[sliceNum].data());
      glDrawElements(GL_POINTS, static_cast<GLsizei>(bucket[sliceNum].size()), GL_UNSIGNED_INT, NULL);
#else
      // バケットソートしない場合は直接描画する
      glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
#endif

      // 通常のフレームバッファに描く
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, window.getWidth(), window.getHeight());

      // 矩形を描くシェーダを選択する
      glUseProgram(rectangleShader);

      // 光源のデータを設定する
      glBindBufferBase(GL_UNIFORM_BUFFER, 0, light.getBuffer());

      // テクスチャユニットを指定する
      const GLint image[] = { f, 1 - f };
      glUniform1iv(imageLoc, 2, image);

      // 法線変換行列を設定する
      glUniformMatrix4fv(mnLoc, 1, GL_FALSE, mn.get());

      // 閾値を設定する
      glUniform1f(thresholdLoc, window.getWheelY() * 0.1f + 1.0f);

#if DEPTH
      // スライスのスクリーン座標系の奥行き値を設定する
      glUniform1f(zClipLoc, zClip);

      // スライスの描画時にはデプスバッファを使う
      glEnable(GL_DEPTH_TEST);
#endif

      // フレームバッファへの加算を無効にする
      glDisable(GL_BLEND);

      // 矩形を描画する
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      // 描画するスライスを入れ替える
      f = 1 - f;
    }

#if DEPTH
    // 図形の描画
    shader.use(light, mp, mv * window.getTrackball(1));
    //object.draw();
#endif

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();

#if TIME
    // 全体の処理時間
#  if USE_GLFINISH
    glFinish();
#  endif
    const double time_total(glfwGetTime());
    accum_total += time_total;
    accum_sort += time_sort;
    if (++frame % 1000 == 0)
    std::cerr << frame << ": " << time_sort << " (" << accum_sort / frame << "), "
      << time_total - time_sort << " (" << (accum_total - accum_sort) / frame << "), "
      << time_total << " (" << accum_total / frame << ")\n";
#endif
  }

  // 作成したオブジェクトを削除する
  glDeleteFramebuffers(2, sliceFbo);
  glDeleteTextures(2, sliceColor);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
#if SORT
  glDeleteBuffers(1, &ibo);
#endif
}
