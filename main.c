#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <math.h>
#include <time.h>
#include <conio.h>
/*
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC ���� ���λ����� ����
�ֵ�� ���⼭ �Լ� ���ڿ� �ִ� ���� �ٲٸ� �ܼ� �۾� ���� �ٲ� �� �־�! �ܼ� ����ǥ��
���ۿ� �ܼ� ����ǥ��� �˻��ϸ� �� ���´�,, �׸��� �� �ֻܼ���ǥ�� ����� �������̶� �ƴѰŶ� �ٸ��Ŵ���,,,
����� �������� ����ǥ�� �������,,,!
*/

//����Ű, �����̽� ���� (PC ������ ��� �ʿ�)
#define UP 72
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
//������ ��� Ű ����
#define ZZ 44 
#define XX 45 //���� �ڲ� ���ļ� �̷��� ����,,,��
#define CC 46

//�� ��ü Ʋ ũ�� ����
#define GBOARD_HEIGHT 40
#define GBOARD_WIDTH 140
//���� â ũ�� ����
#define MAP_HEIGHT 40
#define MAP_WIDTH 51//���� x��ǥ�� 102

typedef struct food//���, ������ �´� ������ ����ü
{
   char *ch[6];
   char *name[5];
}food;

typedef struct nofood//���ǿ� ���� �ʴ� ������ ����ü
{
   char *ch[6];
}nofood;

COORD curPos;
COORD pc;//pc�� ��ǥ
int gameBoardInfo[MAP_HEIGHT+1][MAP_WIDTH+1]; //PC: 123 / 0: ����� / 1: ���� ����(�ʰ��, ����...) / 100~104: �´� ���(�������� ��ġ�ϰ� Info ���� ����) / -100~-104: Ʋ�� ���
int Speed;
int cnt; //PC�� ��Ḧ � �Ծ�����! �������� �Ѿ�� ���� 0���� �ʱ�ȭ�������
int id;
int score=0;
char *plname;

food recipe[10];//�´� ������ �迭
nofood norecipe[10];//���� �ʴ� ������ �迭
nofood myrecipe; //PC�� ���� ��� �����ϴ� ����

//////////////////////// ���� ���� ���� ��!!!!!!

void setcolor(int color, int bgcolor)// ���� ��(���ڻ�, ���� ����) 
{
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((bgcolor&0xf)<<4)|(color&0xf));
}

