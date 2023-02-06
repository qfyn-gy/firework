#include<graphics.h>
#include<conio.h>
#include<math.h>
#include<time.h>
#include<stdio.h>
#include<mmsystem.h>
#pragma comment (lib,"Winmm.lib")
#pragma warning(disable:4996)
#define PI 3.14
#define NUM 13		//�̻�����


//�̻��ṹ
struct Fire {
	int r;					//��ǰ��ը�뾶
	int max_r;				//��ը���ľ����Ե���뾶
	int x, y;				//��ը�����ڴ��ڵ�����
	int cent2LeftTopX, cent2LeftTopY;	//��ը�������ͼƬ���Ͻǵ�����
	int width, height;		//ͼƬ�Ŀ��
	int pix[240][240];		//����ͼƬ���ص�

	bool show;				//�Ƿ�����
	bool draw;				//��ʼ������ص�
	DWORD t1, t2, dt;		//�����ٶ�
}fires[NUM];

struct Bullet {
	int x, y;				//�̻����ĵ�ǰ����
	int topX, topY;			//��ߵ�����------����ֵ��FIRE���x,y
	int height;				//�̻��߶�
	bool shoot;				//�Ƿ���Է���

	DWORD t1, t2, dt;		//�����ٶ�
	IMAGE img[2];			//�洢����һ��һ��ͼƬ
	unsigned char n : 1;	//ͼƬ�±�
}bullets[NUM];

//��ʼ���̻�����
void initFire(int i){
//�ֱ�Ϊ:�̻����ĵ�ͼƬ��Ե����Զ���롢�̻����ĵ�ͼƬ���Ͻǵľ��� (x��y)��������
	int r[13] = { 120,120,155,123,130,147,138,138,130,135,140,132,155 };
	int x[13] = { 120,120,110,117,110, 93,102,102,110,105,110,108,110 };
	int y[13] = { 120,120,85,118,120,103,105,110,110,120,120,104,85 };
	fires[i].x = 0;
	// �̻���������
	fires[i].y = 0;
	fires[i].width = 240;
	//ͼƬ��
	fires[i].height = 240;
	//ͼƬ��
	fires[i].max_r = r[i];
	//���뾶
	fires[i].cent2LeftTopX = x[i];
	//���ľ����ϽǾ���
	fires[i].cent2LeftTopY = y[i];
	fires[i].show = false;
	//�Ƿ�����
	fires[i].dt = 5;
	//����ʱ����
	fires[i].t1 = timeGetTime();
	fires[i].r = 0;
	//��0��ʼ����
	fires[i].draw = false;
}


//����ͼƬ
void loadFireImages() {
	/****�����̻������ص���ɫ****/
	IMAGE fm,gm;
	loadimage(&fm,"fire/flower.png");

	for (int i= 0; i< 13;i++)
	{
		SetWorkingImage (&fm);
		getimage(&gm,i * 240,0,240,240);
		SetWorkingImage(&gm);
		for (int a = 0; a< 240; a++)
			for (int b = 0; b<240; b++)
				fires[i].pix[a][b] = getpixel(a,b);
	}

	//�����̻���
	IMAGE sm;
	loadimage(&sm,"fire/shoot.png");
	for (int i = 0; i < 13; i++)
	{
		SetWorkingImage(&sm);
		int n = rand() % 5;

		getimage(&bullets[i].img[0], n * 20,0,20,50);	//��
		getimage(&bullets[i].img[1], (n + 5) * 20,0,20,50);	//��
	}
	//���û�ͼ�豸ΪĬ�ϻ�ͼ���ڣ����ǵ�ǰ��Ϸ����
	SetWorkingImage() ;			//���ûػ�ͼ����

}



