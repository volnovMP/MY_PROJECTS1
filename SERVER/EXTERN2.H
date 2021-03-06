#include "opred2.h"
#include <dos.h>
#include <setjmp.h>
#include<stdio.h>
#include<FCNTL.H>
#include<stdlib.h>
#include<alloc.h>
#include<bios.h>
#include<process.h>
#include<sys\stat.h>
#include<conio.h>
#include<ctype.h>
#include<io.h>
#include <string.h>
#include <dir.h>
#include <TIME.H>
extern void (interrupt far *s_vect1)(); //�࠭�⥫� ��ண� ����� IRQ5
extern void (interrupt far *s_vect2)(); //�࠭�⥫� ��ண� ����� IRQ7
extern void (interrupt far *s_timer)(); //�࠭�⥫� ��ண� ����� ⠩���
extern void interrupt far reading_char1();//��ࠡ��稪 ���뢠��� ������ࠫ�� ����
extern void interrupt far reading_char2();//��ࠡ��稪 ���뢠��� ������ࠫ�� ���
#ifdef WORK
extern void interrupt far TIMER_TIC();//��ࠡ��稪 ���뢠��� ���������� �������
#else
void TIMER_TIC();//������ ��ࠡ��稪� ���뢠��� ���������� �������
#endif
extern void interrupt far PRNTSCR();//��ࠡ��稪 ���뢠��� PRNSCR
extern void interrupt far CNTRLBREAK();//��ࠡ��稪 ���뢠��� CNTRLBREAK
extern void interrupt far KEYBRD();//��ࠡ��稪 ���뢠��� ����������
void consentr1(void);
void READ_BD(int obj);
void ZAGRUZ_KOM_TUMS(char tms,char grp,char pdgrp,char bt,char kd_cmd);
void prosto_komanda(unsigned char command);
void putch1(char simb,unsigned char cvt,int X, int Y);
void puts1(char *simb,unsigned char cvt,int X, int Y);
void MAKE_KOMANDA(int ARM,int STAT,int ray);
void MAKE_MARSH(int ARM,int STAT);
void otmena_rm(unsigned int objserv);
void perevod_strlk(unsigned char command,unsigned int objserv);
void signaly(unsigned char command,unsigned int objserv);
void sp_up_and_razd(unsigned char command,unsigned int objserv,int arm);
void puti(unsigned char command,unsigned int objserv);
void dopoln_obj(unsigned char command,int arm);
void ob_tums(unsigned char command);
int ANALIZ_MARSH(int KOM,int NACH,int END,int Nstrel,unsigned long POL);
void TUMS_MARSH(int i_m);
void set_vvod(void);
int tst_str_ohr(void);
void re_set(void);
void vidacha1(unsigned int ad);
void out_next(void);
void out_pred(void);
void out_arm_osn(void);
void out_arm_rez(void);
void out_shn_osn(void);
void in_next(void);
void in_pred(void);
void in_arm_osn(void);
void in_arm_rez(void);
void in_shn_osn(void);
void in_tums_osn(void);
void in_tums_rez(void);
void OUT_PAKET_PRED_NEXT(void);
void ARM_OUT(void);
void add_ARM_IN(int arm,int kan);
void add_ARM_OUT(int arm,int kan);
int ZAPOLNI_KVIT(int arm,int knl);
void formula1(void);
void iniciator(void);
void init1(void);
void reset_int_vect1(void);
void main(void);
int TAKE_STROKA(unsigned char GRPP,int sb,int tms);
void ZAPOLNI_FR3(unsigned char GRP,int STRKA,int sob,int tum,unsigned char nov);
int check_summ(unsigned char reg[15]);
int mikrotiki(void);
void new_zapros(void);
void sbros_tums(int a);
void PRIEM_SERV(void);
unsigned char CalculateCRC8(void *pData, int dataLen);
void VYVOD_ON_SCREEN(void);
void ANALIZ_SHN(void);
void ANALIZ_ARM(void);
void RASPAK_ARM(int bb,unsigned char STAT,int arm);
void ANALIZ_KVIT_ARM(int arm,int stat);
int ANALIZ_KVIT_SERV(int sosed);
void ANALIZ_ACTIV_PASSIV(void);
void VYVOD_TUMS(void);
void sbros_kom(void);
void ACTIVNOST(void);
int test_time1(long const_);
void FINAL(void);
void read_t(void);
//void TEST_MARSH(void);
void watchdog(void);
void stop_watchdog(void);
void OSN_TUMS_IN(void);
void REZ_TUMS_IN(void);
void add(int st,int sob,int knl);
int diagnoze(int st,int kan);
void main_win(void);
void win_object(void);
void win_analiz(void);
void win_konfig(void);
void win_comm(void);
void GRAND_TEXT(int X,int Y,char *TXT);
void tablica(void);
void win_gash(void);
int CalculateCRC16(void *pData, int dataLen);
int test_plat(int st,int kan);
unsigned char CalculateCRC8(void *pData, int dataLen);
void MAKE_TIME(int ARM,int STAT);
void read_FR3(int nomer);
void write_FR3(int nomer);
void Soob_For_Arm(int nom_mar,int sos,int kod_er);
void ZERO_TRASSA(void);
void ZERO_TRASSA1(void);
void DeleteMarsh(int i_m);
void PovtorMarsh(int i_m);
void Analiz_Glob_Marsh(void);
void win_marsh(void);
void MARSH_GLOB_LOCAL(void);
int ANALIZ_ST_IN_PUT(int nom_tras,int kom,int st,int marsh,int ind);
void obnovi(int obj1);
extern void (interrupt far *s_vect1)(); //�࠭�⥫� ��ண� ����� IRQ5
extern void (interrupt far *s_vect2)(); //�࠭�⥫� ��ண� ����� IRQ7
extern void (interrupt far *s_timer)(); //�࠭�⥫� ��ண� ����� ⠩��� 