void RemoveCursor(void)   //Ŀ�� ������ ����
{
   CONSOLE_CURSOR_INFO curInfo;
   GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
   curInfo.bVisible=0;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void SetCurrentCursorPos(int x, int y) //Ŀ��������set
{
   COORD pos={ x, y };
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void) //Ŀ��������get
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

void Bottari_board()	// ������ �̸� �����ֱ� ���ؼ�!
{
   SetCurrentCursorPos(116, 4);
   printf("%s ������", plname);
}

void Item_board()	// ������ â ǥ��
{
	// ������1-Ÿ
   SetCurrentCursorPos(113, 7);
   printf("Ÿ");
   SetCurrentCursorPos(113, 9);
   printf("X 2");	// �ӽ�

   // ������2-��
   SetCurrentCursorPos(121, 7);
   printf("��");
   SetCurrentCursorPos(121, 9);
   printf("X 1");	// �ӽ�

   // ������3-��
   SetCurrentCursorPos(129, 7);
   printf("��");
   SetCurrentCursorPos(129, 9);
   printf("X 3");	// �ӽ�
}

void Heart_board()	// ���� â ǥ��
{
	SetCurrentCursorPos(110, 12);
	printf("�� �� ��");	// �ӽ�
}

void drawBoard() // �ܼ�â �׸���
{//�������� -> ���� �´� �ŷ�!!
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

   // �� ����
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(0, y);
      if (y==GBOARD_HEIGHT)
      {
         printf("��");
      }
      else if (y==0)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ���� ȭ��� ��Ȳ �ȳ� ȭ����� ���
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(102, y);
      printf("��");
   }

   // �� ������
   for (y=0; y<=GBOARD_HEIGHT; y++)
   {
      SetCurrentCursorPos(140, y);
      if (y==GBOARD_HEIGHT)
      {
         printf("��");
      }
      else if (y==0)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // �� ��
   for (x=2; x<=138; x+=2)
   {
      SetCurrentCursorPos(x, 0);
      printf("��");
   }

   // �� �Ʒ�
   for (x=2; x<=138; x+=2)
   {
      SetCurrentCursorPos(x, 40);
      printf("��");
   }

   /////// ������

   // ����
   for (y=4; y<=14; y++)
   {
      SetCurrentCursorPos(106, y);
      if (y==14)
      {
         printf("��");
      }
      else if (y==4)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ������
   for (y=4; y<=14; y++)
   {
      SetCurrentCursorPos(136, y);
      if (y==14)
      {
         printf("��");
      }
      else if (y==4)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ��-1
   for (x=108; x<=110; x+=2)
   {
      SetCurrentCursorPos(x, 4);
      printf("��");
   }

   // ��-2
   for (x=132; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 4);
      printf("��");
   }

   // �Ʒ�
   for (x=108; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 14);
      printf("��");
   }


   /////// ������ â

   // ��
   for (x=111; x<=131; x+=2)
   {
      SetCurrentCursorPos(x, 6);
      printf("��");
   }

   // ����
   for (y=6; y<=10; y++)
   {
      SetCurrentCursorPos(109, y);
      if (y==10)
      {
         printf("��");
      }
      else if (y==6)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ������
   for (y=6; y<=10; y++)
   {
      SetCurrentCursorPos(133, y);
      if (y==10)
      {
         printf("��");
      }
      else if (y==6)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // �Ʒ�
   for (x=111; x<=131; x+=2)
   {
      SetCurrentCursorPos(x, 10);
      printf("��");
   }

   // �߰�-1
   for (y=7; y<=9; y++)
   {
      SetCurrentCursorPos(117, y);
      printf("��");
   }

   // �߰�-2
   for (y=7; y<=9; y++)
   {
      SetCurrentCursorPos(125, y);
      printf("��");
   }


   /////// �ð�â
   // ����
   for (y=16; y<=28; y++)
   {
      SetCurrentCursorPos(106, y);
      if (y==28)
      {
         printf("��");
      }
      else if (y==16)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ������
   for (y=16; y<=28; y++)
   {
      SetCurrentCursorPos(124, y);
      if (y==28)
      {
         printf("��");
      }
      else if (y==16)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ��
   for (x=108; x<=122; x+=2)
   {
      SetCurrentCursorPos(x, 16);
      printf("��");
   }

   // �Ʒ�
   for (x=108; x<=122; x+=2)
   {
      SetCurrentCursorPos(x, 28);
      printf("��");
   }

   /////// ��� ȹ�� ��Ȳ
   // ����
   for (y=16; y<=36; y++)
   {
      SetCurrentCursorPos(128, y);
      if (y==36)
      {
         printf("��");
      }
      else if (y==16)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ������
   for (y=16; y<=36; y++)
   {
      SetCurrentCursorPos(136, y);
      if (y==36)
      {
         printf("��");
      }
      else if (y==16)
      {
         printf("��");
      }
      else
      {
         printf("��");
      }
   }

   // ��
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 16);
      printf("��");
   }

   // �߰�-1
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 20);
      printf("��");
   }

   // �߰�-2
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 24);
      printf("��");
   }

   // �߰�-3
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 28);
      printf("��");
   }

   // �߰�-4
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 32);
      printf("��");
   }

   // �Ʒ�
   for (x=130; x<=134; x+=2)
   {
      SetCurrentCursorPos(x, 36);
      printf("��");
   }
}

// �ܼ�â�� �߾ӿ� ��� �ϱ� ���� �Լ�
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

