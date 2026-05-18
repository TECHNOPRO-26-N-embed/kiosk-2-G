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
void drinksPrice(Drink drinks[], int choice, int *money){
    int j;
    j= *money  -   drinks[choice-1].price ;
            if(j>0){
            printf("お釣りは%d円です\n",j);
<<<<<<< HEAD
            *money = 0;
            j=0;
            }else{
                printf("お釣りはありません\n");
                *money = 0;
            
=======
            *money -= drinks[choice-1].price;
            }else{
                printf("お釣りはありません\n");
                printf("現在のお金 : %d\n",*money);
>>>>>>> 790c3afbea108da93d05681ee622a55aab27d152
            }
    return;
}
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
        printf("お金が足りないです、現在のお金 : %d\n",*money);
        InsertCoin(money);
    }
    drinksPrice(drinks, choice, money);


    // money -= drinks[choice-1].price;

    drinks[choice-1].sold += 1;
    drinks[choice-1].num -= 1;
    

    return;
}

void FileCsv(Drink drinks[]){
    FILE *fp = fopen("DrinkSum.csv","w");
// 商品名(char name =30),単価(int price),数量(int num),合計金額(int sum),月(int month),気温(int tmp)
sum = 0;
    fprintf(fp,"商品名, 単価, 数量, 月, 気温");
    for(int i =0; i<5; i++){
        fprintf(fp,"%s, %d, %d, %d, %d\n",drinks[i].name, drinks[i].price, drinks[i].num, drinks[i].month);
        sum += drinks[i].price * drinks[i].sold;
    }
        fprintf(fp,"合計金額 : %d\n", sum);
    fclose(fp);
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
            printf("%d. %s を購入,　価格 : %d, 在庫 : %d, sold : %d\n",i+1, drinks[i].name,
                drinks[i].price, drinks[i].num, drinks[i].sold);
        }

        // 萩原の在庫切れ表示
        for(int j = 0; j < 5; j++){
            if(drinks[j].num == 0){
                printf("%d. %s は在庫切れです\n", j+1, drinks[j].name);
            }
        }

        printf("98. 保存\n");
        printf("99. お金入力\n");

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
<<<<<<< HEAD
        }else if (choice == 98){
              int k=0;
              int l=0;
              int o=0;
            while (k <= 4){
                
                o=drinks[k].price*drinks[k].sold;
                l=l+o;
               
                k++;
                
            }
             printf("売上合計は%d円です\n",l);
            
=======
        }else if(choice ==98){
            FileCsv(drinks);
>>>>>>> 790c3afbea108da93d05681ee622a55aab27d152
        }else{

        }
    }
    return 0;

}