/*************************************************************************\
*                                                                         *
*  KANAL.C - файл канального обмена сервера РПЦ ВНИИАС                    *
*                                                                         *
\*************************************************************************/

#include "opred2.h"
#include "extern2.h"
/********************************************\$$$$Работает по прерыванию канала
*    Процедура выдачи данных в стойки ТУМС   *
*           vidacha1(unsigned int ad)        *
*     ad - адрем порта выдачи данных         *
\********************************************/
void vidacha1(unsigned int ad)
{

	if(ad==ADR_TUMS_OSN)
	{
		outportb(ad,BUF_OUT[UKAZ_OUT]);

		if((REGIM==ANALIZ)&&(STOP_AN==0))
		{
			putch1(BUF_OUT[UKAZ_OUT],0xa,X_ANALIZ_OUT++,Y_ANALIZ_OUT);
			if(X_ANALIZ_OUT>=80){X_ANALIZ_OUT=1;Y_ANALIZ_OUT=Y_ANALIZ_OUT+2;}
			if(Y_ANALIZ_OUT>=44){X_ANALIZ_OUT=1;Y_ANALIZ_OUT=5;}
		}
			BUF_OUT[UKAZ_OUT++]=0;
		if(UKAZ_OUT>=SIZE_BUF)UKAZ_OUT=0;
		if(BUF_OUT[UKAZ_OUT]==0)
		outportb(ad+1,1);
	}
	if(ad==ADR_TUMS_REZ)
	{
		outportb(ad,BUF_OUT1[UKAZ_OUT1]);
		if((REGIM==ANALIZ)&&(STOP_AN==0))
		{
			putch1(BUF_OUT1[UKAZ_OUT1],0xe,X1_ANALIZ_OUT++,Y1_ANALIZ_OUT);
			if(X1_ANALIZ_OUT>=80){X1_ANALIZ_OUT=1;Y1_ANALIZ_OUT=Y1_ANALIZ_OUT+2;}
			if(Y1_ANALIZ_OUT>=45){X1_ANALIZ_OUT=1;Y1_ANALIZ_OUT=6;}
		}
		BUF_OUT1[UKAZ_OUT1++]=0;
		if(UKAZ_OUT1>=SIZE_BUF)UKAZ_OUT1=0;
		if(BUF_OUT1[UKAZ_OUT1]==0)outportb(ad+1,1);

	}
	return;
}
/*****************************************\$$$$Работает по прерыванию канала
* процедура выдачи данных в следующий     *
* сервер   out_next()                     *
\*****************************************/
void out_next(void)
{
	int i,beg,kon;
	beg=cikl_out_next*14;
	kon=(cikl_out_next+1)*14;
	for(i=beg;i<kon;i++)
	{
		outportb(ADR_SERV_NEXT,BUF_OUT_NEXT[i]);
		BUF_OUT_NEXT[i]=0;
	}
  cikl_out_next++;
  if(cikl_out_next>=7)
  {
    cikl_out_next=0;
    outportb(ADR_SERV_NEXT+1,1);
	}
  return;
}

