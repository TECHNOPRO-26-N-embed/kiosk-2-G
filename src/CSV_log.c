#include <stdio.h>
#include <string.h>
#include <time.h>

/* F05 売上データ保存 */
void saveSalesData(int id, char name[], int price, int stock) {
    FILE *fp = fopen("sales.csv", "a");

    if (fp == NULL) {
        printf("売上ファイルの保存に失敗しました。\n");
        return;
    }

    // 日時取得
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm_info);

    // 日時を先頭にしてCSVに出力
    fprintf(fp, "%s,%d,%s,%d,%d\n", datetime, id, name, price, stock);
    fclose(fp);

    printf("売上データを保存しました。\n");
}