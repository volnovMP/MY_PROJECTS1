
#include "opred2.h"
#include "extern2.h"
//===============================================================
void main_win(void)
{
	char nom_serv[10];
	int kk,ll,mm;
	strcpy(nom_serv,"СЕРВЕР-");
	nom_serv[7]=SERVER+48;
	nom_serv[8]=0;
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();
	puts1("ПРОГРАММА РЕЛЕЙНО-ПРОЦЕССОРНОЙ ЦЕНТРАЛИЗАЦИИ ВНИИАС",0xc,15,5);
	puts1("СТАНЦИЯ 'ОРСК'.ВЕРСИЯ 3.0(30.01.2007)",0xc,20,7);
	puts1("F1-просмотр каналов обмена",0xa,2,20);
	puts1("F2-общий просмотр состояния объектов",0xa,2,21);
	puts1("F3-анализ каналов и объектов",0xa,2,22);
	puts1("F4-просмотр конфигурации АРМов",0xa,2,23);
	puts1("F5-просмотр принятых-выданных команд",0xa,2,24);
	puts1("F7-просмотр маршрутов",0xa,2,25);
	GRAND_TEXT(20,10,nom_serv);

	//Изображение АРМ ШН
	putch1(0xda,7,70,20);
	for(kk=1;kk<7;kk++)putch1(0xc4,7,kk+70,20);
	putch1(0xbf,7,77,20);
	putch1(0xb3,7,70,21);
	puts1("АРМ-ШН",7,71,21);
	putch1(0xb3,7,77,21);
	putch1(0xc0,7,70,22);
	for(kk=1;kk<7;kk++)putch1(0xc4,7,kk+70,22);
	putch1(0xd9,7,77,22);

	for(kk=0;kk<3;kk++)putch1(0xc4,1,77+kk,21);
	for(kk=0;kk<65;kk++)putch1(0xCD,6,10+kk,27);
	for(kk=0;kk<73;kk++)putch1(0xCD,6,3+kk,48);

	for(kk=0;kk<46;kk++)putch1(0xCD,2,3+kk,38);putch1(0xbb,2,49,38);
	for(kk=0;kk<7;kk++)putch1(0xba,2,49,kk+39);
	for(kk=0;kk<27;kk++)putch1(0xba,1,80,kk+20);
	for(kk=0;kk<62;kk++)putch1(0xCD,2,10+kk,35);

	for(ll=0;ll<Narm-1;ll++)
	{
		putch1(0x1e,6,11+9*ll,27);
		putch1(0xb3,6,11+9*ll,28);
		putch1(0x1f,6,11+9*ll,29);

		putch1(0xda,7,8+9*ll,30);
		for(kk=1;kk<6;kk++)putch1(0xc4,7,kk+8+9*ll,30);
		putch1(0xbf,7,14+9*ll,30);
		putch1(0xb3,7,8+9*ll,31);
		putch1('А',7,9+9*ll,31);
		putch1('Р',7,10+9*ll,31);
		putch1('М',7,11+9*ll,31);
		putch1(ll+49,7,12+9*ll,31);//номер АРМа
		putch1(0xb3,7,14+9*ll,31);
		putch1(0xc0,7,8+9*ll,32);
		for(kk=0;kk<5;kk++)putch1(0xc4,7,kk+9+9*ll,32);
		putch1(0xd9,7,14+9*ll,32);

		putch1(0x1e,2,11+9*ll,33);
		putch1(0xb3,2,11+9*ll,34);
		putch1(0x1f,2,11+9*ll,35);
	}
	for(ll=0;ll<Nst;ll++)
	{

		putch1(0x1e,2,4+6*ll,38);
		putch1(0xb3,2,4+6*ll,39);
		putch1(0xb3,2,4+6*ll,40);
		putch1(0x1f,2,4+6*ll,41);

		putch1(0xda,7,2+6*ll,42);
		for(kk=0;kk<3;kk++)putch1(0xc4,7,kk+3+6*ll,42);
		putch1(0xbf,7,6+6*ll,42);
		putch1(0xb3,7,2+6*ll,43);

		putch1('У',7,3+6*ll,43);
		putch1('В',7,4+6*ll,43);
		putch1('К',7,5+6*ll,43);
		putch1(0xb3,7,6+6*ll,43);

		putch1(0xb3,7,2+6*ll,44);
		putch1(' ',7,3+6*ll,44);
		putch1(ll+49,7,4+6*ll,44);//номер ТУМСа
		putch1(' ',7,5+6*ll,44);
		putch1(0xb3,7,6+6*ll,44);
		putch1(0xc0,7,2+6*ll,45);

		for(kk=0;kk<3;kk++)putch1(0xc4,7,kk+3+6*ll,45);
		putch1(0xd9,7,6+6*ll,45);
		putch1(0x1e,6,4+6*ll,46);
		putch1(0xb3,6,4+6*ll,47);
		putch1(0x1f,6,4+6*ll,48);
	}
	for(ll=0;ll<3;ll++)
	{
		putch1(0xda,7,53+9*ll,43-2*ll);  // ┌
		for(kk=0;kk<4;kk++)putch1(0xc4,7,kk+54+9*ll,43-2*ll); // ─
		putch1(0xbf,7,58+9*ll,43-2*ll);  // ┐

		putch1(0xb3,7,53+9*ll,44-2*ll);  //│
		putch1(' ',7,54+9*ll,44-2*ll);
		putch1('C',7,55+9*ll,44-2*ll);
		putch1(ll+49,7,56+9*ll,44-2*ll);  //номер сервера
		putch1(' ',7,57+9*ll,44-2*ll);
		putch1(0xb3,7,58+9*ll,44-2*ll);  // │
		putch1(0xb3,7,53+9*ll,45-2*ll);  // │
		putch1(' ',7,54+9*ll,45-2*ll);
		putch1(' ',7,55+9*ll,45-2*ll);
		putch1(' ',7,56+9*ll,45-2*ll);
		putch1(' ',7,57+9*ll,45-2*ll);
		putch1(0xb3,7,58+9*ll,45-2*ll);  // │
		putch1(0xc0,7,53+9*ll,46-2*ll);  // └
		for(kk=0;kk<4;kk++)putch1(0xc4,7,kk+54+9*ll,46-2*ll); //─
		putch1(0xd9,7,58+9*ll,46-2*ll);  // ┘

		for(kk=10*ll,mm=0;kk<(21+ll);kk++,mm++)putch1(0xc4,1,mm+59+9*ll,45-2*ll);

		//основные связи с УВК
		putch1(0x11,2,49,44-2*ll);    // <-
		for(kk=0;kk<(3+9*ll);kk++)putch1(0xc4,2,50+kk,44-2*ll);
		putch1(0x10,2,50+kk,44-2*ll); // ->

		//основные связи с АРМ
		putch1(0x1e,2,54+ll*9,35);
		for(kk=0;kk<7-2*ll;kk++)putch1(0xb3,2,54+ll*9,36+kk);
		putch1(0x1f,2,54+ll*9,36+kk);


		//резервные связи с АРМ
		putch1(30,6,56+9*ll,27);
		for(kk=0;kk<15-2*ll;kk++)putch1(0xb3,6,56+9*ll,28+kk);
		putch1(31,6,56+9*ll,43-2*ll);

		//резервные связи с УВК
		putch1(0x1e,6,54+9*ll,46-2*ll);
		for(kk=0;kk<2*ll+2;kk++)putch1(0xb3,6,54+9*ll,47-2*ll+kk);
		putch1(0x1f,6,54+9*ll,47-2*ll+kk-1);

	}
	return;
}

