unit ButtonOK;
//------------------------------------------------------------------------------
//
//                ��������� ��������� ������ ������������� ������
//
//  ������                    - 1
//  ��������                  - 2
//
//  ���� ���������� ��������� - 26 ��� 2006�.
//------------------------------------------------------------------------------

interface

uses
  Windows, comport, SysUtils;

var
  PortOK        : TComport;
  PortOK1       : TComport;
  PortOK2       : TComport;
  ConfigPortOK  : string;
  ConfigPortOK1 : string;
  ConfigPortOK2 : string;
  ChOKTime      : Double;   // ����� ��������� ��������� ������ ��

function CreateKOK : boolean;
function InitKOK : boolean;
procedure FreeKOK;
function GetKOKStateKvit : byte;
function GetKOKStateTx : byte;

implementation

uses
  Commons,
  VarStruct;

var
  LastTr1       : string;
  LastTr2       : string;
  CntPacketKOK  : word;
  UndefineState : Boolean;
  BadSxemaKOK   : Boolean;
  s1,s2         : string;
  err           : boolean;
  lpModemStat   : Cardinal;

function CreateKOK : boolean;
begin
  CreateKOK := false; CntPacketKOK := 0;
  try PortOK  := TComport.Create(nil); except CreateKOK := true; end;
  try PortOK1 := TComport.Create(nil); except CreateKOK := true; end;
  try PortOK2 := TComport.Create(nil); except CreateKOK := true; end;
end;

function InitKOK : boolean;
begin
  InitKOK := false; UndefineState := false;
  if (ConfigPortOK <> '') and Assigned(PortOK) then if not PortOK.InitPort(ConfigPortOK) then InitKOK := true;
  if (ConfigPortOK1 <> '') and Assigned(PortOK1) then if not PortOK1.InitPort(ConfigPortOK1) then InitKOK := true;
  if (ConfigPortOK2 <> '') and Assigned(PortOK2) then if not PortOK2.InitPort(ConfigPortOK2) then InitKOK := true;
end;

procedure FreeKOK;
begin
  if Assigned(PortOK) then PortOK.Free;
  if Assigned(PortOK1) then PortOK1.Free;
  if Assigned(PortOK2) then PortOK2.Free;
end;

//------------------------------------------------------------------------------
// ��������� ��� �� ����� RS-232
function GetKOKStateKvit : byte;
begin
  result := 255;
  GetCommModemStatus(PortOK.PortHandle, lpModemStat);
  if (lpModemStat and (MS_RLSD_ON + MS_DSR_ON)) = MS_RLSD_ON then
  begin // ������ ������ ��
    if not WorkMode.PushOK then begin ChOKTime := LastTime; result := 1; end;
    WorkMode.PushOK := true; WorkMode.OKError := false;
  end else
  if (lpModemStat and (MS_RLSD_ON + MS_DSR_ON)) = MS_DSR_ON then
  begin // �� ������ ������ ��
    if WorkMode.PushOK then ChOKTime := LastTime;
    WorkMode.PushOK := false; WorkMode.OKError := false; WorkMode.OtvKom := false;
  end else
  begin
    if not WorkMode.OKError then ChOKTime := LastTime;
    PortOK.DTROnOff(true); // ������ DTR
    WorkMode.OKError := true; WorkMode.OtvKom := false;
    if LastTime > ChOKTime + 5/80000 then
    begin // ������������� ������ ��
      ChOKTime := LastTime; result := 0;
    end;
  end;
end;

//------------------------------------------------------------------------------
// ��������� ��� �� ����� RS-422
function GetKOKStateTx : byte;
begin
  result := 255; err := false;
  if not PortOK1.StrFromComm(s1) or not PortOK2.StrFromComm(s2) then
  begin // ������ ��� ������ �� �����
    if not WorkMode.OKError then ChOKTime := LastTime;
    WorkMode.OKError := true; WorkMode.OtvKom := false; WorkMode.PushOK := false;
    if LastTime > ChOKTime + 5/80000 then
    begin // ������������� ������ ��
      ChOKTime := LastTime; result := 0; err := true;
    end;
  end else
  begin // ����� ���������
    if (s1 = '') or (s2 = '') then
    begin // ����� ������
    if not WorkMode.OKError then ChOKTime := LastTime;
      WorkMode.OKError := true; WorkMode.OtvKom := false; WorkMode.PushOK := false; BadSxemaKOK := false;
      if LastTime > ChOKTime + 5/80000 then
      begin // ������������� ������ ��
        ChOKTime := LastTime; result := 0; err := true;
      end;
    end else
    begin // ��������� ���������� ������
      if s1 = LastTr2 then
      begin // ��������� ����������� ���
        WorkMode.OtvKom  := false;
        if not BadSxemaKOK then begin WorkMode.OKError := true; BadSxemaKOK := true; result := 254; end;
      end else
      begin // ��������� ������� ���
        if s2 = LastTr1 then
        begin // ������ ������ ��
          if not WorkMode.PushOK then result := 1;
          WorkMode.OKError := false; WorkMode.PushOK := true; UndefineState := false; BadSxemaKOK := false;
        end else
        if s2 = LastTr2 then
        begin // ��� �� ������
          WorkMode.OKError := false; WorkMode.PushOK := false; WorkMode.OtvKom := false; UndefineState := false; BadSxemaKOK := false;
        end else
        begin // �������������� ��������� ���
          BadSxemaKOK := false;
          if not UndefineState then begin ChOKTime := LastTime; UndefineState := true; end;
          if LastTime > ChOKTime + 5/80000 then
          begin // ������������� ������ ��
            ChOKTime := LastTime; WorkMode.OKError := true; WorkMode.OtvKom  := false; WorkMode.PushOK  := false; result := 0;
          end;
        end;
      end;
    end;
  end;

  if err then
  begin // ��������� ������������� ������ ���
    reportf('��������� ����������������� ������ ������ ��.');
    PortOK1.ClosePort;
    PortOK2.ClosePort;
    InitKOK;
    PortOK1.OpenPort;
    PortOK2.OpenPort;
  end;
  // ����������� ������ � ��������� ������ � ����� ���
  inc(CntPacketKOK);
  LastTr1 := '@'+ IntToHex(config.RMID,2)+ '#'+ IntToHex(CntPacketKOK,4)+ '%AAAAAAAAA@'+ IntToHex(config.RMID,2);
  LastTr2 := '@'+ IntToHex(config.RMID,2)+ '#'+ IntToHex(CntPacketKOK,4)+ '%uuuuuuuuu@'+ IntToHex(config.RMID,2);
  // �������� ������ � �����
  PortOK1.StrToComm(LastTr1);
  PortOK2.StrToComm(LastTr2);
end;

end.
