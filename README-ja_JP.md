# MeowLauncherCli

<a href="https://github.com/MoeCinnamo/MeowLauncherCli/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/MoeCinnamo/MeowLauncherCli/ci.yml?branch=dev&style=flat-square"></a>
<a href="https://codecov.io/gh/MoeCinnamo/MeowLauncherCli"><img alt="Codecov" src="https://img.shields.io/codecov/c/github/MoeCinnamo/MeowLauncherCli?style=flat-square"></a>
<a href="https://github.com/MoeCinnamo/MeowLauncherCli/blob/master/LICENSE"><img alt="GitHub" src="https://img.shields.io/github/license/MoeCinnamo/MeowLauncherCli?style=flat-square"></a>

[日本語](README-ja_JP.md) [English](README.md) [繁體中文](README-zh_TW.md)

> 中国式な親が通報のしようがない Minecraft ランチャー——ターミナルで動くその姿は、「何でも子どもを害するもの」だと思い込む彼らの目には、真面目な開発作業にしか映らない。

## 概要

MeowLauncherCli は **Pure CLI な Minecraft ランチャー**です。Minecraft プレイヤーに必要なほぼすべて——バージョン、Mod、スキン、ワールドなど——を、GUI を一切持たない単一のスクリプト可能なコマンドラインアプリに集約することを目指しています。

ウィンドウもアイコンもトレイ常駐もありません。あるのはターミナルだけ。画面を覗き込まれても、真面目に開発作業をしているようにしか見えません。

## 使い方

```sh
meowlaunchcli            # 引数なし: ヘルプを表示（busybox 風）
meowlaunchcli version    # MeowLauncherCli v0.0.1 build <git>
meowlaunchcli <command>  # サブコマンドを実行
```

## コマンド

| コマンド    | 説明 |
| ----------- | ---- |
| `help`      | このヘルプを表示 |
| `version`   | MeowLauncherCli のバージョンを表示 |
| `install`   | ゲーム・modローダー・mod・パック・統合パックをインストール |
| `remove`    | ゲーム・modローダー・mod・パック・統合パックをアンインストール |
| `update`    | ゲーム情報ソースを更新 |
| `search`    | ゲーム・ローダー・mod・パックのバージョンを検索 |
| `game`      | ゲームバージョンを管理・切り替え |
| `launch`    | ゲームを起動 |
| `mod`       | mod を管理（現在のゲームバージョン） |
| `account`   | アカウントを管理 |
| `world`     | ワールドを管理（現在のゲームバージョン） |
| `resource`   | リソースパックを管理（現在のゲームバージョン） |
| `config`    | 設定を表示・変更 |

## ローカライズ

UI 言語はシステムロケール（`$LANG`）に自動追従します。英語・繁体字中国語・日本語の文字列をビルド時にバイナリへ埋め込みます。`LANGS="ja-JP" ./scripts/build.sh` のように単一言語だけを埋め込むことも可能です。

## ビルド

```sh
./scripts/build.sh     # Linux: musl-gcc、静的リンク
./scripts/test.sh      # ビルド + CLI スモークテスト
```

Windows (MSYS2/MinGW) では同じスクリプトが `gcc` を使い `meowlaunchcli.exe` を生成します。

環境変数で上書き可能: `CC`, `CFLAGS`, `LDFLAGS`, `LANGS`, `GIT_VERSION`。`COVERAGE=1` でカバレッジ計測用ビルドになります。

## 設定

`config.json.example` を `config.json` にコピーし、必要に応じて編集してください。

## 現況

開発初期段階です。`help` と `version` は実装済み、残りのコマンドはモジュール単位で順次実装されます。

## ライセンス

**GNU Affero General Public License v3.0**（AGPL-3.0）に基づいて公開されます。全文は [LICENSE](LICENSE) を参照してください。
