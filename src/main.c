#include <stdio.h>
#include <string.h>

#define MAX_PRODUCTS 20
#define PRODUCT_FILE "products.csv"
#define SALES_FILE "sales.csv"
#define LOG_FILE "operation.log"
#define BACKUP_FILE "products_backup.csv"

typedef struct {
    int id;
    char name[32];
    int price;
    int stock;
    int max_stock;
    int is_active;
} Product;

typedef struct {
    int inserted_money;
    int selected_product_id;
    int cancel_requested;
} RuntimeState;

typedef struct {
    const char* operator_id;
    const char* terminal_id;
    const char* timestamp;
} OperationContext;

/* 商品選択・購入関連 */
int select_product(const Product products[], int product_count,
                   int input_id, int* selected_product_id);

int purchase_product(Product products[], int product_count,
                     RuntimeState* state, int selected_product_id,
                     int* change, const OperationContext* ctx);

/* 金額管理・計算 */
int insert_money(RuntimeState* state, int money,
                 const OperationContext* ctx);

int calculate_change(int inserted_money, int price,
                     int* change);

int return_money(RuntimeState* state, int amount_to_return,
                 const OperationContext* ctx);

/* データ記録・ログ */
int record_sales_data(const Product* product,
                      int inserted_money,
                      int change,
                      const OperationContext* ctx);

int log_operation(const char* message,
                  const OperationContext* ctx);

/* 在庫管理 */
int check_stock(const Product products[], int product_count,
                int product_id);

int update_stock(Product products[], int product_count,
                 int product_id, int stock_delta);

int replace_product(Product products[], int product_count,
                    int product_id,
                    const char* new_name,
                    int new_price,
                    int new_stock,
                    int new_max_stock);

/* セキュリティ・保守 */
int validate_operation(const RuntimeState* state,
                       const OperationContext* ctx);

int backup_data(const char* source_file,
                const char* backup_file);

/* その他 */
int load_product_data(Product products[],
                      int max_products,
                      int* product_count,
                      const char* filename);

int save_product_data(const Product products[],
                      int product_count,
                      const char* filename);

/* 画面表示 */
void print_products(const Product products[], int product_count) {
    printf("\n=== 商品一覧 ===\n");

    for (int i = 0; i < product_count; i++) {
        if (products[i].is_active) {
            printf("%d. %s / %d円 / 在庫: %d\n",
                   products[i].id,
                   products[i].name,
                   products[i].price,
                   products[i].stock);
        }
    }
}

void print_main_menu(void) {
    printf("\n=== 自動販売機メニュー ===\n");
    printf("1. 商品を購入\n");
    printf("2. 投入金額を返金\n");
    printf("3. 管理メニュー\n");
    printf("4. 終了\n");
    printf("選択してください: ");
}

void print_admin_menu(void) {
    printf("\n=== 管理メニュー ===\n");
    printf("1. 在庫を更新\n");
    printf("2. 商品を入れ替え\n");
    printf("3. 商品データを保存\n");
    printf("4. バックアップ作成\n");
    printf("0. メインメニューへ戻る\n");
    printf("選択してください: ");
}