/******************************************\$$$$Работает по прерыванию канала
* процедура выдачи данных в предыдущий     *
* сервер   out_pred()                      *
\******************************************/
void out_pred(void)
{
	int i,beg,kon;
	beg=cikl_out_pred*14;
	kon=(cikl_out_pred+1)*14;
	for(i=beg;i<kon;i++)
	{
		outportb(ADR_SERV_PRED,BUF_OUT_PRED[i]);
		BUF_OUT_PRED[i]=0;
	}
	cikl_out_pred++;
	if(cikl_out_pred>=7)
	{
		cikl_out_pred=0;
		outportb(ADR_SERV_PRED+1,1);

	}
	return;
}
/****************************************\$$$$Работает по прерыванию канала
*  Процедура выдачи данных по основному  *
*	 каналу АРМ   out_arm_osn()            *
\****************************************/
void out_arm_osn(void)
{
	int i,beg,kon;
	if(cikl_out_arm>=5)
	{
		outportb(ADR_ARM_OSN+2,0xC7);
		cikl_out_arm=0;
		VYVOD_ARM=0;
		outportb(ADR_ARM_OSN+1,1);
		return;
	}
	beg=cikl_out_arm*14;
	kon=(cikl_out_arm+1)*14;
	for(i=beg;i<kon;i++)outportb(ADR_ARM_OSN,BUF_OUT_ARM[i]);
	cikl_out_arm++;
	return;
}
/****************************************\$$$$Работает по прерыванию канала
*  Процедура выдачи данных по резервному *
*  каналу АРМ out_arm_rez()              *
\****************************************/
void out_arm_rez(void)
{
	int i,beg1,kon1;
	if(cikl_out_arm1>=5)
	{
		outportb(ADR_ARM_REZ+2,0xC7);
		cikl_out_arm1=0;
		VYVOD_ARM1=0;
		outportb(ADR_ARM_REZ+1,1);
		return;
	}
	beg1=cikl_out_arm1*14;
	kon1=(cikl_out_arm1+1)*14;
	for(i=beg1;i<kon1;i++)outportb(ADR_ARM_REZ,BUF_OUT_ARM1[i]);
	cikl_out_arm1++;
	return;
}
/***************************************\$$$$Работает по прерыванию канала
*  Процедура выдачи данных по основному *
* каналу АРМ ШН *                       *
*               out_arm_rez()           *
\***************************************/
void out_shn_osn(void)
{
	int i,beg1,kon1;
	if(cikl_out_shn>=5)
	{
		outportb(ADR_SHN_OSN+2,0xC7);
		cikl_out_shn=0;
		outportb(ADR_SHN_OSN+1,1);
		return;
	}
	beg1=cikl_out_shn*14;
	kon1=(cikl_out_shn+1)*14;
	for(i=beg1;i<kon1;i++)outportb(ADR_SHN_OSN,BUF_OUT_SHN[i]);
	cikl_out_shn++;
	return;
}
/*******************************\$$$$Работает по прерыванию канала
* Процедура приема данных от    *
* следующего сервера  in_next() *
\*******************************/
void in_next(void)
{
	int i,begin,konec;
	cikl_in_next++;
	if(cikl_in_next>7)cikl_in_next=1;
	begin=(cikl_in_next-1)*14;
	konec=cikl_in_next*14;
	for(i=begin;i<konec;i++)BUF_IN_NEXT[i]=inportb(ADR_SERV_NEXT);
	if((BUF_IN_NEXT[0]!=0x2a)&&(BUF_IN_NEXT[0]!=0x3C))cikl_in_next=0;
	if(cikl_in_next==7)
	{
		cikl_in_next=0;
		SCREEN_NEXT=0xF;
		sosed_NEXT=sosed_NEXT+4;
		if(sosed_NEXT>8)sosed_NEXT=8;
		outportb(ADR_SERV_NEXT+2,0xC7);
	}
	return;
}
/*******************************\$$$$Работает по прерыванию канала
* Процедура приема данных от    *
* предыдущего сервера in_pred() *
\*******************************/
void in_pred(void)
{
	int i,beg,kon;
	cikl_in_pred++;
	if(cikl_in_pred>7)cikl_in_pred=1;
	beg=(cikl_in_pred-1)*14;
	kon=cikl_in_pred*14;
	for(i=beg;i<kon;i++)BUF_IN_PRED[i]=inportb(ADR_SERV_PRED);
	if((BUF_IN_PRED[0]!=0x12)&&(BUF_IN_PRED[0]!=0x3c))cikl_in_pred=0;
	if(cikl_in_pred==7)
	{
		cikl_in_pred=0;
		SCREEN_PRED=0xF;
		sosed_PRED=sosed_PRED+4;
		if(sosed_PRED>8)sosed_PRED=8;
		outportb(ADR_SERV_PRED+2,0xC7);
	}
	return;
}
/**********************************\ $$$$Работает по прерыванию канала
* Процедура приема данных от АРМ   *
* по основному каналу in_arm_osn() *
\**********************************/
void in_arm_osn(void)
{
	int i,konec,begin;
//  if(cikl_in_arm==0)END_ARM=0;
	cikl_in_arm++;
	if(cikl_in_arm>7)cikl_in_arm=0;
	begin=(cikl_in_arm-1)*14;
	konec=cikl_in_arm*14;
	for(i=begin;i<konec;i++)BUF_IN_ARM[i]=inportb(ADR_ARM_OSN);
	if(BUF_IN_ARM[0]!=0xAA)cikl_in_arm=0;
	if(BUF_IN_ARM[(cikl_in_arm-1)*14+13]==0x55)
	{
		END_ARM=(cikl_in_arm-1)*14+13;
		inf_ARM=inf_ARM+(Narm<<1);
		if(inf_ARM>(Narm<<1))inf_ARM=Narm<<1;
		cikl_in_arm=0;
		VVOD_ARM=0;
		outportb(ADR_ARM_OSN+2,0xC7);
	}

	return;
}
/***********************************\ $$$$Работает по прерыванию канала
* Процедура приема данных от АРМ    *
* по резервному каналу in_arm_rez() *
\***********************************/
void in_arm_rez(void)
{
	int i,beg,kon;
//  if(cikl_in_arm1==0)END_ARM1=0;
	cikl_in_arm1++;
	if(cikl_in_arm1>7)cikl_in_arm1=0;
	beg=(cikl_in_arm1-1)*14;
	kon=cikl_in_arm1*14;
	for(i=beg;i<kon;i++)BUF_IN_ARM1[i]=inportb(ADR_ARM_REZ);
	if(BUF_IN_ARM1[0]!=0xAA)cikl_in_arm1=0;
	if(BUF_IN_ARM1[(cikl_in_arm1-1)*14+13]==0x55)
	{
		END_ARM1=(cikl_in_arm1-1)*14+13;
		inf_ARM=inf_ARM+(Narm<<1);
		if(inf_ARM>(Narm<<1))inf_ARM=Narm<<1;
		cikl_in_arm1=0;
		VVOD_ARM1=0;
	}
	outportb(ADR_ARM_REZ+2,0xC7);
	return;
}
/***********************************\ $$$$Работает по прерыванию канала
* Процедура приема данных от АРМ ШН *
* по основному каналу in_arm_osn()  *
\***********************************/
void in_shn_osn(void)
{
	int i,konec,begin;
//  if(cikl_in_arm==0)END_ARM=0;
	cikl_in_shn++;
	if(cikl_in_shn>7)cikl_in_shn=0;
	begin=(cikl_in_shn-1)*14;
	konec=cikl_in_shn*14;
	for(i=begin;i<konec;i++)BUF_IN_SHN[i]=inportb(ADR_SHN_OSN);
	if(BUF_IN_SHN[0]!=0xAA)cikl_in_shn=0;
	if(BUF_IN_SHN[(cikl_in_shn-1)*14+13]==0x55)
	{
		END_SHN=(cikl_in_shn-1)*14+13;
		cikl_in_shn=0;
		outportb(ADR_SHN_OSN+2,0xC7);
	}

	return;
}
/***********************************\
* Процедура приема данных от ТУМС   *
* по основному каналу in_tums_osn() *
\***********************************/
void in_tums_osn(void)
{
	symbol=inportb(ADR_TUMS_OSN);
	if((REGIM==ANALIZ)&&(STOP_AN==0))
	{
		putch1(symbol,0xb,X_ANALIZ_OUT++,Y_ANALIZ_OUT);
		if(X_ANALIZ_OUT>=80){X_ANALIZ_OUT=1;Y_ANALIZ_OUT=Y_ANALIZ_OUT+2;}
		if(Y_ANALIZ_OUT>=44){X_ANALIZ_OUT=1;Y_ANALIZ_OUT=5;}
	}
	if(symbol==0)return;
	if(symbol=='(')n_tums=0;
	BUF_IN[n_tums++]=symbol;//в буфер
	if(n_tums>=SIZE_BUF)n_tums=0;
	symbol=0;
	if(n_tums==12)
	{ END_TUMS=0xF;
		inf_TUMS=inf_TUMS+Nst;
		if(inf_TUMS>Nst)inf_TUMS=Nst;
	}
	return;
}

/************************************\
* Процедура приема данных от ТУМС    *
* по резервному каналу in_tums_rez() *
\************************************/
void in_tums_rez(void)
{
		symbol1=inportb(ADR_TUMS_REZ);

		if((REGIM==ANALIZ)&&(STOP_AN==0))
		{
		 putch1(symbol1,0xc,X1_ANALIZ_OUT++,Y1_ANALIZ_OUT);
		if(X1_ANALIZ_OUT>=80){X1_ANALIZ_OUT=1;Y1_ANALIZ_OUT=Y1_ANALIZ_OUT+2;}
		if(Y1_ANALIZ_OUT>=45){X1_ANALIZ_OUT=1;Y1_ANALIZ_OUT=6;}
	}

		if(symbol1==0)return;
		if(symbol1=='(')n_tums1=0;
		BUF_IN1[n_tums1++]=symbol1;//в буфер
		if(n_tums1>=SIZE_BUF)n_tums1=0;
		symbol1=0;
		if(n_tums1==12)
		{
			END_TUMS1=0xF;
			inf_TUMS=inf_TUMS+Nst;
			if(inf_TUMS>Nst)inf_TUMS=Nst;
		}
	return;
}