//=============================================================
void win_object(void)
{
	int i,j,k,st,soob;
	unsigned char nomer[6],atr;
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();textattr(0xf);
	for(i=1;i<=46;i++)
	{
		if(i<10){nomer[0]=' ';nomer[1]=i+48;}
		else {nomer[0]=i/10+48;nomer[1]=i%10+48;}
		for(j=0;j<=6;j++)
			for(k=0;k<2;k++)putch1(nomer[k],0xf,k+9+j*10,i+1);
	}
	putch1(201,0xa,1,1);
	for(i=1;i<77;i++)putch1(205,0xa,i+1,1);
	putch1(187,0xa,78,1);
	nomer[0]='У';nomer[1]='В';nomer[2]='К';
	for(i=0;i<Nst;i++)
	{
		nomer[3]=i+49;
		for(j=0;j<4;j++)putch1(nomer[j],0xf,i*10+3+j,1);
	}
	for(j=0;j<=46;j++)
	for(i=0;i<Nst;i++)
	{
		k=i+i*9+1;
		if(i==0)k=1;
		if(j<=STR[i])atr=0xa;
		else
			if(j<=(STR[i]+SIG[i]))atr=0xe;
			else
				if(j<=(STR[i]+SIG[i]+DOP[i]))atr=0xd;
				else
					if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]))atr=0x6;
					else
						if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]))atr=0xf;
						else
							if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]+UPR[i]))atr=0x9;
							else atr=0xc;
		if(j==0)
		{
			if(k==1)putch1(201,atr,k,j+1);
			else
				if(k>75)putch1(187,atr,k,j+1);
				else putch1(203,atr,k,j+1);
		}
		else putch1(186,atr,k,j+1);
	}
	for(j=0;j<=46;j++)
  for(i=0;i<Nst;i++)
  {
    k=i*10+8;
    if(j<=STR[i])atr=0xa;
    else
			if(j<=(STR[i]+SIG[i]))atr=0xe;
      else
				if(j<=(STR[i]+SIG[i]+DOP[i]))atr=0xd;
        else
        	if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]))atr=0x6;
          else
          	if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]))atr=0xf;
            else
            	if(j<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]+UPR[i]))atr=0x9;
              else atr=0xc;
    if(j==0)
    {
			if(k==1)putch1(201,atr,k,j+1);
      else
				if(k>75)putch1(187,atr,k,j+1);
				else putch1(203,atr,k,j+1);
    }
		else putch1(186,atr,k,j+1);
  }
	puts1("█-стрелки",0xa,2,48);
	puts1("█-сигналы",0xe,2,49);
	puts1("█-доп.об-ты",0xd,12,48);
	puts1("█-СП и УП",0x6,12,49);
	puts1("█-пути",0xf,24,48);
	puts1("█-эл-ты УВК",0x9,24,49);
	puts1("█-ненормы",0xc,36,48);
	puts1("█-старое",0xb,36,49);
	for(i=0;i<Nst;i++)
	{
    for(soob=0;soob<48;soob++)
    {
    	st=i+1;//сформировать номер стойки
      VVOD[i][soob][6]=0;
      if(VVOD[i][soob][1]!=0)
			for(j=0;j<6;j++)putch1(VVOD[i][soob][j],0xb,j+2+(st-1)*10,soob+2);
			if(soob==44)putch1(VVOD[i][soob][5],0xb,7+(st-1)*10,46);
		}
	}
	return;
}
//======================================================
void win_analiz(void)
{
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();textmode(64);
	_setcursortype(_NOCURSOR);
	textattr(0xC);
	puts1("АНАЛИЗ КАНАЛОВ И ОБЪЕКТОВ (F6-Пуск/стоп)",0xc,20,1);
	puts1("F10 + номер  - просмотр состояния объекта в сервере",0xc,10,3);
	X_ANALIZ_OUT=0;
	Y_ANALIZ_OUT=5;
	X1_ANALIZ_OUT=0;
	Y1_ANALIZ_OUT=6;
}
//=============================================================
void win_konfig(void)
{
	int i,j;
	unsigned char upr;
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();textmode(64);
	_setcursortype(_NOCURSOR);
	puts1("КОНФИГУРАТОР АРМ",0xa,33,1);
	puts1("АРМЫ",0xa,23,3);
	puts1("4  5  6  7  8  9",0xa,17,4);
	puts1("Район 1",0xa,7,6);
	puts1("Район 2",0xa,7,8);
  putch1(0xC9,0xa,15,5);
	for(i=0;i<Narm;i++)puts1("══╦",0xa,16+i*3,5);
	putch1(0xbb,0xa,15+Narm*3,5);
  putch1(0xBA,0xa,15,6);
	for(i=0;i<Narm;i++)puts1("  ║",0xa,16+i*3,6);
	putch1(0xba,0xa,15+Narm*3,6);
  putch1(0xcc,0xa,15,7);
	for(i=0;i<Narm;i++)puts1("══╬",0xa,16+i*3,7);
	putch1(0xb9,0xa,15+Narm*3,7);
  putch1(0xBA,0xa,15,8);
	for(i=0;i<Narm;i++)puts1("  ║",0xa,16+i*3,8);
	putch1(0xba,0xa,15+Narm*3,8);
  putch1(0xc8,0xa,15,9);
	for(i=0;i<Narm;i++)puts1("══╩",0xa,16+i*3,9);
	putch1(0xbc,0xa,15+Narm*3,9);
	for(i=0;i<Narm;i++) //пройти по всем АРМ
	{
		for(j=0;j<Nranj;j++)//пройти по всем районам
		{
			upr='0';
			//если район управляется этим АРМом
			if(KONFIG_ARM[i][j]==0xFF)upr='█';
			if(KONFIG_ARM[i][j]==0x1)upr='1';
			if(KONFIG_ARM[i][j]==0x2)upr='2';
			putch1(upr,0xa,17+i*3,6+j*2);
		}
	}
	return;
}
//=============================================================
void win_comm(void)
{
	int i;
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();textmode(64);
	_setcursortype(_NOCURSOR);
	puts1("КОМАНДЫ АРМА ДСП",0xA,13,1);
	putch1('╔',0xA,2,2);
	for(i=0;i<45;i++)putch1('═',0xa,3+i,2);
	putch1('╗',0xa,48,2);
	for(i=3;i<49;i++)putch1('║',0xa,2,i);
	putch1('╚',0xa,2,48);
	for(i=0;i<45;i++)putch1('═',0xa,3+i,48);
	putch1('╝',0xa,48,48);
	for(i=3;i<48;i++)putch1('║',0xa,48,i);
	puts1("КОМАНДЫ СЕРВЕРА",0xa,60,1);
	putch1('╔',0xa,51,2);
	for(i=0;i<26;i++)putch1('═',0xa,52+i,2);
	putch1('╗',0xa,78,2);
	for(i=3;i<49;i++)putch1('║',0xa,51,i);
	putch1('╚',0xa,51,48);
	for(i=0;i<26;i++)putch1('═',0xa,52+i,48);
	putch1('╝',0xa,78,48);
	for(i=3;i<48;i++)putch1('║',0xa,78,i);
	return;
}
//=====================================================================
void GRAND_TEXT(int X,int Y,char *TXT)
{
	int i=0;
  unsigned char BUKV;
	textmode(64);
	_setcursortype(_NOCURSOR);
	while(TXT[i]!=0)
	{
    BUKV=TXT[i];
		switch(BUKV)
		{
			case 0x91: 	 puts1("▄▄▄",0xa,X+4*i,Y); // "С"
                  putch1('█',0xa,X+4*i,Y+1);
                  putch1('█',0xa,X+4*i,Y+2);
                  putch1('█',0xa,X+4*i,Y+3);
                  putch1('█',0xa,X+4*i,Y+4);
									 puts1("▀▀▀",0xa,X+4*i,Y+5);
    							break;

      case 0x85:	 puts1("▄▄▄",0xa,X+4*i,Y); // "Е"
                  putch1('█',0xa,X+4*i,Y+1);
                   puts1("█▄▄",0xa,X+4*i,Y+2);
                  putch1('█',0xa,X+4*i,Y+3);
                  putch1('█',0xa,X+4*i,Y+4);
									 puts1("▀▀▀",0xa,X+4*i,Y+5);
    							break;
      case 0x90:	puts1("▄▄▄",0xa,X+4*i,Y); // "Р"
                  puts1("█ █",0xa,X+4*i,Y+1);
                  puts1("█ █",0xa,X+4*i,Y+2);
                  puts1("█ █",0xa,X+4*i,Y+3);
                  puts1("█▀▀",0xa,X+4*i,Y+4);
  							 putch1('▀',0xa,X+4*i,Y+5);
                  break;
			case 0x82:  puts1("▄▄▄",0xa,X+4*i,Y); // "В"
                  puts1("█ █",0xa,X+4*i,Y+1);
                  puts1("█▄▌",0xa,X+4*i,Y+2);
                  puts1("█ █",0xa,X+4*i,Y+3);
                  puts1("█ █",0xa,X+4*i,Y+4);
									puts1("▀▀▀",0xa,X+4*i,Y+5);
                  break;
      case 0x2d:	puts1("▄▄▄",0xa,X+4*i,Y+2); // "-"
    							break;
      case 0x31:  puts1(" █",0xa,X+4*i,Y); // "1"
                  puts1("▐█",0xa,X+4*i,Y+1);
									puts1(" █",0xa,X+4*i,Y+2);
                  puts1(" █",0xa,X+4*i,Y+3);
									puts1(" █",0xa,X+4*i,Y+4);
									puts1("▀▀▀",0xa,X+4*i,Y+5);
                  break;
      case 0x32:  puts1("█▀█",0xa,X+4*i,Y); // "2"
                  puts1("  █",0xa,X+4*i,Y+1);
                  puts1("  █",0xa,X+4*i,Y+2);
                  puts1("▄▀  ",0xa,X+4*i,Y+3);
                  puts1("█ ",0xa,X+4*i,Y+4);
									puts1("▀▀▀",0xa,X+4*i,Y+5);
                  break;
      case 0x33:  puts1("█▀█",0xa,X+4*i,Y); // "3"
                  puts1("  █",0xa,X+4*i,Y+1);
                  puts1(" ▐ ",0xa,X+4*i,Y+2);
                  puts1("  █",0xa,X+4*i,Y+3);
                  puts1("▄ █",0xa,X+4*i,Y+4);
									puts1("▀▀▀",0xa,X+4*i,Y+5);
                  break;

			default:		break;
		}
  i++;
  }
	return;
}
//=============================================================
void tablica(void)
{
	char arm[15],tums[15],server[15];
	unsigned int i,j;

	for(i=0;i<15;i++)tums[i]=0;
	strcpy(tums,"УВК-");
	strcpy(server,"СЕРВЕР-");
	for(i=0;i<15;i++)arm[i]=0;
	strcpy(arm,"АРМ");
	//подготовка экрана
	textmode(64);
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();
	for(i=1;i<9;i++)
	{
		tums[4]=i+48;
		atrib=0xC;
		for(j=0;j<5;j++)putch1(tums[j],atrib,j+1,(i-1)*6+1);

		atrib=0xa;
		for(j=6;j<34;j++)putch1(205,atrib,j,(i-1)*6+1); //═
		for(j=6;j<34;j++)putch1(205,atrib,j,(i-1)*6+6); //═
		for(j=2;j<6;j++)putch1(186,atrib,6,(i-1)*6+j);
		for(j=2;j<6;j++)putch1(186,atrib,34,(i-1)*6+j);//║
		putch1(201,atrib,6,(i-1)*6+1);
		putch1(187,atrib,34,(i-1)*6+1);   //╔ ╗
		puts1("ОСН",atrib,1,(i-1)*6+2);
		puts1("=>",atrib,32,(i-1)*6+2);
		puts1("=>",atrib,4,(i-1)*6+3);
		puts1("РЕЗ",atrib,1,(i-1)*6+4);
		puts1("=>",atrib,32,(i-1)*6+4);
		puts1("=>",atrib,4,(i-1)*6+5);

		putch1(200,atrib,6,(i-1)*6+6);
		putch1(188,atrib,34,(i-1)*6+6); //╚  //начало основного цикла программы
	}
	puts1("ТЕКУЩИЙ",0xc,52,1);
	server[7]=SERVER+48; server[8]=0;
	puts1(server,0xc,60,1);
	atrib=0xA;
	for(j=42;j<77;j++)putch1(205,atrib,j,2); //═
	for(j=42;j<77;j++)putch1(205,atrib,j,8); //═
	for(j=42;j<77;j++)putch1(196,atrib,j,5); //─
	for(j=2;j<9;j++)putch1(186,atrib,42,j);
	for(j=2;j<9;j++)putch1(186,atrib,77,j);//║
	putch1(201,atrib,42,2);putch1(187,atrib,77,2);   //╔ ╗

	puts1("След=>",atrib,36,3);
	puts1("=>",atrib,75,4);
	puts1("Пред=>",atrib,36,6);
	puts1("=>",atrib,75,7);

	putch1(199,atrib,42,5); 	putch1(182,atrib,77,5);  //╟  ╢
	putch1(200,atrib,42,8);//╚
	putch1(188,atrib,77,8); //╝

//============================= АРМы ============================
	puts1("ЗАПРОСЫ В АРМы",0xc,52,9);
	for(j=40;j<77;j++)putch1(205,atrib,j,10); //═
	for(j=40;j<77;j++)putch1(205,atrib,j,16); //═
	putch1(196,atrib,j,13); //─
	for(j=2;j<7;j++)putch1(186,atrib,40,j+9);    //║
	for(j=2;j<7;j++)putch1(186,atrib,77,j+9);    //║
	putch1(201,atrib,40,10);
	putch1(187,atrib,77,10);  //╔ ╗

 	puts1("ОСН",0xc,36,11);
	puts1("РЕЗ",0xc,36,14);
	puts1("=>",0xc,78,11);
	puts1("=>",0xc,78,14);
	putch1(199,0xc,40,13);
	putch1(182,0xc,77,13);  //╟  ╢
	putch1(200,atrib,40,16);
	putch1(188,atrib,77,16); //╚

	puts1("ОТВЕТЫ ИЗ АРМов",0xa,52,17);
	for(j=42;j<79;j++)putch1(205,atrib,j,18); //═
	putch1(201,atrib,42,18);
	putch1(187,atrib,79,18);  //╔ ╗
	for(i=1;i<6;i++)
	{ arm[3]=i+48;
		puts1(arm,0xc,37,(i-1)*6+18);
		atrib=0xa;textattr(atrib);
		for(j=43;j<79;j++)putch1(205,atrib,j,(i-1)*6+24); //═
		for(j=42;j<79;j++)putch1(196,atrib,j,(i-1)*6+21); //─
		for(j=3;j<8;j++)putch1(186,atrib,42,(i-1)*6+j+16);    //║
		for(j=3;j<8;j++)putch1(186,atrib,79,(i-1)*6+j+16); //║
		puts1("ОСН=>",atrib,37,(i-1)*6+19);
		puts1("РЕЗ=>",atrib,37,(i-1)*6+22);
		putch1(199,atrib,42,(i-1)*6+21);
		putch1(182,atrib,79,(i-1)*6+21);  //╟  ╢
		putch1(204,atrib,42,(i-1)*6+24);  //╠
		putch1(185,atrib,79,(i-1)*6+24);//╣
	}
	putch1(200,atrib,42,(i-2)*6+24);  //╚
	putch1(188,atrib,79,(i-2)*6+24);//╝
  return;
}
//==================================================================
void win_gash(void)
{
  int ii;
  if(REGIM==0)
	{
		putch1(17,1,77,21); //стрелочка из осн.магистрали ШН в АРМ-ШН
		putch1(16,1,80,21); //стрелочка из АРМ-ШН в осн.магистрали ШН
		for(ii=0;ii<3;ii++)
		{
			putch1(30,2,54+9*ii,35);      //стрелочки на основной канал АРМ
			putch1(31,2,54+9*ii,43-2*ii); //стрелочки из АРМов на основной канал

			putch1(30,6,56+9*ii,27);      //стрелочки на резервный канал АРМ
			putch1(31,6,56+9*ii,43-2*ii); //стрелочки из резервной шины АРМ в сервер

			putch1(17,2,49,44-2*ii);     //стрелочки из серверов на основной УВК
			putch1(16,2,53+9*ii,44-2*ii); //стрелочки в серверы из основного УВК

			putch1(31,6,54+9*ii,48);    //стрелочки из серверов в резервный УВК
			putch1(30,6,54+9*ii,46-2*ii);//стрелочки в серверы из резервного УВК


			putch1(17,1,58+9*ii,45-2*ii);//стрелочки магистрали осн.ШН в серверы
			putch1(16,1,80,45-2*ii);//стрелочки из серверов в магистраль осн. ШН


			putch1(ii+49,7,56+9*ii,44-2*ii);//номер активного сервера
    }

		for(ii=0;ii<Nst;ii++)
    {
				putch1(30,2,4+6*ii,38);
    		putch1(31,2,4+6*ii,41);
				putch1(30,6,4+6*ii,46);
				putch1(31,6,4+6*ii,48);
    }

		for(ii=0;ii<Narm-1;ii++)
    {
			putch1(30,2,11+9*ii,33);
			putch1(31,2,11+9*ii,35);
			putch1(30,6,11+9*ii,27);
			putch1(31,6,11+9*ii,29);
		}

		if(ACTIV==1)
		{ ii=SERVER-1;
			putch1(ii+49,0x8a,56+9*ii,44-2*ii);
    }
	}
	return;
}
//==========================================================================
void obnovi(int obj1)
{
	NOVIZNA[nom_new++]=obj1;
	PEREDACHA[obj1]=PEREDANO;
  if(nom_new>=MAX_NEW)nom_new=0;
}

