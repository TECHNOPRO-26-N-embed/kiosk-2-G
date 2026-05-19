# ��️ 設計ワークシート

> **使い方**: このテンプレートをコピーして、チームの設計内容を記入してください。
> Day 1〜2（手作業フェーズ）で手書きした内容を、最終的にこのMarkdownに清書してGit提出します。

---

## 1. プロジェクト情報

| 項目 | 記入欄 |
| :--- | :--- |
| **チーム名** | 2-G|
| **選択テーマ（A〜C）** | A|
| **アプリ名** |自動販売機 |

---

## 2. 要求仕様書（顧客ヒアリング結果）

### 2.1 顧客の要望（ヒアリングメモ）

* 商品を選択して購入できるようにしたい
* 投入金額を管理し、お釣りを返却できるようにしたい
* 在庫数を管理したい
* 売上データをCSVに保存したい
* 操作ログやバックアップ機能も用意したい

### 2.2 機能要件

| 機能ID | 機能名 | 概要 |
| :---: | :--- | :--- |
| F01 | 商品選択 | 商品番号や商品IDを入力して商品を選択する |
| F02 | 金額投入 | ユーザーから投入金額を受け取り、合計金額を管理する |
| F03 | 商品購入 | 選択商品・投入金額・在庫を確認し、購入処理を行う |
| F04 | お釣り計算・返却 | 購入後のお釣り、またはキャンセル時の投入金額を返却する |
| F05 | 売上データ保存 | 購入データをCSVファイルに記録する |
| F06 | 在庫管理 | 商品の在庫確認・更新・補充を行う |
| F07 | 商品情報管理 | 商品の追加・入れ替え・情報更新を行う |
| F08 | ログ記録 | 操作履歴や障害予兆をログとして記録する |
| F09 | データ読込・保存 | 起動時に商品データを読み込み、変更時に保存する |
| F10 | セキュリティ・バックアップ | 不正操作チェックとデータバックアップを行う |

---

## 3. 基本設計

### 3.1 画面遷移図

```text
メインメニュー → [1] → 商品一覧表示 → [商品番号選択] → 金額投入 → 購入確認 → [確定] → 商品排出・お釣り返却 → メインメニュー
               → [2] → 投入金額返却 → メインメニュー
               → [3] → 管理メニュー → 在庫更新・商品入替・バックアップ
               → [4] → 終了
```

### 3.2 データの洗い出し

| 情報の名前 | 内容・説明 | 例 |
| :--- | :--- | :--- |
| 商品ID | 商品を識別する番号 | 1 |
| 商品名 | 販売する商品の名前 | 「水」 |
| 価格 | 商品の販売価格 | 120円 |
| 在庫数 | 現在の在庫数 | 10個 |
| 投入金額 | ユーザーが投入した金額 | 500円 |
| お釣り | 購入後に返却する金額 | 380円 |
| 売上金額 | 購入された商品の金額 | 120円 |
| 操作ログ | 操作内容やエラー内容 | 「商品購入成功」 |

### 3.3 CSV保存形式

```csv
日時,商品ID,商品名,価格,投入金額,お釣り
2026-05-19 10:30:00,1,水,120,500,380
```

---


1. 商品選択・購入関連
select_product()
 商品を選択する処理。選択された商品IDや番号を受け取る。

purchase_product()
 選択された商品と投入金額をもとに購入処理を行う。購入可否判定も含む。

2. 金額管理・計算
insert_money()
 ユーザーからの投入金額を受け付け、合計金額を管理する。

calculate_change()
 購入後のお釣りを計算し、返却金額を算出する。

return_money()
 購入キャンセル時やお釣り返却時に、投入金額を返却する。

3. データ記録・ログ
record_sales_data()
 売上データをCSV等のファイルに保存する。

log_operation()
 操作履歴や障害予兆などのログを記録する。

4. 在庫管理
check_stock()
 指定商品の在庫数を確認する。

update_stock()
 購入や補充時に在庫数を更新する。