void welcome() {
	setcolor(YELLOW);

	//ģ����Ļ�˶����
	int R = 180;
	int offX = 600; //Բ��ƫ����
	int offY = 200; //Բ��ƫ����
	for (int a = 90;a >= -210;a -=6) {
		int x = offX + R * cos(a / 180.0 * PI );
		int y = offY + R * sin(a / 180.0 * PI);

		cleardevice();
		//90-a:0~300
		//(90-a)/300.0:0~1
		//(90-a)/300.0*50=0~50
		settextstyle((90 - a) / 300.0 * 50, 0,_T("����"));
		outtextxy(x - 80, y,_T("���������"));
		outtextxy(x - 10, y + 100, _T("�׸�ֿ��������"));

		/*circle(x, y, 10);
		Sleep(1000);*/
		Sleep(18);

		getchar();

		cleardevice();
		settextstyle(40, 0, _T("����"));
		outtextxy(400, 200, _T("�Ұ���"));
		outtextxy(400, 250, _T("������������"));
		outtextxy(400, 300, _T("��ѩ����"));
		outtextxy(400, 350, _T("ƽ������"));
		outtextxy(400, 400, _T("��ƶ����"));
		outtextxy(400, 450, _T("�ٻ�����"));
		outtextxy(400, 500, _T("Ŀ������Ҳ����"));

		getchar();
	}
}


//ָ���ͼ�豸����ʾ������
DWORD* pMem;

void drawFire(int i) {
	if (!fires[i].draw) {
		return;
	}
	for (double a = 0; a <= 6.28; a += 0.01)//0-2PI����
	{
		//���Ǻ���
		int x1 = (int)(fires[i].cent2LeftTopX + fires[i].r * cos(a));
		//�����ͼƬ���Ͻǵ�����
		int y1 = (int)(fires[i].cent2LeftTopY - fires[i].r * sin(a));
		//����� easyx��Y�����෴
		if (x1 > 0 && x1 < fires[i].width && y1 > 0 && y1 < fires[i].height) {
			//ֻ���ͼƬ�ڵ����ص�
			int b = fires[i].pix[x1][y1] & 0xff;
			int g = (fires[i].pix[x1][y1] >> 8) & 0xff;
			int r = (fires[i].pix[x1][y1] >> 16);
			//�̻����ص��ڴ����ϵ�����
			int xx = (int)(fires[i].x + fires[i].r * cos(a));
			int yy = (int)(fires[i].y - fires[i].r * sin(a));
			//�ϰ������ص㲻�������ֹԽ��
			//��ά���鵱��һλ����ʹ�õİ���//��ɫֵ�ӽ���ɫ�Ĳ������
			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800) {
				pMem[yy * 1200 + xx] = BGR(fires[i].pix[x1][y1]);// �Դ���������̻�
			}
		}
		fires[i].draw = false;
	}
}

void testFire() {
	int n = 5;
	bullets[n].x = 600;
	bullets[n].y = 600;
	bullets[n].topX = 600;
	bullets[n].topY = 200;
	//�����̻��ĳ�ʼ״̬����:����ʼλ�û����̻�)
	putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
	
	
	while (bullets[n].y > bullets[n].topY) {
		//����
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		bullets[n].y -= 5;
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		Sleep(10);
		
		// �Ȳ����̻���
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		
		fires[n].show = true;
		fires[n].x = bullets[n].x + 10;
		fires[n].y = bullets[n].y;
		while (fires[n].r <= fires[n].max_r) {
			fires[n].draw = true;
			drawFire(n);
			fires[n].r++;Sleep(20);
			drawFire(n);
			fires[n].r++;
			Sleep(20);
		}
	}
}

void clearImage() {
	for (int i = 0; i < 2000; i++)
	{
		int px1 = rand() % 1200;
		int py1 = rand() % 800;

		pMem[py1 * 1200 + px1] = BLACK;
		pMem[py1 * 1200 + px1 + 1] = BLACK;//���Դ渳ֵ�������ص�
	}
}