//======================================================
void win_marsh(void)
{
	int i;
	window(1,49,47,49);clrscr();
	window(1,1,80,48);
	clrscr();textmode(64);
	_setcursortype(_NOCURSOR);
	textattr(0xC);
	puts1("СОСТОЯНИЕ МАРШРУТОВ",0xe,32,1);
	puts1("ГЛОБАЛЬНЫЕ МАРШРУТЫ",0xA,13,2);
	putch1('╔',0xA,2,3);
	for(i=0;i<37;i++)putch1('═',0xa,3+i,3);
	putch1('╗',0xa,40,3);
	for(i=4;i<48;i++)putch1('║',0xa,2,i);
	putch1('╚',0xa,2,48);
	for(i=0;i<37;i++)putch1('═',0xa,3+i,48);
	putch1('╝',0xa,40,48);
	for(i=4;i<48;i++)putch1('║',0xa,40,i);
	puts1("ЛОКАЛЬНЫЕ МАРШРУТЫ",0xa,50,2);
	putch1('╔',0xa,42,3);
	for(i=0;i<37;i++)putch1('═',0xa,43+i,3);
	putch1('╗',0xa,79,3);
	for(i=4;i<48;i++)putch1('║',0xa,42,i);
	putch1('╚',0xa,42,48);
	for(i=0;i<37;i++)putch1('═',0xa,43+i,48);
	putch1('╝',0xa,79,48);
	for(i=4;i<48;i++)putch1('║',0xa,79,i);
	return;


}
