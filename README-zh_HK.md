# MeowLauncherCli

<a href="https://github.com/MoeCinnamo/MeowLauncherCli/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/MoeCinnamo/MeowLauncherCli/ci.yml?branch=main&style=flat-square"></a>
<a href="https://codecov.io/gh/MoeCinnamo/MeowLauncherCli"><img alt="Codecov" src="https://img.shields.io/codecov/c/github/MoeCinnamo/MeowLauncherCli?style=flat-square"></a>
<a href="https://github.com/MoeCinnamo/MeowLauncherCli/blob/master/LICENSE"><img alt="GitHub" src="https://img.shields.io/github/license/MoeCinnamo/MeowLauncherCli?style=flat-square"></a>

[繁體中文 (香港)](README-zh_HK.md) [English](README.md) [繁體中文 (台湾)](README-zh_TW.md) [日本語](README-ja_JP.md)

> 一個中國式家長無從檢舉的 Minecraft 啟動器——因為它完全長在終端機裡，在那些「看見什麼都覺得在害小孩」的家長眼裡，你只是在認真寫程式。

## 簡介

MeowLauncherCli 是一個**純命令列的 Minecraft 啟動器**。它的目標，是把玩 Minecraft 需要的一切——遊戲版本、模組、Skin、世界存檔等等——全部收進單一隻可腳本化、沒有圖形介面的命令列程式。

沒有視窗、沒有圖示、沒有工作列圖示，只有終端機。有人路過偷瞄一眼，只會以為你在忙正事。

## 用法

```sh
meowlaunchcli            # 不帶參數：顯示幫助列表（busybox 風格）
meowlaunchcli version    # MeowLauncherCli v0.0.1 build <git>
meowlaunchcli <command>  # 執行子指令
```

## 指令

| 指令       | 說明 |
| ---------- | ---- |
| `help`     | 顯示此幫助列表 |
| `version`  | 顯示 MeowLauncherCli 版本號 |
| `install`  | 安裝遊戲、模組載入器、模組、資源包、整合包 |
| `remove`   | 移除遊戲、模組載入器、模組、資源包、整合包 |
| `update`   | 更新遊戲來源 |
| `search`   | 搜尋遊戲、載入器、模組、資源包、整合包版本 |
| `game`     | 管理和切換遊戲版本（`game help/switch/current/show`） |
| `mod`      | 管理模組（目前遊戲版本） |
| `account`  | 管理帳號（`account local add/del/show <使用者>`） |
| `world`    | 管理世界（目前遊戲版本） |
| `resource` | 管理資源包（目前遊戲版本） |
| `config`   | 設定 |

## 在地化

介面語言依系統語系（`$LANG`）自動切換。英文、繁體中文、日文文案在構建時嵌入二進位檔；例如 `LANGS="zh-TW" ./scripts/build.sh` 可只編譯單一語言。

## 建置

```sh
./scripts/build.sh     # Linux：musl-gcc，靜態連結
./scripts/test.sh      # 建置 + 命令列冒煙測試
```

Windows（MSYS2/MinGW）下同一支腳本使用 `gcc`，產出 `meowlaunchcli.exe`。

可環境變數覆蓋：`CC`、`CFLAGS`、`LDFLAGS`、`LANGS`、`GIT_VERSION`；加上 `COVERAGE=1` 可輸出程式碼覆蓋率資訊。

## 設定

將 `config.json.example` 複製為 `config.json` 後依需求調整。

## 現況

開發初期。`help`、`version`、`account local add/del/show` 與
`game help/switch/current/show` 已實作，其餘指令正以模組為單位逐步落地。

## 文件

- [English](docs/en/index.md)
- [繁體中文](docs/zh-TW/index.md)
- [日本語](docs/ja-JP/index.md)

## 安全性

請參閱 [SECURITY.md](SECURITY.md)。

## 授權條款

以 **GNU Affero General Public License v3.0**（AGPL-3.0）釋出。全文請參閱 [LICENSE](LICENSE)。