replace_product()
 商品入れ替えや新商品追加時に商品情報を更新する。

5. セキュリティ・保守
validate_operation()
 不正操作を検知・防止するためのチェック処理。

backup_data()
 障害発生時や保守用にデータをバックアップする。

6. その他
load_product_data()
 起動時に商品情報や在庫数をファイル等から読み込む。

save_product_data()
 終了時や在庫変更時に商品情報・在庫数を保存する。





役割分担をするなら
1. 商品・在庫管理担当
商品情報の設計
在庫管理機能（最大在庫数、入れ替え対応）
商品データの読み込み・保存
2. 金銭管理・決済担当
投入金額管理
代金計算・お釣り計算
返却機能
3. 購入・ユーザーインターフェース担当
商品選択・購入処理
ユーザー操作フロー設計
エラーハンドリング
4. データ記録・ログ担当
売上データ記録（CSV等）
操作履歴・障害予兆ログ
障害時のデータ保存・バックアップ
5. セキュリティ・保守性担当
不正操作防止設計
データ保護・アクセス制御
保守性・拡張性の設計














# 自動販売機システム 詳細設計書（関数仕様・処理フロー）

## 0. 前提
- 対象: C言語で実装する自動販売機アプリ。
- 金額単位: 円（`int` で管理）。
- 在庫上限: 1商品あたり最大50個（`max_stock`）。
- 永続化ファイル:
  - `data/products.csv`（商品・在庫）
  - `data/sales.csv`（売上履歴）
  - `data/operation.log`（操作・異常ログ）
- エラーコード方針:
  - `0`: 正常
  - 負値: 異常（関数ごとに定義）

## 1. 共通データ定義

### 1.1 Product
```c
typedef struct {
    int id;               // 商品ID（一意）
    char name[32];        // 商品名
    int price;            // 価格（円）
    int stock;            // 現在在庫
    int max_stock;        // 在庫上限（通常50）
    int is_active;        // 1: 販売中, 0: 販売停止
} Product;
```

### 1.2 RuntimeState
```c
typedef struct {
    int inserted_money;       // 投入金額合計
    int selected_product_id;  // 選択中商品ID（未選択は -1）
    int cancel_requested;     // キャンセル要求フラグ
} RuntimeState;
```

### 1.3 OperationContext（ログ・監査用）
```c
typedef struct {
    const char* operator_id;  // 操作者ID（一般利用者/管理者）
    const char* terminal_id;  // 端末ID
    const char* timestamp;    // ISO8601 文字列
} OperationContext;
```

## 2. 1. 商品選択・購入関連

### select_product()
商品を選択する処理。選択された商品IDや番号を受け取る。

- 想定シグネチャ
```c
int select_product(const Product products[], int product_count,
                   int input_id, int* selected_product_id);
```
- 入力
  - `products`: 商品配列
  - `product_count`: 商品件数
  - `input_id`: ユーザー入力の商品ID
  - `selected_product_id`: 選択結果の出力先
- 出力
  - `selected_product_id` に採用IDを設定
- 戻り値
  - `0`: 正常
  - `-1`: 商品が存在しない
  - `-2`: 販売停止商品
  - `-3`: 引数不正（NULL/件数不正）
- 前提条件
  - `products != NULL`
  - `selected_product_id != NULL`
  - `product_count > 0`
- 処理フロー
  1. 引数の妥当性を確認。
  2. `input_id` に一致する商品を探索。
  3. 未検出なら `-1` を返す。
  4. `is_active == 0` なら `-2` を返す。
  5. `*selected_product_id = input_id` を設定して `0` を返す。

### purchase_product()
選択された商品と投入金額をもとに購入処理を行う。購入可否判定も含む。

- 想定シグネチャ
```c
int purchase_product(Product products[], int product_count,
                     RuntimeState* state, int selected_product_id,
                     int* change, const OperationContext* ctx);
```
- 入力
  - `products`, `product_count`
  - `state->inserted_money`
  - `selected_product_id`
  - `ctx`: ログ用コンテキスト