extern time_t T_TIME;
extern unsigned int t1,t2,T0,MOLCHI,T_MIN_NEXT,T_MIN_PRED,tster,
T_fix,fixir,T_zap,LAST_FR4;

extern char
STOP_AN,
PAKO[2000][22],
TIME[11],
SET_TIME,
delta[10],
sost[7],
VIRT_DISK[3], //��� ����㠫쭮�� ��᪠ ��� �࠭���� ��� 
NAME_FILE[14],
KVIT_TUMS[Nst][10],//ॣ����� ���⠭権 ��� ��।�� � �᭮���� ����� ����
KVIT_TUMS1[Nst][10], //ॣ����� ���⠭権 ��� ��।�� � १�ࢭ� ����� ����
KOMANDA_TUMS[Nst][15];

extern unsigned int crc16_table[256];
extern unsigned char crc8_table[256],KOM_BUFER[28];;

extern unsigned int
SVAZ_TUMS[Nst],
VVOD_OBJ,
OBJECT_ARM,
OBJECT_SERV,
NOMER_OB_ARM[4],
X_VVOD1,
X_ANALIZ_OUT,
X1_ANALIZ_OUT,
Y_ANALIZ_OUT,
Y1_ANALIZ_OUT,
ADR_TUMS_OSN,     //������ ���� �᭮����� ������ ������ ����묨 � ������� 
ADR_TUMS_REZ,     //������ ���� १�ࢭ��� ������ ������ ����묨 � ������� 
ADR_ARM_OSN,      //������ ���� �᭮����� ������ ������ ����묨 � ������
ADR_ARM_REZ,      //������ ���� १�ࢭ��� ������ ������ ����묨 � ������
ADR_SHN_OSN,
ADR_SERV_PRED,    //������ ���� ��� ������ � �।��騬 �ࢥ஬
ADR_SERV_NEXT;    //������ ���� ��� ������ � ��᫥���騬 �ࢥ஬
extern unsigned long
POOO[1900],
SEC_time,
FIR_time,
second_time,
first_time,
long_test;

