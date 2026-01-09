# 40Hz Auditory Stimulation Generator

PNAS論文「Long-term effects of forty-hertz auditory stimulation as a treatment of Alzheimer's disease」に基づいた40Hz聴覚刺激生成プログラムです。

## 論文の仕様

- **トーン周波数**: 1kHz（純音）
- **トーン持続時間**: 1ミリ秒
- **刺激レート**: 40Hz（25ミリ秒ごとに1回）
- **強度**: 60dB

## ダウンロード（macOS）

macOSユーザーは `pnas_sound_static` をダウンロードして、そのまま実行できます（SDL2のインストール不要）。

```bash
chmod +x pnas_sound_static
./pnas_sound_static
```

## 機能

- **40Hzパルスモード**: 論文仕様に基づいた1ms/1kHzトーンを40Hzで繰り返し
- **連続1kHzテストモード**: 1kHz純音の確認用
- **ビジュアルUI**: パルスインジケーター、ステータスバー、経過時間表示
- **自動終了**: 60分経過で自動停止（論文では1日1時間の刺激を使用）

## 操作方法

| キー | 機能 |
|------|------|
| SPACE | 一時停止 / 再開 |
| T | 連続1kHzトーン切り替え（テスト用） |
| Q / ESC | 終了 |

## ソースからビルド

### 必要条件

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

### ビルド方法

#### 動的リンク版（SDL2必要）

```bash
make
./pnas_sound
```

#### 静的リンク版（macOSのみ、SDL2不要で配布可能）

```bash
make static
./pnas_sound_static
```

### CMakeを使用

```bash
mkdir build
cd build
cmake ..
make
```

## ⚠️ 注意事項

**このプログラムは研究・教育目的のみです。**

アルツハイマー病の治療として使用する前に、必ず医療専門家にご相談ください。論文で使用された実験は、厳密に管理された環境下で行われています。

## 参考文献

Wang, W., et al. (2026). Long-term effects of forty-hertz auditory stimulation as a treatment of Alzheimer's disease: Insights from an aged monkey model study. *PNAS*, 123(2), e2529565123.

https://www.pnas.org/doi/10.1073/pnas.2529565123
