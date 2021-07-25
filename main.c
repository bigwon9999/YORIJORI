#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <math.h>
#include <time.h>
#include <conio.h>
/*
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC 색깔 연두색으로 변경
애들아 여기서 함수 인자에 있는 숫자 바꾸면 콘솔 글씨 색깔 바꿀 수 있어! 콘솔 색상표는
구글에 콘솔 색상표라고 검색하면 쫙 나온당,, 그리고 그 콘솔색상표에 배경이 검은색이랑 아닌거랑 다른거더라구,,,
배경이 검은색인 색상표를 사용하작,,,!
*/

//방향키, 스페이스 정의 (PC 움직임 제어에 필요)
#define UP 72
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
//아이템 사용 키 정의
#define ZZ 44 
#define XX 45 //값이 자꾸 겹쳐서 이렇게 하쟈,,,ㅎ
#define CC 46

//맵 전체 틀 크기 정의
#define GBOARD_HEIGHT 40
#define GBOARD_WIDTH 140
//게임 창 크기 정의
#define MAP_HEIGHT 40
#define MAP_WIDTH 51//원래 x좌표는 102

typedef struct food//모양, 순서에 맞는 레시피 구조체
{
   char *ch[6];
   char *name[5];
}food;

typedef struct nofood//조건에 맞지 않는 레시피 구조체
{
   char *ch[6];
}nofood;

COORD curPos;
COORD pc;//pc의 좌표
int gameBoardInfo[MAP_HEIGHT+1][MAP_WIDTH+1]; //PC: 123 / 0: 빈공간 / 1: 막힌 공간(맵경계, 나무...) / 100~104: 맞는 재료(랜덤으로 배치하고 Info 정보 저장) / -100~-104: 틀린 재료
int Speed;
int cnt; //PC가 재료를 몇개 먹었는지! 스테이지 넘어갈때 마다 0으로 초기화해줘야함
int id;
int score=0;
char *plname;

food recipe[10];//맞는 레시피 배열
nofood norecipe[10];//맞지 않는 레시피 배열
nofood myrecipe; //PC가 먹은 재료 저장하는 변수

//////////////////////// 전역 변수 선언 끝!!!!!!

void setcolor(int color, int bgcolor)// 글자 색(글자색, 글자 배경색) 
{
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((bgcolor&0xf)<<4)|(color&0xf));
}

