# 遊戲版本管理

目前遊戲版本存放在 `<exedir>/.meowlaunch/config.json`。
已安裝版本為 `<exedir>/.minecraft/versions/` 下的目錄。

## 說明

```
meowlaunchcli game help
```

## 切換

```
meowlaunchcli game switch <版本>
```

寫入 `{"game_version":"<版本>"}` 到 `.meowlaunch/config.json`。

## 顯示目前版本

```
meowlaunchcli game current
```

顯示目前版本，若未設定則顯示 `(none)`。

## 列出已安裝版本

```
meowlaunchcli game show
```

列出 `.minecraft/versions/` 下的所有目錄。
