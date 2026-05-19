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

//








int main() {
    Drink drinks[1] = {
        {"coke", 120, 5, 260518, 0, 0}
    };

    int money = 0;
    int choice = 0;
    while(1){
        printf("\n==================================\n");
        printf("   自動販売機\n");
        printf("==================================\n");
        printf("1. 商品を買う\n");
        printf("0. 終了\n");
        scanf("%d", &choice);

        if(choice == 1){
            printf("購入");
        }else if(choice==0){
            printf("9. 終了\n");
            break;
        }else{

        }
    }
    return 0;

}