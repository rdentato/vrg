
#define VRGCLI

// re-define the internal messages for Japanese
// WARNING THIS IS ALL MACHINE-TL I do not claim to know Japanese!

#define VRG_STR_USAGE        "使い方"
#define VRG_STR_ERROR        "エラー"
#define VRG_STR_INVALID      "%T '%N' に対する無効な値 '%V'"
#define VRG_STR_INV_OPTION   "オプション"
#define VRG_STR_INV_ARGUMENT "引数"
#define VRG_STR_NO_ARGOPT    "オプション '%.*s' の引数が不足しています"
#define VRG_STR_NO_ARGUMENT  "引数 '%.*s' が不足しています"
#define VRG_STR_OPTIONS      "オプション"
#define VRG_STR_ARGUMENTS    "引数"

#include "vrg.h"

// CHeck if the string is an integer
int isint(char *arg)
{
  if (arg == NULL || *arg == '\0') return 0;
  for(;*arg; arg++) if (*arg < '0' || '9' < *arg) return 0;
  return 1;
}

// Check if the string is an integer greater than n
int isgreaterthan(char *arg, int n)
{
  if (arg == NULL || *arg == '\0') return 0;
  if (atoi(arg)<= n) return 0;
  return 1;
}

int isbool(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  if (strcmp("はい",arg) == 0) return 1;
  if (strcmp("いいえ",arg) == 0) return 1;
  return 0;
}

// check if the specified file exists and is readable.
int isfile(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  FILE *f = fopen(arg,"rb");
  if (f == NULL) return 0;
  fclose(f);
  return 1;
}

int main (int argc, char *argv[])
{

  vrgcli("バージョン: 1.3RC\n  vrgli 関数 の デモ") {

    vrgarg("モデル\tモデル の ファイル 名",isfile) {
      printf("モデル: '%s'\n",vrgarg);
    }

    vrgarg("-h, --ヘルプ\tこ の ヘルプ を表示") {
      vrgusage();
    }

    vrgarg("-n, --数-光線 数\t光線 の 数 (正 の 整数)", isgreaterthan, 0) {
      printf("Rays: '%s'\n",vrgarg);
    }

    vrgarg("-t, --なぞる [はい/いいえ] \t\t輪郭をなぞる (ブーリアン)", isbool) {
      printf("なぞる '%s'\n",vrgarg);
    }

    vrgarg("--練習") {
      printf("練習 '%s'\n",vrgarg);
    }

    vrgarg("-r\t再びなぞる") {
      printf("再びなぞる '%s'\n",vrgarg);
    }

    vrgarg("[出力ファイル名]\t\t生成するファイル") {
      printf("出力ファイル名: '%s'\n",vrgarg);
    }

    vrgarg() {
      if (vrgarg[0] == '-') vrgerror("未知 の オプション '%s'\n",vrgarg);
      else printf("追加の引数 %s\n",vrgarg);
    }

  }

  fprintf(stderr,"残りの引数: %d\n", argc - vrgargn);

}
