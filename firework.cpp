#include<graphics.h>
#include<conio.h>
#include<math.h>
#include<time.h>
#include<stdio.h>
#include<mmsystem.h>
#pragma comment (lib,"Winmm.lib")
#pragma warning(disable:4996)
#define PI 3.14
#define NUM 13		//烟花种类


//烟花结构
struct Fire {
	int r;					//当前爆炸半径
	int max_r;				//爆炸中心距离边缘最大半径
	int x, y;				//爆炸中心在窗口的坐标
	int cent2LeftTopX, cent2LeftTopY;	//爆炸中心相对图片左上角的坐标
	int width, height;		//图片的宽高
	int pix[240][240];		//储存图片像素点

	bool show;				//是否绽放
	bool draw;				//开始输出像素点
	DWORD t1, t2, dt;		//绽放速度
}fires[NUM];

struct Bullet {
	int x, y;				//烟花弹的当前坐标
	int topX, topY;			//最高点坐标------将赋值给FIRE里的x,y
	int height;				//烟花高度
	bool shoot;				//是否可以发射

	DWORD t1, t2, dt;		//发射速度
	IMAGE img[2];			//存储花弹一亮一暗图片
	unsigned char n : 1;	//图片下标
}bullets[NUM];

//初始化烟花参数
void initFire(int i){
//分别为:烟花中心到图片边缘的最远距离、烟花中心到图片左上角的距离 (x、y)两个分量
	int r[13] = { 120,120,155,123,130,147,138,138,130,135,140,132,155 };
	int x[13] = { 120,120,110,117,110, 93,102,102,110,105,110,108,110 };
	int y[13] = { 120,120,85,118,120,103,105,110,110,120,120,104,85 };
	fires[i].x = 0;
	// 烟花中心坐标
	fires[i].y = 0;
	fires[i].width = 240;
	//图片宽
	fires[i].height = 240;
	//图片高
	fires[i].max_r = r[i];
	//最大半径
	fires[i].cent2LeftTopX = x[i];
	//中心距左上角距离
	fires[i].cent2LeftTopY = y[i];
	fires[i].show = false;
	//是否绽放
	fires[i].dt = 5;
	//绽放时间间隔
	fires[i].t1 = timeGetTime();
	fires[i].r = 0;
	//从0开始绽放
	fires[i].draw = false;
}


//加载图片
void loadFireImages() {
	/****储存烟花的像素点颜色****/
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

	//加载烟花弹
	IMAGE sm;
	loadimage(&sm,"fire/shoot.png");
	for (int i = 0; i < 13; i++)
	{
		SetWorkingImage(&sm);
		int n = rand() % 5;

		getimage(&bullets[i].img[0], n * 20,0,20,50);	//暗
		getimage(&bullets[i].img[1], (n + 5) * 20,0,20,50);	//亮
	}
	//设置绘图设备为默认绘图窗口，就是当前游戏窗口
	SetWorkingImage() ;			//设置回绘图窗口

}



void welcome() {
	setcolor(YELLOW);

	//模拟字幕运动规矩
	int R = 180;
	int offX = 600; //圆心偏移量
	int offY = 200; //圆心偏移量
	for (int a = 90;a >= -210;a -=6) {
		int x = offX + R * cos(a / 180.0 * PI );
		int y = offY + R * sin(a / 180.0 * PI);

		cleardevice();
		//90-a:0~300
		//(90-a)/300.0:0~1
		//(90-a)/300.0*50=0~50
		settextstyle((90 - a) / 300.0 * 50, 0,_T("楷体"));
		outtextxy(x - 80, y,_T("浪漫表白日"));
		outtextxy(x - 10, y + 100, _T("献给挚爱的妈妈"));

		/*circle(x, y, 10);
		Sleep(1000);*/
		Sleep(18);

		getchar();

		cleardevice();
		settextstyle(40, 0, _T("楷体"));
		outtextxy(400, 200, _T("我爱你"));
		outtextxy(400, 250, _T("往后余生是你"));
		outtextxy(400, 300, _T("风雪是你"));
		outtextxy(400, 350, _T("平淡是你"));
		outtextxy(400, 400, _T("清贫是你"));
		outtextxy(400, 450, _T("荣华是你"));
		outtextxy(400, 500, _T("目光所致也是你"));

		getchar();
	}
}


//指向绘图设备的显示缓冲区
DWORD* pMem;