- 出力
  - `change`: お釣り額
  - 在庫更新、売上記録、投入金額の更新
- 戻り値
  - `0`: 購入成功
  - `-1`: 商品不正/未選択
  - `-2`: 在庫不足
  - `-3`: 金額不足
  - `-4`: 売上記録失敗
  - `-5`: 商品保存失敗
  - `-6`: 引数不正
- 処理フロー
  1. 引数・選択状態を検証。
  2. `validate_operation("purchase", ...)` を実行。
  3. `check_stock()` で在庫確認（0なら `-2`）。
  4. 投入金額と価格を比較（不足なら `-3`）。
  5. `update_stock(..., -1)` で在庫を減算。
  6. `calculate_change()` でお釣り算出。
  7. `record_sales_data()` で売上CSV追記。
  8. `save_product_data()` で在庫変更を永続化。
  9. `return_money()` でお釣りを返却し投入額をクリア。
  10. `log_operation(INFO, "purchase_product", "success", ...)` を記録。
- 失敗時方針
  - 7以降が失敗した場合は `log_operation(ERROR, ...)` を必ず実行。
  - 在庫更新後に永続化失敗した場合は、次回起動で復旧できるよう障害ログとバックアップを優先。

## 3. 2. 金額管理・計算

### insert_money()
ユーザーからの投入金額を受け付け、合計金額を管理する。

- 想定シグネチャ
```c
int insert_money(RuntimeState* state, int money, const OperationContext* ctx);
```
- 入力
  - `money`: 今回投入額
- 出力
  - `state->inserted_money` を更新
- 戻り値
  - `0`: 正常
  - `-1`: 非対応金種
  - `-2`: 合計金額上限超過
  - `-3`: 引数不正
- 仕様
  - 対応金種: `10, 50, 100, 500, 1000`
  - 合計投入上限: 10000円（運用で変更可）
- 処理フロー
  1. `state` と `money` を検証。
  2. 金種テーブルで受理可否判定。
  3. 加算後の合計が上限超過か判定。
  4. 問題なければ加算し `INFO` ログを残す。

### calculate_change()
購入後のお釣りを計算し、返却金額を算出する。

- 想定シグネチャ
```c
int calculate_change(int inserted_money, int price, int* change);
```
- 入力
  - `inserted_money`, `price`
- 出力
  - `*change = inserted_money - price`
- 戻り値
  - `0`: 正常
  - `-1`: 金額不足
  - `-2`: 引数不正（NULL/負値）
- 処理フロー
  1. `change` ポインタと金額の負値チェック。
  2. `inserted_money < price` なら `-1`。
  3. 差額を格納して `0`。

### return_money()
購入キャンセル時やお釣り返却時に、投入金額を返却する。

- 想定シグネチャ
```c
int return_money(RuntimeState* state, int amount_to_return, const OperationContext* ctx);
```
- 入力
  - `amount_to_return`: 返却対象額
- 出力
  - デバイス返却処理（想定）
  - `state->inserted_money` を減額または0化
- 戻り値
  - `0`: 正常
  - `-1`: 不正金額（負値/過大）
  - `-2`: 状態不整合
  - `-3`: デバイス返却失敗（将来拡張）
- 処理フロー
  1. `state` と返却額を検証。
  2. 返却額が現在投入額以下か確認。
  3. 返却処理を実行。
  4. `inserted_money` を更新（通常は0）。
  5. 返却結果をログ記録。

## 4. 3. データ記録・ログ

### record_sales_data()
売上データをCSV等のファイルに保存する。

- 想定シグネチャ
```c
int record_sales_data(const Product* product, int quantity,
                      int inserted_money, int change,
                      const OperationContext* ctx);
```
- 入力
  - `product`: 購入商品
  - `quantity`: 購入数（通常1）
  - `inserted_money`, `change`
  - `ctx->timestamp`
- 出力
  - `data/sales.csv` に1行追記
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: ファイルオープン失敗
  - `-3`: 書き込み失敗
