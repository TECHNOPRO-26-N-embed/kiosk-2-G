#include <stdio.h>
#include <string.h>

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