void chose(DWORD& t1)	// t1λΪ��һ�ε��̻�����ʱ��
{
	DWORD t2 = timeGetTime();
	if (t2 - t1 > 100)
	{
		int n = rand() % 30;	//ȡ��������Խ���̻�����Ƶ��Խ������Ϊ<13�ĸ��ʾ�Խ��
		
		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
		{
			/****�����̻�����Ԥ������*****/
			bullets[n].x = rand() % 1200;
			bullets[n].y = rand() % 100 + 600;
			bullets[n].topX = bullets[n].x;
			bullets[n].topY = rand() % 400;
			bullets[n].height = bullets[n].y - bullets[n].topY;
			bullets[n].shoot = true;
			//�����̻��ĳ�ʼ״̬����:����ʼλ�û����̻�)
			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n],
				SRCINVERT);
			//����ÿ���̻���������
			char cmd[50];
			sprintf(cmd, "play s%d", n);
			mciSendString(cmd, 0, 0, 0);
		}
			t1 = t2;
	}
}

//�̻�������
void shoot() {
	for (int i = 0;i < 13;i++) {
		bullets[i].t2 = timeGetTime();
		if (bullets[i].t2 - bullets[i].t1 > bullets[i].dt && bullets[i].shoot == true) {
			//����
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
			//�����̻�����λ�ú�ͼƬ״̬
			if (bullets[i].y > bullets[i].topY) {
				bullets[i].n++;
				bullets[i].y -= 5;
				//����λ���ϣ����»���
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
				
				/****�������߶ȵ�3/ 4������*****/
				//��������ߵ㻹��1/4��ʱ�򣬼���

				if ((bullets[i].y - bullets[i].topY) * 4 < bullets[i].height)
					bullets[i].dt = rand() % 4 + 10;

				/****���������߶�*****/
				if (bullets[i].y <= bullets[i].topY) {
					//�رյ��̻�����Ч�������ű�ը����Ч�������´򿪵��̻�����Ч
					char c1[64],c2[64];
					sprintf(c1,"close s%d",i);
					sprintf(c2,"play f%d" ,i);
					mciSendString(c1,0,0,0);
					mciSendString(c2,0,0,0);

					sprintf_s(c1,sizeof(c1),"open fire/shoot.mp3 alias s%d",i);
					mciSendString(c1,0,0,0);

					//�����̻���
					putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
					//׼����Ⱦ���̻���
					fires[i].x = bullets[i].topX + 10;
					//���̻����м䱬ը
					fires[i].y = bullets[i].topY;
					//����ߵ�����
					fires[i].show = true;
					//��ʼ����
					bullets[i].shoot = false;
					//ֹͣ����
				}

				//�����̻�����ʱ��
				bullets[i].t1 = bullets[i].t2;
			}
		}
	}
}

//�����̻�
void showFire() {
	//�̻����׶�����ʱ������������������Ч��
	//Ϊʲô�����С����Ϊ16?
	//Ŀǰ�̻������뾶��155��׼���԰뾶
	//10�ɿ̶ȣ���ͬ�İ뾶�������ٶȲ�ͬ
	//�뾶Խ������Խ��
	int drt[16] = { 5,5,5,5,5,6,25,25,25,25,55,55,55,55,55 };
	for (int i = 0; i < NUM; i++) {
		fires[i].t2 = timeGetTime();

		//���ӱ�ը�뾶�������̻�������ʱ����������Ч��
		if (fires[i].t2 - fires[i].t1 > fires[i].dt
			&& fires[i].show == true) {
			//�����̻��뾶
			if (fires[i].r < fires[i].max_r) {
				fires[i].r++;
				fires[i].dt = drt[fires[i].r / 10];
				fires[i].draw = true;
			}
			//�����̻��������³�ʼ������ŵķɵ����̻�
			if (fires[i].r >= fires[i].max_r) {
				fires[i].draw = false;
				initFire(i);
				//�رձ�ը��Ч�������´򿪱�ը��Ч
				char cmd[64];
				sprintf(cmd, "close f%d", i);
				mciSendString(cmd,0,0,0);
				sprintf_s(cmd, sizeof(cmd),"open fire/bomb.mp3 alias f%d", i);
				mciSendString(cmd,0,0,0);

			}
			//�����̻���ʱ��
			fires[i].t1 = fires[i].t2;
		}

		//����ָ�����̻�
		drawFire(i);
		
	}
}