- CSVレコード例
  - `timestamp,product_id,product_name,price,quantity,total,inserted,change,terminal_id`
- 処理フロー
  1. 入力検証。
  2. `sales.csv` を追記モードで開く。
  3. CSV行を生成して書き込む。
  4. `fflush`/`fclose` し戻り値を返す。

### log_operation()
操作履歴や障害予兆などのログを記録する。

- 想定シグネチャ
```c
int log_operation(const char* level, const char* operation,
                  const char* message, const OperationContext* ctx);
```
- 入力
  - `level`: `INFO/WARN/ERROR`
  - `operation`: 関数名または操作イベント名
  - `message`: 詳細文
  - `ctx`: 操作者・端末・時刻
- 出力
  - `data/operation.log` に追記
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: ログ書き込み失敗
- 処理フロー
  1. 引数とログレベルを検証。
  2. ログファイルを追記モードで開く。
  3. `timestamp,level,operation,operator_id,terminal_id,message` 形式で記録。
  4. 異常時は標準エラーへも出力（運用保険）。

## 5. 4. 在庫管理

### check_stock()
指定商品の在庫数を確認する。

- 想定シグネチャ
```c
int check_stock(const Product products[], int product_count,
                int product_id, int* stock);
```
- 入力
  - `product_id`
- 出力
  - `*stock` に現在在庫
- 戻り値
  - `0`: 正常
  - `-1`: 商品なし
  - `-2`: 引数不正
- 処理フロー
  1. 引数妥当性チェック。
  2. 商品探索。
  3. 見つかれば在庫を返す。

### update_stock()
購入や補充時に在庫数を更新する。

- 想定シグネチャ
```c
int update_stock(Product products[], int product_count,
                 int product_id, int delta, const OperationContext* ctx);
```
- 入力
  - `delta`: 購入時 `-1`、補充時 `+n`
- 出力
  - 対象商品の在庫を更新
- 戻り値
  - `0`: 正常
  - `-1`: 商品なし
  - `-2`: 在庫不足（0未満）
  - `-3`: 在庫上限超過
  - `-4`: 引数不正
- 処理フロー
  1. 対象商品を特定。
  2. 新在庫 `stock + delta` を試算。
  3. `0 <= new_stock <= max_stock` を検証。
  4. 更新し、在庫変更ログを記録。

### replace_product()
商品入れ替えや新商品追加時に商品情報を更新する。

- 想定シグネチャ
```c
int replace_product(Product products[], int* product_count, int max_products,
                    int target_product_id, const Product* new_product,
                    int add_if_not_found, const OperationContext* ctx);
```
- 入力
  - `target_product_id`: 置換対象
  - `new_product`: 新商品情報
  - `add_if_not_found`: 未検出時に追加するか（0/1）
- 出力
  - 商品配列更新
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: 配列上限超過
  - `-3`: 対象なし（追加禁止時）
  - `-4`: 商品ID重複
- 処理フロー
  1. `new_product` の妥当性（価格・在庫・名称長）を確認。
  2. `target_product_id` を探索。
  3. 見つかれば置換、見つからず追加許可なら追加。
  4. `save_product_data()` で永続化。
  5. 成功/失敗を `log_operation()` に記録。

## 6. 5. セキュリティ・保守

### validate_operation()
不正操作を検知・防止するためのチェック処理。

- 想定シグネチャ
```c
int validate_operation(const char* operation,
                       int product_id, int money,
                       const RuntimeState* state,
                       const OperationContext* ctx);
```
- チェック対象
  - 不正商品ID（存在しないID）
  - 金額異常（負値、上限超え）
  - 不正状態遷移（未選択購入、購入後の重複決済）
  - 短時間の異常連打（閾値超過）
- 戻り値
  - `0`: 問題なし
  - `-1`: 入力不正
  - `-2`: 状態遷移不正
  - `-3`: セキュリティ閾値超過
