
#include "opred2.h"
#include "extern2.h"

#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif

void (interrupt far *old1)(); //�࠭�⥫� ��ண� ����� PRNSCR
void (interrupt far *old2)(); //�࠭�⥫� ��ண� ����� CNTRL+BREAK
void interrupt ( *old3)(__CPPARGS); //�࠭�⥫� ��ண� ����� ��ࠡ�⪨ ����������


//----------------------------------------------
/************************************************************\
*  ��楤�� �⥭�� ����஥� �ࢥ� �� 䠩�� ���䨣����  *
*                        formula1()                          *
\************************************************************/
void formula1(void)
{
	int speed=0,speed1=0,fu=0;
	unsigned long speed2;
	void *fai;
	fai=fopen("dat\\tranc.svs","r");
	if(fai==NULL)
	{
		clrscr(); gotoxy(10,10); printf("��� 䠩�� � ��室�묨 ����묨");
		getch(); exit(0);
	}
	fu=0;
	fscanf(fai,"%x",&ADR_TUMS_OSN);//������ ���� �᭮����� ������ ����(1 IRQ5)
	while(fu!='\n')fu=fgetc(fai);
	fu=0;

	fscanf(fai,"%x",&ADR_TUMS_REZ);//������ ���� १�ࢭ��� ������ ����(2 IRQ5)
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%d",&speed);//᪮���� ������ ��� ������� ����
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&ADR_ARM_OSN);//������ ���� �᭮����� ������ ��� (1 IRQ7)
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&ADR_ARM_REZ);//������ ���� १�ࢭ��� ������ ��� (2 IRQ7)
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

	fscanf(fai,"%x",&ADR_SHN_OSN);//������ ���� �᭮����� ������ ��� ��
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%d",&speed1);//᪮���� ������ ��� ������� ���
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&ADR_SERV_PRED);//������ ���� ������ ᫥�.�ࢥ�(3 IRQ5)
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&ADR_SERV_NEXT);//������ ���� ������ �।.��� (3 IRQ7)
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%lu",&speed2);//᪮���� ������ ��� �ࢥ஢
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%c",&SERVER);//����� �ࢥ�
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&V1);//����� ���뢠��� ��� ������ ����
  while(fu!='\n')fu=fgetc(fai);
  fu=0;
  fscanf(fai,"%x",&V2);//����� ���뢠��� ��� ������ ���
  while(fu!='\n')fu=fgetc(fai);
  fu=0;

  fscanf(fai,"%x",&SOST_RANJ[0]);//����� �᭮����� ���� ��� ��ࢮ�� ࠩ���
  while(fu!='\n')fu=fgetc(fai);
  fu=0;
  fscanf(fai,"%x",&SOST_RANJ[1]);//����� �᭮����� ���� ��� ��ண� ࠩ���

  fclose(fai);

  SERVER=SERVER-48;
  switch(speed)
  {
    case 300: mld_ba=0x80; str_ba=0x01; break;
    case 600: mld_ba=0xc0; str_ba=0x00; break;
    case 1200: mld_ba=0x60;str_ba=0x00; break;
    case 2400: mld_ba=0x30;str_ba=0x00; break;
    case 4800: mld_ba=0x18;str_ba=0x00; break;
    case 9600: mld_ba=0x0c;str_ba=0x00; break;
    case 19200:mld_ba=0x06;str_ba=0x00; break;
    default:  clrscr();gotoxy(10,10);
              printf("�������⭠� ᪮���� �����");
              getch();
              exit(0);
  }
  switch(speed1)
	{
    case 300: mld_ba1=0x80; str_ba1=0x01; break;
    case 600: mld_ba1=0xc0; str_ba1=0x00; break;
    case 1200: mld_ba1=0x60;str_ba1=0x00; break;
    case 2400: mld_ba1=0x30;str_ba1=0x00; break;
    case 4800: mld_ba1=0x18;str_ba1=0x00; break;
    case 9600: mld_ba1=0x0c;str_ba1=0x00;  break;
    case 19200: mld_ba1=0x06; str_ba1=0x00; break;
    case 38400l: mld_ba1=0x03; str_ba1=0x00; break;
		case 57600l: mld_ba1=0x02; str_ba1=0x00; break;
    default:  clrscr();gotoxy(10,10);
              printf("�������⭠� ᪮���� ����");
              getch();
              exit(0);
  }

  switch(speed2)
  {
    case 300: mld_ba2=0x80; str_ba2=0x01; break;
    case 600: mld_ba2=0xc0; str_ba2=0x00; break;
    case 1200: mld_ba2=0x60;str_ba2=0x00; break;
    case 2400: mld_ba2=0x30;str_ba2=0x00; break;
    case 4800: mld_ba2=0x18;str_ba2=0x00; break;
    case 9600: mld_ba2=0x0c;str_ba2=0x00;  break;
    case 19200: mld_ba2=0x06; str_ba2=0x00; break;
		case 38400l: mld_ba2=0x03; str_ba2=0x00; break;
    case 57600l: mld_ba2=0x02; str_ba2=0x00; break;
    case 115200l: mld_ba2=0x01; str_ba2=0x00; break;
    default:  clrscr();gotoxy(10,10);
              printf("�������⭠� ᪮���� �������");
							getch();
              exit(0);
	}
  return;
}
/***********************************************\
*  ��楤�� ���樠����樨 ��६�����, ������ *
*  䠩��� ���� ������ �⠭樨 � ��⨢���樨     *
*  ���� �����-�뢮��                            *
\***********************************************/
void iniciator(void)
{
	int ii,jj,kk;
	char name_file[20];
  unsigned char BD_[34],INP_OB[4],OUT_OB[4],SP_STR[12],SP_SP[17];

	for(ii=0;ii<98;ii++)
	{
		BUF_IN_PRED[ii]=0;
		BUF_IN_NEXT[ii]=0;
		BUF_OUT_PRED[ii]=0;
		BUF_OUT_NEXT[ii]=0;

	}
	for(ii=0;ii<32;ii++)OBJ_ARMu[ii].LAST=0;
	for(ii=0;ii<32;ii++)OBJ_ARMu1[ii].LAST=0;
	for(ii=0;ii<Narm;ii++)
	{
		KNOPKA_OK[ii]=0;
		for(jj=0;jj<Nranj;jj++)KONFIG_ARM[ii][jj]=0;
	}

	for(kk=0;kk<Nst;kk++)
	{
		MYTHX[kk]=0;
		MYTHX_TEC[kk]=0;
		for(ii=0;ii<48;ii++)
		for(jj=0;jj<7;jj++)VVOD[kk][ii][jj]=0;
		SHET_KOM[kk]=0;
		tiki_tum[kk]=0;
		TUMS_RABOT[kk]=0;
	}

	formula1();
	strcpy(name_file,"dat\\pako.bin");
	pako_fil=open(name_file,O_BINARY);
	if(pako_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		KOL_VO=(int)(1+filelength(pako_fil)/22);
		for(ii=1;ii<KOL_VO;ii++)
		{
			read(pako_fil,PAKO[ii],22);
			for(jj=0;jj<22;jj++)if(PAKO[ii][jj]<=0x20)PAKO[ii][jj]=0;
		}
	}
	close(pako_fil);

	strcpy(name_file,"dat\\bd_osn.bin");
	bd_osn_fil=open(name_file,O_BINARY);
	if(bd_osn_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=1;ii<=KOL_VO;ii++)
		{
			read(bd_osn_fil,BD_,34);
			for(jj=0;jj<16;jj++)BD_OSN[ii][jj]=BD_[2*jj]*256+BD_[2*jj+1];
		}
	}
	close(bd_osn_fil);

	strcpy(name_file,"dat\\inp.bin");
	inp_fil=open(name_file,O_BINARY);
	if(inp_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=1;ii<KOL_VO;ii++)
		{
			read(inp_fil,INP_OB,4);
			inp_ob[ii]=INP_OB[0]*256+INP_OB[1];
		}

	}
	close(inp_fil);

	strcpy(name_file,"dat\\out.bin");
	out_fil=open(name_file,O_BINARY);
	if(out_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=1;ii<KOL_VO;ii++)
		{
			if(read(out_fil,OUT_OB,4)<4)break;
			if((OUT_OB[0]==0x20)&&(OUT_OB[1]==0x20))out_ob[ii]=0;
			else out_ob[ii]=OUT_OB[0]*256+OUT_OB[1];
		}

	}
	close(out_fil);

	strcpy(name_file,"dat\\spstr.bin");
	spstr_fil=open(name_file,O_BINARY);
	if(spstr_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spstr_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPSTR[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spstr_fil);

	strcpy(name_file,"dat\\spsig.bin");
	spsig_fil=open(name_file,O_BINARY);
	if(spsig_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spsig_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPSIG[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spsig_fil);

	strcpy(name_file,"dat\\spdop.bin");
	spdop_fil=open(name_file,O_BINARY);
	if(spdop_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spdop_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPDOP[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spdop_fil);

	strcpy(name_file,"dat\\spdop_b.bin");
	spdop_fil=open(name_file,O_BINARY);
	if(spdop_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spdop_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPDOP_B[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spdop_fil);

	strcpy(name_file,"dat\\spdop_d.bin");
	spdop_fil=open(name_file,O_BINARY);
	if(spdop_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spdop_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPDOP_D[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spdop_fil);

	strcpy(name_file,"dat\\spdop_t.bin");
	spdop_fil=open(name_file,O_BINARY);
	if(spdop_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spdop_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPDOP_T[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spdop_fil);


	strcpy(name_file,"dat\\spkon.bin");
	spkon_fil=open(name_file,O_BINARY);
	if(spkon_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spdop_fil,SP_STR,12)!=12)break;
			for(jj=0;jj<5;jj++)SPKON[ii][jj]=SP_STR[2*jj]*256+SP_STR[2*jj+1];
		}
	}
	close(spkon_fil);

	strcpy(name_file,"dat\\spspu.bin");
	spspu_fil=open(name_file,O_BINARY);
	if(spspu_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spspu_fil,SP_SP,17)!=17)break;
			for(jj=0;jj<5;jj++)SPSP[ii][jj]=SP_SP[2*jj]*256+SP_SP[2*jj+1];
			for(jj=5;jj<10;jj++)
			{
				SPSP[ii][jj]=SP_SP[jj+5];
			}
		}

	}
	close(spspu_fil);

	strcpy(name_file,"DAT\\spput.bin");
	spput_fil=open(name_file,O_BINARY);
	if(spput_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=0;ii<KOL_VO;ii++)
		{
			if(read(spput_fil,SP_SP,17)!=17)break;
			for(jj=0;jj<5;jj++)SPPUT[ii][jj]=SP_SP[2*jj]*256+SP_SP[2*jj+1];
			for(jj=5;jj<10;jj++)
			{
				SPPUT[ii][jj]=SP_SP[jj+5];
			}
		}
	}
	close(spput_fil);

	strcpy(name_file,"dat\\fr3.bin");
	fr3_fil=open(name_file,O_RDWR|O_BINARY);
	if(fr3_fil==-1)
	{
		clrscr();
		gotoxy(10,10);
		printf("�� ����� 䠩� ");printf(name_file);
		getch(); exit(0);
	}
	else
	{
		for(ii=1;ii<KOL_VO;ii++)
		{
			if(read(fr3_fil,FR3_ALL[ii],34)!=34)break;
			for(jj=0;jj<32;jj++)if(FR3_ALL[ii][jj]==32)FR3_ALL[ii][jj]=0;
#ifdef TEST2
			FR3_ALL[ii][11]=0;
#endif
			FR3_ALL[ii][32]=0;FR3_ALL[ii][33]=0;
		}
	}
	close(fr3_fil);

	for(jj=0;jj<Nst;jj++)
	{
		for(ii=0;ii<15;ii++)

		{
			KOMANDA_ST[jj][ii]=0;
			KOMANDA_TUMS[Nst][ii]=0;
		}
		KOL_VYD_MARSH[jj]=0;	//$$$$ 13_04_07 ��� ���稪�� ����஢ �뤠� ������⮢ � �⮩�� jj
	}
	for(ii=0;ii<10;ii++)NOVIZNA_FR4[ii]=0;
	main_win();