void heartFire(DWORD& st1) {
	DWORD st2 = timeGetTime();
	if (st2 - st1 > 20000)//һ�׸��ʱ��{
	//�Ȳ������ڷ��͵��̻���
		for (int i = 0; i < 13; i++) {
			if (bullets[i].shoot)
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n],
					SRCINVERT);
			//��������
			int x[13] = { 600,750,910,1000,950,750,600,450,250,150,250,410,600 };
			int y[13] = { 650,530,400,220,50,40,200,40,50,220,400,530,650 };
			for (int i = 0; i < NUM; i++)
			{
				bullets[i].x = x[i];
				bullets[i].y = y[i] + 750;//ÿ���̻����ķ�����붼��750��ȷ��ͬʱ��ըbullets[i]. topX = x[i];
				bullets[i].topY = y[i];

				bullets[i].height = bullets[i].y - bullets[i].topY;
				bullets[i].shoot = true;
				bullets[i].dt = 7;
				//��ʾ�̻���
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n],
					SRCINVERT);
				/****�����̻�����***/
				fires[i].x = bullets[i].x + 10;
				fires[i].y = bullets[i].topY;
				fires[i].show = false;
				fires[i].r = 0;
			}
			st1 = st2;
		}
}

//void chose(DWORD& t1)  //t1λΪ��һ�ε��̻�����ʱ��
//{
//	DWORD t2 = timeGetTime();
//	if (t2 - t1 > 100)
//	{
//		int n = rand() % 30;//ȡ��������Խ���̻�����Ƶ��Խ������Ϊ<13�ĸ��ʾ�Խ��
//		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
//			/****�����̻�����Ԥ������*****/
//			bullets[n].x = rand() % 1200;
//			bullets[n].y = rand() % 100 + 600;
//			bullets[n].topX = bullets[n].x;
//			bullets[n].topY = rand() % 400;
//			bullets[n].height = bullets[n].y - bullets[n].topY;
//			bullets[n].shoot = true;
//
//		//�����̻��ĳ�ʼ״̬����:����ʼλ�û����̻�)
//			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n],
//			SRCINVERT);
//
//		/****����ÿ���̻���������****/
//		/*char cmd[50];
//		sprintf(cmd, "play s%d", n);
//		mciSendString(cmd, 0, 0, 0);*/
//	}
//	t1 = t2;
//}



void init() {
	initgraph(1200, 800);

	for (int i = 0;i < NUM;i++) {	//��ʼ���̻����̻���
		initFire(i);
	}

	loadFireImages();

	//����������ڻ�ȡ�����豸����ʾ������ָ��
	pMem = GetImageBuffer();	//��ȡ�����Դ�ָ��

	//����Ч������
	char cmd[128] ;
	for (int i = 0;i < 13;i++) {
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.wav alias s%d", i);
		mciSendString(cmd, 0, 0, 0);

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.mp3 alias f%d", i);
		mciSendString(cmd, 0, 0, 0);
	}
}

int main(void) {
	init();
	//welcome(); //���Ʊ����Ļ

	//testFire();

	DWORD t1 = timeGetTime(); //ɸѡ�̻�ʱ��
	DWORD ht1 = timeGetTime();//���Ż�����ʱ

	BeginBatchDraw();

	while (!kbhit()) {
		Sleep(10);

		clearImage();

		chose(t1);
		shoot();
		showFire();
		heartFire(ht1);
		FlushBatchDraw(); //��ʾǰ���˵�л�ͼ����
		
	}

	return 0;
}