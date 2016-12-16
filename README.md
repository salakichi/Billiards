# OpenGL Billiards
OpenGLのビリヤード。

# 必要なもの
* freeglut(2.8.1)
* GLEW(2.0.0)
* OpenCV(3.1.0)
* The Free Type Library(2.7)
* FTGL(2.1.3~rc5)
* OpenAL(1.17.2)
* freealut(1.1.0)

# その他使用しているライブラリ
* LodePNG

# ライブラリビルドの参考文献
* http://blog.kintarou.com/2016/03/21/bilding_opencv3-1-0_gui/
* http://slis.tsukuba.ac.jp/~fujisawa.makoto.fu/lecture/iml/text/screen_character.html
* http://c-crad.wktk.so/td/?p=314

# 注意
マテリアルは上げてないので、コンパイルはできても動きません。

# 遊び方
* マウスのDragで角度変更
* Enterキーで角度決定→Enterキーで強さ決定
* 白玉が落ちる or 全部玉が落ちたら終了

# TODO
* ポケットの判定がゆるいので、修正
* シャドウマッピングの対応
* MP3形式対応
* 2Pの実装

# (12/15時点)中間報告からの変更点
時間がとれなかったので指摘箇所されたところの修正のみに留まっています。
* delta timeを使用するように変更
* Ballクラスに位置情報を格納するように変更
