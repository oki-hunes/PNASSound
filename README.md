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

#### Apple Silicon (arm64) での注意

Homebrew は Apple Silicon では /opt/homebrew 配下に入ります。もし SDL2 が見つからない場合は、
`CMAKE_PREFIX_PATH=/opt/homebrew` を指定してください。

#### Universal バイナリ（arm64 + x86_64）

Universal 化には Rosetta と x86_64 用 Homebrew（/usr/local）と SDL2 が必要です。

```bash
# Rosetta
softwareupdate --install-rosetta --agree-to-license

# x86_64 Homebrew
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
arch -x86_64 /usr/local/bin/brew install sdl2

# arm64/x86_64 を別々にビルドして結合
arch -x86_64 env PATH=/usr/local/bin:$PATH make clean static
mv pnas_sound_static pnas_sound_static_x86_64

arch -arm64 env PATH=/opt/homebrew/bin:$PATH make clean static
mv pnas_sound_static pnas_sound_static_arm64

lipo -create pnas_sound_static_x86_64 pnas_sound_static_arm64 -output pnas_sound_static
```

#### macOS アプリ化

```bash
mkdir -p PNASSound.app/Contents/{MacOS,Resources}
cp pnas_sound_static PNASSound.app/Contents/MacOS/PNASSound
chmod +x PNASSound.app/Contents/MacOS/PNASSound
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
