/*************************************************\
*                                                  *
* COMMAND.C - файл формирования команд на объекты  *
*                                                  *
\**************************************************/

#include "opred2.h"
#include "extern2.h"
/*************************************************************\
*     MAKE_KOMANDA(int ARM,int STAT,unsigned char ray)        *
*     Процедура трансформации раздельных команд из АРМа       *
*              в коды команд стоек ТУМС                       *
*       ARM  - индекс АРМа, выдавшего команду                 *
*       STAT - код канала,по которому получена команда        *
*       ray  - код района управления                          *
\*************************************************************/
void MAKE_KOMANDA(int ARM,int STAT,int ray)
{
	unsigned int obj,obj_serv,ii,tst_sum,tip_ob,ob_ob;
	unsigned char komanda;
  if(KOMANDA_RAZD[ARM][STAT][0]!=0)//если есть первая раздельная
	{ //выделение номера объекта АРМа
		obj=KOMANDA_RAZD[ARM][STAT][1]+KOMANDA_RAZD[ARM][STAT][2]*256;

		if((obj<=0)||(obj>=KOL_VO))
		{
			for(ii=0;ii<3;ii++)KOMANDA_RAZD[ARM][STAT][ii]=0;
			return;
		}
		komanda=KOMANDA_RAZD[ARM][STAT][0]; //выделение кода команды
		//-----------------------------------------------------------------
		//-----------обработка команд на переключение района и АРМа
		//-----------------------------------------------------------------
    if(obj==999)
    {
			if((komanda==79)&&(PROCESS==0x40))//если автозапрос управления
      {
         //если автозапрос от АРМа предустановки первого района
        if((ARM+4)==SOST_RANJ[0])
        { //освободить данный АРМ от всех районов
          for(ii=0;ii<Nranj;ii++)KONFIG_ARM[ARM][ii]=0;
          //проверить свободность данного района от управления
					tst_sum=0;
          for(ii=0;ii<Narm;ii++)tst_sum=tst_sum+KONFIG_ARM[ii][0];
          if(tst_sum<0xff)//если район не управляется никем
          {
            KONFIG_ARM[ARM][0]=0xFF;//зафиксировать управление районом
            KVIT_ARMu[ARM][STAT][2]=0x81;//разрешить АРМу управление
          }
          else//если район уже кем-то управляется
          {
            KONFIG_ARM[ARM][0]=0x1;//зафиксировать принадлежность АРМа району
            KVIT_ARMu[ARM][STAT][2]=0x6;//указать АРМу район отображения
          }
        }
        else //если автозапрос от АРМа предустановки второго района
				if((ARM+4)==SOST_RANJ[1])
        {
          //освободить данный АРМ от всех районов
          for(ii=0;ii<Nranj;ii++)KONFIG_ARM[ARM][ii]=0;
          //проверить свободность данного района от управления
          tst_sum=0;
          for(ii=0;ii<Narm;ii++)tst_sum=tst_sum+KONFIG_ARM[ii][1];
          if(tst_sum<0xff)//если район не управляется никем
          {
            KONFIG_ARM[ARM][1]=0xFF;
            KVIT_ARMu[ARM][STAT][2]=0x82;
					}
          else
          {
						KVIT_ARMu[ARM][STAT][2]=0x6;
            KONFIG_ARM[ARM][1]=0x2;
          }
        }
      }
      else
      if((komanda==80)&&(PROCESS==0x40))//если запрос оператора 
      {
        //проверяем свободность запрошенного района
        for(ii=0;ii<Narm;ii++)if(KONFIG_ARM[ii][ray-1]==0xFF)break;
        //если район кем-то управляется
        if(ii<Narm)
        {
          KONFIG_ARM[ii][ray-1]=ray; //перевести данный АРМ в резерв
          //готовим снятие управления (номер объекта
          KVIT_ARMu[ii][STAT][0]=KOMANDA_RAZD[ARM][STAT][1];
          KVIT_ARMu[ii][STAT][1]=KOMANDA_RAZD[ARM][STAT][2];
          KVIT_ARMu[ii][STAT][2]=ray;

          KONFIG_ARM[ARM][ray-1]=0xFF;
          KVIT_ARMu[ARM][STAT][1]=KVIT_ARMu[ARM][STAT][1]|0x40;
					KVIT_ARMu[ARM][STAT][2]=0;
        }
        //если район свободен
        else
        {
          //освобождаем данный АРМ от всех районов
          for(ii=0;ii<Nranj;ii++)KONFIG_ARM[ARM][ii]=0;
          KONFIG_ARM[ARM][ray-1]=0xFF; //устанавливаем АРМ управляющим в районе
          KVIT_ARMu[ARM][STAT][1]=KVIT_ARMu[ARM][STAT][1]|0x40;
          KVIT_ARMu[ARM][STAT][2]=0;
        }
			}
      return;
    }
		//------------------------------------------------
		//  обработка команды для перезапуска серверов
		//------------------------------------------------
		if(obj==998)
		{
			if(komanda!=96)return;
			else
			{
				if(ACTIV==1)RESET_TIME=10; //активный сбрасывается 0,5 сек
			}
			return;
		}
		//-------------------------------------------------
		//  обработка остальных команд
		//-------------------------------------------------
		//находим соответствующий объект сервера 

		obj_serv=inp_ob[obj];
		if(obj_serv>=KOL_VO)return;
		if(obj_serv<0)return;
		READ_BD(obj_serv); 											//читаем объект сервера 
		if(bd_osn[0]<100)tip_ob=bd_osn[0]&0xF; //выделяем тип объекта 
		else tip_ob=bd_osn[0];
		if(((tip_ob>=1)&&(tip_ob<=5))||((tip_ob>=8)&&(tip_ob<=10)))
		{
			switch(tip_ob) //в зависимости от объекта вызываем команды
			{
				case 1: perevod_strlk(komanda,obj_serv);break;

				case 2: signaly(komanda,obj_serv);break;
				case 3:
				case 4: sp_up_and_razd(komanda,obj_serv,ARM);break;
				case 5: puti(komanda,obj_serv);break;
				case 8: if((komanda==64)||(komanda==65)||(komanda==87)||(komanda==88))
								prosto_komanda(komanda);
								else  dopoln_obj(komanda,ARM);
								break;
				case 10:
				case 9: ob_tums(komanda);break;
				default: break;
			}
			for(ii=0;ii<16;ii++)bd_osn[ii]=0;
			for(ii=0;ii<12;ii++)KOMANDA_RAZD[ARM][STAT][ii]=0;
			return;
		}
		else
		// если это маневровая колонка
		if(bd_osn[0]==226)
		{
			if(komanda==71) //если немаршрутизированные маневры
			{
				KOMANDA_MARS[ARM][STAT][0]=71;
				ob_ob=bd_osn[1];
				KOMANDA_MARS[ARM][STAT][1]=out_ob[ob_ob]&0xff;
				KOMANDA_MARS[ARM][STAT][2]=(out_ob[ob_ob]&0xff00)>>8;
				ob_ob=bd_osn[2];
				KOMANDA_MARS[ARM][STAT][3]=out_ob[ob_ob]&0xff;
				KOMANDA_MARS[ARM][STAT][4]=(out_ob[ob_ob]&0xff00)>>8;
				KOMANDA_MARS[ARM][STAT][5]=(bd_osn[3]&0xf000)>>12;
				KOMANDA_MARS[ARM][STAT][6]=bd_osn[3]&0xfff;
				KOMANDA_MARS[ARM][STAT][7]=0;
				KOMANDA_MARS[ARM][STAT][8]=0;
				KOMANDA_MARS[ARM][STAT][9]=0;
				for(ii=0;ii<12;ii++)
				{
					KOMANDA_RAZD[ARM][STAT][ii]=0;
				}
				MAKE_MARSH(ARM,STAT);
				for(ii=0;ii<12;ii++)KOMANDA_MARS[ARM][STAT][ii]=0;
			}
			if(komanda==72)otmena_rm(obj_serv);
			if((komanda==64)||(komanda==65)||(komanda==87)||(komanda==88))
			{
				prosto_komanda(komanda);
			}
			for(ii=0;ii<16;ii++)bd_osn[ii]=0;
			for(ii=0;ii<12;ii++)KOMANDA_RAZD[ARM][STAT][ii]=0;
			return;
		}
		else
		if(bd_osn[0]==238)
		{
			if(komanda==73) //если включить день
			{
				obj_serv=bd_osn[1];
				if(bd_osn[3]==1)komanda=64;
				if(bd_osn[3]==2)komanda=87;
				READ_BD(obj_serv);
				prosto_komanda(komanda);
			}
			if(komanda==74) //если включить ночь
			{
				obj_serv=bd_osn[2];
				if(bd_osn[4]==1)komanda=64;
				if(bd_osn[4]==2)komanda=87;
				READ_BD(obj_serv);
				prosto_komanda(komanda);
			}
			for(ii=0;ii<16;ii++)bd_osn[ii]=0;
		}
	}
	for(ii=0;ii<12;ii++)KOMANDA_RAZD[ARM][STAT][ii]=0;
	return;
}
//======================================================================
/**********************************************************\
*  Процедура обработки маршрутных команд принятых от АРМа  *
*                  MAKE_MARSH(int ARM,int STAT)            *
*   ARM - номер(код) АРМа, выдавшего команду маршрута      *
*   STAT - номер(код) канала, по которому принята команда  *
\**********************************************************/
void MAKE_MARSH(int ARM,int STAT)
{
	unsigned int obj,nach_marsh,end_marsh,nstrel,ii;
	unsigned long pol_strel;
	unsigned char komanda;
	if(KOMANDA_MARS[ARM][STAT][0]!=0)//если есть маршрутная
	{
		obj=KOMANDA_MARS[ARM][STAT][1]+KOMANDA_MARS[ARM][STAT][2]*256;
		if((obj<=0)||(obj>=KOL_VO))//если объект начала вне базы
    {
      for(ii=0;ii<12;ii++)KOMANDA_MARS[ARM][STAT][ii]=0;
      return;
		}
		komanda=KOMANDA_MARS[ARM][STAT][0];//получить код команды
		if((komanda==189)||(komanda==188))POVTOR_OTKR=0xFF;
		nach_marsh=inp_ob[obj];
		obj=KOMANDA_MARS[ARM][STAT][3]+KOMANDA_MARS[ARM][STAT][4]*256;
		if((obj<=0)||(obj>=KOL_VO))//если объект начала вне базы
    {
      for(ii=0;ii<12;ii++)KOMANDA_MARS[ARM][STAT][ii]=0;
      return;
		}
		end_marsh=inp_ob[obj];//получить конец маршрута
		nstrel=KOMANDA_MARS[ARM][STAT][5];
		pol_strel=KOMANDA_MARS[ARM][STAT][6]+(KOMANDA_MARS[ARM][STAT][7]<<8)+
		(KOMANDA_MARS[ARM][STAT][8]<<16)+(KOMANDA_MARS[ARM][STAT][9]<<24);
		//анализ топологии маршрута и заполнение массива TRASSA или TRASSA1
		ii=ANALIZ_MARSH(komanda,nach_marsh,end_marsh,nstrel,pol_strel);
		//ii - номер строки глобального маршрута , куда записан данный маршрут
		if(ii<Nst*3)TUMS_MARSH(ii);
		else add(ii,8888,77);
//		else Soob_For_Arm(ii,nach_marsh,ii);
	}
	return;
}
//==========================================================================
/*************************************************************************\
* otmena_rm - модуль формирования команды отмены немаршрут.маневров       *
* int bd_osn[16] - строка описания объекта управления для РМ в БД         *
* unsigned int objserv - объект в сервере для управляемой колонки         *
\****************************************************************а********/
void otmena_rm(unsigned int objserv)
{
	unsigned char tums,//номер стойки
	gruppa,//код группы для данного объекта
	podgruppa, //код подгруппы
	kod_cmd, //код команды
	bit,    //номер бaйта для данного объекта
	ii;     //вспомогательная переменная
	int i_m,i_s,i_sig,kontr;
	unsigned int ob_next;
	tums=(bd_osn[13]&0xff00)>>8;
	gruppa='Q';
	podgruppa=bd_osn[15]&0xff;
	kod_cmd='F';//отмена поездного маршрута
	ii=0;
	objserv=bd_osn[1];
	if(objserv>0)
	{ kontr=0;
		for(i_m=0;i_m<Nst*3;i_m++)
		{
			for(i_s=0;i_s<Nst;i_s++)
			{
				for(i_sig=0;i_sig<10;i_sig++)
				{
					if(MARSHRUT_ALL[i_m].SIG[i_s][i_sig]==objserv)
					{
						DeleteMarsh(i_m);
						kontr=15;
						break;
					}
				}
				if(kontr!=0)break;
			}
			if(kontr!=0)break;
		}
	}
	while(objserv!=0)
	{
		if((objserv>=KOL_VO)||(objserv<=0))return;
		read_FR3(objserv);
		if(((FR3[20]&0x80)==0x80)&&(ii!=0))break;
		ii++;
    //снять признаки замыкания
    if((FR3[12]!=0)||(FR3[13]!=0)||(FR3[14]!=0)||(FR3[15]!=0))
    {
      //снять признаки замыкания
      FR3[12]=0;FR3[13]=0;
      FR3[14]=0;FR3[15]=0;
      obnovi(objserv);
    }
    ob_next=(FR3[20]&0xF)*256+FR3[21];
    FR3[20]=0;FR3[21]=0;
    //сохранить измененные массивы на виртуальном диске 
    write_FR3(objserv);
    objserv=ob_next;
  }
  bit=(bd_osn[15]&0xe000)>>13;
  ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
  return;
}
//=========================================================================
/*************************************************************************\
* perevod_strlk - модуль формирования раздельной команды на стрелку       *
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления для стрелки в БД    *
\*************************************************************************/
void perevod_strlk(unsigned char command,unsigned int objserv)
{
  unsigned char tums,//номер стойки
  gruppa,//код группы для данного объекта
  podgruppa, //код подгруппы для стрелки
  kod_cmd, //код команды для стрелки
  bit;    //номер байта для данного объекта
  //--------------------------------------------------------------
	int Err=0,i_m,i_s,i_str;
  tums=(bd_osn[13]&0xff00)>>8;
  gruppa='C';
  podgruppa=bd_osn[15]&0xff;
  switch(command)
  {
    //отключить стрелку от управления
    case 31:  FR4[objserv]=FR4[objserv]|1;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
    //включить стрелку в управление
    case 32:  FR4[objserv]=FR4[objserv]&0xFE;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;


    //закрыть движение по стрелке
    case 33:  FR4[objserv]=FR4[objserv]|4;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
    //открыть движение по стрелке
    case 34:  FR4[objserv]=FR4[objserv]&0xFB;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
    //закрыть движение по дальней стрелке
    case 35:  FR4[objserv]=FR4[objserv]|8;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
    //открыть движение по дальней стрелке
    case 36:  FR4[objserv]=FR4[objserv]&0xF7;
							NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
		//	установить макет на стрелку
    case 37:  FR4[objserv]=FR4[objserv]|2;
    					NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //снять стрелку с макета
		case 38:  FR4[objserv]=FR4[objserv]&0xFD;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    	//закрыть противошерстное движение по стрелке
    case 114:  FR4[objserv]=FR4[objserv]|0x10;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //открыть противошерстное движение
    case 115: FR4[objserv]=FR4[objserv]&0xEF;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //закрыть противошерстное по дальней
    case 116: FR4[objserv]=FR4[objserv]|0x20;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //открыть противошерстное по дальней
    case 117: FR4[objserv]=FR4[objserv]&0xDF;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    case 41:  kod_cmd='A';break;  //простой перевод стрелки в плюс
    case 42:  kod_cmd='B';break;  //простой перевод в минус
		case 43:  kod_cmd='Q';break;  //вспомогательный в плюс
    case 44:  kod_cmd='R';break;  //вспомогательный в минус
    case 81:  kod_cmd='E';break;  //перевод в плюс на макете
    case 82:  kod_cmd='F';break;  //перевод в минус на макете
    case 83:  kod_cmd='U';break;  //вспомогательный на макете в плюс
    case 84:  kod_cmd='V';break;  //вспомогательный на макете в минус

    case 107: kod_cmd='I';break; //перевод в плюс с реверсом
    case 127: kod_cmd='J';break;  //перевод в минус с реверсом
    default:  return;
  }
  
  if((command>=41)&&(command<=84))
  {
  	for(i_m=0;i_m<Nst*3;i_m++)
  	for(i_s=0;i_s<Nst;i_s++)
  	for(i_str=0;i_str<10;i_str++)
		if((MARSHRUT_ALL[i_m].STREL[i_s][i_str]&0xfff)==objserv)
  	{
			add(i_m,8888,13);
			DeleteMarsh(i_m);
		}
  }
  if(Err==0)
  {
  	bit=(bd_osn[15]&0xe000)>>13;
  	ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
	}  	
  return;
}
//===========================================================================
/*************************************************************************\
* signaly - модуль формирования раздельных команд на сигнал               *
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления для сигнала в БД    *
* unsigned objserv - объект в сервере для управляемого сигнала            *
\*************************************************************************/
void signaly(unsigned char command,unsigned int objserv)
{
  unsigned char tums,//номер стойки
  gruppa,//код группы для данного объекта
  podgruppa, //код подгруппы для стрелки
  kod_cmd, //код команды для стрелки
	sp_in_put,
  spar_sp,
	i_m,
	i_s,
	i_sig,
	bit,
	ii;     //вспомогательная переменная
	unsigned int ob_next,job;
	int shag,kk;
	tums=(bd_osn[13]&0xff00)>>8;
	gruppa='E';
	podgruppa=bd_osn[15]&0xff;
	bit=(bd_osn[15]&0xe000)>>13;
	switch(command)
	{
								 //закрытие движения
		case 33:  FR4[objserv]=FR4[objserv]|4;
							NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							return;
		case 34:  FR4[objserv]=FR4[objserv]&0xFB;
							NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							return;
		case  95:
		case 111:
		case  45:  kod_cmd='A';break;

		case 112:
		case  46:  kod_cmd='B';break;

		case 47:  kod_cmd='E';//отмена маневрового маршрута
							ii=0;
							//Пройти по всем глобальным
							for(i_m=0;i_m<Nst*3;i_m++)
							{
								//пройти по всем стойкам
								for(i_s=0;i_s<Nst;i_s++)
								{
									//пройти по списку сигналов в стойке
									for(i_sig=0;i_sig<10;i_sig++)
									{
										//если в этом глобальном для этой стойки
										//обнаружен этот сигнал, то удалить глобальный
										if(MARSHRUT_ALL[i_m].SIG[i_s][i_sig]==objserv)
										{
											add(i_m,8888,15);
											DeleteMarsh(i_m);
											ii=15;
											break;
										}
									}
									if(ii==15)break;
								}
								if(ii==15)break;
							}
							ii=0;
							READ_BD(objserv);//прочитать базу сигнала
							if(bd_osn[1]==0)shag=-1;//для четного сигнала идти назад
							else shag=1; //для нечетного идти вперед
							read_FR3(objserv);
							FR3[20]=FR3[20]&0xF;
							obnovi(objserv);
							write_FR3(objserv);


							//начать со следующего объекта и идти до СП или УП
							for(kk=objserv+shag;(kk>0)&&(kk<KOL_VO);)
							{
								READ_BD(kk);
								if((bd_osn[0]==3)||(bd_osn[0]==4))break;
								if(bd_osn[0]==6)
								{
									if((shag==1)&&(bd_osn[3]==1))
									{
										kk=bd_osn[1];
										if(bd_osn[2])
										{
											shag=-shag; //если инверсный переход
											kk=kk+shag;
										}
									}
									else
									{ //если движемся в четном и переход четный
										if((shag==-1)&&(bd_osn[3]==0))
										{
											kk=bd_osn[1];
											if(bd_osn[2])
											{
												shag=-shag; //если инверсный переход
												kk=kk+shag;
											}
										}
										else
										{ //если встрeчный переход
											kk=kk+shag;
										}
									}
								}
								else kk=kk+shag;
							}
							if((kk>=KOL_VO)||(kk<=0))return; //найти FR3
							read_FR3(kk);//прочитать состояние УП или СП
							if((FR3[2]==0)&&(FR3[3]==0)) // если разомкнуто
							{
								if((objserv>=KOL_VO)||(objserv<=0))return;
								read_FR3(objserv);//прочитать состояние сигнала
								//если для сигнала не фиксировалось открытие
								//или замыкания перекрывной секции
								if((FR3[24]&0x80)==0)
								{
									FR3[12]=0;FR3[13]=0;  //снять признаки начала
									obnovi(objserv);
									write_FR3(objserv);
								}
							}
							while(objserv!=0)
							{
								if((objserv>=KOL_VO)||(objserv<=0))return;
								READ_BD(objserv);
								read_FR3(objserv);

								if(bd_osn[0]==2)//если сигнал
								//если открывался или было замыкание перекрывной
								// и это не первый сигнал, то прервать
								if(((FR3[20]&0x80)==0x80)&&(ii!=0))break;

								//снять признаки замыкания
								if((FR3[3]==0)&&(FR3[5]==0)&&(FR3[7]==0)&&(ii!=0))
								{
									if((FR3[24]&0x80)==0)//для первого сигнала или для СП
									{
										FR3[12]=0;FR3[13]=0; //убрать начала или замыкания
										//если СП, УП или путь - обновить для АРМа
										if((bd_osn[0]>=3)||(bd_osn[0]<=5))obnovi(objserv);
										//если путь - обработать сочлененный
										if(bd_osn[0]==5)
										{
											if(bd_osn[1]!=0)
											{
												job=bd_osn[1];
												read_FR3(job);
												FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
												FR3[20]=0;FR3[21]=0;
												write_FR3(job);
												if(bd_osn[0]<6)obnovi(objserv);
											}
										}
									}

								}
								ob_next=(FR3[20]&0xF)*256+FR3[21];
								if(ii){FR3[20]=0;FR3[21]=0;}
								ii++;
								//сохранить измененные массивы
								if((objserv>=KOL_VO)||(objserv<=0))return;
								write_FR3(objserv);
								objserv=ob_next;
							}
							break;

		case 48:  kod_cmd='F';//отмена поездного маршрута
							ii=0;
							//поиск сигнала в таблице маршрутов
							for(i_m=0;i_m<Nst*3;i_m++)
							{
								for(i_s=0;i_s<Nst;i_s++)
								{
									for(i_sig=0;i_sig<10;i_sig++)
									{
										if(MARSHRUT_ALL[i_m].SIG[i_s][i_sig]==objserv)
										{
											//если сигнал найден-удалить маршрут с этим сигналом
											add(i_m,8888,14);
											DeleteMarsh(i_m);
											ii=15;
											break;
										}
									}
									if(ii==15)break;
								}
								if(ii==15)break;
							}
							ii=0;
							READ_BD(objserv);//прочитать объект сигнала
							//определить направление движения
							if(bd_osn[1]==0)shag=-1;
							else shag=1;
							//прочитать состояние сигнала
							read_FR3(objserv);
							FR3[20]=FR3[20]&0xF;//снять признаки начала
							obnovi(objserv); //установить признак новизны для объекта
							write_FR3(objserv); //записать в массив FR3
							//пройти по объектам от данного сигнала вперед или назад
							//до упора, для поиска перекрывной секции
							for(kk=objserv+shag;(kk>0)&&(kk<KOL_VO);)
							{
								READ_BD(kk);//прочитать объект
								//если это СП или УП - прерваться
								if((bd_osn[0]==3)||(bd_osn[0]==4))break;
								//если переход - то идти дальше
								if(bd_osn[0]==6)
								{
									if((shag==1)&&(bd_osn[3]==1))
									{
										kk=bd_osn[1];
										if(bd_osn[2])
										{
											shag=-shag; //если инверсный переход
											kk=kk+shag;
										}
									}
									else
									{ //если движемся в четном и переход четный
										if((shag==-1)&&(bd_osn[3]==0))
										{
											kk=bd_osn[1];
											if(bd_osn[2])
											{
												shag=-shag; //если инверсный переход
												kk=kk+shag;
											}
										}
										else
										{ //если встрeчный переход
											kk=kk+shag;
										}
									}
								}
								else kk=kk+shag;
							}
							if((kk>=KOL_VO)||(kk<=0))return;
							read_FR3(kk); //прочитать состояние УП или СП
							//если нет замыкания и нет разделки
							if((FR3[2]==0)&&(FR3[3]==0))
							{
								if((objserv>=KOL_VO)||(objserv<=0))return;
								read_FR3(objserv);//прочитать сигнал
								//если
								if((FR3[24]&0x80)==0)
								{
									FR3[12]=0;FR3[13]=0;
									FR3[14]=0;FR3[15]=0;
									obnovi(objserv);
									write_FR3(objserv);
								}
								read_FR3(kk);
							}
							while(objserv!=0)
							{
								if((objserv>=KOL_VO)||(objserv<=0))return;
								read_FR3(objserv);
								READ_BD(objserv);
								if((bd_osn[0]==2)&&((FR3[20]&0x80)==0x80)&&(ii))break;
								if((bd_osn[0]==7)&&(bd_osn[1]==15))
								{
                	sp_in_put=bd_osn[3];
                	read_FR3(sp_in_put);
                	FR3[12]=0;FR3[13]=0;
                	FR3[14]=0;FR3[15]=0;
                	obnovi(sp_in_put);
                	write_FR3(sp_in_put);
                	spar_sp=bd_osn[5];
                	if(spar_sp!=0)
									{
                		read_FR3(spar_sp);
                		FR3[12]=0;FR3[13]=0;
                		FR3[14]=0;FR3[15]=0;
                		obnovi(spar_sp);
                		write_FR3(spar_sp);
                	}
								}
                if(bd_osn[0]==5)
                {
                	if(bd_osn[1]!=0)
                	{
                		job=bd_osn[1];
                		read_FR3(job);
                		FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
                		obnovi(job);
                 		write_FR3(job);
                	}
                }
                //остановиться если
								if(((FR3[20]&0x80)==0x80)&&(ii!=0))break;
                //снять признаки замыкания

								if((FR3[1]==0)&&(FR3[3]==0)&&(FR3[5]==0)&&(FR3[7]==0)&&(ii!=0))
                {
									FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
                	if((bd_osn[0]>=3)&&(bd_osn[0]<=5))
                  {
	                  obnovi(objserv);
                	}
                }
                ob_next=(FR3[20]&0xF)*256+FR3[21];
                if(ii)
                { 
                	FR3[20]=0;FR3[21]=0;
									if(bd_osn[0]<6)obnovi(objserv);
                }
                ii++;
                //сохранить измененные массивы на виртуальном диске
                write_FR3(objserv);
                objserv=ob_next;
							}
              break;
    case 49:  kod_cmd='A';break;
    case 50:  kod_cmd='B';break;
    case 62:  kod_cmd='B';break;
		case 85:  if((objserv>=KOL_VO)||(objserv<=0))return;
    					read_FR3(objserv);
              //снять признаки замыкания
              FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
              FR3[20]=0;FR3[21]=0;
              obnovi(objserv);
              write_FR3(objserv);
              return;
    default: return;
	}
  ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
  return;
}
//=========================================================================
/*************************************************************************\
* sp_up_and_razd модуль формирования раздельных команд на СП,УП и разделку*
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления для СП,УП и РИ      *
\*************************************************************************/
void sp_up_and_razd(unsigned char command,unsigned int objserv,int arm)
{

  unsigned char tums,//номер стойки
  gruppa,//код группы для данного объекта
	podgruppa, //код подгруппы для стрелки
  kod_cmd, //код команды для стрелки
  bit;    //номер бaйта для данного объекта
  tums=(bd_osn[13]&0xff00)>>8;
  gruppa='F';
	podgruppa=bd_osn[15]&0xff;
  switch(command)
  {
		case 85:  if((objserv>=KOL_VO)||(objserv<=0))return;
    					read_FR3(objserv);
              //снять признаки замыкания
              FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
              FR3[20]=0;FR3[21]=0;
              obnovi(objserv);
              write_FR3(objserv);
              return;
             //закрытие движения
    case 33:  FR4[objserv]=FR4[objserv]|4;
    					NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;
    case 34:  FR4[objserv]=FR4[objserv]&0xFB;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              return;

            //закрыть движение на электротяге
    case 118: FR4[objserv]=FR4[objserv]|8;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //открыть движение на электротяге
    case 119: FR4[objserv]=FR4[objserv]&0xF7;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
   //закрыть движение на электротяге переменного тока
    case 120:  FR4[objserv]=FR4[objserv]|2;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //открыть движение на электротяге переменного тока
    case 121: FR4[objserv]=FR4[objserv]&0xFD;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
   //закрыть движение на постоянном токе
    case 122:  FR4[objserv]=FR4[objserv]|1;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;
    //открыть движение на постоянном токе
    case 123: FR4[objserv]=FR4[objserv]&0xFE;
              NOVIZNA_FR4[new_fr4++]=objserv;
              if(new_fr4>=10)new_fr4=0;
              return;

    case 66:  kod_cmd='L';break; //включить выдержку времени ГРИ

    case 6:
		case 9:
    case 10:
		case 1:   if(KNOPKA_OK[arm]==1){kod_cmd='T';break;}
              else return;

    case 198:
    case 201:
    case 202:
    case 193:
#ifndef TEST
              if(KNOPKA_OK[arm]==1)
#endif
              {kod_cmd='S';break;}
#ifndef TEST
              else return;
#endif
    default: return;
  }
  bit=(bd_osn[15]&0xe000)>>13;
  ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
  return;
}
//=============================================================================
/*************************************************************************\
* puti - модуль формирования раздельных команд для пути                   *
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления для пути в БД       *
\****************************************************************а********/
void puti(unsigned char command,unsigned int objserv)
{
  int job;
  switch(command)
  { //нормализация пути
		case 85:  if((objserv>=KOL_VO)||(objserv<=0))return;
              read_FR3(objserv);
							//снять признаки замыкания
              FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
              FR3[20]=0;FR3[21]=0;
              obnovi(objserv);            
              write_FR3(objserv);
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
								if((job>=KOL_VO)||(job<=0))return;
                read_FR3(job);
              	//снять признаки замыкания
              	FR3[12]=0;FR3[13]=0;FR3[14]=0;FR3[15]=0;
              	FR3[20]=0;FR3[21]=0;
              	obnovi(job);
                write_FR3(job);
              }
              return;
    //закрытие движения по пути
    case 33:  FR4[objserv]=FR4[objserv]|4;
              NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							READ_BD(objserv);
							if(bd_osn[1]!=0)
							{
								job=bd_osn[1];
								FR4[job]=FR4[job]|4;
								NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
								if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							}
							return;
		//открыть движение по пути
		case 34:  FR4[objserv]=FR4[objserv]&0xFB;
							NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							READ_BD(objserv);
							if(bd_osn[1]!=0)
							{
								job=bd_osn[1];
								FR4[job]=FR4[job]&0xFB;
								NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
								if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							}
							return;
		//закрытие движения на электротяге
		case 118:	FR4[objserv]=FR4[objserv]|8;
							NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
							READ_BD(objserv);
							if(bd_osn[1]!=0)
							{
              	job=bd_osn[1];
                FR4[job]=FR4[job]|8;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
                if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
              return;

    //открытие движения на электротяге
    case 119:	FR4[objserv]=FR4[objserv]&0xF7;
          		NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
                FR4[job]=FR4[job]&0xF7;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
								if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
							return;

    //закрытие движения на электротяге переменного тока
    case 120:	FR4[objserv]=FR4[objserv]|2;
          		NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
                FR4[job]=FR4[job]|2;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
                if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
              return;

    //открытие движения на электротяге переменного тока
    case 121:	FR4[objserv]=FR4[objserv]&0xFD;
          		NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
                FR4[job]=FR4[job]&0xFD;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
                if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
              return;

    //закрытие движения на электротяге постоянного тока
    case 122:	FR4[objserv]=FR4[objserv]|1;
          		NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
							if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
                FR4[job]=FR4[job]|1;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
                if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
              return;

    //открытие движения на электротяге постоянного тока
    case 123:	FR4[objserv]=FR4[objserv]&0xFE;
          		NOVIZNA_FR4[new_fr4++]=objserv;//запомнить номер обновленного объекта
              if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              READ_BD(objserv);
              if(bd_osn[1]!=0)
              {
              	job=bd_osn[1];
                FR4[job]=FR4[job]&0xFE;
                NOVIZNA_FR4[new_fr4++]=job;//запомнить номер обновленного объекта
                if(new_fr4>=10)new_fr4=0; //если не передано более 10 новизн,начать с 0
              }
              return;


    default: return;
  }
}
//=============================================================================
/*************************************************************************\
* dopoln_obj - модуль формирования раздельных команд для доп.объектов     *
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления для пути в БД       *
\****************************************************************а********/
void dopoln_obj(unsigned char command,int arm)
{
  unsigned char tums,//номер стойки
  gruppa,//код группы для данного объекта
	podgruppa, //код подгруппы для стрелки
	kod_cmd, //код команды для стрелки
	bit;    //номер бaйта для данного объекта
	tums=(bd_osn[13]&0xff00)>>8;
	gruppa='Q';
	podgruppa=bd_osn[15]&0xff;
	switch(command)
	{
		case 8:
		case 200: if(KNOPKA_OK[arm]==1){kod_cmd='X';break;}
							else return;

		case 55: if(KNOPKA_OK[arm]==1){kod_cmd='N';break;}
						 else return;

		case 198:
		case 199:
		case 195:
		case 202:
		case 3:
		case 6:
		case 7:
		case 194:
		case 2:  if(KNOPKA_OK[arm]==1){kod_cmd='T';break;}
						 else return;

		case 103:
		case 60: if(KNOPKA_OK[arm]==1){kod_cmd='A';break;}
						 else return;

		case 104: if(KNOPKA_OK[arm]==1){kod_cmd='M';break;}
							else return;

		case 125: {kod_cmd='C';break;}

		case 53:
		case 75:
		case 105:
		case 67:
		case 61: kod_cmd='A';break;



		case 197: if(KNOPKA_OK[arm]==1){kod_cmd='M';break;}
						 else return;

		case 106:
		case 68: kod_cmd='M';break;

		case 63:
		case 76:
		case 78:
		case 59:
		case 57: kod_cmd='B';break;


		case 113:
		case 86:
		case 56:
		case 77:
		case 98:
		case 58: kod_cmd='N';break;

		default: return;
	}
	bit=(bd_osn[15]&0xe000)>>13;
	ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
	return;
}
//============================================================================
/*************************************************************************\
* prosto_komanda модуль формирования раздельных команд для доп.объектов   *
* command - байт управляющей команды от сервера                           *
* int bd_osn[16] - строка описания объекта управления в БД                *
\*************************************************************************/
void prosto_komanda(unsigned char command)
{
  unsigned char tums,//номер стойки
  gruppa,//код группы для данного объекта
  podgruppa, //код подгруппы для стрелки
  kod_cmd, //код команды для стрелки
  bit;    //номер бaйта для данного объекта
  int ii;
  if((bd_osn[0]<8)||(bd_osn[0]>250))
  {
    for(ii=0;ii<16;ii++)bd_osn[ii]=0;
    return;
  }
  tums=(bd_osn[13]&0xff00)>>8;
  gruppa='Q';
  podgruppa=bd_osn[15]&0xff;
  switch(command)
  {

    case 64: kod_cmd='A';break;
    case 65: kod_cmd='M';break;

    case 87: kod_cmd='B';break;
    case 88: kod_cmd='N';break;

    default: return;
  }
  bit=(bd_osn[15]&0xe000)>>13;
  ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
  return;
}
//======================================================================
/***************************************************\
*  Процедура выдачи команды на объект контроллера   *
*  ob_tums(unsigned char command,int obj_serv)      *
\***************************************************/
void ob_tums(unsigned char command)
{
	unsigned char tums,//номер стойки
	tip, //тип объекта контроллера
	gruppa,//код группы для данного объекта
	podgruppa, //код подгруппы для стрелки
	kod_cmd, //код команды для стрелки
	bit;    //номер бaйта для данного объекта
	int ii;
	tip=bd_osn[0]&0xf;
	if((tip!=9)&&(tip!=10))
	{for(ii=0;ii<16;ii++)bd_osn[ii]=0;return;}
	tums=(bd_osn[13]&0xff00)>>8;
	if(tip==9)gruppa='L';
	if(tip==10)gruppa='T';
	podgruppa=bd_osn[15]&0xff;
	switch(command)
  {

    case 97: kod_cmd='D';break;
    default: return;
  }
  bit=(bd_osn[15]&0xe000)>>13;
  ZAGRUZ_KOM_TUMS(tums,gruppa,podgruppa,bit,kod_cmd);
  return;
}
//=======================================================================
/**************************************************************\
* ZAGRUZ_KOM_TUMS - модуль загрузки раздельной команды в ТУМС  *
* tms - номер ТУМС                                             *
* grp - код группы                                             *
* pdgrp - код подгруппы                                        *
* kd_cmd - код команды                                         *
* bt -номер байта в сообщении                                  *
* int bd_osn[16] - строка описания объекта управления в БД     *
\*****************************************************а********/
void ZAGRUZ_KOM_TUMS(char tms,char grp,char pdgrp,char bt,char kd_cmd)
{
	int i,j;
  unsigned char adr_kom;
	for(i=0;i<15;i++)KOMANDA_ST[tms-1][i]=0;
  switch(tms)
  {
		case 1: adr_kom='G';break;
    case 2: adr_kom='K';break;
    case 3: adr_kom='M';break;
    case 4: adr_kom='N';break;
    case 5: adr_kom='S';break;
    case 6: adr_kom='U';break;
    case 7: adr_kom='V';break;
    case 8: adr_kom='Y';break;
    default:return;
  }
  KOMANDA_ST[tms-1][0]='(';
  KOMANDA_ST[tms-1][1]=adr_kom; //сформировать адрес
  KOMANDA_ST[tms-1][2]=grp;
  KOMANDA_ST[tms-1][3]=pdgrp;
  for(j=4;j<10;j++)KOMANDA_ST[tms-1][j]='|';
	KOMANDA_ST[tms-1][3+bt]=kd_cmd;
	KOMANDA_ST[tms-1][10]=check_summ(KOMANDA_ST[tms-1]);
	KOMANDA_ST[tms-1][11]=')';
  KOMANDA_ST[tms-1][12]=0;
	add(tms-1,9999,0);
	if(ACTIV!=1)
	{
		if(REGIM==COMMAND)
		{
			putch1(0x10,0xc,1,Y_KOM);
			if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
			else putch1(' ',0xc,1,46);
			for(i=0;i<12;i++)putch1(KOMANDA_ST[tms-1][i],0xc,55+i,Y_KOM);
			Y_KOM++;
			puts1("                                   ",0xa,3,Y_KOM);
			puts1("             ",0xa,55,Y_KOM);
			if(Y_KOM>=47)Y_KOM=3;
		}
		for(i=0;i<15;i++)KOMANDA_ST[tms-1-1][i]=0;
	}
	SHET_KOM[tms-1]=0;
  return;
}
//=============================================================================
/*****************************************************************\
* KOM - код маршрутной команды полученный из АРМа                 *
* ANALIZ_MARSH - модуль анализа и формирования маршрутных команд  *
* NACH - объект сервера для начала маршрута                       *
* END  - объект сервера для конца  маршрута                       *
* Nst - число противошерстных стрелок в трассе маршрута           *
* POL - битовая строка требуемого положения стрелок для маршрута  *
\*****************************************************************/
int ANALIZ_MARSH(int KOM,int NACH,int END,int Nstrel,unsigned long POL)
{
	unsigned char PROHOD,PROHOD1;
	int ERROR_MARSH=0;
	int shag=0,
	ind_str[Nst],
	ind_sp[Nst],
	ind_sig[Nst],
	ii=0,
	jj=0,
	tek_strel=0,
  stroka_tek,
	stroka_pred,
  i_n,
  n_sig=0,
  tums;

	unsigned int kod_beg,kod_end;
	PROHOD=0;
	PROHOD1=0;
	//очистить массив трассы
	ZERO_TRASSA();
	//сбросить индексы сигналов, стрелок и СП_УП
	for(ii=0;ii<Nst;ii++){ind_sig[ii]=0;ind_str[ii]=0;ind_sp[ii]=0;}
	KOM=KOM&0xFF;//получить байт команды
	//сформировать код команды маршрута по байту команды
	switch(KOM)
	{
		case 188:
		case 191: kod_marsh='a';break; //маневровый

		case 189:
		case 192: kod_marsh='b'; break; //поездной

		case 71: kod_marsh='d';break;  //колонка
		default: kod_marsh=0;ERROR_MARSH=1005; break; //непредусмотренный
	}
	if(ERROR_MARSH>1000)return(ERROR_MARSH);
	//прочитать базу данных для начала маршрута
	READ_BD(NACH);
	if((bd_osn[13]&0x00ff)==1)//если первый район управления
	{
		if(bd_osn[1]==0)shag=-1;//если четный сигнал
		else
			if(bd_osn[1]==1)shag=1;//если сигнал нечетный
			else return(1001);
		//если начало не от сигнала
		if(bd_osn[0]!=2)ERROR_MARSH=1001;
		else
		//если маневровый маршрут, а сигнал не имеет маневрового показания
		if(((KOM==191)||(KOM==188)||(KOM==71))&&(bd_osn[6]==1))ERROR_MARSH=1002;
		else
		//если поездной, а сигнал не имеет поездного показания
		if(((KOM==192)||(KOM==189))&&(bd_osn[6]==0))ERROR_MARSH=1003;
		else
		{
			ii=NACH;
			//заполнить номер стойки
			TRASSA[jj].stoyka=(bd_osn[13]&0x0f00)>>8;
			//получить строку топологии
			stroka_tek=(bd_osn[14]&0xFF00)>>8;
			stroka_pred=stroka_tek;
			//начать с 0-ой строки глобальных маршрутов
			i_n=0;
			//сканировать глобальную таблицу на совпадение маршрутов
			while((MARSHRUT_ALL[i_n].NACH!=NACH)||(MARSHRUT_ALL[i_n].END!=END)||
			(MARSHRUT_ALL[i_n].NSTR!=Nstrel)||(MARSHRUT_ALL[i_n].POL_STR!=POL))
			{
				i_n++;
				if(i_n>=Nst*3)break;
			}
			//если точно такого маршрута нет в таблице
			if(i_n>=Nst*3)
			{
				i_n=0;
				//сканировать на наличие другого маршрута от этого сигнала
				while(MARSHRUT_ALL[i_n].NACH!=NACH)
				{
					i_n++;
					if(i_n>=(Nst*3))break;
				}
				if(i_n<Nst*3)
				{
					return(1050);//если от сигнала уже задаётся маршрут
				}
				else
				{
					i_n=0;

					//сканировать таблицу на занятость
					while(MARSHRUT_ALL[i_n].NACH!=0){i_n++;if(i_n>=(Nst*3))break;}
					if(i_n>=Nst*3)ERROR_MARSH=1004; //если вся таблица занята
				}
			}
			else
			{
				if(POVTOR_OTKR!=0)DeleteMarsh(i_n);
				else return(25000);
			}

			if(ERROR_MARSH==0)// если все в порядке - внести маршрут в таблицу
			{
				MARSHRUT_ALL[i_n].KMND=kod_marsh;
				MARSHRUT_ALL[i_n].NACH=NACH;
				MARSHRUT_ALL[i_n].END=END;
				MARSHRUT_ALL[i_n].NSTR=Nstrel;
				MARSHRUT_ALL[i_n].POL_STR=POL;
				add(NACH,7777,END);
				for(ii=0;ii<Nst;ii++)MARSHRUT_ALL[i_n].KOL_STR[ii]=0;
			}
			ii=NACH;
			if(ERROR_MARSH<1000)ERROR_MARSH=i_n;
			if(ERROR_MARSH<1000)

			while(jj<200)//двигаться в пределах массива TRASSA
			{
				READ_BD(ii);//читаем базу очередного объекта топологии
				read_FR3(ii);//читаем текущее состояние очередного объекта топологии
				stroka_tek=(bd_osn[14]&0xFF00)>>8;//получить текущую базовую строку
				TRASSA[jj].stoyka=(bd_osn[13]&0x0f00)>>8;//запомнить стойку
				//если объект в  топологии
				if((bd_osn[0]<7)&&(bd_osn[0]>0))tums=TRASSA[jj].stoyka-1;
				TRASSA[jj].tip=bd_osn[0]&0xff;

				switch(bd_osn[0])//далее двигаться в зависимости от объекта
				{
					//если стрелка
					case 1:
						read_FR3(ii);
						if(FR3[9]!=0)return(2000+ii);//стрелка потеряла контроль
						if(FR3[11]!=0)return(3000+ii);//стрелка непарафазна
						//занести стрелку в список стрелок маршрута
						if(((bd_osn[7]==0)&&(shag==-1))|| //если стрелка противошерстная
						((bd_osn[7]==1)&&(shag==1)))
						{
							if((bd_osn[8]&0x4000)==0x4000)//если стрелка не передается из АРМ
							{
								if((bd_osn[8]&0x8000)==0)//если сброс по минусу
								{
//$$1
							 MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
									//записать стрелку с признаком сброса и положения "+"
									TRASSA[jj].object=ii|0x4000;
									read_FR3(ii);
									//если стрелка не в плюсе
									if((FR3[1]!=1)||(FR3[3]!=0))
									{
										//увеличить число переводимых стрелок
										MARSHRUT_ALL[i_n].KOL_STR[tums]++;
										//выделить состояние маршрута
										MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
										//установить состояние нового маршрута
										MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
									}
									ii=ii+shag; //пройти к следующему объекту по строке базы
								}
								//если сброс по плюсу
								else
								{
//$$2
									MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
									//записать стрелку с признаком сброса и положения "-"
									TRASSA[jj].object=ii|0xC000;
									//если стрелка не в минусе
									if((FR3[1]!=0)||(FR3[3]!=1))
									{
										MARSHRUT_ALL[i_n].KOL_STR[tums]++;
										//выделить состояние
										MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
										//установить состояние нового маршрута
										MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
									}
									ii=bd_osn[2]; //пройти к следующему объекту по ссылке
								}

								if((bd_osn[8]&0x2000)==0x2000)//если нет передачи ТУМС
								{
									//установить в трассу признак отключения передачи
									TRASSA[jj].object=ii|0x2000;
								}
								break;
							}
							//если трасса не совпадает
							if(tek_strel>Nstrel){ERROR_MARSH=1006;break;}

							if((POL&(1<<tek_strel))==0)//если данная стрелка нужна в плюсе
							{
//$$3
								MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
								//записать стрелку в трассу с признаком "+"
								TRASSA[jj].object=ii|0x4000;
								//если стрелка не в плюсе
								if((FR3[1]!=1)||(FR3[3]!=0))
								{
									//увеличить число переводимых стрелок
									MARSHRUT_ALL[i_n].KOL_STR[tums]++;
									//выделить состояние
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
									//установить состояние нового маршрута
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
								}
								ii=ii+shag;//следующий объект по строке топологии
							}
							//если данная стрелка нужна в минусе
							else
							{
//$$4
								MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
								//записать стрелку в трассу с "-"
								TRASSA[jj].object=ii|0xC000;
								//если стрелка не в минусе
								if((FR3[1]!=0)||(FR3[3]!=1))
								{
									//увеличить число переводимых стрелок
									MARSHRUT_ALL[i_n].KOL_STR[tums]++;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
								}
								ii=bd_osn[2];//осуществить переход по минусу стрелки
							}
							tek_strel++;
						}
						else
						if(((bd_osn[7]==1)&&(shag==-1))|| //если стрелка пошерстная
						((bd_osn[7]==0)&&(shag==1)))
						{ //если пред.объект в другой строке записать с признаком "-"
							if(stroka_tek!=stroka_pred)
							{
//$$5
								MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
								//установить признак минусового положения
								TRASSA[jj].object=ii|0x8000;
								//если стрелка не в минусе
								if((FR3[1]!=0)||(FR3[3]!=1))
								{ //увеличить число переводимых стрелок
									MARSHRUT_ALL[i_n].KOL_STR[tums]++;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|0x3;
								}
							}
							//если стрелка нужна в плюсе
							else
							{
//$$6
								MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
								TRASSA[jj].object=ii;//записать стрелку в трассу с "+"
								//если стрелка не в плюсе
								if((FR3[1]!=1)||(FR3[3]!=0))
								{
									MARSHRUT_ALL[i_n].KOL_STR[tums]++;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
									MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
								}
							}
							ii=ii+shag;//двигаться по строке топологии
						}
						break;

					//обработка базового перехода
					case 6:
						TRASSA[jj].object=ii;
						//если движемся в нечетном и переход нечетный
						if((shag==1)&&(bd_osn[3]==1))
						{
							ii=bd_osn[1];
							if(bd_osn[2])
							{
								shag=-shag; //если инверсный переход
								ii=ii+shag;
							}

						}
						else
							{ //если движемся в четном и переход четный
								if((shag==-1)&&(bd_osn[3]==0))
								{
									ii=bd_osn[1];
									if(bd_osn[2])
									{
										shag=-shag; //если инверсный переход
										ii=ii+shag;
									}
								}
								else
								{ //если встрeчный переход
									ii=ii+shag;
								}
							}

						break;
					//обработка доп.зависимости
					case 7:
						TRASSA[jj].object=ii;
//						if(bd_osn[1]==15) //если проверка стрелки в пути
//						{
//							ERROR_MARSH=ERROR_MARSH+ANALIZ_ST_IN_PUT(jj,kod_marsh,tums,i_n,ind_str[tums]);
//						}
//						if(bd_osn[1]==14)ERROR_MARSH=ERROR_MARSH+tst_str_ohr();
						ii=ii+shag;
						break;

					default:
						TRASSA[jj].object=ii;
						//если СП или УП - записать объект в список
						if((bd_osn[0]==3)||(bd_osn[0]==4))
						{
							read_FR3(ii);
//							if(FR3[11]!=0)ERROR_MARSH=3000+ii;//если СП или УП непарафазны
//							if(((KOM!=191)&&(KOM!=188)&&(KOM!=71))||(bd_osn[0]==3))
//							{
//								if(FR3[1]!=0)ERROR_MARSH=4000+ii;//если СП или УП занят
//							}
//							if(FR3[3]!=0)ERROR_MARSH=5000+ii;//если СП или УП замкнут
//							if(FR3[5]!=0)ERROR_MARSH=6000+ii;//если СП или УП в разделке
//							if(FR3[7]!=0)ERROR_MARSH=7000+ii;//если в работе МСП
							if(ERROR_MARSH>1000)
							return(ERROR_MARSH);
							MARSHRUT_ALL[i_n].SP_UP[tums][ind_sp[tums]++]=ii;
						}
						if(bd_osn[0]==2)//если объект - сигнал
						{
//							if(bd_osn[11]!=43690)
//							{
								read_FR3(ii);//прочитать текущее состояние сигнала
//								if(FR3[11]!=0)return(3000+ii);//если сигнал непарафазен
								//если направление маршрута - нечетное,а сигнал четный
								//или направление маршрута - четное,а сигнал нечетный
//								if(((shag==1)&&(bd_osn[1]==0))||
//								((shag==-1)&&(bd_osn[1]==1)))
//								if((FR3[1]!=0)||(FR3[3]!=0))return(8000+ii); //если сигнал открыт
//							}
							switch(shag)
							{ //нечетное направление
								case 1:
									switch(kod_marsh)
									{ //маневровый
										case 'a':
										case 'd':	kod_beg=256; kod_end=1; break;

										 //поездной
										case 'b':	kod_beg=4096;kod_end=16;break;

										default:	ERROR_MARSH=1010;break; //чужой код команды
									}
									break;

								//четное направление
								case -1:
									switch(kod_marsh)
									{ //маневровый
										case 'a':
										case 'd':	kod_beg=1024; kod_end=4; break;

										//поездной
										case 'b': kod_beg=16384;kod_end=64;break;

										default: ERROR_MARSH=1010;break; //чужой код команды
									}
									break;

								default: ERROR_MARSH=1010;break;
							}
							if(ERROR_MARSH>1000)break;
							if((bd_osn[11]&kod_beg)==kod_beg)//если есть такое начало
							{
								TRASSA[jj].object=TRASSA[jj].object|0x8000;
								MARSHRUT_ALL[i_n].SIG[tums][ind_sig[tums]++]=ii;
							}
							if((bd_osn[11]&kod_end)==kod_end)//если есть такой конец
							//установить на трассовый объект сигнала признак наличия конца
							TRASSA[jj].object=TRASSA[jj].object|0x4000;

							//если у сигнала есть поездное показание
							if(bd_osn[6]!=0)
								//на трассовый объект сигнала признак наличия поездного сигнала
								TRASSA[jj].object=TRASSA[jj].object|0x2000;

							//взять код подгруппы
							TRASSA[jj].podgrup=bd_osn[15]&0x00ff;
							//взять код номера байта для команды
							TRASSA[jj].kod_bit=(((bd_osn[15]&0xe000)>>13)-1)|0x40;

							//если второй проход и сигнал может быть концом, то закончить
							if((PROHOD==1)&&((bd_osn[11]&kod_end)==kod_end))
							{
								if((bd_osn[1]==0)&&(shag==-1))return(ERROR_MARSH);
								if((bd_osn[1]==1)&&(shag==1))return(ERROR_MARSH);
							}
						}
						ii=ii+shag;
						break;
				}//конец переключений по типу объекта базы

				if(ERROR_MARSH>1000)break; //если поймана ошибка - выход

				stroka_pred=stroka_tek; //текущую строку перевести в ранг предыдущей

				//если не совпадают трассы
				if(tek_strel>Nstrel){ERROR_MARSH=1006;break;}

				if(ii==END)PROHOD=1;//если достигнут конец маршрута - установить признак

				//если конец был достигнут и вышли на путь или УП
				if((PROHOD==1)&&((TRASSA[jj].tip==5)||(TRASSA[jj].tip==4)))
				{
					jj++; //пройти далльше
					READ_BD(ii);
					TRASSA[jj].object=ii;
					stroka_tek=(bd_osn[14]&0xFF00)>>8;//получить текущую базовую строку
					TRASSA[jj].stoyka=(bd_osn[13]&0x0f00)>>8;//запомнить стойку
					TRASSA[jj].tip=bd_osn[0]&0xff;
					if(ERROR_MARSH>1000)break;
					if(bd_osn[0]==2)//если объект - сигнал
					{
						switch(shag)
						{ //нечетное направление
							case 1:
								switch(kod_marsh)
								{ //маневровый
									case 'a':
									case 'd':	kod_beg=256; kod_end=1; break;
									//поездной
									case 'b':	kod_beg=4096;kod_end=16;break;
									default:	ERROR_MARSH=1010;	break;
								}
								break;

							//четное направление
							case -1:
								switch(kod_marsh)
								{
									//маневровый
									case 'a':
									case 'd':	kod_beg=1024; kod_end=4; break;
									//поездной
									case 'b':	kod_beg=16384; kod_end=64; break;
									default:	ERROR_MARSH=1010; break;
								}
								break;
							default: ERROR_MARSH=1010;break;
						}
						if(ERROR_MARSH>1000)break;
						//если для сигнала есть такое начало
						if((bd_osn[11]&kod_beg)==kod_beg)
						{ //записать в трассовый объект признак наличия начала
							TRASSA[jj].object=TRASSA[jj].object|0x8000;
							//добавить сигнал в список сигналов
							n_sig=ind_sig[tums]++;
							MARSHRUT_ALL[i_n].SIG[tums][n_sig]=TRASSA[jj].object&0xfff;
						}
						if((bd_osn[11]&kod_end)==kod_end)//если есть такой конец
							//записать в трассовый объект признак наличия конца
							TRASSA[jj].object=TRASSA[jj].object|0x4000;
						//взять код подгруппы
						TRASSA[jj].podgrup=bd_osn[15]&0x00ff;
						//взять код номера байта для команды
						TRASSA[jj].kod_bit=(((bd_osn[15]&0xe000)>>13)-1)|0x40;
						//если достигался конец и может быть конец
						if((PROHOD==1)&&((bd_osn[11]&kod_end)==kod_end))
						{
							if((bd_osn[1]==0)&&(shag==-1))return(ERROR_MARSH);
							if((bd_osn[1]==1)&&(shag==1))return(ERROR_MARSH);
						}
					} //конец анализа сигнала
					break;

				}//конец анализа объекта за конечным сигналом

				jj++;
				if(ERROR_MARSH>1000)break;
				if(jj>=200)break;
			}
		}
		if(ERROR_MARSH>1000)ZERO_TRASSA();
		return(ERROR_MARSH);
	}
// далее все аналогично для второго района управления
R2:

	if((bd_osn[13]&0x00ff)==2)//если второй район управления
	{
		ZERO_TRASSA1();
		KOM=KOM&0xFF;
		switch(KOM)
		{
			case 188:
			case 191: kod_marsh1='a'; break;  //маневровый
			case 189:
			case 192:	kod_marsh1='b';break;   //поездной
			case 71:	kod_marsh1='d'; break;  //колонка
			default:	kod_marsh1=0;	ERROR_MARSH=1005; //недопустимая команда
		}
		READ_BD(NACH);
		if(bd_osn[1]==0)shag=-1;//если четный сигнал
		else
			if(bd_osn[1]==1)shag=1;//если сигнал нечетный
			else return(1001);

		if(bd_osn[0]!=2)ERROR_MARSH=1001;
		else
			if(((KOM==191)||(KOM==188)||(KOM==71))&&(bd_osn[6]==1))ERROR_MARSH=1002;
			else
				if(((KOM==192)||(KOM==189))&&(bd_osn[6]==0))ERROR_MARSH=1003;
				else
				{
					ii=NACH;
					TRASSA1[jj].stoyka=(bd_osn[13]&0x0f00)>>8;
					stroka_tek=(bd_osn[14]&0xFF00)>>8;
					stroka_pred=stroka_tek;
					i_n=0;
					//сканировать глобальную таблицу на совпадение маршрутов
					while((MARSHRUT_ALL[i_n].NACH!=NACH)||(MARSHRUT_ALL[i_n].END!=END)||
					(MARSHRUT_ALL[i_n].NSTR!=Nstrel)||(MARSHRUT_ALL[i_n].POL_STR!=POL))
					{
						i_n++;
						if(i_n>=Nst*3)break;
					}
					//если точно такого маршрута нет в таблице
					if(i_n>=Nst*3)
					{
						i_n=0;
						//сканировать на наличие другого маршрута от этого сигнала
						while(MARSHRUT_ALL[i_n].NACH!=NACH)
						{
							i_n++;
							if(i_n>=(Nst*3))break;
						}
						if(i_n<Nst*3)return(1050); //если от этого сигнала уже задаётся маршрут
						else
						{
							i_n=0;
							//сканировать таблицу на занятость
							while(MARSHRUT_ALL[i_n].NACH!=0){i_n++;if(i_n>=(Nst*3))break;}
							if(i_n>=Nst*3)ERROR_MARSH=1004; //если вся таблица занята
						}
					}
					else
					if(POVTOR_OTKR!=0)DeleteMarsh(i_n);
					else return(25000);
					if(ERROR_MARSH==0)
					{
						MARSHRUT_ALL[i_n].KMND=kod_marsh1;
						MARSHRUT_ALL[i_n].NACH=NACH;
						MARSHRUT_ALL[i_n].END=END;
						MARSHRUT_ALL[i_n].NSTR=Nstrel;
						MARSHRUT_ALL[i_n].POL_STR=POL;
						add(NACH,7777,END);
						for(ii=0;ii<Nst;ii++)MARSHRUT_ALL[i_n].KOL_STR[ii]=0;
					}
					ii=NACH;
					if(ERROR_MARSH<1000)ERROR_MARSH=i_n;
					if(ERROR_MARSH<1000)
					while(jj<200)
					{
						READ_BD(ii);
						stroka_tek=(bd_osn[14]&0xFF00)>>8;//получить текущую базовую строку
						TRASSA1[jj].stoyka=(bd_osn[13]&0x0f00)>>8;//запомнить стойку
						tums=TRASSA1[jj].stoyka-1;
						TRASSA1[jj].tip=bd_osn[0]&0xff;
						switch(bd_osn[0])//далее двигаться в зависимости от объекта
						{
							//если стрелка
							case 1:
									read_FR3(ii);
//								if(FR3[9]!=0)return(2000+ii);//стрелка потеряла контроль
//								if(FR3[11]!=0)return(3000+ii);//стрелка непарафазна
//$$0
								if(((bd_osn[7]==0)&&(shag==-1))|| //если стрелка противошерстная
								((bd_osn[7]==1)&&(shag==1)))
								{
									if((bd_osn[8]&0x4000)==0x4000)//если стрелка не передается из АРМ
									{
										if((bd_osn[8]&8000)==0)//если сброс по минусу
										{
//$$1
											MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
											TRASSA1[jj].object=ii|0x4000;
											read_FR3(ii);
											//если стрелка не в плюсе
											if((FR3[1]!=1)||(FR3[3]!=0))
											{
												MARSHRUT_ALL[i_n].KOL_STR[tums]++;
												MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
												MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
											}
											ii=ii+shag;
										}
										else  //если сброс по плюсу
										{
//$$2
											MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
											TRASSA[jj].object=ii|0xC000;
											read_FR3(ii);
											//если стрелка не в минусе
											if((FR3[1]!=0)||(FR3[3]!=1))
											{
												MARSHRUT_ALL[i_n].KOL_STR[tums]++;
												MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
												MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
											}
											ii=bd_osn[2];
										}
										if((bd_osn[8]&0x2000)==0x2000)//если нет передачи ТУМС
										{
											TRASSA1[jj].object=ii|0x2000;
										}
										break;
									}
									if(tek_strel>Nstrel){ERROR_MARSH=1006;break;}

									if((POL&(1<<tek_strel))==0)//если данная стрелка нужна в плюсе
									{
//$$3
										MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
										//записать стрелку в трассу с признаком "+"
										TRASSA1[jj].object=ii|0x4000;
										if((FR3[1]!=1)||(FR3[3]!=0))//если стрелка не в плюсе
										{
											MARSHRUT_ALL[i_n].KOL_STR[tums]++;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
										}
										ii=ii+shag;
									}
									//если стрелка нужна в минусе
									else
									{
//$$4
										MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
										TRASSA1[jj].object=ii|0xC000;//записать стрелку в трассу с "-"
										if((FR3[1]!=0)||(FR3[3]!=1))//если стрелка не в минусе
										{
											MARSHRUT_ALL[i_n].KOL_STR[tums]++;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
										}
										ii=bd_osn[2];//осуществить переход по минусу стрелки
									}
									tek_strel++;
								}
								else
								if(((bd_osn[7]==1)&&(shag==-1))|| //если стрелка пошерстная
								((bd_osn[7]==0)&&(shag==1)))
								{ //если предыдущий объект в другой строке
									//записать с признаком "-"
									if(stroka_tek!=stroka_pred)
									{
//$$5
										MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii|0x1000;
										TRASSA1[jj].object=ii|0x8000;  //записать с признаком "-"
										if((FR3[1]!=0)||(FR3[3]!=1)) //если стрелка не в минусе
										{
											MARSHRUT_ALL[i_n].KOL_STR[tums]++;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
										}

									}
									else//если стрелка нужна в плюсе
									{
//$$6
										MARSHRUT_ALL[i_n].STREL[tums][ind_str[tums]++]=ii;
										TRASSA1[jj].object=ii;//записать стрелку в трассу с "+"
										if((FR3[1]!=1)||(FR3[3]!=0)) //если стрелка не в плюсе
										{
											MARSHRUT_ALL[i_n].KOL_STR[tums]++;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST&0xC0;
											MARSHRUT_ALL[i_n].SOST=MARSHRUT_ALL[i_n].SOST|3;
										}
									}
									ii=ii+shag;//двигаться по строке топологии
								}
								break;

							case 6: TRASSA1[jj].object=ii;
											//если направление движения нечетное и направление
											//перехода тоже нечетное
											if((shag==1)&&(bd_osn[3]==1))
											{
												ii=bd_osn[1];
												if(bd_osn[2])//если переход инверсный
												{
													shag=-shag;
													ii=ii+shag;
												}
											}
											else
											{
												if((shag==-1)&&(bd_osn[3]==0))
												{
													ii=bd_osn[1];
													if(bd_osn[2])
													{
														shag=-shag;
														ii=ii+shag;
													}
												}
												else ii=ii+shag;
											}
											break;


							case 7:	TRASSA1[jj].object=ii;
//											if(bd_osn[1]==15)
//											{
//												ERROR_MARSH=ERROR_MARSH+ANALIZ_ST_IN_PUT(jj,kod_marsh,tums,i_n,ind_str[tums]);
//											}
//											if(bd_osn[1]==14)
//											{
//												ERROR_MARSH=ERROR_MARSH+tst_str_ohr();
//											}
											ii=ii+shag;
											break;
						 default: TRASSA1[jj].object=ii;
											if((bd_osn[0]==3)||(bd_osn[0]==4))
											{
//												read_FR3(ii);
//												if(FR3[11]!=0)ERROR_MARSH=3000+ii;//если СП или УП непарафазны
//												if(((KOM!=191)&&(KOM!=188)&&(KOM!=71))||(bd_osn[0]==3))
//												{
//													if(FR3[1]!=0)ERROR_MARSH=4000+ii;//если СП или УП занят
//												}
//												if(FR3[3]!=0)ERROR_MARSH=5000+ii;//если СП или УП замкнут
//												if(FR3[5]!=0)ERROR_MARSH=6000+ii;//если СП или УП в разделке
//												if(FR3[7]!=0)ERROR_MARSH=8000+ii;//если в работе МСП
												if(ERROR_MARSH>1000)return(ERROR_MARSH);

												MARSHRUT_ALL[i_n].SP_UP[tums][ind_sp[tums]++]=ii;
											}
											//если объект - сигнал
											if(bd_osn[0]==2)
											{ //если не смена направления
//												if(bd_osn[11]!=43690)
//												{
 //													read_FR3(ii);//прочитать текущее состояние сигнала
 //													if(FR3[11]!=0)return(3000+ii);//если сигнал непарафазен
//													if(((shag==1)&&(bd_osn[1]==0))||
//													((shag==-1)&&(bd_osn[1]==1)))
//													if((FR3[1]!=0)||(FR3[3]!=0))return(8000+ii); //если сигнал открыт
//												}
												switch(shag)
												{ //нечетное направление
													case 1: switch(kod_marsh1)
																	{ //маневровый
																		case 'a':
																		case 'd':
																							kod_beg=256;
																							kod_end=1;
																							break;
																		//поездной
																		case 'b': kod_beg=4096;
																							kod_end=16;
																							break;
																		default:  ERROR_MARSH=1010;break;

																	}
																	break;
													//четное направление
													case -1:  switch(kod_marsh1)
																		{ //маневровый
																			case 'a':
																			case 'd':
																								kod_beg=1024;
																								kod_end=4;
																								break;
																			//поездной
																			case 'b': kod_beg=16384;
																								kod_end=64;
																								break;
																			default:  ERROR_MARSH=1010;break;
																		}
																		break;
													 default: ERROR_MARSH=1010;break;
												}
												if(ERROR_MARSH>1000)break;
												if((bd_osn[11]&kod_beg)==kod_beg)//если есть такое начало
												{
													TRASSA1[jj].object=TRASSA1[jj].object|0x8000;
													MARSHRUT_ALL[i_n].SIG[tums][ind_sig[tums]++]=ii;
												}
												if((bd_osn[11]&kod_end)==kod_end)//если есть такой конец
												TRASSA1[jj].object=TRASSA1[jj].object|0x4000;
												 //если у сигнала есть поездное показание
												if(bd_osn[6]!=0)
												TRASSA1[jj].object=TRASSA1[jj].object|0x2000;
																				//взять код подгруппы
												TRASSA1[jj].podgrup=bd_osn[15]&0x00ff;
												//взять код битовой команды
												TRASSA1[jj].kod_bit=(((bd_osn[15]&0xe000)>>13)-1)|0x40;
												if((PROHOD1==1)&&((bd_osn[11]&kod_end)==kod_end))
												{
													if((bd_osn[1]==0)&&(shag==-1))return(ERROR_MARSH);
													if((bd_osn[1]==1)&&(shag==1))return(ERROR_MARSH);
												}
											}
											ii=ii+shag;
											break;
						}//конец переключателя по типу объекта базы
						if(ERROR_MARSH>1000)break;
						stroka_pred=stroka_tek;
						if(tek_strel>Nstrel){ERROR_MARSH=1006;break;} //$$$$$$######
						if(ii==END)PROHOD1=1;
						if((PROHOD1==1)&&((TRASSA1[jj].tip==5)||(TRASSA1[jj].tip==4)))
						{
							jj++;
							READ_BD(ii);
							TRASSA1[jj].object=ii;
							stroka_tek=(bd_osn[14]&0xFF00)>>8;//получить текущую базовую строку
							TRASSA1[jj].stoyka=(bd_osn[13]&0x0f00)>>8;//запомнить стойку
							TRASSA1[jj].tip=bd_osn[0]&0xff;
							if(ERROR_MARSH>1000)break;

							if(bd_osn[0]==2)//если объект - сигнал
							{
								switch(shag)
								{ //нечетное направление
									case 1: switch(kod_marsh1)
													{ //маневровый
														case 'a':
														case 'd':
																			kod_beg=256;
																			kod_end=1;
																			break;
														//поездной
														case 'b': kod_beg=4096;
																			kod_end=16;
																			break;
														default:  ERROR_MARSH=1010;break;
													}
													break;
									//четное направление
									case -1:  switch(kod_marsh1)
														{ //маневровый
															case 'a':
															case 'd':
																				kod_beg=1024;
																				kod_end=4;
																				break;
															//поездной
															case 'b': kod_beg=16384;
																				kod_end=64;
																				break;
															default: ERROR_MARSH=1010;break;
														}
														break;
									default:  ERROR_MARSH=1010;break;
								}
								if(ERROR_MARSH>1000)break;
								if((bd_osn[11]&kod_beg)==kod_beg)//если есть такое начало
								{
									TRASSA1[jj].object=TRASSA1[jj].object|0x8000;
									n_sig=ind_sig[tums]++;
									MARSHRUT_ALL[i_n].SIG[tums][n_sig]=TRASSA[jj].object&0xfff;
								}
								if((bd_osn[11]&kod_end)==kod_end)//если есть такой конец
								TRASSA1[jj].object=TRASSA1[jj].object|0x4000;
								//взять код подгруппы
								TRASSA1[jj].podgrup=bd_osn[15]&0x00ff;
								//взять код битовой команды
								TRASSA1[jj].kod_bit=(((bd_osn[15]&0xe000)>>13)-1)|0x40;
								if((PROHOD1==1)&&((bd_osn[11]&kod_end)==kod_end))
								{
									if((bd_osn[1]==0)&&(shag==-1))return(ERROR_MARSH);
									if((bd_osn[1]==1)&&(shag==1))return(ERROR_MARSH);
								}
							}
							break;
						}
						jj++;
						if(ERROR_MARSH>1000)break;
						if(jj>=200)break;
					}
					if(ERROR_MARSH>1000)ZERO_TRASSA1();
		}
		return(ERROR_MARSH);
	}
	return(ERROR_MARSH);
}
//=============================================================================
/*******************************************************************\
*   TUMS_MARSH(i_m) - Модуль подготовки  маршрутных команд для ТУМС *
*  i_m - номер строки глобальных маршрутов с новым маршрутом        *
\*******************************************************************/
void TUMS_MARSH(int i_m	)
{
	int ii=0,i_s=0,s_m,last_end,first_beg,jj,test,sp_in_put,spar_sp,sosed;
	unsigned char tums_tek,tums_pred,adr_kom;
	unsigned char n_strel,kmnd,perevod_str;
	unsigned int objk,objk_next,fiktiv=0,nachalo=0,END;
	unsigned long pol_strel=0l;
	test=0;
	//если набрана трасса первого района
	if(TRASSA[0].object)
	{
		//если в трассе и строке разные маршруты
		if(MARSHRUT_ALL[i_m].NACH!=(TRASSA[0].object&0xFFF))
		{
			ZERO_TRASSA();
			return;
		}
		else
		{
			//получить команду и конец
			kmnd=MARSHRUT_ALL[i_m].KMND;
			END=MARSHRUT_ALL[i_m].END;
		}
		ii=0; //начать с первого
		last_end=-1,first_beg=-1;//установить начало поиска

		tums_pred=TRASSA[0].stoyka;
		tums_tek=tums_pred;            //взять ТУМС

		//считаем число стрелок, которые надо перевести
		for(i_s=0;i_s<Nst;i_s++)test=test+MARSHRUT_ALL[i_m].KOL_STR[i_s];

		//устанавливаем признак перевода стрелок
		if(test==0)perevod_str=0;
		else perevod_str=0xf;

		while(ii<200)//проход по всей трассе
		{
			//если сигнал или конец данных
			if((TRASSA[ii].tip==2)||(TRASSA[ii].tip==0))
			{
				tums_tek=TRASSA[ii].stoyka;
				if(tums_tek!=tums_pred)//переход в другую стойку
				{
					if((first_beg&0x8000)==0x8000)//если реального начала нет
					{
						last_end=-1;first_beg=-1;tums_tek=tums_pred; //сброс начала и конца
					}
				}

				if(tums_tek!=tums_pred)//переход в другую стойку
				{
					//если есть реальное начало
					if((last_end!=-1)&&(first_beg!=last_end))//если есть конец и начало
					{
						i_s=tums_pred-1;
						//фиксируем вхождение стойки i_s в маршрут
						MARSHRUT_ALL[i_m].STOYKA[i_s]=1;

						if(perevod_str!=0) //если маршрут требует перевода стрелок
						{
							MARSHRUT_ALL[i_m].SOST=0x43;// маршрут предварительный
							if(MARSHRUT_ALL[i_m].KOL_STR[i_s]!=0) //если в этой стойке перевод
							{
								MARSHRUT_ALL[i_m].STOYKA[i_s]=2; //фиксируем вхождение с переводом
							}
							else goto NEXT; //если в данной стойке нет перевода -  идти далее
						}
						else //если стрелки стоят по маршруту
						{
							MARSHRUT_ALL[i_m].SOST=0x83; //маршрут исполнительный
						}
						for(s_m=0;s_m<MARS_STOY;s_m++) //пройти по строчкам локальной таблицы
						//если маршрут в работе - прервать
						if((MARSHRUT_ST[i_s][s_m].NUM-100)==i_m)break;

						if(s_m>=MARS_STOY)//если этот маршрут не в работе
							//найти свободное место
							for(s_m=0;s_m<MARS_STOY;s_m++)
							if(MARSHRUT_ST[i_s][s_m].NUM==0)break; //если чисто - прервать

						//если нет свободной структуры для локального задания - завершить

						if(s_m>=MARS_STOY)goto out;

						//если строка свободна - начинаем писать команду
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[0]='(';
						//сформировать адрес
						switch(tums_pred)
						{
							case 1: adr_kom='G';break;
							case 2: adr_kom='K';break;
							case 3: adr_kom='M';break;
							case 4: adr_kom='N';break;
							case 5: adr_kom='S';break;
							case 6: adr_kom='U';break;
							case 7: adr_kom='V';break;
							case 8: adr_kom='Y';break;
							default:return;
						}
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[1]=adr_kom;
						//готовим предварительную или исполнительную команду
						if(perevod_str==0)MARSHRUT_ST[i_s][s_m].NEXT_KOM[2]=kmnd;
						else MARSHRUT_ST[i_s][s_m].NEXT_KOM[2]=kmnd|8;
						//заполняем описание для начала маршрута
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]=TRASSA[first_beg].podgrup;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[4]=TRASSA[first_beg].kod_bit;
						//заполняем описание для конца маршрута
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[5]=TRASSA[last_end].podgrup;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[6]=TRASSA[last_end].kod_bit;
						//готовим число и положение стрелок
						n_strel=0;
						pol_strel=0;
						for(jj=first_beg;jj<=last_end;jj++)
						{
							if(TRASSA[jj].tip==1)//если стрелка
							{
								if((TRASSA[jj].object&0x4000)==0x4000)//если противошерстная
								{
									if((TRASSA[jj].object&0x2000)!=0x2000) //если нет глушения
									{
										if((TRASSA[jj].object&0x8000)==0x8000)//если в минусе
										{
											pol_strel=pol_strel|(1<<n_strel);
										}
										n_strel++;
									}
								}
							}
						}
						//заполняем число стрелок
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[7]=0x40|n_strel;
						//заполняем положение для первых 6 стрелок
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[8]=0x40|(pol_strel&0x3f);
						//заполняем положение для вторых 6 стрелок
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[9]=0x40|((pol_strel&0xFC0)>>6);
						//считаем контрольную сумму
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[10]=0;
						//закрываем пакет команды
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[11]=')';
						KOL_VYD_MARSH[i_s]==0;				//$$$$ 13_04_07 сброс счетчика числа повторов выдачи маршрута
						add(i_s,6666,s_m);

						//устанавливаем состояние локального маршрута
						if(perevod_str!=0)MARSHRUT_ST[i_s][s_m].SOST=0x47;
						else MARSHRUT_ST[i_s][s_m].SOST=0x87;

						//запоминаем номер строки маршрута
						MARSHRUT_ST[i_s][s_m].NUM=i_m+100;

						if(MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]==0)fiktiv=0xf;
						else fiktiv=0;

NEXT:
						for(jj=first_beg;jj<=(last_end+1);jj++)
						{ if(fiktiv==0)objk=TRASSA[jj].object&0xfff;
							else fiktiv=0;

							if(objk==0)break;

							objk_next=TRASSA[jj+1].object&0xfff;

							if(TRASSA[jj].tip==7)
							{
								READ_BD(objk_next);
								//если ДЗ для стрелки в пути и поездной маршрут
								if((bd_osn[0]==7)&&(bd_osn[1]==15)&&((kmnd=='b')||(kmnd=='j')))
								{
									sp_in_put=bd_osn[3]; //получаем объект для СП стрелки в пути
									spar_sp=bd_osn[5]; //получаем СП для спаренной стрелки
									read_FR3(sp_in_put);//читаем состояние СП стрелки в пути
									FR3[12]=0;FR3[13]=1;//включаем предварительное замыкание СП
									write_FR3(sp_in_put); //записываем замыкание в общий массив
									obnovi(sp_in_put); //добавляем к новизне СП
									if(spar_sp!=0)//если есть спаренная стрелка со своим СП
									{
										read_FR3(spar_sp);//получаем состояние СП парной стрелки
										FR3[12]=0;FR3[13]=1; //включаем пред.замыкание
										obnovi(spar_sp); //добавляем к новизне
										write_FR3(spar_sp);
									}
								}
								read_FR3(objk);//читаем состояние объекта ДЗ
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;//записываем следующий объект трассы
								//сохранить измененные массивы на виртуальном диске
								write_FR3(objk);
							}
							else//если не ДЗ
							if((TRASSA[jj].tip>=3)&&(TRASSA[jj].tip<=5))//если СП,УП,путь
							{
								if(nachalo)//если есть начало
								{
									//если объект СП или УП
									if((TRASSA[jj].tip==3)||(TRASSA[jj].tip==4))
									{
										read_FR3(objk);
										FR3[24]=nachalo/256; //запомнить номер сигнала
										FR3[25]=nachalo%256; //начала маршрута
										write_FR3(objk);
										nachalo=0;
									}
								}
								if((objk>=KOL_VO)||(objk<=0))return;
								read_FR3(objk);

								//установить признаки замыкания
								FR3[12]=0;
								FR3[13]=1;
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;
								obnovi(objk);
								write_FR3(objk);
								//если это путь или УП
								if((TRASSA[jj].tip==5)||(TRASSA[jj].tip==4))
								{
									READ_BD(objk);//прочитать объект
									if(bd_osn[1]!=0) //если по этому объекту деление ТУМС
									{
										sosed=bd_osn[1]; //получить объект соседа
										read_FR3(sosed);
										FR3[12]=0;       //выполнить его замыкание
										FR3[13]=1;
										write_FR3(sosed);
										obnovi(sosed);

									}
								}
							}
							else
							if((TRASSA[jj].tip==2)&&(objk!=END)) //если сигнал
							{
								if((TRASSA[jj].object&0x8000)==0x8000)//если это начало
								{
									if((objk>=KOL_VO)||(objk<=0))return;
									nachalo=objk;
									read_FR3(objk);
									if(kmnd=='a') //если маневровый маршрут
									{
										FR3[12]=0;FR3[13]=1; //маневровое начало зафиксировать
									}
									if(kmnd=='b')  //если поездной маршрут
									{
										if((TRASSA[jj].object&0x2000)==0x2000) //есть поездное
										{
											FR3[14]=0;FR3[15]=1; //поездное начало зафиксировать
										}
									}

									FR3[20]=FR3[20]|(objk_next/256);

									if((s_m<MARS_STOY)&&(objk_next)&&(MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]))
									//если есть маневровое или поездное начало, то выставить флаг
									if(FR3[13]||FR3[15])FR3[20]=FR3[20]|0x80;
									FR3[21]=objk_next%256;
									obnovi(objk);
									write_FR3(objk);
								}
								else //если на сигнале начала нет
								{
									if((objk>=KOL_VO)||(objk<=0))return;
									read_FR3(objk);
									FR3[20]=FR3[20]|(objk_next/256);
									FR3[21]=objk_next%256;
									//сохранить измененные массивы на виртуальном диске
									write_FR3(objk);
								}
							}
							else
							{
							if((objk>=KOL_VO)||(objk<=0))return;
								read_FR3(objk);
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;
								//сохранить измененные массивы на виртуальном диске
								write_FR3(objk);
							}
						}//конец прохода на замыкание
						add(tums_pred-1,9999,0);
						if((REGIM==COMMAND)&&(ACTIV==0)
						)
						{
							putch1(0x10,0xc,1,Y_KOM);
							if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
							else putch1(' ',0xc,1,46);
							puts1(KOMANDA_TUMS[tums_pred-1],0xa,55,Y_KOM);
							Y_KOM++;
							if(Y_KOM>=47)Y_KOM=3;
							puts1("                                   ",0xa,3,Y_KOM);
						}
						first_beg=-1;
						last_end=-1;
					}
					else//если нет начала или конца
					{
						if(last_end==first_beg)first_beg=-1;
						if(last_end==-1)first_beg=-1;
					}
				}
				tums_pred=tums_tek; //совместить стойки

				if((TRASSA[ii].object&0x8000)==0x8000)//если сигнал может быть началом
				{
					if(first_beg)//если устанавливается команда на перевод стрелок
					{
						if(first_beg==-1)first_beg=ii;
					}
					else //если устанавливается команда на открытие сигнала
					{
						if(kmnd=='b') //поездной
						{
							if((TRASSA[ii].object&0x2000)==0x2000) //есть поездное
							{
								if(first_beg==-1)first_beg=ii; //если не было начала - взять
							}
							else//если нет поездного показания
								if(first_beg==-1)first_beg=0x8000|ii;//псевдоначало
						}
						else if(first_beg==-1)first_beg=ii;//для маневрового всегда начало
					}
				}//конец анализа сигнала для начала

				if((TRASSA[ii].object&0x4000)==0x4000)//если сигнал может быть концом
				{
					if(first_beg!=-1)last_end=ii; //если было начало, то взять конец
				}

				if((TRASSA[ii].object&0xC000)==0)//если сигнал никакой
				{
					objk=TRASSA[ii].object&0xfff;
					objk_next=TRASSA[ii+1].object&0xfff;
					if(objk<=0)return;
					if(objk>=KOL_VO)return;
					read_FR3(objk);
					FR3[20]=FR3[20]|(objk_next/256);
					FR3[21]=objk_next%256;
					//сохранить измененные массивы на виртуальном диске
					write_FR3(objk);
				}
			}
			if((TRASSA[ii].tip==0)&&(TRASSA[ii].object==0))break;
			ii++;//идти далее
			//далее вставка для участка пути 2ЧГП
			if(TRASSA[ii].tip==4)//если УП
			{
				objk=TRASSA[ii].object&0xfff;
				objk_next=TRASSA[ii+1].object&0xfff;
				if((objk>=KOL_VO)||(objk<=0))return;
				read_FR3(objk);
				//установить признаки замыкания
				FR3[12]=0;
				FR3[13]=1;
				FR3[20]=FR3[20]|(objk_next/256);
				FR3[21]=objk_next%256;
				obnovi(objk);
				write_FR3(objk);
			}
			if(TRASSA[ii].tip==6)//если переход
			{
				objk=TRASSA[ii].object&0xfff;
				objk_next=TRASSA[ii+1].object&0xfff;
				if((objk>=KOL_VO)||(objk<=0))return;
				read_FR3(objk);
				FR3[20]=FR3[20]|(objk_next/256);
				FR3[21]=objk_next%256;
				//сохранить измененные массивы на виртуальном диске
				write_FR3(objk);
			}
		}
