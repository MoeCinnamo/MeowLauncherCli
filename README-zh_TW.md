# MeowLauncherCli

[繁體中文](README-zh_TW.md) [English](README.md) [日本語](README-ja_JP.md)

> 一個中國式家長無從檢舉的 Minecraft 啟動器——因為它完全長在終端機裡，在那些「看見什麼都覺得在害小孩」的家長眼裡，你只是在認真寫程式。

## 簡介

MeowLauncherCli 是一個**純命令列的 Minecraft 啟動器**。它的目標，是把玩 Minecraft 需要的一切——遊戲版本、模組、Skin、世界存檔等等——全部收進單一隻可腳本化、沒有圖形介面的命令列程式。

沒有視窗、沒有圖示、沒有工作列圖示，只有終端機。有人路過偷瞄一眼，只會以為你在忙正事。

## 功能

- **版本管理** — 安裝並切換 Minecraft 正式版與快照版。
- **模組管理** — 從命令列安裝、啟用、停用模組。
- **資源包與光影** — 管理材質包、資源包與光影包（Shader）。
- **Skin 管理** — 不用開任何圖形介面就能更換 Skin。
- **世界與存檔管理** — 列出、備份與還原世界存檔。
- **帳號** — 支援離線帳號；Microsoft 登入規劃中。
- **啟動設定** — 透過 `config.json` 設定 JVM 參數、遊戲目錄與記憶體上限。
- **伺服器書籤** — 儲存伺服器位址，快速連線。
- **介面在地化** — 提供英文、繁體中文與日文，由 `lang/*.json` 驅動。
- **可腳本化** — 每個動作都是一個子命令，能自然搭配 shell 腳本與自動化流程。

## 架構

```
cmd/main/main.c           進入點；只負責呼叫 app_run()
internal/app/             應用程式生命週期：初始化、事件迴圈、資源清理
internal/                 內部模組，不對外部消費者公開
lang/                     介面字串：en.json、zh-TW.json、ja-JP.json
scripts/                  建置與測試腳本
tests/                    測試套件
```

MeowLauncherCli 以 **C11** 撰寫，並以 **musl libc** 為目標函式庫，方便編譯成小巧、靜態連結的單一執行檔。模組隔離在 `internal/` 之下，內部函式預設加上 `static`，且專案優先使用 C 標準函式庫，不仰賴第三方依賴。

## 建置

```sh
./scripts/build.sh
```

需要 C11 編譯器；建議使用 musl 工具鏈（例如 `musl-gcc`）以符合目標執行環境。建置腳本隨開發初期逐步整備中。

## 設定

將 `config.json.example` 複製為 `config.json` 後依需求調整。所有面向使用者的字串都抽出放在 `lang/` 目錄，介面語言依你的語系設定切換。

## 現況

開發初期。模組邊界與骨架已就緒，各項功能正以模組為單位逐步落地。

## 授權條款

以 **GNU Affero General Public License v3.0**（AGPL-3.0）釋出。全文請參閱 [LICENSE](LICENSE)。