extern unsigned int error1,error2,
CIKL_MAIN,	//$$$$ - 14_04_07 $$$$ - ��६����� ��।������ �⭮�� � ���⭮�� �᭮����� 横��
KOL_VYD_MARSH[Nst],//$$$$ ���稪 �᫠ ࠧ �뤠� ������ ������� (ࠧ�襭 ⮫쪮 ���� ����� �뤠�)
Schet_Com[Nst],
Schet_Takt[Nst],
KOL_VO,  //��饥 �᫮ ��ꥪ⮢ �� �⠭樨
chas_,
min_,
sec_,
PAROL,//$$$$ - 14_04_07 - ��������� ��� ��室� �� ���� ࠡ����� �ࢥ��
povtor_out,
POVTOR_OTKR,
FLAG_KOM,
PAKET[32][2],//����஫�� �㬬� CRC-16 ��� ����஢ ��।����� ����⮢
MYTHX[Nst],  //�ਧ���� ���ﭨ� ������⮢ � �⮩���
TUMS_RABOT[Nst], //�ਧ��� ������� �⮩��
NOVIZNA[MAX_NEW], //���ᨢ ��᫥���� �ਭ�⮩ �������
NOVIZNA_FR4[10], //���ᨢ ��᫥���� ������� ��� FR4
BD_OSN[1900][17],  //��ப� ���ᠭ�� ��ꥪ�
inp_ob[1900],
out_ob[1900],
bd_osn[16],
nom_new, //㪠��⥫� ����� ������� ���ᨢ� ������� ��� ᫥���饩 �����
new_fr4, //㪠��⥫� ����� ������� ���ᨢ� ������� ��� ᫥���饩 �����
nom_per,
FR4_NEXT, //㪠��⥫� �� ��ꥪ� ��� ᫥���饣� �ࢥ�
FR4_PRED, //㪠��⥫� �� ��ꥪ� ��� �।��饣� �ࢥ�
mld_ba,str_ba, //����⥫� ��� ������� ������ ����
mld_ba1,str_ba1,            //����⥫� ��� ������� ������ ���
mld_ba2,str_ba2,            //����⥫� ��� ������� �ࢥ�
V1,//����� ��� ��ࠡ�⪨ irq5
V2,//����� ��� ��ࠡ�⪨ irq7
VT,//����� ��� ��ࠡ�⪨ ⠩��୮�� ���뢠���
ST,  //����� ��⨢��� �⮩�� �᭮����� ������
ST1, //����� ��⨢��� �⮩�� १�ࢭ��� ������
s01,s02,
stak,
ttt,
ttt1,
hndl,
REGIM,  //०�� �뢮�� �� ��࠭ 
cikl,//���稪 横��� ⠩���
cikl_marsh, //���稪 横��� ��������� ������⮢
cikl_arm,
cikl_out_arm, //��� 14-� ������ 横��� �뢮�� ������ � ��� �� ��.������
cikl_out_arm1,//��� 14-� ������ 横��� �뢮�� ������ � ��� �� १.������
cikl_out_shn,
cikl_in_arm, //��� 14-� ������ 横��� �ਥ�� ������ �� ��� �� ��.������
cikl_in_arm1, //��� 14-� ������ 横��� �ਥ�� ������ �� ��� �� १.������
cikl_in_shn,
cikl_out_pred, //� �� ��� �뢮�� � �।��騩 �ࢥ�
cikl_out_next, //� �� ��� �뢮�� � ᫥���騩 �ࢥ�
cikl_in_pred, //� �� ��� �ਥ�� �� �।��饣� �ࢥ� 
cikl_in_next, //� �� ��� �ਥ�� �� ᫥���饣� �ࢥ� 
n_tums,
n_tums1,
Y_KOM,
X_out[Nst],         //X �뢮�� ��� �᭮����� ������ �⮩��
Y_out[Nst],    //Y �뢮�� ��� �᭮����� ������ �⮩��
X_out_rez[Nst],     //X �뢮�� ��� १�ࢭ��� ������ �⮩��
Y_out_rez[Nst],//Y �뢮�� ��� १�ࢭ��� ������ �⮩��
X_in[Nst],           //X ����� ��� �᭮����� ������ �⮩��
Y_in[Nst],     //Y ����� ��� �᭮����� ������ �⮩��
X_in_rez[Nst],           //X ����� ��� �᭮����� ������ �⮩��
Y_in_rez[Nst];     //Y ����� ��� �᭮����� ������ �⮩��

extern unsigned char
MYTHX_TEC[Nst],
yearr_,
monn_,
dayy_,
old_day,
new_day,
DIAGNOZ[3],
ERR_PLAT[6],
KORZINA[Nst],
MARSH_VYDAN[Nst], //�ਧ���� �뤠� ������⮢ � �⮩��
OK_KNOPKA,      //�������� ������� ������ ��
SHET_KOM[Nst],  //���稪 ����஢ �뤠� ࠧ���� ������ (�᫨ �� �뫮 ���⠭樨)
SHET_MARSH[Nst],//���稪 ����஢ �뤠� ������⮢ (�᫨ �� �뫮 ���⠭樨)
tiki_tum[Nst],
PAUZA,
PAROL_TXT[4], //$$$$ - 14_04_07 - ��������� ��� ��室� �� ���� ࠡ����� �ࢥ��
SERVER,       //����� ⥪�饣� �ࢥ� 
SERVER_PRED,//����� �।��饣� �ࢥ� 
SERVER_NEXT,  //����� ᫥���饣� �ࢥ� 
KOD_SERVER,  //��� ���ﭨ� ��⥬� �ࢥ��+����
STOP_SERVER, //䫠� �ਮ�⠭���� ࠡ��� �ࢥ� 
ACTIV_SERV,
POLUCHIL_UPR_OT_NEXT,//䫠��  ����祭�� �ࠢ�����
POLUCHIL_UPR_OT_PRED,

PEREDAL_UPR_K_NEXT,  //䫠� ��।�� �ࠢ����� ᫥���饬�
PEREDAL_UPR_K_PRED,   //䫠� ��।�� �ࠢ����� ᫥���饬�
REG_ARM[100], //ॣ���� ���ଠ樨, �ਭ�⮩ �� ���� �� �᭮����� ������
REG_ARM1[100], //ॣ���� ���ଠ樨, �ਭ�⮩ �� ���� �� १�ࢭ��� ������
ZAGRUZKA,
VVOD[Nst][48][7],
SOST_SERV[3], //����� ���ﭨ� �ࢥ஢
SOST_RANJ[Nranj], //����� ���ﭨ� ࠩ���� 
podtv_NEXT, //�ਧ��� ������ ���⢥ত���� �� �।��饣� �ࢥ� 
podtv_PRED, //�ਧ��� ������ ���⢥ত���� �� ᫥���饣� �ࢥ� 

