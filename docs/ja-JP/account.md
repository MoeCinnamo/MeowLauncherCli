# アカウント管理

MeowLauncherCli はローカルオフラインアカウントを `~/.meowlaunch/data/users.json` に保存します。

## ローカルアカウントを追加

```
meowlaunchcli account local add <ユーザー名>
```

- `name`: 指定したユーザー名
- `type`: `"local"`（オフライン）
- `uuid`: ランダムな v4 形式 UUID（8-4-4-4-12）

## ローカルアカウントを削除

```
meowlaunchcli account local del <ユーザー名>
```

見つからない場合は「アカウントが見つかりません」と表示されます。

## 保存場所

- Linux/macOS: `~/.meowlaunch/data/users.json`
- Windows: `C:\Users\<あなた>\.meowlaunch\data\users.json`
