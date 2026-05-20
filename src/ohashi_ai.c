#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	int id;               // 商品ID（一意）
	char name[32];        // 商品名
	int price;            // 価格（円）
	int stock;            // 現在在庫
	int max_stock;        // 在庫上限（通常50）
	int is_active;        // 1: 販売中, 0: 販売停止
} Product;

typedef struct {
	int inserted_money;       // 投入金額合計
	int selected_product_id;  // 選択中商品ID（未選択は -1）
	int cancel_requested;     // キャンセル要求フラグ
} RuntimeState;

typedef struct {
	const char* operator_id;  // 操作者ID（一般利用者/管理者）
	const char* terminal_id;  // 端末ID
	const char* timestamp;    // ISO8601 文字列
} OperationContext;

void print_main_menu(void) {
	printf("\n====================================\n");
	printf("      自動販売機システム メニュー\n");
	printf("====================================\n");
	printf("1. 商品選択・購入\n");
	printf("2. 金額管理・計算\n");
	printf("3. データ記録・ログ\n");
	printf("4. 在庫管理\n");
	printf("9. 終了\n");
	printf("------------------------------------\n");
}

int main(void) {
	char input[64];
	int running = 1;
	RuntimeState state = {0, -1, 0};
	OperationContext ctx = {"user", "terminal01", "2026-05-20T00:00:00"};
	Product products[] = {
		{1, "コーラ", 120, 10, 50, 1},
		{2, "お茶", 110, 8, 50, 1}
	};
	int product_count = (int)(sizeof(products) / sizeof(products[0]));

	while (running) {
		char *end_ptr;
		long menu_no;

		print_main_menu();
		printf("現在の状態: 投入金額=%d円, 選択商品ID=%d\n",
			   state.inserted_money, state.selected_product_id);
		printf("メニュー番号を入力してください: ");

		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("\n入力の読み取りに失敗したため終了します。\n");
			break;
		}

		menu_no = strtol(input, &end_ptr, 10);

		while (*end_ptr != '\0') {
			if (!isspace((unsigned char)*end_ptr)) {
				break;
			}
			end_ptr++;
		}

		if (end_ptr == input || *end_ptr != '\0') {
			printf("入力エラー: 数字でメニュー番号を入力してください。\n");
			continue;
		}

		switch (menu_no) {
			case 1:
				printf("[遷移] 商品選択・購入処理を実行します。\n");
				printf("       (連携先: 商品選択・購入担当の関数群)\n");
				if (product_count > 0) {
					state.selected_product_id = products[0].id;
					printf("       仮選択商品IDを %d に設定しました。\n", state.selected_product_id);
				}
				break;
			case 2:
				printf("[遷移] 金額管理・計算処理を実行します。\n");
				printf("       (連携先: 金銭管理・決済担当の関数群)\n");
				state.inserted_money += 100;
				printf("       テスト投入: 100円 (合計 %d円)\n", state.inserted_money);
				break;
			case 3:
				printf("[遷移] データ記録・ログ処理を実行します。\n");
				printf("       (連携先: データ記録・ログ担当の関数群)\n");
				printf("       監査情報: operator=%s terminal=%s time=%s\n",
					   ctx.operator_id, ctx.terminal_id, ctx.timestamp);
				break;
			case 4:
				printf("[遷移] 在庫管理処理を実行します。\n");
				printf("       (連携先: 商品・在庫管理担当の関数群)\n");
				if (product_count > 0) {
					printf("       先頭商品: %s (在庫 %d/%d)\n",
						   products[0].name, products[0].stock, products[0].max_stock);
				}
				break;
			case 9:
				printf("システムを終了します。\n");
				running = 0;
				break;
			default:
				printf("入力エラー: 1, 2, 3, 4, 9 のいずれかを入力してください。\n");
				break;
		}
	}

	return 0;
}