/*********************************************\
*                                             *
*        OUT_PAKET_PRED_NEXT()                *
* процедура выдачи данных в соседние серверы  *
*                                             *
\*********************************************/
void OUT_PAKET_PRED_NEXT(void)
{ unsigned int i,CRC16,ob_fr4;
	//работа с копиями пакетов в предыдущий сервер
	//запись пакета основного канала
	for(i=0;i<21;i++)
	{
		//младший байт
		BUF_OUT_PRED[2*i+1]=OBJ_ARMu[N_PAKET].OBJ[i]&0xff;
		//старший байт
		BUF_OUT_PRED[2*i+2]=(OBJ_ARMu[N_PAKET].OBJ[i]&0xff00)>>8;
	}
	BUF_OUT_PRED[43]=PAKETs[N_PAKET].KS_OSN&0xff;//младший
	BUF_OUT_PRED[44]=(PAKETs[N_PAKET].KS_OSN&0xff00)>>8;//старший
	//запись пакета резервного канала
	for(i=0;i<21;i++)
	{
		//младший байт
		BUF_OUT_PRED[2*i+45]=OBJ_ARMu1[N_PAKET].OBJ[i]&0xff;
		//старший байт
		BUF_OUT_PRED[2*i+46]=(OBJ_ARMu1[N_PAKET].OBJ[i]&0xff00)>>8;
	}
	BUF_OUT_PRED[87]=PAKETs[N_PAKET].KS_REZ&0xff;//младший
	BUF_OUT_PRED[88]=(PAKETs[N_PAKET].KS_REZ&0xff00)>>8;//старший
	//работа с объектами по FR4
	for(i=0;i<10;i++)
	{
		if(NOVIZNA_FR4[i]!=0)
		{
			BUF_OUT_PRED[91]=NOVIZNA_FR4[i]&0xff;      //записать номер объекта
			BUF_OUT_PRED[92]=(NOVIZNA_FR4[i]&0xfff)>>8;
			BUF_OUT_PRED[93]=FR4[NOVIZNA_FR4[i]&0xfff];//записать ограничения
			ob_fr4=NOVIZNA_FR4[i]&0xfff;//заполнить объект FR4
			add(FR4[ob_fr4],200,ob_fr4);//записать в архив
			// навешивание флагов
			// 0x2000 - первая передача соседу
			// 0x4000 - вторая передача соседу
			if((NOVIZNA_FR4[i]&0x2000)==0)NOVIZNA_FR4[i]=NOVIZNA_FR4[i]|0x2000;
			else
				if((NOVIZNA_FR4[i]&0x4000)==0)NOVIZNA_FR4[i]=NOVIZNA_FR4[i]|0x4000;
			//если передано 2 раза соседям и в АРМы, то снять новизну
			if((NOVIZNA_FR4[i]&0x7000)==0x7000)NOVIZNA_FR4[i]=0;
			break;
		}
	}
	if(i==10)				//если не было новизны по FR4
	{
		for(i=LAST_FR4;i<KOL_VO;i++)//поиск очередного установленого ограничения
		{
			if(FR4[i]!=0) //если имеется ограничение
			{
				BUF_OUT_PRED[91]=i&0xff;       // записать номер младшую часть
				BUF_OUT_PRED[92]=(i&0xfff)>>8; // записать номер старшую часть
				BUF_OUT_PRED[93]=FR4[i];       // записать само ограничение

				if(fixir!=0)                  // если наступило время фиксации
				{
					if(ZAFIX_FR4[i]==0)   //если ограничение не фиксировалось
					{
						add(FR4[i],200,i);  //записать ограничение в архив
						ZAFIX_FR4[i]=0xf;   //установить признак фиксации записи
					}
				}
				break;
			}
		}
		LAST_FR4=i+1;
		if((LAST_FR4>=KOL_VO)||(LAST_FR4<=0))LAST_FR4=0;
	}
	//работа с копиями пакетов в следующий сервер
	for(i=0;i<98;i++)BUF_OUT_NEXT[i]=BUF_OUT_PRED[i];

	//если прошло более 65 секунд, то сбросить все команды принудительно 
	if(FLAG_KOM==0)//если нет команд
	{
		if(T_MIN_NEXT>1092)//если прошла 1 минута
		{
			if(sosed_NEXT>0)//если следующий сосед "живой" - передать управление
			{ putch1(0x1a,0xa,34,50); //вывод на экран стрелочки вправо "nxt"
				putch1(0x20,0xa,29,50);
				puts1("    ",0xa,51,50);
				puts1("    ",0xa,56,50);
				ACTIV=0;
				switch(SERVER)
				{
					case 1: ACTIV_SERV=2; break;
					case 2: ACTIV_SERV=3; break;
					case 3: ACTIV_SERV=1; break;
				}
				BUF_OUT_NEXT[89]=ACTIV_SERV;  BUF_OUT_NEXT[90]=ACTIV_SERV;
				BUF_OUT_PRED[89]=ACTIV_SERV;  BUF_OUT_PRED[90]=ACTIV_SERV;
				STOP_SERVER=4;
				PEREDAL_UPR_K_NEXT=0xF;   PEREDAL_UPR_K_PRED=0;
				POLUCHIL_UPR_OT_NEXT=0;   POLUCHIL_UPR_OT_PRED=0;

				for(i=0;i<100;i++)
				{
					BUF_OUT_PRED[i]=0;
					BUF_IN_PRED[i]=0;
				}
//        puts1("a6",0xa,63,50);
				if(REGIM==KANAL)tablica();
        T0=0;
        T_MIN_PRED=0;
        T_MIN_NEXT=0;
      }
      else  T_MIN_NEXT=0;
    }
    else
    if((T_MIN_PRED>1092))//если прошла 1 минута
    {
      if(sosed_PRED>0)//если предыдущий сосед "живой" - передать управление
      { putch1(0x1b,0xc,29,50);//вывод на экран стрелочки влево "prd"
        putch1(0x20,0xc,34,50);
        puts1("    ",0xa,51,50);
        puts1("    ",0xa,56,50);
        ACTIV=0;
        switch(SERVER)
        {
          case 1: ACTIV_SERV=3; break;
          case 2: ACTIV_SERV=1; break;
          case 3: ACTIV_SERV=2; break;
        }
				BUF_OUT_PRED[89]=ACTIV_SERV;    BUF_OUT_PRED[90]=ACTIV_SERV;
				BUF_OUT_NEXT[89]=ACTIV_SERV;    BUF_OUT_NEXT[90]=ACTIV_SERV;

				for(i=0;i<100;i++)
				{
					BUF_OUT_NEXT[i]=0;
					BUF_IN_NEXT[i]=0;
				}

				PEREDAL_UPR_K_PRED=0xF;   PEREDAL_UPR_K_NEXT=0;
				POLUCHIL_UPR_OT_PRED=0;   POLUCHIL_UPR_OT_NEXT=0;
				STOP_SERVER=4;
				if(REGIM==KANAL)tablica();
				T0=0;
				T_MIN_NEXT=0;
        T_MIN_PRED=0;

      }
      else T_MIN_PRED=0;
    }
    else
    {
			BUF_OUT_PRED[89]=SERVER;    BUF_OUT_PRED[90]=SERVER;
			BUF_OUT_NEXT[89]=SERVER;    BUF_OUT_NEXT[90]=SERVER;
    }
  }
  else
  {
		BUF_OUT_PRED[89]=SERVER;    BUF_OUT_PRED[90]=SERVER;
		BUF_OUT_NEXT[89]=SERVER;    BUF_OUT_NEXT[90]=SERVER;
  }
  CRC16=CalculateCRC16(&BUF_OUT_NEXT[1],94);
  BUF_OUT_NEXT[95]=CRC16&0xFF;
  BUF_OUT_NEXT[96]=(CRC16&0xFF00)>>8;

  CRC16=CalculateCRC16(&BUF_OUT_PRED[1],94);
  BUF_OUT_PRED[95]=CRC16&0xFF;
  BUF_OUT_PRED[96]=(CRC16&0xFF00)>>8;

  BUF_OUT_PRED[0]='*'; //код начала
	BUF_OUT_NEXT[0]=0x12;
	BUF_OUT_PRED[97]=0xd;
	BUF_OUT_NEXT[97]=0xd;
	return;
}
/*********************************************************\
*                                                         *
* ARM_OUT() подготовка данных для передачи в текущий АРМ  *
*                                                         *
\*********************************************************/
void ARM_OUT(void)
{
	int i,j,k,n_out,n_arm,n_arm1,ARM,ARM1,stoyka,bait,ii;
	unsigned int CRC; //контрольная сумма CRC-16
	unsigned char
	OUT_NOM[2], //номер объекта вывода (объекта АРМ ДСП)
	OUT_BYTE,   //байт состояния объекта АРМ ДСП
	ZAGOL,      //байт заголовка для сообщения в АРМ ДСП
	ZAGOL_SHN,  //байт заголовка для сообщения в АРМ ШН
	SOST;       //байт состояния объекта АРМ ДСП
	if(cikl_arm>=2) //если пришло время передачи в арм
	{
		for(i=0;i<100;i++)//Очистить буфера ввода-вывода
		{
			BUF_IN_ARM[i]=0;BUF_IN_ARM1[i]=0;BUF_OUT_ARM[i]=0;BUF_OUT_ARM1[i]=0;
		}
		//формирование адресности запросов
		ZAPROS_ARM++;  if(ZAPROS_ARM>8)ZAPROS_ARM=4;
		ZAPROS_ARM1++; if(ZAPROS_ARM1>8)ZAPROS_ARM1=4;

		//получить индексы основного и резервного АРМов
		ARM=ZAPROS_ARM-4;  ARM1=ZAPROS_ARM1-4;

		// заполнить квитанции на команды
		// и вернуть указатель на своб.байт в буфере
		n_arm=ZAPOLNI_KVIT(ARM,0);
		n_arm1=ZAPOLNI_KVIT(ARM1,1);

		n_out=n_arm;


		OBJ_ARMu[N_PAKET].LAST=0;
		if((DIAGNOZ[1]!=0)||(DIAGNOZ[2]!=0))
		{
			BUF_OUT_ARM[n_out++]=DIAGNOZ[0];DIAGNOZ[0]=0;
			BUF_OUT_ARM[n_out++]=DIAGNOZ[1];DIAGNOZ[1]=0;
			BUF_OUT_ARM[n_out++]=DIAGNOZ[2];
			DIAGNOZ[2]=0; //записать в буфер диагностику
		}
		if(ERR_PLAT[1]!=0)
		{
			BUF_OUT_ARM[n_out++]=ERR_PLAT[0];ERR_PLAT[0]=0;
			BUF_OUT_ARM[n_out++]=ERR_PLAT[1];ERR_PLAT[1]=0;
			BUF_OUT_ARM[n_out++]=ERR_PLAT[2];ERR_PLAT[2]=0;
			BUF_OUT_ARM[n_out++]=ERR_PLAT[3];ERR_PLAT[3]=0;
			BUF_OUT_ARM[n_out++]=ERR_PLAT[4];ERR_PLAT[4]=0;
			BUF_OUT_ARM[n_out++]=ERR_PLAT[5];ERR_PLAT[5]=0;
		}
		if(SET_TIME!=0)
		{
			BUF_OUT_ARM[n_out++]=0;
			BUF_OUT_ARM[n_out++]=0x18;
			rg.h.ah=2;
			int86(0x1a,&rg,&rg);
			if(rg.x.cflag==0) //если успешное чтение часов 
			{
				BUF_OUT_ARM[n_out++]=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf);
				BUF_OUT_ARM[n_out++]=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf);
				BUF_OUT_ARM[n_out++]=((rg.h.ch&0xf0)>>4)*10+(rg.h.ch&0xf);
			}
			else
			{
				BUF_OUT_ARM[n_out++]=0;
				BUF_OUT_ARM[n_out++]=0;
				BUF_OUT_ARM[n_out++]=0;
			}

			rg.h.ah=4;
			int86(0x1a,&rg,&rg);
			if(rg.x.cflag==0) //если успешное чтение даты
			{

				BUF_OUT_ARM[n_out++]=((rg.h.dl&0xf0)>>4)*10+(rg.h.dl&0xf);
				BUF_OUT_ARM[n_out++]=((rg.h.dh&0xf0)>>4)*10+(rg.h.dh&0xf);
				BUF_OUT_ARM[n_out++]=((rg.h.cl&0xf0)>>4)*10+(rg.h.cl&0xf);
			}
			else
			{

				BUF_OUT_ARM[n_out++]=0;
				BUF_OUT_ARM[n_out++]=0;
				BUF_OUT_ARM[n_out++]=0;
			}

			BUF_OUT_ARM[n_out++]=0;
			BUF_OUT_ARM[n_out++]=0;
			SET_TIME=0;
		}
		for(i=0;i<MAX_NEW;i++)//пройти по перечню имеющейся новизны 
		{
			if(NOVIZNA[i]==0)continue;//если нет новизны-перейти к след.ячейке

			//если объект за пределами базы
			if((NOVIZNA[i]>=KOL_VO)||(NOVIZNA[i]<=0)){NOVIZNA[i]=0;continue;}

			//если новизна есть-прочитать текущее состояние объекта 
			read_FR3(NOVIZNA[i]);
			//прочитать номер объекта для АРМов 
		  OUT_NOM[0]=(out_ob[NOVIZNA[i]]&0xFF00)>>8;
      OUT_NOM[1]=out_ob[NOVIZNA[i]]&0xff;
			 //если объект не для АРМ ничего не делать и сбросить новизну
			if((OUT_NOM[1]==32)&&(OUT_NOM[0]==32)){NOVIZNA[i]=0;continue;}
			if((OUT_NOM[1]==0)&&(OUT_NOM[0]==0)){NOVIZNA[i]=0;continue;}

			OUT_BYTE=0; //байт состояния объекта
			//заполнить байт состояния текущим состоянием объекта 
			for(j=0;j<8;j++){ k=1<<j;if(FR3[2*j+1]==1)OUT_BYTE=OUT_BYTE|k;}
//			news=0xF;
			//записать в буфер вывода АРМ номер для данного объекта 
			BUF_OUT_ARM[n_out++]=OUT_NOM[1];
			BUF_OUT_ARM[n_out++]=OUT_NOM[0];
			BUF_OUT_ARM[n_out++]=OUT_BYTE;  //записать в буфер состояние объекта 
			FR3[27]=FR3[27]|0x1f;           //установить метку передачи в АРМы
			FR3[26]=FR3[26]|(N_PAKET&0x1F); //запомнить номер пакета передачи
			write_FR3(NOVIZNA[i]);

			j=OBJ_ARMu[N_PAKET].LAST++;
			//записать передаваемый объект в структуру пакета
			OBJ_ARMu[N_PAKET].OBJ[j]=NOVIZNA[i];
			NOVIZNA[i]=0;//удалить объект из новизны 
			if(n_out>64)break;
		}

		if(n_out<65)//если в буфере передачи осталось место 
		//пройти по перечню имеющейся новизны в ограничениях
		for(i=0;i<10;i++)
		{
			if(NOVIZNA_FR4[i]==0)continue;//если нет новизны-перейти к след.ячейке
			if((NOVIZNA_FR4[i]&0xfff)>=KOL_VO)
			{
				NOVIZNA_FR4[i]=0;
				continue;//если нет объекта 
			}
			OUT_NOM[0]=(out_ob[NOVIZNA_FR4[i]&0xFFF]&0xff00)>>8;//прочитать номер для АРМов
			OUT_NOM[1]=out_ob[NOVIZNA_FR4[i]&0xFFF]&0xff;
			if((OUT_NOM[1]==32)&&(OUT_NOM[0]==32))
			{
				NOVIZNA_FR4[i]=0;
				continue;//если объект не для АРМ,
			}
			if((OUT_NOM[1]==0)&&(OUT_NOM[0]==0))
			{
				NOVIZNA_FR4[i]=0;
				continue;//то перейти к след.ячейке
			}
//			news=0xf;
			OUT_BYTE=FR4[NOVIZNA_FR4[i]&0xfff];
			BUF_OUT_ARM[n_out++]=OUT_NOM[1];//записать в буфер вывода АРМ номер для
			BUF_OUT_ARM[n_out++]=OUT_NOM[0]|0x80;//данного объекта 
			BUF_OUT_ARM[n_out++]=OUT_BYTE;  //записать в буфер состояние объекта

			NOVIZNA_FR4[i]=NOVIZNA_FR4[i]+0x1000; //установить флаг передачи в АРМ
			if((NOVIZNA_FR4[i]&0x3000)==0x3000)   //если передано везде,то
			NOVIZNA_FR4[i]=0;											//удалить объект из новизны
			if(n_out>64)break;
		}
		if(i==10)new_fr4=0;

