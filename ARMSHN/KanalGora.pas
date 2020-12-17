unit KanalGora;
//****************************************************************************
//
//       Процедуры работы с каналом РМ ДСП - Горка
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
    trmcnt  : integer; // счетчик количества переданых символов
    tpkcnt  : integer; // счетчик количества передаых пакетов
  end;

const
  LnSoobGora  = 70;    // длина сообщения для горки
  LIMIT_GORA = 400;

var
  CurrGoraSoob : integer; // последнее циклически переданное сообщение в канал горки
  KanalGora1 : TKanalGora;
//------------------------------------------------------------------------------
//                  Массив датчиков из канала ЛП-ДЦ - АРМ
//------------------------------------------------------------------------------

function CreateKanalGora : Boolean;                // Создать экземпляры класса TComPort
function DestroyKanalGora : Boolean;               // Деструктор структур канала
function GetKanalGoraStatus: Byte;  // Получить состояние канала
function InitKanalGora  : Byte;       // Инициализация канала
function ConnectKanalGora : Byte;    // Установить связь по каналу
function DisconnectKanalGora : Byte; // Разрыв связи по каналу
function SyncGoraReady : Boolean;
procedure WriteSoobGora;                   // Процедура передачи в канал COM-ports
procedure FixStatKanalGora; // Сохранить в файле протокола статистику работы канала

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
WrBuffer    : array[0..69] of char; // буфер записи
sz          : string;
stime       : string;


//-----------------------------------------------------------------------------
// Создать экземпляры класса TComPort
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
// Деструктор структур канала
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
// Получить состояние канала
function GetKanalGoraStatus : Byte;
begin
  if not Assigned(KanalGora1.port) then result := 255 else result := 0;
end;

//-----------------------------------------------------------------------------
// Инициализация канала
function InitKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // СОМ-порт
    if KanalGora1.port.InitPort(IntToStr(KanalGora1.Index)+ ','+ KanalGora1.config) then
    begin
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('Выполнена инициализация канала Гора '+ stime);
      result := 0;
    end else
    begin
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('Ошибка 254 инициализации канала Гора '+ stime);
      result := 254;
    end;
  end else
    result := 253;
end;

//-----------------------------------------------------------------------------
// Установить связь по каналу
function ConnectKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // СОМ-порт
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
      reportf('Выполнено открытие коммуникационного порта канала Гора '+ stime);
      result := 0;
    end else
    begin
      result := 254;
      DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
      reportf('Не удается открыть коммуникационный порт канала Гора '+ stime);
    end;
  end else
    result := 1;
end;

//-----------------------------------------------------------------------------
// Разрыв связи по каналу
function DisconnectKanalGora : Byte;
begin
  result := 255;
  if not Assigned(KanalGora1.port) then exit;
  if Assigned(KanalGora1.port) then
  begin // COM-порт
    if KanalGora1.port.PortIsOpen then
    begin
      if Kanalgora1.port.ClosePort then
      begin
        Kanalgora1.active := false;
        DateTimeToString(stime, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
        reportf('Коммуникационный порт канала Гора '+ stime+ ' закрыт');
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
  if AppStart then exit; // до окончания инициализации не обрабатывать канал АРМ-Гора

  //
  // Обновить состояние датчиков в буфере канала горки
  //

  //
  // Для обмена с горкой используется канал RS-232.
  // Канал не имеет приоритета, используктся по готовности данных.
  //
  if KanalGora1.active then WriteSoobGora; // выдать в 1-ый канал данные
end;
//-----------------------------------------------------------------------------
// Процедура передачи в канал Гора
procedure WriteSoobGora;
begin
  if Assigned(KanalGora1.port) then
  begin
    KanalGora1.port.BufToComm(@Bufer_Out,LnSoobGora);
    KanalGora1.trmcnt := KanalGora1.trmcnt + LnSoobGora; inc(KanalGora1.tpkcnt);
  end;
end;
//------------------------------------------------------------------------------
// Сохранить в файле протокола статистику работы канала
procedure FixStatKanalGora;
begin
  if Assigned(KanalGora1.port) then
  begin
    reportf('Канал Гора : Байт отправлено '+IntToStr(KanalGora1.trmcnt));
    reportf('Канал Гора : Пакетов отправлено '+IntToStr(KanalGora1.tpkcnt));
  end;
end;

end.
