unit VarStruct;
//------------------------------------------------------------------------------
//                 ���������� ��� ��-���, ���-��, ��������� ������
//
//
//    ������ 1
//    �������� 5
//
//    ���� ��������� �������� 05 �������� 2006 ����
//------------------------------------------------------------------------------

{$INCLUDE CfgProject}

interface

uses
  Windows,
  Classes,
  Registry,
  Graphics,
  TypeRpc;

var
  reg : TRegistry;        // ������ ��� ������� � ���������� � �������
  ListMessages : string;  // ������ �������������� ��� ��������� �� ��������� ����
  newListMessages : Boolean;
  ListNeisprav : string;  // ������ ��������������
  newListNeisprav : Boolean;
  ListDiagnoz  : string;  // ������ ��������������� ��������� ���
  newListDiagnoz : Boolean;
  database     : string;  // ���� � ���� ������, ����������� ������ �������

  findSrv  : boolean;
  mem_page : boolean;     // false - �������� �� Tablo1, true - �������� �� Tablo2
  tab_page : boolean;     // ������������� ������� ��� ����������� ������������� ���������
  command  : boolean;     // ������� ������ ������� �� ����
  ID_obj   : smallInt;    // ������������� ��������� ������� ������������ ��� ������ ��������
  ID_menu  : SmallInt;    // ������������� ��������� �������/����
  cur_obj  : smallInt;    // ����� ������� ������������ ��� �������� �����
  StartObj : smallint;    // ����� �������, � �������� ���������� ���������� � ����������
{$IFNDEF RMDSP}
  ID_ViewObj : smallInt;  // ������ ������� ��� ������������ �������� ��������� � ���-�� � ��� ��������� ������
{$ENDIF}

  // ��������� ������������ ������ ������ �� ���
  asTestMode : Byte; // ������� ��������� ������ �������� ��� - $aa,
                     // ��� ���������� ������ - $55
  WorkMode   : TWorkMode;

  cntObjZav   : Integer;  // ����������� �RC ������ ����������� �������
  cntObjView  : Integer;  // ����������� CRC ������ ����������� �������
  cntOVBuffer : Integer;  // ����������� CRC ����� ����������� �������
  cntObjUprav : Integer;  // ����������� CRC ������ ���������� �������

  Lex : array[1..1000] of TStringLex; // ������ �������� ��������� �� ���
  Lex2 : array[1..500] of TStringLex; // ������ ���������
  Lex3 : array[1..500] of TStringLex;

  LinkFR3 : array[1..8192] of TLinkFR3; // ������ �������� � ������
{$IFDEF RMSHN}
  LinkTCDC : array[1..80] of TLinkTCDC; // ������ �������� � ������ ��
{$ENDIF}
  MYT : array[1..9] of Word; // ������ �������� FR3 ���������� ���������� ��������� �� �����

  CmdBuff     : TCommands;           // ������� ����������� ����������, ����������� � �.�.
  ParamDouble : TParamDouble;        // ����� ���������� Double ��� �������� �� ������

  maket_strelki_index : SmallInt;
  maket_strelki_name  : string;

  MyMarker       : array[1..2] of Boolean;  // ������� ��������� ������� �� ������� 1,2
  LastRcv        : TDateTime; // ����� ���������� ������ ������� �����
  LastTime       : TDateTime; // �������� ���������� �������
  LastSync       : TDateTime; // ����� ��������� �������������
  LastMove       : TDateTime; // ����� ���������� ����������� ������� �����
  LastReper      : Double;    // ����� ���������� ���������� 10-�� ��������� ������ ���������
  ObjHintIndex   : Integer;   // ������ ������������� �������� �������
  LockHint       : Boolean;   // ���������� ������ �������� ������� ��� ��������� ������ ����
  LockCommandDsp : Boolean;   // ���������� �������� ��������� �� ����� ���������� ��������������
  ShowWarning    : Boolean;   // ������� ������ ������� ���������, ����������� �������� �������
  SingleBeep     : Boolean;   // ������ �������� ������ 1
  SingleBeep2    : Boolean;   // ������ �������� ������ 2
  SingleBeep3    : Boolean;   // ������ �������� ������ 3
  SingleBeep4    : Boolean;   // ������ �������� ������ 4
  SingleBeep5    : Boolean;   // ������ �������� ������ 5
  SingleBeep6    : Boolean;   // ������ �������� ������ 6
  Ip1Beep        : Boolean;   // ������ ������� �����������
  Ip2Beep        : Boolean;   // ������ ������� �����������
  MsgStateRM     : string;    // ��������� �� ������ ������ - ���
  MsgStateClr    : Integer;   // ��� ����� ���������
  CmdSendT       : Double;    // ����� ������ ������� � �����
  LockTablo      : Boolean;   // ���������� ����� �� ����� ����������
  KOKCounter     : Integer;   // ������� ������� ������ ��
  NewNeisprav    : Boolean;   // �������� ����� �������������
  OperatorDirect : Double;    // ����� ���������� ����������� ��������� � ���������� ��� �����

//*********************************************************************
//
//
//                            ��� �����
//
//
//*********************************************************************

  isChengeRegion : Boolean;// ���������� ��������� �������� ���� ��� ����� ������ ����������
  Tablo1        : TBitmap; // 1- �� ����� �����
  Tablo2        : TBitmap; // 2- �� ����� �����
  Sound         : Boolean;  // �������� ������������� ����
  LastX         : SmallInt; // ���������� ������� �� �����������
  LastY         : SmallInt; // ���������� ������� �� ���������
  shortmsg      : array[1..4] of string; // ������ �������� ���������
  shortmsgcolor : array[1..4] of TColor;
  FixMessage    : TFixMessage;  // ��������� ���������, ��������� �������������
  DspMenu       : TDspMenu;     // ��������� ��������� �������� ���� ��-���
  DspCommand    : TDspCommand;  // ��������� ������ ������� ���������
  VspPerevod    : TVspPerevod;  // ��������� �������� ���������������� �������� �������
  OtvCommand    : TOtvCommand;  // ��������� �������� ����� �������������� ������������� �������
  IntervalAutoMarsh : Integer;  // ����� �������� ��� ������ ��������� ������� ��������� �������� ������������

  ObjView  : array[1..2000]  of TOVStruct;   // ������ ���������� ��������
  OVBuffer : array[1..2000]  of TOVBuffer;   // ��������� ����� �������� ������������ � ����������� �� �����
  ObjZav   : array[1..2000]  of TOZStruct;   // ������ �������� ������������
  ObjUprav : array[1..2000]  of TObjUprav;   // ������ �������� ����������
  MsgList  : array[1..10000] of string;      // ��������� ��� ���������� ��������
  AKNR     : array[1..1000]  of TAutoTrace;  // ������ ����
  Timer    : array[1..100]   of Integer;     // ������ �������� ��� ����������� �� �����
  Ikonki   : array[1..200,1..3] of SmallInt; // ������ ��� ���������� ������� �� ������ ��1
  Ikonki2  : array[1..200,1..3] of SmallInt; // ������ ��� ���������� ������� �� ������ ��2
  Stellaj  : array[1..20]     of Boolean;    // ������ ��� ������ �� ���������
  IkonPri  : Byte;                           // ��������� ������� �������
  IkonNew  : Boolean;                        // ������� ������� � ������� �������
  IkonSend : Boolean;                        // ������� ������������� �������� ������� �������� � ����� ���
  IkonkaMove   : Boolean; // ������� ���������� ����������� ������
  IkonkaMoved  : Boolean; // ������� ������������ ����������� ������
  IkonkaDown   : Integer; // ����� ������������ ������
  IkonkaDeltaX : Integer; // �������� ����� ������� ������
  IkonkaDeltaY : Integer; //
  SetIkonRezNonOK : Boolean; // ��������� ��������� ������ � ���������� ��-��� ��� ������� ���
  BuffArc  : array[1..32768] of byte;   // ����� ��� ������ � �����
  LenArc   : integer;                   // ����� ������ ��� ������ � �����

  SrvActive : Byte;                     // ������ ��������� �������
  SrvCount  : Byte;                     // ������� ���������� ���������� ��������
  SrvState  : Byte;                     // ����� ��������� ��������
  DirState  : array[1..4] of Byte;      // ������ ��������� ������� ���������� � ������� ��� ����� (�� ����� ��-��� ������������ 1-�, �� ���-�� - �������� ��� ������� ��-���)
  ArmSrvCh  : array[1..2] of Byte;      // ����� ��� ����������� ��������� ������� ����� � ������ ������� (���� ����������)
  ArmAsuCh  : array[1..3] of Byte;      // ����� ��� ����������� ��������� ������� ����� � ������� ������� (���� ���)
  ArmDCCh   : array[1..2] of Byte;      // ����� ��� ����������� ��������� ������� ����� � ��-�� (���� ����������)
  configRU  : array[1..3] of TConfigRU; // ������ �������� ������� ����������
  config    : TConfigProject;           // ��������� ��������� �������� ������������ ������ �������
  StateRU   : Byte;                     // ��������� �� �������
  ArmState  : Byte;                     // ��������� "������" ����������
  // ���������� �����:
  // 1- ���������� ����������, 2- ���������� ����������, 3- ������, 4- �����������,
  // 5- ���.������� �������, 6- ������ ���, 7- ��������� ��������� ������, 8- ����������� �����������
  StartRM   : Boolean;                  // ������� ���������� ��������� ��������
  ChDirect  : Boolean;                  // ������� ��������� ������� ��������� ������� ����������
  StDirect  : Boolean;                  // �������� ��������� ����������, ���������� �� �������
  ChRegion  : Boolean;                  // ������� ��������� ������� ��������� ������ ����������
  NewRegion : Byte;                     // ������ ������ ������ ����������
  DiagnozON : Boolean;                  // ������� ��������� ����������� ��� �� ����
  SyncTime  : Boolean;                  // ������� ������������ ������� ������������� ������� ��������
  SyncCmd   : Boolean;                  // ������� ������ ������� ������������� ������� �������� � ����� ��

  hWaitKanal : THandle; // ������� �������� ��������� ������ �����

{$IFDEF RMARC}
  ArcReady       : Boolean;   // ������ ������ ������ ��� �����������
  SpeedZoom      : Double;    // ������� ������� ��� ��������� ������
  CurrentTime    : Double;    // ������� ����� ��� �������������� ���������
  LastSyncArc    : Double;    // ��������� ����� ���������� ������� ��� ��������� ������
  LastFixed      : integer;   // ������� ���������� ������������������� ���������
  SndNewWar,
  SndNewUvk,
  SndNewMsg : Boolean;
{$ENDIF}

  ObjectWav    : TStringList; // ������ �������� �������� ��� ����������� ��������� �������� ������������
  IpWav        : TStringList; // ������ �������� �������� ��� ����������� ������� �������� �����������
{$IFDEF RMSHN}
  NotifyWav    : TStringList; // ������ �������� �������� ��� ����������� ������� �� ��� ��
  FixNotify    : array[1..10] of TFixNotify;  // ��������� ��� �������� ������� �� ��
  NameFR       : TStringList; // ������ ���� �������� FR3 �� ��
{$ENDIF}

  TryFixed : Integer; // �������� ���� � ������ ���������
  LoopSync   : Boolean; // ������� ������������ ������� ������ � ��������
  LoopHandle : THandle; // ������������� ������ ��������� ������� ������ � ��������
  ReBoot     : Boolean; // ������� ������������� ����������� ���������� ����� ���������� ������ ��������� �����

//  TestHead : Boolean;

const
  ChTO = 3; // ������������ ��������� ��������� ����� � ������� ������
  MaxLiveCtr = 500; // ������������ �������� �������� �������� ������� ������

implementation
end.