// ���� �� ������ x,y ��ǥ ��ġ�� �̵��ϴ°� ���� �Լ�
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
   //printf("%2d",score);3�ٴ�
   SetCurrentCursorPos(108, 30);
   switch(score)
   {   
   case -10:
      printf("         ��   ������      ");
      SetCurrentCursorPos(108, 31);
      printf("     ����   ��      ��      ");
       SetCurrentCursorPos(108, 32);
      printf("���     ��   ��      ��      ");
      SetCurrentCursorPos(108, 33);
      printf("         ��   ��      ��      ");
      SetCurrentCursorPos(108, 34);
      printf("      ����  ������      ");
      break;
   case 10:
      printf("     ��      ������      ");
      SetCurrentCursorPos(108, 31);
      printf(" ����      ��      ��      ");
       SetCurrentCursorPos(108, 32);
      printf("     ��      ��      ��      ");
      SetCurrentCursorPos(108, 33);
      printf("     ��      ��      ��      ");
      SetCurrentCursorPos(108, 34);
      printf("  �����   ������      ");
      break;
   case -9:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��     ��             ");
       SetCurrentCursorPos(108, 32);
      printf("���� ������             ");
      SetCurrentCursorPos(108, 33);
      printf("             ��             ");
      SetCurrentCursorPos(108, 34);
      printf("      ��     ��             ");
      SetCurrentCursorPos(108, 35);
      printf("      ������             ");
      break;
   case 9:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("      ������             ");
      SetCurrentCursorPos(108, 33);
      printf("              ��             ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��             ");
      SetCurrentCursorPos(108, 35);
      printf("      ������             ");
      break;
   case -8:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("����  ����              ");
      SetCurrentCursorPos(108, 33);
      printf("      ��      ��             ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��             ");
      SetCurrentCursorPos(108, 35);
      printf("      ������             ");
      break;
   case 8:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("       �����             ");
      SetCurrentCursorPos(108, 33);
      printf("      ��      ��             ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��             ");
      SetCurrentCursorPos(108, 35);
      printf("      ������             ");
      break;
   case -7:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("����       ��              ");
      SetCurrentCursorPos(108, 33);
      printf("            ��               ");
      SetCurrentCursorPos(108, 34);
      printf("           ��                ");
      SetCurrentCursorPos(108, 35);
      printf("          ��                 ");
      break;
   case 7:
      printf("      ������             ");
      SetCurrentCursorPos(108,31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("             ��              ");
      SetCurrentCursorPos(108, 33);
      printf("            ��               ");
      SetCurrentCursorPos(108,34);
      printf("           ��                ");
      SetCurrentCursorPos(108,35);
      printf("          ��                 ");
      break;
    case -6:
      printf("       ������             ");
      SetCurrentCursorPos(108, 31);
      printf("       ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("���� ��                    ");
      SetCurrentCursorPos(108, 33);
      printf("       ������              ");
      SetCurrentCursorPos(108, 34);
      printf("       ��      ��             ");
      SetCurrentCursorPos(108, 35);
      printf("       ������             ");
      break;
   case 6:
      printf("      ������             ");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��             ");
       SetCurrentCursorPos(108, 32);
      printf("      ��                    ");
      SetCurrentCursorPos(108, 33);
      printf("      ������             ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case -5:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("      ��       ");
       SetCurrentCursorPos(108, 32);
      printf("���� �����");
      SetCurrentCursorPos(108, 33);
      printf("              ��");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case 5:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("      ��       ");
       SetCurrentCursorPos(108, 32);
      printf("      ������");
      SetCurrentCursorPos(108, 33);
      printf("              ��");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case -4:
      printf("             ���");
      SetCurrentCursorPos(108, 31);
      printf("           ��  ��");
       SetCurrentCursorPos(108, 32);
      printf("����   ��    ��");
      SetCurrentCursorPos(108, 33);
      printf("       �������");
      SetCurrentCursorPos(108,34);
      printf("               ��");
      SetCurrentCursorPos(108, 35);
      printf("             ����");
      break;
   case 4:
      printf("             ���");
      SetCurrentCursorPos(108, 31);
      printf("           ��  ��");
       SetCurrentCursorPos(108, 32);
      printf("         ��    ��");
      SetCurrentCursorPos(108, 33);
      printf("       �������");
      SetCurrentCursorPos(108,34);
      printf("               ��");
      SetCurrentCursorPos(108, 35);
      printf("             ����");
      break;
   case -3:
      printf("      ������");
      SetCurrentCursorPos(108,31);
      printf("              ��");
       SetCurrentCursorPos(108,32);
      printf("����  �����");
      SetCurrentCursorPos(108,33);
      printf("              ��");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case 3:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("              ��");
       SetCurrentCursorPos(108, 32);
      printf("        �����");
      SetCurrentCursorPos(108, 33);
      printf("              ��");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case -2:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("              ��");
       SetCurrentCursorPos(108, 32);
      printf("���� ������");
      SetCurrentCursorPos(108, 33);
      printf("      ��      ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
   case 2:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("              ��");
       SetCurrentCursorPos(108, 32);
      printf("      ������");
      SetCurrentCursorPos(108, 33);
      printf("      ��      ");
      SetCurrentCursorPos(108, 34);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 35);
      printf("      ������");
      break;
   case -1:
      printf("           ��");
      SetCurrentCursorPos(108, 31);
      printf("       ����");
       SetCurrentCursorPos(108, 32);
      printf("����     ��");
      SetCurrentCursorPos(108,33);
      printf("           �� ");
      SetCurrentCursorPos(108,34);
      printf("       �����");
      break;
   case 1:
      printf("           ��");
      SetCurrentCursorPos(108, 31);
      printf("       ����");
       SetCurrentCursorPos(108, 32);
      printf("           ��");
      SetCurrentCursorPos(108, 33);
      printf("           �� ");
      SetCurrentCursorPos(108, 34);
      printf("        �����");
      break;
   case 0:
      printf("      ������");
      SetCurrentCursorPos(108, 31);
      printf("      ��      ��");
       SetCurrentCursorPos(108, 32);
      printf("      ��      ��");
      SetCurrentCursorPos(108, 33);
      printf("      ��      ��");
      SetCurrentCursorPos(108,34);
      printf("      ������");
      break;
   }
   
}
void EatMyRecipe(int posX, int posY)//PC�� ���� ��Ḧ �迭�� �����ϴ� �Լ� -> �浹�˻��ؼ� true�� ������ �� ���� ������ 100�̻����� ���� -> �̻��̸� �� �Լ� ȣ��
{
   int eat=gameBoardInfo[posY][posX];

   //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), "f");//���� ������� ����

   if (eat>=100&&eat<=104) // 100, 101, 102, 103, 104������� Info�� ����
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
   else if (eat<=-100&&eat>=-104)//-100, -101, -102, -103, -104������� Info�� ����
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

int DetectCollisionPC(int posX, int posY)//->PC �浹 ��
{
   int arrX=posX/2;
   int arrY=posY;

   if (gameBoardInfo[arrY][arrX]==0)
      return 1;
   else if (gameBoardInfo[arrY][arrX]==1)//��ֹ� �浹 ��
      return 0;
   else if ((gameBoardInfo[arrY][arrX]>=100&&gameBoardInfo[arrY][arrX] < 105)||(gameBoardInfo[arrY][arrX]<=-100&&gameBoardInfo[arrY][arrX] > -105))
   {
      EatMyRecipe(arrX, arrY);
      return 1;
   }
}

int DetectCollisionNPC(int posX, int posY, char NPCModel[3][7])//NPC�� PC�� �浹�� ��
{

}

void Recipe_Set() //������ ����,��� ����� �������ִ� �Լ�
{
   //�����, ���İ�Ƽ, ��ä, �����, ���, ��ġ��ġ�, ������Į����, ���, ���Ƕ��̽�, �ܹ���
   //recipe 0 : �����
   recipe[0].ch[0]="��"; //���̹���
   recipe[0].name[0]="���̹���";
   recipe[0].ch[1]="��"; //����
   recipe[0].name[1]="����";
   recipe[0].ch[2]="��"; //����
   recipe[0].name[2]="����";
   recipe[0].ch[3]="��"; //�κ�
   recipe[0].name[3]="�κ�";
   recipe[0].ch[4]="��"; //��ȣ��
   recipe[0].name[4]="��ȣ��";
   recipe[0].ch[5]="�����";
   //recipe 1 : ���İ�Ƽ
   recipe[1].ch[0]="��"; //��
   recipe[1].name[0]="��";
   recipe[1].ch[1]="��"; //�丶��ҽ�
   recipe[1].name[1]="�丶��ҽ�";
   recipe[1].ch[2]="��"; //����
   recipe[1].name[2]="����";
   recipe[1].ch[3]="��"; //������
   recipe[1].name[3]="������";
   recipe[1].ch[4]="��"; //����
   recipe[1].name[4]="����";
   recipe[1].ch[5]="���İ�Ƽ";
   //recipe 2 : ��ä
   recipe[2].ch[0]="��"; //���
   recipe[2].name[0]="���";
   recipe[2].ch[1]="��"; //����
   recipe[2].name[1]="����";
   recipe[2].ch[2]="��"; //���⸧
   recipe[2].name[2]="���⸧";
   recipe[2].ch[3]="��"; //�ñ�ġ
   recipe[2].name[3]="�ñ�ġ";
   recipe[2].ch[4]="��"; //���
   recipe[2].name[4]="���";
   recipe[2].ch[5]="��ä";
   //recipe 3 : �����
   recipe[3].ch[0]="��"; //��
   recipe[3].name[0]="��";
   recipe[3].ch[1]="��"; //������
   recipe[3].name[1]="������";
   recipe[3].ch[2]="��"; //�ñ�ġ
   recipe[3].name[2]="�ñ�ġ";
   recipe[3].ch[3]="��"; //���⸧
   recipe[3].name[3]="���⸧";
   recipe[3].ch[4]="��"; //����Ķ���
   recipe[3].name[4]="����Ķ���";
   recipe[3].ch[5]="�����";
   //recipe 4 : ���
   recipe[4].ch[0]="��"; //��
   recipe[4].name[0]="��";
   recipe[4].ch[1]="��"; //����
   recipe[4].name[1]="����";
   recipe[4].ch[2]="��";//����
   recipe[4].name[2]="����";
   recipe[4].ch[3]="��";//��
   recipe[4].name[3]="��";
   recipe[4].ch[4]="��";//�ķ���ũ
   recipe[4].name[4]="�ķ���ũ";
   recipe[4].ch[5]="���";
   //recipe 5 : ��ġ��ġ�
   recipe[5].ch[0]="��";//��
   recipe[5].name[0]="��";
   recipe[5].ch[1]="��";//��ġ
   recipe[5].name[1]="��ġ";
   recipe[5].ch[2]="��";//��ġ
   recipe[5].name[2]="��ġ";
   recipe[5].ch[3]="��";//��齺��
   recipe[5].name[3]="��齺��";
   recipe[5].ch[4]="��";//��
   recipe[5].name[4]="��";
   recipe[5].ch[5]="��ġ��ġ�";
   //recipe 6 : ������Į����
   recipe[6].ch[0]="��";//��ġ
   recipe[6].name[0]="��ġ";
   recipe[6].ch[1]="��";//������
   recipe[6].name[1]="������";
   recipe[6].ch[2]="��";//��
   recipe[6].name[2]="��";
   recipe[6].ch[3]="��";//��ȣ��
   recipe[6].name[3]="��ȣ��";
   recipe[6].ch[4]="��";//����
   recipe[6].name[4]="����";
   recipe[6].ch[5]="������Į����";
   //recipe 7 : ���
   recipe[7].ch[0]="��";//�ñ�ġ
   recipe[7].name[0]="�ñ�ġ";
   recipe[7].ch[1]="��";//����
   recipe[7].name[1]="����";
   recipe[7].ch[2]="��";//��
   recipe[7].name[2]="��";
   recipe[7].ch[3]="��";//�ܹ���
   recipe[7].name[3]="�ܹ���";
   recipe[7].ch[4]="��";//��
   recipe[7].name[4]="��";
   recipe[7].ch[5]="���";
   //recipe 8 : ���Ƕ��̽�
   recipe[8].ch[0]="��";//����
   recipe[8].name[0]="����";
   recipe[8].ch[1]="��";//����
   recipe[8].name[1]="����";
   recipe[8].ch[2]="��";//���
   recipe[8].name[2]="���";
   recipe[8].ch[3]="��";//���
   recipe[8].name[3]="���";
   recipe[8].ch[4]="��";//��
   recipe[8].name[4]="��";
   recipe[8].ch[5]="���Ƕ��̽�";
   //recipe 9 : �ܹ���
   recipe[9].ch[0]="��";//������
   recipe[9].name[0]="������";
   recipe[9].ch[1]="��";//��������Ƽ
   recipe[9].name[1]="������Ƽ";		/// �������ϸ� ĭ�� �Ѿ������ �ٿ����ϴ�
   recipe[9].ch[2]="��";//Ư���Ѽҽ�
   recipe[9].name[2]="Ư���Ѽҽ�";
   recipe[9].ch[3]="��";//�����
   recipe[9].name[3]="�����";
   recipe[9].ch[4]="��";//ġ��
   recipe[9].name[4]="ġ��";
   recipe[9].ch[5]="�ܹ���";
}
void NoRecipe_Set() //�����ǿ� ���� �ʴ� ����� �������ִ� �Լ�
{
   norecipe[0].ch[0]="��";
   norecipe[0].ch[1]="��";
   norecipe[0].ch[2]="��";
   norecipe[0].ch[3]="��";
   norecipe[0].ch[4]="��";

   norecipe[1].ch[0]="��";
   norecipe[1].ch[1]="��";
   norecipe[1].ch[2]="��";
   norecipe[1].ch[3]="��";
   norecipe[1].ch[4]="��";

   norecipe[2].ch[0]="��";
   norecipe[2].ch[1]="��";
   norecipe[2].ch[2]="��";
   norecipe[2].ch[3]="��";
   norecipe[2].ch[4]="��";

   norecipe[3].ch[0]="��";
   norecipe[3].ch[1]="��";
   norecipe[3].ch[2]="��";
   norecipe[3].ch[3]="��";
   norecipe[3].ch[4]="��";

   norecipe[4].ch[0]="��";
   norecipe[4].ch[1]="��";
   norecipe[4].ch[2]="��";
   norecipe[4].ch[3]="��";
   norecipe[4].ch[4]="��";

   norecipe[5].ch[0]="��";
   norecipe[5].ch[1]="��";
   norecipe[5].ch[2]="��";
   norecipe[5].ch[3]="��";
   norecipe[5].ch[4]="��";

   norecipe[6].ch[0]="��";
   norecipe[6].ch[1]="��";
   norecipe[6].ch[2]="��";
   norecipe[6].ch[3]="��";
   norecipe[6].ch[4]="��";

   norecipe[7].ch[0]="��";
   norecipe[7].ch[1]="��";
   norecipe[7].ch[2]="��";
   norecipe[7].ch[3]="��";
   norecipe[7].ch[4]="��";

   norecipe[8].ch[0]="��";
   norecipe[8].ch[1]="��";
   norecipe[8].ch[2]="��";
   norecipe[8].ch[3]="��";
   norecipe[8].ch[4]="��";

   norecipe[9].ch[0]="��";
   norecipe[9].ch[1]="��";
   norecipe[9].ch[2]="��";
   norecipe[9].ch[3]="��";
   norecipe[9].ch[4]="��";
}
void Recipe_Set2() //�����Ǹ� ��Ÿ���ִ� �Լ�
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
   ////////// ��Ȳ �ȳ� ȭ�� //////////

   /////// �������� �̸�
   //setcolor(6,0);
   SetCurrentCursorPos(113, 2);
   printf("STAGE %d - %s", 1, recipe[id].ch[5]);
   //setcolor(7,0);
   curPos.X=108;
   curPos.Y=18;
   SetCurrentCursorPos(curPos.X, curPos.Y);
   for (i=0; i<5; i++) //�����ǿ� ������ �ܼ�â�� ��Ÿ���� ..(ù��° ������ ����� �� �ΰ� �������� �𸣰���)
   {
      printf("%s : %s\n", recipe[id].ch[i], recipe[id].name[i]);
      curPos.Y+=2;//->�� ĭ ���
               //curPos.Y+=1;
      SetCurrentCursorPos(curPos.X, curPos.Y);
   }
   while (1)//�����Ǹ� 10�ʸ� �����ֱ� ����
   {
      curPos.X=108;
      curPos.Y=18;
      SetCurrentCursorPos(curPos.X, curPos.Y);
      if (clock()-Start>100)//10�ʰ� ������
      {
         for (i=0; i<5; i++)
         {
            printf("                 "); //�����Ǹ� �����ش�
            curPos.Y+=2;
            SetCurrentCursorPos(curPos.X, curPos.Y);
         }
         for (i=0; i<5; i++) //�����Ǹ� ����� �� ��, ���� �ʿ��� ���� ��� ����� ��Ÿ����
         {
            curPos=GetCurrentCursorPos();
            random=(rand()%51);
            if (random==0) random++; //�ʰ�迡�� ������ �ȵ�
            if (random==51) random--;
            curPos.X=random*2;//->�� ĭ�� �� �°� ��������! 2�� ����� ����
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
         for (i=0; i<5; i++) //�����Ǹ� ����� �� ��, ���� �ʿ��� ������ ��� ����� ��Ÿ����
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
/////////////////////////////////////////////////////////////////////////////�Ʒ����� PC �̵� �Լ�

void ShowNPC(char NPCModel[3][7])//NPC �����ִ� �Լ�
{

}

void DeleteNPC(char NPCModel[3][7])//NPC ���� �Լ�
{

}

void ShowPC()//PC �����ִ� �Լ�
{
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC ���� ���λ����� ����

   SetCurrentCursorPos(pc.X, pc.Y);
   printf("��");
}

void DeletePC()
{
   SetCurrentCursorPos(pc.X, pc.Y);
   printf(" ");
}

int BlockDown()//�Ʒ��� �̵�
{
   if (DetectCollisionPC(pc.X, pc.Y+1)==0)
      return 0;
   DeletePC();
   pc.Y+=1;
   ShowPC();

   return 1;
}

int BlockUp()//���� �̵�
{
   if (DetectCollisionPC(pc.X, pc.Y-1)==0)
      return 0;
   DeletePC();
   pc.Y-=1;
   ShowPC();

   return 1;
}

int ShiftLeft()//�������� �̵�
{
   if (DetectCollisionPC(pc.X-2, pc.Y)==0)
      return 0;
   DeletePC();
   pc.X-=2;
   ShowPC();

   return 1;
}

int ShiftRight()//���������� �̵�
{
   if (DetectCollisionPC(pc.X+2, pc.Y)==0)
      return 0;
   DeletePC();
   pc.X+=2;
   ShowPC();

   return 1;
}

void ProcessKeyInput()//�Է� Ű�� ���� PC �̵��ϰ� ������ ��� �Լ�
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

      case ZZ: //zŰ ������ �� 

         break;
      case XX: //xŰ ������ �� 

         break;
      case CC: //cŰ ������ �� 

         break;
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////////

void Intro()
{
   int cnt=1;
   SetCurrentCursorPos(2, cnt++);
   printf("�̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢�");
   SetCurrentCursorPos(2, cnt++);
   printf("�̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�");
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("      ����~");
   SetCurrentCursorPos(2, cnt++);
   printf("    �丮~     ��            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("              ��            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("             ���           ��");
   SetCurrentCursorPos(2, cnt++);
   printf("            ��  ��          ��                            ���������");
   SetCurrentCursorPos(2, cnt++);
   printf("          ��      ��        ��                                          ��");
   SetCurrentCursorPos(2, cnt++);
   printf("          ��      ��        ��                            ���������");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                          ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ��������                          ���������");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ��          ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ��          ��                        �������������");
   SetCurrentCursorPos(2, cnt++);
   printf("                  ��������    ��            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ��            ��        ����������");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ���������        ��              ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ��            ��        ��              ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ��            ��        ����������");
   SetCurrentCursorPos(2, cnt++);
   printf("                                    ���������");
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("                                  �����������");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                            ��    ������   ����   ��    ��  �����   ���");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      ��      ��    ��  ��    ��  ��     ��  ���");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      ��      ��    ��  ��    ��  �����    ��");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      ��      ��    ��  ��    ��  ��   �� ");
   SetCurrentCursorPos(2, cnt++);
   printf("                                                      ��       ����    ����   ��     ��   ��");
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   cnt++;
   SetCurrentCursorPos(2, cnt++);
   printf("�̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�  �̢�");
   SetCurrentCursorPos(2, cnt++);
   printf("�̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢̢�");

   SetCurrentCursorPos(48, 34);
   printf("��START��");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("��START��");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("��START��");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("��START��");
   Sleep(500);
   SetCurrentCursorPos(48, 34);
   printf("��START��");
}

void GameMapDelete()//���� â ��ü�� �����ִ� �Լ�.. ���� �ֿ����ּ���
{
   int y;

   for (y=1; y<MAP_HEIGHT; y++)
   {
      SetCurrentCursorPos(2, y);
      printf("                                                                                                    ");
   }
}

void GameRule()//���� ����� �����ִ� �Լ�
{
   SetCurrentCursorPos(40, 4);
   printf("������ ���� ��� ������");
   Sleep(1000);

   SetCurrentCursorPos(4, 10);
   printf("1. ����Ű�� ���� ������ �� �־��!");
   Sleep(2000);

   SetCurrentCursorPos(4, 13);
   printf("2. ���� �ð�(90��)���� ȭ�� �������� �����Ǹ� �ܿ�� ������� �丮 ��Ḧ ȹ���� ������ ����!");
   Sleep(2500);

   SetCurrentCursorPos(4, 16);
   printf("3. �������� ȹ���Ͽ� ����غ�����!");
   Sleep(2000);

   SetCurrentCursorPos(7, 18);
   printf("- Ÿ�̸� ������: zŰ�� ���� ��� ����, ���� �ð� +5��");
   SetCurrentCursorPos(7, 20);
   printf("- ���� ������ ������: xŰ�� ���� ��� ����, �����Ǹ� 5�� ���� �ٽ� ������");
   SetCurrentCursorPos(7, 22);
   printf("- ���� ī�� ������: cŰ�� ���� ��� ����, 3���� ����� �� �������� �ٷ� Ŭ����");
   SetCurrentCursorPos(7, 24);
   printf("- ��Ʈ ������: ȹ�� ��� �ڵ� ���, ��� +1��");
   Sleep(3800);

   SetCurrentCursorPos(4, 27);
   printf("4. ��, ����, ��Ȧ � �ε����� ����� �پ����!");
   Sleep(2000);

   SetCurrentCursorPos(4, 30);
   printf("5. ��� 3���� ��� ������ ��ü ���������� ����ſ�!");
   Sleep(2000);

   SetCurrentCursorPos(4, 33);
   printf("6. ��ü �������� 10�ܰ谡 ������ ������ ���� ������ �� �� �־��!");
   Sleep(2000);

   SetCurrentCursorPos(40, 37);
   printf("=> ��, ���� ������ �ɺθ��� �Ϸ� ��� (@ '-'@)/~!");
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
   printf("��");
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
   Sleep(3500);
}

void Drama()
{
	SetCurrentCursorPos(14, 7);
	printf("������");
	SetCurrentCursorPos(13, 8);
	printf("��������");
	SetCurrentCursorPos(15, 9);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 8);
	printf("%s~ ������ �������� �ɺθ� �� �ٳ��", plname);
	Sleep(1500);

	SetCurrentCursorPos(14, 12);
	printf("������");
	SetCurrentCursorPos(13, 13);
	printf("��������");
	SetCurrentCursorPos(15, 14);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 13);
	printf("�˰ھ�� ����");
	Sleep(1500);


	SetCurrentCursorPos(14, 17);
	printf("������");
	SetCurrentCursorPos(13, 18);
	printf("��������");
	SetCurrentCursorPos(15, 19);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 18);
	printf("�ڵ��� ���� ����� ������� ��Ȧ �����ϰ�");
	SetCurrentCursorPos(24, 20);
	printf("�� ��Ƽ� ���ƿ��Ŷ�...");
	Sleep(1500);

	SetCurrentCursorPos(14, 22);
	printf("������");
	SetCurrentCursorPos(13, 23);
	printf("��������");
	SetCurrentCursorPos(15, 24);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 23);
	printf("�˰ھ�� ����.. ���� ���� �� �Ǵٱ���");
	Sleep(1500);


	SetCurrentCursorPos(14, 30);
	printf("������");
	SetCurrentCursorPos(13, 31);
	printf("��������");
	SetCurrentCursorPos(15, 32);
	printf(" ��");
	Sleep(500);

	SetCurrentCursorPos(24, 31);
	printf("���̻� �̷� ����� ���� �ʰھ�!");
	SetCurrentCursorPos(24, 33);
	printf("���� ȭ���� �ɺθ� �Ƿ��� �����ְ� ���׾�!");
	Sleep(2000);
}