N_PAKET, //����� ⥪�饣� ����� ��।�� � ��� 
str_bd[32], //��ப� ���ᠭ�� ��ꥪ� 
FR3_ALL[1900][34],  //��ப� �⥭��/����� ���ᨢ� FR3
FR3[34],
ACTIV,
symbol,
symbol1,
vvod_set,
OSNOVA,      //�����䨪��� ����� �ࢥ� 
STATUS[Narm],//�����䨪��� ����� ��� (�����+ ��������/���������)
KOD_STR[2],//��� ��ॢ��� ��५�� � +,-
KOD_SIGO[2],//��� ������ ᨣ���� �����஢�,��������
KOD_SIGZ[2],//��� ������� ᨣ���� �����஢�,��������
KOD_VKL[2],//���� ����祭�� ��ꥪ⮢ ��� ��⮢ 0,1
KOD_OTKL[2],//���� �⪫�祭�� ��ꥪ⮢ ��� ��⮢ 0,1
KOD_RAZD,  //��� ������� �����⢥���� ࠧ�����
KOD_KOMPL, //��� ������� ��४��祭�� �������� 
KOD_GRI,   //��� ������� ����祭�� �뤥প� ��� 
KONFIG_ARM[Narm][Nranj],//���䨣���� ����� 
KNOPKA_OK[Narm], //���ᨢ ���ﭨ� ������ �⢥��⢥���� ������
ZAPROS_ARM,   //ᤢ������ ॣ���� 䫠��� �᭮���� ����ᮢ � ����
ZAPROS_ARM1,  //ᤢ������ ॣ���� 䫠��� १�ࢭ�� ����ᮢ � ����
summa,
atrib,  //��ਡ�� ⥪�⮢��� ��࠭�
dlt[10],
//=============������� � ��������� ��� ������ � ������������ ����
//=================================================================
//--------------------------�������� ����� 
//-----�᭮���� �����
ADR_TUMS_IN,   //���᭠� ���� ���뫪�,����祭��� �� �᭮����� ������ ����
REG_IN[Nst][12], //����a �ਥ�� �� �᭮����� ������
BUF_IN[SIZE_BUF], //����a �ਥ�� �� �᭮����� ������
UKAZ_IN, //㪠��⥫� �� ���� ����� � ���� �ਥ�� �᭮����� ������
BEGIN_TUMS,  //�ਧ��� ����祭�� ��砫� ���뫪� �� �᭮����� ������ ����
END_TUMS,    //�ਧ��� ����祭�� ���� ���뫪� �� १�ࢭ��� ������ ����
//-----१�ࢭ� ����� 
ADR_TUMS_IN1,//���᭠� ���� ���뫪�,����祭��� �� १�ࢭ��� ������ ����
REG1_IN[Nst][12], //����a �ਥ�� �� १�ࢭ��� ������
BUF_IN1[SIZE_BUF],//����a �ਥ�� �� १�ࢭ��� ������
UKAZ_IN1,//㪠��⥫� �� ���� ����� � ���� �ਥ�� १�ࢭ��� ������
BEGIN_TUMS1, //�ਧ��� ����祭�� ��砫� ���뫪� �� १�ࢭ��� ������ ����
END_TUMS1,   //�ਧ��� ����祭�� ���� ���뫪� �� १�ࢭ��� ������ ����
//=======================================================================
//------------------------���������� ����� 
//-----�᭮���� ����� 
ADR_TUMS_OUT, //���᭠� ���� �᭮����� �����(���⠭樨)������
BUF_OUT[SIZE_BUF], //���� �뤠� � �᭮���� ������ࠫ�
UKAZ_ZAP,//㪠��⥫� �� ���� ����� � �᭮���� ���� �뢮��
UKAZ_OUT,//㪠��⥫� �� ���� �뤠� � �᭮���� ���� �뢮��
ZAPROS[6], //�࠭�⥫� �᭮���� ����ᮢ � �⮩�� ����
ZPRS_TMS, //�ਧ��� �뤠� ����� �� �᭮����� ������ � �⮩��
ZAPROS_TUMS,//ॣ���� ����� �᭮���� ����ᮢ � �⮩�� ����
TUMS_ZAPROS[Nst], //��ப� ����� ����ᮢ ��� �⮥� ����
REG_TUMS[Nst][12], //ॣ����� ������ �᭮����� ������ �⮥� ����
kod_marsh,
kod_marsh1,
//----१�ࢭ� ����� 
ADR_TUMS_OUT1,//���᭠� ���� १�ࢭ��� �����(���⠭樨)������
BUF_OUT1[SIZE_BUF],//���� �뤠� � १�ࢭ�� ������ࠫ�
UKAZ_ZAP1,//㪠��⥫� �� ���� ����� � १�ࢭ�� ���� �뢮��
UKAZ_OUT1,//㪠��⥫� �� ���� �뤠� � १�ࢭ�� ���� �뢮��
ZAPROS1[6], //�࠭�⥫� १�ࢭ�� ����ᮢ � �⮩�� ����
ZPRS_TMS1,  //�ਧ��� �뤠� ����� �� १�ࢭ��� ������ � �⮩��
ZAPROS_TUMS1,//ॣ���� ����� १�ࢭ�� ����ᮢ � �⮩�� ����
REG_TUMS1[Nst][12], //ॣ����� ������ १�ࢭ��� ������ �⮥� ����
//===========================================================================
//=============������� � ��������� ��� ������ � ������������ ��������
//===========================================================================
//--------------------------�������� ����� 
//-----����� �।��饣� �ࢥ� 
ADR_PRED_IN,   //���᭠� ���� ���뫪�,����祭��� �� �।��饣� �ࢥ� 
BUF_IN_PRED[100], //���� �ਥ�� �� �।��饣� �ࢥ� 
UKAZ_IN_PRED, //㪠��⥫� �� ���� ����� � ���� �ਥ�� �।��饣� �ࢥ� 
BEGIN_PRED,  //�ਧ��� ����祭�� ��砫� ���뫪� �� �।��饣� �ࢥ� 
END_PRED,    //�ਧ��� ����祭�� ���� ���뫪� �� �।��饣� �ࢥ� 
RGSTR_PRED[14],//ॣ���� �ਥ�� ������ �� �।��饣� �ࢥ� 
//-----����� ᫥���饣� �ࢥ� 
ADR_NEXT_IN,//���᭠� ���� ���뫪�,����祭��� �� ᫥���饣� �ࢥ� 
BUF_IN_NEXT[100],//���� �ਥ�� �� ᫥���饣� �ࢥ� 
UKAZ_IN_NEXT,//㪠��⥫� �� ���� ����� � ���� �ਥ�� ᫥���饣� �ࢥ� 
BEGIN_NEXT,  //�ਧ��� ����祭�� ��砫� ���뫪� �� ᫥���饣� �ࢥ� 
END_NEXT,   //�ਧ��� ����祭�� ���� ���뫪� �� ᫥���饣� �ࢥ� 
RGSTR_NEXT[14],//ॣ���� �ਥ�� ������ �� ᫥���饣� �ࢥ� 
//=======================================================================
//------------------------���������� ����� 
//-----�।��騩 �ࢥ�
ADR_PRED_OUT, //���᭠� ���� ����� � �।��騩 �ࢥ�
BUF_OUT_PRED[100], //���� �뤠� � �।��騩 �ࢥ�
UKAZ_ZAP_PRED,//㪠��⥫� �� ���� ����� � ���� �뢮�� �।��饣� �ࢥ� 
UKAZ_OUT_PRED,//㪠��⥫� �� ���� �뤠� � ���� �뢮�� �।��饣� �ࢥ� 
ZAPROS_PRED[14],//�࠭�⥫� ����ᮢ ��� �।��饣� �ࢥ� 
ZPRS_PRED, //�ਧ��� �뤠� ����� � �।��騩 �ࢥ�
KVIT_PRED[14],//ॣ����� ���⠭権 ��� ��।�� � �।��騩 �ࢥ�
//-----᫥���騩 �ࢥ�
ADR_NEXT_OUT, //���᭠� ���� ����� � ᫥���騩 �ࢥ�
BUF_OUT_NEXT[100], //���� �뤠� � ᫥���騩 �ࢥ�
UKAZ_ZAP_NEXT,//㪠��⥫� �� ���� ����� � ���� �뢮�� ᫥���饣� �ࢥ�
UKAZ_OUT_NEXT,//㪠��⥫� �� ���� �뤠� � ���� �뢮�� ᫥���饣�
ZAPROS_NEXT[14],//�࠭�⥫� ����ᮢ ��� ᫥���饣� �ࢥ� 
ZPRS_NEXT, //�ਧ��� �뤠� ����� � ᫥���騩 �ࢥ�
KVIT_NEXT[14],//ॣ����� ���⠭権 ��� ��।�� � ᫥���騩 �ࢥ�

