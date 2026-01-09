# 40Hz Auditory Stimulation Generator

PNAS論文「Long-term effects of forty-hertz auditory stimulation as a treatment of Alzheimer's disease」に基づいた40Hz聴覚刺激生成プログラムです。

## 論文の仕様

- **トーン周波数**: 1kHz（純音）
- **トーン持続時間**: 1ミリ秒
- **刺激レート**: 40Hz（25ミリ秒ごとに1回）
- **強度**: 60dB

## 必要条件

- C++17対応コンパイラ
- SDL2ライブラリ

### macOSでのSDL2インストール

```bash
brew install sdl2
```

### Ubuntu/Debianでのインストール

```bash
sudo apt-get install libsdl2-dev
```

## ビルド方法

### Makefileを使用

```bash
make
```

### CMakeを使用

```bash
mkdir build
cd build
cmake ..
make
```

## 実行

```bash
./pnas_sound
```

または

```bash
make run
```

## 操作方法

- **スペースキー**: 一時停止/再開
- **Q または ESC**: 終了

## ⚠️ 注意事項

**このプログラムは研究・教育目的のみです。**

アルツハイマー病の治療として使用する前に、必ず医療専門家にご相談ください。論文で使用された実験は、厳密に管理された環境下で行われています。

## 参考文献

Wang, W., et al. (2026). Long-term effects of forty-hertz auditory stimulation as a treatment of Alzheimer's disease: Insights from an aged monkey model study. *PNAS*, 123(2), e2529565123.

https://www.pnas.org/doi/10.1073/pnas.2529565123