#ifdef WORK
	init1(); //���樠������ ���� �����-�뢮��
#endif
	cikl=0;ZPRS_TMS1=0;ZPRS_TMS=0;ZAPROS_TUMS=0; ZAPROS_TUMS1=3;
	_setcursortype(_NOCURSOR);
	for(ii=0;ii<KOL_VO;ii++)PEREDACHA[ii]=0;
	for(ii=0;ii<KOL_VO;ii++)PRIEM[ii]=0;
	for(ii=0;ii<KOL_VO;ii++)FR4[ii]=0;
	for(ii=0;ii<KOL_VO;ii++)ZAFIX_FR4[ii]=0;
	T0=0;
	T_MIN_NEXT=0;
	T_MIN_PRED=0;
	POLUCHIL_UPR_OT_NEXT=0;
	POLUCHIL_UPR_OT_PRED=0;
	PEREDAL_UPR_K_NEXT=0;
	PEREDAL_UPR_K_PRED=0;
	return;
}
/**********************************************\
*  ��楤�� ��⨢���樨 ����஢ ���뢠��� � *
*  ���� �����-�뢮�� �ࢥ� init1()           *
\**********************************************/
void init1(void)
{
  disable();
  old1=getvect(0x5);
	old2=getvect(0x1b);
	old3=getvect(0x9);
	setvect(0x5,PRNTSCR);
	setvect(0x1b,CNTRLBREAK);
	setvect(0x9,KEYBRD);

	if(V1>0x70) {s01=inportb(0xa1);outportb(0xa1,s01|0x80);}//����� IRQ15
	else {s02=inportb(0x21);outportb(0x21,s02|0x20);}//����� IRQ5
	if(V2>0x70) {s02=inportb(0xa1);outportb(0xa1,s02|0x80);}//����� IRQ15
	else {s02=inportb(0x21); outportb(0x21,s02|0x80);}//����� IRQ7
	s_vect1=getvect(V1);
	s_vect2=getvect(V2);
#ifdef WORK
	s_timer=getvect(VT);
#endif
	setvect(V1,reading_char1);//��⠭����� ��ࠡ��稪 ��� ����� 1
	setvect(V2,reading_char2);//��⠭����� ��ࠡ��稪 ��� ����� 2
#ifdef WORK
	setvect(VT,TIMER_TIC);
#endif
	//��⠭���� ᪮��⥩ ������ ��� ������� ����
	inportb(ADR_TUMS_OSN+5);//��� �訡��
	outportb(ADR_TUMS_OSN+3,0x00);
	outportb(ADR_TUMS_OSN+1,0x00);
	outportb(ADR_TUMS_OSN+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
	outportb(ADR_TUMS_OSN+1,str_ba);// ���訩 ���� ����⥫� �����
	outportb(ADR_TUMS_OSN,mld_ba);  // ����訩 ���� ����⥫� ����� ��� ᪮��� 2400
	outportb(ADR_TUMS_OSN+3,0x0e); // N ���
	outportb(ADR_TUMS_OSN+1,1); // ����� ���뢠��� ������� ����

	inportb(ADR_TUMS_REZ+5);//��� �訡��
	outportb(ADR_TUMS_REZ+3,0x00);
	outportb(ADR_TUMS_REZ+1,0x00);
	outportb(ADR_TUMS_REZ+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
	outportb(ADR_TUMS_REZ+1,str_ba);// ���訩 ���� ����⥫� �����
	outportb(ADR_TUMS_REZ,mld_ba);  // ����訩 ���� ����⥫� ����� ��� ᪮��� 2400
	outportb(ADR_TUMS_REZ+3,0x0e); // N ���
	outportb(ADR_TUMS_REZ+1,1); // ����� ���뢠��� ������� ����

	//��⠭���� ᪮��⥩ ������ ��� ������� ���
	inportb(ADR_ARM_OSN+5);//��� �訡��
	outportb(ADR_ARM_OSN+3,0x00);
	outportb(ADR_ARM_OSN+1,0x00);//������� ���뢠���
	outportb(ADR_ARM_OSN+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
	outportb(ADR_ARM_OSN+1,str_ba1);// ���訩 ���� ����⥫� �����
	outportb(ADR_ARM_OSN,mld_ba1);  // ����訩 ���� ����⥫� ����� ��� ᪮��� 2400
	outportb(ADR_ARM_OSN+3,0x0f); // 8 ���+2�⮯�+��_�����
	outportb(ADR_ARM_OSN+2,1);//ࠧ���� ���뢠��� FIFO
	outportb(ADR_ARM_OSN+2,0xC7);//ࠧ���� ���뢠��� FIFO �� 14 ᨬ�����
	outportb(ADR_ARM_OSN+1,1);//ࠧ�襭�� ���뢠��� ������
	inportb(ADR_ARM_OSN);

	inportb(ADR_ARM_REZ+5);//��� �訡��
	outportb(ADR_ARM_REZ+3,0x00);
	outportb(ADR_ARM_REZ+1,0x00);
	outportb(ADR_ARM_REZ+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
	outportb(ADR_ARM_REZ+1,str_ba1);// ���訩 ���� ����⥫� �����
	outportb(ADR_ARM_REZ,mld_ba1);  // ����訩 ���� ����⥫� ����� ��� ᪮���
	outportb(ADR_ARM_REZ+3,0x0f); // 8 ���+2�⮯�+��_�����
	outportb(ADR_ARM_REZ+2,1);//ࠧ���� ���뢠��� FIFO
	outportb(ADR_ARM_REZ+2,0xC7);//ࠧ���� ���뢠��� FIFO �� 14 ᨬ�����
	outportb(ADR_ARM_REZ+1,1); // ࠧ�襭�� ���뢠��� ������
	inportb(ADR_ARM_REZ);

	inportb(ADR_SHN_OSN+5);//��� �訡��
	outportb(ADR_SHN_OSN+3,0x00);
	outportb(ADR_SHN_OSN+1,0x00);
	outportb(ADR_SHN_OSN+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
	outportb(ADR_SHN_OSN+1,str_ba1);// ���訩 ���� ����⥫� �����
	outportb(ADR_SHN_OSN,mld_ba1);  // ����訩 ���� ����⥫� ����� ��� ᪮���
	outportb(ADR_SHN_OSN+3,0x0f); // 8 ���+2�⮯�+��_�����
	outportb(ADR_SHN_OSN+2,1);//ࠧ���� ���뢠��� FIFO
	outportb(ADR_SHN_OSN+2,0xC7);//ࠧ���� ���뢠��� FIFO �� 14 ᨬ�����
	outportb(ADR_SHN_OSN+1,1); // ࠧ�襭�� ���뢠��� ������
	inportb(ADR_SHN_OSN);

	inportb(ADR_SERV_PRED+5);//��� �訡��
	outportb(ADR_SERV_PRED+3,0x00);
	outportb(ADR_SERV_PRED+1,0x00);//������� ���뢠���
	outportb(ADR_SERV_PRED+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
  outportb(ADR_SERV_PRED+1,str_ba2);// ���訩 ���� ����⥫� �����
  outportb(ADR_SERV_PRED,mld_ba2);  // ����訩 ���� ����⥫� ����� ��� ᪮��� 2400
  outportb(ADR_SERV_PRED+3,0x0f); // 8 ���+2�⮯�+��_�����
  outportb(ADR_SERV_PRED+2,1);//ࠧ���� ���뢠��� FIFO
  outportb(ADR_SERV_PRED+2,0xC7);//ࠧ���� ���뢠��� FIFO �� 14 ᨬ�����
  outportb(ADR_SERV_PRED+1,1);//ࠧ�襭�� ���뢠��� ������ �।.�ࢥ�
  inportb(ADR_SERV_PRED);

  inportb(ADR_SERV_NEXT+5);//��� �訡��
  outportb(ADR_SERV_NEXT+3,0x00);
  outportb(ADR_SERV_NEXT+1,0x00);
  outportb(ADR_SERV_NEXT+3,0x80); // ॣ���� �ࠢ����� ������ - 80� ��� ��⠭���� ᪮���
  outportb(ADR_SERV_NEXT+1,str_ba2);// ���訩 ���� ����⥫� �����
  outportb(ADR_SERV_NEXT,mld_ba2);  // ����訩 ���� ����⥫� ����� ��� ᪮��� 2400
  outportb(ADR_SERV_NEXT+3,0x0f); // 8 ���+2�⮯�+��_�����
  outportb(ADR_SERV_NEXT+2,1);//ࠧ���� ���뢠��� FIFO
  outportb(ADR_SERV_NEXT+2,0xC7);//ࠧ���� ���뢠��� FIFO �� 14 ᨬ�����
  outportb(ADR_SERV_NEXT+1,1); // ࠧ�襭�� ���뢠��� ������ ᫥�.�ࢥ�
  inportb(ADR_SERV_NEXT);
  if(V1>0x70)
  {
    s01=inportb(0xa1);
    outportb(0xa1,s01&(~0x80));//ࠧ���� IRQ15
  }
  else
  {
    s01=inportb(0x21);
    outportb(0x21,s01&(~0x20));//ࠧ���� IRQ5
  }
  if(V2>0x70)
  {
    s02=inportb(0xa1);
    outportb(0xa1,s02&(~0x80));//ࠧ���� IRQ15
  }
	else
  {
    s02=inportb(0x21);
    outportb(0x21,s02&(~0x80));//ࠧ���� IRQ7
  }
	enable();
  return;
}
/********************************************\
*  ��楤�� ������ ����஢ ���뢠��� �� *
*  ���� ��।������ ����㧪�� ����         *
*            reset_int_vect1()               *
\********************************************/
void reset_int_vect1(void)
{
	disable();
	outportb(0x21,s01|0x20);//����� IRQ5
	if((V1>0x70)||(V2>0x70))
	{
		s02=inportb(0xa1);
		outportb(0xa1,s02|0x80);//����� IRQ15
	}
	else
	{
		s02=inportb(0x21);
		outportb(0x21,s02|0x80);//����� IRQ7
	}
	setvect(V1,s_vect1);
	setvect(V2,s_vect2);
	setvect(VT,s_timer);
	setvect(0x5,old1);
	setvect(0x1b,old2);
	setvect(0x9,old3);
	enable();
	return;
}
/**************************************\
*  ��楤�� ����� ��ࠡ�⪨ ������   *
*  ������ <PrtScr>                    *
\**************************************/
void interrupt far PRNTSCR()
{
	outportb(0x20,0x20);
}
/**************************************\
* ��楤�� ����� ��ࠡ�⪨ ������    *
* ������ <Ctrl>+<Break>                *
\**************************************/
void interrupt far CNTRLBREAK()
{
	outportb(0x20,0x20);
}
/*************************************\
* ��楤�� ����� ��ࠡ�⪨ ������   *
* ��� ������ � ��⠭�� � <Ctrl>    *
\*************************************/
void interrupt KEYBRD(__CPPARGS)
{
	unsigned char value;
	value = peek(0x0040, 0x0017);
	if((value&0xbd)==0)goto LOO; //�᫨ �� ����� CTRL
	poke(0x0040,0x0017,0);
	value = peek(0x0040, 0x0018);
	if(value==0)goto LOO; //�᫨ �� ����� CTRL
	poke(0x0040,0x0018,0);
LOO:
	old3();
	return;
}


