#include "opred2.h"
#include "extern2.h"
//=====================================================
/***************************************************\
*            OSN_TUMS_IN()                          *
* ������ � ��������� ������� ��������� ������ ����  *
\***************************************************/
void OSN_TUMS_IN(void)
{
	int i,svoi,tum,ik,s_m,nom,ob_str;
  unsigned char PODGR,bait;
  if(END_TUMS==0xF)//�᫨ ���� �ਥ� ������ �� �᭮����� ������ (12 ����)
	{
		ADR_TUMS_IN=BUF_IN[1];//�뤥���� �� �ਭ�⮣� ������ ����
		ADR_TUMS_OUT=ADR_TUMS_IN;
		switch(ADR_TUMS_IN) //��।������ �⮩�� �� ���᭮� ���
		{
			case 71: ST=1;break;
      case 75: ST=2;break;
			case 77: ST=3;break;
			case 78: ST=4;break;
      case 83: ST=5;break;
			case 85: ST=6;break;
			case 86: ST=7;break;
			case 89: ST=8;break;
			default: ST=0;break;
		}

		//�᫨ ���� �� ������,��� ���஢
		if(ST<=0) {	sbros_tums(0); return; }
		tum=ST-1;
		//�᫨ ��⭮� ����砭��
		if((BUF_IN[11]>=0x23)&&(BUF_IN[11]<=0x2C))
		{
			//��९���� � ॣ���� �ਥ�� ������
			for(i=0;i<12;i++){REG_IN[ST-1][i]=BUF_IN[i];BUF_IN[i]=0;}

			//�᫨ ����஫쭠� �㬬� ᮢ����, � �����⮢��� ���⠭��
			summa=check_summ(REG_IN[ST-1]);
			if(summa==REG_IN[ST-1][10])//�᫨ ����஫쭠� �㬬� � ��ଥ
			{
				SVAZ_TUMS[ST-1]=0;//����⠭����� �ਧ��� �裡 � ����
				PODGR=REG_IN[ST-1][9];  //�뤥���� �ਭ���� �����㯯� ���⠭権
				ANALIZ_MYTHX(PODGR);
				if((PODGR>=0x30)&&(PODGR<=0x58))//�᫨ ��諠 ���⠭�� �� �������
				{ bait=REG_IN[ST-1][11];//�뤥���� ��� ����� ����
					switch(bait)
					{
						case 0x23: bait=0;break;
						case 0x26: bait=1;break;
						case 0x25: bait=2;break;
						case 0x2a: bait=3;break;
						case 0x2c: bait=4;break;
						default:   bait=0xf;break;
					}

					if(KOMANDA_TUMS[ST-1][11]!=0) //�᫨ ��� �⮩�� �뫠 ������⭠�
					{
						if((KOMANDA_TUMS[ST-1][3]==PODGR)&& //�᫨ �����㯯� �뫠 � �������
						(KOMANDA_TUMS[ST-1][4]==(bait|0x40))) //�᫨ ���� ᮮ⢥�����
						{
							puts1("M-",0xa,51,49); //�뢥�� �������� �����.���⢥ত����
							putch1(PODGR,0xa,53,49); //�뢥�� �����㯯� ���⠭樨
							putch1(bait|0x40,0xa,54,49);
							for(i=0;i<15;i++)KOMANDA_TUMS[ST-1][i]=0;//����� �������
						}
						else //�᫨ ����祭� ���ࠢ��쭠� ���⠭��
						{
							puts1("M!",0x8a,51,49); //�뢥�� �������� �訡�� �����.���⢥ত����
							putch1(PODGR,0x8a,53,49); //�뢥�� �����㯯� ���⠭樨
							putch1(bait|0x40,0x8a,54,49); //�뢥�� ���� ���⠭樨
						}
					}

					if(KOMANDA_ST[ST-1][11]!=0)//�᫨ ��� �⮩�� �뫠 ࠧ���쭠�
					{
						if(((KOMANDA_ST[ST-1][3]==PODGR)&& //�᫨ �����㯯� �뫠 � �������
						(KOMANDA_ST[ST-1][4+bait])!=124))  //�᫨ ���� ᮮ⢥�����
						{
							puts1("R-",0xa,56,49); //�뢥�� �������� ࠧ�.���⢥ত����
							putch1(PODGR,0xa,58,49);
							putch1(bait+48,0xa,59,49);
							for(i=0;i<15;i++)KOMANDA_ST[ST-1][i]=0;//����� �������
						}
						else
						{
							puts1("R!",0x8a,56,49); //�뢥�� �������� �訡�� ���⢥ত����
							putch1(PODGR,0x8a,58,49); //�뢥�� �����㯯� ���⠭樨
							putch1(REG_IN[ST-1][11],0x8a,59,49);
						}
					}
				}
				else putch1(ST+48,0xa,49+ST,50); //�᫨ �� �⮩�� �� ���⠭��

				//��������� ���⠭�� �ࢥ� ��� �����
				KVIT_TUMS[ST-1][0]='$';
				KVIT_TUMS[ST-1][1]=REG_IN[ST-1][1];
				KVIT_TUMS[ST-1][2]=REG_IN[ST-1][3];
				KVIT_TUMS[ST-1][3]=0;
				KVIT_TUMS[ST-1][4]=0;
				ZPRS_TMS=0;

				if(ACTIV==1)
				{
					for(i=0;i<3;i++)//��ࠢ��� ���⠭�� � �����
					{ BUF_OUT[UKAZ_ZAP++]=KVIT_TUMS[ST-1][i];
						if(UKAZ_ZAP>=SIZE_BUF)UKAZ_ZAP=0;
					}
					outportb(ADR_TUMS_OSN+1,3);//������ ���뢠��� ��।��
				}
			}
			else
			{
				add(ST-1,3333,0);
				sbros_tums(0); //�᫨ ����஫쭠� �㬬� �� � ��ଥ
			}

			if(ACTIV==0)
			{
				//�᫨ ���� �⢥⨫ � �� �맮� �ணࠬ�� �뢮�� �� ��࠭
				if((ADR_TUMS_IN>0)&&(ZAPROS[5]!=0))
				{ //ᮧ���� ���� �����
					ZAPROS_TUMS=ST-1;
					//��������� �����
					ZAPROS[0]='!';ZAPROS[1]=ADR_TUMS_IN;ZAPROS[2]=')';
					//��⠭����� �ਧ��� ����⢮����� �����
					ZPRS_TMS=0xF;
					//��⠭����� �ਧ��� �ॡ������ �뢮�� �� ��࠭
					ZAPROS[5]=0;
				}
			}
			if(REGIM==KANAL)//�᫨ ��ᬮ�� ������� ������
			{
				for(i=0;i<12;i++)
				{
					putch1(REG_IN[ST-1][i],atrib,X_in[ST-1],Y_in[ST-1]);
					X_in[ST-1]=X_in[ST-1]+1;
				}
				if(X_in[ST-1]>25)X_in[ST-1]=7;
				atrib=atrib+1;if(atrib>=15)atrib=1;//ᬥ���� 梥�
			}
			if(REGIM==0)
			{
				putch1(30,10,4+6*(ST-1),38);
				putch1(16,10,53+9*(SERVER-1),44-2*(SERVER-1));
			}
		}
		else sbros_tums(0);//�᫨ ���ࠢ��쭮� ����砭�� - ��� ������
		END_TUMS=0;
	}
	return;
}
//===========================================================
/*********************************************************\
*                      REZ_TUMS_IN()                      *
*   ������ � ��������� ������� ���������� ������ ����     *
\*********************************************************/
void REZ_TUMS_IN(void)
{
	int i,svoi,tum,s_m,nom,ik,ob_str;
	unsigned char PODGR,bait;
	if(END_TUMS1==0xF)//�᫨ ���� ����� ���뫪� �� १�ࢭ��� ������
	{
		ADR_TUMS_IN1=BUF_IN1[1];		//�뤥���� ������ ����
		ADR_TUMS_OUT1=ADR_TUMS_IN1;
		switch(ADR_TUMS_IN1)       //�� ���᭮� ��� ��।����� �⮩��
		{
			case 71: ST=1;break;
			case 75: ST=2;break;
			case 77: ST=3;break;
			case 78: ST=4;break;
			case 83: ST=5;break;
			case 85: ST=6;break;
			case 86: ST=7;break;
			case 89: ST=8;break;
			default: ST=0;break;
		}
		if(ST<=0){sbros_tums(1);return;}
    tum=ST-1;
		if((BUF_IN1[11]>=0x23)&&(BUF_IN1[11]<=0x2C)) //�᫨ ��⭮� ����砭��
		{
			//��९���� � ���� �ਥ�� १�ࢭ��� ������
			for(i=0;i<12;i++){REG1_IN[ST-1][i]=BUF_IN1[i];BUF_IN1[i]=0;}
			//�᫨ ����஫쭠� �㬬� ᮢ����, � �����⮢��� ���⠭��
			summa=check_summ(REG1_IN[ST-1]);
			if(summa==REG1_IN[ST-1][10])	//�᫨ ����஫쭠� �㬬� � ��ଥ
			{
				SVAZ_TUMS[ST-1]=0;
				PODGR=REG1_IN[ST-1][9]; //�뤥���� �ਭ���� �����㯯� ���⠭権
				ANALIZ_MYTHX(PODGR);
				if((PODGR>=0x30)&&(PODGR<=0x58))//�᫨ ��諠 ���⠭�� �� �������
				{ bait=REG1_IN[ST-1][11]; //���� ��� ����� ���� ���⠭樨
					switch(bait)
					{
						case 0x23: bait=0;break;
						case 0x26: bait=1;break;
						case 0x25: bait=2;break;
						case 0x2a: bait=3;break;
						case 0x2c: bait=4;break;
						default:   bait=0xf;break;
					}
					if(KOMANDA_TUMS[ST-1][11]!=0)  //�᫨ ��� �⮩�� �뫠 �����.�������
					{
						if((KOMANDA_TUMS[ST-1][3]==PODGR)&& //�᫨ ��ꥪ� ���⠭樨 � ������� ᮢ������
						(KOMANDA_TUMS[ST-1][4]==(bait|0x40))) //�᫨ ���� ���⠭樨 � ������� ᮢ������
						{
							puts1("M-",0xc,51,49); //�뢥�� �������� �����.���⢥ত����
							putch1(PODGR,0xc,53,49); //�뢥�� �����㯯� ���⠭樨
							putch1(bait|0x40,0xc,54,49); //�뢥�� ��� ����
							for(i=0;i<15;i++)KOMANDA_TUMS[ST-1][i]=0;//���� ������� ����.
						}
						else
						{
							puts1("M!",0x8c,51,49); //�뢥�� �������� �訡�� �����.���⢥ত����
							putch1(PODGR,0x8c,53,49); //�뢥�� �����㯯� ���⠭樨
							putch1(bait|0x40,0x8c,54,49); //�뢥ᨬ ���� ���⠭樨
						}
					}
					if(KOMANDA_ST[ST-1][11]!=0) //�᫨ ��� �⮩�� �뫠 ࠧ���쭠�
					{
						if(((KOMANDA_ST[ST-1][3]==PODGR)&& //�᫨ ��ꥪ� ᮢ���
						(KOMANDA_ST[ST-1][4+bait])!=124))  //�᫨ ���� �������⢨� ᮢ���
						{
							puts1("R-",0xc,56,49); //�뢥�� �������� ࠧ�.���⢥ত����
							putch1(PODGR,0xc,58,49);
							putch1(bait+48,0xc,59,49);
							for(i=0;i<15;i++)KOMANDA_ST[ST-1][i]=0; //����� ࠧ�.�������
						}
						else //�᫨ ���⠭�� �� ᮢ����
						{
							puts1("R!",0x8c,56,49); //�뢥�� �������� �����.���⢥ত����
							putch1(PODGR,0x8c,58,49); //�뢥�� �����㯯� ���⠭樨
							putch1(REG1_IN[ST-1][11],0x8c,59,49);
						}
					}
				}
				else putch1(ST+48,0xc,49+ST,50);
				KVIT_TUMS1[ST-1][0]='$';
				KVIT_TUMS1[ST-1][1]=REG1_IN[ST-1][1];
				KVIT_TUMS1[ST-1][2]=REG1_IN[ST-1][3];
				KVIT_TUMS1[ST-1][3]=0;
				KVIT_TUMS1[ST-1][4]=0;
				ZPRS_TMS1=0;

				if(ACTIV==1)
				{
					for(i=0;i<3;i++)//��ࠢ��� ���⠭�� � �����
					{ BUF_OUT1[UKAZ_ZAP1++]=KVIT_TUMS1[ST-1][i];
						if(UKAZ_ZAP1>=SIZE_BUF)UKAZ_ZAP1=0;
					}
					outportb(ADR_TUMS_REZ+1,3);//������ ���뢠��� ��।��
				}
			}
			else
			{
				add(ST-1,3333,1);
				sbros_tums(1);
			}

			if(ACTIV==0)
			{
				if((ADR_TUMS_IN1>0)&&(ZAPROS1[5]!=0))
				{
					ZAPROS_TUMS1=ST;
					//��������� �����
					ZAPROS1[0]='!';ZAPROS1[1]=ADR_TUMS_OUT1; ZAPROS1[2]=')';
					ZPRS_TMS1=0xF;
					ZAPROS1[5]=0;
				}
			}
			if(REGIM==KANAL)
			{
				for(i=0;i<12;i++)
				{
					putch1(REG1_IN[ST-1][i],atrib, X_in_rez[ST-1],Y_in_rez[ST-1]);
					X_in_rez[ST-1]=X_in_rez[ST-1]+1;
				}
				if(X_in_rez[ST-1]>25)X_in_rez[ST-1]=7;
				atrib=atrib+1;if(atrib>=15)atrib=1;//ᬥ���� 梥�
			}
			if(REGIM==0)
			{
				putch1(31,14,4+6*(ST-1),48);
				putch1(30,14,54+9*(SERVER-1),46-2*(SERVER-1));
			}
		}
		else sbros_tums(1);
		END_TUMS1=0;
	}
	return;
}
//=================================================================================
/****************************************\
*             consentr1()                *
*  ����� � ����묨, �ਭ��묨 �� ����   *
\****************************************/
void consentr1(void)
{
	int i,jk,st,soob,attr,nov_bit,jj;
	unsigned char novizna,j,GRUPPA;
	char STROKA;
	for(i=0;i<Nst;i++)//�ன� �� �ᥬ �⮩��� ����
	{
		//�᫨ ���� �����㦥��� �ਥ� ������ �᭮����� ������
		if((KVIT_TUMS[i][1]!=0)&&(KVIT_TUMS[i][4]==0))
		{
			st=i+1;//��ନ஢��� ����� �⮩��
			GRUPPA=REG_IN[i][2]; //�뤥���� ��� ��㯯�
			soob=REG_IN[i][3]-48;//�뤥���� ����� ᮮ�饭��
			if((GRUPPA=='R')&&(REG_IN[i][3]=='y'))//�᫨ ����ᥭ��
			{
				soob=45;	add(i,soob,0);
				if(diagnoze(i,0)==-1)	{for(jj=0;jj<3;jj++)DIAGNOZ[jj]=0;	for(jj=0;jj<12;jj++)REG_IN[i][jj]=0; goto aa1;}
			}
			else
			{
				if(((GRUPPA=='X')||(GRUPPA=='J'))&&(FLAG_KOM==0))
				{ if(test_plat(i,0)==-1)for(jj=0;jj<6;jj++)ERR_PLAT[jj]=0;
					soob=44; add(i,soob,0);//�믮����� ���४��
					novizna=0; nov_bit=0;
					for(j=0;j<6;j++)//�ன� �� �ᥬ ���⠬ ᮮ�饭��
					{
						if(VVOD[i][soob][j]!=REG_IN[i][j+4])
						{
							if(j<5)novizna=novizna|(1<<j); //���� �������
						}
						VVOD[i][soob][j]=REG_IN[i][j+4];//������� ����� � ���ᨢ �����
					}
					if((novizna==0)&&(nov_bit!=0))novizna=0x1f;
					else novizna=0;
				}
				else  //��� ��� ��⠫��� ᮮ�饭��
				{
					novizna=0;	nov_bit=0;
					for(j=0;j<6;j++)//�ன� �� �ᥬ ���⠬ ᮮ�饭��
					{
						//���� �������
						if(VVOD[i][soob][j]!=REG_IN[i][j+4])
						{
							if(j<5)novizna=novizna|(1<<j);
						}
						VVOD[i][soob][j]=REG_IN[i][j+4];//������� ����� � ���ᨢ �����
					}
				}
			}
			VVOD[i][soob][6]=0;//��࠭��⥫� ��ப�
#ifndef TEST2
			if((novizna!=0)||(fixir!=0))add(i,soob,0);
#else
			add(i,soob,0);
#endif
			if((soob!=45)&&(soob!=44))//�᫨ ᮮ�饭�� �� ���������᪨�
			{
				STROKA=TAKE_STROKA(GRUPPA,soob,i);//���᫥��� ��ப� ����஢ ��ꥪ⮢
				if(STROKA>=0)ZAPOLNI_FR3(GRUPPA,STROKA,soob,i,novizna);//��������� FR3 �ࢥ�
			}
			if(soob==44)VVOD[i][44][5]=REG_IN[i][3];//��� �������⨪� ���� �������� �㪢�
			if(novizna!=0)nov_bit=0x70;

aa1:
			KVIT_TUMS[i][4]=0xF;//��⠭����� �ਧ��� ���㦨�����
			if(REGIM==OBJECT)
			{
				if((soob==44)||(soob==45))attr=0xc;
				else
				{
					attr=0xf+nov_bit;
					if((soob+1)<=STR[i])attr=0xa+nov_bit;
					else
						if((soob+1)<=(STR[i]+SIG[i]))attr=0xe+nov_bit;
						else
							if((soob+1)<=(STR[i]+SIG[i]+DOP[i]))attr=0xd+nov_bit;
							else
								if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]))attr=0x6+nov_bit;
								else
									if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]))attr=0xf+nov_bit;
									else
										if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]+UPR[i]))attr=0x9+nov_bit;
										else
											attr=0xc+nov_bit;
				}
				for(jk=0;jk<6;jk++)putch1(VVOD[i][soob][jk],attr,jk+2+(st-1)*10,soob+2);
				if(soob==44)putch1(REG_IN[i][3],attr,7+(st-1)*10,46);
			}
			if(REGIM==ANALIZ)
			{

			}
			if(ACTIV==0)sbros_tums(0);
		}
		//�᫨ ���� �����㦥��� �ਥ� ������ १�ࢭ��� ������
		if((KVIT_TUMS1[i][1]!=0)&&(KVIT_TUMS1[i][4]==0))
		{
			st=i+1;//��ନ஢��� ����� �⮩��
			GRUPPA=REG1_IN[i][2]; //�뤥���� ��� ��㯯�
			soob=REG1_IN[i][3]-48;//�뤥���� ����� ᮮ�饭��
			if((GRUPPA=='R')&&(REG1_IN[i][3]=='y'))//�᫨ ����ᥭ��
			{
				soob=45;
				add(i,soob,0);
				if(diagnoze(i,1)==-1)
				{
					for(jj=0;jj<3;jj++)DIAGNOZ[jj]=0;
					for(jj=0;jj<12;jj++)REG1_IN[i][jj]=0;
					goto aa2;
				}
			}
			else
			//�᫨ �������⨪�
			if(REG1_IN[i][3]>=112)
			{
				if((GRUPPA=='J')&&(FLAG_KOM==0))
				{
					if(test_plat(i,1)==-1)
					{
						for(jj=0;jj<6;jj++)ERR_PLAT[jj]=0;
						continue;
					}
					soob=44;
					add(i,soob,0);
				}
				else continue;
				novizna=0;
				nov_bit=0;
				for(j=0;j<6;j++)//�ன� �� �ᥬ ���⠬ ᮮ�饭��
				{
					//���� �������
					if(VVOD[i][soob][j]!=REG1_IN[i][j+4])
					{
						if(j<5)novizna=novizna|(1<<j);
					}
					VVOD[i][soob][j]=REG1_IN[i][j+4];//������� ����� � ���ᨢ �����
					nov_bit=nov_bit+(VVOD[i][soob][j]&0x3f);
				}
				if((novizna==0)&&(nov_bit!=0))novizna=0x1f;
				else novizna=0;
			}
			novizna=0;
			nov_bit=0;
			for(j=0;j<6;j++)
			{
				//���� �������
				if(VVOD[i][soob][j]!=REG1_IN[i][j+4])
				{
					if(j<5)novizna=novizna|(1<<j);
				}
				VVOD[i][soob][j]=REG1_IN[i][j+4];//������� ����� � ���ᨢ �����
			}
			VVOD[i][soob][6]=0;//��࠭��⥫� ��ப�
#ifndef TEST2
			if((novizna!=0)||(fixir!=0))add(i,soob,1);
#else
			add(i,soob,1);
#endif
			if((soob!=45)&&(soob!=44))
			{
				STROKA=TAKE_STROKA(GRUPPA,soob,i);//���᫥��� ��ப� ����஢ ��ꥪ⮢
				if(STROKA>=0)ZAPOLNI_FR3(GRUPPA,STROKA,soob,i,novizna);
			}
			if(soob==44)VVOD[i][soob][5]=REG1_IN[i][3];
			if(novizna!=0)nov_bit=0x70;
aa2:
			KVIT_TUMS1[i][4]=0xF;
			if(REGIM==OBJECT)
			{
				if((soob==44)||(soob==45))attr=0xc;
				else
				{
					attr=0xf+nov_bit;
					if((soob+1)<=STR[i])attr=0xa+nov_bit;
					else
						if((soob+1)<=(STR[i]+SIG[i]))attr=0xe+nov_bit;
						else
							if((soob+1)<=(STR[i]+SIG[i]+DOP[i]))attr=0xd+nov_bit;
							else
								if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]))attr=0x6+nov_bit;
								else
									if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]))attr=0xf+nov_bit;
									else
										if((soob+1)<=(STR[i]+SIG[i]+DOP[i]+UCH[i]+PUT[i]+UPR[i]))attr=0x9+nov_bit;
										else attr=0xc+nov_bit;
					for(jk=0;jk<6;jk++)putch1(VVOD[i][soob][jk],attr,jk+2+(st-1)*10,soob+2);
					if(soob==44)putch1(REG1_IN[i][3],attr,7+(st-1)*10,46);

				}
			}
			if(REGIM==ANALIZ)
			{

			}
			if(ACTIV==0)sbros_tums(1);
		}
	}
	return;
}
//========================================================================
void add(int st,int sob,int knl)
{
	char ZAPIS[40],tms[2],nom[3],nom_fr4[4],OGR[2],VREMA[9];
	unsigned int ito,i,kk;
	for(ito=0;ito<40;ito++)ZAPIS[ito]=0;
	//������� �६� � ����� �����
	strcpy(ZAPIS,TIME);
	strncat(ZAPIS," ",1);
	if((sob!=200)&&(sob!=300)&&(sob!=7777)&&(sob!=6666)&&(sob!=3333))
	{
		tms[0]=st+49;
		tms[1]=32;
		strncat(ZAPIS,tms,2);
		strncat(ZAPIS," ",1);
	}
	if(sob==300)//�᫨ ��⠭���� �६���
	{
		strncat(ZAPIS,"<TIME> ",7);

		if(KOM_TIME[2]<10)VREMA[0]=0x30;
		else VREMA[0]=(KOM_TIME[2]/10)|0x30;
		VREMA[1]=(KOM_TIME[2]%10)|0x30;
		VREMA[2]=':';

		if(KOM_TIME[1]<10)VREMA[3]=0x30;
		else VREMA[3]=(KOM_TIME[1]/10)|0x30;
		VREMA[4]=(KOM_TIME[1]%10)|0x30;
		VREMA[5]=':';
		if(KOM_TIME[0]<10)VREMA[6]=0x30;
		else VREMA[6]=(KOM_TIME[0]/10)|0x30;
		VREMA[7]=(KOM_TIME[0]%10)|0x30;
		VREMA[8]=0;
		strcat(ZAPIS,VREMA);
	}
	else
		if(sob==200)
		{
			strncat(ZAPIS,"<FR4> ",6);
			itoa(knl,nom_fr4,10);
			strcat(ZAPIS,nom_fr4);
			strncat(ZAPIS,"-",1);
			OGR[0]=st|0x40;
			OGR[1]=0;
			strncat(ZAPIS,OGR,1);
		}
		else
			if(sob==100) strncat(ZAPIS,"BEGIN",5);
			else
				if(sob==3333)//�᫨ ����襭� ����஫쭠� �㬬�
				{
					strncat(ZAPIS,"$$$ ",4);
					strncat(ZAPIS,REG_IN[st],12);
					strncat(ZAPIS,"->",2);
					if(knl==0)strncat(ZAPIS,"��",3);
					else strncat(ZAPIS,"१",3);
				}
				else
				if(sob==8888)  //�᫨ 㤠����� �������
				{
					strncat(ZAPIS,"{㤠�}",6);
					strncat(ZAPIS,PAKO[MARSHRUT_ALL[st].NACH],strlen(PAKO[MARSHRUT_ALL[st].NACH]));
					itoa(knl,nom_fr4,10);
					strncat(ZAPIS,"->",2);
					strncat(ZAPIS,nom_fr4,2);//������� ��� 㤠�����
				}
				else
				if(sob==7777)  //�᫨ ᮧ��� ��������
				{
					strncat(ZAPIS,PAKO[st],strlen(PAKO[st]));
					strncat(ZAPIS,"->",2);
					strncat(ZAPIS,PAKO[knl],strlen(PAKO[knl]));
				}
				else
				if(sob==6666)  //�᫨ ᮧ��� �������
				{
					strncat(ZAPIS,MARSHRUT_ST[st][knl].NEXT_KOM,13);
				}
				else
				if(sob!=9999)	//�᫨ �� �������
				{
					nom[0]=0;nom[1]=0;nom[2]=0;
					if(sob<10)
					{
						nom[0]=48;
						nom[1]=sob+48;
					}
					else
					{
						nom[0]=(sob/10)+48;
						nom[1]=(sob%10)+48;
					}
					strcat(ZAPIS,nom); 						//�������� ����� ᮮ�饭��
																				//�������� ����� ������
					if(knl==0)strncat(ZAPIS,"-1",2);
					else strncat(ZAPIS,"-2",2);
					kk=strlen(ZAPIS);
				//�������� ᮤ�ন��� �室���� ॣ����
					if(knl==0)
					{
						for(i=0;i<12;i++)ZAPIS[kk+i]=REG_IN[st][i];
					}
					else
					{
						for(i=0;i<12;i++)ZAPIS[kk+i]=REG1_IN[st][i];
					}
				}
				else  								//�᫨ �믮������ �������
				{
					strncat(ZAPIS,"<Kom>",5);
					if(KOMANDA_ST[st][11]!=0)   //�᫨ ���� ࠧ���쭠� �������
					{
						kk=strlen(ZAPIS);
						for(i=0;i<12;i++)ZAPIS[kk+i]=KOMANDA_ST[st][i];
					}
					else
					if(KOMANDA_TUMS[st][11]!=0) //�᫨ ���� ������⭠� �������
					{ kk=strlen(ZAPIS);
						for(i=0;i<12;i++)ZAPIS[kk+i]=KOMANDA_TUMS[st][i];
					}
				}
	if(ACTIV==1)strncat(ZAPIS,"_A",2);
	ZAPIS[strlen(ZAPIS)]=0xd;
	ZAPIS[strlen(ZAPIS)]=0;
	ito=write(file_arc,ZAPIS,strlen(ZAPIS));
	close(file_arc);
	file_arc=open(NAME_FILE,O_APPEND|O_RDWR,O_BINARY);
	return;
}
//==============================================================================
/*********************************************\
*   ��楤�� ������� ��ୠ�� �������⨪�     *
*           diagnoze(int st,int kan)          *
* st -  ����� �⮩�� (0,1,2....)              *
* kan - ����� ������ (0,1)                    *
\*********************************************/
int diagnoze(int st,int kan)
{
	int nom_serv,strk,error_diag=0;
  unsigned int nm[15],ff;
	unsigned char gru,podgru,bt,kod;
	while(error_diag!=-1)
	{
		if((st<0)||(st>7))error_diag=-1;
		if(kan==0)
		{
			gru=REG_IN[st][4];
			podgru=REG_IN[st][5];
			bt=REG_IN[st][6]&0xf;
			kod=REG_IN[st][7];
		}
		else
		if(kan==1)
		{
			gru=REG1_IN[st][4];
			podgru=REG1_IN[st][5];
			bt=REG1_IN[st][6]&0xf;
			kod=REG1_IN[st][7];
		}
		strk=TAKE_STROKA(gru,podgru-48,st);
		if((strk>=0)&&(error_diag==0))
		{
			switch(gru)//��宦����� ��ꥪ� �ࢥ� 
			{
				case 'E': for(ff=0;ff<5;ff++)nm[ff]=SPSIG[strk][ff];break;
				case 'F': for(ff=0;ff<10;ff++)nm[ff]=SPSP[strk][ff];break;  //$$$$ 13_04_07 㤠���� ��-�� �訡�筮� �������⨪�
				case 'I': for(ff=0;ff<10;ff++)nm[ff]=SPPUT[strk][ff];break; //$$$$ 13_04_07 㤠���� ��-�� �訡�筮� �������⨪�
				default: error_diag=-1;break;
			}
			nom_serv=nm[bt];
			if((nom_serv>=KOL_VO)||(nom_serv<=0))error_diag=-1;
			else
			{
				DIAGNOZ[1]=((out_ob[nom_serv]&0xFF00)>>8)|0x20;//��宦����� ��ꥪ� �����
				DIAGNOZ[0]=out_ob[nom_serv]&0xFF;
			}
		}
		else	error_diag=-1;

		switch(kod)
		{
			case 'P': DIAGNOZ[2]=1;break;
			case 'Z': DIAGNOZ[2]=1;break;
			case 'S': DIAGNOZ[2]=2;break;
			case 'T': DIAGNOZ[2]=4;break;
			default: error_diag=-1;break;
		}
		break;
	}
	return(error_diag);
}
//=============================================================================
/*********************************************\
*   ��楤�� ���஢���� ���� ���           *
*       test_plat(int st,int kan)             *
* st -  ����� �⮩�� (0,1,2....)              *
* kan - ����� ������ (0,1)                    *
\*********************************************/
int test_plat(int st,int kan)
{
	int i,error_plat=0;
	unsigned char podgr,kod,plata,baity[5];
	unsigned int bits=0;
	while(error_plat!=-1)
	{
		if((st<0)||(st>7))error_plat=-1;
		if(kan==0)
		{
			podgr=REG_IN[st][3];
			for(i=0;i<5;i++)baity[i]=REG_IN[st][4+i];
		}
		else
			if(kan==1)
			{
				podgr=REG1_IN[st][3];
				for(i=0;i<5;i++)baity[i]=REG1_IN[st][4+i];
			}
		switch(podgr)
		{
			case 'p': kod=1; plata=1;          //��ꥤ������ ��㯯
								for(i=0;i<3;i++)bits=bits|((baity[i]&0x1f)<<(i*5));
								break;
			case 'q': kod=2; plata=1;    			//���� ��㯯
								for(i=0;i<3;i++)bits=bits|((baity[i]&0x1f)<<(i*5));
								break;
			case 'r': kod=3; plata=9;    //������⢨� 0 � �201-1
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 's': kod=3; plata=10;   //������⢨� 0 � �201-2
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 't': kod=3; plata=11;   //������⢨� 0 � �201-3
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 'u': kod=3; plata=12;   //������⢨� 0 � �201-4
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 'v': kod=4; plata=9;  //������⢨� 1 � M201-1
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 'w': kod=4; plata=10; //������⢨� 1 � M201-2
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			case 'x': kod=4; plata=11;  //������⢨� 1 � M201-3
								for(i=0;i<4;i++)bits=bits|((baity[i]&0x1f)<<(i*4));
								break;
			default: error_plat=-1;
		}

		break;
	}

	ERR_PLAT[0]=0;
	ERR_PLAT[1]=0x14;

	ERR_PLAT[5]=((st+1)&0xf)<<4;
	ERR_PLAT[5]=ERR_PLAT[5]|(((KORZINA[st]+1)&3)<<2);
	ERR_PLAT[5]=ERR_PLAT[5] | ( (plata&0xc)>>2 );

	ERR_PLAT[4]=(plata&0x3)<<6;
	ERR_PLAT[4]=ERR_PLAT[4] | (kod&0x3f);

	ERR_PLAT[2]=bits&0xFF;
	ERR_PLAT[3]=(bits&0xFF00)>>8;

	return(error_plat);
}
//===========================================================================
void ZERO_TRASSA(void)
{
	int ii;		
	for(ii=0;ii<200;ii++)
  {
  	TRASSA[ii].object=0;
    TRASSA[ii].tip=0;
    TRASSA[ii].stoyka=0;
    TRASSA[ii].podgrup=0;
	}
}
//================================================================================
void ZERO_TRASSA1(void)
{
	int ii;		
	for(ii=0;ii<200;ii++)
  {
  	TRASSA1[ii].object=0;
    TRASSA1[ii].tip=0;
    TRASSA1[ii].stoyka=0;
    TRASSA1[ii].podgrup=0;
	}
}			
//================================================
void DeleteMarsh(int i_m)
{
	int i_s,s_m,ii,strelka;

	for(i_s=0;i_s<Nst;i_s++)//�ன� �� �ᥬ �⮩���
	{
		MARSHRUT_ALL[i_m].KOL_STR[i_s]=0; //㤠���� ���稪� ��५�� ��� ��� �⮥�
		MARSHRUT_ALL[i_m].STOYKA[i_s]=0; //㤠���� �ਧ���� �宦����� �⮥� � �������
		for(s_m=0;s_m<10;s_m++) //�ன� �� ⠡��栬 ��५��,ᨣ����� � ��_��
		{
			strelka=MARSHRUT_ALL[i_m].STREL[i_s][s_m]&0xfff;//�뤥���� ����� ��५��
			if(strelka!=0) //�᫨ ������ ����� ��५��
			{
				POOO[strelka]=0l; //����� ���稪 �६��� ��� ��५��
				MARSHRUT_ALL[i_m].STREL[i_s][s_m]=0;//㤠���� ��५�� �� ⠡����
			}
			MARSHRUT_ALL[i_m].SIG[i_s][s_m]=0;//㤠���� ᨣ��� �� ⠡����
			MARSHRUT_ALL[i_m].SP_UP[i_s][s_m]=0; //㤠���� �� ��� �� �� ⠡����
		}
	}
	MARSHRUT_ALL[i_m].KMND=0;    //������ �祩�� �������
	MARSHRUT_ALL[i_m].NACH=0;    //������ �祩�� ��砫�
	MARSHRUT_ALL[i_m].END=0;	//������ �祩�� ����
	MARSHRUT_ALL[i_m].NSTR=0;	//������ �᫮ ��५��
	MARSHRUT_ALL[i_m].POL_STR=0;	//������ ��������� ��५��
	MARSHRUT_ALL[i_m].SOST=0;    //������ ���稪 ���ﭨ� ������쭮�� �������

	for(i_s=0;i_s<Nst;i_s++)	//�ன� �� �⮩���
	for(s_m=0;s_m<MARS_STOY;s_m++) //�ன� �� ������� � �⮩��
	{
		if((MARSHRUT_ST[i_s][s_m].NUM-100)==i_m) //�᫨ ������� ��� 㤠�塞���
		{
			for(ii=0;ii<13;ii++)MARSHRUT_ST[i_s][s_m].NEXT_KOM[ii]=0; //㤠���� �������
			TUMS_RABOT[i_s]=0; //$$$$ 13_04_07 - ࠧ�����஢���� �⮩�� �� ���ਭ�訩 �������
			MARSHRUT_ST[i_s][s_m].NUM=0; //㤠���� ����� ������쭮��
			MARSHRUT_ST[i_s][s_m].SOST=0; //������ ���ﭨ�
			MARSHRUT_ST[i_s][s_m].T_VYD=0l; //������ �६� �뤠�
			MARSHRUT_ST[i_s][s_m].T_MAX=0l; //������ �६� ���ᨬ㬠
			MARSH_VYDAN[i_s]=0; //㤠���� �ਧ��� �뤠�
		}
	}
	return;
}
//=======================================================================
void  Analiz_Glob_Marsh(void)
{
	int i_m, i_s, s_m,ii,mars_st,ij,ik,ijk,ob_str,polojen;
	unsigned int KOM;
	time_t t_tek;
	double Delta;
	unsigned char kateg, Sost=0;
	// �ன� �� �ᥩ ������쭮� ⠡��� �����⮢
	for(i_m=0; i_m<Nst*3; i_m++)
	{
		if(MARSHRUT_ALL[i_m].SOST==0)continue;//�᫨ ��ப� ����� - � ᫥���饩
		kateg=0xC0&MARSHRUT_ALL[i_m].SOST; //����� ��⥣��� ������쭮�� �������
		mars_st=0; //��⠥� ������� �� ஧�����
		Sost=0x3f; //����砫쭮 ����뢠�� �� �ᯥ譮� �����襭��

		for(i_s=0;i_s<Nst;i_s++) //�ன� �� �ᥬ �⮩���
		{ if(MARSHRUT_ALL[i_m].STOYKA[i_s]!=0)	//�᫨ �⮩�� ������
			{
				for(s_m=0;s_m<MARS_STOY;s_m++)  	//�ன� �� �ᥬ ������� � �⮩��
				{
					if(MARSHRUT_ST[i_s][s_m].NUM==0)continue;//�᫨ ��� - � ᫥���饬�

					if((MARSHRUT_ST[i_s][s_m].NUM-100)==i_m) //�᫨ ������
					{
						mars_st++; //㢥����� ���稪 �⮥�
						//�᫨ �� �뤠�, ��३� � ᫥���饬�
						if(MARSHRUT_ST[i_s][s_m].T_VYD==0)
						MARSHRUT_ST[i_s][s_m].SOST=(MARSHRUT_ST[i_s][s_m].SOST&0xC)|0x7;				
						if((MARSHRUT_ST[i_s][s_m].SOST&0x1f)!=0x1f)//�᫨ ������� �� ���ਭ��
						{
#ifdef WORK
							if((T_TIME-MARSHRUT_ST[i_s][s_m].T_VYD)>2)//�᫨ ��諮 ����� 2 ᥪ �� �뤠�
#else
							if((T_TIME-MARSHRUT_ST[i_s][s_m].T_VYD)>2)
#endif
							{
								if(MARSHRUT_ST[i_s][s_m].T_VYD!=0) //�᫨ 䨪�஢����� �뤠�
								{
									if(KOL_VYD_MARSH[i_s]==0)  				//$$$$ 13_04_07 �᫨ �뫠 ���� �뤠�
									{
										MARSHRUT_ST[i_s][s_m].T_VYD=0;		//$$$$
										MARSHRUT_ST[i_s][s_m].SOST=MARSHRUT_ST[i_s][s_m].SOST&0x7; //$$$$
										KOL_VYD_MARSH[i_s]++;				//SSSS
										TUMS_RABOT[i_s]=0;					//SSSS
										KOMANDA_TUMS[i_s][10]=0;			//SSSS
									}
									else
									{
										add(i_m,8888,40);
										DeleteMarsh(i_m);//㤠���� �������
										return;//�४���� ������� ������⮢
									}
								}
							}
						}
						else //�᫨ ������� ���ਭ��
						{
							//�ன� �� �ᥬ ��५��� ���ਭ�⮣� �������
							for(ik=0;ik<10;ik++)
							{ //������� ��।��� ��५��
								ob_str=MARSHRUT_ALL[i_m].STREL[i_s][ik]&0xfff;
								polojen=MARSHRUT_ALL[i_m].STREL[i_s][ik]&0x1000;
								if(ob_str==0)continue;//�᫨ �� ���, ��� �����
								read_FR3(ob_str); //������ ���ﭨ� ��५��
								//�᫨ ��५�� ��� ����஫� - ��ࢠ�� ��ᬮ��
								if(FR3[1]==FR3[3])
							break; //��� ����஫� - ����� ������ ��५��
								if(polojen==0)//�᫨ �㦭� � ����
								{
									if((FR3[1]!=1)||(FR3[3]!=0))
							break; //�� � ���� - ����� ��५��
								}
								else //�᫨ �㦭� � �����
								{
									if((FR3[1]!=0)||(FR3[3]!=1))
							break; //�� � ����� - ����� ��५��
								}
							}
							if(ik>=10) //�᫨ �� ��५�� ��⠭������ (������� �믮����)
							{
								//��⠭����� ��� �����쭮�� - ���� �����襭��
								MARSHRUT_ST[i_s][s_m].SOST=
								MARSHRUT_ST[i_s][s_m].SOST|0x3f;
							}
							else //�᫨ ��५�� �� ��⮢�
							{
								MARSHRUT_ST[i_s][s_m].SOST=
								(MARSHRUT_ST[i_s][s_m].SOST&0xC0)|0x1f; //�࠭��� �����⨥
								if((T_TIME-MARSHRUT_ST[i_s][s_m].T_VYD)> //�᫨ �ॢ�襭 ����.
								MARSHRUT_ST[i_s][s_m].T_MAX)
								{
									add(i_m,8888,41);
									DeleteMarsh(i_m);
									return;
								}
							}
						}
						Sost=(Sost&MARSHRUT_ST[i_s][s_m].SOST);// ����� ���ﭨ� � ��ࢠ����
				break; // ����� ������ �������� � �⮩ �⮩�� ��� �⮣� ������쭮��
					}
				}
			}
		}

		if(mars_st!=0)//�᫨ � ������� �室�� ��� �� ���� �⮩��
		{
			MARSHRUT_ALL[i_m].SOST=kateg|Sost; //�ନ�㥬 ����� �⮩�� � ����.����.
			//�᫨ ���ﭨ� ������� = 㤠筮� �����襭��
			if((Sost&0x3F)==0x3f)
			{
				 //�᫨ �����訫�� �।���⥫�� �������
				if(kateg==0x40)	{PovtorMarsh(i_m);continue;}
				else //�᫨ �� �ᯮ���⥫�� �������
				{
					add(i_m,8888,26);
					DeleteMarsh(i_m);
					continue;
				}
			}
			else
			//�᫨ ���ﭨ� ��㤠筮�
			if((Sost&0x3f)==0x1)
			{
				add(i_m,8888,21);
				Soob_For_Arm(ii,MARSHRUT_ALL[i_m].NACH,ii);
				DeleteMarsh(i_m);
				return;
			}
		}
	}
}
//==================================================================
//����� ������� ��� �ନ஢���� ������� ��᫥ ��⠭���� ��५��
void PovtorMarsh(int i_m)
{
	int ii;
	char KMND;
	unsigned int NACH=MARSHRUT_ALL[i_m].NACH; //����� ��砫� �������
	int END=MARSHRUT_ALL[i_m].END; //����� ����� �������
	int NSTR=MARSHRUT_ALL[i_m].NSTR;//����� �᫮ ��५��
	unsigned long POL=MARSHRUT_ALL[i_m].POL_STR; //����� ��������� ��५��
	for(ii=0;ii<Nst;ii++) //�ன� �� �ᥬ �⮩���
	{
		//�᫨ �뤠�� ������⭠� �������, � ���� ��� ॠ�樨 - ��室���
		if(MARSH_VYDAN[ii]!=0)return;
	}
	switch(MARSHRUT_ALL[i_m].KMND) //��ନ஢��� �������
	{
		case 'a': KMND=191; break;
		case 'b': KMND=192; break;
		case 'd': KMND=71; break;
		default:  break;
	}
	add(i_m,8888,20); //�������, �� �뤠�� � ����⨥�
	DeleteMarsh(i_m); //㤠���� ���� ������� �� ⠡����
	ii=ANALIZ_MARSH(KMND,NACH,END,NSTR,POL); //�஢��� ������
	if(ii<Nst*3)TUMS_MARSH(ii);//�᫨ ������� ���ᠫ�� � ⠡���� - ������ � �����
	return;
}
//=========================================================================
/****************************************************\
* ��楤�� ������� ������ � ���� ��५�� � ���  *
* �� ��㣮� �⮩�� ANALIZ_ST_IN_PUT()                *
\****************************************************/
int ANALIZ_ST_IN_PUT(int nom_tras,int kom,int st,int marsh,int ind)
{
	int ii,Error;
	long DT;
	int str_in_put,sp_in_put,spar_str,spar_sp,pol_str,tms_str;
	ii=nom_tras;
	Error=0;
	if((TRASSA[ii].tip==7)&&(kom==98))
	{
  	READ_BD(TRASSA[ii].object);
    //�᫨ ����ᨬ���� �� ��� ��������� ������� ��� ������� �� ��������
    if((bd_osn[1]!=15)||(kom!=98))return 0;
    str_in_put=bd_osn[2];//������� ��ꥪ� ��� ��५�� � ���
    sp_in_put=bd_osn[3]; //������� ��ꥪ� ��� �� �⮩ ��५��
		spar_str=bd_osn[4];  //������� ��ꥪ� ��� ᯠ७��� ��५��
    spar_sp=bd_osn[5];  //������� ��ꥪ� ��� �� ᯠ७��� ��५��
    pol_str=bd_osn[6];  //������� �ॡ㥬�� ��������� ��५�� � ���
		READ_BD(str_in_put); //������ ���� ��५�� � ���
    tms_str=((bd_osn[13]&0x0f00)>>8)-1; //������� �⮩�� ��� ��५�� � ���
    if(pol_str==0)//�᫨ ��५�� ������ ���� � ����
    { //�᫨ ��५�� �� � ����
      read_FR3(str_in_put);
    	if((FR3[1]!=1)||(FR3[3]!=0))
			{
				MARSHRUT_ALL[marsh].KOL_STR[tms_str]++;
				MARSHRUT_ALL[marsh].STREL[tms_str][ind++]=str_in_put;
				MARSHRUT_ALL[marsh].SOST=MARSHRUT_ALL[marsh].SOST&0xc0;
				MARSHRUT_ALL[marsh].SOST=MARSHRUT_ALL[marsh].SOST|0x3;
				read_FR3(sp_in_put);
				if((FR3[1]==0)&& //�᫨ ᢮� �� � ��ଥ
				(FR3[3]==0)&&
				(FR3[5]==0)&&
				(FR3[11]==0))
				{
					if(spar_str!=0) //�᫨ ���� ��ୠ� ��५��
					{
						read_FR3(sp_in_put);
						if(FR3[1]==0) //�᫨ ᢮� �� � ��ଥ
						{
							read_FR3(spar_sp);
							if((FR3[3]==0)&&  //�᫨ �� ���� � ��ଥ
							(FR3[5]==0)&&
							(FR3[11]==0))
							{
								if(POOO[str_in_put]==0l)
								{
									perevod_strlk(107,str_in_put);
									POOO[str_in_put]=time(NULL);
								}
								else
								{
									DT=time(NULL)-POOO[str_in_put];
									if(DT>40l)
									{

										add(marsh,8888,16);
										DeleteMarsh(marsh);
										POOO[str_in_put]=0l;
										Error=1015;
									}
								}
							}
							else Error=1015; //�᫨ �� ���� �� � ��ଥ
						}
						else  Error=1015; //�᫨ ᢮� �� �� � ��ଥ
					} //�᫨ ��� ��୮� ��५��
					else
					{
						if(POOO[str_in_put]==0l)
						{
							perevod_strlk(107,str_in_put);
							POOO[str_in_put]=time(NULL);
						}
						else
						{
							DT=time(NULL)-POOO[str_in_put];
							if(DT>40l)
              {
								add(marsh,8888,17);
								DeleteMarsh(marsh);
								POOO[str_in_put]=0l;
                Error=1015;
              }
            }
          }
        }
        else  Error=1015;//�᫨ ᢮� �� �� � ��ଥ
      }
		}//����� ���ᮢ��� ��������� ��५��
		if(pol_str==1)//�᫨ ��५�� ������ ���� � �����
		{
			read_FR3(str_in_put);
			if((FR3[3]!=1)||(FR3[1]!=0))//�᫨ ��५�� �� � �����
			{
				MARSHRUT_ALL[marsh].KOL_STR[tms_str]++;
				MARSHRUT_ALL[marsh].STREL[tms_str][ind++]=str_in_put|0x1000;
				MARSHRUT_ALL[marsh].SOST=MARSHRUT_ALL[marsh].SOST&0xc0;
				MARSHRUT_ALL[marsh].SOST=MARSHRUT_ALL[marsh].SOST|0x3;
				read_FR3(sp_in_put);
        if((FR3[1]==0)&& //�᫨ ᢮� �� � ��ଥ
        (FR3[3]==0)&&
        (FR3[5]==0)&&
        (FR3[11]==0))
        {
          if(spar_str!=0) //�᫨ ���� ��ୠ� ��५��
          {
            read_FR3(spar_sp);
            if((FR3[3]==0)&& //�᫨ �� ���� � ��ଥ
          	(FR3[5]==0)&&
           	(FR3[11]==0))
           	{
             	if(POOO[str_in_put]==0l)
             	{
               	perevod_strlk(127,str_in_put);
               	POOO[str_in_put]=time(NULL);
             	}
             	else
             	{
               	DT=time(NULL)-POOO[str_in_put];
               	if(DT>40l)
               	{
									add(marsh,8888,18);
									DeleteMarsh(marsh);
									POOO[str_in_put]=0l;
                 	Error=1015;
               	}
             	}
           	}
           	else    Error=1015;// �᫨ �� ���� �� � ��ଥ
					}
          else  //�᫨ ��� ��୮� ��५��
          {
            if(POOO[str_in_put]==0l)
            {
              perevod_strlk(127,str_in_put);
              POOO[str_in_put]=time(NULL);
            }
            else
            {
              DT=time(NULL)-POOO[str_in_put];
							if(DT>40l)
              {
								add(marsh,8888,19);
								DeleteMarsh(marsh);
								POOO[str_in_put]=0l;
								Error=1015;
							}
						}
					}
				}
				else Error=1015;//�᫨ ᢮� �� �� � ��ଥ
			}
		}//����� ������� ��५�� � �����
	}//����� ������� ��ꥪ⮢
	return Error;
}
//===============================================================
//������ �ਧ����� ���ﭨ� ࠡ��� �⮥� � ������⠬� �� �����,
//�ਭ��� �� �⮩��, ����� PODGR - ���� � ����묨 � MYTHX
ANALIZ_MYTHX(unsigned char PODGR)
{
	int svoi,tum,s_m,nom,ik,ob_str,ijk;
	char sym_myt;
	unsigned char prov;
	 //�᫨ �� �� MYTHX, � ���⠭�� , � ���
	if(((PODGR<0x59)||(PODGR>0x7C))&&(PODGR!=0x6E))return;

	svoi=0;//����砫쭮 ��⠥�, �� MYTHX �㦮�

	if(ACTIV!=1)//�᫨ �ࢥ� ���ᨢ��, � ���塞 �� ����� �⮩��
							//⥪�饥 �������饥 ���祭��
	{
		switch(PODGR&0xF)
		{
			case 0x9: MYTHX_TEC[ST-1]=0x50;break;
			case 0xA: MYTHX_TEC[ST-1]=0x60;break;
			case 0xC: MYTHX_TEC[ST-1]=0x70;break;
			default: break;
		}
	}

	tum=ST-1;

	//��४���⥫� �� �������饬� ��� �⮩�� ��������
	switch(MYTHX_TEC[tum])
	{
		//�᫨ �������� 1-� ������� � �⮩�� �������� � ��ࢮ�,� ᢮�
		case 0x50: if((PODGR&0xf)==0x9)svoi=0xf;break;

		//�᫨ ������� 2-�� � �⮩�� �������� � ��஬, � ᢮�
		case 0x60: if((PODGR&0xf)==0xA)svoi=0xf;break;

		//�᫨ ��� ��������, � �� ᢮�
		case 0x6e:
		case 0:     svoi=0xf;break;

			//�᫨ ������� 3-�� � �⮩�� �������� � ���쥬, � ᢮�
		case 0x70: if((PODGR&0xf)==0xC)svoi=0xf;
							 else
								if(PODGR==0x6e)svoi=0xf;
								svoi=0xf;
		default: break;
	}

	if(svoi!=0)//�᫨ ᢮�
	{
		//�᫨ ���������
		if(MYTHX[tum]!=PODGR)MARSH_VYDAN[tum]=0; //���� �ਧ��� ࠧ��� �������

		for(s_m=0;s_m<MARS_STOY;s_m++) // �ன� �� �ᥬ ������⠬ �⮩��
		{
			//�뤥���� MYTHX �� �����㯯� � �������� �� MYTHX �������
			// � 12 - �� ���� ����� MYTHX, � ����� 諠 �������
			// � �����㯯� � ����襬 ���㡠�� ����� MYTHX �������騩 � �����
			// 59 - 1� �뤠� � ���� � ࠡ��
			// 6A - ��ன �뤠� � ��ன � ࠡ��
			// 7C - ��⨩ �뤠� � ��⨩ � ࠡ��
			prov=MARSHRUT_ST[tum][s_m].NEXT_KOM[12]|(PODGR&0xf);
			// �᫨ ��� � �⮩�� ᮮ⢥����� �뤠����� � �������
			if((prov==0x59)||(prov==0x6A)||(prov==0x7C))
			{
				switch(PODGR&0xF0)
				{            //������� � ࠡ��
										 //��⠭����� ���ﭨ� ������� � ���� �������
					case 0x70: MARSHRUT_ST[tum][s_m].SOST=
										 (MARSHRUT_ST[tum][s_m].SOST&0xC0)|0x1f;
										 for(ijk=0;ijk<15;ijk++)KOMANDA_TUMS[tum][ijk]=0;
										 TUMS_RABOT[tum]=0xf; //���⠢��� 䫠� ���� � ࠡ��
										 break;

										 //��㤠筮� ����砭��
					case 0x50: MARSHRUT_ST[tum][s_m].SOST=0x1; //��⠭����� ��㤠��
										 //����� �������
										 for(ijk=0;ijk<15;ijk++)KOMANDA_TUMS[tum][ijk]=0;
										 //���� 䫠� ���� � ࠡ��
										 TUMS_RABOT[tum]=0;
										 break;

										 //㤠筮� ����砭��
					case 0x60: MARSHRUT_ST[tum][s_m].SOST=
										 (MARSHRUT_ST[tum][s_m].SOST&0xC0)|0x1f;
										 for(ijk=0;ijk<15;ijk++)KOMANDA_TUMS[tum][ijk]=0;
										 TUMS_RABOT[tum]=0;
										 break;

						default: break;
				}
				break;
			}
			else //�᫨ ���� �� ᮮ⢥������
			{
				switch(PODGR&0xF0)
				{
					case 0x70: TUMS_RABOT[tum]=0xf;break; //�⮩�� �����

					case 0x50: TUMS_RABOT[tum]=0;break;

					case 0x60: TUMS_RABOT[tum]=0; break;

					default: break;
				}
				if(PODGR==0x6e)TUMS_RABOT[tum]=0; //�⮩�� ��१���㧨����

			}
		}
	}

	if(svoi!=0)MYTHX[tum]=PODGR; //�᫨ �� ᢮� MYTHX, ��������� ���

	if(MYTHX[tum]==0)MYTHX[tum]=PODGR;//�᫨ ���� �ਥ� - ���������

	switch(MYTHX[tum]&0xf)
	{
		case 0x9: sym_myt='1';break;
		case 0xA: sym_myt='2';break;
		case 0xC: sym_myt='3';break;
		default:  sym_myt=MYTHX[tum];break;
	}
	switch(MYTHX[tum]&0xf0)//��४���⥫� �� ���ﭨ� �⮩��
	{
		case 0x70:	putch1(sym_myt,14,66+tum,50);break; //� ࠡ��
		case 0x60:  putch1(sym_myt,10,66+tum,50);break; // 㤠�� �����
		case 0x50:  putch1(sym_myt,12,66+tum,50); break;// ��㤠�� �����
			default:  putch1(sym_myt,140,66+tum,50); break;// ��㤠�� �����
	}
	switch(MYTHX_TEC[tum]&0xf0)//��४���⥫� �� ���ﭨ� �������
	{
		case 0x70:	putch1('3',14,66+tum,49); break; //����� ��⨩
		case 0x60:  putch1('2',10,66+tum,49); break; //����� ��ன
		case 0x50:  putch1('1',12,66+tum,49); break;// ����� ����
			default:	putch1('?',140,66+tum,49);break; //������ ����
	}
	return;
}