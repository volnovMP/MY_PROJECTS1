unit ButtonOK;
//------------------------------------------------------------------------------
//
//                Процедуры обработки кнопки ответственных команд
//
//  версия                    - 1
//  редакция                  - 2
//
//  дата последнего изменения - 26 мая 2006г.
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
  ChOKTime      : Double;   // время изменения состояния кнопки ОК

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
// Обработка КОК по схеме RS-232
function GetKOKStateKvit : byte;
begin
  result := 255;
  GetCommModemStatus(PortOK.PortHandle, lpModemStat);
  if (lpModemStat and (MS_RLSD_ON + MS_DSR_ON)) = MS_RLSD_ON then
  begin // Нажата кнопка ОК
    if not WorkMode.PushOK then begin ChOKTime := LastTime; result := 1; end;
    WorkMode.PushOK := true; WorkMode.OKError := false;
  end else
  if (lpModemStat and (MS_RLSD_ON + MS_DSR_ON)) = MS_DSR_ON then
  begin // Не нажата кнопка ОК
    if WorkMode.PushOK then ChOKTime := LastTime;
    WorkMode.PushOK := false; WorkMode.OKError := false; WorkMode.OtvKom := false;
  end else
  begin
    if not WorkMode.OKError then ChOKTime := LastTime;
    PortOK.DTROnOff(true); // выдать DTR
    WorkMode.OKError := true; WorkMode.OtvKom := false;
    if LastTime > ChOKTime + 5/80000 then
    begin // неисправность кнопки ОК
      ChOKTime := LastTime; result := 0;
    end;
  end;
end;

//------------------------------------------------------------------------------
// Обработка КОК по схеме RS-422
function GetKOKStateTx : byte;
begin
  result := 255; err := false;
  if not PortOK1.StrFromComm(s1) or not PortOK2.StrFromComm(s2) then
  begin // ошибки при приеме из порта
    if not WorkMode.OKError then ChOKTime := LastTime;
    WorkMode.OKError := true; WorkMode.OtvKom := false; WorkMode.PushOK := false;
    if LastTime > ChOKTime + 5/80000 then
    begin // неисправность кнопки ОК
      ChOKTime := LastTime; result := 0; err := true;
    end;
  end else
  begin // прием состоялся
    if (s1 = '') or (s2 = '') then
    begin // порты пустые
    if not WorkMode.OKError then ChOKTime := LastTime;
      WorkMode.OKError := true; WorkMode.OtvKom := false; WorkMode.PushOK := false; BadSxemaKOK := false;
      if LastTime > ChOKTime + 5/80000 then
      begin // неисправность кнопки ОК
        ChOKTime := LastTime; result := 0; err := true;
      end;
    end else
    begin // проверить полученные данные
      if s1 = LastTr2 then
      begin // ошибочное подключения КОК
        WorkMode.OtvKom  := false;
        if not BadSxemaKOK then begin WorkMode.OKError := true; BadSxemaKOK := true; result := 254; end;
      end else
      begin // проверить нажатие КОК
        if s2 = LastTr1 then
        begin // нажата кнопка ОК
          if not WorkMode.PushOK then result := 1;
          WorkMode.OKError := false; WorkMode.PushOK := true; UndefineState := false; BadSxemaKOK := false;
        end else
        if s2 = LastTr2 then
        begin // КОК не нажата
          WorkMode.OKError := false; WorkMode.PushOK := false; WorkMode.OtvKom := false; UndefineState := false; BadSxemaKOK := false;
        end else
        begin // неопределенное состояние КОК
          BadSxemaKOK := false;
          if not UndefineState then begin ChOKTime := LastTime; UndefineState := true; end;
          if LastTime > ChOKTime + 5/80000 then
          begin // неисправность кнопки ОК
            ChOKTime := LastTime; WorkMode.OKError := true; WorkMode.OtvKom  := false; WorkMode.PushOK  := false; result := 0;
          end;
        end;
      end;
    end;
  end;

  if err then
  begin // повторная инициализация портов КОК
    reportf('Выполнена переинициализация портов кнопки ОК.');
    PortOK1.ClosePort;
    PortOK2.ClosePort;
    InitKOK;
    PortOK1.OpenPort;
    PortOK2.OpenPort;
  end;
  // Подготовить данные к следующей записи в порты КОК
  inc(CntPacketKOK);
  LastTr1 := '@'+ IntToHex(config.RMID,2)+ '#'+ IntToHex(CntPacketKOK,4)+ '%AAAAAAAAA@'+ IntToHex(config.RMID,2);
  LastTr2 := '@'+ IntToHex(config.RMID,2)+ '#'+ IntToHex(CntPacketKOK,4)+ '%uuuuuuuuu@'+ IntToHex(config.RMID,2);
  // Записать данные в порты
  PortOK1.StrToComm(LastTr1);
  PortOK2.StrToComm(LastTr2);
end;

end.
