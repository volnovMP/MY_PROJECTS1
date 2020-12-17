/*************************************************************************\
*                                                                         *
* SERVER.C - файл диспетчиризации сервера РПЦ ВНИИАС                      *
*                                                                         *
\*************************************************************************/

#include "opred2.h"
#include "extern2.h"
/************************************************\
*                                                *
*   main - основная функция-диспетчер сервера    *
*                                                *
\************************************************/
void main(void)
{
	int ii,jj,kk,i,j,k;
	unsigned char vvod;
	char FR3_ANA[34];
//	int T1,T2;
//	char DelTT[10];
	int iNt;
	ACTIV=0xF;
	T_fix=10890;T_zap=0;fixir=0;
	//получить текущую дату
	rg.h.ah=4;
	int86(0x1a,&rg,&rg);
	if(rg.x.cflag==0) //если успешное чтение даты
	{

		for(ii=0;ii<14;ii++)NAME_FILE[ii]=0;
	//получение имени для архива приема из АРМа
		strcpy(NAME_FILE,"RESULT//");

		NAME_FILE[8]=((rg.h.dl&0xf0)>>4)|0x30;
		NAME_FILE[9]=(rg.h.dl&0x0f)|0x30;
		strcat(NAME_FILE,".in");
		NAME_FILE[13]=0;
		new_day=((rg.h.dl&0xf0)>>4)*10+(rg.h.dl&0xf);
		old_day=new_day;
  }
	else
	{
#ifdef TEST
		strcpy(NAME_FILE,"RESULT//");
	  NAME_FILE[8]=((0x10)>>4)|0x30;
  	NAME_FILE[9]=(0x10&0x0f)|0x30;
	  strcat(NAME_FILE,".in");
		NAME_FILE[13]=0;
		new_day=((0x10&0xf0)>>4)*10+(0x10&0xf);
		old_day=new_day;
#else
    clrscr();
    puts1("Нет доступа к таймеру реального времени",0xc,10,10);
    getch();
		exit(1);
#endif
  }
	//открытие или создание файла приема из АРМ
#ifdef ARM_ARHIV
	file_arm_in=open(NAME_FILE,O_APPEND|O_RDWR,O_BINARY);
	if(file_arm_in<0)
	{ iNt=mkdir("result");
		file_arm_in=open(NAME_FILE,O_CREAT|O_TRUNC|O_APPEND|O_RDWR,S_IWRITE|O_BINARY);
	}
	if(file_arm_in<0)
	{ clrscr();
		puts1("Нарушение файловой структуры,работа невозможна",0xc,10,10);
		getch();
		exit(1);
	}
#endif
	//формирование текстового формата и получение имени суточного архива
	for(ii=10;ii<14;ii++)NAME_FILE[ii]=0;
	strcat(NAME_FILE,".ogo");
	NAME_FILE[14]=0;
	//открытие или создание файла суточного архива ТУМС
	file_arc=open(NAME_FILE,O_APPEND|O_RDWR,O_BINARY);
	if(file_arc<0)
	{ iNt=mkdir("result");
		file_arc=open(NAME_FILE,O_CREAT|O_TRUNC|O_APPEND|O_RDWR,S_IWRITE|O_BINARY);
	}
	if(file_arc<0)
	{ clrscr();
		puts1("Нарушение файловой структуры,работа невозможна",0xc,10,10);
		getch();
		exit(1);
	}
#ifdef ARM_ARHIV
	for(ii=10;ii<14;ii++)NAME_FILE[ii]=0;//очистить имя файла
	//получение имени для архива выдачи в АРМ
	strcat(NAME_FILE,".out");
	NAME_FILE[14]=0;

	//открытие или создание файла выдачи в АРМ
	file_arm_out=open(NAME_FILE,O_APPEND|O_RDWR,O_BINARY);
	if(file_arm_out<0)
	{ iNt=mkdir("result");
		file_arm_out=open(NAME_FILE,O_CREAT|O_TRUNC|O_APPEND|O_RDWR,S_IWRITE|O_BINARY);
	}
	if(file_arm_out<0)
	{ clrscr();
		puts1("Нарушение файловой структуры,работа невозможна",0xc,10,10);
		getch();
		exit(1);
	}
#endif
	read_t(); //формирование строки текущего времени TIME
	iniciator();
	sbros_kom();
	RESET_TIME=0xFFF;
	add(0,100,0);
// ----------------- Main CIKL -------------------------------------------
met0:
//	T1=mikrotiki();
#ifndef WORK
	ACTIV=1;
#endif
	if(CIKL_MAIN!=0)
	{
		CIKL_MAIN=0;
		outportb(0x3fc,0);
		outportb(0x2fc,0);
	}
	else
	{
		CIKL_MAIN=0xF;
		outportb(0x3fc,1);
		outportb(0x2fc,1);
	}
	if(T_zap>300)
	{
		fixir=0;
		T_fix=0;
		T_zap=0;
		for(ii=0;ii<KOL_VO;ii++)ZAFIX_FR4[ii]=0;
		putch1(219,0,17,50);
	}

	if(RESET_TIME==0)
	{
		stop_watchdog();
		re_set();
		exit(0);
	}
#ifndef TEST
	watchdog();//переинициализация таймера
#endif
	iNt=test_time1(18L);//проверить прохождение 1 сек и начала нового дня
	if(iNt==1)//если прошла секунда
	{
		T_TIME=time(NULL);
		FIR_time=SEC_time;
#ifndef WORK
	cikl_marsh=cikl_marsh+3;
	cikl_arm++;
	if(cikl_marsh>10)cikl_marsh=0;
#endif
	}
  //получение признака наличия незавершенных команд и маршрутов
#ifndef WORK
	if(cikl_marsh>1)
#else
	if(cikl_marsh>8)
#endif
	MARSH_GLOB_LOCAL();
#ifndef WORK
	if((cikl_marsh%2)==0)
	if(END_TUMS==0)
	{
    T_TIME = T_TIME + 19;
		TIMER_TIC();
	}
#endif
	FLAG_KOM=0;
//  FLAG_KOM=FLAG_KOM+MARSHRUT+MARSHRUT1;
	for(ii=0;ii<Narm;ii++)
		for(jj=0;jj<12;jj++)
		{
			FLAG_KOM=FLAG_KOM+KOMANDA_MARS[ii][0][jj];
			FLAG_KOM=FLAG_KOM+KOMANDA_RAZD[ii][0][jj];
			FLAG_KOM=FLAG_KOM+KOMANDA_MARS[ii][1][jj];
			FLAG_KOM=FLAG_KOM+KOMANDA_RAZD[ii][1][jj];


		}
		for(ii=0;ii<Nst;ii++)
		{
			for(jj=0;jj<15;jj++)
			{
				FLAG_KOM=FLAG_KOM+KOMANDA_ST[ii][jj];
				FLAG_KOM=FLAG_KOM+KOMANDA_TUMS[ii][jj];
			}
	 }
	 for(ii=0;ii<3;ii++)FLAG_KOM=FLAG_KOM+DIAGNOZ[ii];
	 for(ii=0;ii<6;ii++)FLAG_KOM=FLAG_KOM+ERR_PLAT[ii];
	 for(ii=0;ii<Nst*3;ii++)FLAG_KOM=FLAG_KOM+MARSHRUT_ALL[ii].NACH;

	//перевод пассивного сервера в промежуточное состояние, если нет
	//ответов от стоек и АРМа
	if(ACTIV==0)
	{
    if((inf_TUMS==0)&&(inf_ARM==0))//если молчат ТУМСы и АРМы
    {
      ACTIV=0xF;T0=0; //перевод сервера в фазу ожидания
      switch(SERVER)
      {
        case 1: STOP_SERVER=6;break;   //установка тайм-аута для сервера
        case 2: STOP_SERVER=12;break;
				case 3: STOP_SERVER=18;break;
			}
		}
	}
	 //если сервер активен, то выдать данные в АРМы
	if(ACTIV==1)ARM_OUT();
	else  if(cikl_arm>=2)cikl_arm=0;
	if(cikl_marsh>=9)Analiz_Glob_Marsh();
	OSN_TUMS_IN(); //работа с возможным приемом основного канала
	REZ_TUMS_IN(); //работа с возможным приемом резервного канала
  consentr1(); //выполнить обработку данных
//  TEST_MARSH();//проверка хода выполнения маршрутных команд
/*$$$$$$$$$$$$$$$$$$$$$$$
	if(MARSHRUT==0xF) //если завершен предварительный маршрут в 1-ом районе
	{ MARSHRUT=0xa;   //установить признак работы окончательного маршрута 1 р-на
		TUMS_MARSH(KONEC);//подготовить и выдать окончательные маршруты
	}
	if(MARSHRUT1==0xF)//если завершен предварительный маршрут районе 2
	{
		MARSHRUT1=0xA; //установить признак работы окончательного маршрута 2 р-на
		TUMS_MARSH(KONEC1);//подготовить и выдать окончательные маршруты
	}
*/
  ANALIZ_ARM();      //анализ данных,принятых из АРМ
  ANALIZ_SHN();      //анализ данных из АРМ-ШН
	VYVOD_ON_SCREEN(); //вывод на экран
	PRIEM_SERV();      //обработка данных пассивного сервера
	if(tiki_tum[ZAPROS_TUMS-1]==32)//если маршрутный таймер истек
  {
/*
		if(MYTHX[ZAPROS_TUMS-1]==0x50)MARSH_GOT[ZAPROS_TUMS-1]=0; //маршрут готов
		else
			if(MYTHX[ZAPROS_TUMS-1]==0x40)MARSH_GOT[ZAPROS_TUMS-1]=0xf; //маршрут устанавливается
			else  MARSH_GOT[ZAPROS_TUMS-1]=1; //маршрут не установлен
				if(MYTHX[ZAPROS_TUMS-1]=='P')putch1(MYTHX[ZAPROS_TUMS-1],10,66+ZAPROS_TUMS-1,50);
				else putch1(MYTHX[ZAPROS_TUMS-1],12,66+ZAPROS_TUMS-1,50);
  */
	}
/*
	if(REGIM==COMMAND)
	{
		if(KOMANDA_ST[ZAPROS_TUMS-1][11]!=0)
		{
			 putch1(0x10,0xc,1,Y_KOM);
			 if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
			 else putch1(' ',0xc,1,47);
			 for(i=0;i<12;i++)putch1(KOMANDA_ST[ZAPROS_TUMS-1][i],0xc,55+i,Y_KOM);
			 Y_KOM++;
			 puts1("                                   ",0xa,3,Y_KOM);
			 puts1("             ",0xa,55,Y_KOM);
			 if(Y_KOM>=48)Y_KOM=3;
		}
	}
	*/
	if(REGIM==ANAL_MARSH)
	{
		//вывод глобальных маршрутов
		for(i=0;i<5;i++)
		{
			if(MARSHRUT_ALL[i].KMND==0)putch1('0',0xc,4,4+8*i);
			else putch1(MARSHRUT_ALL[i].KMND,0xc,4,4+8*i);

			if(MARSHRUT_ALL[i].NACH==0)puts1("      ",0xc,6,4+8*i);
			else puts1(PAKO[MARSHRUT_ALL[i].NACH],0xc,6,4+8*i);

			if(MARSHRUT_ALL[i].END==0)puts1("      ",0xc,15,4+8*i);
			puts1(PAKO[MARSHRUT_ALL[i].END],0xc,15,4+8*i);

			switch(MARSHRUT_ALL[i].SOST)
			{
				case 0x43:	puts1("ПН",0xc,25,4+8*i);break;
				case 0x47:	puts1("ПР",0xc,25,4+8*i);break;
				case 0x4F:	puts1("ПВ",0xc,25,4+8*i);break;
				case 0x5F:	puts1("ПП",0xc,25,4+8*i);break;
				case 0x7F:  puts1("ПУ",0xc,25,4+8*i);break;
				case 0x41:	puts1("ПО",0xc,25,4+8*i);break;
				case 0x83:	puts1("ОН",0xc,25,4+8*i);break;
				case 0x87:	puts1("ОР",0xc,25,4+8*i);break;
				case 0x8F:	puts1("ОВ",0xc,25,4+8*i);break;
				case 0x9F:	puts1("ОП",0xc,25,4+8*i);break;
				case 0xbF:  puts1("ОУ",0xc,25,4+8*i);break;
				case 0x81:	puts1("ОО",0xc,25,4+8*i);break;
				default:	  puts1("??",0xc,25,4+8*i);break;
			}
			for(jj=0;jj<Nst;jj++)
			{
				for(kk=0;kk<3;kk++)
				{
					if((MARSHRUT_ST[jj][kk].NUM-100)==i)
					{
						MARSHRUT_ST[jj][kk].NEXT_KOM[13]=0;
						puts1(MARSHRUT_ST[jj][kk].NEXT_KOM,0xa,44,4+4*jj+kk);
						switch(MARSHRUT_ST[jj][kk].SOST)
						{
							case 0x47:	puts1("ПР",0xa,60,4+4*jj+kk);break;
							case 0x4f:	puts1("ПВ",0xa,60,4+4*jj+kk);break;
							case 0x5f:	puts1("ПП",0xa,60,4+4*jj+kk);break;
							case 0x7f:	puts1("ПУ",0xa,60,4+4*jj+kk);break;
							case 0x41:	puts1("ПО",0xa,60,4+4*jj+kk);break;
							case 0x87:	puts1("ОР",0xa,60,4+4*jj+kk);break;
							case 0x8f:	puts1("ОВ",0xa,60,4+4*jj+kk);break;
							case 0x9f:	puts1("ОП",0xa,60,4+4*jj+kk);break;
							case 0xbf:	puts1("ОУ",0xa,60,4+4*jj+kk);break;
							case 0x81:	puts1("ОО",0xa,60,4+4*jj+kk);break;
							default:    puts1("??",0xa,60,4+4*jj+kk);break;
						}
					}
					else
					{
						if(MARSHRUT_ST[jj][kk].NUM==0)
						{
							puts1("            ",0xa,44,4+4*jj+kk);
							puts1("??",0xa,60,4+4*jj+kk);
						}
					}
				}
			}
		}
	}
	if(kbhit())  //если нажата клавиша
	{
#ifndef WORK
		cikl_arm++; //для тестового режима
#endif
		vvod=getch(); //прочитать клавиатуру
		if(vvod==0)
		{
			VVOD_OBJ=0;
			OBJECT_ARM=0;
			vvod=getch();
			if(vvod==59)//F1-просмотр каналов
			{
				if(REGIM!=KANAL){REGIM=KANAL;tablica();vvod=0xff;}
				else  { REGIM=0; main_win(); vvod=0xff; }
			}
      if(vvod==60)//F2-общий обзор объектов
      {
        if(REGIM!=OBJECT)
        {REGIM=OBJECT;win_object();vvod=0xff;}
        else { REGIM=0; main_win(); vvod=0xff; }
      }
      if(vvod==61)//F3-анализ состояния каналов и объектов
      {
        if(REGIM!=ANALIZ)
				{
					REGIM=ANALIZ;
					win_analiz();
					vvod=0xff;
				}
        else
        {
          REGIM=0;
          main_win();
          vvod=0xff;
        }
      }
      if(vvod==62)//F4-обзор конфигуратора АРМов
      {
        if(REGIM!=KONFIG){REGIM=KONFIG;win_konfig();vvod=0xff;}
				else
        {
          REGIM=0;
          main_win();
          vvod=0xff;
        }
      }
      if(vvod==63)//F5-просмотр принимаемых и передаваемых команд
			{
        if(REGIM!=COMMAND){REGIM=COMMAND;win_comm();vvod=0xff;}
				else
        {
          REGIM=0;
          main_win();
          vvod=0xff;
        }
			}
			if((vvod==64)&&(REGIM==ANALIZ))//если нажата F6 в режиме анализа
			{
				if(STOP_AN==0)
				{
					STOP_AN=0xf;
					putch1(0xff,0xff,X_ANALIZ_OUT,Y_ANALIZ_OUT);
					putch1(0xff,0xff,X1_ANALIZ_OUT,Y1_ANALIZ_OUT);
				}
				else STOP_AN=0;
			}
      if(vvod==65) //F7 - просмотр глобальных и локальных маршрутов
      {
        if(REGIM!=ANAL_MARSH)
        {
					win_marsh();
        	REGIM=ANAL_MARSH;
      	}
        else
				{
					REGIM=0;
          main_win();
      	}
			}
      if((vvod==68)&&(REGIM==ANALIZ))//если нажата F10
      {
				puts1("ВВЕДИТЕ НОМЕР ОБЪЕКТА",0xf,1,47);
				if(VVOD_OBJ==0)VVOD_OBJ=0xff;
				else VVOD_OBJ=0;
			}
#ifdef TEST
      if(vvod==71)vvod_set=15;//Home - прямой ввод данных
      if(vvod_set==15)set_vvod();
#endif
			if(vvod==82)//ins
			{
				REGIM=0;
			}
			if(vvod==79) //если <End>
			{
//         FINAL();   //$#
#ifndef TEST
				if(ACTIV_SERV==SERVER)
				{
					putch1('А',0x8c,18,50);
					putch1('К',0x8c,19,50);
					putch1('Т',0x8c,20,50);
					putch1('И',0x8c,21,50);
					putch1('В',0x8c,22,50);
					putch1('Н',0x8c,23,50);
					putch1('Ы',0x8c,24,50);
					putch1('Й',0x8c,25,50);
				}
				else
				if((sosed_NEXT==0)||(sosed_PRED==0))  //$$$$ - 14_04_07 выход с паролем
				{
					putch1('П',0x8c,18,50);
					putch1('А',0x8c,19,50);
					putch1('Р',0x8c,20,50);
					putch1('О',0x8c,21,50);
					putch1('Л',0x8c,22,50);
					putch1('Ь',0x8c,23,50);
					putch1(':',0x8c,24,50);
					PAROL = 1;
				}
				else
#endif
				{
					FINAL();
				}
			}
		}
		else
		{
			if(PAROL!=0)             //$$$$ 14_04_07 ввод пароля
			{
			 if((PAROL!=0)&&(PAROL!=0x15))  //
			 {
					if((vvod==0xd)||(X_VVOD1>=4))
					{
						PAROL=0;
						X_VVOD1=0;
						for(kk=0;kk<4;kk++)
						{
							if(PAROL_TXT[kk]==0)break;

						}
						if(PAROL_TXT[0]=='o')
							if(PAROL_TXT[1]=='r')
								if(PAROL_TXT[2]=='s')
									if(PAROL_TXT[3]=='k')FINAL();
					}
					else
					{
						if(vvod==8)
						{
							if(X_VVOD1>0)X_VVOD1--;
							PAROL_TXT[X_VVOD1]=0;
							putch1(0x20,0xf,X_VVOD1+24,50);
						}
						else
						{
							putch1('*',0xf,X_VVOD1+24,50);
							PAROL_TXT[X_VVOD1]=vvod;
							X_VVOD1++;
						}
					}
				}
			}
			if((VVOD_OBJ!=0)&&(VVOD_OBJ!=0x15))
			{
				if((vvod==0xd)||(X_VVOD1>=5))
				{
					OBJECT_ARM=0;
					for(kk=0;kk<4;kk++)
					{
						if(NOMER_OB_ARM[kk]==0)break;
						OBJECT_ARM=OBJECT_ARM*10+(NOMER_OB_ARM[kk]-48);
					}
					if(OBJECT_ARM>=KOL_VO)OBJECT_ARM=KOL_VO-1;
					if(OBJECT_ARM<=0)OBJECT_ARM=1;
					if(((inp_ob[OBJECT_ARM]&0xff)==0x20)&&
						(((inp_ob[OBJECT_ARM]&0xff00)>>8)==0x20))OBJECT_SERV=0;
					else  OBJECT_SERV=inp_ob[OBJECT_ARM];
					X_VVOD1=1;
					VVOD_OBJ=0x15;
				}
				else
				{
					if(vvod==8)
					{
					 if(X_VVOD1>1)X_VVOD1--;
					 NOMER_OB_ARM[X_VVOD1-1]=0;
					 putch1(0x20,0xf,X_VVOD1,48);
					}
					else
					{
						if((vvod>=48)&&(vvod<=57))
						{
							putch1(vvod,0xf,X_VVOD1,48);
							NOMER_OB_ARM[X_VVOD1-1]=vvod;
							X_VVOD1++;
						}
					}
				}
			}
		}
  }
	if(VVOD_OBJ==0x15)
	{
		puts1(PAKO[OBJECT_SERV],0xc,6,48);
		for(kk=0;kk<8;kk++)FR3_ANA[kk]=FR3_ALL[OBJECT_SERV][2*kk]+FR3_ALL[OBJECT_SERV][2*kk+1]+48;
		FR3_ANA[8]=0;
		puts1(FR3_ANA,0xc,20,48);
	}
//  T2=mikrotiki();
//	if(T2<T1)T1=T1-T2;
//	else T1=T1+(65535-T2);
//	itoa(T1,DelTT,10);
//	puts1("    ",4,1,1);
//	puts1(DelTT,4,1,1);
	goto met0;
}
//=====================================================================================
/*******************************************************\
*   int TAKE_STROKA(unsigned char GRPP,int sb,int tms)  *
*  процедура определения номера строки, содержащей      *
*  номера объектов для сообщения,принятого из ТУМС      *
*  GRPP - код группы сообщения                          *
*  sb - номер сообщения                                 *
*  tms - номер стойки                                   *
\*******************************************************/
int TAKE_STROKA(unsigned char GRPP,int sb,int tms)
{
  int j,STRK=0;
  switch(GRPP)//переключатель по группам сообщений
	{ //сообщение по стрелкам
		case 'C': for(j=0;j<tms;j++)STRK=STRK+STR[j];   STRK=STRK+sb;   break;

		case 'E': for(j=0;j<tms;j++)STRK=STRK+SIG[j];  STRK=STRK+sb-STR[tms];  break;

		case 'Q': for(j=0;j<tms;j++)STRK=STRK+DOP[j];
							STRK=STRK+sb-STR[tms]-SIG[tms]; break;
		case 'b': for(j=0;j<tms;j++)STRK=STRK+DOP_B[j];
							STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]-UCH[tms]-PUT[tms]-UPR[tms];
							break;
		case 'd': for(j=0;j<tms;j++)STRK=STRK+DOP_D[j];
							STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]-DOP_B[tms]-UCH[tms]-PUT[tms]-UPR[tms];
							break;
		case 'T': for(j=0;j<tms;j++)STRK=STRK+DOP_T[j];
							STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]-DOP_B[tms]-DOP_D[tms]-UCH[tms]-PUT[tms]-UPR[tms]; break;

		case 'F': for(j=0;j<tms;j++)STRK=STRK+UCH[j]; STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]; break;

		case 'I': for(j=0;j<tms;j++)STRK=STRK+PUT[j]; STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]-UCH[tms];  break;

		case 'L': for(j=0;j<tms;j++)STRK=STRK+UPR[j]; STRK=STRK+sb-STR[tms]-SIG[tms]-DOP[tms]-UCH[tms]-PUT[tms];  break;

		default: return(-1);
  }
	return(STRK);
}
//==================================================================================
/**************************************************************************\
*ZAPOLNI_FR3(unsigned char GRP,int STRKA,int sob,int tum,unsigned char nov)*
*      процедура заполнения элементов массива FR3 принятыми данными        *
*      GRP - код группы принятых данных                                    *
*      STRKA - строка в файле номеров объектов данной группы               *
*      sob - номер принятого сообщения                                     *
*      tum - номер стойки                                                  *
*      nov - признак новизны байтов сообщения                              *
\**************************************************************************/
void ZAPOLNI_FR3(unsigned char GRP,int STRKA,int sob,int tum,unsigned char nov)
{
	unsigned int i,j,jj,k,num[10],ii,sgnl=0;
  int i_m,i_sig,i_s;
	unsigned char /*nom[15],*/l,maska,tester;
  //if(nov==0)return;
  //для группы GRP и строки STRKA найти номера объектов сервера
	switch(GRP)
  {
    case 'C': for(i=0;i<5;i++)num[i]=SPSTR[STRKA][i]; break;
    case 'E': for(i=0;i<5;i++)num[i]=SPSIG[STRKA][i]; break;
		case 'Q': for(i=0;i<5;i++)num[i]=SPDOP[STRKA][i]; break;
		case 'b': for(i=0;i<5;i++)num[i]=SPDOP_B[STRKA][i]; break;
		case 'd': for(i=0;i<5;i++)num[i]=SPDOP_D[STRKA][i]; break;
		case 'T': for(i=0;i<5;i++)num[i]=SPDOP_T[STRKA][i]; break;
		case 'F': sgnl=0;
							for(i=0;i<10;i++)num[i]=SPSP[STRKA][i]; break;
		case 'I': for(i=0;i<10;i++)num[i]=SPPUT[STRKA][i];break;
    case 'L': for(i=0;i<5;i++)num[i]=SPKON[STRKA][i]; break;
    case 'J': break;
    default: return;
	}
	//сформировать массив номеров объектов для принятых данных
	//пройти по всем объектам из массива номеров
	for(i=0;i<5;i++) //пройти по всем объектам сообщения
	{
		if(num[i]>=KOL_VO)continue; //если объект за пределами - ничего не делать
		if(num[i]<=0)continue;
		j=0;
		k=1<<i;
		jj=num[i];
		//выйти на строку объекта num[i] и взять состояние этого объекта
		read_FR3(num[i]);
		for(j=0;j<6;j++) //пройти побитнo по данным принятым из ТУМС
		{
			l=1<<j;//сформировать тест бит
			FR3[j*2]=0;
			//изменить состояние в соответствии с данными от ТУМСа
			if((VVOD[tum][sob][i]&l)!=0)//если бит перешел в 1
			{
				if(GRP=='E')//если данные получены по сигналу
				{
					if(j<4) //если это сигнальные или ВС биты
					{
						for(i_m=0;i_m<Nst*3;i_m++)//пройти по глобальным маршрутам
						{	//если нет маршрута - продолжать
							if(MARSHRUT_ALL[i_m].KMND==0)continue;

							i_sig=0; //есть маршрут - считаем сигналы

							//проход по стойкам
							for(i_s=0;i_s<Nst;i_s++)
							{ //если в стойке нет задействованных сигналов - продолжить
								if(MARSHRUT_ALL[i_m].SIG[i_s][0]==0)continue;

								//если для маршрута стойки этот сигнал первый - продолжить
								if(MARSHRUT_ALL[i_m].SIG[i_s][0]==jj)continue;
								//если не этот сигнал - увеличить счетчик
								else i_sig++;
							}
							//если сигналы открылись или в стойках участницах нет сигналов
							//и маршрут воспринят, то удалить маршрут по открытию начал
							//или по их отсутствию
							if((i_sig==0)&&((MARSHRUT_ALL[i_m].SOST&0x1F)==0x1F))
							{
								add(i_m,8888,11);
								DeleteMarsh(i_m);
							}
						}
					}
				}
				if(GRP=='I')//если путь перешел в состояние замыкания
				{
					//если принято из одной стойки
					//if((tum+1)==(num[5+i]&0xf))FR3[17]=FR3[17]|l; //установить
					//если принято  из другой стойки
					//if((tum+1)==((num[5+i]&0xf0)>>4))FR3[16]=FR3[16]|l;//установить
					if((j==1)||(j==2)||(j==4))//если ЧИ, НИ или КМ
					{
						FR3[13]=0; //снять предварительное замыкание
						//к номеру сигнала добавить признак релейного замыкания
						if((FR3[24]!=0)||(FR3[25]!=0))FR3[24]=FR3[24]|0x80;
						READ_BD(jj);
						if(bd_osn[1]!=0)//если есть сочленение с путем другой стойки
						{
							write_FR3(jj);//сохранить состояние пути
							ii=bd_osn[1]; //получить объект для сочлененного пути
							read_FR3(ii);
							FR3[13]=0; //снять замыкание
							//к номеру сигнала добавить признак релейного замыкания
							if((FR3[24]!=0)||(FR3[25]!=0))FR3[24]=FR3[24]|0x80;
							FR3[27]=0;FR3[26]=FR3[26]|0XE0; //установить признак новизны
							write_FR3(ii);//сохранить изменения для сопряженного пути
							NOVIZNA[nom_new++]=ii;
							PEREDACHA[ii]=PEREDANO;
							if(nom_new>=MAX_NEW)nom_new=0;
							read_FR3(jj);//вернуться к первому объекту пути
						}
					}
				}
				//если СП или УП перешел в состояние замыкания
				if((GRP=='F')&&((j==1)||(j==4))) //если замыкание или МИ
				{
					if((tum+1)==(num[5+i]&0xf))FR3[17]=FR3[17]|l; //установить
					//если принято  из другой стойки
					if((tum+1)==((num[5+i]&0xf0)>>4))FR3[16]=FR3[16]|l;//установить
					FR3[13]=0; //снять предварительное замыкание
					//если секция перекрывная, то для номера сигнала установить
					//признак включения замыкания
					if((FR3[24]!=0)||(FR3[25]!=0))FR3[24]=FR3[24]|0x80;

					sgnl=FR3[24]*256+FR3[25]; //получить объект сигнала в момент замыкания
					READ_BD(jj);
					if((bd_osn[0]==4)&&(bd_osn[1]!=0)) //если  УП и в двух стойках
					{
						write_FR3(jj);//сохранить состояние
						ii=bd_osn[1]; //получить объект для сочлененного пути
						read_FR3(ii);
						FR3[13]=0; //снять замыкание
						//установить для сигнала признак выполнения замыкания
						if((FR3[24]!=0)||(FR3[25]!=0))FR3[24]=FR3[24]|0x80;
						FR3[27]=0;FR3[26]=FR3[26]|0XE0; //установить признак новизны
						write_FR3(ii);//сохранить изменения для сопряженного пути
						NOVIZNA[nom_new++]=ii;//запомнить номер обновленного объекта
						PEREDACHA[ii]=PEREDANO; // сбросить флаг выполненной передачи для объекта
						if(nom_new>=MAX_NEW)nom_new=0; //если не передано более 20 новизн,начать с 0
						read_FR3(jj);//вернуться к первому объекту пути
					}
				}
				FR3[j*2+1]=1;//в любом случае установить общий
				if(GRP=='C')
				{
					READ_BD(jj);
					if(bd_osn[12]!=9999)//если стрелка спаренная
					{
						if(bd_osn[12]==1) //если основная в паре
						{
							ii=bd_osn[8]&0xFFF; //выделить парную
							write_FR3(jj);      //записать состояние
							read_FR3(ii);				//прочитать состояние парной
							FR3[j*2]=0;
							FR3[j*2+1]=1;
							write_FR3(ii);			//записать состояние парной
							read_FR3(jj);       //вернуться к основной
							if(j==3)            //если потеря контроля
							{
								for(i_m=0;i_m<Nst*3;i_m++) //пройти по маршрутам
								{
									for(i_s=0;i_s<10;i_s++) //пройти по стойкам
									{
										if((MARSHRUT_ALL[i_m].STREL[tum][i_s]&0xfff)==jj)
										{ //если потерявшая контроль стрелка в маршруте найдена
											add(i_m,8888,12);
											DeleteMarsh(i_m);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			else //если бит перешел в 0
			{
				//if((GRP=='I')||(GRP=='F'))//если объект пути
				//{
					//если вторая стойка
				 //	if((tum+1)==(num[5+i]&0xf))FR3[17]=FR3[17]&((~l)&0xff);
					//если первая стойка
				 //	if((tum+1)==((num[5+i]&0xf0)>>4))FR3[16]=FR3[16]&((~l)&0xff);
				//	if(((FR3[17]|FR3[16])&l)==0)FR3[j*2+1]=0;
				//}
				FR3[j*2+1]=0;
				if((GRP=='F')&&(j==1))//если СП разомкнулось
				{
					if((FR3[24]&0x80)!=0)//если фиксировалось ранее релейное замыкание
					{
						sgnl=(FR3[24]&0x7f)*256+FR3[25]; //получит объект сигнала
						FR3[24]=0; FR3[25]=0; //сбросить номер сигнала в СП
					}
				}
				if(GRP=='C')
				{
					READ_BD(jj);
					if(bd_osn[12]!=9999)
					{
						if(bd_osn[12]==1)
						{
							ii=bd_osn[8]&0xFFF;
							write_FR3(jj);
							read_FR3(ii);
							FR3[2*j]=0;
							FR3[2*j+1]=0;
							write_FR3(ii);
							read_FR3(jj);
						}
					}
				}
			}
		}
		FR3[27]=0;FR3[26]=FR3[26]|0XE0; //установить признак новизны
		//сохранить измененные массивы
		//изменения для инвертированного датчика день/ночь
		if((jj==912)||(jj==1107))FR3[1]=FR3[1]^1; //$$$$  13_04_07
		write_FR3(jj);
		if(GRP=='L') //если это группа контроллера
		{
			READ_BD(jj); //прочитать базу для объекта
			if((bd_osn[0]&0xff00)==0x400)//если это объект общее РУ
			{
				maska=bd_osn[0]&0xff;//прочитать требуемое состояние режимм ОУ
				tester=0;
				for(j=0;j<5;j++)tester=tester+(FR3[j*2+1]<<j); //получить текущее
				if(tester==maska)PROCESS=0x40; //если совпали, то управление от УВК
				else PROCESS=0; //иначе управление от пульта
			}
		}
FIN:
		//если был найден сигнал для перекрывной секции,
		if(sgnl!=0)
		{
			if(sgnl<0x8000) //если пришли из размыкания перекрывной секции
			{
				read_FR3(sgnl); //прочитать сигнал
				FR3[13]=0;FR3[15]=0;
				FR3[24]=0;
				FR3[27]=0;FR3[26]=FR3[26]|0xE0; //установить признак новизны
				//если байт обновился
				obnovi(sgnl);
				//сохранить измененные массивы на виртуальном диске
				write_FR3(sgnl);
				sgnl=0;
			 }
			 if(sgnl>=0x8000)//если пришли из замыкания перекрывной секции
			 {
				 ii=sgnl&&0x7FFF;
				 read_FR3(ii);//прочитать состояние сигнала
				 //сохранить измененные массивы на виртуальном диске
					//в сигнале установить признак замыкания перекрывной секции
				 FR3[24]=0x80;
				 write_FR3(ii);
				 obnovi(ii);
				 sgnl=0;
			 }
		 }
		 //если байт обновился
		if((nov&k)!=0)obnovi(jj);
		if(tiki_tum[tum]>30)//если маршрутный таймер истек
		{
			if(GRP=='L') //если это группа контроллера
			{
				READ_BD(jj); //прочитать базу для объекта
				if(bd_osn[0]==25)KORZINA[tum]=VVOD[tum][sob][i]&1;
			}
		}
	}
	return;
}
//===========================================================================
/*************************************************\
*     void interrupt far reading_char1()          *
* обработчик прерывания первой 4-х портовой платы *
\*************************************************/
void interrupt far reading_char1()
{
	unsigned int adr,dd,knl;
	enable();
snova:
  dd=inportb(0x300);
  if((dd&1)==0)knl=0; //активность первого канала
  else
    if((dd&2)==0)knl=1; //активность второго канала
    else
      if((dd&4)==0)knl=2; //активность третьего канала
      else
        if((dd&8)==0)knl=3; //активность четвертого канала
        else goto end;//если нет активности каналов
	adr=ADR_TUMS_OSN+knl*8;//определение базового адреса активного канала
  dd=inportb(adr+2); //определить прерывание канала
  if((dd&7)==6)
  {
    inportb(adr+5);//если прерывание по ошибке
    goto snova;
  }
	if(adr==ADR_TUMS_OSN)
  {
		if((dd&7)==4)in_tums_osn();//если есть прием - принять
    else
			if((dd&7)==2)vidacha1(ADR_TUMS_OSN);//если есть передача - передать
  }
  if(adr==ADR_TUMS_REZ)
  {
    if((dd&7)==4)in_tums_rez();//если есть прием - принять
    else
      if((dd&7)==2)vidacha1(ADR_TUMS_REZ);//если есть передача - передать
  }
  if(adr==ADR_SERV_NEXT)
  {
    if((dd&0xc)==0xc)//если прерывание по тайм-ауту
    {
      dd=inportb(ADR_SERV_NEXT);
      outportb(ADR_SERV_NEXT+2,0xC7);//очистить FIFO
    }
    if((dd&7)==4)in_next();
    else
      if((dd&7)==2)out_next();
  }
  if(adr==ADR_SHN_OSN)
  {
    if((dd&0xc)==0xc)//если прерывание по тайм-ауту
    {
      dd=inportb(ADR_SHN_OSN);
      outportb(ADR_SHN_OSN+2,0xC7);//очистить FIFO
      goto snova;
    }
    else
      if((dd&7)==4)in_shn_osn();//если прерывение по приему
      else
        if((dd&7)==2)out_shn_osn();//если прерывание по передаче
				else
          if(dd==0xc1)goto snova;
  }
 //перейти к повторной проверке активности каналов
	goto snova;
end:
  outportb(0x20,0x20);//выход из IRQ5
  if(V1>0x70)outportb(0xa0,0x20);//выход из IRQ15
  return;
}
/*************************************************\
*     void interrupt far reading_char1()          *
* обработчик прерывания второй 4-х портовой платы *
\*************************************************/
void interrupt far reading_char2()
{
  unsigned int adr,dd,knl;
  enable();
snova:
  dd=inportb(0x310);
  if((dd&1)==0)knl=0; //активность первого канала
  else
    if((dd&2)==0)knl=1; //активность второго канала
    else
      if((dd&4)==0)knl=2; //активность третьего канала
      else
        if((dd&8)==0)knl=3; //активность четвертого канала
        else goto end;
  adr=ADR_ARM_OSN+knl*8;
  dd=inportb(adr+2); //определить прерывание канала
  if((dd&7)==6)
  {
    inportb(adr+5);//если прерывание по ошибке
    goto snova;
  }
	if(adr==ADR_ARM_OSN)
  {
    if((dd&0xc)==0xc)//если прерывание по тайм-ауту
    {
			dd=inportb(ADR_ARM_OSN);
      outportb(ADR_ARM_OSN+2,0xC7);//очистить FIFO
      goto snova;
    }
    else
    if((dd&7)==4)in_arm_osn();
    else
      if((dd&7)==2)out_arm_osn();
      else
        if(dd==0xc1)goto snova;
  }
  if(adr==ADR_ARM_REZ)
  {
    if((dd&0xc)==0xc)//если прерывание по тайм-ауту
    {
      dd=inportb(ADR_ARM_REZ);
      outportb(ADR_ARM_REZ+2,0xC7);//очистить FIFO
      goto snova;
    }
    else
    if((dd&7)==4)in_arm_rez();
    else
      if((dd&7)==2)out_arm_rez();
      else
        if(dd==0xc1)goto snova;
  }
  if(adr==ADR_SERV_PRED)
  {
    if((dd&0xc)==0xc)//если прерывание по тайм-ауту
    {
      dd=inportb(ADR_SERV_PRED);
			outportb(ADR_SERV_PRED+2,0xC7);//очистить FIFO
      goto snova;
    }
    else
			if((dd&7)==4)in_pred();//если прерывение по приему
      else
        if((dd&7)==2)out_pred();//если прерывание по передаче
        else
          if(dd==0xc1)goto snova;
  }
  goto snova;
end:
  outportb(0x20,0x20);//выход из IRQ7
  if(V2>0x70)outportb(0xa0,0x20);//выход из IRQ15
  return;
}
//==============================================================================
/******************************************\
*    void interrupt far TIMER_TIC()        *
* обработчик прерываний системного таймера *
\******************************************/
#ifdef WORK
void interrupt far TIMER_TIC()
#else
void TIMER_TIC()
#endif
{
	int i,j,s_m;
	unsigned char aa;
	cikl_marsh++;
	if(cikl_marsh>10)cikl_marsh=0;
	win_gash();
	if(RESET_TIME<0x999){RESET_TIME--;if(RESET_TIME<=0)RESET_TIME=0;}
	T_fix++;

	if((T_fix>=10920)&&(ACTIV==1))//наступило время записи всего для активного
	{ fixir=0xff; T_fix=10920; T_zap++; putch1('*',0x8b,17,50); }

	//текущее время связи в тиках с предыдущим
	aa=(T_MIN_PRED>>8)+48;    putch1(aa,0xc,30,50);
	aa=(T_MIN_PRED&0xff)+48;  putch1(aa,0xc,31,50);
	//текущее время связи в тиках с последующим
	aa=(T_MIN_NEXT>>8)+48;    putch1(aa,0xa,32,50);
	aa=(T_MIN_NEXT&0xff)+48;  putch1(aa,0xa,33,50);
	//стрелочки к соседям налево и направо
	aa=sosed_PRED+48;         putch1(aa,0xc,28,50);
	aa=sosed_NEXT+48;         putch1(aa,0xa,35,50);
	//флаг существования команд в сервере
	aa=FLAG_KOM/256+48;       putch1(aa,0xe,40,50);
	aa=FLAG_KOM%256+48;       putch1(aa,0xe,41,50);

	//признак наличия маршрута в сервере
	aa=0;
	for(i=0;i<Narm;i++)
	for(j=0;j<12;j++)aa=aa|KOMANDA_MARS[i][0][j];
	for(j=0;j<12;j++)aa=aa|KOMANDA_MARS[i][1][j];
	putch1(aa|0x48,0xe,43,50);
	//признак наличия раздельной в сервере
	aa=0;
	for(i=0;i<Narm;i++)
	for(j=0;j<12;j++)aa=aa|KOMANDA_RAZD[i][0][j];
	for(j=0;j<12;j++)aa=aa|KOMANDA_RAZD[i][1][j];
	putch1(aa|0x48,0xe,44,50);
	//признак наличия раздельной в ТУМСах
	aa=0;
	for(i=0;i<Narm;i++)
	for(j=0;j<15;j++)aa=aa|KOMANDA_ST[i][j];
	putch1(aa|0x48,0xe,45,50);
	//признак наличия маршрута в ТУМСах
	aa=0;
	for(i=0;i<Narm;i++)
	for(j=0;j<15;j++)aa=aa|KOMANDA_TUMS[i][j];
	putch1(aa|0x48,0xe,46,50);
	//признак наличия диагностики
	aa=0;
	for(i=0;i<3;i++)aa=aa|DIAGNOZ[i];
	putch1(aa|0x48,0xe,47,50);
	//признак повреждения плат
	aa=0;
	for(i=0;i<6;i++)aa=aa|ERR_PLAT[i];
	putch1(aa|0x48,0xe,48,50);


	//номер данного сервера
	putch1(SERVER+48,0xa,13,50);
	putch1(0x2d,0xa,14,50);
	//номер активного сервера
	putch1(ACTIV_SERV+48,0xa,15,50);


	for(i=0;i<Nst;i++)
	{ //признаки наличия связи с ТУМСами
		SVAZ_TUMS[i]++;  if(SVAZ_TUMS[i]>80)SVAZ_TUMS[i]=80;
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		if(MARSH_GOT[i]==0xF)tiki_tum[i]++;
		if(tiki_tum[i]>30)tiki_tum[i]=32;
*/
	}
	//признаки наличия связи с соседними серверами
	sosed_PRED--;  sosed_NEXT--;  inf_TUMS--;  inf_ARM--;
	if(sosed_PRED<0)sosed_PRED=0;  if(sosed_NEXT<0)sosed_NEXT=0;

	if(sosed_NEXT==0)//если не вижу соседа следующего
	{
		T_MIN_NEXT=0;  cikl_out_next=0; cikl_in_next=0;
		outportb(ADR_SERV_NEXT+2,0xc7); //переинициализировать FIFO
	}

	if(sosed_PRED==0)//если не вижу соседа предыдущего
	{
		T_MIN_PRED=0; cikl_out_pred=0; cikl_in_pred=0;
		outportb(ADR_SERV_PRED+2,0xc7);//переинициализировать FIFO
	}

	if(inf_TUMS<0)inf_TUMS=0;   if(inf_ARM<0)inf_ARM=0;

	//если получена команда на останов сервера
	if(STOP_SERVER>0){STOP_SERVER--; return; }

	T0++;  //счетчик тактов простоя при активизации серверов

	cikl_arm++; //счетчик для циклов передачи АРМам

	if(ACTIV==0)return;

	if(T0>=16)T0=16;

	//счетчики минуты для следующего и предыдущего серверов
	T_MIN_NEXT=T_MIN_NEXT+1;
	T_MIN_PRED=T_MIN_PRED+1;

	//выделение моментов для сброса признаков
	//получения и передачи управления
	if(((T_MIN_NEXT>100)&&(T_MIN_NEXT<150))||
	((T_MIN_PRED>100)&&(T_MIN_PRED<150)))
	{
		POLUCHIL_UPR_OT_NEXT=0; POLUCHIL_UPR_OT_PRED=0;
		PEREDAL_UPR_K_NEXT=0;   PEREDAL_UPR_K_PRED=0;
	}

	if(ACTIV==1)
	{
		//очистка буферов и новые номера запросов в осн. и рез. каналы
		new_zapros();
		//работа с основным запросом
		ADR_TUMS_OUT=TUMS_ZAPROS[ZAPROS_TUMS-1]; //сформировать адрес
		//работа с резервным запросом
		ADR_TUMS_OUT1=TUMS_ZAPROS[ZAPROS_TUMS1-1];

		ZAPROS[0]='!';ZAPROS[1]=ADR_TUMS_OUT;
		ZAPROS[2]=0;
		ZAPROS[4]=0;

		ZAPROS1[0]='!'; ZAPROS1[1]=ADR_TUMS_OUT1;
		ZAPROS1[2]=0;
		ZAPROS1[4]=0;

		//РАБОТА С ЗАПРОСАМИ В ТУМСы
		for(i=0;i<2;i++) //перенести запросы в буфера вывода
		{ BUF_OUT[UKAZ_ZAP++]=ZAPROS[i];if(UKAZ_ZAP>=SIZE_BUF)UKAZ_ZAP=0;
			BUF_OUT1[UKAZ_ZAP1++]=ZAPROS1[i];if(UKAZ_ZAP1>=SIZE_BUF)UKAZ_ZAP1=0;
		}
		//работа с маршрутами
		if((KOMANDA_TUMS[ZAPROS_TUMS-1][10]!=0))
		{	if(ADR_TUMS_OUT==KOMANDA_TUMS[ZAPROS_TUMS-1][1])
			{ s_m=KOMANDA_TUMS[ZAPROS_TUMS-1][14];
				if((T_TIME-MARSHRUT_ST[ZAPROS_TUMS-1][s_m].T_VYD)>2L)
				{
					MARSHRUT_ST[ZAPROS_TUMS-1][s_m].T_VYD=T_TIME;
					MARSHRUT_ST[ZAPROS_TUMS-1][s_m].T_MAX=
					MARSHRUT_ST[ZAPROS_TUMS-1][s_m].T_MAX+T_TIME;
					//установить состояние=выдано
					MARSHRUT_ST[ZAPROS_TUMS-1][s_m].SOST=
					(MARSHRUT_ST[ZAPROS_TUMS-1][s_m].SOST&0xC0)|0xF;
					KOMANDA_TUMS[ZAPROS_TUMS-1][11]=')';
					ZAPROS[2]='(';
					TUMS_RABOT[ZAPROS_TUMS-1]=0xF;
					for(i=0;i<12;i++) //перенести команду в буфера вывода
					{ BUF_OUT[UKAZ_ZAP++]=KOMANDA_TUMS[ZAPROS_TUMS-1][i];
						if(UKAZ_ZAP>=SIZE_BUF)UKAZ_ZAP=0;
						//если маршрут выдавался 3 раза - сбросить команду УБРАНО 14_04_07$$$$
						//if(SHET_MARSH[ZAPROS_TUMS-1]>3)
						KOMANDA_TUMS[ZAPROS_TUMS-1][i]=0;

					}
					//увеличить счетчик числа выданных команд УБРАНО 14_04_07$$$$
					// SHET_MARSH[ZAPROS_TUMS-1]++;
					/*
					if(SHET_MARSH[ZAPROS_TUMS-1]>4)//если окончен цикл повторов команд
					{
						SHET_MARSH[ZAPROS_TUMS-1]=0; //сбросить счетчик числа повторов
						for(i=0;i<15;i++)KOMANDA_TUMS[ZAPROS_TUMS-1][i]=0;
					} */
					if(REGIM==COMMAND)
					{
						putch1(0x10,0xc,1,Y_KOM); //нарисовать стрелочку в позиции 1,Y_KOM
						if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1); //стереть старую стрелочку
						else putch1(' ',0xc,1,46);  //стереть старую стрелочку в конце
						puts1(KOMANDA_TUMS[ZAPROS_TUMS-1],0x80+SHET_MARSH[ZAPROS_TUMS-1]+1,55,Y_KOM); //нарисовать новую стрелку
						Y_KOM++; //перейти на следующую строку
						if(Y_KOM>=47)Y_KOM=3;
						//стереть следующую команду
						puts1("                                   ",0xa,3,Y_KOM);
					}
				}
			}
		}
		else
		{ //РАБОТА С РАЗДЕЛЬНЫМИ КОМАНДАМИ В СТОЙКУ
			if(REGIM==COMMAND)
			{
				if(KOMANDA_ST[ZAPROS_TUMS-1][10]!=0)
				{
					putch1(0x10,0xc,1,Y_KOM);
					if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
					else putch1(' ',0xc,1,46);
					for(i=0;i<12;i++)putch1(KOMANDA_ST[ZAPROS_TUMS-1][i],0xc,55+i,Y_KOM);
					Y_KOM++;
					puts1("                                   ",0xa,3,Y_KOM);
					puts1("             ",0xa,55,Y_KOM);
					if(Y_KOM>=47)Y_KOM=3;
				}
			}
			//если есть раздельная команда в ТУМС
			if(KOMANDA_ST[ZAPROS_TUMS-1][10]!=0)
			{
				//если адрес запроса совпадает с адресом команды
				if(ADR_TUMS_OUT==KOMANDA_ST[ZAPROS_TUMS-1][1])
				{ ZAPROS[2]='(';
					for(i=0;i<12;i++) //перенести команду в буфера вывода
					{ BUF_OUT[UKAZ_ZAP++]=KOMANDA_ST[ZAPROS_TUMS-1][i];
						if(UKAZ_ZAP>=SIZE_BUF)UKAZ_ZAP=0;
						//если нажато кнопка ОК, то команду сбросить, исключая повторы
						if(OK_KNOPKA!=0)KOMANDA_ST[ZAPROS_TUMS-1][i]=0;
						//если выдавалась 3 раза, то сбросить команду
						if(SHET_KOM[ZAPROS_TUMS-1]>3)KOMANDA_ST[ZAPROS_TUMS-1][i]=0;
					}
					if(OK_KNOPKA!=0)KOMANDA_ST[ZAPROS_TUMS-1][0]=0;
					SHET_KOM[ZAPROS_TUMS-1]++; //увеличить счетчик повторов выдачи
					if(SHET_KOM[ZAPROS_TUMS-1]>4) //если выдавалась 3 раза
					{
						SHET_KOM[ZAPROS_TUMS-1]=0; //сбросить счетчик повторов
						for(i=0;i<15;i++)KOMANDA_ST[ZAPROS_TUMS-1][i]=0;
					}

				}
			}
		}
		if(ZAPROS[2]==0)
		{
			BUF_OUT[UKAZ_ZAP++]=')';
			if(UKAZ_ZAP>=SIZE_BUF)UKAZ_ZAP=0;
		}
		//аналогично для резервного канала
		if(KOMANDA_TUMS[ZAPROS_TUMS1-1][10]!=0)
		{ if(ADR_TUMS_OUT1==KOMANDA_TUMS[ZAPROS_TUMS1-1][1])
			{
				s_m=KOMANDA_TUMS[ZAPROS_TUMS1-1][14];
				if((T_TIME-MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].T_VYD)>2l)
				{ MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].T_VYD=T_TIME;
					MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].T_MAX=
					MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].T_MAX+T_TIME;

					//установить состояние=выдано
					MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].SOST=
					(MARSHRUT_ST[ZAPROS_TUMS1-1][s_m].SOST&0xC0)|0xF;
					KOMANDA_TUMS[ZAPROS_TUMS1-1][11]=')';
					ZAPROS1[2]='(';
					TUMS_RABOT[ZAPROS_TUMS1-1]=0xF;
					for(i=0;i<12;i++) //перенести команду в буфера вывода
					{ BUF_OUT1[UKAZ_ZAP1++]=KOMANDA_TUMS[ZAPROS_TUMS1-1][i];
						if(UKAZ_ZAP1>=SIZE_BUF)UKAZ_ZAP1=0;
						//if(SHET_MARSH[ZAPROS_TUMS1-1]>3) $$$$ УБРАНО 14_04_07
						KOMANDA_TUMS[ZAPROS_TUMS1-1][i]=0;
					}
					/*
					SHET_MARSH[ZAPROS_TUMS1-1]++;
					if(SHET_MARSH[ZAPROS_TUMS1-1]>4)
					{
						SHET_MARSH[ZAPROS_TUMS1-1]=0;
						for(i=0;i<15;i++)KOMANDA_TUMS[ZAPROS_TUMS1-1][i]=0;
					}
					*/
					if(REGIM==COMMAND)
					{
						putch1(0x10,0xc,1,Y_KOM);
						if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
						else putch1(' ',0xc,1,46);
						puts1(KOMANDA_TUMS[ZAPROS_TUMS1],0x81+SHET_MARSH[ZAPROS_TUMS1-1],55,Y_KOM);
						Y_KOM++;
						if(Y_KOM>=47)Y_KOM=3;
						puts1("                                   ",0xa,3,Y_KOM);
					}
				}
			}
		}
		else
		{
			if(REGIM==COMMAND)
			{
				if(KOMANDA_ST[ZAPROS_TUMS1-1][10]!=0)
				{
					putch1(0x10,0xc,1,Y_KOM);
					if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
					else putch1(' ',0xc,1,46);
					for(i=0;i<12;i++)putch1(KOMANDA_ST[ZAPROS_TUMS1-1][i],0xc,55+i,Y_KOM);
					Y_KOM++;
					puts1("                                   ",0xa,3,Y_KOM);
					puts1("             ",0xa,55,Y_KOM);
					if(Y_KOM>=47)Y_KOM=3;
				}
			}
			//РАБОТА С РАЗДЕЛЬНЫМИ КОМАНДАМИ В СТОЙКУ
			if(KOMANDA_ST[ZAPROS_TUMS1-1][10]!=0)
			{
				if(ADR_TUMS_OUT1==KOMANDA_ST[ZAPROS_TUMS1-1][1])
				{
					ZAPROS1[2]='(';
					for(i=0;i<12;i++) //перенести команду в буфера вывода
					{ BUF_OUT1[UKAZ_ZAP1++]=KOMANDA_ST[ZAPROS_TUMS1-1][i];
						if(UKAZ_ZAP1>=SIZE_BUF)UKAZ_ZAP1=0;
						if(OK_KNOPKA!=0)KOMANDA_ST[ZAPROS_TUMS1-1][i]=0;
						if(SHET_KOM[ZAPROS_TUMS1-1]>3)KOMANDA_ST[ZAPROS_TUMS1-1][i]=0;
					}
					if(OK_KNOPKA!=0)KOMANDA_ST[ZAPROS_TUMS1-1][0]=0;
					SHET_KOM[ZAPROS_TUMS1-1]++;
					if(SHET_KOM[ZAPROS_TUMS1-1]>4)
					{
						SHET_KOM[ZAPROS_TUMS1-1]=0;
						for(i=0;i<15;i++)KOMANDA_ST[ZAPROS_TUMS1-1][i]=0;
					}
				}
			}
		}
		if(ZAPROS1[2]==0)
		{
			BUF_OUT1[UKAZ_ZAP1++]=')';
			if(UKAZ_ZAP1>=SIZE_BUF)UKAZ_ZAP1=0;
		}
		outportb(ADR_TUMS_OSN+1,0x3);
		outportb(ADR_TUMS_REZ+1,0x3);
	}
	return;
}
//==========================================================================
/****************************************************\
*         int check_summ(unsigned char reg[12])      *
* процедура подсчета контрольной суммы для ТУМС      *
\****************************************************/
int check_summ(unsigned char reg[15])
{
	unsigned char sum=0;
	int ic;
	for(ic=1;ic<10;ic++)sum=sum^reg[ic];
	sum=sum|0x40;
	return(sum);
}
//===========================================================================
/***************************************\
*        int mikrotiki()                *
* процедура получения текущего значения *
* счетчика "микротиков" по ~0.8 мкс     *
\***************************************/
int mikrotiki(void)
{
  unsigned int mikro;
//  outportb(0x43,0);
  mikro=inport(0x40)&0xFF;//младший байт
  mikro=mikro+(inport(0x40)<<8);//старший байт //зафиксировать время выдачи
  return(mikro);
 }
 //============================================================================
/*************************************************\
*                  new_zapros()                   *
*    процедура формирования очередных запросов    *
* в стойки ТУМС по основному и резервному каналам *
\*************************************************/
void new_zapros(void)
{
	 //сбросить все содержимое буферов осн. канала приема и передачи в ТУМС
	sbros_tums(0);
	ZPRS_TMS=0xF; ZAPROS_TUMS++;  if(ZAPROS_TUMS>8)ZAPROS_TUMS=1;
	if(REGIM==0)
	{
		putch1(31,0xa,4+6*(ZAPROS_TUMS-1),41);
		putch1(17,0xa,49,44-2*(SERVER-1));
	}
	 //сбросить все содержимое буферов рез. канала приема и передачи в ТУМС
	sbros_tums(1);
	ZPRS_TMS1=0xF;ZAPROS_TUMS1++; if(ZAPROS_TUMS1>8)ZAPROS_TUMS1=1;
	if(REGIM==0)
	{
		putch1(30,0xe,4+6*(ZAPROS_TUMS1-1),46);
		putch1(31,0xe,54+9*(SERVER-1),48);
	}
	return;
}
/***********************************************\
*            sbros_tums(int a)                  *
* процедура сброса данных и запросов ТУМСов     *
\***********************************************/
void sbros_tums(int a)
{
  int ik;
  switch(a)
  {
    case 0: n_tums=0;
            for(ik=0;ik<SIZE_BUF;ik++)BUF_OUT[ik]=0;
            for(ik=0;ik<SIZE_BUF;ik++)BUF_IN[ik]=0;
						for(ik=0;ik<6;ik++)ZAPROS[ik]=0;
            UKAZ_IN=0;
						UKAZ_ZAP=0;UKAZ_OUT=0;
            END_TUMS=0;ADR_TUMS_IN=0;
            break;

    case 1: n_tums1=0;
            for(ik=0;ik<SIZE_BUF;ik++)BUF_OUT1[ik]=0;
            for(ik=0;ik<SIZE_BUF;ik++)BUF_IN1[ik]=0;
            for(ik=0;ik<6;ik++)ZAPROS1[ik]=0;
            UKAZ_IN1=0;
            UKAZ_ZAP1=0; UKAZ_OUT1=0;
            END_TUMS1=0;ADR_TUMS_IN1=0;
            break;
  }
  return;
}
//===============================================================================
/*******************************************\
*              PRIEM_SERV()                 *
*    Процедура анализа принятых данных      *
*    по каналу серверов и вывода на экран   *
*    принятых байт, а также подготовки      *
*        ответов активному серверу          *
\*******************************************/
void PRIEM_SERV(void)
{
  int i,inf_next,inf_pred,pas_next,pas_pred;
  unsigned int cr16;
  inf_pred=0;
	inf_next=0;
	pas_pred=0;
	pas_next=0;
	//********АНАЛИЗ НАЛИЧИЯ ИНФОРМАЦИИ ОТ ПРЕДЫДУЩЕГО СЕРВЕРА******
	if(cikl_in_pred==0) //если прошли циклы приема данных от предыдущего
	{
		//если приняты данные по объектам, переданным в АРМ
		if((BUF_IN_PRED[0]==0x12)&&(BUF_IN_PRED[97]==13))
		{
			//если прием от предыдущего соседа
			if(ANALIZ_KVIT_SERV(0)==0)inf_pred=0xF;//если прием успешный
			else
			{
				for(i=0;i<98;i++)BUF_IN_PRED[i]=0;
				outportb(ADR_SERV_PRED+2,0xc7); //$$$
			}
			if(((BUF_IN_PRED[91]!=0)||(BUF_IN_PRED[92]!=0))&&ACTIV==0)
			{
				i=((BUF_IN_PRED[92]&0xf)<<8)+BUF_IN_PRED[91];
				FR4[i]=BUF_IN_PRED[93];
			}
		}
		else //если данные по FR4 и сервер пассивный
		if((BUF_IN_PRED[0]==0x3C)&&(BUF_IN_PRED[97]==0x3e)&&(ACTIV==0))//????
		{
			pas_pred=0xf; //установить признак пассивности предыдущего
			cr16=CalculateCRC16(&BUF_IN_PRED[1],94);//подсчитать контр.сумму
			if( (BUF_IN_PRED[95]!=(cr16&0xFF)) ||
			(BUF_IN_PRED[96]!=((cr16&0xFF00)>>8)))
			{
				for(i=0;i<98;i++)BUF_IN_PRED[i]=0;
				outportb(ADR_SERV_PRED+2,0xc7); //$$$
			}
			else //если контрольная сумма совпала
			{
			}
		}
		if(REGIM==KANAL)
		{
			if(SCREEN_PRED!=0)//если есть прием от предыдущего
			{ //вывести прием из предыдущего сервера
				if(inf_pred!=0)for(i=0;i<15;i++)putch1(BUF_IN_PRED[i]|0x40,0xc,43+i,6);
				else
					if(pas_pred!=0)for(i=0;i<15;i++)putch1(BUF_IN_PRED[i]|0x40,atrib,43+i,7);
				SCREEN_PRED=0;
			}
		}
	}
	//********АНАЛИЗ НАЛИЧИЯ ИНФОРМАЦИИ ОТ СЛЕДУЮЩЕГО СЕРВЕРА******
	if(cikl_in_next==0)
	{ //если был прием информации по переданным объектам
		if((BUF_IN_NEXT[0]==0x2a)&&(BUF_IN_NEXT[97]==13))
		{
			if(ANALIZ_KVIT_SERV(1)==0)inf_next=0xf;//если принято правильно
			else
			{
				for(i=0;i<98;i++)BUF_IN_NEXT[i]=0;
				outportb(ADR_SERV_NEXT+2,0xc7);//$$$
			}
			if(((BUF_IN_NEXT[91]!=0)||(BUF_IN_NEXT[92]!=0))&&ACTIV==0)
			{
				i=((BUF_IN_NEXT[92]&0xf)<<8)+BUF_IN_NEXT[91];
				FR4[i]=BUF_IN_NEXT[93];
			}

		}
		else
		//если приняты ограничения по FR4
		if((BUF_IN_NEXT[0]==0x3c)&&(BUF_IN_NEXT[97]==0x3e)&&(ACTIV==0)) //????
		{
			pas_next=0xf;
			cr16=CalculateCRC16(&BUF_IN_NEXT[1],94);//подсчитать контр.сумму
			//если данные плохие - сбросить
			if( (BUF_IN_NEXT[95]!=(cr16&0xFF) )||
			(BUF_IN_NEXT[96]!=((cr16&0xFF00)>>8)))
			{
				for(i=0;i<98;i++)BUF_IN_NEXT[i]=0;
				outportb(ADR_SERV_NEXT+2,0xc7);//$$$
			}
			else
			{
			}

		}
		if(REGIM==KANAL)
		{
			if(SCREEN_NEXT!=0)//если есть прием от следующего
			{ //вывести прием из следующего сервера (активного)
        if(inf_next!=0)for(i=0;i<15;i++)putch1(BUF_IN_NEXT[i]|0x40,0xa,43+i,3);
        else
           //вывести прием из пассивного сервера
          if(pas_next!=0)for(i=0;i<15;i++)putch1(BUF_IN_NEXT[i]|0x40,atrib,43+i,4);
        SCREEN_NEXT=0;
      }
    }
  }
  if((ACTIV==1)&&(inf_pred!=0))
  {
    if((BUF_IN_PRED[89]!=SERVER)||(BUF_IN_PRED[90]!=SERVER))
    {
      if(BUF_IN_PRED[89]<SERVER){ACTIV=0;return;}
    }
  }
  if((ACTIV==1)&&(inf_next!=0))
  {
    if((BUF_IN_NEXT[89]!=SERVER)||(BUF_IN_NEXT[90]!=SERVER))
    {
      if(BUF_IN_NEXT[89]!=SERVER){ACTIV=0;return;}
    }
  }
  if((ACTIV==0)&&(STOP_SERVER==0))//если сервер пассивный и вышел на режим
  { //если активен предыдущий сервер
		if(inf_pred!=0)
    { if((BUF_IN_PRED[89]==SERVER)&&(BUF_IN_PRED[90]==SERVER))
      {
        POLUCHIL_UPR_OT_PRED=0xF; PEREDAL_UPR_K_PRED=0;
        POLUCHIL_UPR_OT_NEXT=0;   PEREDAL_UPR_K_NEXT=0;
        ACTIVNOST();
        T0=0;
        putch1(0x1a,0xc,29,50);
        putch1(0x20,0xc,34,50);
      }
      else
        if((BUF_IN_PRED[89]!=0)&&(BUF_IN_PRED[89]!=ACTIV_SERV))
        {
          ACTIV_SERV=BUF_IN_PRED[89];
          ACTIV=0;
        }
      for(i=0;i<98;i++)//завернуть ответ активному серверу
      {
        BUF_OUT_PRED[i]=BUF_IN_PRED[i];
        BUF_IN_PRED[i]=0;
      }
      //если ранее передал управление кому-нибудь
      if((PEREDAL_UPR_K_PRED!=0)||(PEREDAL_UPR_K_NEXT!=0))
      {
        //ПЕРЕДАЛ УПРАВЛЕНИЕ
        BUF_OUT_PRED[89]=ACTIV_SERV;
        BUF_OUT_PRED[90]=ACTIV_SERV;
      }
      BUF_OUT_PRED[0]='*';
      cr16=CalculateCRC16(&BUF_OUT_PRED[1],94);
      BUF_OUT_PRED[95]=cr16&0xFF;
      BUF_OUT_PRED[96]=(cr16&0xFF00)>>8;
      BUF_OUT_PRED[97]=0xd;

      for(i=0;i<98;i++)BUF_OUT_NEXT[i]=0; //????
/*
      k=1;
      //пассивный сервер передает ограничения другому пассивному серверу
      for(i=FR4_NEXT;i<KOL_VO;i++)//пройти по всем объектам FR4
      {
        if(FR4[i]!=0) //если на объект наложены ограничения
        {

          BUF_OUT_NEXT[k++]=i/256;  //старший байт номера объекта
          BUF_OUT_NEXT[k++]=i%256;  //младший байт номера объекта
          BUF_OUT_NEXT[k++]=FR4[i]; //записать ограничения
        }
        FR4_NEXT++;//перейти к следующему объекту
        if(k>=94)break;//если достигнут конец буфера

      }
*/
      cr16=CalculateCRC16(&BUF_OUT_NEXT[1],94);//вычислить контрольную сумму
      BUF_OUT_NEXT[95]=cr16&0xFF;
      BUF_OUT_NEXT[96]=(cr16&0xFF00)>>8;
      //оформить обрамление пакета
      BUF_OUT_NEXT[0]=0x3C; BUF_OUT_NEXT[97]=0x3E;
      atrib++;
      if(atrib>15)atrib=9;

      if(REGIM==KANAL)
      { //вывести на экран ответ-эхо активному серверу
        for(i=0;i<15;i++)putch1(BUF_OUT_PRED[i]|0x40,0xc,60+i,6);
        //вывести на экран ограничения в пассивный сервер
        for(i=0;i<15;i++)putch1(BUF_OUT_NEXT[i]|0x40,atrib,60+i,4);
      }
      //разрешить передачу данных в сервера-соседи
      outportb(ADR_SERV_PRED+1,3);
      outportb(ADR_SERV_NEXT+1,3);
    }

    //если активен следующий сервер
    if(inf_next!=0)
    {
      //если в приемном буфере указан мой сервер как активный
      if((BUF_IN_NEXT[89]==SERVER)&&(BUF_IN_NEXT[90]==SERVER))
      {
        POLUCHIL_UPR_OT_NEXT=0xF;//установить признак получения управления
        PEREDAL_UPR_K_NEXT=0;    //сбросить признак передачи управления
        POLUCHIL_UPR_OT_PRED=0;
        PEREDAL_UPR_K_PRED=0;
        ACTIVNOST(); //выполнить операции активизации сервера

        putch1(0x1b,0xc,34,50); //вывести на экран символ получения
        putch1(0x20,0xc,29,50); //управления от следующего
      }
      else //если управление передано другому серверу
        if((BUF_IN_NEXT[89]!=0)&&(ACTIV_SERV!=BUF_IN_NEXT[89]))
        {
          ACTIV_SERV=BUF_IN_NEXT[89];
          ACTIV=0;
        }
      for(i=0;i<98;i++)
      {
        BUF_OUT_NEXT[i]=BUF_IN_NEXT[i];
        BUF_IN_NEXT[i]=0;
      }
      if((PEREDAL_UPR_K_PRED!=0)||(PEREDAL_UPR_K_NEXT!=0))
      {
        BUF_OUT_NEXT[89]=ACTIV_SERV;
        BUF_OUT_NEXT[90]=ACTIV_SERV;
      }
      BUF_OUT_NEXT[0]=0x12;
      cr16=CalculateCRC16(&BUF_OUT_NEXT[1],94);
      BUF_OUT_NEXT[95]=cr16&0xFF;
			BUF_OUT_NEXT[96]=(cr16&0xFF00)>>8;
      BUF_OUT_NEXT[97]=0xd;

      //подготовить данные по ограничениям для предыдущего сервера
      for(i=0;i<98;i++)BUF_OUT_PRED[i]=0; //????
  /*    
      k=1;
      for(i=FR4_PRED;i<KOL_VO;i++)//пройти по всем объектам FR4
      {
        if(FR4[i]!=0) //если на объект наложены ограничения
        {

          BUF_OUT_PRED[k++]=i/256;  //старший байт номера объекта
          BUF_OUT_PRED[k++]=i%256;  //младший байт номера объекта
          BUF_OUT_PRED[k++]=FR4[i]; //записать ограничения
        }
        FR4_NEXT++;//перейти к следующему объекту
        if(k>=94)break;
      }
  */    
      //контрольная сумма
      cr16=CalculateCRC16(&BUF_OUT_PRED[1],94);
      BUF_OUT_PRED[95]=cr16&0xFF;BUF_OUT_PRED[96]=(cr16&0xFF00)>>8;

      BUF_OUT_PRED[0]=0x3C; BUF_OUT_PRED[97]=0x3E;//обрамление пакета
      atrib++;if(atrib>15)atrib=9;//изменение цвета

      if(REGIM==KANAL)
      {
        //вывести на экран ответ активному серверу(следующему)
        for(i=0;i<15;i++)putch1(BUF_OUT_NEXT[i]|0x40,0xa,60+i,3);
        //вывести на экран передаваемые ограничения (предыдущему)
        for(i=0;i<15;i++)putch1(BUF_OUT_PRED[i]|0x40,atrib,60+i,7);
      }
      outportb(ADR_SERV_NEXT+1,3);
			outportb(ADR_SERV_PRED+1,3);
    }
  }
  //если статус сервера не определен
  if(ACTIV==0xF)
  {
    ANALIZ_ACTIV_PASSIV();
  }
  return;
}
//============================================================================
/***********************************************\
*  int CalculateCRC16(void *pData, int dataLen) *
*        процедура расчета CRC-16               *
* pData - указатель на буфер данных контроля    *
* dataLen -длина буфера данных                  *
\***********************************************/
int CalculateCRC16(void *pData, int dataLen)
{
  unsigned char *ptr = (unsigned char *)pData;
  unsigned int c = 0xffff;
  int n;
  for (n = 0; n < dataLen; n++)
    c = crc16_table[ *ptr++ ^ (c>>8) ] ^ (c<<8);
  return c ^ 0xffff;
}
//==============================================================================
/***********************************************\
*  int CalculateCRC8(void *pData, int dataLen)  *
*        процедура расчета CRC-8                *
* pData - указатель на буфер данных контроля    *
* dataLen -длина буфера данных                  *
\***********************************************/
unsigned char CalculateCRC8(void *pData, int dataLen)
{
  unsigned char *ptr = (unsigned char *)pData;
  unsigned char c = 0xff;
  int n;
  for (n = 0; n < dataLen; n++)
    c = crc8_table[ *ptr++ ^ c ];
  return c ^ 0xff;
}
//================================================================================
/*******************************************\
*          VYVOD_ON_SCREEN()                *
* вывод на экран текущих значений некоторых *
* параметров системы РПЦ                    *
\*******************************************/
void VYVOD_ON_SCREEN(void)
{
  int i,j;
  unsigned char upr;
  if(REGIM==KONFIG)
  { textattr(0xa);
    for(i=0;i<Narm;i++) //пройти по всем АРМ 
    for(j=0;j<Nranj;j++)//пройти по всем районам 
    {
      upr=' ';
      //если район управляется этим АРМом 
      if(KONFIG_ARM[i][j]==0xFF)upr='█';
      if(KONFIG_ARM[i][j]==0x1)upr='1';
      if(KONFIG_ARM[i][j]==0x2)upr='2';
      putch1(upr,0xa,17+i*3,6+j*2);
    }
  }
  VYVOD_TUMS();           //вывод квитанций ТУМСа 
  return;
}
//============================================================================
/**********************************************\
*  Процедура анализа информации из АРМ-ШН      *
*           ANALIZ_SHN()                       *
\**********************************************/
void ANALIZ_SHN(void)
{
  int i;
  unsigned char CRCs;
  if(END_SHN!=0)
  {
    if((BUF_IN_SHN[0]==0xAA)&&(BUF_IN_SHN[END_SHN]==0x55))//если окантовка 
    {
      if(OSN_SHN[0]==0) for(i=0;i<28;i++)OSN_SHN[i]=BUF_IN_SHN[i];
      for(i=0;i<28;i++)BUF_IN_SHN[i]=0; //очистить буфер 
      if(OSN_SHN[0]==0xAA)//если начало пакета соответствует 
      {
        CRCs=CalculateCRC8(&OSN_SHN[1],25);//подсчитать контр.сумму 
        if(CRCs!=OSN_SHN[26])for(i=0;i<28;i++)OSN_SHN[i]=0; //если не та
        else
        {
          if(REGIM==0)
          {
            putch1(16,9,80,21);
            putch1(17,9,58+9*(SERVER-1),45-2*(SERVER-1));
          }
        }
      }
    }
  }
  return;
}
//=================================================================================
/*****************************************\
*    Процедура анализа данных,            *
*         принятых из АРМа                *
*             ANALIZ_ARM()                *
\*****************************************/
void ANALIZ_ARM(void)
{
	int ik,il,serv;
	unsigned char CRCs;
	if(END_ARM!=0)//если есть конец информации АРМа по основному каналу
	{ if((BUF_IN_ARM[0]==0xAA)&&(BUF_IN_ARM[END_ARM]==0x55))//если окантовка
		{ //если свободен 1-й буфер основного канала - закачать его данными
			if(OSN1_KS[0]==0) for(il=0;il<28;il++)OSN1_KS[il]=BUF_IN_ARM[il];
			else
				if(OSN2_KS[0]==0)for(il=0;il<28;il++)OSN2_KS[il]=BUF_IN_ARM[il];
				//если сервер не активен, то выделить адрес запроса
				if(ACTIV!=1)ZAPROS_ARM=(BUF_IN_ARM[1]&0xF0)>>4;

				//если АРМ активный в каком-либо районе,то записать в архив
//        if((KONFIG_ARM[ZAPROS_ARM-4][0]==0xFF)||(KONFIG_ARM[ZAPROS_ARM-4][1]==0xFF))
//        add_ARM_IN(ZAPROS_ARM,0); //записать принятое в архив
				for(il=0;il<28;il++)BUF_IN_ARM[il]=0; //очистить буфер приема
		}

		if(OSN1_KS[0]==0xAA)//если начало пакета соответствует
		{
			CRCs=CalculateCRC8(&OSN1_KS[1],25);//подсчитать контр.сумму
			if(CRCs!=OSN1_KS[26])for(il=0;il<28;il++)OSN1_KS[il]=0; //если не та
			else
			{
				if(ACTIV==1)
				{
					serv=OSN1_KS[1]&0xF;
					if(serv!=SERVER)ACTIV=0;
				}
				if(ACTIV!=1)ZAPROS_ARM=(OSN1_KS[1]&0xF0)>>4;//выделить запрос
				if(REGIM==0)//для основного окна изобразить яркие стрелки
				{
					putch1(31,0xa,11+9*(ZAPROS_ARM-4),35);
					putch1(31,0xa,54+9*(SERVER-1),43-2*(SERVER-1));
				}
				if(REGIM==KANAL)//для окна каналов вывести строку буфера
				for(ik=0;ik<28;ik++)putch1(OSN1_KS[ik]|0x40,atrib,43+ik,19+(ZAPROS_ARM-4)*6);
			}
		}
		if(OSN2_KS[0]==0xAA)//аналогично с запасным буфером
		{
			CRCs=CalculateCRC8(&OSN2_KS[1],25);
			if(CRCs!=OSN2_KS[26])for(il=0;il<28;il++)OSN2_KS[il]=0;
			else
			{
				if(ACTIV==1)
				{
					serv=OSN2_KS[1]&0xF;
					if(serv!=SERVER)ACTIV=0;
				}
				if(ACTIV==0)ZAPROS_ARM=(OSN2_KS[1]&0xF0)>>4;
				if(REGIM==0)
				{
					putch1(31,0xa,11+9*(ZAPROS_ARM-4),35);
					putch1(31,0xa,54+9*(SERVER-1),43-2*(SERVER-1));
				}
				if(REGIM==KANAL)
				for(ik=0;ik<28;ik++)putch1(OSN2_KS[ik]|0x40,atrib,43+ik,19+(ZAPROS_ARM-4)*6);
			}
		}
		if(OSN1_KS[0]==0xAA)RASPAK_ARM(1,0,0xff);
		if(OSN2_KS[0]==0xAA)RASPAK_ARM(2,0,0xff);
		if(OSN1_KS[0]==0xAA)for(ik=0;ik<28;ik++)OSN1_KS[ik]=0;
		if(OSN2_KS[0]==0xAA)for(ik=0;ik<28;ik++)OSN2_KS[ik]=0;
		END_ARM=0;
	}

	if(END_ARM1!=0)//если есть конец информации по резервному каналу
	{
		if((BUF_IN_ARM1[0]==0xAA)&&(BUF_IN_ARM1[END_ARM1]==0x55))
		{
			if(REZ1_KS[0]==0)for(il=0;il<28;il++)REZ1_KS[il]=BUF_IN_ARM1[il];
			else
				if(REZ2_KS[0]==0)for(il=0;il<28;il++)REZ2_KS[il]=BUF_IN_ARM1[il];
				if(ACTIV==1)
				{
					serv=BUF_IN_ARM1[1]&0xF;
					if(serv!=SERVER)ACTIV=0;
				}
				if(ACTIV!=1)ZAPROS_ARM1=(BUF_IN_ARM1[1]&0xF0)>>4;//резервный запрос
				if(REGIM==0)//на основном экране вывести яркие стрелки "запрос-ответ"
				{
					putch1(30,0xe,11+9*(ZAPROS_ARM1-4),27);
//          putch1(17,15,58+9*(SERVER-1),45-2*(SERVER-1));
					putch1(31,14,56+9*(SERVER-1),43-2*(SERVER-1)); //стрелочки из резервной шины АРМ в сервер
				}
				//на экране каналов вывести буфер
				if(REGIM==KANAL)for(ik=0;ik<28;ik++)putch1(BUF_IN_ARM1[ik]|0x40,atrib,43+ik,22+(ZAPROS_ARM1-4)*6);
//				if((KONFIG_ARM[ZAPROS_ARM1-4][0]==0xFF)||
//				(KONFIG_ARM[ZAPROS_ARM1-4][1]==0xFF))
//        add_ARM_IN(ZAPROS_ARM1,1);
				for(il=0;il<28;il++)BUF_IN_ARM1[il]=0;//очистить резервный буфер
		}
		if(REZ1_KS[0]==0xAA)
		{
			CRCs=CalculateCRC8(&REZ1_KS[1],25);
			if(CRCs!=REZ1_KS[26])for(il=0;il<28;il++)REZ1_KS[il]=0;
		}
		if(REZ2_KS[0]==0xAA)
		{
			CRCs=CalculateCRC8(&REZ2_KS[1],25);
			if(CRCs!=REZ2_KS[26])for(il=0;il<28;il++)REZ2_KS[il]=0;
		}
		if(REZ1_KS[0]==0xAA)
		{
			RASPAK_ARM(3,1,0xff);
			for(ik=0;ik<28;ik++)REZ1_KS[ik]=0;
		}
		if(REZ2_KS[0]==0xAA)
		{
			RASPAK_ARM(4,1,0xff);
			for(ik=0;ik<28;ik++)REZ2_KS[ik]=0;
		}
		END_ARM1=0;
	}
	return;
}
//============================================================================
/***************************************\
/*     Процедура распаковки данных,     *
/*          принятых из АРМа            *
/* RASPAK_ARM(int bb,unsigned char STAT)*
/* bb - код  буфера записи данных       *
/* STAT -код канала (0-осн.; 1-рез.)    *
\***************************************/
void RASPAK_ARM(int bb,unsigned char STAT,int arm)
{
	unsigned char ARM,RAY;
	int ii,jj,kvit;
	if(bb!=0xff)
	{
		for(ii=0;ii<28;ii++)KOM_BUFER[ii]=0;
		switch(bb) //перезапись данных
		{
			case 1: for(ii=0;ii<28;ii++)KOM_BUFER[ii]=OSN1_KS[ii];break;
			case 2: for(ii=0;ii<28;ii++)KOM_BUFER[ii]=OSN2_KS[ii];break;
			case 3: for(ii=0;ii<28;ii++)KOM_BUFER[ii]=REZ1_KS[ii];break;
			case 4: for(ii=0;ii<28;ii++)KOM_BUFER[ii]=REZ2_KS[ii];break;
			default: return;
		}
	}
	else
	{
		if(arm!=0xff)ARM=arm;
	}
	if(KOM_BUFER[1]!=0)
	{ ARM=((KOM_BUFER[1]&0xf0)>>4)-4;  // выделение номера АРМа
		if((KOM_BUFER[2]&0x20)==0x20)
		{
			KNOPKA_OK[ARM]=1;              // выделение признака ОК
			OK_KNOPKA=OK_KNOPKA|(1<<ARM);
		}
		else
		{
			KNOPKA_OK[ARM]=0;
			OK_KNOPKA=OK_KNOPKA&(~(1<<ARM));
		}
		RAY=KOM_BUFER[2]&0x3;            //выделение района
		if(ACTIV!=1)
		{
			if((KOM_BUFER[2]&0x80)==0x80)  //если АРМ основной в районе
			{
				for(ii=0;ii<Narm;ii++)
				{
					if(KONFIG_ARM[ii][RAY-1]!=0)KONFIG_ARM[ii][RAY-1]=RAY;
				}
				KONFIG_ARM[ARM][RAY-1]=0xFF;
			}
			else KONFIG_ARM[ARM][RAY-1]=RAY;

		}
	}
	//если поступила команда установки времени
	if((KOM_BUFER[3]==102)||(KOM_BUFER[3]==101))
	{
		for(ii=0;ii<7;ii++)KOM_TIME[ii]=KOM_BUFER[4+ii];//заполнить буфер команд
		if(KOM_BUFER[11]>224)            //если вначале квитанции
		{
			kvit=KOM_BUFER[11]-224;        //определить число квитанций и переписать
			for(ii=0,jj=12;(ii<kvit)&&(jj<26);ii++,jj=jj+2)
			KVIT_ARM[ARM][STAT][ii]=KOM_BUFER[jj]+(KOM_BUFER[jj+1]<<8);
		}
//    add_ARM_IN(ARM,3); //записать принятое в архив
		MAKE_TIME(ARM,STAT);
		for(ii=0;ii<7;ii++)KOM_TIME[ii]=0;
		for(ii=0;ii<28;ii++)KOM_BUFER[ii]=0;
		return;
	}
	else
	{
		if(((KOM_BUFER[3]>0)&&(KOM_BUFER[3]<187))|| //если начало-раздельная команда
		((KOM_BUFER[3]>192)&&(KOM_BUFER[3]<203)))   //то переписать ее в буф. команд
		{
			for(ii=0,jj=3;ii<3,jj<6;ii++,jj++)KOMANDA_RAZD[ARM][STAT][ii]=KOM_BUFER[jj];

			if(KOM_BUFER[6]>224)               //если далее квитанции
			{ kvit=KOM_BUFER[6]-224;           //определить число квитанций
				for(ii=0,jj=7;(ii<kvit)&&(jj<26);ii++,jj=jj+2)//переписать квитанции
				KVIT_ARM[ARM][STAT][ii]=KOM_BUFER[jj]+(KOM_BUFER[jj+1]<<8);
			}
//      if((KONFIG_ARM[ZAPROS_ARM][0]!=0xFF)&&
//			(KONFIG_ARM[ZAPROS_ARM][1]!=0xFF))

//      add_ARM_IN(ARM,3); //записать принятое в архив

		}
		else                              //если начало без команд
		{
			if(KOM_BUFER[3]>224)            //если вначале квитанции
			{
				kvit=KOM_BUFER[3]-224;        //определить число квитанций и переписать
				for(ii=0,jj=4;(ii<kvit)&&(jj<26);ii++,jj=jj+2)
				KVIT_ARM[ARM][STAT][ii]=KOM_BUFER[jj]+(KOM_BUFER[jj+1]<<8);
			}
			else                            //если в начале нет квитанций
			{
				if((KOM_BUFER[3]>=187)&&(KOM_BUFER[3]<=192))  //вначале маршр. команда
				{
					for(ii=0,jj=3;ii<10,jj<13;ii++,jj++)        //команду в буфер
					KOMANDA_MARS[ARM][STAT][ii]=KOM_BUFER[jj];
					if(KOM_BUFER[13]>224)                       //если далее квитанции
					{
						kvit=KOM_BUFER[13]-224;                   // число квитанций
						for(ii=0,jj=14;(ii<kvit)&&(jj<26);ii++,jj=jj+2)
						KVIT_ARM[ARM][STAT][ii]=KOM_BUFER[jj]+(KOM_BUFER[jj+1]<<8);
					}
				}
			}
		}
		for(ii=0;ii<28;ii++)KOM_BUFER[ii]=0; //очистить буфер
		ANALIZ_KVIT_ARM(ARM,STAT); //проанализировать квитанции АРМа
		//если есть раздельная
		if(KOMANDA_RAZD[ARM][STAT][0]!=0)
		{
			KVIT_ARMu[ARM][STAT][0]=KOMANDA_RAZD[ARM][STAT][1];
			KVIT_ARMu[ARM][STAT][1]=KOMANDA_RAZD[ARM][STAT][2]|0x40;
			KVIT_ARMu[ARM][STAT][2]=0;
			//если есть первая раздельная и просмотр команд
			if((KOMANDA_RAZD[ARM][STAT][0]!=0)&&(REGIM==COMMAND))
			{
				if(Y_KOM!=3)putch1(' ',0x5,1,Y_KOM-1);    //погасить старый указатель
				else putch1(' ',0x5,1,46);
				putch1(0x10,0x5,1,Y_KOM);                 //включить новый указатель
				puts1(TIME,0xa,3,Y_KOM);

				t1=KOMANDA_RAZD[ARM][STAT][0];
				itoa(t1,delta,10);  puts1(delta,0x5,13,Y_KOM);//код команды на экран
				if(t1<100)putch1(0x20,0x5,15,Y_KOM);
				if(t1<10)putch1(0x20,0x5,14,Y_KOM);

				t1=KOMANDA_RAZD[ARM][STAT][1]+KOMANDA_RAZD[ARM][STAT][2]*256;
				itoa(t1,delta,10); puts1(delta,0x5,17,Y_KOM);  //объект команды на экран
				if(t1<100)putch1(0x20,0x5,19,Y_KOM);
				if(t1<10)putch1(0x20,0x5,18,Y_KOM);
				if(((KOMANDA_RAZD[ARM][STAT][0]<=36)&&(KOMANDA_RAZD[ARM][STAT][0]>=31))||
				(KOMANDA_RAZD[ARM][STAT][0]==79)||(KOMANDA_RAZD[ARM][STAT][0]==80)||(KOMANDA_RAZD[ARM][STAT][0]==85))
				Y_KOM++;
				if(Y_KOM>=47)Y_KOM=3;
			}
			MAKE_KOMANDA(ARM,STAT,RAY);
			for(ii=0;ii<16;ii++)KOMANDA_RAZD[ARM][STAT][ii]=0;

		}
		if(KOMANDA_MARS[ARM][STAT][0]!=0)
		{
			KVIT_ARMu[ARM][STAT][0]=KOMANDA_MARS[ARM][STAT][1];
			KVIT_ARMu[ARM][STAT][1]=KOMANDA_MARS[ARM][STAT][2]|0x40;
			KVIT_ARMu[ARM][STAT][2]=1;
			//далее для режима просмотра команд
			if((KOMANDA_MARS[ARM][STAT][0]!=0)&&(REGIM==COMMAND)) //если есть маршрутная
			{
				if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1); //погасить старый указатель
				else putch1(' ',0xc,1,46);
				putch1(0x10,0xc,1,Y_KOM);//включить новый указатель
				puts1(TIME,0xa,3,Y_KOM);
				t1=KOMANDA_MARS[ARM][STAT][0];
				itoa(t1,delta,10);  puts1(delta,0xd,13,Y_KOM);  //код команды на экран
				if(t1<100)putch1(0x20,0xd,15,Y_KOM);
				if(t1<10)putch1(0x20,0xd,14,Y_KOM);

				//номер объекта начала вывести на экран
				t1=KOMANDA_MARS[ARM][STAT][1]+KOMANDA_MARS[ARM][STAT][2]*256;
				itoa(t1,delta,10);  puts1(delta,0xd,18,Y_KOM);
				if(t1<1000)putch1(0x20,0xd,21,Y_KOM);
				if(t1<100)putch1(0x20,0xd,20,Y_KOM);
				if(t1<10)putch1(0x20,0xd,19,Y_KOM);

				//номер объекта конца вывести на экран
				t1=KOMANDA_MARS[ARM][STAT][3]+KOMANDA_MARS[ARM][STAT][4]*256;
				itoa(t1,delta,10);  puts1(delta,0xd,23,Y_KOM);
				if(t1<1000)putch1(0x20,0xd,26,Y_KOM);
				if(t1<100)putch1(0x20,0xd,25,Y_KOM);
				if(t1<10)putch1(0x20,0xd,24,Y_KOM);

				//число стрелок вывести на экран
				t1=KOMANDA_MARS[ARM][STAT][5];
				itoa(t1,delta,10); puts1(delta,0xd,28,Y_KOM);
				if(t1<10)putch1(0x20,0xd,29,Y_KOM);

				//положение стрелок вывести на экран
				long_test=KOMANDA_MARS[ARM][STAT][6]+KOMANDA_MARS[ARM][STAT][7]*256+
				KOMANDA_MARS[ARM][STAT][8]*256*256+KOMANDA_MARS[ARM][STAT][9]*256*256*256;
				ltoa(long_test,delta,16);strcat(delta,"    ");
				puts1(delta,0xd,31,Y_KOM);
			}
			MAKE_MARSH(ARM,STAT);
			for(ii=0;ii<10;ii++)KOMANDA_MARS[ARM][STAT][ii]=0;
		}
		return;
	}
}
//=========================================================================
/**********************************************\
*      ANALIZ_KVIT_ARM(int arm,int stat)       *
* процедура анализа квитанций,принятых от АРМа *
* arm - индекс АРМа, закончившего сеанс обмена *
* stat- код канала связи,осуществившего обмен  *
\**********************************************/
void ANALIZ_KVIT_ARM(int arm,int stat)
{
  int ii,jj,oo,ob;
  if(KVIT_ARM[arm][stat][0]==0)return;
  switch(stat)
  {
    case 0: for(ii=0;ii<18;ii++)//пройти по всем возможным квитанциям
            {
              if(KVIT_ARM[arm][0][ii]==0)continue;
              for(jj=0;jj<32;jj++)//пройти по всем пакетам
              { if(KVIT_ARM[arm][0][ii]==PAKETs[jj].KS_OSN)//найден в основном
                {
                  for(oo=0;oo<21;oo++)//занести прием во все объекты пакета
                  {
                    ob=OBJ_ARMu[jj].OBJ[oo];
                    PRIEM[ob]=PRIEM[ob]|(1<<arm);
                  }
                  PAKETs[jj].ARM_OSN_KAN=PAKETs[jj].ARM_OSN_KAN&(~(1<<arm));//записать прием
                  KVIT_ARM[arm][0][ii]=0;//сбросить квитанцию
                  break;
                }
              }
            }
            break;
    case 1: for(ii=0;ii<18;ii++)
            { if(KVIT_ARM[arm][1][ii]==0)continue;
              for(jj=0;jj<32;jj++)
              { if(KVIT_ARM[arm][1][ii]==PAKETs[jj].KS_REZ)
                {
                  for(oo=0;oo<21;oo++)
                  {
                    ob=OBJ_ARMu1[jj].OBJ[oo];
                    PRIEM[ob]=PRIEM[ob]|(1<<arm);
                  }
                  PAKETs[jj].ARM_REZ_KAN=PAKETs[jj].ARM_REZ_KAN&(~(1<<arm));
                  KVIT_ARM[arm][1][ii]=0;
                  break;
                }
              }
            }
            break;
    default:break;
  }
  KVIT_ARM[arm][stat][0]=0;
  return;
}
//==============================================================================
/**************************************************\
*        int ANALIZ_KVIT_SERV(int sosed)           *
* анализ квитанции,полученной от соседнего сервера *
* sosed - код соседа: 0-предыдущий 1-следующий     *
\**************************************************/
int ANALIZ_KVIT_SERV(int sosed)
{
  unsigned int cr16=0,ii;
  switch(sosed)//переключатель по соседу 0-предыдущий 1-следующий
  {
    case 0: cr16=CalculateCRC16(&BUF_IN_PRED[1],94);
            if(cr16!=(BUF_IN_PRED[95]+(BUF_IN_PRED[96]<<8)))
            {
              if(REGIM==KANAL)putch1((error1++),0xc,79,6);
              outportb(ADR_SERV_PRED+2,0xc7);
              return(-1);
            }
            else
            {
              error1=48;
              if(REGIM==KANAL)putch1('N',0xa,79,7);
              for(ii=0;ii<21;ii++)
              {
                OBJ_ARMu[N_PAKET].OBJ[ii]=BUF_IN_PRED[2*ii+1]+(BUF_IN_PRED[2*ii+2]<<8);
                PEREDACHA[OBJ_ARMu[N_PAKET].OBJ[ii]]=PEREDANO;
              }
              PAKETs[N_PAKET].KS_OSN=BUF_IN_PRED[43]+(BUF_IN_PRED[44]<<8);
              for(ii=0;ii<21;ii++)
              {
                OBJ_ARMu1[N_PAKET].OBJ[ii]=BUF_IN_PRED[2*ii+45]+(BUF_IN_PRED[2*ii+46]<<8);
                PEREDACHA[OBJ_ARMu1[N_PAKET].OBJ[ii]]=PEREDANO;
              }
              PAKETs[N_PAKET++].KS_REZ=BUF_IN_PRED[87]+(BUF_IN_PRED[88]<<8);
              if(N_PAKET==32)N_PAKET=0;
              return(0);
            }

    case 1:
            cr16=CalculateCRC16(&BUF_IN_NEXT[1],94);
            if(cr16!=(BUF_IN_NEXT[95]+(BUF_IN_NEXT[96]<<8)))
            {
              if(REGIM==KANAL)putch1(error2++,0xc,79,4);
              outportb(ADR_SERV_NEXT+2,0xc7);
              return(-1);
            }
            else
            {
              error2=48;
              if(REGIM==KANAL)putch1('N',0xa,79,3);
              for(ii=0;ii<21;ii++)
              {
                OBJ_ARMu[N_PAKET].OBJ[ii]=BUF_IN_NEXT[2*ii+1]+(BUF_IN_NEXT[2*ii+2]<<8);
                PEREDACHA[OBJ_ARMu[N_PAKET].OBJ[ii]]=PEREDANO;
              }
              PAKETs[N_PAKET].KS_OSN=BUF_IN_NEXT[43]+(BUF_IN_NEXT[44]<<8);
              for(ii=0;ii<21;ii++)
              {
                OBJ_ARMu1[N_PAKET].OBJ[ii]=BUF_IN_NEXT[2*ii+45]+(BUF_IN_NEXT[2*ii+46]<<8);
                PEREDACHA[OBJ_ARMu1[N_PAKET].OBJ[ii]]=PEREDANO;
              }
              PAKETs[N_PAKET++].KS_REZ=BUF_IN_NEXT[87]+(BUF_IN_NEXT[88]<<8);
              if(N_PAKET==32)N_PAKET=0;
              return(0);
            }
   default: return(0);
  }
}
//=============================================================================
/*******************************************\
*          READ_BD(int obj)                 *
* чтение данных из основной базы данных     *
* obj - номер объекта базы данных сервера   *
\*******************************************/
void READ_BD(int obj)
{
  int ii;
  for(ii=0;ii<16;ii++)bd_osn[ii]=BD_OSN[obj][ii];
  return;
}
//================================================================================
/***********************************************\
*                                               *
*          ANALIZ_ACTIV_PASSIV()                *
*  процедура активизации сервера при включении  *
*                                               *
\***********************************************/
void ANALIZ_ACTIV_PASSIV(void)
{
  if(T0<5){MOLCHI=0;ACTIV=0xF;}
  else
  {
    if(ACTIV==0xF)//если переходное состояние
    { //если все молчат
      if((sosed_NEXT<=0)&&(sosed_PRED<=0)&&(inf_TUMS<=0)&&(inf_ARM<=0))
      {
        if(T0>=10)//если время перехода закончено
        { if(MOLCHI>0)
          {
            if((MOLCHI&0xFFF)==SERVER)
            {
              if((podtv_NEXT==1)&&(podtv_PRED==1))
              {

                sbros_kom();
                ACTIVNOST();
                ZAPROS_TUMS=0; ZAPROS_TUMS1=3;
                ZAPROS_ARM=4; ZAPROS_ARM1=8;

                ACTIV_SERV=SERVER;
                T_MIN_NEXT=0;
                T_MIN_PRED=0;
              }
              else
              if(T0>=16)
              {

                if(ZAPROS_TUMS<6)ZAPROS_TUMS1=ZAPROS_TUMS+3;
                else ZAPROS_TUMS1=ZAPROS_TUMS-5;

                if(ZAPROS_ARM!=4)ZAPROS_ARM1=ZAPROS_ARM-1;
                else ZAPROS_ARM1=8;
                cikl_in_pred=0;
                cikl_out_pred=0;
                cikl_in_next=0;
                cikl_out_next=0;
                T_MIN_NEXT=0;
                T_MIN_PRED=0;
                T0=16;
                outportb(ADR_SERV_NEXT+2,0xC7);
                outportb(ADR_SERV_PRED+2,0xC7);
                outportb(ADR_ARM_OSN+2,0xC7);
                outportb(ADR_ARM_REZ+2,0xC7);
                ACTIVNOST();
                ZAPROS_TUMS=0; ZAPROS_TUMS1=3;
                ZAPROS_ARM=4; ZAPROS_ARM1=8;
                sbros_kom();
                ACTIV_SERV=SERVER;
              }
            }
            else
            if(T0>=16)
            {
              if(ZAPROS_TUMS<6)ZAPROS_TUMS1=ZAPROS_TUMS+3;
              else ZAPROS_TUMS1=ZAPROS_TUMS-5;
              if(ZAPROS_ARM!=4)ZAPROS_ARM1=ZAPROS_ARM-1;
              else ZAPROS_ARM1=8;
              cikl_in_pred=0;
              cikl_out_pred=0;
              cikl_in_next=0;
              cikl_out_next=0;
              T_MIN_NEXT=0;
              T_MIN_PRED=0;
              T0=16;
              outportb(ADR_SERV_NEXT+2,0xC7);
              outportb(ADR_SERV_PRED+2,0xC7);
              outportb(ADR_ARM_OSN+2,0xC7);
              outportb(ADR_ARM_REZ+2,0xC7);
              ACTIVNOST();

              ZAPROS_TUMS=0; ZAPROS_TUMS1=3;
              ZAPROS_ARM=4; ZAPROS_ARM1=8;
              sbros_kom();
              ACTIV_SERV=SERVER;
            }
          }
          else
          if((MOLCHI&0xFFF)!=SERVER)
          {
            MOLCHI=SERVER|0x8000;//подготовить "молчи"
            BUF_OUT_NEXT[89]=SERVER;//отправить молчи соседям
            BUF_OUT_NEXT[90]=SERVER;
            BUF_OUT_PRED[89]=SERVER;
            BUF_OUT_PRED[90]=SERVER;
            T0=6;
          }
        }
        else //если время перехода не закончено
        {
          if((MOLCHI>0)&&(MOLCHI<4))//если есть принятый молчи
          {
            if(MOLCHI==SERVER)//если мой молчи
            {
              if((BUF_IN_NEXT[89]==SERVER)&&(BUF_IN_NEXT[90]==SERVER))podtv_NEXT=1;
              if((BUF_IN_PRED[89]==SERVER)&&(BUF_IN_PRED[90]==SERVER))podtv_PRED=1;
            }
            else  //если чужой молчи
            {

              if((BUF_IN_NEXT[89]>0)&&(BUF_IN_NEXT[90]<4))
              { putch1('5',0xb,41,50);
                BUF_OUT_PRED[89]=BUF_IN_NEXT[89];
                BUF_OUT_PRED[90]=BUF_IN_NEXT[90];
                BUF_OUT_NEXT[89]=BUF_IN_NEXT[89];
                BUF_OUT_NEXT[90]=BUF_IN_NEXT[90];

              }
              if((BUF_IN_PRED[89]>0)&&(BUF_IN_PRED[90]<4))
              {
                BUF_OUT_NEXT[89]=BUF_IN_PRED[89];
                BUF_OUT_NEXT[90]=BUF_IN_PRED[90];
                BUF_OUT_PRED[89]=BUF_IN_PRED[89];
                BUF_OUT_PRED[90]=BUF_IN_PRED[90];

              }
              cikl_in_pred=0;
              cikl_out_pred=0;
              cikl_in_next=0;
              cikl_out_next=0;
              T_MIN_PRED=0;
              T_MIN_NEXT=0;
              T0=16;
              outportb(ADR_SERV_NEXT+2,0xC7);
              outportb(ADR_SERV_PRED+2,0xC7);
              outportb(ADR_SERV_NEXT+1,1);
              outportb(ADR_SERV_PRED+1,1);
              outportb(ADR_ARM_OSN+2,0xC7);
              outportb(ADR_ARM_REZ+2,0xC7);
              ACTIV=0;
              if(REGIM==KANAL)tablica();
              else
              {
                REGIM=0;
                main_win();
              }
            }
          }
          else//если нет "молчи"
          {
            if((MOLCHI&0xFFF)!=SERVER)
            {
              MOLCHI=SERVER|0x8000;//подготовить "молчи"
              BUF_OUT_NEXT[89]=SERVER;//отправить молчи соседям
              BUF_OUT_NEXT[90]=SERVER;
              BUF_OUT_PRED[89]=SERVER;
              BUF_OUT_PRED[90]=SERVER;
              T0=6;
            }
          }
        }
      }
      else
      {
        cikl_in_pred=0;
        cikl_out_pred=0;
        cikl_in_next=0;
        cikl_out_next=0;
        T_MIN_PRED=0;
        T_MIN_NEXT=0;
        T0=16;
        outportb(ADR_SERV_NEXT+2,0xC7);
        outportb(ADR_SERV_PRED+2,0xC7);
        outportb(ADR_SERV_NEXT+1,1);
        outportb(ADR_SERV_PRED+1,1);
        outportb(ADR_ARM_OSN+2,0xC7);
        outportb(ADR_ARM_REZ+2,0xC7);
        ACTIV=0;
        if(REGIM==KANAL)tablica();
        else
        {
          REGIM=0;
          main_win();
        }
      }
    }
  }
  return;
}
//==============================================================================
/*************************************\
*   отладочная вставка для проверки   *
*   длительности коротких процессов   *
\*************************************/
/*t1=mikrotiki();
  X0=wherex();
  Y0=wherey();
  t2=mikrotiki();
  if(t2<t1)t1=t1-t2;
  else t1=t1+(65535-t2);
  itoa(t1,delta,10);strcat(delta," ");
  if(t1>30000)gotoxy(60,50);
  else
    if(t1>20000)gotoxy(50,50);
    else
      if(t1>10000)gotoxy(40,50);
      else
        if(t1>5000)gotoxy(30,49);
        else
          if(t1>2500)gotoxy(20,49);
          else
            if(t1>1000)gotoxy(10,49);
            else gotoxy(1,49);
  puts(delta);
  gotoxy(X0,Y0);
*/
//==========================================================================
/***********************************************************\
* putch1(unsigned char simb,unsigned char cvt,int X, int Y) *
*        прямой вывод в видеопамять байта текста            *
*        simb - выводимый байт данных                       *
*        cvt - цветовой атрибут символа                     *
*        X - координата x на экране точки вывода символа    *
*        Y - координата y на экране точки вывода символа    *
\***********************************************************/
void putch1(char simb,unsigned char cvt,int X, int Y)
{
  int dx,simbol;
  unsigned char smb;
  smb=simb;
  Y=Y-1;
  X=X-1;
  simbol=(cvt<<8)+smb;
  dx=Y*160+2*X;  //смещение в памяти
  poke(0xB800,dx,simbol);
  return;
}
//=============================================================================
/*****************************************************************\
*     puts1(unsigned char *simb,unsigned char cvt,int X, int Y)   *
* прямой вывод в видеопамять строки текстовых байтов              *
*        simb - указатель на буфер выводимой строки               *
*        cvt - цветовой атрибут символа                           *
*        X - координата x на экране точки вывода символа          *
*        Y - координата y на экране точки вывода символа          *
\*****************************************************************/

void puts1(char *simb,unsigned char cvt,int X, int Y)
{
  int dx,simbol,i;
  unsigned char smb;
  i=0;
  Y=Y-1;
  X=X-1;
  while(simb[i]!=0)
  {
    smb=simb[i++];
    simbol=(cvt<<8)+smb;
    dx=Y*160+2*X;  //смещение в памяти
    poke(0xB800,dx,simbol);
    X++;
  }
  return;
}
//=============================================================================
/*****************************************\
*         VYVOD_TUMS()                    *
* определение запроса, на который получен *
* ответ из ТУМСа и его вывод на экран     *
\*****************************************/
void VYVOD_TUMS(void)
{
  int i;
  if(ADR_TUMS_OUT>0)//если есть адрес вызова ТУМСа по основному каналу
  { //если есть новый вызов ТУМСа по тикеру или был прием сообщ. от ТУМСа
    if((ZAPROS[4]==0)||(ZAPROS[5]==0))//
    {
      ZAPROS[4]=0xF;
      ZAPROS[5]=0XF;
      if(REGIM==KANAL)
      {
        for(i=0;i<3;i++)
        {
          putch1(ZAPROS[i],atrib,X_out[ZAPROS_TUMS-1],Y_out[ZAPROS_TUMS-1]);//показать запрос на экране
          X_out[ZAPROS_TUMS-1]=X_out[ZAPROS_TUMS-1]+1;
        }
        if(X_out[ZAPROS_TUMS-1]>=30)//если дошли до конца окна вывода
        {
          X_out[ZAPROS_TUMS-1]=7;//вернуться в начало окна
          atrib++;//поменять цвет букв
          if(atrib>=15)atrib=1;
        }
      }
    }
  }
  if(ADR_TUMS_OUT1>0)//если есть адрес вызова ТУМСа по резервному каналу
  {
    if((ZAPROS1[4]==0)||(ZAPROS1[5]==0))
    {
      ZAPROS1[4]=0xF;
      ZAPROS1[5]=0xF;
      if(REGIM==KANAL)
      {
        for(i=0;i<3;i++)
        {
          putch1(ZAPROS1[i],atrib,X_out_rez[ZAPROS_TUMS1-1],Y_out_rez[ZAPROS_TUMS1-1]);
          X_out_rez[ZAPROS_TUMS1-1]=X_out_rez[ZAPROS_TUMS1-1]+1;
        }
        if(X_out_rez[ZAPROS_TUMS1-1]>=30)X_out_rez[ZAPROS_TUMS1-1]=7;
      }
    }
  }
  //------------РАБОТА С основным каналом ТУМСов
  if(KVIT_TUMS[ZAPROS_TUMS-1][4]!=0)//есть готовая квитанция для ТУМС
  { if(REGIM==KANAL)
    { gotoxy(X_out[ZAPROS_TUMS-1],Y_out[ZAPROS_TUMS-1]);
      cputs(KVIT_TUMS[ZAPROS_TUMS-1]);//на экран
      X_out[ZAPROS_TUMS-1]=wherex();
      if(X_out[ZAPROS_TUMS-1]>29)X_out[ZAPROS_TUMS-1]=7;
      if(ACTIV==0)KVIT_TUMS[ZAPROS_TUMS-1][4]=0;
    }
    for(i=0;i<5;i++)KVIT_TUMS[ZAPROS_TUMS-1][i]=0;
  }
  //---------------------РАБОТА С РЕЗЕРВНЫМ КАНАЛОМ ТУМСов
  //есть готовая квитанция для ТУМС
  if((KVIT_TUMS1[ZAPROS_TUMS1-1][4]!=0)&&(REGIM==KANAL))
  {
    if(REGIM==KANAL)
    {
      gotoxy(X_out_rez[ZAPROS_TUMS1-1],Y_out_rez[ZAPROS_TUMS1-1]);
      cputs(KVIT_TUMS1[ZAPROS_TUMS1-1]);//на экран
      X_out_rez[ZAPROS_TUMS1-1]=wherex();
      if(X_out_rez[ZAPROS_TUMS1-1]>29)X_out_rez[ZAPROS_TUMS1-1]=7;
      if(ACTIV==0)KVIT_TUMS1[ZAPROS_TUMS1-1][4]=0;
    }
    for(i=0;i<5;i++)KVIT_TUMS1[ZAPROS_TUMS1-1][i]=0;
  }
  return;
}
//*********************************************
void sbros_kom(void)
{
  int i,j;
  for(i=0;i<Narm;i++)
    for(j=0;j<12;j++)
    {
      KOMANDA_MARS[i][0][j]=0;
      KOMANDA_RAZD[i][0][j]=0;
    }
  for(i=0;i<Nst;i++)
    for(j=0;j<15;j++)
    {
      KOMANDA_ST[i][j]=0;
			KOMANDA_TUMS[i][j]=0;
    }
  for(i=0;i<3;i++)DIAGNOZ[i]=0;
  for(i=0;i<6;i++)ERR_PLAT[i]=0;

  return;
}
//****************************************************
void ACTIVNOST(void)
{ int i_m;
	ACTIV=1;
  SET_TIME=0xF;
  cikl_arm=0;
  ZAPROS_TUMS=0; ZAPROS_TUMS1=3;
  ZAPROS_ARM=4; ZAPROS_ARM1=8;
  ACTIV_SERV=SERVER;
	sbros_kom();
	for(i_m=0;i_m<Nst*3;i_m++)DeleteMarsh(i_m);
  cikl_in_pred=0; cikl_out_pred=0;
  cikl_in_next=0; cikl_out_next=0;
  outportb(ADR_SERV_NEXT+2,0xC7); outportb(ADR_SERV_PRED+2,0xC7);
  outportb(ADR_ARM_OSN+2,0xC7);   outportb(ADR_ARM_REZ+2,0xC7);
  outportb(ADR_SERV_NEXT+1,1);
  outportb(ADR_SERV_PRED+1,1);
  T_MIN_PRED=0; T_MIN_NEXT=0;
  T0=0;
	return;
}
//--------------------------------------------------------------
int test_time1(long const_)
{
	int i;
	SEC_time=biostime(0,0l); // прочитать текущее время
	if(labs(SEC_time-FIR_time)>const_) //если прошла очередная секунда
	{
		if(SEC_time<19l)
		{
			rg.h.ah=4;//ЧИТАЕМ ДАТУ ИЗ ТАЙМЕРА РЕАЛЬНОГО ВРЕМЕНИ
			int86(0x1a,&rg,&rg);
			if(rg.x.cflag==0) //если успешное чтение даты
			{
				yearr_=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить год
				monn_ =((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить месяц
				dayy_ =((rg.h.dl&0xf0)>>4)*10+(rg.h.dl&0xf); //вычислить день
				new_day=dayy_;
				rg.h.ah=2;//ЧИТАЕМ ВРЕМЯ ИЗ RTC
				int86(0x1a,&rg,&rg);
				if(rg.x.cflag==0) //если успешное чтение времени
				{
					chas_=((rg.h.ch&0xf0)>>4)*10+(rg.h.ch&0xf); //вычислить часы
					min_=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить минуты
					sec_=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить секунды
					//установка системной даты и системного времени
					rg.h.ah=0x2b;
					rg.x.cx=yearr_+2000;
					rg.h.dh=monn_;
					rg.h.dl=dayy_;
					int86(0x21,&rg,&rg);

					rg.h.ah=0x2d;
					rg.h.ch=chas_;
					rg.h.cl=min_;
					rg.h.dh=sec_;
					int86(0x21,&rg,&rg);
				}
			}
		}
		if(old_day!=new_day)
		{
			if(file_arc>0)close(file_arc);
			file_arc=0;
			for(i=0;i<14;i++)NAME_FILE[i]=0;
			strcpy(NAME_FILE,"RESULT//");
			if(dayy_<10){NAME_FILE[8]=0x30;NAME_FILE[9]=dayy_|0x30;}
			else {NAME_FILE[8]=(dayy_/10)|0x30;NAME_FILE[9]=(dayy_%10)|0x30;}
			strcat(NAME_FILE,".ogo");
			NAME_FILE[14]=0;
			file_arc=open(NAME_FILE,O_CREAT|O_TRUNC|O_APPEND|O_WRONLY,S_IREAD|S_IWRITE|O_BINARY);
			if(file_arc<0)
			{
				clrscr();
				gotoxy(10,10);
				puts("Нарушение файловой структуры,работа невозможна");
				getch();
				FINAL();
			}
			cikl_arm=0;
			T_fix=10915;
			FIR_time=SEC_time;
			old_day=new_day;
			return(1);
		}
		if((SEC_time>=1572462l)&&(SEC_time<=1572480l))//если конец суток
		{
			rg.h.ah=4;//ЧИТАЕМ ДАТУ ИЗ ТАЙМЕРА РЕАЛЬНОГО ВРЕМЕНИ
			int86(0x1a,&rg,&rg);
			if(rg.x.cflag==0) //если успешное чтение даты
			{
				yearr_=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить год 
				monn_ =((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить месяц 
				dayy_ =((rg.h.dl&0xf0)>>4)*10+(rg.h.dl&0xf); //вычислить день
				old_day=dayy_;
				rg.h.ah=2;//ЧИТАЕМ ВРЕМЯ ИЗ RTC
				int86(0x1a,&rg,&rg);
				if(rg.x.cflag==0) //если успешное чтение времени
				{
					chas_=((rg.h.ch&0xf0)>>4)*10+(rg.h.ch&0xf); //вычислить часы
					min_=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить минуты
					sec_=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить секунды 
					//установка системной даты и системного времени 
					rg.h.ah=0x2b;
					rg.x.cx=yearr_+2000;
					rg.h.dh=monn_;
					rg.h.dl=dayy_;
					int86(0x21,&rg,&rg);

					rg.h.ah=0x2d;
					rg.h.ch=chas_;
					rg.h.cl=min_;
					rg.h.dh=sec_;
					int86(0x21,&rg,&rg);
				}
			}
			FIR_time=SEC_time;
			second_time=SEC_time;
			first_time=SEC_time;
		}
		else  //если не конец суток 
		{
			read_t();
			puts1(TIME,0x8C,1,50);
			return(1);
		}
		FIR_time=SEC_time;
	}
	else return(0);//если секунда не прошла
  return(0);
}
//======================================================================
void FINAL(void)
{
#ifdef WORK
	reset_int_vect1();
#endif
	stop_watchdog();
	clrscr();
  close(file_arc);
/*
#ifndef TEST
	close(file_arm_in);
#endif
	close(file_arm_out);
*/
  farfree(PEREDACHA);
  farfree(PRIEM);
	farfree(FR4);
  farfree(ZAFIX_FR4);
  exit(0);
  return;
}
//=======================================================================
/***********************************************************\
*  					процедура чтения текущего времени               *
*								из таймера реального времени                *
\***********************************************************/
void read_t(void)
{
  char h_0,h_1,m_0,m_1,s_0,s_1;
  rg.h.ah=0x2c; int86(0x21,&rg,&rg);
  h_0=rg.h.ch%10;h_1=rg.h.ch/10;
  m_0=rg.h.cl%10;m_1=rg.h.cl/10;
  s_0=rg.h.dh%10;s_1=rg.h.dh/10;
  ho_ur=h_0+h_1*10;
  mi_n=m_0+m_1*10;
  se_c=s_0+s_1*10;
  TIME[0]=0x20;    TIME[1]=h_1|0x30;TIME[2]=h_0|0x30;TIME[3]=0x3a;
  TIME[4]=m_1|0x30;TIME[5]=m_0|0x30;TIME[6]=0x3a;    TIME[7]=s_1|0x30;
  TIME[8]=s_0|0x30;TIME[9]=0x20; TIME[10]=0;
  return;
}
//=============================================================================
/*********************************************************\
*                  TEST_MARSH()                           *
*  проверка выполнения маршрутных команд стойками ТУМС    *
*                                                         *
\*********************************************************/
/*
void TEST_MARSH(void)
{
  int i,ii;
  unsigned char TEST1=0;
  for(i=0;i<Nst;i++)
	{
		if(STOYKI_MARSH[i]!=0)//если стойка в маршруте
		{
			STOYKI_MARSH[i]=MARSH_GOT[i];
			TEST1=TEST1+STOYKI_MARSH[i]; //получить состояние всего маршрута
		}
	}
	if(TEST1==0) //если маршрут закрылся
	{
		if(MARSHRUT==0xAA)MARSHRUT=0xF;
	}
	else
		if((TEST1<15)&&(MARSHRUT!=0))
		{
			MARSHRUT=0;
			for(i=0;i<Nst;i++)
			{
				if(STOYKI_MARSH[i]!=0)//если стойка в маршруте
				{
					STOYKI_MARSH[i]=0;
          MARSH_GOT[i]=0; //получить состояние всего маршрута
        }
      }
      sbros_kom();
      for(ii=0;ii<200;ii++)
      {
        TRASSA[ii].object=0;
        TRASSA[ii].tip=0;
				TRASSA[ii].stoyka=0;
        TRASSA[ii].podgrup=0;
      }
    }
  putch1(TEST1|0x40,0xa,75,50);
  TEST1=0;
  for(i=0;i<Nst;i++)
	{
    if(STOYKI_MARSH1[i]!=0)
    {
      STOYKI_MARSH1[i]=MARSH_GOT[i];
      TEST1=TEST1+STOYKI_MARSH1[i];
    }
  }
  if(TEST1==0)
  {
    if(MARSHRUT1==0xAA)MARSHRUT1=0xF;
  }
  else
    if((TEST1<15)&&(MARSHRUT1!=0))
    {
      MARSHRUT1=0;
      for(i=0;i<Nst;i++)
      {
        if(STOYKI_MARSH1[i]!=0)//если стойка в маршруте
        {
					STOYKI_MARSH1[i]=0;
          MARSH_GOT[i]=0; //получить состояние всего маршрута
        }
      }
      sbros_kom();
      for(ii=0;ii<200;ii++)
      {
        TRASSA1[ii].object=0;
				TRASSA1[ii].tip=0;
				TRASSA1[ii].stoyka=0;
				TRASSA1[ii].podgrup=0;
			}
		}
	putch1(TEST1|0x40,0xa,78,50);
	putch1(MARSHRUT|0x40,0xc,76,50);
	putch1(MARSHRUT1|0x40,0xc,79,50);
	return;
}
*/
//===================================================================
/********************************************\
*   процедура переинициализации сторожевика  *
*                watchdog()                  *
\********************************************/
void watchdog(void)
{
  unsigned char a;

  outportb(0x2e,0x87);
  outportb(0x2e,0x87);//разблокировать таймер

  outportb(0x2e,0x7);
  outportb(0x2f,0x8); //выбрать регистры таймера

  outportb(0x2e,0x30);
  outportb(0x2f,0x1); //разрешить функции таймера

	outportb(0x2e,0xf5);
  a=inportb(0x2f);
  a=a&0xf7;
  outportb(0x2f,a);   //установить секундный диапазон

  outportb(0x2e,0xf6);
  outportb(0x2f,0x2);  //установить 2 секунды на таймер

  outportb(0x2e,0xaa); //заблокировать таймер
  return;
}
//=======================================================================
/*************************************\
*   процедура выключения сторожевика  *
*           stop_watchdog()           *
\*************************************/
void stop_watchdog(void)
{
  unsigned char a;

  outportb(0x2e,0x87);
  outportb(0x2e,0x87);//разблокировать таймер

  outportb(0x2e,0x7);
  outportb(0x2f,0x8); //выбрать регистры таймера

  outportb(0x2e,0x30);
  outportb(0x2f,0x1); //разрешить функции таймера

  outportb(0x2e,0xf5);
  a=inportb(0x2f);
  a=a&0xf7;
  outportb(0x2f,a);   //установить секундный диапазон

	outportb(0x2e,0xf6);
	outportb(0x2f,0);  //выключить таймер

	outportb(0x2e,0xaa); //заблокировать таймер
	return;
}
//========================================================================
/******************************************\
*  Программа установки системного таймера  *
*            MAKE_TIME()                   *
\******************************************/
void MAKE_TIME(int AR,int ST)
{
	unsigned long t_last,t_new;
	unsigned int aa,aa2,aa1,aa0;
	rg.h.ah=4;//ЧИТАЕМ ДАТУ ИЗ ТАЙМЕРА РЕАЛЬНОГО ВРЕМЕНИ
	int86(0x1a,&rg,&rg);
	if(rg.x.cflag==0) //если успешное чтение даты
	{
		aa=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить год
		if(aa!=KOM_TIME[5])return;

		aa=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить месяц
		if(aa!=KOM_TIME[4])return;

		aa=((rg.h.dl&0xf0)>>4)*10+(rg.h.dl&0xf); //вычислить день
		if(aa!=KOM_TIME[3])return;
	}
	else return;

	rg.h.ah=2;//ЧИТАЕМ ВРЕМЯ ИЗ RTC
	int86(0x1a,&rg,&rg);
	if(rg.x.cflag==0) //если успешное чтение времени
	{
		aa2=((rg.h.ch&0xf0)>>4)*10+(rg.h.ch&0xf); //вычислить часы
		aa1=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf); //вычислить минуты
		aa0=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf); //вычислить секунды
		t_last=aa2*3600+aa1*60+aa0;
	}
	else return;
	//установка системного таймера
	t_new=(KOM_TIME[2]*3600l)+(KOM_TIME[1]*60l)+KOM_TIME[0]*1l;

	if(KOM_BUFER[3]==101)//если автоустановка
	{
		if(labs(t_new-t_last)>10l)return;
	}
	else
	{
		if((biostime(0,0l)>1501500l)||(biostime(0,0l)<70980l))
		{
			KVIT_ARMu[AR][ST][2]=0x6;
			return;
		}
		if(labs(t_new-t_last)>3840l)
		{
			KVIT_ARMu[AR][ST][2]=0x6;
			return;
		}
	}
	rg.h.ah=0x2d;
	rg.h.ch=KOM_TIME[2];
	rg.h.cl=KOM_TIME[1];
	rg.h.dh=KOM_TIME[0];
	rg.h.dl=0;
	int86(0x21,&rg,&rg);
	//установка таймера реального времени
	rg.h.ah=3;
	rg.h.ch=((KOM_TIME[2]/10)<<4)|(KOM_TIME[2]%10);//установка часов
	rg.h.cl=((KOM_TIME[1]/10)<<4)|(KOM_TIME[1]%10);//установка минут
	rg.h.dh=((KOM_TIME[0]/10)<<4)|(KOM_TIME[0]%10);//установка секунд
	int86(0x1a,&rg,&rg);
	SET_TIME=0xff;
  add(0,300,0);
	return;
}
//=============================================================
void write_FR3(int nomer)
{
	int ff;
 	for(ff=0;ff<32;ff++)FR3_ALL[nomer][ff]=FR3[ff];
}
//==============================================================
void read_FR3(int nomer)
{
  int ff;
	for(ff=0;ff<32;ff++)FR3[ff]=FR3_ALL[nomer][ff];
}