a1:
		//далее до конца заполняется основной канал
		if(n_out<65)
		{
			for(i=povtor_out;i<KOL_VO;i++)
			{
				//прочитать состояние объекта
				read_FR3(i);
				stoyka=(FR3[28]&0xF0)>>4;//выделить стойку
				if(stoyka==0)continue;
				bait=FR3[28]&0xf;        //выделить байт
				if(SVAZ_TUMS[stoyka-1]>=80)//если потеряна связь с ТУМС
				{

					if(FR3[29]>=48)
					{
						FR3[11]=1; //навязать непарафазность
						VVOD[stoyka-1][FR3[29]-48][bait-1]=VVOD[stoyka-1][FR3[29]-48][bait-1]|0x20;
						READ_BD(i);
						if(bd_osn[0]==1)//если стрелка
						{
							if(bd_osn[12]!=9999)//если спаренная
							{
								ii=bd_osn[8]&0xfff;
								write_FR3(i);
								read_FR3(ii);
								FR3[11]=1;
								write_FR3(ii);
								read_FR3(i);
							}
						}
					}
				}
				//прочитать номер объекта АРМов
				OUT_NOM[0]=(out_ob[i]&0xFF00)>>8;
				OUT_NOM[1]=out_ob[i]&0xff;
				//если объект не для АРМа, то ничего не делать
				if((OUT_NOM[1]==32)&&(OUT_NOM[0]==32))continue;
				if((OUT_NOM[1]==0)&&(OUT_NOM[0]==0))continue;

				OUT_BYTE=0;
				for(j=0;j<8;j++){k=1<<j;if(FR3[2*j+1]==1)OUT_BYTE=OUT_BYTE|k;}

				BUF_OUT_ARM[n_out++]=OUT_NOM[1];
				BUF_OUT_ARM[n_out++]=OUT_NOM[0];
				BUF_OUT_ARM[n_out++]=OUT_BYTE;
				if(FR4[i]!=0)
				{
					OUT_BYTE=FR4[i];
					BUF_OUT_ARM[n_out++]=OUT_NOM[1];//записать в буфер вывода АРМ номер для
					BUF_OUT_ARM[n_out++]=OUT_NOM[0]|0x80;//данного объекта
					BUF_OUT_ARM[n_out++]=OUT_BYTE;  //записать в буфер состояние объекта
				}

				FR3[27]=FR3[27]|0x1f;//установить метку передачи в АРМы
				FR3[26]=FR3[26]|(N_PAKET&0x1F);//запомнить номер пакета передачи

				write_FR3(i);

				j=OBJ_ARMu[N_PAKET].LAST++;
				OBJ_ARMu[N_PAKET].OBJ[j]=i;//передаваемый объект
				if(n_out>64)break;
			}
			povtor_out=i;
			if((povtor_out>=KOL_VO)||(povtor_out<=0))povtor_out=1;
			goto a1;
		}
		//формирование заголовка
		ZAGOL=0;
		ZAGOL=ZAGOL|(ZAPROS_ARM&0xf);
		ZAGOL=ZAGOL|((SERVER<<4)&0xf0);
		SOST=0x10;
		SOST=SOST|(1<<(SERVER-1));//установить признак работающего сервера
		j=0;
		if(KONFIG_ARM[ZAPROS_ARM-4][0]==0xFF)j=1;//АРМ основной в 1-ом районе
		else
			if(KONFIG_ARM[ZAPROS_ARM-4][1]==0xFF)j=3;//АРМ основной во 2-ом районе
			else
				if(KONFIG_ARM[ZAPROS_ARM-4][0]==1)j=2;//АРМ резервный в 1-ом районе
				else
					if(KONFIG_ARM[ZAPROS_ARM-4][1]==2)j=4;//АРМ резервный во 2-ом районе

		if(j>0)
		{
			switch(j)
			{ //АРМ в первом районе

				case 1: SOST=SOST|0x80;  //арм основной
								break;

				 //АРМ во втором районе
				case 3: SOST=SOST|0x80;break; //арм основной

				default:SOST=SOST&0x7f;break; //арм резервный 
			}
		}
		//проверяем наличие 2-ух АРМов в заданном районе
		if(KONFIG_ARM[ZAPROS_ARM-4][0]!=0)//если данный АРМ в 1-ом районе
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][0]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		if(KONFIG_ARM[ZAPROS_ARM-4][1]!=0)//если данный АРМ вo 2-ом районе
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][1]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		switch(SERVER)
		{
			case 1: if(sosed_PRED!=0)SOST=SOST|4;
							if(sosed_NEXT!=0)SOST=SOST|2;
							break;
			case 2: if(sosed_PRED!=0)SOST=SOST|1;
							if(sosed_NEXT!=0)SOST=SOST|4;
							break;
			case 3: if(sosed_PRED!=0)SOST=SOST|2;
							if(sosed_NEXT!=0)SOST=SOST|1;
							break;
			default: SOST=0; break;
		}
		if(PROCESS==0)SOST=SOST&0xef;
		if(KNOPKA_OK[ARM]==1)SOST=SOST|0x20; //добавить нажатие кнопки
		SOST=SOST|PROCESS; //добавить состояние режима управления УВК или ПУЛЬТ 

		BUF_OUT_ARM[2]=SOST;
		BUF_OUT_ARM[1]=ZAGOL;
		CRC=CalculateCRC16(&BUF_OUT_ARM[1],66); //подсчитать CRC

		PAKETs[N_PAKET].KS_OSN=CRC;//запомнить контрольную сумму пакета
		PAKETs[N_PAKET].ARM_OSN_KAN=0x1f;//заполнить биты передачи в АРМы

		BUF_OUT_ARM[68]=(CRC&0xFF00)>>8;
		BUF_OUT_ARM[67]=CRC&0xFF;
		BUF_OUT_ARM[0]=0xAA;
		BUF_OUT_ARM[69]=0x55;

	//Передача данных в АРМ ШН
		ZAGOL_SHN=0;
		if(ZAPROS_ARM==1)ZAGOL_SHN=ZAGOL_SHN|(ARM_SHN&0xf);
		else ZAGOL_SHN=ZAGOL_SHN|(ZAPROS_ARM&0xf);
		ZAGOL_SHN=ZAGOL_SHN|((SERVER<<4)&0xf0);

		BUF_OUT_SHN[2]=SOST&0x7f;
		BUF_OUT_SHN[1]=ZAGOL_SHN;
		for(i=3;i<67;i++)BUF_OUT_SHN[i]=BUF_OUT_ARM[i];
		CRC=CalculateCRC16(&BUF_OUT_SHN[1],66); //подсчитать CRC
		BUF_OUT_SHN[68]=(CRC&0xFF00)>>8;
		BUF_OUT_SHN[67]=CRC&0xFF;
		BUF_OUT_SHN[0]=0xAA;
		BUF_OUT_SHN[69]=0x55;
		outportb(ADR_SHN_OSN+1,3);//разрешить прерывания передачи
