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
extern void (interrupt far *s_vect1)(); //хранитель старого вектора IRQ5
extern void (interrupt far *s_vect2)(); //хранитель старого вектора IRQ7
extern void (interrupt far *s_timer)(); //хранитель старого вектора таймера
extern void interrupt far reading_char1();//обработчик прерываний магистралей ТУМС
extern void interrupt far reading_char2();//обработчик прерываний магистралей АРМ
#ifdef WORK
extern void interrupt far TIMER_TIC();//обработчик прерываний СИСТЕМНОГО ТАЙМЕРА
#else
void TIMER_TIC();//имитация обработчика прерываний СИСТЕМНОГО ТАЙМЕРА
#endif
extern void interrupt far PRNTSCR();//обработчик прерываний PRNSCR
extern void interrupt far CNTRLBREAK();//обработчик прерываний CNTRLBREAK
extern void interrupt far KEYBRD();//обработчик прерываний клавиатуры
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
extern void (interrupt far *s_vect1)(); //хранитель старого вектора IRQ5
extern void (interrupt far *s_vect2)(); //хранитель старого вектора IRQ7
extern void (interrupt far *s_timer)(); //хранитель старого вектора таймера 

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
VIRT_DISK[3], //имя виртуального диска для хранения баз 
NAME_FILE[14],
KVIT_TUMS[Nst][10],//регистры квитанций для передачи в основной канал ТУМС
KVIT_TUMS1[Nst][10], //регистры квитанций для передачи в резервный канал ТУМС
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
ADR_TUMS_OSN,     //базовый адрес основного канала обмена данными с ТУМСами 
ADR_TUMS_REZ,     //базовый адрес резервного канала обмена данными с ТУМСами 
ADR_ARM_OSN,      //базовый адрес основного канала обмена данными с АРМами
ADR_ARM_REZ,      //базовый адрес резервного канала обмена данными с АРМами
ADR_SHN_OSN,
ADR_SERV_PRED,    //базовый адрес для обмена с предыдущим сервером
ADR_SERV_NEXT;    //базовый адрес для обмена с последующим сервером
extern unsigned long
POOO[1900],
SEC_time,
FIR_time,
second_time,
first_time,
long_test;