int main(void) {
    Product products[MAX_PRODUCTS] = {
        {1, "水",              120, 10, 50, 1},
        {2, "コーラ",          150,  8, 50, 1},
        {3, "コーヒー",        120,  5, 50, 1},
        {4, "緑茶",            130,  7, 50, 1},
        {5, "ジュース",        160,  6, 50, 1},
        {6, "エナジードリンク", 200,  4, 50, 1}
    };

    int product_count = 6;

    RuntimeState state = {
        .inserted_money = 0,
        .selected_product_id = -1,
        .cancel_requested = 0
    };

    OperationContext ctx = {
        .operator_id = "user",
        .terminal_id = "VM-001",
        .timestamp = "2026-05-20 00:00:00"
    };

    int running = 1;

    load_product_data(products, MAX_PRODUCTS, &product_count, PRODUCT_FILE);
    log_operation("アプリケーションを起動しました。", &ctx);

    while (running) {
        int menu;

        printf("\n現在の投入金額: %d円\n", state.inserted_money);
        print_main_menu();

        if (scanf("%d", &menu) != 1) {
            printf("入力が正しくありません。\n");
            while (getchar() != '\n');
            continue;
        }

        switch (menu) {
        case 1: {
            int input_id;
            int money;
            int change = 0;
            int result;

            print_products(products, product_count);

            printf("購入する商品のIDを入力してください: ");
            scanf("%d", &input_id);

            result = select_product(
                products,
                product_count,
                input_id,
                &state.selected_product_id
            );

            if (result != 0) {
                printf("商品選択に失敗しました。\n");
                log_operation("商品選択に失敗しました。", &ctx);
                break;
            }

            printf("投入する金額を入力してください: ");
            scanf("%d", &money);

            result = insert_money(&state, money, &ctx);

            if (result != 0) {
                printf("金額投入に失敗しました。\n");
                log_operation("金額投入に失敗しました。", &ctx);
                break;
            }

            result = validate_operation(&state, &ctx);

            if (result != 0) {
                printf("不正な操作が検出されました。\n");
                log_operation("不正な操作が検出されました。", &ctx);
                break;
            }

            result = purchase_product(
                products,
                product_count,
                &state,
                state.selected_product_id,
                &change,
                &ctx
            );

            if (result == 0) {
                printf("購入が完了しました。\n");
                printf("お釣り: %d円\n", change);

                save_product_data(products, product_count, PRODUCT_FILE);
                log_operation("商品購入が完了しました。", &ctx);

                state.selected_product_id = -1;
            } else {
                printf("購入に失敗しました。\n");
                log_operation("商品購入に失敗しました。", &ctx);
            }

            break;
        }

        case 2: {
            int refund = state.inserted_money;

            if (refund <= 0) {
                printf("返金する金額がありません。\n");
                break;
            }

            if (return_money(&state, refund, &ctx) == 0) {
                printf("%d円を返金しました。\n", refund);
                log_operation("投入金額を返金しました。", &ctx);
            } else {
                printf("返金に失敗しました。\n");
                log_operation("返金に失敗しました。", &ctx);
            }

            state.selected_product_id = -1;
            break;
        }

        case 3: {
            int admin_running = 1;

            while (admin_running) {
                int admin_menu;

                print_admin_menu();

                if (scanf("%d", &admin_menu) != 1) {
                    printf("入力が正しくありません。\n");
                    while (getchar() != '\n');
                    continue;
                }

                switch (admin_menu) {
                case 1: {
                    int product_id;
                    int stock_delta;

                    printf("在庫を更新する商品IDを入力してください: ");
                    scanf("%d", &product_id);

                    printf("在庫の増減数を入力してください: ");
                    scanf("%d", &stock_delta);

                    if (update_stock(products, product_count, product_id, stock_delta) == 0) {
                        printf("在庫を更新しました。\n");
                        save_product_data(products, product_count, PRODUCT_FILE);
                        log_operation("在庫を更新しました。", &ctx);
                    } else {
                        printf("在庫更新に失敗しました。\n");
                        log_operation("在庫更新に失敗しました。", &ctx);
                    }

                    break;
                }

                case 2: {
                    int product_id;
                    char new_name[32];
                    int new_price;
                    int new_stock;
                    int new_max_stock;

                    printf("入れ替える商品IDを入力してください: ");
                    scanf("%d", &product_id);

                    printf("新しい商品名を入力してください: ");
                    scanf("%31s", new_name);

                    printf("新しい価格を入力してください: ");
                    scanf("%d", &new_price);

                    printf("新しい在庫数を入力してください: ");
                    scanf("%d", &new_stock);

                    printf("新しい最大在庫数を入力してください: ");
                    scanf("%d", &new_max_stock);

                    if (replace_product(
                            products,
                            product_count,
                            product_id,
                            new_name,
                            new_price,
                            new_stock,
                            new_max_stock
                        ) == 0) {
                        printf("商品情報を更新しました。\n");
                        save_product_data(products, product_count, PRODUCT_FILE);
                        log_operation("商品情報を更新しました。", &ctx);
                    } else {
                        printf("商品情報の更新に失敗しました。\n");
                        log_operation("商品情報の更新に失敗しました。", &ctx);
                    }

                    break;
                }

                case 3:
                    if (save_product_data(products, product_count, PRODUCT_FILE) == 0) {
                        printf("商品データを保存しました。\n");
                        log_operation("商品データを保存しました。", &ctx);
                    } else {
                        printf("商品データの保存に失敗しました。\n");
                    }
                    break;

                case 4:
                    if (backup_data(PRODUCT_FILE, BACKUP_FILE) == 0) {
                        printf("バックアップを作成しました。\n");
                        log_operation("バックアップを作成しました。", &ctx);
                    } else {
                        printf("バックアップ作成に失敗しました。\n");
                        log_operation("バックアップ作成に失敗しました。", &ctx);
                    }
                    break;

                case 0:
                    admin_running = 0;
                    break;

                default:
                    printf("存在しないメニューです。\n");
                    break;
                }
            }

            break;
        }

        case 4:
            if (state.inserted_money > 0) {
                return_money(&state, state.inserted_money, &ctx);
                printf("残高を返金しました。\n");
            }

            save_product_data(products, product_count, PRODUCT_FILE);
            log_operation("アプリケーションを終了しました。", &ctx);

            running = 0;
            break;

        default:
            printf("存在しないメニューです。\n");
            break;
        }
    }

    printf("プログラムを終了します。\n");

    return 0;
}