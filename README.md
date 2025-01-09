# Raspberry Pi Pico/Pico 2 向け TOPPERS/FMP3

Raspberry Pi Pico と Pico 2 向けの[TOPPERS/FMP3](https://www.toppers.jp/fmp3-kernel.html)の個別パッケージになります。

[TOPPERS/FMP3 Ver.3.2 Raspberry Pi Pico 個別パッケージ](https://www.toppers.jp/fmp3-e-download.html#raspi_pico)を、Ver.3.3に更新してあります。

## ビルド環境

ビルド環境は[Visual Studio Code](https://code.visualstudio.com/)の[Raspberry Pi Pico 拡張機能](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)を利用します。

ツールチェインのダウンロードは、Visual Studio Code の Raspberry Pi Pico 拡張機能で行います。一度プロジェクトを作成してください。Pico SDK のバージョンは 2.1.0 に対応していますので、バージョンを合わせてプロジェクトを作成してください。作成したプロジェクトは使用しません。

TOPPERSのコンフィギュレーターは[Ruby](https://www.ruby-lang.org/)を使用しますので、インストールしてください。

Raspberry Pi Pico 拡張機能付属の[CMake](https://cmake.org/)を使ってビルドします。

## ビルド

このフォルダにある`pico_fmp3.code-workspace`を Visual Studio Code で開いてください。マルチワークスペース環境となっています。

`pico1_proj`が Raspberry Pi Pico 用の sample1 のプロジェクトになっています。

`pico2_proj`が Raspberry Pi Pico 2 用の sample1 のプロジェクトになっています。

ビルドの実行はメニューから「ターミナル」→「タスクの実行」→「clean build pico1_proj」または、「clean build pico2_proj」を選択してください。

Windows では PowerShell 向けのコマンドになっていますので、ターミナルの既定を PowerShell にしてください。

## デバッグ

Raspberry Pi Pico 拡張機能と同じく[Raspberry Pi Debug Probe](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html)でデバッグすることが出来ます。
