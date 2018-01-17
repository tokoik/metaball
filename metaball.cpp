// アプリケーション本体
#include "GgApplication.h"

// 乱数
#include <random>

// メタボールをバケットソートする場合は 1
#define SORT 1

// 時間を計測する場合は 1
#define TIME 0

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
// フレームバッファオブジェクト
//

// FBO のサイズ
constexpr GLsizei fboWidth(128), fboHeight(128);

// スライスの数
constexpr int slices(128);

//
// メタボール
//

// メタボールの数
constexpr int sphereCount(10000);

// メタボールの半径
constexpr GLfloat sphereRadius(0.2f);

// 点
typedef std::array<GLfloat, 3> Point;

// 点群
typedef std::vector<Point> Points;

// 点群の生成
static void generatePoints(Points &points, int count)
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
  // 平均 0、標準偏差 0.25 で分布させる
  std::normal_distribution<GLfloat> normal(0.0f, 0.25f);

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
  // フレームバッファオブジェクト
  //

  // カラーバッファに使うテクスチャ
  GLuint cb;
  glGenTextures(1, &cb);
  glBindTexture(GL_TEXTURE_2D, cb);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboWidth, fboHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // フレームバッファオブジェクト
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cb, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 矩形を描くシェーダ
  const GLuint rectangleShader(ggLoadShader("rectangle.vert", "rectangle.frag"));

  // 法線変換病列
  const GLint mnLoc(glGetUniformLocation(rectangleShader, "mn"));

  // テクスチャ
  const GLint imageLoc(glGetUniformLocation(rectangleShader, "image"));

  // 閾値
  const GLint thresholdLoc(glGetUniformLocation(rectangleShader, "threshold"));

  //
  // 点群
  //

  // 点群を生成する
  Points points;
  generatePoints(points, sphereCount);

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
#endif

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

  // デプスバッファは使わない
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // 背景色のアルファ値は 0 にする
  glClearColor(background[0], background[1], background[2], 0.0f);

  // フレームバッファオブジェクトの初期値
  constexpr GLfloat zero[] = { 0.0f, 0.0f, 0.f, 0.0f };

  // ビュー変換行列
  const GgMatrix mv(ggLookat(cameraPosition, cameraTarget, cameraUp));

  // 法線変換行列
  const GgMatrix mn(mv.normal());

  // 投影変換行列
  const GgMatrix mp(ggPerspective(cameraFovy, 1.0f, cameraNear, cameraFar));

  // バケット
  std::vector<GLuint> bucket[slices];

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
    const GgMatrix mw(mv * window.getLeftTrackball());

    //
    // データの転送
    //

    // メタボールの中心の位置の位置のデータを送信する
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(Point), points.data());

#if SORT
    //
    // バケットソート
    //

    // バケットのクリア
    for (int bucketNum = 0; bucketNum < slices; ++bucketNum)
      bucket[bucketNum].clear();

    // すべてのメタボールの中心について
    for (size_t pointNum = 0; pointNum < points.size(); ++pointNum)
    {
      // メタボールの中心の視点座標系の z 値
      const GLfloat zw(mw.get(2) * points[pointNum][0] + mw.get(6) * points[pointNum][1]
        + mw.get(10) * points[pointNum][2] + mw.get(14));

      // メタボールの前端の視点座標系の z 値
      const GLfloat zwf(zw + sphereRadius);

      // メタボールの前端のスクリーン座標系における z 値
      const GLfloat zsf((mp.get(10) * zwf + mp.get(14)) / (mp.get(11) * zwf));

      // メタボールの前端の位置におけるバケット番号
      int bucketFront(static_cast<int>(ceil((zsf * 0.5f + 0.5f) * slices - 0.5f)));
      if (bucketFront < 0) bucketFront = 0;

      // メタボールの後端の視点座標系の z 値
      const GLfloat zwr(zw - sphereRadius);

      // メタボールの後端のスクリーン座標系における z 値
      const GLfloat zsr((mp.get(10) * zwr + mp.get(14)) / (mp.get(11) * zwr));

      // メタボールの後端の位置におけるバケット番号
      int bucketBack(static_cast<int>(floor((zsr * 0.5f + 0.5f) * slices - 0.5f)));
      if (bucketBack >= slices) bucketBack = slices - 1;

      // バケットソート
      for (int bucketNum = bucketFront; bucketNum <= bucketBack; ++bucketNum)
        bucket[bucketNum].emplace_back(static_cast<GLuint>(pointNum));
    }
#endif

#if TIME
    // 前処理（バケットソート）の処理時間
    const double time_sort(glfwGetTime());
#endif

    // 表示ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // 各スライスについて
    for (int sliceNum = slices; --sliceNum >= 0;)
    {
      // スクリーン座標系におけるスライスの位置
      const GLfloat zClip(static_cast<GLfloat>(sliceNum * 2 + 1) / static_cast<GLfloat>(slices) - 1.0f);

      // 視点座標系のスライスの位置
      const GLfloat zSlice(mp.get(14) / (zClip * mp.get(11) - mp.get(10)));

      // FBO に描く
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glViewport(0, 0, fboWidth, fboHeight);

      // フレームバッファへの加算を有効にする
      glEnable(GL_BLEND);

      // メタボールを描くシェーダを指定する
      pointShader.use();

      // フレームバッファのサイズを設定する
      glUniform2f(sizeLoc, static_cast<GLfloat>(fboWidth), static_cast<GLfloat>(fboHeight));

      // 変換行列を設定する
      pointShader.loadMatrix(mp, mw);

      // スライスの位置を設定する
      glUniform1f(zSliceLoc, zSlice);

      // メタボールの半径を設定する
      glUniform1f(radiusLoc, sphereRadius);

      // フレームバッファを消去する
      glClearBufferfv(GL_COLOR, 0, zero);

      // 描画
      glBindVertexArray(vao);
#if SORT
      // バケットソートするときはインデックスを使って描画する
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bucket[sliceNum].size() * sizeof (GLuint), bucket[sliceNum].data());
      glDrawElements(GL_POINTS, static_cast<GLsizei>(bucket[sliceNum].size()), GL_UNSIGNED_INT, NULL);
#else
      // バケットソートしない場合は直接描画する
      glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
#endif

      // 通常のフレームバッファに描く
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, window.getWidth(), window.getHeight());

      // フレームバッファへの加算を無効にする
      glDisable(GL_BLEND);

      // 矩形を描くシェーダを選択する
      glUseProgram(rectangleShader);

      // テクスチャを割り当てる
      glBindTexture(GL_TEXTURE_2D, cb);
      glActiveTexture(GL_TEXTURE0);
      glUniform1i(imageLoc, 0);

      // 法線変換行列を設定する
      glUniformMatrix4fv(mnLoc, 1, GL_FALSE, mn.get());

      // 閾値を設定する
      glUniform1f(thresholdLoc, window.getWheel() * 0.1f + 1.0f);

      // 矩形を描画する
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

#if TIME
    // 全体の処理時間
    glFinish();
    const double time_total(glfwGetTime() - time_sort);
    accum_sort += time_sort;
    accum_total += time_total;
    ++frame;
    std::cerr << "sort:" << time_sort << " (" << accum_sort / frame << "), draw:" << time_total << " (" << accum_total / frame << ")\n";
#endif

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
