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

# 自動販売機システム 詳細設計（関数仕様・処理フロー）

## 1. 設計方針
- 目的: 「まず動く」「止まらない」を優先しつつ、在庫・売上・ログの一貫性を保つ。
- 在庫上限: 1商品あたり最大50個。
- 永続化: 商品情報はCSV、操作ログもCSV（またはテキスト）に出力。
- 失敗時方針: ファイル書き込み失敗時はエラーを返却し、`log_operation()` で異常ログを残す。

## 2. データモデル

### 2.1 Product構造体（想定）
```c
typedef struct {
	int id;                 // 商品ID（一意）
	char name[32];          // 商品名
	int price;              // 価格（円）
	int stock;              // 在庫数
	int max_stock;          // 最大在庫（通常50）
	int is_active;          // 販売中フラグ（1:販売中, 0:停止）
} Product;
```

### 2.2 RuntimeState構造体（想定）
```c
typedef struct {
	int inserted_money;     // 現在の投入金額合計
	int selected_product_id;// 選択中商品ID（未選択は-1）
} RuntimeState;
```

### 2.3 ファイル形式（想定）
- `data/products.csv`: `id,name,price,stock,max_stock,is_active`
- `data/sales.csv`: `timestamp,product_id,product_name,price,quantity,total,inserted,change`
- `data/operation.log`: `timestamp,level,operation,message`

## 3. 関数仕様と処理フロー

## 3.1 商品選択・購入関連

### select_product()
#### 目的
ユーザー入力（商品IDまたは番号）を受け取り、有効な商品選択を確定する。

#### シグネチャ例
```c
int select_product(const Product products[], int product_count, int input_id, int* selected_product_id);
```

#### 入力
- `products`: 商品配列
- `product_count`: 商品数
- `input_id`: ユーザーが入力した商品ID
- `selected_product_id`: 選択結果出力先

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 該当商品なし
- 戻り値 `-2`: 販売停止商品

#### 前提条件
- `products != NULL`
- `selected_product_id != NULL`
- `product_count > 0`

#### 副作用
- 正常時 `*selected_product_id` を更新。

#### 処理フロー
1. 引数妥当性をチェック（NULL、件数）。
2. `input_id` と一致する商品を線形探索。
3. 見つからなければ `-1` を返す。
4. `is_active == 0` なら `-2` を返す。
5. 選択IDを出力し `0` を返す。

---

### purchase_product()
#### 目的
選択商品の購入可否を判定し、購入成立時に在庫更新・売上記録・お釣り計算まで行う。

#### シグネチャ例
```c
int purchase_product(Product products[], int product_count, int selected_product_id,
					 RuntimeState* state, int* change);
```

#### 入力
- `products`, `product_count`
- `selected_product_id`: 購入対象
- `state->inserted_money`: 投入金額

#### 出力
- `change`: 購入後のお釣り
- 戻り値 `0`: 購入成功
- 戻り値 `-1`: 商品未選択/商品不正
- 戻り値 `-2`: 在庫不足
- 戻り値 `-3`: 金額不足
- 戻り値 `-4`: 記録処理失敗（売上/保存）

#### 前提条件
- `products != NULL`
- `state != NULL`
- `change != NULL`

#### 副作用
- 在庫を1減算
- 売上CSV追記
- 商品データ保存
- `state->inserted_money` をお釣り返却後に0へ戻す

#### 処理フロー
1. 引数・商品ID妥当性を確認。
2. `check_stock()` で在庫確認。
3. `state->inserted_money` と商品価格を比較。
4. 不足なら `-3` を返却（投入金額は保持）。
5. `update_stock()` で在庫を1減らす。
6. `calculate_change()` でお釣り計算。
7. `record_sales_data()` で売上記録。
8. `save_product_data()` で在庫変更を永続化。
9. `return_money()` でお釣り返却し、投入金額を0化。
10. `log_operation()` で購入成功ログ。

## 3.2 金額管理・計算

### insert_money()
#### 目的
投入可能金種を受け付け、投入合計金額に加算する。

#### シグネチャ例
```c
int insert_money(RuntimeState* state, int money);
```

#### 入力
- `money`: 今回投入額

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 非対応金種
- 戻り値 `-2`: 合計上限超過

#### 前提条件
- `state != NULL`
- 対応金種は例として `{10, 50, 100, 500, 1000}`

