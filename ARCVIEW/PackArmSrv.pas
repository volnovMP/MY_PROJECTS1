unit PackArmSrv;
//****************************************************************************
//
//       ��������� ������ � ������� ������ - �� ���
//
//****************************************************************************


interface

uses
  Windows, Dialogs, SysUtils, Controls, Classes, Messages, Graphics, Forms, StdCtrls, Registry, SyncObjs, comport;


type TKanalErrors = (keOk,keErrCRC,keErrSrc,keErrDst);

type TKanal = record
    config  : string;
    Index   : Byte;
    nPipe   : string;   // ��� �����
    port    : TComPort;
    active  : Boolean;
    rcvsoob : string;
    isrcv   : boolean;
    issync  : boolean;
    iserror : boolean;
    cnterr  : integer;
    rcvcnt  : integer;
  end;

const
  ArchivPath : string = 'c:\arc\';
  LnSoobSrv  : integer = 70;    // ����� ��������� �� �������

  FR_LIMIT = 4096;

var
  AnsverTimeOut    : Double; // �������� ����������� ������������� ������� ������� ������ ���-������ ����� ������ �������
  MaxTimeOutRecave : Double; // �������� ����������� ������������� ������� �������� ������ � FR3 �� ������ �������� ����������� (������������� ������)
  NewFR    : array[1..2] of string;     // ����� ������� ��� ������
  NewCmd   : array[1..2] of string;     // ����� ��������� ��������� � ������ ��� ������
  NewMenuC : string;                    // ����� ������ ����, �������������� ����������
  NewMsg   : string;                    // ����� ��������� �� ������ FR3
  BackCRC  : array[1..20,1..2] of WORD; // ����� ���������
  LastCRC  : array[1..2] of Byte;       // ��������� � ���������� ���������
  LastSrv  : array[1..2] of Byte;       // ����� �������������� �������
  buffarc  : array[1..32768] of byte;   // ����� ��� ������ � �����
  CmdCnt   : Byte;                      // ������� ������ ����������� ����������, ������� � ��������
  DoubleCnt: Byte;                      // ������� ���������� Double ������� � �������� �� ������
  MySync   : array[1..2] of Boolean;    // ������� ��������� ������������� �� ������ ������

  // ��������� ��� ��������� ������
  DTFrameBegin  : Double;  // ������ ���������
  DTFrameEnd    : Double;  // ����� ���������
  DTFrameOffset : Double;  // ����� � ����� ��������� ���������
  FrameOffset   : Integer; // ��������� �� ����� ��������� ���������

  // ������ ��� ��������������� ������
  arhiv : string;


//------------------------------------------------------------------------------
//                  ������ �������� �� ������ ������ - ���
//------------------------------------------------------------------------------
var
  FR3    : array[1..FR_LIMIT] of Byte;      // ����� ������� ���������
  FR3inp : array[1..FR_LIMIT] of Char;      // ����� ������ FR3
  FR3s   : array[1..FR_LIMIT] of TDateTime; // ����������� ������ FR3

//------------------------------------------------------------------------------
//                 ������ �����������, ���������� �� �������
//------------------------------------------------------------------------------
var
  FR4    : array[1..FR_LIMIT] of Byte;      // ����� ������� ���������
  FR4inp : array[1..FR_LIMIT] of Char;      // ����� ������ FR4
  FR4s   : array[1..FR_LIMIT] of TDateTime; // ����������� ������ FR4

//------------------------------------------------------------------------------
//                         ������ ��������� �����������
//------------------------------------------------------------------------------
var
  FR5    : array[1..FR_LIMIT] of Byte;      // ����� �����������

//------------------------------------------------------------------------------
//                        ������ ����������� ����������
//------------------------------------------------------------------------------
var
  FR6 : array[1..1024] of Word;

//------------------------------------------------------------------------------
//                      ������ �������������� ����������
//------------------------------------------------------------------------------
var
  FR7 : array[1..1024] of Cardinal;

//------------------------------------------------------------------------------
//                      ������ ������������� ����������
//------------------------------------------------------------------------------
var
  FR8 : array[1..1024] of int64;

var
  ArchName : string;   // ��� ����� ������ (��� ����)
  ArcIndex : cardinal; // ������ ������ � ������
  KanalSrv : array[1..2] of TKanal;

  savearc  : boolean; // ���������� ������ ����������� ������ � �����
  chnl1    : string;  // ����� ������ �� 1 ������
  chnl2    : string;  // ����� ������ �� 2 ������

function SyncReady : Boolean;
function GetFR5(param : Word) : Byte;
function GetFR4State(param : Word) : Boolean;
function GetFR3(const param : Word; var nep, ready : Boolean) : Boolean;

implementation

uses
  crccalc,
  commands,
  commons,
  mainloop,
  VarStruct;

var
  OLS : TOverlapped;

//-----------------------------------------------------------------------------
// ��� �������������
function SyncReady : Boolean;
begin
  result := false;
end;






function GetFR5(param : Word) : Byte;
begin
  result := FR5[param];
  FR5[param] := 0; // �������� ��������
end;

function GetFR3(const param : Word; var nep, ready : Boolean) : Boolean;
  var p,d : integer;
begin
  result := false;
  ready := true;
  if param < 8 then exit;
  d := param and 7; // ����� �������������� ����
  p := param shr 3; // ����� �������������� �����
  if p > 4096 then exit;

  // ��������� ������������� ������
  ready := ArcReady;

  // ��������� ������� ������������ ���������
  if not nep then nep := (FR3[p] and $20) = $20;

  // �������� ��������
  case d of
    1 : result := (FR3[p] and 2) = 2;
    2 : result := (FR3[p] and 4) = 4;
    3 : result := (FR3[p] and 8) = 8;
    4 : result := (FR3[p] and $10) = $10;
    5 : result := (FR3[p] and $20) = $20;
    6 : result := (FR3[p] and $40) = $40;
    7 : result := (FR3[p] and $80) = $80;
  else
    result := (FR3[p] and 1) = 1;
  end;
end;

function GetFR4State(param : Word) : Boolean;
  var p,d : integer;
begin
  result := false;
  if param < 8 then exit;
  d := param and 7;
  p := param shr 3;
  if p > 4096 then exit;

  // ��������� ���������� ������� ����� ������
  if (LastTime - FR4s[p]) < MaxTimeOutRecave then
  begin
    case d of
      1 : result := (FR4[p] and 2) = 2;
      2 : result := (FR4[p] and 4) = 4;
      3 : result := (FR4[p] and 8) = 8;
      4 : result := (FR4[p] and $10) = $10;
      5 : result := (FR4[p] and $20) = $20;
      6 : result := (FR4[p] and $40) = $40;
      7 : result := (FR4[p] and $80) = $80;
    else
      result := (FR4[p] and 1) = 1;
    end;
  end else
    result := false;
end;

end.