out:
		ZERO_TRASSA();
	}
	if(TRASSA1[0].object)
	{
		if(MARSHRUT_ALL[i_m].NACH!=(TRASSA1[0].object&0xFFF))
		{
			ZERO_TRASSA1();
			return;
		}
		else
		{
			kmnd=MARSHRUT_ALL[i_m].KMND;
			END=MARSHRUT_ALL[i_m].END;
		}
		ii=0;
		last_end=-1,first_beg=-1;//установить начало поиска

		tums_pred=TRASSA1[0].stoyka;
		tums_tek=tums_pred;            //взять ТУМС

		for(i_s=0;i_s<Nst;i_s++)test=test+MARSHRUT_ALL[i_m].KOL_STR[i_s];

		if(test==0)perevod_str=0;
		else perevod_str=0xf;

		while(ii<200)//проход по всей трассе
		{
			//если сигнал или конец данных
			if((TRASSA1[ii].tip==2)||(TRASSA1[ii].tip==0))
			{
				tums_tek=TRASSA1[ii].stoyka;
				if(tums_tek!=tums_pred)//переход в другую стойку
				{
					if((first_beg&0x8000)==0x8000)//если реального начала нет
					{
						last_end=-1;first_beg=-1;tums_tek=tums_pred; //сброс начала и конца
					}
				}

				if(tums_tek!=tums_pred)//переход в другую стойку
				{
					//если есть реальное начало
					if((last_end!=-1)&&(first_beg!=last_end))//если есть конец и начало
					{
						i_s=tums_pred-1;
						MARSHRUT_ALL[i_m].STOYKA[i_s]=1;//фиксируем вхождение стойки в маршрут

						if(perevod_str!=0) //если маршрут требует перевода стрелок
						{
							MARSHRUT_ALL[i_m].SOST=0x43;// маршрут предварительный
							if(MARSHRUT_ALL[i_m].KOL_STR[i_s]!=0) //если в этой стойке перевод
							{
								MARSHRUT_ALL[i_m].STOYKA[i_s]=2; //фиксируем вхождение с переводом
							}
							else goto NEXT1;
						}
						else //если стрелки стоят по маршруту
						{
							MARSHRUT_ALL[i_m].SOST=0x83; //маршрут исполнительный
						}
						for(s_m=0;s_m<MARS_STOY;s_m++)if((MARSHRUT_ST[i_s][s_m].NUM-100)==i_m)break;

						if(s_m>=MARS_STOY)//если этот маршрут не в работе
							for(s_m=0;s_m<MARS_STOY;s_m++)if(MARSHRUT_ST[i_s][s_m].NUM==0)break;

						//если нет свободной структуры для локального задания
						if(s_m>=MARS_STOY)goto out1;

						MARSHRUT_ST[i_s][s_m].NEXT_KOM[0]='(';
						//сформировать адрес
						switch(tums_pred)
						{
							case 1: adr_kom='G';break;
							case 2: adr_kom='K';break;
							case 3: adr_kom='M';break;
							case 4: adr_kom='N';break;
							case 5: adr_kom='S';break;
							case 6: adr_kom='U';break;
							case 7: adr_kom='V';break;
							case 8: adr_kom='Y';break;
							default:return;
						}
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[1]=adr_kom;
						if(perevod_str==0)MARSHRUT_ST[i_s][s_m].NEXT_KOM[2]=kmnd;
						else MARSHRUT_ST[i_s][s_m].NEXT_KOM[2]=kmnd|8;

						MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]=TRASSA1[first_beg].podgrup;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[4]=TRASSA1[first_beg].kod_bit;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[5]=TRASSA1[last_end].podgrup;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[6]=TRASSA1[last_end].kod_bit;
						n_strel=0;
						pol_strel=0;
						for(jj=first_beg;jj<=last_end;jj++)
						{
							if(TRASSA1[jj].tip==1)//если стрелка
							{
								if((TRASSA1[jj].object&0x4000)==0x4000)//если противошерстная
								{
									if((TRASSA1[jj].object&0x2000)!=0x2000) //если нет глушения
									{
										if((TRASSA1[jj].object&0x8000)==0x8000)//если в минусе
										{
											pol_strel=pol_strel|(1<<n_strel);
										}
										n_strel++;
									}
								}
							}
						}
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[7]=0x40|n_strel;
						//заполняем положение для первых 6 стрелок
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[8]=0x40|(pol_strel&0x3f);
						//заполняем положение для вторых 6 стрелок
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[9]=0x40|((pol_strel&0xFC0)>>6);
						//считаем контрольную сумму
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[10]=0;
						MARSHRUT_ST[i_s][s_m].NEXT_KOM[11]=')';
						add(i_s,6666,s_m);
						if(perevod_str!=0)MARSHRUT_ST[i_s][s_m].SOST=0x47;
						else MARSHRUT_ST[i_s][s_m].SOST=0x87;
						MARSHRUT_ST[i_s][s_m].NUM=i_m+100;

						if(MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]==0)fiktiv=0xf;
						else fiktiv=0;

NEXT1:
						for(jj=first_beg;jj<=(last_end+1);jj++)
						{ if(fiktiv==0)objk=TRASSA1[jj].object&0xfff;
							else fiktiv=0;

							if(objk==0)break;

							objk_next=TRASSA1[jj+1].object&0xfff;

							if(TRASSA1[jj].tip==7)
							{
								READ_BD(objk_next);
								if((bd_osn[0]==7)&&(bd_osn[1]==15)&&((kmnd=='b')||(kmnd=='j')))
								{
									sp_in_put=bd_osn[3];
									spar_sp=bd_osn[5];
									read_FR3(sp_in_put);
									FR3[12]=0;FR3[13]=1;
									write_FR3(sp_in_put);
									obnovi(sp_in_put);
									if(spar_sp!=0)
									{
										read_FR3(spar_sp);
										FR3[12]=0;FR3[13]=1;
										write_FR3(spar_sp);
										obnovi(spar_sp);
									}
								}
								read_FR3(objk);
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;
								//сохранить измененные массивы на виртуальном диске
								write_FR3(objk);
							}
							else
							if((TRASSA1[jj].tip>=3)&&(TRASSA1[jj].tip<=5))//если СП,УП,путь
							{
								if(nachalo)
								{
									if((TRASSA1[jj].tip==3)||(TRASSA1[jj].tip==4))
									{
										read_FR3(objk);
										FR3[24]=nachalo/256;
										FR3[25]=nachalo%256;
										write_FR3(objk);
										nachalo=0;
									}
								}
								if((objk>=KOL_VO)||(objk<=0))return;
								read_FR3(objk);

								//установить признаки замыкания
								FR3[12]=0;
								FR3[13]=1;
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;
								obnovi(objk);
								write_FR3(objk);
								if((TRASSA1[jj].tip==5)||(TRASSA1[jj].tip==4)) //если это путь or UP
								{
									READ_BD(objk);
									if(bd_osn[1]!=0)
									{
										sosed=bd_osn[1];
										read_FR3(sosed);
										FR3[12]=0;
										FR3[13]=1;
										write_FR3(sosed);
										obnovi(sosed);
									}
								}
							}
							else
							if((TRASSA1[jj].tip==2)&&(objk!=END)) //если сигнал
							{
								if((TRASSA1[jj].object&0x8000)==0x8000)//если это начало
								{
									if((objk>=KOL_VO)||(objk<=0))return;
									nachalo=objk;
									read_FR3(objk);
									if(kmnd=='a') //если маневровый маршрут
									{
										FR3[12]=0;FR3[13]=1;
									}
									if(kmnd=='b')  //если поездной маршрут
									{
										if((TRASSA1[jj].object&0x2000)==0x2000) //есть поездное
										{
											FR3[14]=0;FR3[15]=1;
										}
									}

									FR3[20]=FR3[20]|(objk_next/256);
									if((s_m<MARS_STOY)&&(objk_next)&&(MARSHRUT_ST[i_s][s_m].NEXT_KOM[3]))
									if(FR3[13]||FR3[15])FR3[20]=FR3[20]|0x80;
									FR3[21]=objk_next%256;
									obnovi(objk);
									write_FR3(objk);
								}
								else //если на сигнале начала нет
								{
									if((objk>=KOL_VO)||(objk<=0))return;
									read_FR3(objk);
									FR3[20]=FR3[20]|(objk_next/256);
									FR3[21]=objk_next%256;
									//сохранить измененные массивы на виртуальном диске
									write_FR3(objk);
								}
							}
							else
							{
								if((objk>=KOL_VO)||(objk<=0))return;
								read_FR3(objk);
								FR3[20]=FR3[20]|(objk_next/256);
								FR3[21]=objk_next%256;
								//сохранить измененные массивы на виртуальном диске
								write_FR3(objk);
							}
						}//конец прохода на замыкание

						add(tums_pred-1,9999,0);

						if((REGIM==COMMAND)&&(ACTIV==0))
						{
							putch1(0x10,0xc,1,Y_KOM);
							if(Y_KOM!=3)putch1(' ',0xc,1,Y_KOM-1);
							else putch1(' ',0xc,1,46);
							puts1(KOMANDA_TUMS[tums_pred-1],0xa,55,Y_KOM);
							Y_KOM++;
							if(Y_KOM>=47)Y_KOM=3;
							puts1("                                   ",0xa,3,Y_KOM);
						}
						first_beg=-1;
						last_end=-1;
					}
					else//если нет начала или конца
					{
						if(last_end==first_beg)first_beg=-1;
						if(last_end==-1)first_beg=-1;
					}
				}
				tums_pred=tums_tek; //совместить стойки

				if((TRASSA1[ii].object&0x8000)==0x8000)//если сигнал может быть началом
				{
					if(first_beg)//если устанавливается команда на перевод стрелок
					{
						if(first_beg==-1)first_beg=ii;
					}
					else //если устанавливается команда на открытие сигнала
					{
						if(kmnd=='b') //поездной
						{
							if((TRASSA1[ii].object&0x2000)==0x2000) //есть поездное
							{
								if(first_beg==-1)first_beg=ii; //если не было начала - взять
							}
							else//если нет поездного показания
								if(first_beg==-1)first_beg=0x8000|ii;//псевдоначало
						}
						else if(first_beg==-1)first_beg=ii;//для маневрового всегда начало
					}
				}//конец анализа сигнала для начала

				if((TRASSA1[ii].object&0x4000)==0x4000)//если сигнал может быть концом
				{
					if(first_beg!=-1)last_end=ii; //если было начало, то взять конец
				}

				if((TRASSA1[ii].object&0xC000)==0)//если сигнал никакой
				{
					objk=TRASSA1[ii].object&0xfff;
					objk_next=TRASSA1[ii+1].object&0xfff;
					if((objk>=KOL_VO)||(objk<=0))return;
					read_FR3(objk);
					FR3[20]=FR3[20]|(objk_next/256);
					FR3[21]=objk_next%256;
					//сохранить измененные массивы на виртуальном диске
					write_FR3(objk);
				}
			}
			if((TRASSA1[ii].tip==0)&&(TRASSA1[ii].object==0))break;
			ii++;//идти далее
			//далее вставка для участка пути 2ЧГП
			if(TRASSA1[ii].tip==4)//если УП
			{
				objk=TRASSA1[ii].object&0xfff;
				objk_next=TRASSA1[ii+1].object&0xfff;
				if((objk>=KOL_VO)||(objk<=0))return;
				read_FR3(objk);
				//установить признаки замыкания
				FR3[12]=0;
				FR3[13]=1;
				FR3[20]=FR3[20]|(objk_next/256);
				FR3[21]=objk_next%256;
				obnovi(objk);
				write_FR3(objk);
			}
			if(TRASSA1[ii].tip==6)//если переход
			{
				objk=TRASSA1[ii].object&0xfff;
				objk_next=TRASSA1[ii+1].object&0xfff;
				if((objk>=KOL_VO)||(objk<=0))return;
				read_FR3(objk);
				FR3[20]=FR3[20]|(objk_next/256);
				FR3[21]=objk_next%256;
				//сохранить измененные массивы на виртуальном диске
				write_FR3(objk);
			}
		}