void InputPlayer()
{
	SetCurrentCursorPos(38, 10);
	printf("������ ���� �̸��� ������ ������");
	SetCurrentCursorPos(44, 14);
	printf("�� �̸�: ");
	scanf("%s", plname);
}

int main()
{
   int i;
   plname = (char *)malloc(20*sizeof(char));

   // ���� ��ü Ʋ = ����: 140, ����: 40
   system("title �丮���� �ɺθ� ���� p(���ԣ�)q");
   system("mode con: cols=145 lines=45");   // ����, ����                   // �ܼ�â ũ��.. ���� ���� �ϱ����� �÷������ϴ�,,
   center();
   RemoveCursor();

   drawBoard();
   Item_board();	// ������ â ǥ��
   Heart_board();	// ���� â ǥ��

   Intro();
   GameMapDelete();
   GameRule();//���� ��� ����
   GameMapDelete();//���� â ��ü �����ִ� �Լ�

   InputPlayer();//�÷��̾� �̸� �Է� �޴� �Լ�
   plname = "���ѱ�";	// ���ǻ� ���ѱ۷� �ʱ�ȭ
   Bottari_board();//�Էµ� �÷��̾� �̸��� �������� ����
   Sleep(500);
   GameMapDelete();

   Drama();//�ɺθ� �ϰԵǴ� ���丮 �����ֱ�
   GameMapDelete();

   Recipe_Set2();

   //printInfo();

   pc.X=50;//PC �ʱ� ��ǥ ����
   pc.Y=20;
   cnt=0;
   ScorePrint();
   for (i=0; i < 5; i++)
   {
      myrecipe.ch[i]="";
   }

   SetCurrentCursorPos(pc.X, pc.Y);//�ʱ� ��ǥ�� Ŀ�� �̵�
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 012);//PC ���� ���λ����� ����

   ShowPC();
   ProcessKeyInput();//PC ������ ����
   
   return 0;
}