#include <stdio.h>

typedef struct {
    char name[30];
    int price;
    int num;
    int month;
    int tmp;    //冷たい0ホット1
    int sold;
} Drink;

int sum;

// 商品名(char name =30),単価(int price),数量(int num),合計金額(int sum),月(int month),気温(int tmp)

// コミョングン
void InsertCoin(int *money){
    int insert =0;
    printf("お金入力 : ");
    scanf("%d", &insert);
    *money += insert;
    return;
}

void BuyDrink(Drink drinks[], int choice, int *money){
    printf("%d. %s商品を購入\n" , choice,drinks[choice-1].name);
    if(drinks[choice+1].price>*money){
        InsertCoin(money);
    }
    //drinksPrice(drinks, choice, money);


    // money -= drinks[choice-1].price;

    drinks[choice-1].sold += 1;
    drinks[choice-1].num -= 1;
    

    return;
}
//








int main() {
    Drink drinks[5] = {
        {"coke", 120, 5, 260518, 0, 0},
        {"水", 100, 5, 260518, 0, 0},
        {"おいお茶", 110, 5, 260518, 0, 0},
        {"ホットコーヒー", 140, 5, 260518, 1, 0},
        {"メロンソーダ", 150, 5, 260518, 0, 0}
    };

    int money = 0;
    int choice = 0;
    while(1){
        printf("\n==================================\n");
        printf("   自動販売機\n");
        printf("==================================\n");
        //printf("1. 商品を買う\n");
        for(int i = 0; i< 5; i++){
            printf("%d. %s を購入, 在庫 : %d, sold : %d\n",i+1, drinks[i].name, drinks[i].num, drinks[i].sold);
        }
        printf("0. 終了\n");
        scanf("%d", &choice);

        if(choice >= 1 && choice <=5){
            printf("購入\n");
            BuyDrink(drinks, choice, &money);
        }else if(choice==0){
            printf("0. 終了\n");
            break;
        }else if(choice==99){
            InsertCoin(&money);
        }else{

        }
    }
    return 0;

}