//--------------------������� � ���������� ��� ������ � ������
BEGIN_ARM,            //�ਧ��� ����祭�� ��砫� ���뫪� �� ���
BEGIN_ARM1,
END_ARM,            //�ਧ��� ����祭�� ���� ���뫪� �� ��� 
END_ARM1,
END_SHN,
ZPRS_RM,                //�ਧ��� �뤠� ����� � ��� 
BUF_IN_ARM[100],         //ॣ����� �ਥ�� ������ ��� �⮥� ��� 
BUF_IN_ARM1[100],
BUF_IN_SHN[100],
BUF_OUT_ARM[100],
BUF_OUT_ARM1[100],
BUF_OUT_SHN[100],
OSN1_KS[28],//���� �ਥ�� ��ࢮ� ���뫪� �� �᭮����� ������ �� ����
OSN2_KS[28],//���� �ਥ�� ��ன ���뫪� �� �᭮����� ������ �� ����
OSN_SHN[28],
REZ1_KS[28],//���� �ਥ�� ��ࢮ� ���뫪� �� १�ࢭ��� ������ �� ����
REZ2_KS[28],//���� �ਥ�� ��ன ���뫪� �� १�ࢭ��� ������ �� ����
KOM_TIME[8], //ॣ���� ������� ��⠭���� �६��� 
KOMANDA_MARS[Narm][2][12],//ॣ����� ������ ������⭮�� �ࠢ�����
KOMANDA_RAZD[Narm][2][12],//ॣ����� ������ ࠧ���쭮�� �ࠢ�����
KOMANDA_ST[Nst][15], //�������� ॣ���஢ ������ ࠧ���쭮�� �ࠢ����� ��� �⮥� 
KVIT_ARMu[Narm][2][12], //�������� ���ᨢ�� ��� �࠭���� ���⠭権 ����
KVIT_ARMu1[Narm][2][12], //�������� ���ᨢ�� ��� ᮮ�饭�� �� �ࢥ� ����
VYVOD_ARM,
VYVOD_ARM1,
VVOD_ARM,
VVOD_ARM1,
OUT_NEXT_STOP,
OUT_PRED_STOP,
SCREEN_PRED,
SCREEN_NEXT,
PROCESS;