extern unsigned int error1,error2,
CIKL_MAIN,	//$$$$ - 14_04_07 $$$$ - переменная определения четного и нечетного основного цикла
KOL_VYD_MARSH[Nst],//$$$$ счетчик числа раз выдачи одного маршрута (разрешен только один повтор выдачи)
Schet_Com[Nst],
Schet_Takt[Nst],
KOL_VO,  //общее число объектов на станции
chas_,
min_,
sec_,
PAROL,//$$$$ - 14_04_07 - добавлена для выхода при двух работающих серверах
povtor_out,
POVTOR_OTKR,
FLAG_KOM,
PAKET[32][2],//контрольные суммы CRC-16 для номеров переданных пакетов
MYTHX[Nst],  //признаки состояния маршрутов в стойках
TUMS_RABOT[Nst], //признак занятости стойки
NOVIZNA[MAX_NEW], //массив последней принятой новизны
NOVIZNA_FR4[10], //массив последней новизны для FR4
BD_OSN[1900][17],  //строка описания объекта
inp_ob[1900],
out_ob[1900],
bd_osn[16],
nom_new, //указатель номера элемента массива новизны для следующей записи
new_fr4, //указатель номера элемента массива новизны для следующей записи
nom_per,
FR4_NEXT, //указатель на объект для следующего сервера
FR4_PRED, //указатель на объект для предыдущего сервера
mld_ba,str_ba, //делители для каналов обмена ТУМС
mld_ba1,str_ba1,            //делители для каналов обмена АРМ
mld_ba2,str_ba2,            //делители для каналов сервера
V1,//вектор для обработки irq5
V2,//вектор для обработки irq7
VT,//вектор для обработки таймерного прерывания
ST,  //номер активной стойки основного канала
ST1, //номер активной стойки резервного канала
s01,s02,
stak,
ttt,
ttt1,
hndl,
REGIM,  //режим вывода на экран 
cikl,//счетчик циклов таймера
cikl_marsh, //счетчик циклов анализатора маршрутов
cikl_arm,
cikl_out_arm, //счет 14-ти байтных циклов вывода данных в АРМ по осн.каналу
cikl_out_arm1,//счет 14-ти байтных циклов вывода данных в АРМ по рез.каналу
cikl_out_shn,
cikl_in_arm, //счет 14-ти байтных циклов приема данных из АРМ по осн.каналу
cikl_in_arm1, //счет 14-ти байтных циклов приема данных из АРМ по рез.каналу
cikl_in_shn,
cikl_out_pred, //то же для вывода в предыдущий сервер
cikl_out_next, //то же для вывода в следующий сервер
cikl_in_pred, //то же для приема из предыдущего сервера 
cikl_in_next, //то же для приема из следующего сервера 
n_tums,
n_tums1,
Y_KOM,
X_out[Nst],         //X вывода для основного канала стойки
Y_out[Nst],    //Y вывода для основного канала стойки
X_out_rez[Nst],     //X вывода для резервного канала стойки
Y_out_rez[Nst],//Y вывода для резервного канала стойки
X_in[Nst],           //X ввода для основного канала стойки
Y_in[Nst],     //Y ввода для основного канала стойки
X_in_rez[Nst],           //X ввода для основного канала стойки
Y_in_rez[Nst];     //Y ввода для основного канала стойки

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
MARSH_VYDAN[Nst], //признаки выдачи маршрутов в стойки
OK_KNOPKA,      //индикатор нажатости кнопки ОК
SHET_KOM[Nst],  //счетчик повторов выдачи разовых команд (если не было квитанции)
SHET_MARSH[Nst],//счетчик повторов выдачи маршрутов (если не было квитанции)
tiki_tum[Nst],
PAUZA,
PAROL_TXT[4], //$$$$ - 14_04_07 - добавлена для выхода при двух работающих серверах
SERVER,       //номер текущего сервера 
SERVER_PRED,//номер предыдущего сервера 
SERVER_NEXT,  //номер следующего сервера 
KOD_SERVER,  //код состояния системы серверы+АРМы
STOP_SERVER, //флаг приостановки работы сервера 
ACTIV_SERV,
POLUCHIL_UPR_OT_NEXT,//флаги  получения управления
POLUCHIL_UPR_OT_PRED,

PEREDAL_UPR_K_NEXT,  //флаг передачи управления следующему
PEREDAL_UPR_K_PRED,   //флаг передачи управления следующему
REG_ARM[100], //регистр информации, принятой из АРМа по основному каналу
REG_ARM1[100], //регистр информации, принятой из АРМа по резервному каналу
ZAGRUZKA,
VVOD[Nst][48][7],
SOST_SERV[3], //байты состояния серверов
SOST_RANJ[Nranj], //байты состояния районов 
podtv_NEXT, //признак наличия подтверждения от предыдущего сервера 
podtv_PRED, //признак наличия подтверждения от следующего сервера 

