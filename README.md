# Raspberry Pi Pico/Pico 2 向け TOPPERS/FMP3

Raspberry Pi Pico と Pico 2 向けの[TOPPERS/FMP3](https://www.toppers.jp/fmp3-kernel.html)の個別パッケージになります。

[バージョン 3.2](https://www.toppers.jp/fmp3-e-download.html#raspi_pico)を、3.3にアップテーとしてあります。

## ビルド環境

[Visual Studio Code](https://code.visualstudio.com/)の[Raspberry Pi Pico 拡張機能](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)を利用します。

ツールチェインのダウンロードは、Visual Studio Code の Raspberry Pi Pico 拡張機能で行います。一度プロジェクトを作成してください。Pico SDK のバージョンは 2.1.0 に対応していますので、合わせたバージョンでプロジェクトを作成してください。作成したプロジェクトは使用しません。

拡張機能と同様に[CMake](https://cmake.org/)を使ったビルドファイルを用意しました。

TOPPERSのコンフィギュレーターは[Ruby](https://www.ruby-lang.org/)を使用しますので、インストールしてください。

## ビルド

このフォルダにある`pico_fmp3.code-workspace`を Visual Studio Code で開いてください。マルチワークスペース環境となっています。

`pico1_proj`が Raspberry Pi Pico 用の sample1 のプロジェクトになっています。

`pico2_proj`が Raspberry Pi Pico 2 用の sample1 のプロジェクトになっています。

ビルドの実行はメニューから「ターミナル」→「タスクの実行」→「clean build pico1_proj」または、「clean build pico2_proj」を選択してください。

Windows では PowerShell 向けのコマンドになっています。ターミナルをPowerShellにしてください。

## デバッグ

Raspberry Pi Pico 拡張機能と同じく[Raspberry Pi Debug Probe](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html)でデバッグすることが出来ます。