- 処理フロー
  1. 操作種別ごとにルールセットを適用。
  2. NG時は `ERROR` ログ、軽微時は `WARN` ログ。
  3. 重大異常は処理続行を禁止して呼び出し元へ返す。

### backup_data()
障害発生時や保守用にデータをバックアップする。

- 想定シグネチャ
```c
int backup_data(const char* src_dir, const char* backup_root,
                const OperationContext* ctx);
```
- 入力
  - `src_dir`: `data` ディレクトリ
  - `backup_root`: バックアップ保存先
- 出力
  - タイムスタンプ付きバックアップディレクトリ作成
  - 商品・売上・ログファイルをコピー
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: ディレクトリ作成失敗
  - `-3`: ファイルコピー失敗
- 処理フロー
  1. バックアップ先名を生成（例: `backup_YYYYMMDD_HHMMSS`）。
  2. 対象ファイルを順次コピー。
  3. 失敗時は即時 `ERROR` ログ。
  4. 完了時は `INFO` ログ。

## 7. 6. その他

### load_product_data()
起動時に商品情報や在庫数をファイル等から読み込む。

- 想定シグネチャ
```c
int load_product_data(const char* file_path,
                      Product products[], int* product_count,
                      int max_products, const OperationContext* ctx);
```
- 入力
  - `file_path`: `data/products.csv`
- 出力
  - 商品配列と件数を初期化
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: ファイルオープン失敗
  - `-3`: CSVフォーマット不正
  - `-4`: 商品数上限超過
- 処理フロー
  1. CSVを開いてヘッダを確認。
  2. 各行をパースし、項目妥当性を検証。
  3. 配列へ格納し `product_count` 更新。
  4. 読み込み結果をログ出力。

### save_product_data()
終了時や在庫変更時に商品情報・在庫数を保存する。

- 想定シグネチャ
```c
int save_product_data(const char* file_path,
                      const Product products[], int product_count,
                      const OperationContext* ctx);
```
- 入力
  - 商品配列・件数
- 出力
  - `products.csv` 更新
- 戻り値
  - `0`: 正常
  - `-1`: 引数不正
  - `-2`: 一時ファイル作成/オープン失敗
  - `-3`: 書き込み失敗
  - `-4`: ファイル置換失敗
- 処理フロー
  1. 一時ファイルに全件書き込み。
  2. 書き込み成功後、原子的に本ファイルへ置換。
  3. 失敗時は旧ファイルを維持し `ERROR` ログ。

## 8. 関数間シーケンス（標準購入）
1. `insert_money()` を必要回数実行。
2. `select_product()` で商品確定。
3. `validate_operation("purchase", ...)` を実行。
4. `purchase_product()` を実行。
5. 内部で `check_stock()` → `update_stock()` → `calculate_change()`。
6. 内部で `record_sales_data()` → `save_product_data()`。
7. 内部で `return_money()`（お釣り）と `log_operation()`。

## 9. 役割分担（実装担当案）

### 9.1 商品・在庫管理担当
- `check_stock()`
- `update_stock()`
- `replace_product()`
- `load_product_data()`
- `save_product_data()`

### 9.2 金銭管理・決済担当
- `insert_money()`
- `calculate_change()`
- `return_money()`

### 9.3 購入・ユーザーインターフェース担当
- `select_product()`
- `purchase_product()`
- エラーメッセージ仕様（不足金額、在庫切れ、入力不正）

### 9.4 データ記録・ログ担当
- `record_sales_data()`
- `log_operation()`
- `backup_data()`

### 9.5 セキュリティ・保守性担当
- `validate_operation()`
- 異常検知ルール（連打・不正入力・状態遷移）
- 将来拡張（管理者認証、アクセス制御、改ざん検知）

## 10. 実装時の補足
- 文字列入力は長さ制限付きで受け取る。
- CSV入出力はカンマ・改行混入対策を行う。
- 価格・在庫の整数範囲チェックを徹底する。
- 購入処理は「在庫更新」と「売上記録」の整合性を重視し、障害時はログとバックアップで追跡可能にする。