void RemoveCursor(void)   //커서 깜박임 제거
{
   CONSOLE_CURSOR_INFO curInfo;
   GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
   curInfo.bVisible=0;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void SetCurrentCursorPos(int x, int y) //커서포지션set
{
   COORD pos={ x, y };
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void) //커서포지션get
{
   COORD curPoint;
   CONSOLE_SCREEN_BUFFER_INFO curInfo;
   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
   curPoint.X=curInfo.dwCursorPosition.X;
   curPoint.Y=curInfo.dwCursorPosition.Y;

   return curPoint;
}

void printInfo()
{
   int x, y;

   SetCurrentCursorPos(0, 0);

   for (y=0; y<=MAP_HEIGHT; y++)
   {
      for (x=0; x<=MAP_WIDTH; x++)
      {
         if (gameBoardInfo[y][x]==0)
         {
            printf("  ");
            continue;
         }
         printf("%d ", gameBoardInfo[y][x]);
      }
      printf("\n");
   }
}

void Bottari_board()	// 보따리 이름 지어주기 위해서!
{
   SetCurrentCursorPos(116, 4);
   printf("%s 보따리", plname);
}

void Item_board()	// 아이템 창 표현
{
	// 아이템1-타
   SetCurrentCursorPos(113, 7);
   printf("타");
   SetCurrentCursorPos(113, 9);
   printf("X 2");	// 임시

   // 아이템2-컨
   SetCurrentCursorPos(121, 7);
   printf("컨");
   SetCurrentCursorPos(121, 9);
   printf("X 1");	// 임시

   // 아이템3-엄
   SetCurrentCursorPos(129, 7);
   printf("엄");
   SetCurrentCursorPos(129, 9);
   printf("X 3");	// 임시
}

void Heart_board()	// 생명 창 표현
{
	SetCurrentCursorPos(110, 12);
	printf("♥ ♡ ♡");	// 임시
}

void drawBoard() // 콘솔창 그리기
{//┌┐┘└ -> 굵기 맞는 거로!!
   int x, y;

   for (y=0; y<=MAP_HEIGHT; y++)
   {
      gameBoardInfo[y][0]=1;
      gameBoardInfo[y][MAP_WIDTH]=1;
   }

   for (x=0; x<=MAP_WIDTH; x++)
   {
      gameBoardInfo[0][x]=1;
      gameBoardInfo[MAP_HEIGHT][x]=1;
   }

   // 맨 왼쪽
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(0, y);
      if (y==GBOARD_HEIGHT)
      {
         printf("└");
      }
      else if (y==0)
      {
         printf("┌");
      }
      else
      {
         printf("│");
      }
   }

   // 게임 화면과 상황 안내 화면과의 경계
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(102, y);
      printf("│");
   }

   // 맨 오른쪽
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(140, y);
      if (y==GBOARD_HEIGHT)
      {
         printf("┘");
      }
      else if (y==0)
      {
         printf("┐");
      }
      else
      {
         printf("│");
      }
   }

   // 맨 위
   for (x=2; x<=138; x+=2)
   {
      SetCurrentCursorPos(x, 0);
      printf("─");
   }

   // 맨 아래
   for (x=2; x<=138; x+=2)
   {
      SetCurrentCursorPos(x, 40);
      printf("─");
   }

   /////// 보따리

   // 왼쪽
   for (y=4; y<=14; y++)
   {
      SetCurrentCursorPos(106, y);
      if (y==14)
      {
         printf("└");
      }
      else if (y==4)
      {
         printf("┌");
      }
      else
      {
         printf("│");
      }
   }

   // 오른쪽
   for (y=4; y<=14; y++)
   {
      SetCurrentCursorPos(136, y);
      if (y==14)
      {
         printf("┘");
      }
      else if (y==4)
      {
         printf("┐");
      }
      else
      {
         printf("│");
      }
   }

   // 위-1
   for (x=108; x<=110; x+=2)
   {
      SetCurrentCursorPos(x, 4);
      printf("─");
   }

   // 위-2
   for (x=132; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 4);
      printf("─");
   }

   // 아래
   for (x=108; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 14);
      printf("─");
   }


   /////// 아이템 창

   // 위
   for (x=111; x<=131; x+=2)
   {
      SetCurrentCursorPos(x, 6);
      printf("─");
   }

   // 왼쪽
   for (y=6; y<=10; y++)
   {
      SetCurrentCursorPos(109, y);
      if (y==10)
      {
         printf("└");
      }
      else if (y==6)
      {
         printf("┌");
      }
      else
      {
         printf("│");
      }
   }

   // 오른쪽
   for (y=6; y<=10; y++)
   {
      SetCurrentCursorPos(133, y);
      if (y==10)
      {
         printf("┘");
      }
      else if (y==6)
      {
         printf("┐");
      }
      else
      {
         printf("│");
      }
   }

   // 아래
   for (x=111; x<=131; x+=2)
   {
      SetCurrentCursorPos(x, 10);
      printf("─");
   }

   // 중간-1
   for (y=7; y<=9; y++)
   {
      SetCurrentCursorPos(117, y);
      printf("│");
   }

   // 중간-2
   for (y=7; y<=9; y++)
   {
      SetCurrentCursorPos(125, y);
      printf("│");
   }


   /////// 시간창
   // 왼쪽
   for (y=16; y<=28; y++)
   {
      SetCurrentCursorPos(106, y);
      if (y==28)
      {
         printf("└");
      }
      else if (y==16)
      {
         printf("┌");
      }
      else
      {
         printf("│");
      }
   }

   // 오른쪽
   for (y=16; y<=28; y++)
   {
      SetCurrentCursorPos(124, y);
      if (y==28)
      {
         printf("┘");
      }
      else if (y==16)
      {
         printf("┐");
      }
      else
      {
         printf("│");
      }
   }

   // 위
   for (x=108; x<=122; x+=2)
   {
      SetCurrentCursorPos(x, 16);
      printf("─");
   }

   // 아래
   for (x=108; x<=122; x+=2)
   {
      SetCurrentCursorPos(x, 28);
      printf("─");
   }

   /////// 재료 획득 상황
   // 왼쪽
   for (y=16; y<=36; y++)
   {
      SetCurrentCursorPos(128, y);
      if (y==36)
      {
         printf("└");
      }
      else if (y==16)
      {
         printf("┌");
      }
      else
      {
         printf("│");
      }
   }

   // 오른쪽
   for (y=16; y<=36; y++)
   {
      SetCurrentCursorPos(136, y);
      if (y==36)
      {
         printf("┘");
      }
      else if (y==16)
      {
         printf("┐");
      }
      else
      {
         printf("│");
      }
   }

   // 위
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 16);
      printf("─");
   }

   // 중간-1
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 20);
      printf("─");
   }

   // 중간-2
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 24);
      printf("─");
   }

   // 중간-3
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 28);
      printf("─");
   }

   // 중간-4
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 32);
      printf("─");
   }

   // 아래
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 36);
      printf("─");
   }
}

