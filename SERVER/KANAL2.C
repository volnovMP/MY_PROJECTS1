/*************************************************************************\
*                                                                         *
*  KANAL.C - 䠩� �����쭮�� ������ �ࢥ� ��� ������                    *
*                                                                         *
\*************************************************************************/

#include "opred2.h"
#include "extern2.h"
/********************************************\$$$$����⠥� �� ���뢠��� ������
*    ��楤�� �뤠� ������ � �⮩�� ����   *
*           vidacha1(unsigned int ad)        *
*     ad - ��६ ���� �뤠� ������         *
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
/*****************************************\$$$$����⠥� �� ���뢠��� ������
* ��楤�� �뤠� ������ � ᫥���騩     *
* �ࢥ�   out_next()                     *
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

/******************************************\$$$$����⠥� �� ���뢠��� ������
* ��楤�� �뤠� ������ � �।��騩     *
* �ࢥ�   out_pred()                      *
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
/****************************************\$$$$����⠥� �� ���뢠��� ������
*  ��楤�� �뤠� ������ �� �᭮�����  *
*	 ������ ���   out_arm_osn()            *
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
/****************************************\$$$$����⠥� �� ���뢠��� ������
*  ��楤�� �뤠� ������ �� १�ࢭ��� *
*  ������ ��� out_arm_rez()              *
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
/***************************************\$$$$����⠥� �� ���뢠��� ������
*  ��楤�� �뤠� ������ �� �᭮����� *
* ������ ��� �� *                       *
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
/*******************************\$$$$����⠥� �� ���뢠��� ������
* ��楤�� �ਥ�� ������ ��    *
* ᫥���饣� �ࢥ�  in_next() *
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
/*******************************\$$$$����⠥� �� ���뢠��� ������
* ��楤�� �ਥ�� ������ ��    *
* �।��饣� �ࢥ� in_pred() *
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
/**********************************\ $$$$����⠥� �� ���뢠��� ������
* ��楤�� �ਥ�� ������ �� ���   *
* �� �᭮����� ������ in_arm_osn() *
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
/***********************************\ $$$$����⠥� �� ���뢠��� ������
* ��楤�� �ਥ�� ������ �� ���    *
* �� १�ࢭ��� ������ in_arm_rez() *
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
/***********************************\ $$$$����⠥� �� ���뢠��� ������
* ��楤�� �ਥ�� ������ �� ��� �� *
* �� �᭮����� ������ in_arm_osn()  *
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
* ��楤�� �ਥ�� ������ �� ����   *
* �� �᭮����� ������ in_tums_osn() *
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
	BUF_IN[n_tums++]=symbol;//� ����
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
* ��楤�� �ਥ�� ������ �� ����    *
* �� १�ࢭ��� ������ in_tums_rez() *
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
		BUF_IN1[n_tums1++]=symbol1;//� ����
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
* ��楤�� �뤠� ������ � �ᥤ��� �ࢥ��  *
*                                             *
\*********************************************/
void OUT_PAKET_PRED_NEXT(void)
{ unsigned int i,CRC16,ob_fr4;
	//ࠡ�� � ����ﬨ ����⮢ � �।��騩 �ࢥ�
	//������ ����� �᭮����� ������
	for(i=0;i<21;i++)
	{
		//����訩 ����
		BUF_OUT_PRED[2*i+1]=OBJ_ARMu[N_PAKET].OBJ[i]&0xff;
		//���訩 ����
		BUF_OUT_PRED[2*i+2]=(OBJ_ARMu[N_PAKET].OBJ[i]&0xff00)>>8;
	}
	BUF_OUT_PRED[43]=PAKETs[N_PAKET].KS_OSN&0xff;//����訩
	BUF_OUT_PRED[44]=(PAKETs[N_PAKET].KS_OSN&0xff00)>>8;//���訩
	//������ ����� १�ࢭ��� ������
	for(i=0;i<21;i++)
	{
		//����訩 ����
		BUF_OUT_PRED[2*i+45]=OBJ_ARMu1[N_PAKET].OBJ[i]&0xff;
		//���訩 ����
		BUF_OUT_PRED[2*i+46]=(OBJ_ARMu1[N_PAKET].OBJ[i]&0xff00)>>8;
	}
	BUF_OUT_PRED[87]=PAKETs[N_PAKET].KS_REZ&0xff;//����訩
	BUF_OUT_PRED[88]=(PAKETs[N_PAKET].KS_REZ&0xff00)>>8;//���訩
	//ࠡ�� � ��ꥪ⠬� �� FR4
	for(i=0;i<10;i++)
	{
		if(NOVIZNA_FR4[i]!=0)
		{
			BUF_OUT_PRED[91]=NOVIZNA_FR4[i]&0xff;      //������� ����� ��ꥪ�
			BUF_OUT_PRED[92]=(NOVIZNA_FR4[i]&0xfff)>>8;
			BUF_OUT_PRED[93]=FR4[NOVIZNA_FR4[i]&0xfff];//������� ��࠭�祭��
			ob_fr4=NOVIZNA_FR4[i]&0xfff;//��������� ��ꥪ� FR4
			add(FR4[ob_fr4],200,ob_fr4);//������� � ��娢
			// ����訢���� 䫠���
			// 0x2000 - ��ࢠ� ��।�� �ᥤ�
			// 0x4000 - ���� ��।�� �ᥤ�
			if((NOVIZNA_FR4[i]&0x2000)==0)NOVIZNA_FR4[i]=NOVIZNA_FR4[i]|0x2000;
			else
				if((NOVIZNA_FR4[i]&0x4000)==0)NOVIZNA_FR4[i]=NOVIZNA_FR4[i]|0x4000;
			//�᫨ ��।��� 2 ࠧ� �ᥤ� � � ����, � ���� �������
			if((NOVIZNA_FR4[i]&0x7000)==0x7000)NOVIZNA_FR4[i]=0;
			break;
		}
	}
	if(i==10)				//�᫨ �� �뫮 ������� �� FR4
	{
		for(i=LAST_FR4;i<KOL_VO;i++)//���� ��।���� ��⠭�������� ��࠭�祭��
		{
			if(FR4[i]!=0) //�᫨ ������� ��࠭�祭��
			{
				BUF_OUT_PRED[91]=i&0xff;       // ������� ����� ������� ����
				BUF_OUT_PRED[92]=(i&0xfff)>>8; // ������� ����� ������ ����
				BUF_OUT_PRED[93]=FR4[i];       // ������� ᠬ� ��࠭�祭��

				if(fixir!=0)                  // �᫨ ����㯨�� �६� 䨪�樨
				{
					if(ZAFIX_FR4[i]==0)   //�᫨ ��࠭�祭�� �� 䨪�஢�����
					{
						add(FR4[i],200,i);  //������� ��࠭�祭�� � ��娢
						ZAFIX_FR4[i]=0xf;   //��⠭����� �ਧ��� 䨪�樨 �����
					}
				}
				break;
			}
		}
		LAST_FR4=i+1;
		if((LAST_FR4>=KOL_VO)||(LAST_FR4<=0))LAST_FR4=0;
	}
	//ࠡ�� � ����ﬨ ����⮢ � ᫥���騩 �ࢥ�
	for(i=0;i<98;i++)BUF_OUT_NEXT[i]=BUF_OUT_PRED[i];

	//�᫨ ��諮 ����� 65 ᥪ㭤, � ����� �� ������� �ਭ㤨⥫쭮 
	if(FLAG_KOM==0)//�᫨ ��� ������
	{
		if(T_MIN_NEXT>1092)//�᫨ ��諠 1 �����
		{
			if(sosed_NEXT>0)//�᫨ ᫥���騩 �ᥤ "�����" - ��।��� �ࠢ�����
			{ putch1(0x1a,0xa,34,50); //�뢮� �� ��࠭ ��५�窨 ��ࠢ� "nxt"
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
    if((T_MIN_PRED>1092))//�᫨ ��諠 1 �����
    {
      if(sosed_PRED>0)//�᫨ �।��騩 �ᥤ "�����" - ��।��� �ࠢ�����
      { putch1(0x1b,0xc,29,50);//�뢮� �� ��࠭ ��५�窨 ����� "prd"
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

  BUF_OUT_PRED[0]='*'; //��� ��砫�
	BUF_OUT_NEXT[0]=0x12;
	BUF_OUT_PRED[97]=0xd;
	BUF_OUT_NEXT[97]=0xd;
	return;
}
/*********************************************************\
*                                                         *
* ARM_OUT() �����⮢�� ������ ��� ��।�� � ⥪�騩 ���  *
*                                                         *
\*********************************************************/
void ARM_OUT(void)
{
	int i,j,k,n_out,n_arm,n_arm1,ARM,ARM1,stoyka,bait,ii;
	unsigned int CRC; //����஫쭠� �㬬� CRC-16
	unsigned char
	OUT_NOM[2], //����� ��ꥪ� �뢮�� (��ꥪ� ��� ���)
	OUT_BYTE,   //���� ���ﭨ� ��ꥪ� ��� ���
	ZAGOL,      //���� ��������� ��� ᮮ�饭�� � ��� ���
	ZAGOL_SHN,  //���� ��������� ��� ᮮ�饭�� � ��� ��
	SOST;       //���� ���ﭨ� ��ꥪ� ��� ���
	if(cikl_arm>=2) //�᫨ ��諮 �६� ��।�� � ��
	{
		for(i=0;i<100;i++)//������ ���� �����-�뢮��
		{
			BUF_IN_ARM[i]=0;BUF_IN_ARM1[i]=0;BUF_OUT_ARM[i]=0;BUF_OUT_ARM1[i]=0;
		}
		//�ନ஢���� ���᭮�� ����ᮢ
		ZAPROS_ARM++;  if(ZAPROS_ARM>8)ZAPROS_ARM=4;
		ZAPROS_ARM1++; if(ZAPROS_ARM1>8)ZAPROS_ARM1=4;

		//������� ������� �᭮����� � १�ࢭ��� �����
		ARM=ZAPROS_ARM-4;  ARM1=ZAPROS_ARM1-4;

		// ��������� ���⠭樨 �� �������
		// � ������ 㪠��⥫� �� ᢮�.���� � ����
		n_arm=ZAPOLNI_KVIT(ARM,0);
		n_arm1=ZAPOLNI_KVIT(ARM1,1);

		n_out=n_arm;


		OBJ_ARMu[N_PAKET].LAST=0;
		if((DIAGNOZ[1]!=0)||(DIAGNOZ[2]!=0))
		{
			BUF_OUT_ARM[n_out++]=DIAGNOZ[0];DIAGNOZ[0]=0;
			BUF_OUT_ARM[n_out++]=DIAGNOZ[1];DIAGNOZ[1]=0;
			BUF_OUT_ARM[n_out++]=DIAGNOZ[2];
			DIAGNOZ[2]=0; //������� � ���� �������⨪�
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
			if(rg.x.cflag==0) //�᫨ �ᯥ譮� �⥭�� �ᮢ 
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
			if(rg.x.cflag==0) //�᫨ �ᯥ譮� �⥭�� ����
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
		for(i=0;i<MAX_NEW;i++)//�ன� �� ����� ����饩�� ������� 
		{
			if(NOVIZNA[i]==0)continue;//�᫨ ��� �������-��३� � ᫥�.�祩��

			//�᫨ ��ꥪ� �� �।����� ����
			if((NOVIZNA[i]>=KOL_VO)||(NOVIZNA[i]<=0)){NOVIZNA[i]=0;continue;}

			//�᫨ ������� ����-������ ⥪�饥 ���ﭨ� ��ꥪ� 
			read_FR3(NOVIZNA[i]);
			//������ ����� ��ꥪ� ��� ����� 
		  OUT_NOM[0]=(out_ob[NOVIZNA[i]]&0xFF00)>>8;
      OUT_NOM[1]=out_ob[NOVIZNA[i]]&0xff;
			 //�᫨ ��ꥪ� �� ��� ��� ��祣� �� ������ � ����� �������
			if((OUT_NOM[1]==32)&&(OUT_NOM[0]==32)){NOVIZNA[i]=0;continue;}
			if((OUT_NOM[1]==0)&&(OUT_NOM[0]==0)){NOVIZNA[i]=0;continue;}

			OUT_BYTE=0; //���� ���ﭨ� ��ꥪ�
			//��������� ���� ���ﭨ� ⥪�騬 ���ﭨ�� ��ꥪ� 
			for(j=0;j<8;j++){ k=1<<j;if(FR3[2*j+1]==1)OUT_BYTE=OUT_BYTE|k;}
//			news=0xF;
			//������� � ���� �뢮�� ��� ����� ��� ������� ��ꥪ� 
			BUF_OUT_ARM[n_out++]=OUT_NOM[1];
			BUF_OUT_ARM[n_out++]=OUT_NOM[0];
			BUF_OUT_ARM[n_out++]=OUT_BYTE;  //������� � ���� ���ﭨ� ��ꥪ� 
			FR3[27]=FR3[27]|0x1f;           //��⠭����� ���� ��।�� � ����
			FR3[26]=FR3[26]|(N_PAKET&0x1F); //��������� ����� ����� ��।��
			write_FR3(NOVIZNA[i]);

			j=OBJ_ARMu[N_PAKET].LAST++;
			//������� ��।������ ��ꥪ� � �������� �����
			OBJ_ARMu[N_PAKET].OBJ[j]=NOVIZNA[i];
			NOVIZNA[i]=0;//㤠���� ��ꥪ� �� ������� 
			if(n_out>64)break;
		}

		if(n_out<65)//�᫨ � ���� ��।�� ��⠫��� ���� 
		//�ன� �� ����� ����饩�� ������� � ��࠭�祭���
		for(i=0;i<10;i++)
		{
			if(NOVIZNA_FR4[i]==0)continue;//�᫨ ��� �������-��३� � ᫥�.�祩��
			if((NOVIZNA_FR4[i]&0xfff)>=KOL_VO)
			{
				NOVIZNA_FR4[i]=0;
				continue;//�᫨ ��� ��ꥪ� 
			}
			OUT_NOM[0]=(out_ob[NOVIZNA_FR4[i]&0xFFF]&0xff00)>>8;//������ ����� ��� �����
			OUT_NOM[1]=out_ob[NOVIZNA_FR4[i]&0xFFF]&0xff;
			if((OUT_NOM[1]==32)&&(OUT_NOM[0]==32))
			{
				NOVIZNA_FR4[i]=0;
				continue;//�᫨ ��ꥪ� �� ��� ���,
			}
			if((OUT_NOM[1]==0)&&(OUT_NOM[0]==0))
			{
				NOVIZNA_FR4[i]=0;
				continue;//� ��३� � ᫥�.�祩��
			}
//			news=0xf;
			OUT_BYTE=FR4[NOVIZNA_FR4[i]&0xfff];
			BUF_OUT_ARM[n_out++]=OUT_NOM[1];//������� � ���� �뢮�� ��� ����� ���
			BUF_OUT_ARM[n_out++]=OUT_NOM[0]|0x80;//������� ��ꥪ� 
			BUF_OUT_ARM[n_out++]=OUT_BYTE;  //������� � ���� ���ﭨ� ��ꥪ�

			NOVIZNA_FR4[i]=NOVIZNA_FR4[i]+0x1000; //��⠭����� 䫠� ��।�� � ���
			if((NOVIZNA_FR4[i]&0x3000)==0x3000)   //�᫨ ��।��� �����,�
			NOVIZNA_FR4[i]=0;											//㤠���� ��ꥪ� �� �������
			if(n_out>64)break;
		}
		if(i==10)new_fr4=0;

a1:
		//����� �� ���� ���������� �᭮���� �����
		if(n_out<65)
		{
			for(i=povtor_out;i<KOL_VO;i++)
			{
				//������ ���ﭨ� ��ꥪ�
				read_FR3(i);
				stoyka=(FR3[28]&0xF0)>>4;//�뤥���� �⮩��
				if(stoyka==0)continue;
				bait=FR3[28]&0xf;        //�뤥���� ����
				if(SVAZ_TUMS[stoyka-1]>=80)//�᫨ ����ﭠ ��� � ����
				{

					if(FR3[29]>=48)
					{
						FR3[11]=1; //���易�� �����䠧�����
						VVOD[stoyka-1][FR3[29]-48][bait-1]=VVOD[stoyka-1][FR3[29]-48][bait-1]|0x20;
						READ_BD(i);
						if(bd_osn[0]==1)//�᫨ ��५��
						{
							if(bd_osn[12]!=9999)//�᫨ ᯠ७���
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
				//������ ����� ��ꥪ� �����
				OUT_NOM[0]=(out_ob[i]&0xFF00)>>8;
				OUT_NOM[1]=out_ob[i]&0xff;
				//�᫨ ��ꥪ� �� ��� ����, � ��祣� �� ������
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
					BUF_OUT_ARM[n_out++]=OUT_NOM[1];//������� � ���� �뢮�� ��� ����� ���
					BUF_OUT_ARM[n_out++]=OUT_NOM[0]|0x80;//������� ��ꥪ�
					BUF_OUT_ARM[n_out++]=OUT_BYTE;  //������� � ���� ���ﭨ� ��ꥪ�
				}

				FR3[27]=FR3[27]|0x1f;//��⠭����� ���� ��।�� � ����
				FR3[26]=FR3[26]|(N_PAKET&0x1F);//��������� ����� ����� ��।��

				write_FR3(i);

				j=OBJ_ARMu[N_PAKET].LAST++;
				OBJ_ARMu[N_PAKET].OBJ[j]=i;//��।������ ��ꥪ�
				if(n_out>64)break;
			}
			povtor_out=i;
			if((povtor_out>=KOL_VO)||(povtor_out<=0))povtor_out=1;
			goto a1;
		}
		//�ନ஢���� ���������
		ZAGOL=0;
		ZAGOL=ZAGOL|(ZAPROS_ARM&0xf);
		ZAGOL=ZAGOL|((SERVER<<4)&0xf0);
		SOST=0x10;
		SOST=SOST|(1<<(SERVER-1));//��⠭����� �ਧ��� ࠡ���饣� �ࢥ�
		j=0;
		if(KONFIG_ARM[ZAPROS_ARM-4][0]==0xFF)j=1;//��� �᭮���� � 1-�� ࠩ���
		else
			if(KONFIG_ARM[ZAPROS_ARM-4][1]==0xFF)j=3;//��� �᭮���� �� 2-�� ࠩ���
			else
				if(KONFIG_ARM[ZAPROS_ARM-4][0]==1)j=2;//��� १�ࢭ� � 1-�� ࠩ���
				else
					if(KONFIG_ARM[ZAPROS_ARM-4][1]==2)j=4;//��� १�ࢭ� �� 2-�� ࠩ���

		if(j>0)
		{
			switch(j)
			{ //��� � ��ࢮ� ࠩ���

				case 1: SOST=SOST|0x80;  //�� �᭮����
								break;

				 //��� �� ��஬ ࠩ���
				case 3: SOST=SOST|0x80;break; //�� �᭮����

				default:SOST=SOST&0x7f;break; //�� १�ࢭ� 
			}
		}
		//�஢��塞 ����稥 2-�� ����� � �������� ࠩ���
		if(KONFIG_ARM[ZAPROS_ARM-4][0]!=0)//�᫨ ����� ��� � 1-�� ࠩ���
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][0]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		if(KONFIG_ARM[ZAPROS_ARM-4][1]!=0)//�᫨ ����� ��� �o 2-�� ࠩ���
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
		if(KNOPKA_OK[ARM]==1)SOST=SOST|0x20; //�������� ����⨥ ������
		SOST=SOST|PROCESS; //�������� ���ﭨ� ०��� �ࠢ����� ��� ��� ����� 

		BUF_OUT_ARM[2]=SOST;
		BUF_OUT_ARM[1]=ZAGOL;
		CRC=CalculateCRC16(&BUF_OUT_ARM[1],66); //�������� CRC

		PAKETs[N_PAKET].KS_OSN=CRC;//��������� ����஫��� �㬬� �����
		PAKETs[N_PAKET].ARM_OSN_KAN=0x1f;//��������� ���� ��।�� � ����

		BUF_OUT_ARM[68]=(CRC&0xFF00)>>8;
		BUF_OUT_ARM[67]=CRC&0xFF;
		BUF_OUT_ARM[0]=0xAA;
		BUF_OUT_ARM[69]=0x55;

	//��।�� ������ � ��� ��
		ZAGOL_SHN=0;
		if(ZAPROS_ARM==1)ZAGOL_SHN=ZAGOL_SHN|(ARM_SHN&0xf);
		else ZAGOL_SHN=ZAGOL_SHN|(ZAPROS_ARM&0xf);
		ZAGOL_SHN=ZAGOL_SHN|((SERVER<<4)&0xf0);

		BUF_OUT_SHN[2]=SOST&0x7f;
		BUF_OUT_SHN[1]=ZAGOL_SHN;
		for(i=3;i<67;i++)BUF_OUT_SHN[i]=BUF_OUT_ARM[i];
		CRC=CalculateCRC16(&BUF_OUT_SHN[1],66); //�������� CRC
		BUF_OUT_SHN[68]=(CRC&0xFF00)>>8;
		BUF_OUT_SHN[67]=CRC&0xFF;
		BUF_OUT_SHN[0]=0xAA;
		BUF_OUT_SHN[69]=0x55;
		outportb(ADR_SHN_OSN+1,3);//ࠧ���� ���뢠��� ��।��
//		if((news>0)||(fixir>0))add_ARM_OUT(ZAPROS_ARM,0);
		//======����� � १�ࢭ� ������� 
		ZAGOL=0;
		ZAGOL=ZAGOL|(ZAPROS_ARM1&0xf);
		ZAGOL=ZAGOL|((SERVER<<4)&0xf0);
		SOST=0x10;
		SOST=SOST|(1<<(SERVER-1));//��⠭����� �ਧ��� ࠡ���饣� �ࢥ� 
		j=0;
		if(KONFIG_ARM[ZAPROS_ARM1-4][0]==0xFF)j=1;//��� �᭮���� � 1-�� ࠩ���
		else
			if(KONFIG_ARM[ZAPROS_ARM1-4][1]==0xFF)j=3;//��� �᭮���� �� 2-�� ࠩ���
			else
				if(KONFIG_ARM[ZAPROS_ARM1-4][0]==1)j=2;//��� १�ࢭ� � 1-�� ࠩ���
				else
					if(KONFIG_ARM[ZAPROS_ARM1-4][1]==2)j=4;//��� १�ࢭ� �� 2-�� ࠩ���
		if(j>0)
		{
			switch(j)
			{ //��� � ��ࢮ� ࠩ���
				case 1: SOST=SOST|0x80;break;
				//��� �� ��஬ ࠩ���
				case 3: SOST=SOST|0x80;break;
				default: SOST=SOST&0x7F;break;
			}
		}
		//�஢��塞 ����稥 2-�� ����� � �������� ࠩ���
		if(KONFIG_ARM[ZAPROS_ARM1-4][0]!=0)//�᫨ ����� ��� � 1-�� ࠩ���
		{
			j=0;
			for(i=0;i<Narm;i++)
			{
				if(KONFIG_ARM[i][0]!=0)j++;
			}
			if(j>=2)SOST=SOST|0x8;
		}
		if(KONFIG_ARM[ZAPROS_ARM1-4][1]!=0)//�᫨ ����� ��� �o 2-�� ࠩ���
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
		//��९���� �� �᭮����� ������ �� ᢮����� ���� 
		for(i=n_arm1,j=n_arm;(i<67)&&(j<67);i++,j++)
		BUF_OUT_ARM1[i]=BUF_OUT_ARM[j];
		CRC=CalculateCRC16(&BUF_OUT_ARM1[1],66);
		PAKETs[N_PAKET].KS_REZ=CRC;//��������� ����஫��� �㬬� ����� १.���.
		PAKETs[N_PAKET].ARM_REZ_KAN=0x1f;//��������� ���� ��।�� � ����
		BUF_OUT_ARM1[68]=(CRC&0xFF00)>>8;
		BUF_OUT_ARM1[67]=CRC&0xFF;
		BUF_OUT_ARM1[0]=0xAA;
		BUF_OUT_ARM1[69]=0x55;
//    if((news>0)||(fixir>0))add_ARM_OUT(ZAPROS_ARM1,1);
		//��।�� � �ᥤ��� �ࢥ� ����� ����⮢ ��
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
			outportb(ADR_ARM_OSN+1,3);//ࠧ���� ���뢠��� ��।��

			if(REGIM==KANAL)for(i=0;i<=28;i++)putch1(BUF_OUT_ARM1[i]|0x40,atrib,41+i,14);
			outportb(ADR_ARM_REZ+1,3);//ࠧ���� ���뢠��� ��।��
		}
		else
		{
			outportb(ADR_ARM_OSN+1,1);//ࠧ���� ���뢠��� �ਥ��
			outportb(ADR_ARM_REZ+1,1);//ࠧ���� ���뢠��� �ਥ��
		}
		if(REGIM==0)
		{
			putch1(30,0xa,54+9*(SERVER-1),35);    // ����� � �᭮���� ����� ��� 
			putch1(16,9,80,45-2*(SERVER-1));      // ����� � �᭮���� ����� ��
			putch1(17,9,77,21);
			putch1(31,14,11+9*(ZAPROS_ARM-4),29); //१�ࢭ� ������ �� ����� 
			putch1(30,10,11+9*(ZAPROS_ARM1-4),33);//�᭮���� ������ �� ����� 
			putch1(30,14,56+9*(SERVER-1),27);      //��५�窨 �� १�ࢭ� ����� ��� 


		}
		outportb(ADR_SERV_PRED+1,0x3);
		outportb(ADR_SERV_NEXT+1,0x3);
		cikl_arm=0;
	}
	return;
}

/**************************************************\
* ��楤�� ����� � 䠩� ������ ����祭��� �� ��� *
*           add_ARM_IN(int arm,int kan)            *
*    arm - �����(���) ���� - ���筨�� ������      *
*    kan - �����(���) ������,�� ���஬� �� �ਥ� *
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
*   ��楤�� ����� � 䠩� ������,�뤠���� � ���  *
*           add_ARM_OUT(int arm,int kan)           *
* arm - �����(���) ���� - �ਥ����� ������         *
* kan - �����(���) ������,�� ���஬� �뫠 ��।��*
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
*     �ணࠬ�� ���������� ���⠭権 ��� ����   *
*         ZAPOLNI_KVIT(int arm,int knl)         *
*         arm - ������ ����                     *
*         knl - ������ ������                   *
\***********************************************/
int ZAPOLNI_KVIT(int arm,int knl)
{
  int i,n_ou=4;
  //�᫨  ���� ���⠭�� ��� �⮣� ������
  if((KVIT_ARMu[arm][knl][0]!=0)||(KVIT_ARMu[arm][knl][1]!=0))
  {
    if(knl==0)
    { //������ ���⠭樨 
      for(i=0;i<3;i++)BUF_OUT_ARM[n_ou++]=KVIT_ARMu[arm][0][i];
      //�᫨ �� ��ꥪ� 999 - ����� �ࠢ�����, � ������ ��।�� FR3
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
		//�᫨ ���� ���⠭�� ��� १�ࢭ��� ������
    if(knl==1)
    {
      for(i=0;i<3;i++)BUF_OUT_ARM1[n_ou++]=KVIT_ARMu[arm][1][i];
      //�᫨ ��ꥪ� 999
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
		{ //������ ���⠭樨
			for(i=0;i<3;i++)BUF_OUT_ARM[n_ou++]=KVIT_ARMu1[arm][0][i];
			for(i=0;i<3;i++)KVIT_ARMu1[arm][0][i]=0;
		}
		else
		//�᫨ ���� ���⠭�� ��� १�ࢭ��� ������
		if(knl==1)
		{
			for(i=0;i<3;i++)BUF_OUT_ARM1[n_ou++]=KVIT_ARMu1[arm][1][i];
			for(i=0;i<3;i++)KVIT_ARMu1[arm][1][i]=0;
		}
	}
	return(n_ou);
}
/*********************************************\
* ��楤�� ������� ������ ��⮢�� ������⮢ *
* ��� ����� � ���� ��������� ������ �⮩�� *
\*********************************************/
void MARSH_GLOB_LOCAL(void)
{
	int i_s,s_m,ii,kk;
	unsigned char sum;
	for(i_s=0;i_s<Nst;i_s++)     //�ன� �� �ᥬ �⮩��� �⠭樨
	{
		//�᫨ �⮩�� �����-�த������
		if(TUMS_RABOT[i_s]!=0)continue;
		//�᫨ ��� �⮩�� 㦥 ���� �����⢥ত������ ������� - �த������
		if(KOMANDA_TUMS[i_s][10]!=0)continue;
		//�ன� �� ������� ������⠬ �⮩��
		for(s_m=0;s_m<MARS_STOY;s_m++)
		{
			//�᫨ ���� ������� � �⮩�� i_s ��ப� s_m � ��� �� �� �뤠�� � ����
			if((MARSHRUT_ST[i_s][s_m].NEXT_KOM[0]!=0)&&
			(MARSHRUT_ST[i_s][s_m].T_VYD==0l))
			{
//				if(MARSH_VYDAN[i_s]==0)
				//�᫨ ���ﭨ� ������� "ࠧ��� �� �������" � �⮩�� �� �����
				if((MARSHRUT_ST[i_s][s_m].SOST&0xF)==0x7)
				{
					//��������� ������� ��� ����
					for(kk=0;kk<12;kk++)KOMANDA_TUMS[i_s][kk]=MARSHRUT_ST[i_s][s_m].NEXT_KOM[kk];
					//��⠭����� �ਧ��� �뤠� �������
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
					add(i_s,9999,0); //������� �� ���
					//��䨪�஢��� �६� �뤠� �������
					KOMANDA_TUMS[i_s][14]=s_m; //��������� ��ப� �����쭮� ⠡����
					MARSHRUT_ST[i_s][s_m].NEXT_KOM[12]=MYTHX_TEC[i_s];//��������� ���
					//��䨪�஢��� �६� �������� ��� �������
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