#### 副作用
- `state->inserted_money` を加算。

#### 処理フロー
1. 引数チェック。
2. 金種が許可リストにあるか判定。
3. 合計金額上限（例: 10000円）を超えないか確認。
4. 問題なければ加算し成功返却。

---

### calculate_change()
#### 目的
投入金額と商品価格の差分からお釣り額を算出する。

#### シグネチャ例
```c
int calculate_change(int inserted_money, int price, int* change);
```

#### 入力
- `inserted_money`
- `price`

#### 出力
- `change`: `inserted_money - price`
- 戻り値 `0`: 正常
- 戻り値 `-1`: 金額不足
- 戻り値 `-2`: 不正引数

#### 処理フロー
1. `change` のNULL確認。
2. `inserted_money < 0` または `price < 0` ならエラー。
3. `inserted_money < price` なら不足エラー。
4. 差分を計算し返却。

---

### return_money()
#### 目的
キャンセル時または購入後に返却すべき金額を返す。

#### シグネチャ例
```c
int return_money(RuntimeState* state, int amount_to_return);
```

#### 入力
- `amount_to_return`

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 不正金額
- 戻り値 `-2`: 状態不整合

#### 副作用
- 返却後に `state->inserted_money = 0`

#### 処理フロー
1. 引数チェック（負値禁止）。
2. `amount_to_return` が現在投入額を超えていないか確認。
3. 返却処理（画面表示・硬貨返却デバイス呼び出し想定）。
4. 投入額を0に初期化。

## 3.3 データ記録・ログ

### record_sales_data()
#### 目的
購入トランザクションを売上CSVへ追記する。

#### シグネチャ例
```c
int record_sales_data(const Product* product, int inserted_money, int change, const char* timestamp);
```

#### 入力
- `product`: 購入商品
- `inserted_money`, `change`
- `timestamp`

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: ファイルオープン失敗
- 戻り値 `-3`: 書き込み失敗

#### 副作用
- `data/sales.csv` に1行追加。

#### 処理フロー
1. 引数妥当性確認。
2. `sales.csv` を追記モードで開く。
3. CSV 1行を整形して書き込む。
4. flush/close し結果を返す。

---

### log_operation()
#### 目的
通常操作・警告・エラーを時系列に記録する。

#### シグネチャ例
```c
int log_operation(const char* level, const char* operation, const char* message, const char* timestamp);
```

#### 入力
- `level`: `INFO/WARN/ERROR`
- `operation`: 操作名（例: `purchase_product`）
- `message`: 詳細

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: ファイル書き込み失敗

#### 処理フロー
1. 引数チェック。
2. `operation.log` を追記で開く。
3. `timestamp,level,operation,message` 形式で書き込む。
4. 書き込み結果を返す。

## 3.4 在庫管理

### check_stock()
#### 目的
指定商品の在庫可否を返す。

#### シグネチャ例
```c
int check_stock(const Product products[], int product_count, int product_id, int* stock);
```

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 商品なし
- 戻り値 `-2`: 引数不正

#### 処理フロー
1. 引数チェック。
2. 商品探索。
3. 見つかれば `stock` に設定して返却。

---

### update_stock()
#### 目的
購入（減算）または補充（加算）で在庫を更新する。

#### シグネチャ例
```c
int update_stock(Product products[], int product_count, int product_id, int delta);
```

#### 入力
- `delta`: 購入時 `-1`、補充時 `+n`

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 商品なし
- 戻り値 `-2`: 在庫不足（0未満になる）
- 戻り値 `-3`: 上限超過（50超え）

#### 処理フロー
1. 商品探索。
2. 新在庫 `stock + delta` を試算。
3. `0 <= new_stock <= max_stock` を満たすか確認。
4. 問題なければ在庫を更新。

---

### replace_product()
#### 目的
既存商品の入替、または新商品の追加を行う。

#### シグネチャ例
```c
int replace_product(Product products[], int* product_count, int max_products,
					int target_product_id, const Product* new_product, int add_if_not_found);
```

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: 配列上限超過
- 戻り値 `-3`: 対象商品なし（追加不可設定時）

#### 処理フロー
1. 対象ID探索。
2. 見つかれば該当要素を `new_product` で置換。
3. 見つからず追加許可なら末尾追加。
4. 変更後 `save_product_data()` を呼び永続化。
5. 操作ログを記録。

## 3.5 セキュリティ・保守

