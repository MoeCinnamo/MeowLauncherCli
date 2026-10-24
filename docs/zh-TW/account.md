# 帳號管理

MeowLauncherCli 將本機離線帳號存放在 `<exedir>/.meowlaunch/data/users.json`。

## 新增本機帳號

```
meowlaunchcli account local add <使用者名稱>
```

建立以下欄位：
- `name`：你輸入的使用者名稱
- `type`：`"local"`（離線）
- `uuid`：隨機 v4 格式 UUID（8-4-4-4-12 十六進位）

## 移除本機帳號

```
meowlaunchcli account local del <使用者名稱>
```

找不到時印出「找不到帳號」。

## 列出本機帳號

```
meowlaunchcli account local show
```

每行印出一個本機帳號名稱。

## 儲存位置

`<exedir>/.meowlaunch/data/users.json`
