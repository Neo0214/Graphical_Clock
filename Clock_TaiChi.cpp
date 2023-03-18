#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")//播放歌曲用

#define BackgroundWidth 600//窗口大小
#define Circle_R 200//圆盘半径
#define PI 3.1416
#define Text_position_x 150
#define Text_position_y 230
#define Number_Text_position_x -62
#define Number_Text_position_y Circle_R+50
using namespace std;

struct color
{
	float R;
	float G;
	float B;
};
struct Time_In_Text {
	char Hour_Tens;
	char Hour_Unit;
	char Minute_Tens;
	char Minute_Unit;
	char Second_Tens;
	char Second_Unit;
};

/***********************************
直线抗锯齿，默认混合背景黑色
**********************************/
void line_SDF_Alpha(double scope, int x_begin, int y_begin, int x_end, int y_end)
{//针对画线类型的抗锯齿
	int x_ba = x_begin - x_end, y_ba = y_begin - y_end;//向量ab
	double ba_square = double(x_ba) * x_ba + double(y_ba) * y_ba;//ab模平方
	int i, j, x_pa, y_pa;
	double  h, d, alpha;
	color Font{
		255,255,255
	};
	color Background{
		0,0,0
	};
	for (i = -20 - Circle_R; i < 20 + Circle_R; i++)
	{
		for (j = -20 - Circle_R; j < 20 + Circle_R; j++)
		{
			x_pa = x_begin - i;
			y_pa = y_begin - j;
			h = (double(x_pa) * x_ba + double(y_pa) * y_ba) / ba_square;
			if (h >= 0 && h <= 1)
			{
				d = sqrt((x_pa - x_ba * h) * (x_pa - x_ba * h) + (y_pa - y_ba * h) * (y_pa - y_ba * h));
				if (d < scope)
				{
					alpha = scope / 3 - d / 3;
					putpixel(i, j, RGB(Font.R * alpha + (1 - alpha) * Background.R, Font.G * alpha + (1 - alpha) * Background.G, Font.B * alpha + (1 - alpha) * Background.B));
				}
			}
		}
	}

}
/***********************************
圆抗锯齿，自动检测混合颜色
**********************************/
void circle_SDF_Alpha(int radius)
{//只针对圆心在原点的圆类抗锯齿
	int i, j;
	double distance, alpha;
	color Background_mix;//需要混合的背景色
	unsigned long get_mix;//获取背景色
	color Font{
		255,255,255
	};
	DWORD* buffer = GetImageBuffer();
	for (i = -Circle_R - 15; i < Circle_R + 15; i++)
	{
		for (j = -Circle_R - 15; j < Circle_R + 15; j++)
		{
			distance = radius - sqrt(i * i + j * j);//求出距离边缘距离
			if (distance > -1.5 && distance < 1.5)
			{//距离非常贴近
				alpha = 0.5 - fabs(distance) / 3;
				get_mix = getpixel(i, j);
				Background_mix.R = GetBValue(get_mix);
				Background_mix.G = GetGValue(get_mix);
				Background_mix.B = GetRValue(get_mix);
				buffer[(j + 300) * BackgroundWidth + i + 300] = RGB(
					(1 - alpha) * Background_mix.R + alpha * Font.R,
					(1 - alpha) * Background_mix.G + alpha * Font.G,
					(1 - alpha) * Background_mix.B + alpha * Font.B);
			}
		}
	}
	FlushBatchDraw();
}
void Run_TaiChi(double beta)
{
	int core_x = Circle_R / 2, core_y = Circle_R / 2, core_r = Circle_R / 2;
	setfillcolor(0xffffff);
	solidpie(int(core_r * sin(beta) - core_x),
		int(-core_r * cos(beta) - core_y),
		int(core_r * sin(beta) + core_x),
		int(-core_r * cos(beta) + core_y),
		-PI / 2 - beta, PI / 2 - beta);
	setfillcolor(0x000000);
	solidpie(int(core_r * sin(beta + PI) - core_x),
		int(-core_r * cos(beta + PI) - core_y),
		int(core_r * sin(beta + PI) + core_x),
		int(-core_r * cos(beta + PI) + core_y),
		PI / 2 - beta, 3 * PI / 2 - beta);
}
/***********************************
绘制关键数字
**********************************/
void Draw_Number()
{
	LOGFONT f;
	gettextstyle(&f);
	f.lfQuality = ANTIALIASED_QUALITY;
	_tcscpy_s(f.lfFaceName, _T("新宋体"));
	f.lfHeight = 36;
	f.lfWidth = 18;
	settextstyle(&f);
	outtextxy(-10, -Circle_R - 52, '0');
	outtextxy(-10, Circle_R + 12, '6');
	outtextxy(Circle_R + 12, -20, '3');
	outtextxy(-Circle_R - 32, -20, '9');
	return;
}
/***********************************
绘制太极初始状态
**********************************/
void Draw_TaiChi(int SecInHour, double beta)
{
	setfillcolor(0xffffff);//设置为白色
	solidpie(-Circle_R, -Circle_R, Circle_R, Circle_R, PI / 2 - beta, 3 * PI / 2 - beta);//太极白色部分
	setfillcolor(0x000000);
	solidpie(-Circle_R, -Circle_R, Circle_R, Circle_R, -PI / 2 - beta, PI / 2 - beta);//太极黑色部分
	return;
}
void Show_Time_In_Text(tm* t, int mode)
{
	char tmp = ' ' - ':';
	settextcolor(0x000000);
	LOGFONT f;
	gettextstyle(&f);
	f.lfQuality = ANTIALIASED_QUALITY;
	_tcscpy_s(f.lfFaceName, _T("新宋体"));
	f.lfHeight = 32;
	f.lfWidth = 16;
	settextstyle(&f);
	setbkmode(OPAQUE);
	for (int i = Number_Text_position_x - 130; i < Number_Text_position_x + 150; i++)
		outtextxy(i, Number_Text_position_y, ' ');
	Time_In_Text Mode_Soft{
		char(t->tm_hour) / 10 + '0', char(t->tm_hour) % 10 + '0',
		char(t->tm_min) / 10 + '0', char(t->tm_min) % 10 + '0',
		char(t->tm_sec) / 10 + '0', char(t->tm_sec) % 10 + '0'
	};
	Time_In_Text Mode_Fight{
		char((23 - t->tm_hour)) / 10 + '0',char((23 - t->tm_hour)) % 10 + '0',
		char((59 - t->tm_min)) / 10 + '0',char((59 - t->tm_min)) % 10 + '0',
		char((60 - t->tm_sec)) / 10 + '0',char((60 - t->tm_sec)) % 10 + '0'
	};
	settextcolor(0xffffff);
	setbkmode(TRANSPARENT);
	if (mode == 1)
	{
		LPCTSTR Time_Left_Text = _T("今日剩余");
		settextcolor(RGB(255, 0, 0));
		outtextxy(Number_Text_position_x - 130, Number_Text_position_y, Time_Left_Text);
		settextcolor(0xffffff);
		outtextxy(Number_Text_position_x, Number_Text_position_y, Mode_Fight.Hour_Tens);
		outtextxy(Number_Text_position_x + 20, Number_Text_position_y, Mode_Fight.Hour_Unit);
		outtextxy(Number_Text_position_x + 32, Number_Text_position_y, ':' + t->tm_sec % 2 * tmp);
		outtextxy(Number_Text_position_x + 45, Number_Text_position_y, Mode_Fight.Minute_Tens);
		outtextxy(Number_Text_position_x + 65, Number_Text_position_y, Mode_Fight.Minute_Unit);
		outtextxy(Number_Text_position_x + 77, Number_Text_position_y, ':' + t->tm_sec % 2 * tmp);
		outtextxy(Number_Text_position_x + 90, Number_Text_position_y, Mode_Fight.Second_Tens);
		outtextxy(Number_Text_position_x + 110, Number_Text_position_y, Mode_Fight.Second_Unit);
		return;
	}
	outtextxy(Number_Text_position_x, Number_Text_position_y, Mode_Soft.Hour_Tens);
	outtextxy(Number_Text_position_x + 20, Number_Text_position_y, Mode_Soft.Hour_Unit);
	outtextxy(Number_Text_position_x + 32, Number_Text_position_y, ':' + t->tm_sec % 2 * tmp);
	outtextxy(Number_Text_position_x + 45, Number_Text_position_y, Mode_Soft.Minute_Tens);
	outtextxy(Number_Text_position_x + 65, Number_Text_position_y, Mode_Soft.Minute_Unit);
	outtextxy(Number_Text_position_x + 77, Number_Text_position_y, ':' + t->tm_sec % 2 * tmp);
	outtextxy(Number_Text_position_x + 90, Number_Text_position_y, Mode_Soft.Second_Tens);
	outtextxy(Number_Text_position_x + 110, Number_Text_position_y, Mode_Soft.Second_Unit);
}
void init(tm* t)
{//用于绘制静态部分的函数与初始化
	int i;
	for (i = 1; i < 12; i++)
	{//打印5分标线
		if (i != 3 && i != 6 && i != 9)
			line_SDF_Alpha(2, int((Circle_R + 14) * sin(PI * i / 6)), int(-(Circle_R + 14) * cos(PI * i / 6)), int((Circle_R + 29) * sin(PI * i / 6)), int(-(Circle_R + 29) * cos(PI * i / 6)));
	}
	for (i = 0; i < 60; i++)
	{//打印分钟线
		if (i % 5 != 0)
			line_SDF_Alpha(1.5, int((Circle_R + 14) * sin(PI * i / 30)), int(-(Circle_R + 14) * cos(PI * i / 30)), int((Circle_R + 20) * sin(PI * i / 30)), int(-(Circle_R + 20) * cos(PI * i / 30)));
	}
	Draw_Number();//绘制重点数字
	int SecInHalfday = t->tm_hour % 12 * 3600 + t->tm_min * 60 + t->tm_sec;
	//十二小时制内，已经过去的秒数
	int SecInHour = t->tm_min * 60 + t->tm_sec;//该小时已经过去了多少秒
	float H;
	double beta = t->tm_sec * PI / 30;
	Draw_TaiChi(SecInHour, beta);//绘制太极图
	int core_x = Circle_R / 2, core_y = Circle_R / 2, core_r = Circle_R / 2;
	setfillcolor(0xffffff);
	solidpie(int(core_r * sin(beta) - core_x),
		int(-core_r * cos(beta) - core_y),
		int(core_r * sin(beta) + core_x),
		int(-core_r * cos(beta) + core_y),
		0, 2 * PI);
	setfillcolor(0x000000);
	solidpie(int(core_r * sin(beta + PI) - core_x),
		int(-core_r * cos(beta + PI) - core_y),
		int(core_r * sin(beta + PI) + core_x),
		int(-core_r * cos(beta + PI) + core_y),
		0, 2 * PI);
	setrop2(R2_MASKPENNOT);
	setfillcolor(0xffffff);
	solidcircle(int(Circle_R / 2 * sin(PI * SecInHour / 1800)), int(-Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	solidcircle(int(-Circle_R / 2 * sin(PI * SecInHour / 1800)), int(Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	setrop2(R2_COPYPEN);
	//绘制太极点，根据分针情况
	setlinestyle(PS_SOLID, 2);
	for (H = 0; H < SecInHalfday / 120.0; H += 0.1f)
	{
		setlinecolor(HSLtoRGB(H, 1, 0.7f));
		line(int((Circle_R + 1) * sin(PI * H / 180)), int(-(Circle_R + 1) * cos(PI * H / 180)), int((Circle_R + 9) * sin(PI * H / 180)), int(-(Circle_R + 9) * cos(PI * H / 180)));
	}//绘制彩环
	circle_SDF_Alpha(Circle_R + 10);
	circle_SDF_Alpha(Circle_R);
	//绘制两层外环

	//底部时间
	Show_Time_In_Text(t, 0);
}
void erasure(tm* t_past)
{
	int SecInHour = t_past->tm_min * 60 + t_past->tm_sec;
	setrop2(R2_NOT);
	setfillcolor(0xffffff);
	solidcircle(int(Circle_R / 2 * sin(PI * SecInHour / 1800)), int(-Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	solidcircle(int(-Circle_R / 2 * sin(PI * SecInHour / 1800)), int(Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	setrop2(R2_COPYPEN);
}
void re_create(tm* t)
{
	int SecInHour = t->tm_min * 60 + t->tm_sec;
	setrop2(R2_MASKPENNOT);
	setfillcolor(0xffffff);
	solidcircle(int(Circle_R / 2 * sin(PI * SecInHour / 1800)), int(-Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	solidcircle(int(-Circle_R / 2 * sin(PI * SecInHour / 1800)), int(Circle_R / 2 * cos(PI * SecInHour / 1800)), Circle_R / 8);
	setrop2(R2_COPYPEN);
	if (t->tm_hour % 12 == 0 && t->tm_sec == 0 && t->tm_min == 0)
	{//将迎来新的一圈，色轮全部清除
		setlinecolor(0x000000);
		setlinestyle(PS_SOLID, 2);
		for (int i = Circle_R; i < Circle_R + 10; i++)
			circle(0, 0, i);
		circle_SDF_Alpha(Circle_R + 10);
		circle_SDF_Alpha(Circle_R);
		return;
	}
	float H;
	int SecInHalfday = t->tm_hour % 12 * 3600 + t->tm_min * 60 + t->tm_sec;
	setlinestyle(PS_SOLID, 2);
	for (H = SecInHalfday / 120.0f; H < SecInHalfday / 120.0; H += 0.1f)
	{
		setlinecolor(HSLtoRGB(H, 1, 0.7f));
		line(int((Circle_R + 1) * sin(PI * H / 180)), int(-(Circle_R + 1) * cos(PI * H / 180)), int((Circle_R + 9) * sin(PI * H / 180)), int(-(Circle_R + 9) * cos(PI * H / 180)));
	}
	circle_SDF_Alpha(Circle_R + 10);
	circle_SDF_Alpha(Circle_R);
}


int main()
{
	struct tm t, t_past;
	time_t now;
	time(&now);
	localtime_s(&t, &now);
	time_t past = now;//记录当前时间为past
	bool soft_or_fight = 0;
	double beta = t.tm_sec * PI / 30;

	initgraph(BackgroundWidth, BackgroundWidth);
	setorigin(BackgroundWidth / 2, BackgroundWidth / 2);//设置画布中心为原点
	init(&t);//初始化表盘等图形

	LPCTSTR soft_or_fight_text[2] = { _T("当前模式：柔和纯享"), _T("当前模式：奋战DDL") };
	LPCTSTR shift_mode = _T("按此切换模式");
	settextstyle(16, 8, _T("宋体"));
	outtextxy(Text_position_x, Text_position_y, soft_or_fight_text[soft_or_fight]);
	outtextxy(Text_position_x, Text_position_y + 30, shift_mode);
	MCI_OPEN_PARMS open[2];
	open[soft_or_fight].lpstrElementName = _T(".\\soft.mp3");//0元素播放soft
	open[!soft_or_fight].lpstrElementName = _T(".\\fight.mp3");
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, DWORD_PTR(&open[0]));
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, DWORD_PTR(&open[1]));
	MCI_PLAY_PARMS play;
	mciSendCommand(open[soft_or_fight].wDeviceID, MCI_PLAY, 0, DWORD_PTR(&play));//默认播放歌曲soft
	struct MOUSEMSG mouse;


	while (!_kbhit())
	{

		if (MouseHit())
		{
			mouse = GetMouseMsg();
			if (mouse.x >= Text_position_x + BackgroundWidth / 2
				&& mouse.x <= Text_position_x + 128 + BackgroundWidth / 2
				&& mouse.y >= Text_position_y + 20 + BackgroundWidth / 2
				&& mouse.y <= Text_position_y + 50 + BackgroundWidth / 2)
			{//鼠标移动到切换选项上
				settextcolor(0x00ff00);//字体切换为其他色
				settextstyle(16, 8, _T("宋体"));
				outtextxy(Text_position_x, Text_position_y + 30, shift_mode);//选项卡切换提示颜色
				if (mouse.mkLButton)
				{//按下切换选项卡
					settextcolor(0xffffff);
					setbkmode(OPAQUE);
					outtextxy(Text_position_x, Text_position_y, _T("                            "));
					outtextxy(Text_position_x, Text_position_y, soft_or_fight_text[!soft_or_fight]);
					mciSendCommand(open[soft_or_fight].wDeviceID, MCI_STOP, 0, 0);//停止当前播放
					mciSendCommand(open[soft_or_fight = !soft_or_fight].wDeviceID, MCI_PLAY, 0, DWORD_PTR(&play));//播放新歌曲
				}
			}
			else
			{
				settextcolor(0xffffff);
				settextstyle(16, 8, _T("宋体"));
				outtextxy(Text_position_x, Text_position_y + 30, shift_mode);
			}
		}
		time(&now);
		if (now == past)
			continue;
		//时间不变时，重新读取，直到时间变化


		localtime_s(&t, &now);//t变为新时间
		localtime_s(&t_past, &past);//t_past为上一秒
		erasure(&t_past);//用于擦除上一秒的痕迹
		beta += PI / 30;
		Run_TaiChi(beta - 0.05);
		Run_TaiChi(beta);
		re_create(&t);
		Show_Time_In_Text(&t, soft_or_fight);
		past = now;//全部完成绘制，记录上一秒时间为过去一秒
	}


	char ch;
	ch = _getch();
	closegraph();
	return 0;
}
