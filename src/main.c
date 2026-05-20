#include <stdio.h>
#include <string.h>

#define PRODUCT_COUNT 3

typedef struct {
    int id;
    char name[30];
    int price;
    int stock;
} Product;

/* 関数宣言 */
void showProducts();
int selectProduct();
void insertMoney();
void buyProduct();
void returnChange();
void saveSalesData(int id, char name[], int price, int stock);
void manageStock();

/* 商品データ */
Product products[PRODUCT_COUNT] = {
    {1, "コーラ", 120, 5},
    {2, "水", 80, 10},
    {3, "コーヒー", 150, 3}
};

int selectedIndex = -1;
int insertedMoney = 0;

/* F01 商品選択 */
int selectProduct() {
    showProducts();
    int id;

    printf("\n商品番号を入力してください: ");
    scanf("%d", &id);

    for (int i = 0; i < PRODUCT_COUNT; i++) {
        if (products[i].id == id) {
            selectedIndex = i;
            printf("%s が選択されました。\n", products[i].name);
            return 1;
        }
    }

    printf("存在しない商品です。\n");
    return 0;
}

/* F02 金額投入 */
void insertMoney() {
    int money;

    printf("\n投入金額を入力してください: ");
    scanf("%d", &money);

    if (money > 0) {
        insertedMoney += money;
        printf("現在の投入金額: %d円\n", insertedMoney);
    } else {
        printf("正しい金額を入力してください。\n");
    }
}

/* F03 商品購入 */
void buyProduct() {
    if (selectedIndex == -1) {
        printf("先に商品を選択してください。\n");
        return;
    }

    Product *p = &products[selectedIndex];

    if (p->stock <= 0) {
        printf("在庫がありません。\n");
        return;
    }

    if (insertedMoney < p->price) {
        printf("金額が不足しています。不足金額: %d円\n", p->price - insertedMoney);
        return;
    }

    insertedMoney -= p->price;
    p->stock--;

    printf("%s の購入が完了しました。\n", p->name);
    printf("残りの残高: %d円\n", insertedMoney);

    saveSalesData(p->id, p->name, p->price, p->stock);
}

/* F04 お釣り計算・返却 */
void returnChange() {
    printf("返却金額: %d円\n", insertedMoney);
    insertedMoney = 0;
   
}

/* F05 売上データ保存 */
void saveSalesData(int id, char name[], int price, int stock) {
    FILE *fp = fopen("sales.csv", "a");

    if (fp == NULL) {
        printf("売上ファイルの保存に失敗しました。\n");
        return;
    }

    fprintf(fp, "%d,%s,%d,%d\n", id, name, price, stock);
    fclose(fp);

    printf("売上データを保存しました。\n");
}

/* F06 在庫管理 */
void manageStock() {
    int id, amount;

    printf("\n在庫を追加する商品番号を入力してください: ");
    scanf("%d", &id);

    printf("追加する数量を入力してください: ");
    scanf("%d", &amount);

    for (int i = 0; i < PRODUCT_COUNT; i++) {
        if (products[i].id == id) {
            products[i].stock += amount;
            printf("%s の在庫は %d 個になりました。\n",
                   products[i].name, products[i].stock);
            return;
        }
    }

    printf("存在しない商品です。\n");
}

/* 商品一覧表示 */
void showProducts() {
    printf("\n===== 商品一覧 =====\n");

    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d. %s / %d円 / 在庫 %d個\n",
               products[i].id,
               products[i].name,
               products[i].price,
               products[i].stock);
    }
}

/* 基本ループ */
int main() {
    int menu;

    while (1) {
        printf("\n===== 自動販売機メニュー =====\n");
        printf("1. 商品一覧を見る\n");
        printf("2. 商品を選択する\n");
        printf("3. 金額を投入する\n");
        printf("4. 商品を購入する\n");
        printf("5. お金を返却する / キャンセル\n");
        printf("6. 在庫を管理する\n");
        printf("0. 終了する\n");
        printf("メニューを選択してください: ");

        if (scanf("%d", &menu) != 1) {
            printf("入力エラーが発生しました。正しい数字を入力してください。\n");
            // 入力バッファをクリア
            while (getchar() != '\n');
            continue;
        }

        if (menu == 1) {
            showProducts();
        } else if (menu == 2) {
            selectProduct();
        } else if (menu == 3) {
            insertMoney();
        } else if (menu == 4) {
            buyProduct();
        } else if (menu == 5) {
            returnChange();
        } else if (menu == 6) {
            manageStock();
        } else if (menu == 0) {
            returnChange();
            printf("プログラムを終了します。\n");
            break;
        } else {
            printf("正しいメニュー番号を入力してください。\n");
        }
    }

    return 0;
}