### validate_operation()
#### 目的
不正入力・不正遷移・危険操作を事前に検知する。

#### シグネチャ例
```c
int validate_operation(const char* operation, int product_id, int money, const RuntimeState* state);
```

#### チェック観点
- 存在しない商品IDアクセス
- 金額の負値/異常高額
- 状態遷移不正（未選択購入、二重購入など）
- 連続異常操作（閾値超過時にWARN/ERROR）

#### 出力
- 戻り値 `0`: 問題なし
- 戻り値 `-1`: 入力不正
- 戻り値 `-2`: 状態遷移不正
- 戻り値 `-3`: セキュリティ閾値超過

#### 処理フロー
1. 操作種別ごとに検証ルールを分岐。
2. 失敗時は `log_operation(ERROR, ...)` を呼ぶ。
3. 成功時は必要に応じて `INFO` ログ。

---

### backup_data()
#### 目的
障害時復旧のため、商品データ・売上・ログをバックアップする。

#### シグネチャ例
```c
int backup_data(const char* src_dir, const char* backup_dir, const char* timestamp);
```

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: ディレクトリ作成失敗
- 戻り値 `-3`: コピー失敗

#### 処理フロー
1. バックアップ先 `backup_YYYYMMDD_HHMMSS` を生成。
2. `products.csv`, `sales.csv`, `operation.log` を順次コピー。
3. すべて成功なら完了ログを記録。

## 3.6 その他（起動/終了）

### load_product_data()
#### 目的
起動時に商品CSVを読み込み、メモリ上の商品配列を初期化する。

#### シグネチャ例
```c
int load_product_data(const char* file_path, Product products[], int* product_count, int max_products);
```

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: ファイルオープン失敗
- 戻り値 `-3`: フォーマット不正
- 戻り値 `-4`: 商品数上限超過

#### 処理フロー
1. ファイルを読み込み。
2. ヘッダをスキップし、1行ずつパース。
3. 値検証（価格>=0, 在庫範囲, max_stock<=50）。
4. 配列へ格納し件数更新。
5. 起動ログを出力。

---

### save_product_data()
#### 目的
在庫変更・入替後の商品配列をCSVへ保存する。

#### シグネチャ例
```c
int save_product_data(const char* file_path, const Product products[], int product_count);
```

#### 出力
- 戻り値 `0`: 正常
- 戻り値 `-1`: 引数不正
- 戻り値 `-2`: ファイルオープン失敗
- 戻り値 `-3`: 書き込み失敗

#### 処理フロー
1. 一時ファイルへ全件書き込み。
2. 書き込み成功後に本ファイルへ置換（原子的更新）。
3. 失敗時は旧ファイルを維持。
4. 保存結果をログ記録。

## 4. 全体処理フロー（運用シナリオ）

### 4.1 起動
1. `load_product_data()`
2. `backup_data()`（任意: 日次/起動時）
3. `log_operation(INFO, "startup", ... )`

### 4.2 通常購入
1. `insert_money()` を繰り返し実行。
2. `select_product()` で商品選択。
3. `validate_operation("purchase", ...)`
4. `purchase_product()`
5. 成功時: 在庫更新、売上記録、お釣り返却。
6. 失敗時: エラー表示とログ記録（必要なら `return_money()`）。

### 4.3 キャンセル
1. `validate_operation("cancel", ...)`
2. `return_money(state, state->inserted_money)`
3. `log_operation(INFO, "cancel", ... )`

### 4.4 保守（補充・入替）
1. 管理モード認証（将来拡張）
2. `update_stock()` または `replace_product()`
3. `save_product_data()`
4. `backup_data()`
5. `log_operation(INFO, "maintenance", ... )`

## 5. エラーコード方針
- 共通: `0` は正常。
- `-1` 系: 引数不正/対象なし。
- `-2` 系: 状態不整合/リソース失敗（在庫・ファイル等）。
- `-3` 以下: ドメイン固有異常（不足、上限超過、セキュリティ違反など）。

## 6. 実装時の注意点
- 金額は `int`（円単位）で扱い、浮動小数点を使わない。
- 文字列入力は長さ制限を必ず設ける。
- CSV書き込みは改行・カンマ混入を考慮してサニタイズする。
- 購入処理は「在庫更新と売上記録」を1トランザクションとして扱い、途中失敗時はロールバック方針を定義する。