//		if((news>0)||(fixir>0))add_ARM_OUT(ZAPROS_ARM,0);
		//======Работа с резервным каналом 
		ZAGOL=0;
		ZAGOL=ZAGOL|(ZAPROS_ARM1&0xf);
		ZAGOL=ZAGOL|((SERVER<<4)&0xf0);
		SOST=0x10;
		SOST=SOST|(1<<(SERVER-1));//установить признак работающего сервера 
		j=0;
		if(KONFIG_ARM[ZAPROS_ARM1-4][0]==0xFF)j=1;//АРМ основной в 1-ом районе
		else
			if(KONFIG_ARM[ZAPROS_ARM1-4][1]==0xFF)j=3;//АРМ основной во 2-ом районе
			else
				if(KONFIG_ARM[ZAPROS_ARM1-4][0]==1)j=2;//АРМ резервный в 1-ом районе
				else
					if(KONFIG_ARM[ZAPROS_ARM1-4][1]==2)j=4;//АРМ резервный во 2-ом районе
		if(j>0)
		{
			switch(j)
			{ //АРМ в первом районе
				case 1: SOST=SOST|0x80;break;
				//АРМ во втором районе
				case 3: SOST=SOST|0x80;break;
				default: SOST=SOST&0x7F;break;
			}
		}
		//проверяем наличие 2-ух АРМов в заданном районе
		if(KONFIG_ARM[ZAPROS_ARM1-4][0]!=0)//если данный АРМ в 1-ом районе
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][0]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		if(KONFIG_ARM[ZAPROS_ARM1-4][1]!=0)//если данный АРМ вo 2-ом районе
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][1]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		switch(SERVER)
		{
			case 1: if(sosed_PRED!=0)SOST=SOST|4;
							if(sosed_NEXT!=0)SOST=SOST|2;
							break;
			case 2: if(sosed_PRED!=0)SOST=SOST|1;
							if(sosed_NEXT!=0)SOST=SOST|4;
							break;
			case 3: if(sosed_PRED!=0)SOST=SOST|2;
							if(sosed_NEXT!=0)SOST=SOST|1;
							break;
			default: SOST=0; break;
		}
		if(PROCESS==0)SOST=SOST&0xef;
		if(KNOPKA_OK[ARM1]==1)SOST=SOST|0x20;
		SOST=SOST|PROCESS;
		BUF_OUT_ARM1[2]=SOST;
		BUF_OUT_ARM1[1]=ZAGOL;
		//переписать из основного канала на свободные места 
		for(i=n_arm1,j=n_arm;(i<67)&&(j<67);i++,j++)
		BUF_OUT_ARM1[i]=BUF_OUT_ARM[j];
		CRC=CalculateCRC16(&BUF_OUT_ARM1[1],66);
		PAKETs[N_PAKET].KS_REZ=CRC;//запомнить контрольную сумму пакета рез.кан.
		PAKETs[N_PAKET].ARM_REZ_KAN=0x1f;//заполнить биты передачи в АРМы
		BUF_OUT_ARM1[68]=(CRC&0xFF00)>>8;
		BUF_OUT_ARM1[67]=CRC&0xFF;
		BUF_OUT_ARM1[0]=0xAA;
		BUF_OUT_ARM1[69]=0x55;