N_PAKET, //номер текущего пакета передачи в АРМ 
str_bd[32], //строка описания объекта 
FR3_ALL[1900][34],  //строка чтения/записи массива FR3
FR3[34],
ACTIV,
symbol,
symbol1,
vvod_set,
OSNOVA,      //идентификатор статуса сервера 
STATUS[Narm],//идентификатор статуса АРМ (РАЙОН+ ОСНОВНОЙ/РЕЗЕРВНЫЙ)
KOD_STR[2],//код перевода стрелок в +,-
KOD_SIGO[2],//код открытия сигнала маневровый,поездной
KOD_SIGZ[2],//код закрытия сигнала маневровый,поездной
KOD_VKL[2],//Коды включения объектов для битов 0,1
KOD_OTKL[2],//Коды отключения объектов для битов 0,1
KOD_RAZD,  //код команды искусственной разделки
KOD_KOMPL, //код команды переключения комплекта 
KOD_GRI,   //код команды включения выдержки ГРИ 
KONFIG_ARM[Narm][Nranj],//конфигуратор АРМов 
KNOPKA_OK[Narm], //массив состояния кнопок ответственных команд
ZAPROS_ARM,   //сдвигаемый регистр флагов основных запросов в АРМЫ
ZAPROS_ARM1,  //сдвигаемый регистр флагов резервных запросов в АРМЫ
summa,
atrib,  //атрибут текстового экрана
dlt[10],
//=============МАССИВЫ И УКАЗАТЕЛИ ДЛЯ РАБОТЫ С МАГИСТРАЛЯМИ ТУМС
//=================================================================
//--------------------------ПРИЕМНАЯ ЧАСТЬ 
//-----основной канал
ADR_TUMS_IN,   //адресная часть посылки,полученной по основному каналу ТУМС
REG_IN[Nst][12], //буферa приема из основного канала
BUF_IN[SIZE_BUF], //буферa приема из основного канала
UKAZ_IN, //указатель на байт записи в буфере приема основного канала
BEGIN_TUMS,  //признак получения начала посылки по основному каналу ТУМС
END_TUMS,    //признак получения конца посылки по резервному каналу ТУМС
//-----резервный канал 
ADR_TUMS_IN1,//адресная часть посылки,полученной по резервному каналу ТУМС
REG1_IN[Nst][12], //буферa приема из резервного канала
BUF_IN1[SIZE_BUF],//буферa приема по резервному каналу
UKAZ_IN1,//указатель на байт записи в буфере приема резервного канала
BEGIN_TUMS1, //признак получения начала посылки по резервному каналу ТУМС
END_TUMS1,   //признак получения конца посылки по резервному каналу ТУМС
//=======================================================================
//------------------------ПЕРЕДАЮЩАЯ ЧАСТЬ 
//-----основной канал 
ADR_TUMS_OUT, //адресная часть основного запроса(квитанции)ТУМСам
BUF_OUT[SIZE_BUF], //буфер выдачи в основную магистраль
UKAZ_ZAP,//указатель на байт записи в основном буфере вывода
UKAZ_OUT,//указатель на байт выдачи в основном буфере вывода
ZAPROS[6], //хранитель основных запросов в стойки ТУМС
ZPRS_TMS, //признак выдачи запроса по основному каналу в стойку
ZAPROS_TUMS,//регистр номера основных запросов в стойки ТУМС
TUMS_ZAPROS[Nst], //строка кодов запросов для стоек ТУМС
REG_TUMS[Nst][12], //регистры команд основного канала стоек ТУМС
kod_marsh,
kod_marsh1,
//----резервный канал 
ADR_TUMS_OUT1,//адресная часть резервного запроса(квитанции)ТУМСам
BUF_OUT1[SIZE_BUF],//буфер выдачи в резервную магистраль
UKAZ_ZAP1,//указатель на байт записи в резервном буфере вывода
UKAZ_OUT1,//указатель на байт выдачи в резервном буфере вывода
ZAPROS1[6], //хранитель резервных запросов в стойки ТУМС
ZPRS_TMS1,  //признак выдачи запроса по резервному каналу в стойку
ZAPROS_TUMS1,//регистр номера резервных запросов в стойки ТУМС
REG_TUMS1[Nst][12], //регистры команд резервного канала стоек ТУМС
//===========================================================================
//=============МАССИВЫ И УКАЗАТЕЛИ ДЛЯ РАБОТЫ С МАГИСТРАЛЯМИ СЕРВЕРОВ
//===========================================================================
//--------------------------ПРИЕМНАЯ ЧАСТЬ 
//-----канал предыдущего сервера 
ADR_PRED_IN,   //адресная часть посылки,полученной от предыдущего сервера 
BUF_IN_PRED[100], //буфер приема от предыдущего сервера 
UKAZ_IN_PRED, //указатель на байт записи в буфере приема предыдущего сервера 
BEGIN_PRED,  //признак получения начала посылки от предыдущего сервера 
END_PRED,    //признак получения конца посылки от предыдущего сервера 
RGSTR_PRED[14],//регистр приема данных от предыдущего сервера 
//-----канал следующего сервера 
ADR_NEXT_IN,//адресная часть посылки,полученной от следующего сервера 
BUF_IN_NEXT[100],//буфер приема от следующего сервера 
UKAZ_IN_NEXT,//указатель на байт записи в буфере приема следующего сервера 
BEGIN_NEXT,  //признак получения начала посылки от следующего сервера 
END_NEXT,   //признак получения конца посылки от следующего сервера 
RGSTR_NEXT[14],//регистр приема данных от следующего сервера 
//=======================================================================
//------------------------ПЕРЕДАЮЩАЯ ЧАСТЬ 
//-----предыдущий сервер
ADR_PRED_OUT, //адресная часть запроса в предыдущий сервер
BUF_OUT_PRED[100], //буфер выдачи в предыдущий сервер
UKAZ_ZAP_PRED,//указатель на байт записи в буфере вывода предыдущего сервера 
UKAZ_OUT_PRED,//указатель на байт выдачи в буфере вывода предыдущего сервера 
ZAPROS_PRED[14],//хранитель запросов для предыдущего сервера 
ZPRS_PRED, //признак выдачи запроса в предыдущий сервер
KVIT_PRED[14],//регистры квитанций для передачи в предыдущий сервер
//-----следующий сервер
ADR_NEXT_OUT, //адресная часть запроса в следующий сервер
BUF_OUT_NEXT[100], //буфер выдачи в следующий сервер
UKAZ_ZAP_NEXT,//указатель на байт записи в буфере вывода следующего сервера
UKAZ_OUT_NEXT,//указатель на байт выдачи в буфере вывода следующего
ZAPROS_NEXT[14],//хранитель запросов для следующего сервера 
ZPRS_NEXT, //признак выдачи запроса в следующий сервер
KVIT_NEXT[14],//регистры квитанций для передачи в следующий сервер