extern unsigned char
FR4[1900], //���ᨢ ��࠭�祭��
ZAFIX_FR4[1900],
PEREDACHA[1900], //���ᨢ ��� 䨪�樨 䠪� ��।�� ������
PRIEM[1900];//���ᨢ ��� 䨪�樨 䠪� �ਥ�� ���⠭権

extern unsigned int
  tst_int,  //�㦥���� ��६����� ��� ���஢���� �ணࠬ�� � ������
  KVIT_ARM[Narm][2][18],//ॣ����� �࠭���� ����஫��� �㬬, �ਭ���� �� ���
  STR[Nst],
  SIG[Nst],
	DOP[Nst],
	DOP_B[Nst],
	DOP_D[Nst],
	DOP_T[Nst],
	UCH[Nst],
  PUT[Nst],
  UPR[Nst],
  DGN[Nst],
  N_SOOB[Nst],//�᫮ ���ଠ樮���� ᮮ�饭�� �� �⮩�� ����

	SPSTR[100][5],

	SPSIG[100][5],

	SPDOP[100][5],
	SPDOP_B[100][5],
	SPDOP_D[100][5],
	SPDOP_T[100][5],

	SPKON[100][5],

	SPSP[100][10],
	SPPUT[100][10];
extern int
	flag_err,
	file_arc,
	ISPOLNIT[Nst*3],
//  file_arm_in,
//  file_arm_out,
  ho_ur,
  mi_n,
  se_c,
  sosed_NEXT,  //�ਧ��� ������ ᫥���饣� �ᥤ�
  sosed_PRED,  //�ਧ��� ������ �।��饣� �ᥤ�
  inf_TUMS,   //�ਧ��� ������ ���ଠ樨 �� ����� 
  inf_ARM,    //�ਧ��� ������ ���ଠ樨 �� ����
  NACHALO,  //��ꥪ� ��砫� ������� 1-�� ࠩ���
  NACHALO1, //��ꥪ� ��砫� ������� 2-�� ࠩ���
  KONEC,    //��ꥪ� ���� ������� 1-�� ࠩ���
  KONEC1,   //��ꥪ� ���� ������� 2-�� ࠩ���
  RESET_TIME,
  pako_fil,
  bd_osn_fil,
  inp_fil,
  out_fil,
  spstr_fil,
  spsig_fil,
  spdop_fil,
  spkon_fil,
  spspu_fil,
  spdgn_fil,
  spput_fil,
  fr3_fil;
  
extern struct MARS_ALL
{
	char KMND;
	unsigned int NACH;
	int END;
	int NSTR;
	unsigned long POL_STR;
	int KOL_STR[Nst];
	unsigned int STREL[Nst][10];
	unsigned int SIG[Nst][10];
	unsigned int SP_UP[Nst][10];
	unsigned char SOST;
	int STOYKA[Nst];
}MARSHRUT_ALL[Nst*3];

extern struct MARS_ST
{
	char NEXT_KOM[14];
	char NUM;
	unsigned char SOST;
	time_t T_VYD;
	long T_MAX;
}MARSHRUT_ST[Nst][MARS_STOY];

extern struct PAKET
{
  unsigned int KS_OSN;
  unsigned char ARM_OSN_KAN;
  unsigned int KS_REZ;
  unsigned char ARM_REZ_KAN;
}PAKETs[32];

extern struct OBJect
{
  unsigned int OBJ[21];
  unsigned int LAST;
}OBJ_ARMu[32],OBJ_ARMu1[32];

extern  struct TRAS
{
  unsigned int object;
  unsigned char tip;
  unsigned char stoyka;
  unsigned char podgrup;
  unsigned char kod_bit;
}TRASSA[200],TRASSA1[200];

extern union REGS rg;
extern struct dosdate_t dat;
extern struct dostime_t tt;