//    if((news>0)||(fixir>0))add_ARM_OUT(ZAPROS_ARM1,1);
		//передача в соседние сервера копий пакетов ТС
		OUT_PAKET_PRED_NEXT();
		if(REGIM==KANAL)
		{
			for(i=0;i<15;i++)putch1(BUF_OUT_NEXT[i]|0x40,0xc,60+i,3);
			for(i=0;i<15;i++)putch1(BUF_OUT_PRED[i]|0x40,0xa,60+i,6);
		}
		N_PAKET++;
		if(N_PAKET==32) N_PAKET=0;
		if(ACTIV==1)
		{
			if(REGIM==KANAL)for(i=0;i<=28;i++)putch1(BUF_OUT_ARM[i]|0x40,atrib,41+i,11);
			outportb(ADR_ARM_OSN+1,3);//разрешить прерывания передачи

			if(REGIM==KANAL)for(i=0;i<=28;i++)putch1(BUF_OUT_ARM1[i]|0x40,atrib,41+i,14);
			outportb(ADR_ARM_REZ+1,3);//разрешить прерывания передачи
		}
		else
		{
			outportb(ADR_ARM_OSN+1,1);//разрешить прерывания приема
			outportb(ADR_ARM_REZ+1,1);//разрешить прерывания приема
		}
		if(REGIM==0)
		{
			putch1(30,0xa,54+9*(SERVER-1),35);    // запрос в основной канал АРМ 
			putch1(16,9,80,45-2*(SERVER-1));      // запрос в основной канал ШН
			putch1(17,9,77,21);
			putch1(31,14,11+9*(ZAPROS_ARM-4),29); //резервные запросы на АРМах 
			putch1(30,10,11+9*(ZAPROS_ARM1-4),33);//основныые запросы на АРМах 
			putch1(30,14,56+9*(SERVER-1),27);      //стрелочки на резервный канал АРМ 


		}
		outportb(ADR_SERV_PRED+1,0x3);
		outportb(ADR_SERV_NEXT+1,0x3);
		cikl_arm=0;
	}
	return;
}

