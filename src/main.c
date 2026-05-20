#include <stdio.h>

#define ITEM_COUNT 5

typedef struct {
    char name[30];
    int price;
    int stock;
    int sold;
} Drink;


/* 入力バッファ削除 */
void clearBuffer(){
    while(getchar() != '\n');
}


/* お金投入 */
void insertMoney(int *money){

    int insert;

    printf("投入金額 : ");

    if(scanf("%d", &insert) != 1){
        printf("入力エラー\n");
        clearBuffer();
        return;
    }

    if(insert <= 0){
        printf("正しい金額を入力してください\n");
        return;
    }

    *money += insert;

    printf("現在金額 : %d円\n", *money);
}


/* メニュー表示 */
void showMenu(Drink drinks[], int money){

    printf("\n========================\n");
    printf("      自動販売機\n");
    printf("========================\n");

    printf("現在金額 : %d円\n\n", money);

    for(int i = 0; i < ITEM_COUNT; i++){

        printf("%d. %-15s %3d円 在庫:%d",
               i + 1,
               drinks[i].name,
               drinks[i].price,
               drinks[i].stock);

        if(drinks[i].stock == 0){
            printf(" [売切]");
        }

        printf("\n");
    }

    printf("\n96. 返金\n");
    printf("97. 売上確認\n");
    printf("98. CSV保存\n");
    printf("99. お金投入\n");
    printf("0. 終了\n");

    printf("\n番号入力 : ");
}


/* 商品購入 */
void buyDrink(Drink drinks[], int choice, int *money){

    Drink *d = &drinks[choice - 1];

    if(d->stock == 0){
        printf("%s は売り切れです\n", d->name);
        return;
    }

    if(*money < d->price){
        printf("お金が足りません\n");
        return;
    }

    *money -= d->price;

    d->stock--;
    d->sold++;

    printf("%s を購入しました\n", d->name);
    printf("残金 : %d円\n", *money);
}


/* 売上計算 */
int calcSales(Drink drinks[]){

    int total = 0;

    for(int i = 0; i < ITEM_COUNT; i++){
        total += drinks[i].price * drinks[i].sold;
    }

    return total;
}


/* CSV保存 */
void saveCSV(Drink drinks[]){

    FILE *fp = fopen("DrinkSum.csv", "w");

    if(fp == NULL){
        printf("保存失敗\n");
        return;
    }

    fprintf(fp, "商品名,価格,在庫,販売数\n");

    for(int i = 0; i < ITEM_COUNT; i++){

        fprintf(fp,
                "%s,%d,%d,%d\n",
                drinks[i].name,
                drinks[i].price,
                drinks[i].stock,
                drinks[i].sold);
    }

    fprintf(fp, "売上,%d\n", calcSales(drinks));

    fclose(fp);

    printf("CSV保存完了\n");
}


/* メイン */
int main(){

    Drink drinks[ITEM_COUNT] = {
        {"コーラ",120,5,0},
        {"水",100,5,0},
        {"お茶",110,5,0},
        {"コーヒー",140,5,0},
        {"メロンソーダ",150,5,0}
    };

    int money = 0;
    int choice;

    while(1){

        showMenu(drinks, money);

        if(scanf("%d", &choice) != 1){
            printf("数字を入力してください\n");
            clearBuffer();
            continue;
        }

        if(choice >= 1 && choice <= ITEM_COUNT){

            buyDrink(drinks, choice, &money);

        }else if(choice == 99){

            insertMoney(&money);

        }else if(choice == 98){

            saveCSV(drinks);

        }else if(choice == 97){

            printf("現在売上 : %d円\n", calcSales(drinks));

        }else if(choice == 96){

            if(money == 0){
                printf("返金するお金がありません\n");
            }else{
                printf("%d円返金しました\n", money);
                money = 0;
            }

        }else if(choice == 0){

            printf("終了します\n");
            break;

        }else{

            printf("無効な番号です\n");
        }
    }

    return 0;
}