unit KanalGora;
//****************************************************************************
//
//       ��������� ������ � ������� �� ��� - �����
//
//****************************************************************************
interface

uses
  Windows, Dialogs, SysUtils, Controls, Classes, Messages, Graphics, Forms, StdCtrls, Registry, SyncObjs, comport, KanalArmSrv;

type TKanalGora = record
    State   : Byte;
    config  : string;
    Index   : Byte;
    port    : TComPort;
    active  : Boolean;
    hPipe   : THandle;
    isrcv   : boolean;
    issync  : boolean;
    iserror : boolean;
    cnterr  : integer;
    trmcnt  : integer; // ������� ���������� ��������� ��������
    tpkcnt  : integer; // ������� ���������� �������� �������
  end;

const
  LnSoobGora  = 70;    // ����� ��������� ��� �����
  LIMIT_GORA = 400;

var
  CurrGoraSoob : integer; // ��������� ���������� ���������� ��������� � ����� �����
  KanalGora1 : TKanalGora;
//------------------------------------------------------------------------------
//                  ������ �������� �� ������ ��-�� - ���
//------------------------------------------------------------------------------

function CreateKanalGora : Boolean;                // ������� ���������� ������ TComPort
function DestroyKanalGora : Boolean;               // ���������� �������� ������
function GetKanalGoraStatus: Byte;  // �������� ��������� ������
function InitKanalGora  : Byte;       // ������������� ������
function ConnectKanalGora : Byte;    // ���������� ����� �� ������
function DisconnectKanalGora : Byte; // ������ ����� �� ������
function SyncGoraReady : Boolean;
procedure WriteSoobGora;                   // ��������� �������� � ����� COM-ports
procedure FixStatKanalGora; // ��������� � ����� ��������� ���������� ������ ������

implementation
uses
  TabloForm,
  crccalc,
  commands,
  commons,
  mainloop,
  marshrut,
  VarStruct;

var
WrBuffer    : array[0..69] of char; // ����� ������
sz          : string;
stime       : string;


//-----------------------------------------------------------------------------
// ������� ���������� ������ TComPort
function CreateKanalGora : Boolean;
begin
  try
    if KanalGora1.Index > 0 then KanalGora1.port := TComPort.Create(nil);
    result := true;
  except
    result := false;
  end;
end;

//-----------------------------------------------------------------------------
// ���������� �������� ������
function DestroyKanalGora : Boolean;
begin
  try
    if Assigned(KanalGora1.port) then KanalGora1.port.Destroy;
    result := true;
  except
    result := false;
  end;
end;

//-----------------------------------------------------------------------------
// �������� ��������� ������
function GetKanalGoraStatus : Byte;
begin
  if not Assigned(KanalGora1.port) then result := 255 else result := 0;
end;

//-----------------------------------------------------------------------------
// ������������� ������
function InitKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // ���-����
    if KanalGora1.port.InitPort(IntToStr(KanalGora1.Index)+ ','+ KanalGora1.config) then
    begin
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('��������� ������������� ������ ���� '+ stime);
      result := 0;
    end else
    begin
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('������ 254 ������������� ������ ���� '+ stime);
      result := 254;
    end;
  end else
    result := 253;
end;

//-----------------------------------------------------------------------------
// ���������� ����� �� ������
function ConnectKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // ���-����
    if KanalGora1.port.PortIsOpen then
    begin
      KanalGora1.active := true; result := 0;
    end else
    if KanalGora1.port.OpenPort then
    begin
      PurgeComm(Kanalgora1.port.PortHandle,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);
      KanalGora1.active := true;
      KanalGora1.hPipe := KanalGora1.port.PortHandle;
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('��������� �������� ����������������� ����� ������ ���� '+ stime);
      result := 0;
    end else
    begin
      result := 254;
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('�� ������� ������� ���������������� ���� ������ ���� '+ stime);
    end;
  end else
    result := 1;
end;

//-----------------------------------------------------------------------------
// ������ ����� �� ������
function DisconnectKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // COM-����
    if KanalGora1.port.PortIsOpen then
    begin
      if Kanalgora1.port.ClosePort then
      begin
        Kanalgora1.active := false;
        DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
        reportf('���������������� ���� ������ ���� '+ stime+ ' ������');
        result := 0;
      end else
        result := 253;
    end else
      result := 0;
  end else
    result := 0;
end;
//============================================================
function SyncGoraReady : Boolean;
  var i,j : integer; b,c : boolean; d : byte;
begin
  result := false;
  if AppStart then exit; // �� ��������� ������������� �� ������������ ����� ���-����

  //
  // �������� ��������� �������� � ������ ������ �����
  //

  //
  // ��� ������ � ������ ������������ ����� RS-232.
  // ����� �� ����� ����������, ������������ �� ���������� ������.
  //
  if KanalGora1.active then WriteSoobGora; // ������ � 1-�� ����� ������
end;
//-----------------------------------------------------------------------------
// ��������� �������� � ����� ����
procedure WriteSoobGora;
begin
  if Assigned(KanalGora1.port) then
  begin
    KanalGora1.port.BufToComm(@Bufer_Out,LnSoobGora);
    KanalGora1.trmcnt := KanalGora1.trmcnt + LnSoobGora; inc(KanalGora1.tpkcnt);
  end;
end;
//------------------------------------------------------------------------------
// ��������� � ����� ��������� ���������� ������ ������
procedure FixStatKanalGora;
begin
  if Assigned(KanalGora1.port) then
  begin
    reportf('����� ���� : ���� ���������� '+IntToStr(KanalGora1.trmcnt));
    reportf('����� ���� : ������� ���������� '+IntToStr(KanalGora1.tpkcnt));
  end;
end;

end.