/**************************************************\
* Процедура записи в файл данных полученных от АРМ *
*           add_ARM_IN(int arm,int kan)            *
*    arm - номер(код) АРМа - источника данных      *
*    kan - номер(код) канала,по которому был прием *
\**************************************************/
/*
void add_ARM_IN(int arm,int kan)
{
  char ZAPIS[100],ZAP[60];
  char nom[5],ARM[2];
  unsigned int ito,i,j,k;
  for(ito=0;ito<100;ito++)ZAPIS[ito]=0;
  for(ito=0;ito<60;ito++)ZAP[ito]=0;
  strcpy(ZAPIS,TIME);
  strncat(ZAPIS," ",1);
  ARM[0]=arm+49;ARM[1]=32;
  strncat(ZAPIS,ARM,2);
  strncat(ZAPIS," ",1);
  itoa(kan,nom,10);
	strcat(ZAPIS,nom);
  strncat(ZAPIS," ",1);
  switch(kan)
  {
    case 0: for(i=0;i<28;i++)
            {
              j=i<<1;
              k=j+1;
              ZAP[j]=BUF_IN_ARM[i]/16;
              if(ZAP[j]<10)ZAP[j]=ZAP[j]+48;
              else ZAP[j]=ZAP[j]+55;
              ZAP[k]=BUF_IN_ARM[i]%16;
              if(ZAP[k]<10)ZAP[k]=ZAP[k]+48;
              else ZAP[k]=ZAP[k]+55;
            }
						break;
    case 1: for(i=0;i<28;i++)
            {
              j=i<<1;
							k=j+1;
              ZAP[j]=BUF_IN_ARM1[i]/16;
							if(ZAP[j]<10)ZAP[j]=ZAP[j]+48;
              else ZAP[j]=ZAP[j]+55;
              ZAP[k]=BUF_IN_ARM1[i]%16;
              if(ZAP[k]<10)ZAP[k]=ZAP[k]+48;
              else ZAP[k]=ZAP[k]+55;
            }
            break;
   case 3: for(i=0;i<28;i++)
            {
              j=i<<1;
              k=j+1;
              ZAP[j]=KOM_BUFER[i]/16;
              if(ZAP[j]<10)ZAP[j]=ZAP[j]+48;
              else ZAP[j]=ZAP[j]+55;
							ZAP[k]=BUF_IN_ARM1[i]%16;
              if(ZAP[k]<10)ZAP[k]=ZAP[k]+48;
              else ZAP[k]=ZAP[k]+55;
            }
            break;
  }
  strcat(ZAPIS,ZAP);
  if(ACTIV==1)strncat(ZAPIS,"_A",2);
  ZAPIS[strlen(ZAPIS)]=0xd;
  ZAPIS[strlen(ZAPIS)]=0;
#ifndef TEST
  ito=write(file_arm_in,ZAPIS,strlen(ZAPIS));
#endif
	return;
}
*/
/**************************************************\
*   Процедура записи в файл данных,выданных в АРМ  *
*           add_ARM_OUT(int arm,int kan)           *
* arm - номер(код) АРМа - приемника данных         *
* kan - номер(код) канала,по которому была передача*
\**************************************************/
/*
void add_ARM_OUT(int arm,int kan)
{
  char ZAPIS[200],ZAP[141],nom[2],ARM[3];
  unsigned int ito,i,j,k;
  for(ito=0;ito<200;ito++)ZAPIS[ito]=0;
  for(ito=0;ito<140;ito++)ZAP[ito]=0;
  strcpy(ZAPIS,TIME);
  ARM[0]=arm+48;ARM[1]=32;ARM[2]=0;
  strcat(ZAPIS,ARM);
  nom[0]=kan+48;nom[1]=0;
  strcat(ZAPIS,nom);
  strncat(ZAPIS," ",1);
	switch(kan)
  {
    case 0: for(i=0;i<70;i++)
            {
              j=i<<1;
              k=j+1;
              ZAP[j]=BUF_OUT_ARM[i]/16;
              if(ZAP[j]<10)ZAP[j]=ZAP[j]+48;
              else ZAP[j]=ZAP[j]+55;

              ZAP[k]=BUF_OUT_ARM[i]%16;
              if(ZAP[k]<10)ZAP[k]=ZAP[k]+48;
              else ZAP[k]=ZAP[k]+55;
            }
            ZAP[140]=0;
            break;
    case 1: for(i=0;i<70;i++)
            {
              j=i<<1;
              k=j+1;
              ZAP[j]=BUF_OUT_ARM1[i]/16;
              if(ZAP[j]<10)ZAP[j]=ZAP[j]+48;
							else ZAP[j]=ZAP[j]+55;
							ZAP[k]=BUF_OUT_ARM1[i]%16;
							if(ZAP[k]<10)ZAP[k]=ZAP[k]+48;
							else ZAP[k]=ZAP[k]+55;
						}
						ZAP[140]=0;
						break;
	}
	strcat(ZAPIS,ZAP);
	if(ACTIV==1)strncat(ZAPIS,"_A",2);
	ZAPIS[strlen(ZAPIS)]=0xd;
	ZAPIS[strlen(ZAPIS)]=0;
	ito=write(file_arm_out,ZAPIS,strlen(ZAPIS));
	return;
}
*/
/***********************************************\
*     программа заполнения квитанций для АРМа   *
*         ZAPOLNI_KVIT(int arm,int knl)         *
*         arm - индекс АРМа                     *
*         knl - индекс канала                   *
\***********************************************/
int ZAPOLNI_KVIT(int arm,int knl)
{
  int i,n_ou=4;
  //если  есть квитанция для этого канала
  if((KVIT_ARMu[arm][knl][0]!=0)||(KVIT_ARMu[arm][knl][1]!=0))
  {
    if(knl==0)
    { //запись квитанции 
      for(i=0;i<3;i++)BUF_OUT_ARM[n_ou++]=KVIT_ARMu[arm][0][i];
      //если был объект 999 - запрос управления, то имитация передачи FR3
      if((KVIT_ARMu[arm][0][0]==0xE7)&&((KVIT_ARMu[arm][0][1]&0xf)==3))
      {
        BUF_OUT_ARM[n_ou++]=KVIT_ARMu[arm][0][0];
        BUF_OUT_ARM[n_ou++]=KVIT_ARMu[arm][0][1]&0xf;
        if(KONFIG_ARM[arm][0]==0xFF)BUF_OUT_ARM[n_ou++]=1;
        else
					if(KONFIG_ARM[arm][0]==1)BUF_OUT_ARM[n_ou++]=1;
          else
            if(KONFIG_ARM[arm][1]==0xFF)BUF_OUT_ARM[n_ou++]=2;
            else
              if(KONFIG_ARM[arm][1]==2)BUF_OUT_ARM[n_ou++]=2;
              else BUF_OUT_ARM[n_ou++]=0;
      }
      for(i=0;i<3;i++)KVIT_ARMu[arm][0][i]=0;
    }
    else
		//если есть квитанция для резервного канала
    if(knl==1)
    {
      for(i=0;i<3;i++)BUF_OUT_ARM1[n_ou++]=KVIT_ARMu[arm][1][i];
      //если объект 999
      if((KVIT_ARMu[arm][1][0]==0xE7)&&((KVIT_ARMu[arm][1][1]&0xf)==3))
      {
				BUF_OUT_ARM1[n_ou++]=KVIT_ARMu[arm][1][0];
				BUF_OUT_ARM1[n_ou++]=KVIT_ARMu[arm][1][1]&0xf;
				if(KONFIG_ARM[arm][0]==0xFF)BUF_OUT_ARM1[n_ou++]=0x81;
				else
					if(KONFIG_ARM[arm][0]==1)BUF_OUT_ARM1[n_ou++]=1;
					else
						if(KONFIG_ARM[arm][1]==0xFF)BUF_OUT_ARM1[n_ou++]=0x82;
						else
							if(KONFIG_ARM[arm][1]==2)BUF_OUT_ARM1[n_ou++]=2;
							else BUF_OUT_ARM1[n_ou++]=0;
			}
			for(i=0;i<3;i++)KVIT_ARMu[arm][1][i]=0;
		}
	}
	if((KVIT_ARMu1[arm][0][0]!=0)||(KVIT_ARMu1[arm][0][1]!=0))
	{
		if(knl==0)
		{ //запись квитанции
			for(i=0;i<3;i++)BUF_OUT_ARM[n_ou++]=KVIT_ARMu1[arm][0][i];
			for(i=0;i<3;i++)KVIT_ARMu1[arm][0][i]=0;
		}
		else
		//если есть квитанция для резервного канала
		if(knl==1)
		{
			for(i=0;i<3;i++)BUF_OUT_ARM1[n_ou++]=KVIT_ARMu1[arm][1][i];
			for(i=0;i<3;i++)KVIT_ARMu1[arm][1][i]=0;
		}
	}
	return(n_ou);
}
/*********************************************\
* Процедура анализа наличия готовых маршрутов *
* для записи в буфер маршрутных команд стойки *
\*********************************************/
void MARSH_GLOB_LOCAL(void)
{
	int i_s,s_m,ii,kk;
	unsigned char sum;
	for(i_s=0;i_s<Nst;i_s++)     //Пройти по всем стойкам станции
	{
		//если стойка занята-продолжить
		if(TUMS_RABOT[i_s]!=0)continue;
		//если для стойки уже есть неподтвержденнная команда - продолжить
		if(KOMANDA_TUMS[i_s][10]!=0)continue;
		//Пройти по локальным маршрутам стойки
		for(s_m=0;s_m<MARS_STOY;s_m++)
		{
			//если есть команда в стойке i_s строке s_m и она еще не выдана в ТУМС
			if((MARSHRUT_ST[i_s][s_m].NEXT_KOM[0]!=0)&&
			(MARSHRUT_ST[i_s][s_m].T_VYD==0l))
			{
//				if(MARSH_VYDAN[i_s]==0)
				//если состояние маршрута "разбит на локальные" и стойка не занята
				if((MARSHRUT_ST[i_s][s_m].SOST&0xF)==0x7)
				{
					//заполнить команду для ТУМС
					for(kk=0;kk<12;kk++)KOMANDA_TUMS[i_s][kk]=MARSHRUT_ST[i_s][s_m].NEXT_KOM[kk];
					//установить признак выдачи маршрута
					switch(MYTHX_TEC[i_s])
					{
						case 0x50: MYTHX_TEC[i_s]=0x60;
											 putch1('2',140,66+i_s,49);
											 break;

						case 0x60: MYTHX_TEC[i_s]=0x70;
											 putch1('3',140,66+i_s,49);
											 break;
						case 0:
						case 0x6e:
						case 0x70: MYTHX_TEC[i_s]=0x50;
											 putch1('1',140,66+i_s,49);
											 break;
						default: break;
					}
					KOMANDA_TUMS[i_s][7]=KOMANDA_TUMS[i_s][7]|MYTHX_TEC[i_s];
					sum=0;
					for(kk=1;kk<10;kk++)sum=sum^KOMANDA_TUMS[i_s][kk];
					sum=sum|0x40;
					KOMANDA_TUMS[i_s][10]=sum;
					if(REGIM==COMMAND)
					{
						putch1(0x10,0xc,1,Y_KOM);
						if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
						else putch1(' ',0xc,1,46);
						puts1(KOMANDA_TUMS[i_s],0xa,55,Y_KOM);
						Y_KOM++;
						if(Y_KOM>=47)Y_KOM=3;
						puts1("                                   ",0xa,3,Y_KOM);
					}
					add(i_s,9999,0); //записать на диск
					//зафиксировать время выдачи маршрута
					KOMANDA_TUMS[i_s][14]=s_m; //запомнить строку локальной таблицы
					MARSHRUT_ST[i_s][s_m].NEXT_KOM[12]=MYTHX_TEC[i_s];//запомнить МИФ
					//зафиксировать время ожидания для маршрута
					kk=MARSHRUT_ST[i_s][s_m].NUM-100;
					kk=MARSHRUT_ALL[kk].KOL_STR[i_s]*20+10;
					MARSHRUT_ST[i_s][s_m].T_MAX=kk;
					TUMS_RABOT[i_s]=0xf;
					MARSH_VYDAN[i_s]=0xf;
				}
			}
		}
	}
}
