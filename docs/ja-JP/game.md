# ゲームバージョン管理

現在のゲームバージョンは `<exedir>/.meowlaunch/config.json` に保存されます。
インストール済みバージョンは `<exedir>/.minecraft/versions/` 以下のディレクトリです。

## ヘルプ

```
meowlaunchcli game help
```

## 切り替え

```
meowlaunchcli game switch <バージョン>
```

`{"game_version":"<バージョン>"}` を `.meowlaunch/config.json` に書き込みます。

## 現在のバージョンを表示

```
meowlaunchcli game current
```

現在のバージョンを表示します。未設定の場合は `(none)` と表示されます。

## インストール済みを一覧

```
meowlaunchcli game show
```

`.minecraft/versions/` 以下のディレクトリを一覧表示します。