void drawFire(int i) {
	if (!fires[i].draw) {
		return;
	}
	for (double a = 0; a <= 6.28; a += 0.01)//0-2PI弧度
	{
		//三角函数
		int x1 = (int)(fires[i].cent2LeftTopX + fires[i].r * cos(a));
		//相对于图片左上角的坐标
		int y1 = (int)(fires[i].cent2LeftTopY - fires[i].r * sin(a));
		//方向和 easyx的Y坐标相反
		if (x1 > 0 && x1 < fires[i].width && y1 > 0 && y1 < fires[i].height) {
			//只输出图片内的像素点
			int b = fires[i].pix[x1][y1] & 0xff;
			int g = (fires[i].pix[x1][y1] >> 8) & 0xff;
			int r = (fires[i].pix[x1][y1] >> 16);
			//烟花像素点在窗口上的坐标
			int xx = (int)(fires[i].x + fires[i].r * cos(a));
			int yy = (int)(fires[i].y - fires[i].r * sin(a));
			//较暗的像素点不输出、防止越界
			//二维数组当成一位数组使用的案例//颜色值接近黑色的不输出。
			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800) {
				pMem[yy * 1200 + xx] = BGR(fires[i].pix[x1][y1]);// 显存操作绘制烟花
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
	//绘制烟花的初始状态（即:在起始位置绘制烟花)
	putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
	
	
	while (bullets[n].y > bullets[n].topY) {
		//擦除
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		bullets[n].y -= 5;
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		Sleep(10);
		
		// 先擦除烟花弹
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
		pMem[py1 * 1200 + px1 + 1] = BLACK;//对显存赋值擦出像素点
	}
}

void chose(DWORD& t1)	// t1位为上一次点烟花弹的时间
{
	DWORD t2 = timeGetTime();
	if (t2 - t1 > 100)
	{
		int n = rand() % 30;	//取摸的数字越大，烟花发射频率越慢，因为<13的概率就越低
		
		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
		{
			/****重置烟花弹，预备发射*****/
			bullets[n].x = rand() % 1200;
			bullets[n].y = rand() % 100 + 600;
			bullets[n].topX = bullets[n].x;
			bullets[n].topY = rand() % 400;
			bullets[n].height = bullets[n].y - bullets[n].topY;
			bullets[n].shoot = true;
			//绘制烟花的初始状态（即:在起始位置绘制烟花)
			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n],
				SRCINVERT);
			//播放每个烟花弹的声音
			char cmd[50];
			sprintf(cmd, "play s%d", n);
			mciSendString(cmd, 0, 0, 0);
		}
			t1 = t2;
	}
}

//烟花弹升空
void shoot() {
	for (int i = 0;i < 13;i++) {
		bullets[i].t2 = timeGetTime();
		if (bullets[i].t2 - bullets[i].t1 > bullets[i].dt && bullets[i].shoot == true) {
			//擦除
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
			//更新烟花弹的位置和图片状态
			if (bullets[i].y > bullets[i].topY) {
				bullets[i].n++;
				bullets[i].y -= 5;
				//在新位置上，重新绘制
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
				
				/****上升到高度的3/ 4，减速*****/
				//即距离最高点还有1/4的时候，减速

				if ((bullets[i].y - bullets[i].topY) * 4 < bullets[i].height)
					bullets[i].dt = rand() % 4 + 10;

				/****上升到最大高度*****/
				if (bullets[i].y <= bullets[i].topY) {
					//关闭点烟花的音效，并播放爆炸的音效，并重新打开点烟花的音效
					char c1[64],c2[64];
					sprintf(c1,"close s%d",i);
					sprintf(c2,"play f%d" ,i);
					mciSendString(c1,0,0,0);
					mciSendString(c2,0,0,0);

					sprintf_s(c1,sizeof(c1),"open fire/shoot.mp3 alias s%d",i);
					mciSendString(c1,0,0,0);

					//擦除烟花弹
					putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
					//准备渲染“烟花”
					fires[i].x = bullets[i].topX + 10;
					//在烟花弹中间爆炸
					fires[i].y = bullets[i].topY;
					//在最高点绽放
					fires[i].show = true;
					//开始绽放
					bullets[i].shoot = false;
					//停止发射
				}

				//更新烟花弹的时间
				bullets[i].t1 = bullets[i].t2;
			}
		}
	}
}