out1:
		ZERO_TRASSA1();
	}
}
//============================================================
/*********************************************\
* Отладочная процедура set_vvod() для задания *
*     команд непосредственно с клавиатуры     *
\*********************************************/
void set_vvod(void)
{
	char vvod[32];
	int Kom,NAC,KON,K_STR,i,j;
begin0:
	textattr(0xA);
	gotoxy(4,10);cputs("Команда ");gets(vvod);Kom=atoi(vvod);
	if(Kom==1000) //прямой ввод данных ТУМСа
	{
snova:
  	gotoxy(4,11);cputs("Номер ТУМС ");gets(vvod);NAC=atoi(vvod);
		if(NAC>=Nst)goto snova;
    gotoxy(4,12);cputs("Пакет ТС от ТУМС ");
    i=0;
		while(1)
		{
      if(kbhit())
      {
				j=getch();
				if(j!=8)
      	{
      		if(j==13)break;
      		vvod[i]=j;
        	putch(vvod[i++]);
				}
  	    else
    	  {
      		if(i!=0)
					{
						i--;
						putch(8);
        		putch(0x20);
        		putch(8);
        	}
				}
    	}
		}


		KVIT_TUMS[NAC][1]='(';
		KVIT_TUMS[NAC][4]=0;
		for(i=1;i<10;i++)BUF_IN[i]=vvod[i-1];
		BUF_IN[0]='(';
		BUF_IN[11]=')';
		BUF_IN[10]=check_summ(BUF_IN);
		END_TUMS=0xF;
		vvod_set=0;
		return;
	}

	if((Kom>=187)&&(Kom<=192))
  {
begin:
		gotoxy(4,11);cputs("Начало маршрута ");gets(vvod);NAC=atoi(vvod);
		if((NAC>=KOL_VO)||(NAC<=0))goto begin;
begin1:
    gotoxy(4,12);cputs("Конец маршрута ");gets(vvod);KON=atoi(vvod);
		if((KON>=KOL_VO)||(KON<=0))goto begin1;
begin2:
    gotoxy(4,13);cputs("Число стрелок ");gets(vvod);K_STR=atoi(vvod);
    if(K_STR>=32)goto begin2;
    i=0;
    gotoxy(4,14);cputs("Положение стрелок ");
begin3:
    for(i=0;i<32;i++)vvod[i]=0;
		i=0;
    while(vvod[i]!=13)
    {
      vvod[i]=getch();
      if((vvod[i]=='0')||(vvod[i]=='1'))
      {
        putch(vvod[i]);
        i++;
      }
      if(i>K_STR)goto begin3;
    }
    KOMANDA_MARS[0][0][0]=Kom;
    KOMANDA_MARS[0][0][1]=NAC%256;
    KOMANDA_MARS[0][0][2]=NAC/256;
    KOMANDA_MARS[0][0][3]=KON%256;
    KOMANDA_MARS[0][0][4]=KON/256;
		KOMANDA_MARS[0][0][5]=K_STR;
		KOMANDA_MARS[0][0][6]=0;
		for(i=0;i<8;i++)if(vvod[i]=='1')KOMANDA_MARS[0][0][6]=KOMANDA_MARS[0][0][6]|(1<<i);
    for(i=0;i<8;i++)if(vvod[i+8]=='1')KOMANDA_MARS[0][0][7]=KOMANDA_MARS[0][0][7]|(1<<i);
    for(i=0;i<8;i++)if(vvod[i+16]=='1')KOMANDA_MARS[0][0][8]=KOMANDA_MARS[0][0][8]|(1<<i);
    for(i=0;i<8;i++)if(vvod[i+24]=='1')KOMANDA_MARS[0][0][9]=KOMANDA_MARS[0][0][9]|(1<<i);
    vvod_set=0;
		MAKE_MARSH(0,0);
    vvod_set=0;
		return;
  }
  if((Kom==47)||(Kom==48))
  {
beg:
    puts1("Начало маршрута ",0xa,4,10);gets(vvod);NAC=atoi(vvod);
		if((NAC>=KOL_VO)||(NAC<=0))goto begin;
    KOMANDA_RAZD[0][0][0]=Kom;
		KOMANDA_RAZD[0][0][1]=NAC%256;
    KOMANDA_RAZD[0][0][2]=NAC/256;
		MAKE_KOMANDA(0,0,0);
    vvod_set=0;
    return;
  }
  if((Kom==96)||(Kom==97))
  {
    puts1("Объект ",0xa,4,11);
    gotoxy(11,11);
    gets(vvod);NAC=atoi(vvod);
    KOMANDA_RAZD[0][0][0]=Kom;
    KOMANDA_RAZD[0][0][1]=NAC%256;
    KOMANDA_RAZD[0][0][2]=NAC/256;
    MAKE_KOMANDA(0,0,0);
    vvod_set=0;
    return;
	}
	if((Kom==101)||(Kom==102))
	{
		KOM_BUFER[3]=Kom;
		MAKE_TIME(0,0);
		return;
	}
  puts1("Объект",0xa,4,12);gotoxy(8,2);gets(vvod);NAC=atoi(vvod);
	KOMANDA_RAZD[0][0][0]=Kom;
  KOMANDA_RAZD[0][0][1]=NAC%256;
	KOMANDA_RAZD[0][0][2]=NAC/256;
  MAKE_KOMANDA(0,0,0);
  vvod_set=0;
  return;
}
//=============================================================
/*************************************\
*   проверка ДЗ охранной стрелки      *
*          int tst_str_ohr()          *
\*************************************/
int tst_str_ohr(void)
{
	int strlka,para,sp_osn,sp_par,poloj;
	strlka=bd_osn[2];//охранная стрелка
	sp_osn=bd_osn[3];//СП охранной стрелки
	para=bd_osn[4];  //парная стрелка
	sp_par=bd_osn[5];//СП парной стрелки
	poloj=bd_osn[6]; //охранное положение
	//проверка охранной стрелки
	read_FR3(strlka);//прочитать состояние стрелки
	if(FR3[11]==1)return(1010);	 //если стрелка непарафазна
	if(FR3[9]==1)return(1011);   //если стрелка потеряла контроль
	if(poloj==0)		 //если положение стрелки плюсовое
	{
		if((FR3[1]!=1)||(FR3[3]!=0))//если стрелка не в плюсе (надо переводить)
		{
			read_FR3(sp_osn); //прочитать текущее состояние основного СП
			if(FR3[11]==1)return(1016);//если СП непарафазно
			if(FR3[1]!=0)return(1012); //если СП занято
			if(FR3[3]!=0)return(1013); //если СП замкнуто
			if(FR3[5]!=0)return(1014); //если СП в разделке
			if(FR3[7]!=0)return(1015); //если в работе МСП
		}
	}
	if(poloj==1)		 //если положение стрелки минусовое
	{
		if((FR3[1]!=0)||(FR3[3]!=1))//если стрелка не в минусе (надо переводить)
		{
			read_FR3(sp_osn); //прочитать текущее состояние основного СП
			if(FR3[1]!=0)return(1012); //если СП занято
			if(FR3[3]!=0)return(1013); //если СП замкнуто
			if(FR3[5]!=0)return(1014); //если СП в разделке
			if(FR3[7]!=0)return(1015); //если в работе МСП
		}
	}
	if(para==0)return(0);
	//проверка парной стрелки
	read_FR3(para);//прочитать состояние стрелки
	if(FR3[11]==1)return(1010);	 //если стрелка непарафазна
	if(FR3[9]==1)return(1011);   //если стрелка потеряла контроль
	if(poloj==0)		 //если положение стрелки плюсовое
	{
		if((FR3[1]!=1)||(FR3[3]!=0))//если стрелка не в плюсе (надо переводить)
		{
			read_FR3(sp_par); //прочитать текущее состояние СП пары
			if(FR3[11]==1)return(1016);//если СП непарафазно
			if(FR3[1]!=0)return(1012); //если СП занято
			if(FR3[3]!=0)return(1013); //если СП замкнуто
			if(FR3[5]!=0)return(1014); //если СП в разделке
			if(FR3[7]!=0)return(1015); //если в работе МСП
		}
	}
	if(poloj==1)		 //если положение стрелки минусовое
	{
		if((FR3[1]!=0)||(FR3[3]!=1))//если стрелка не в минусе (надо переводить)
		{
			read_FR3(sp_par); //прочитать текущее состояние основного СП
			if(FR3[1]!=0)return(1012); //если СП занято
			if(FR3[3]!=0)return(1013); //если СП замкнуто
			if(FR3[5]!=0)return(1014); //если СП в разделке
			if(FR3[7]!=0)return(1015); //если в работе МСП
		}
	}
	return(0);
}
//================================================================
/***************************************\
* Процедура вызова рестарта компьютера  *
\***************************************/
void re_set(void)
{
	 int value;
   jmp_buf jumper;
#ifdef WORK
  reset_int_vect1();
#endif
  clrscr();
#ifdef TEST
  close(file_arc);
//  close(file_arm_in);
//  close(file_arm_out);
#endif
  value = setjmp(jumper);
  if(value!=0)exit(0);
  poke(0x40,0x72,0x1234);
  jumper[0].j_cs=0xFFFF;
	jumper[0].j_ip=0;
  longjmp(jumper,1);
  return;
}
//====================================================================
/**************************************************\
*                                                  *
*     Процедура передачи сообщений в АРМ ДСП       *
*                                                  *
*     N_mar - номер глобального маршрута           *
*     sos - объект сервера для начала маршрута     *
* ARM_N - номер АРМа,которому передается сообщение *
* R_U - район управления                           *
*                                                  *
\**************************************************/
void Soob_For_Arm(int N_mar, int sos, int kodik)
{
	int ARM_N,objserv,ranj;
	unsigned char BYTES[2],chislo[6];
	if(sos!=0)objserv=sos;
	else return;
	itoa(kodik,chislo,10);
	BYTES[1]=((out_ob[objserv]&0xff00)>>8)|0x40;
	BYTES[0]=out_ob[objserv]&0xff;
	READ_BD(sos);
	ranj=bd_osn[13]&0xff;
	for(ARM_N=0;ARM_N<Narm;ARM_N++)
	{
		if(KONFIG_ARM[ARM_N][ranj-1]==0)continue;
		KVIT_ARMu1[ARM_N][0][0]=BYTES[0];
		KVIT_ARMu1[ARM_N][1][0]=BYTES[0];
		KVIT_ARMu1[ARM_N][0][1]=BYTES[1];
		KVIT_ARMu1[ARM_N][1][1]=BYTES[1];
		KVIT_ARMu1[ARM_N][0][2]=3;
		KVIT_ARMu1[ARM_N][1][2]=3;
	}
	if(REGIM==ANAL_MARSH)
	{
			puts1(PAKO[objserv],12,45,45);
			puts1(chislo,12,45,46);
	}
}