// 콘솔창을 중앙에 띄게 하기 위한 함수
void center()
{
   HWND hWnd_console;
   HWND hWnd_desktop;
   RECT rect_console={ 0, };
   RECT rect_desktop={ 0, };
   int x, y;

   hWnd_console=GetConsoleWindow();
   hWnd_desktop=GetDesktopWindow();

   GetWindowRect(hWnd_desktop, &rect_desktop);
   GetWindowRect(hWnd_console, &rect_console);

   x=(rect_desktop.left+rect_desktop.right-rect_console.right+rect_console.left)/2;
   y=(rect_desktop.top+rect_desktop.bottom-rect_console.bottom+rect_console.top)/2;

   SetWindowPos(hWnd_console, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

// 뭔진 잘 모르지만 x,y 좌표 위치로 이동하는거 같은 함수
void xyconsole(int x, int y)
{
   HWND hWnd_console;

   hWnd_console=GetConsoleWindow();
   SetWindowPos(hWnd_console, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}
void ScorePrint()
{
   SetCurrentCursorPos(108,30); //-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10
   printf("                            ");
   SetCurrentCursorPos(108, 31);
   printf("                            ");
   SetCurrentCursorPos(108, 32);
   printf("                            ");
   SetCurrentCursorPos(108, 33);
   printf("                            ");
   SetCurrentCursorPos(108, 34);
   printf("                            ");
   SetCurrentCursorPos(108, 35);
   printf("                            ");
   //printf("%2d",score);3줄더
   SetCurrentCursorPos(108, 30);
   switch(score)
   {   
   case -10:
      printf("         ■   ■■■■■      ");
      SetCurrentCursorPos(108, 31);
      printf("     ■■■   ■      ■      ");
       SetCurrentCursorPos(108, 32);
      printf("■■     ■   ■      ■      ");
      SetCurrentCursorPos(108, 33);
      printf("         ■   ■      ■      ");
      SetCurrentCursorPos(108, 34);
      printf("      ■■■  ■■■■■      ");
      break;
   case 10:
      printf("     ■      ■■■■■      ");
      SetCurrentCursorPos(108, 31);
      printf(" ■■■      ■      ■      ");
       SetCurrentCursorPos(108, 32);
      printf("     ■      ■      ■      ");
      SetCurrentCursorPos(108, 33);
      printf("     ■      ■      ■      ");
      SetCurrentCursorPos(108, 34);
      printf("  ■■■■   ■■■■■      ");
      break;
   case -9:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■     ■             ");
       SetCurrentCursorPos(108, 32);
      printf("■■■ ■■■■■             ");
      SetCurrentCursorPos(108, 33);
      printf("             ■             ");
      SetCurrentCursorPos(108, 34);
      printf("      ■     ■             ");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■             ");
      break;
   case 9:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 33);
      printf("              ■             ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■             ");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■             ");
      break;
   case -8:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("■■■  ■■■              ");
      SetCurrentCursorPos(108, 33);
      printf("      ■      ■             ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■             ");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■             ");
      break;
   case 8:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("       ■■■■             ");
      SetCurrentCursorPos(108, 33);
      printf("      ■      ■             ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■             ");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■             ");
      break;
   case -7:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("■■■       ■              ");
      SetCurrentCursorPos(108, 33);
      printf("            ■               ");
      SetCurrentCursorPos(108, 34);
      printf("           ■                ");
      SetCurrentCursorPos(108, 35);
      printf("          ■                 ");
      break;
   case 7:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108,31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("             ■              ");
      SetCurrentCursorPos(108, 33);
      printf("            ■               ");
      SetCurrentCursorPos(108,34);
      printf("           ■                ");
      SetCurrentCursorPos(108,35);
      printf("          ■                 ");
      break;
    case -6:
      printf("       ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("       ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("■■■ ■                    ");
      SetCurrentCursorPos(108, 33);
      printf("       ■■■■■              ");
      SetCurrentCursorPos(108, 34);
      printf("       ■      ■             ");
      SetCurrentCursorPos(108, 35);
      printf("       ■■■■■             ");
      break;
   case 6:
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■             ");
       SetCurrentCursorPos(108, 32);
      printf("      ■                    ");
      SetCurrentCursorPos(108, 33);
      printf("      ■■■■■             ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case -5:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("      ■       ");
       SetCurrentCursorPos(108, 32);
      printf("■■■ ■■■■");
      SetCurrentCursorPos(108, 33);
      printf("              ■");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case 5:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("      ■       ");
       SetCurrentCursorPos(108, 32);
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 33);
      printf("              ■");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case -4:
      printf("             ■■");
      SetCurrentCursorPos(108, 31);
      printf("           ■  ■");
       SetCurrentCursorPos(108, 32);
      printf("■■■   ■    ■");
      SetCurrentCursorPos(108, 33);
      printf("       ■■■■■■");
      SetCurrentCursorPos(108,34);
      printf("               ■");
      SetCurrentCursorPos(108, 35);
      printf("             ■■■");
      break;
   case 4:
      printf("             ■■");
      SetCurrentCursorPos(108, 31);
      printf("           ■  ■");
       SetCurrentCursorPos(108, 32);
      printf("         ■    ■");
      SetCurrentCursorPos(108, 33);
      printf("       ■■■■■■");
      SetCurrentCursorPos(108,34);
      printf("               ■");
      SetCurrentCursorPos(108, 35);
      printf("             ■■■");
      break;
   case -3:
      printf("      ■■■■■");
      SetCurrentCursorPos(108,31);
      printf("              ■");
       SetCurrentCursorPos(108,32);
      printf("■■■  ■■■■");
      SetCurrentCursorPos(108,33);
      printf("              ■");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case 3:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("              ■");
       SetCurrentCursorPos(108, 32);
      printf("        ■■■■");
      SetCurrentCursorPos(108, 33);
      printf("              ■");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case -2:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("              ■");
       SetCurrentCursorPos(108, 32);
      printf("■■■ ■■■■■");
      SetCurrentCursorPos(108, 33);
      printf("      ■      ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
   case 2:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("              ■");
       SetCurrentCursorPos(108, 32);
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 33);
      printf("      ■      ");
      SetCurrentCursorPos(108, 34);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 35);
      printf("      ■■■■■");
      break;
   case -1:
      printf("           ■");
      SetCurrentCursorPos(108, 31);
      printf("       ■■■");
       SetCurrentCursorPos(108, 32);
      printf("■■■     ■");
      SetCurrentCursorPos(108,33);
      printf("           ■ ");
      SetCurrentCursorPos(108,34);
      printf("       ■■■■");
      break;
   case 1:
      printf("           ■");
      SetCurrentCursorPos(108, 31);
      printf("       ■■■");
       SetCurrentCursorPos(108, 32);
      printf("           ■");
      SetCurrentCursorPos(108, 33);
      printf("           ■ ");
      SetCurrentCursorPos(108, 34);
      printf("        ■■■■");
      break;
   case 0:
      printf("      ■■■■■");
      SetCurrentCursorPos(108, 31);
      printf("      ■      ■");
       SetCurrentCursorPos(108, 32);
      printf("      ■      ■");
      SetCurrentCursorPos(108, 33);
      printf("      ■      ■");
      SetCurrentCursorPos(108,34);
      printf("      ■■■■■");
      break;
   }
   
}
void EatMyRecipe(int posX, int posY)//PC가 먹은 재료를 배열에 저장하는 함수 -> 충돌검사해서 true가 나오면 그 값의 절댓값이 100이상인지 조사 -> 이상이면 이 함수 호출
{
   int eat=gameBoardInfo[posY][posX];

   //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), "f");//색깔 흰색으로 변경

   if (eat>=100&&eat<=104) // 100, 101, 102, 103, 104순서대로 Info값 설정
   {
      myrecipe.ch[cnt]=recipe[id].ch[eat-100];
      gameBoardInfo[posY][posX]=0;
     if(cnt==eat-100)
     {
        score+=2;
        ScorePrint();
     }
     else if(cnt!=eat-100)
     {
        score+=1;
        ScorePrint();
     }
     cnt++;
   }
   else if (eat<=-100&&eat>=-104)//-100, -101, -102, -103, -104순서대로 Info값 설정
   {
      myrecipe.ch[cnt]=norecipe[id].ch[(eat*(-1))-100];
      gameBoardInfo[posY][posX]=0;
      score-=2;
     ScorePrint();
     cnt++;
   }
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
   SetCurrentCursorPos(132, 18);
   printf("%s", myrecipe.ch[0]);

   SetCurrentCursorPos(132, 22);
   printf("%s", myrecipe.ch[1]);

   SetCurrentCursorPos(132, 26);
   printf("%s", myrecipe.ch[2]);

   SetCurrentCursorPos(132, 30);
   printf("%s", myrecipe.ch[3]);

   SetCurrentCursorPos(132, 34);
   printf("%s", myrecipe.ch[4]);
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);

   return;
}

int DetectCollisionPC(int posX, int posY)//->PC 충돌 시
{
   int arrX=posX/2;
   int arrY=posY;

   if (gameBoardInfo[arrY][arrX]==0)
      return 1;
   else if (gameBoardInfo[arrY][arrX]==1)//장애물 충돌 시
      return 0;
   else if ((gameBoardInfo[arrY][arrX]>=100&&gameBoardInfo[arrY][arrX] < 105)||(gameBoardInfo[arrY][arrX]<=-100&&gameBoardInfo[arrY][arrX] > -105))
   {
      EatMyRecipe(arrX, arrY);
      return 1;
   }
}

int DetectCollisionNPC(int posX, int posY, char NPCModel[3][7])//NPC가 PC와 충돌할 때
{

}

void Recipe_Set() //레시피 순서,재료 모양을 설정해주는 함수
{
   //된장찌개, 스파게티, 잡채, 비빔밥, 라면, 참치김치찌개, 바지락칼국수, 김밥, 오므라이스, 햄버거
   //recipe 0 : 된장찌개
   recipe[0].ch[0]="♤"; //팽이버섯
   recipe[0].name[0]="팽이버섯";
   recipe[0].ch[1]="♠"; //감자
   recipe[0].name[1]="감자";
   recipe[0].ch[2]="♧"; //된장
   recipe[0].name[2]="된장";
   recipe[0].ch[3]="◑"; //두부
   recipe[0].name[3]="두부";
   recipe[0].ch[4]="§"; //애호박
   recipe[0].name[4]="애호박";
   recipe[0].ch[5]="된장찌개";
   //recipe 1 : 스파게티
   recipe[1].ch[0]="♨"; //면
   recipe[1].name[0]="면";
   recipe[1].ch[1]="◐"; //토마토소스
   recipe[1].name[1]="토마토소스";
   recipe[1].ch[2]="▩"; //마늘
   recipe[1].name[2]="마늘";
   recipe[1].ch[3]="♣"; //베이컨
   recipe[1].name[3]="베이컨";
   recipe[1].ch[4]="▣"; //버섯
   recipe[1].name[4]="버섯";
   recipe[1].ch[5]="스파게티";
   //recipe 2 : 잡채
   recipe[2].ch[0]="＠"; //당면
   recipe[2].name[0]="당면";
   recipe[2].ch[1]="▒"; //간장
   recipe[2].name[1]="간장";
   recipe[2].ch[2]="◆"; //참기름
   recipe[2].name[2]="참기름";
   recipe[2].ch[3]="◐"; //시금치
   recipe[2].name[3]="시금치";
   recipe[2].ch[4]="♤"; //당근
   recipe[2].name[4]="당근";
   recipe[2].ch[5]="잡채";
   //recipe 3 : 비빔밥
   recipe[3].ch[0]="⊙"; //밥
   recipe[3].name[0]="밥";
   recipe[3].ch[1]="◈"; //고추장
   recipe[3].name[1]="고추장";
   recipe[3].ch[2]="◁"; //시금치
   recipe[3].name[2]="시금치";
   recipe[3].ch[3]="＠"; //참기름
   recipe[3].name[3]="참기름";
   recipe[3].ch[4]="♧"; //계란후라이
   recipe[3].name[4]="계란후라이";
   recipe[3].ch[5]="비빔밥";
   //recipe 4 : 라면
   recipe[4].ch[0]="☏"; //면
   recipe[4].name[0]="면";
   recipe[4].ch[1]="§"; //냄비
   recipe[4].name[1]="냄비";
   recipe[4].ch[2]="◑";//스프
   recipe[4].name[2]="스프";
   recipe[4].ch[3]="♤";//물
   recipe[4].name[3]="물";
   recipe[4].ch[4]="♣";//후레이크
   recipe[4].name[4]="후레이크";
   recipe[4].ch[5]="라면";
   //recipe 5 : 참치김치찌개
   recipe[5].ch[0]="▩";//물
   recipe[5].name[0]="물";
   recipe[5].ch[1]="◎";//참치
   recipe[5].name[1]="참치";
   recipe[5].ch[2]="◁";//김치
   recipe[5].name[2]="김치";
   recipe[5].ch[3]="◑";//라면스프
   recipe[5].name[3]="라면스프";
   recipe[5].ch[4]="▧";//파
   recipe[5].name[4]="파";
   recipe[5].ch[5]="참치김치찌개";
   //recipe 6 : 바지락칼국수
   recipe[6].ch[0]="▽";//멸치
   recipe[6].name[0]="멸치";
   recipe[6].ch[1]="▒";//바지락
   recipe[6].name[1]="바지락";
   recipe[6].ch[2]="◈";//면
   recipe[6].name[2]="면";
   recipe[6].ch[3]="▣";//애호박
   recipe[6].name[3]="애호박";
   recipe[6].ch[4]="♧";//양파
   recipe[6].name[4]="양파";
   recipe[6].ch[5]="바지락칼국수";
   //recipe 7 : 김밥
   recipe[7].ch[0]="◐";//시금치
   recipe[7].name[0]="시금치";
   recipe[7].ch[1]="⊙";//지단
   recipe[7].name[1]="지단";
   recipe[7].ch[2]="♤";//밥
   recipe[7].name[2]="밥";
   recipe[7].ch[3]="▨";//단무지
   recipe[7].name[3]="단무지";
   recipe[7].ch[4]="◁";//김
   recipe[7].name[4]="김";
   recipe[7].ch[5]="김밥";
   //recipe 8 : 오므라이스
   recipe[8].ch[0]="▒";//케찹
   recipe[8].name[0]="케찹";
   recipe[8].ch[1]="▽";//양파
   recipe[8].name[1]="양파";
   recipe[8].ch[2]="♣";//계란
   recipe[8].name[2]="계란";
   recipe[8].ch[3]="§";//당근
   recipe[8].name[3]="당근";
   recipe[8].ch[4]="▤";//밥
   recipe[8].name[4]="밥";
   recipe[8].ch[5]="오므라이스";
   //recipe 9 : 햄버거
   recipe[9].ch[0]="▥";//참깨빵
   recipe[9].name[0]="참깨빵";
   recipe[9].ch[1]="▒";//순쇠고기패티
   recipe[9].name[1]="쇠고기패티";		/// 순쇠고기하면 칸을 넘어버려서 줄였읍니다
   recipe[9].ch[2]="▤";//특별한소스
   recipe[9].name[2]="특별한소스";
   recipe[9].ch[3]="◑";//양상추
   recipe[9].name[3]="양상추";
   recipe[9].ch[4]="▧";//치즈
   recipe[9].name[4]="치즈";
   recipe[9].ch[5]="햄버거";
}
void NoRecipe_Set() //레시피에 맞지 않는 모양을 설정해주는 함수
{
   norecipe[0].ch[0]="＠";
   norecipe[0].ch[1]="△";
   norecipe[0].ch[2]="◐";
   norecipe[0].ch[3]="◎";
   norecipe[0].ch[4]="▲";

   norecipe[1].ch[0]="♧";
   norecipe[1].ch[1]="△";
   norecipe[1].ch[2]="◈";
   norecipe[1].ch[3]="♤";
   norecipe[1].ch[4]="⊙";

   norecipe[2].ch[0]="▩";
   norecipe[2].ch[1]="◑";
   norecipe[2].ch[2]="♣";
   norecipe[2].ch[3]="▶";
   norecipe[2].ch[4]="■";

   norecipe[3].ch[0]="▤";
   norecipe[3].ch[1]="◐";
   norecipe[3].ch[2]="♣";
   norecipe[3].ch[3]="▶";
   norecipe[3].ch[4]="◀";

   norecipe[4].ch[0]="○";
   norecipe[4].ch[1]="☎";
   norecipe[4].ch[2]="♧";
   norecipe[4].ch[3]="◈";
   norecipe[4].ch[4]="◀";

   norecipe[5].ch[0]="◐";
   norecipe[5].ch[1]="▷";
   norecipe[5].ch[2]="⊙";
   norecipe[5].ch[3]="＠";
   norecipe[5].ch[4]="※";

   norecipe[6].ch[0]="▶";
   norecipe[6].ch[1]="▷";
   norecipe[6].ch[2]="▼";
   norecipe[6].ch[3]="♣";
   norecipe[6].ch[4]="■";

   norecipe[7].ch[0]="◑";
   norecipe[7].ch[1]="♧";
   norecipe[7].ch[2]="▧";
   norecipe[7].ch[3]="▣";
   norecipe[7].ch[4]="▩";

   norecipe[8].ch[0]="◐";
   norecipe[8].ch[1]="◁";
   norecipe[8].ch[2]="◑";
   norecipe[8].ch[3]="▼";
   norecipe[8].ch[4]="▩";

   norecipe[9].ch[0]="▨";
   norecipe[9].ch[1]="▣";
   norecipe[9].ch[2]="▦";
   norecipe[9].ch[3]="◐";
   norecipe[9].ch[4]="▩";
}
void Recipe_Set2() //레시피를 나타내주는 함수
{
   int random, i, k=1, j=0;
   clock_t Start;
   RemoveCursor();
   Recipe_Set();
   NoRecipe_Set();
   srand((unsigned int)time(NULL));
   Start=clock();
  // id=(rand()%10);
   id=0;
   ////////// 상황 안내 화면 //////////

   /////// 스테이지 이름
   //setcolor(6,0);
   SetCurrentCursorPos(113, 2);
   printf("STAGE %d - %s", 1, recipe[id].ch[5]);
   //setcolor(7,0);
   curPos.X=108;
   curPos.Y=18;
   SetCurrentCursorPos(curPos.X, curPos.Y);
   for (i=0; i<5; i++) //레시피와 순서를 콘솔창에 나타내줌 ..(첫번째 레시피 모양이 왜 두개 나오는지 모르겠음)
   {
      printf("%s : %s\n", recipe[id].ch[i], recipe[id].name[i]);
      curPos.Y+=2;//->한 칸 띄움
               //curPos.Y+=1;
      SetCurrentCursorPos(curPos.X, curPos.Y);
   }
   while (1)//레시피를 10초만 보여주기 위해
   {
      curPos.X=108;
      curPos.Y=18;
      SetCurrentCursorPos(curPos.X, curPos.Y);
      if (clock()-Start>100)//10초가 지나면
      {
         for (i=0; i<5; i++)
         {
            printf("                 "); //레시피를 지워준다
            curPos.Y+=2;
            SetCurrentCursorPos(curPos.X, curPos.Y);
         }
         for (i=0; i<5; i++) //레시피를 지우고 난 후, 게임 맵에서 옳은 재료 모양을 나타내줌
         {
            curPos=GetCurrentCursorPos();
            random=(rand()%51);
            if (random==0) random++; //맵경계에는 나오면 안됨
            if (random==51) random--;
            curPos.X=random*2;//->한 칸에 딱 맞게 나오려고! 2의 배수로 설정
            random=(rand()%40);
            if (random==0) random++;
            if (random==40) random--;
            curPos.Y=random;
            SetCurrentCursorPos(curPos.X, curPos.Y);
            printf("%s", recipe[id].ch[j]);
            gameBoardInfo[curPos.Y][curPos.X/2]=100+j;
            j++;
         }
         j=0;
         for (i=0; i<5; i++) //레시피를 지우고 난 후, 게임 맵에서 엉뚱한 재료 모양을 나타내줌
         {
            curPos=GetCurrentCursorPos();
            random=(rand()%51);
            if (random==0) random++;
            if (random==51) random--;
            curPos.X=random*2;
            random=(rand()%40);
            if (random==0) random++;
            if (random==40) random--;
            curPos.Y=random;
            SetCurrentCursorPos(curPos.X, curPos.Y);
            printf("%s", norecipe[id].ch[j]);
            gameBoardInfo[curPos.Y][curPos.X/2]=(j*(-1))-100;
            j++;
         }
         break;
      }
   }
   //printInfo();
}
/////////////////////////////////////////////////////////////////////////////아래부터 PC 이동 함수

void ShowNPC(char NPCModel[3][7])//NPC 보여주는 함수
{

}

void DeleteNPC(char NPCModel[3][7])//NPC 삭제 함수
{

}

void ShowPC()//PC 보여주는 함수
{
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC 색깔 연두색으로 변경

   SetCurrentCursorPos(pc.X, pc.Y);
   printf("■");
}

void DeletePC()
{
   SetCurrentCursorPos(pc.X, pc.Y);
   printf(" ");
}

int BlockDown()//아래로 이동
{
   if (DetectCollisionPC(pc.X, pc.Y+1)==0)
      return 0;
   DeletePC();
   pc.Y+=1;
   ShowPC();

   return 1;
}

int BlockUp()//위로 이동
{
   if (DetectCollisionPC(pc.X, pc.Y-1)==0)
      return 0;
   DeletePC();
   pc.Y-=1;
   ShowPC();

   return 1;
}

int ShiftLeft()//왼쪽으로 이동
{
   if (DetectCollisionPC(pc.X-2, pc.Y)==0)
      return 0;
   DeletePC();
   pc.X-=2;
   ShowPC();

   return 1;
}

int ShiftRight()//오른쪽으로 이동
{
   if (DetectCollisionPC(pc.X+2, pc.Y)==0)
      return 0;
   DeletePC();
   pc.X+=2;
   ShowPC();

   return 1;
}

void ProcessKeyInput()//입력 키에 따라 PC 이동하고 아이템 사용 함수
{
   int i;
   int key;

   while (1)
   {
      while (!_kbhit)
      {
         ShowPC();
      }
      key=_getch();
      switch (key)
      {
      case LEFT:
         ShiftLeft();
         break;

      case RIGHT:
         ShiftRight();
         break;

      case UP:
         BlockUp();
         break;

      case DOWN:
         BlockDown();
         break;

      case ZZ: //z키 눌렀을 때 

         break;
      case XX: //x키 눌렀을 때 

         break;
      case CC: //c키 눌렀을 때 

         break;
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////////

void Intro()
{
   int cnt=1;
   SetCurrentCursorPos(2, cnt++);
   printf("▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩");
   SetCurrentCursorPos(2, cnt++);
   printf("▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩");
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("      조리~");
   SetCurrentCursorPos(2, cnt++);
   printf("    요리~     ■            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("              ■            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("             ■■           ■");
   SetCurrentCursorPos(2, cnt++);
   printf("            ■  ■          ■                            ■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("          ■      ■        ■                                          ■");
   SetCurrentCursorPos(2, cnt++);
   printf("          ■      ■        ■                            ■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                          ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ■■■■■■■                          ■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ■          ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ■          ■                        ■■■■■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ■■■■■■■    ■            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ■            ■        ■■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ■■■■■■■■        ■              ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ■            ■        ■              ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ■            ■        ■■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ■■■■■■■■");
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("                                  ■■■■■■■■■■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ■");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ■    □□□□□   □□□   □    □  □□□□   □□");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      □      □    □  □    □  □     □  □□");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      □      □    □  □    □  □□□□    □");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      □      □    □  □    □  □   □ ");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      □       □□□    □□□   □     □   □");
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩  ▩▩");
   SetCurrentCursorPos(2, cnt++);
   printf("▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩▩");

   SetCurrentCursorPos(48, 34);
   printf("▷START◁");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("▶START◀");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("▷START◁");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("▶START◀");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("▷START◁");
}

void GameMapDelete()//게임 창 전체를 지워주는 함수.. 자주 애용해주세요
{
   int y;

   for (y=1; y<MAP_HEIGHT; y++)
   {
      SetCurrentCursorPos(2, y);
      printf("                                                                                                    ");
   }
}

void GameRule()//게임 방법을 보여주는 함수
{
   SetCurrentCursorPos(40, 4);
   printf("▶▷▶ 게임 방법 ◀◁◀");
   Sleep(1000);

   SetCurrentCursorPos(4, 10);
   printf("1. 방향키로 나를 움직일 수 있어요!");
   Sleep(2000);

   SetCurrentCursorPos(4, 13);
   printf("2. 제한 시간(90초)동안 화면 오른쪽의 레시피를 외우고 순서대로 요리 재료를 획득해 점수를 얻어요!");
   Sleep(2500);

   SetCurrentCursorPos(4, 16);
   printf("3. 아이템을 획득하여 사용해보세요!");
   Sleep(2000);

   SetCurrentCursorPos(7, 18);
   printf("- 타이머 아이템: z키를 통해 사용 가능, 제한 시간 +5초");
   SetCurrentCursorPos(7, 20);
   printf("- 컨닝 페이퍼 아이템: x키를 통해 사용 가능, 레시피를 5초 동안 다시 보여줌");
   SetCurrentCursorPos(7, 22);
   printf("- 엄마 카드 아이템: c키를 통해 사용 가능, 3개를 모았을 때 스테이지 바로 클리어");
   SetCurrentCursorPos(7, 24);
   printf("- 하트 아이템: 획득 즉시 자동 사용, 목숨 +1개");
   Sleep(3800);

   SetCurrentCursorPos(4, 27);
   printf("4. 차, 기차, 맨홀 등에 부딪히면 목숨이 줄어들어요!");
   Sleep(2000);

   SetCurrentCursorPos(4, 30);
   printf("5. 목숨 3개를 모두 잃으면 전체 스테이지가 종료돼요!");
   Sleep(2000);

   SetCurrentCursorPos(4, 33);
   printf("6. 전체 스테이지 10단계가 끝나면 점수에 따라 엔딩을 볼 수 있어요!");
   Sleep(2000);

   SetCurrentCursorPos(40, 37);
   printf("=> 자, 이제 열심히 심부름을 하러 출발 (@ '-'@)/~!");
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
   printf("♥");
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
   Sleep(3500);
}

void Drama()
{
	SetCurrentCursorPos(14, 7);
	printf("ㅁㅁㅁ");
	SetCurrentCursorPos(13, 8);
	printf("ㅁ··ㅁ");
	SetCurrentCursorPos(15, 9);
	printf(" ˘");
	Sleep(500);

	SetCurrentCursorPos(24, 8);
	printf("%s~ 누워만 있지말고 심부름 좀 다녀와", plname);
	Sleep(1500);

	SetCurrentCursorPos(14, 12);
	printf("ｗｗｗ");
	SetCurrentCursorPos(13, 13);
	printf("⊂··⊃");
	SetCurrentCursorPos(15, 14);
	printf(" ˘");
	Sleep(500);

	SetCurrentCursorPos(24, 13);
	printf("알겠어요 엄마");
	Sleep(1500);


	SetCurrentCursorPos(14, 17);
	printf("ㅁㅁㅁ");
	SetCurrentCursorPos(13, 18);
	printf("ㅁ··ㅁ");
	SetCurrentCursorPos(15, 19);
	printf(" ˘");
	Sleep(500);

	SetCurrentCursorPos(24, 18);
	printf("자동차 기차 비행기 오토바이 맨홀 조심하고");
	SetCurrentCursorPos(24, 20);
	printf("꼭 살아서 돌아오거라...");
	Sleep(1500);

	SetCurrentCursorPos(14, 22);
	printf("ｗｗｗ");
	SetCurrentCursorPos(13, 23);
	printf("⊂··⊃");
	SetCurrentCursorPos(15, 24);
	printf(" ˘");
	Sleep(500);

	SetCurrentCursorPos(24, 23);
	printf("알겠어요 엄마.. 저도 이제 다 컸다구요");
	Sleep(1500);


	SetCurrentCursorPos(14, 30);
	printf("ｗｗｗ");
	SetCurrentCursorPos(13, 31);
	printf("⊂··⊃");
	SetCurrentCursorPos(15, 32);
	printf(" ˘");
	Sleep(500);

	SetCurrentCursorPos(24, 31);
	printf("더이상 이런 수모는 겪지 않겠어!");
	SetCurrentCursorPos(24, 33);
	printf("나의 화려한 심부름 실력을 보여주고 말테야!");
	Sleep(2000);
}

void InputPlayer()
{
	SetCurrentCursorPos(38, 10);
	printf("▶▷▶ 너의 이름을 말해줘 ◀◁◀");
	SetCurrentCursorPos(44, 14);
	printf("내 이름: ");
	scanf("%s", plname);
}

int main()
{
   int i;
   plname = (char *)malloc(20*sizeof(char));

   // 게임 전체 틀 = 가로: 140, 세로: 40
   system("title 요리조리 심부름 투어 p(´∇｀)q");
   system("mode con: cols=145 lines=45");   // 가로, 세로                   // 콘솔창 크기.. 보기 좋게 하기위해 늘려봤읍니다,,
   center();
   RemoveCursor();

   drawBoard();
   Item_board();	// 아이템 창 표시
   Heart_board();	// 생명 창 표시

   Intro();
   GameMapDelete();
   GameRule();//게임 방법 설명
   GameMapDelete();//게임 창 전체 지워주는 함수

   InputPlayer();//플레이어 이름 입력 받는 함수
   plname = "이한글";	// 편의상 이한글로 초기화
   Bottari_board();//입력된 플레이어 이름을 보따리에 적용
   Sleep(500);
   GameMapDelete();

   Drama();//심부름 하게되는 스토리 보여주기
   GameMapDelete();

   Recipe_Set2();

   //printInfo();

   pc.X=50;//PC 초기 좌표 셋팅
   pc.Y=20;
   cnt=0;
   ScorePrint();
   for (i=0; i < 5; i++)
   {
      myrecipe.ch[i]="";
   }

   SetCurrentCursorPos(pc.X, pc.Y);//초기 좌표로 커서 이동
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC 색깔 연두색으로 변경

   ShowPC();
   ProcessKeyInput();//PC 움직임 제어
   
   return 0;
}