//绽放烟花
void showFire() {
	//烟花个阶段绽放时间间隔，制作变速绽放效果
	//为什么数组大小定义为16?
	//目前烟花的最大半径是155，准备以半径
	//10可刻度，不同的半径，绽放速度不同
	//半径越大，绽放越慢
	int drt[16] = { 5,5,5,5,5,6,25,25,25,25,55,55,55,55,55 };
	for (int i = 0; i < NUM; i++) {
		fires[i].t2 = timeGetTime();

		//增加爆炸半径，绽放烟花，增加时间间隔做变速效果
		if (fires[i].t2 - fires[i].t1 > fires[i].dt
			&& fires[i].show == true) {
			//更新烟花半径
			if (fires[i].r < fires[i].max_r) {
				fires[i].r++;
				fires[i].dt = drt[fires[i].r / 10];
				fires[i].draw = true;
			}
			//销毁烟花，并重新初始化该序号的飞弹和烟花
			if (fires[i].r >= fires[i].max_r) {
				fires[i].draw = false;
				initFire(i);
				//关闭爆炸音效，并重新打开爆炸音效
				char cmd[64];
				sprintf(cmd, "close f%d", i);
				mciSendString(cmd,0,0,0);
				sprintf_s(cmd, sizeof(cmd),"open fire/bomb.mp3 alias f%d", i);
				mciSendString(cmd,0,0,0);

			}
			//更新烟花的时间
			fires[i].t1 = fires[i].t2;
		}

		//绘制指定的烟花
		drawFire(i);
		
	}
}

void heartFire(DWORD& st1) {
	DWORD st2 = timeGetTime();
	if (st2 - st1 > 20000)//一首歌的时间{
	//先擦除正在发送的烟花弹
		for (int i = 0; i < 13; i++) {
			if (bullets[i].shoot)
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n],
					SRCINVERT);
			//心形坐标
			int x[13] = { 600,750,910,1000,950,750,600,450,250,150,250,410,600 };
			int y[13] = { 650,530,400,220,50,40,200,40,50,220,400,530,650 };
			for (int i = 0; i < NUM; i++)
			{
				bullets[i].x = x[i];
				bullets[i].y = y[i] + 750;//每个烟花弹的发射距离都是750，确保同时爆炸bullets[i]. topX = x[i];
				bullets[i].topY = y[i];

				bullets[i].height = bullets[i].y - bullets[i].topY;
				bullets[i].shoot = true;
				bullets[i].dt = 7;
				//显示烟花弹
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n],
					SRCINVERT);
				/****设置烟花参数***/
				fires[i].x = bullets[i].x + 10;
				fires[i].y = bullets[i].topY;
				fires[i].show = false;
				fires[i].r = 0;
			}
			st1 = st2;
		}
}

//void chose(DWORD& t1)  //t1位为上一次点烟花弹的时间
//{
//	DWORD t2 = timeGetTime();
//	if (t2 - t1 > 100)
//	{
//		int n = rand() % 30;//取摸的数字越大，烟花发射频率越慢，因为<13的概率就越低
//		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
//			/****重置烟花弹，预备发射*****/
//			bullets[n].x = rand() % 1200;
//			bullets[n].y = rand() % 100 + 600;
//			bullets[n].topX = bullets[n].x;
//			bullets[n].topY = rand() % 400;
//			bullets[n].height = bullets[n].y - bullets[n].topY;
//			bullets[n].shoot = true;
//
//		//绘制烟花的初始状态（即:在起始位置绘制烟花)
//			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n],
//			SRCINVERT);
//
//		/****播放每个烟花弹的声音****/
//		/*char cmd[50];
//		sprintf(cmd, "play s%d", n);
//		mciSendString(cmd, 0, 0, 0);*/
//	}
//	t1 = t2;
//}



void init() {
	initgraph(1200, 800);

	for (int i = 0;i < NUM;i++) {	//初始化烟花和烟花弹
		initFire(i);
	}

	loadFireImages();

	//这个函数用于获取绘制设备的显示缓冲区指针
	pMem = GetImageBuffer();	//获取窗口显存指针

	//打开音效并设置
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
	//welcome(); //绘制表白字幕

	//testFire();

	DWORD t1 = timeGetTime(); //筛选烟花时间
	DWORD ht1 = timeGetTime();//播放花样计时

	BeginBatchDraw();

	while (!kbhit()) {
		Sleep(10);

		clearImage();

		chose(t1);
		shoot();
		showFire();
		heartFire(ht1);
		FlushBatchDraw(); //显示前面的说有绘图操作
		
	}

	return 0;
}