//--------------------МАССИВЫ И ПЕРЕМЕННЫЕ ДЛЯ РАБОТЫ С АРМами
BEGIN_ARM,            //признак получения начала посылки от АРМ
BEGIN_ARM1,
END_ARM,            //признак получения конца посылки от АРМ 
END_ARM1,
END_SHN,
ZPRS_RM,                //признак выдачи запроса в АРМ 
BUF_IN_ARM[100],         //регистры приема данных для стоек АРМ 
BUF_IN_ARM1[100],
BUF_IN_SHN[100],
BUF_OUT_ARM[100],
BUF_OUT_ARM1[100],
BUF_OUT_SHN[100],
OSN1_KS[28],//буфер приема первой посылки по основному каналу из АРМа
OSN2_KS[28],//буфер приема второй посылки по основному каналу из АРМа
OSN_SHN[28],
REZ1_KS[28],//буфер приема первой посылки по резервному каналу из АРМа
REZ2_KS[28],//буфер приема второй посылки по резервному каналу из АРМа
KOM_TIME[8], //регистр команды установки времени 
KOMANDA_MARS[Narm][2][12],//регистры команд маршрутного управления
KOMANDA_RAZD[Narm][2][12],//регистры команд раздельного управления
KOMANDA_ST[Nst][15], //комплект регистров команд раздельного управления для стоек 
KVIT_ARMu[Narm][2][12], //комплект массивов для хранения квитанций АРМу
KVIT_ARMu1[Narm][2][12], //комплект массивов для сообщений от сервера АРМу
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
FR4[1900], //массив ограничений
ZAFIX_FR4[1900],
PEREDACHA[1900], //массив для фиксации факта передачи данных
PRIEM[1900];//массив для фиксации факта приема квитанций

extern unsigned int
  tst_int,  //служебная переменная для тестирования программы и данных
  KVIT_ARM[Narm][2][18],//регистры хранения контрольных сумм, принятых из АРМ
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
  N_SOOB[Nst],//число информационных сообщений от стойки ТУМС

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
  sosed_NEXT,  //признак наличия следующего соседа
  sosed_PRED,  //признак наличия предыдущего соседа
  inf_TUMS,   //признак наличия информации из ТУМСа 
  inf_ARM,    //признак наличия информации из АРМа
  NACHALO,  //объект начала маршрута 1-го района
  NACHALO1, //объект начала маршрута 2-го района
  KONEC,    //объект конца маршрута 1-го района
  KONEC1,   //объект конца маршрута 2-го района
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
