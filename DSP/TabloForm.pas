unit TabloForm;
{$INCLUDE CfgProject}  // параметры компиляции
{$UNDEF SAVEKANAL}     // сохранять данные каналов в файлы

////////////////////////////////////////////////////////////////////////////////
//
//                       Главное окно программы РМ-ДСП
//
// Версия      1
// Редакция    5
// Дата        16 января 2006 года
//
////////////////////////////////////////////////////////////////////////////////

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, ImgList, StdCtrls, ComCtrls, Registry, Menus, MMSystem, comport;

type
  TTabloMain = class(TForm)
    ImageList: TImageList;
    MainTimer: TTimer;
    BeepTimer: TTimer;
    ImageListRU: TImageList;
    ImageList32: TImageList;
    ImageList16: TImageList;
    ImageListIcon: TImageList;
    ASU: TTimer;
    ilGlobus: TImageList;
    procedure FormCreate(Sender: TObject);
    procedure FormPaint(Sender: TObject);
    procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X, Y: Integer);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure FormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure FormDestroy(Sender: TObject);
    procedure DrawTablo(tablo: TBitmap);
    procedure WMEraseBkgnd(var Message: TWMEraseBkgnd); message WM_ERASEBKGND;
    procedure FormActivate(Sender: TObject);
    procedure MainTimerTimer(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure BeepTimerTimer(Sender: TObject);
    procedure DspPopupHandler(Sender: TObject);
    procedure FormKeyUp(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer; var Resize: Boolean);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure ASUTimer(Sender: TObject);
  private
    function RefreshTablo : Boolean; // Обновление образа табло
{$IFNDEF DEBUG}
    procedure KeyOtvKomState; // Проверка состояния кнопки ОК
{$ENDIF}
    procedure SetPlakat(X,Y : integer);
    procedure GetPlakat(X,Y : integer);
    procedure DrawPlakat(X,Y : integer);
  public
    PopupMenuCmd  : TPopupMenu;
    FindCursor    : Boolean; // включить поиск курсора
    FindCursorCnt : Integer; // счетчик для поиска курсора
  end;

var
  TabloMain: TTabloMain;

  RefreshTimeOut : Double;       // максимальное время ожидания синхронизации из канала
  StartTime      : Double;       // время запуска системы
  TimeLockCmdDsp : Double;       // время блокирования двойного щелчка мышкой
  IsCloseRMDSP   : Boolean;      // выполнить закрытие главного окна программы
  AppStart       : Boolean;      // признак старта главного окна программы
  SendToSrvCloseRMDSP : Boolean; // послать уведомление серверу о завершении работы АРМа
  SendRestartServera  : Boolean; // выдать команду на перезагрузку сервера
  OpenMsgForm         : Boolean; // Запрос открытия формы сообщений
  shiftscr    : integer; // сдвиг картинки
  GlobusIndex : integer; //

procedure ChangeDirectState(State : Boolean);
procedure ChangeRegion(RU : Byte);
procedure ResetCommands; // сброс всех активных команд
procedure PresetObjParams;
procedure IncrementKOK;

const
  CurTablo1   = 1;
  CurTablo1ok = 2;

  ReportFileName = 'Dsp.rpt';
  KeyName : string = '\Software\DSPRPCTUMS';

implementation

uses
  TypeRpc,
  VarStruct,
  crccalc,
  Load,
  KanalArmSrv,
  Objsost,
  Commands,
  Marshrut,
  MainLoop,
  CMenu,
  Commons,
  ButtonOK,
  Password,
  PipeProc,
  ASUProc,
  MsgForm,
  TimeInput,
  ViewFr;

{$R *.DFM}

//var
//  MaxLastCRC : integer;

var
  sMsg,sPar : string; // строковые переменные для всех спроцедур формы

procedure TTabloMain.FormDestroy(Sender: TObject);
begin
  // остановить обработку программных каналов
  if DspToDspEnabled then CloseHandle(DspToDspThread);
  if hDspToDspEventWrt <> INVALID_HANDLE_VALUE then CloseHandle(hDspToDspEventWrt);
  if hDspToDspEventRd <> INVALID_HANDLE_VALUE then CloseHandle(hDspToDspEventRd);
  if hDspToDspPipe <> INVALID_HANDLE_VALUE then CloseHandle(hDspToDspPipe);
  if DspToArcEnabled then CloseHandle(DspToArcThread);
  if hDspToArcEventWrt <> INVALID_HANDLE_VALUE then CloseHandle(hDspToArcEventWrt);
  if hDspToArcEventRd <> INVALID_HANDLE_VALUE then CloseHandle(hDspToArcEventRd);
  if hDspToArcPipe <> INVALID_HANDLE_VALUE then CloseHandle(hDspToArcPipe);
  if hWaitKanal <> INVALID_HANDLE_VALUE then CloseHandle(hWaitKanal);

  DateTimeToString(sMsg, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
  reportf('Завершение работы программы '+ sMsg);
  if Assigned(MsgFormDlg) then MsgFormDlg.Free;
  if Assigned(TimeInputDlg) then TimeInputDlg.Free;
  reg.Free;
  DestroyKanalSrv;
  FreeKOK;
  if Assigned(Tablo1) then Tablo1.Free;
  if Assigned(Tablo2) then Tablo2.Free;
  if Assigned(ObjectWav) then ObjectWav.Free;
  if Assigned(IpWav) then IpWav.Free;
end;

procedure TTabloMain.FormCreate(Sender: TObject);
  var err: boolean; i,h : integer;
begin
  hDspToDspEventWrt := INVALID_HANDLE_VALUE;
  hDspToDspEventRd := INVALID_HANDLE_VALUE;
  DspToDspThread := INVALID_HANDLE_VALUE;
  hDspToDspPipe := INVALID_HANDLE_VALUE;
  hDspToArcEventWrt := INVALID_HANDLE_VALUE;
  hDspToArcEventRd := INVALID_HANDLE_VALUE;
  DspToArcThread := INVALID_HANDLE_VALUE;
  hDspToArcPipe := INVALID_HANDLE_VALUE;
  ASU.Interval := ASU_TIMER_INTERVAL;
  GlobusIndex := 0;
  ilGlobus.BkColor := armcolor15;
  Caption := 'Табло';
  FormStyle := fsStayOnTop; // если нет отладки - установить окно поверх остальных
  IsCloseRMDSP := false; SendToSrvCloseRMDSP := false;
  reg := TRegistry.Create; // Объект для доступа к реестру
  reg.RootKey := HKEY_LOCAL_MACHINE;
  // Загрузить предыдущий протокол
  if FileExists(ReportFileName) then
  begin
    h := FileOpen(ReportFileName,fmOpenRead);
    if h > 0 then
    begin
      i := FileSeek(h,0,2);
      if i > 99999 then DeleteFile(ReportFileName);
      FileClose(h);
    end;
  end;
  DateTimeToString(sMsg, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
  reportf('@');
  reportf('Начало работы программы '+ sMsg);

  PopupMenuCmd := TPopupMenu.Create(self);
  PopupMenuCmd.AutoPopup := false;
  shiftscr := 0;

  err := false;
  if Reg.OpenKey(KeyName, false) then
  begin
    if reg.ValueExists('databasepath') then database    := reg.ReadString('databasepath') else begin err := true; reportf('Нет ключа "databasepath"'); end;
    if reg.ValueExists('path') then config.path := reg.ReadString('path') else begin err := true; reportf('Нет ключа "path"'); end;
    if reg.ValueExists('arcpath') then config.arcpath := reg.ReadString('arcpath') else begin err := true; reportf('Нет ключа "arcpath"'); end;
    if reg.ValueExists('ru') then config.ru := reg.ReadInteger('ru') else begin err := true; reportf('Нет ключа "ru"'); end;
    if (config.ru < 1) or (config.ru > 3) then config.ru := 1; DirState[1] := config.ru;
    config.def_ru := config.ru; // район по предустановке
    if reg.ValueExists('auto') then config.auto := reg.ReadBool('auto') else begin err := true; reportf('Нет ключа "auto"'); end;
    if reg.ValueExists('RMID') then config.RMID := reg.ReadInteger('RMID') else begin err := true; reportf('Нет ключа "RMID"'); end;
    if reg.ValueExists('Master') then config.Master := true else
    if reg.ValueExists('Slave') then config.Slave := true;
    if reg.ValueExists('KOK') then KOKCounter  := reg.ReadInteger('KOK' ) else begin err := true; reportf('Нет ключа "KOK"'); end;
    if reg.ValueExists('configkanal') then sMsg := reg.ReadString('configkanal') else begin err := true; reportf('Нет ключа "configkanal"'); end;
    KanalSrv[1].config := sMsg; KanalSrv[2].config := sMsg;
    if reg.ValueExists('namepipein')  then KanalSrv[1].nPipe := reg.ReadString('namepipein') else sMsg := '';
    if reg.ValueExists('namepipeout') then KanalSrv[2].nPipe := reg.ReadString('namepipeout') else sMsg := '';
    if (KanalSrv[1].nPipe = '') and (KanalSrv[2].nPipe = '') then KanalType := 0 else
    if (KanalSrv[1].nPipe <> '') and (KanalSrv[2].nPipe <> '') then KanalType := 1 else
    begin err := true; reportf('Неверно определен тип канала связи с сервером'); end;
    if reg.ValueExists('configok') then ConfigPortOK := reg.ReadString('configok') else begin err := true; reportf('Нет ключа "configok"'); end;
    if reg.ValueExists('configok1') then ConfigPortOK1 := reg.ReadString('configok1') else begin err := true; reportf('Нет ключа "configok1"'); end;
    if reg.ValueExists('configok2') then ConfigPortOK2 := reg.ReadString('configok2') else begin err := true; reportf('Нет ключа "configok2"'); end;
    if reg.ValueExists('AnsverTimeOut') then AnsverTimeOut := reg.ReadDateTime('AnsverTimeOut') else begin err := true; reportf('Нет ключа "AnsverTimeOut"'); end;
    if reg.ValueExists('RefreshTimeOut') then RefreshTimeOut := reg.ReadDateTime('RefreshTimeOut') else begin err := true; reportf('Нет ключа "RefreshTimeOut"'); end;
    if reg.ValueExists('TimeOutRdy') then MaxTimeOutRecave := reg.ReadDateTime('TimeOutRdy') else begin err := true; reportf('Нет ключа "TimeOutRdy"'); end;
    if reg.ValueExists('IntervalAutoMarsh') then IntervalAutoMarsh := reg.ReadInteger('IntervalAutoMarsh') else IntervalAutoMarsh := 15;
    if reg.ValueExists('DiagnozUVK') then DiagnozON := reg.ReadBool('DiagnozUVK') else DiagnozON := false;
    if reg.ValueExists('configKRU') then config.configKRU := reg.ReadInteger('configKRU') else config.configKRU := 0;
    if reg.ValueExists('ServerSync') then WorkMode.ServerSync := true else WorkMode.ServerSync := false;
    if reg.ValueExists('SetIkonRez') then SetIkonRezNonOK := true;
    if reg.ValueExists('ASUpipe1')  then sMsg := reg.ReadString('ASUpipe1') else sMsg := '';
    if sMsg = '' then
    begin // программный канал АСУ1 не определен
      DspToDspEnabled := false; nDspToDspPipe := '';
    end else
    begin // программный канал АСУ1 определен (ДСП(ведущий)-ДСП(ведомый))
      if sMsg[1] = '0' then DspToDspType := 0 else
      if sMsg[1] = '1' then DspToDspType := 1 else
      begin err := true; reportf('неверный параметр конфигурации канала АСУ1'); Beep; end;
      if not err then DspToDspEnabled := true;
      nDspToDspPipe := ''; for i := 3 to Length(sMsg) do nDspToDspPipe := nDspToDspPipe + sMsg[i];
    end;

    if reg.ValueExists('ARCpipe')  then sMsg := reg.ReadString('ARCpipe') else sMsg := '';
    if sMsg = '' then
    begin // программный канал хранилища архива не определен
      DspToArcEnabled := false; nDspToArcPipe := '';
    end else
    begin // программный канал хранилища архива
      DspToArcEnabled := true; nDspToArcPipe := ''; for i := 1 to Length(sMsg) do nDspToArcPipe := nDspToArcPipe + sMsg[i];
    end;

// Технологическая функция
    if reg.ValueExists('SaveArc') then savearc := true;

    DesktopSize.X := Screen.DesktopWidth; // получить размер рабочего стола Windows
    DesktopSize.Y := Screen.DesktopHeight;

    if reg.ValueExists('kanal1') then
    begin
      i := reg.ReadInteger('kanal1'); KanalSrv[1].Index := i;
    end else
    begin KanalSrv[1].Index := 0; err := true; reportf('Нет ключа "kanal1"'); end;
    if reg.ValueExists('kanal2') then
    begin
      i := reg.ReadInteger('kanal2'); KanalSrv[2].Index := i;
    end else
    begin KanalSrv[2].Index := 0; err := true; reportf('Нет ключа "kanal2"'); end;

    if reg.ValueExists('cur_id') then // Разрешить проверку ключа тестового режима
      config.cur_id := reg.ReadInteger('cur_id')
    else // Запретить проверку ключа тестового режима
      config.cur_id := 0;
    reg.CloseKey;

    if not FileExists(database) then begin err := true; reportf('Файл конфигурации базы данных станции не найден.'); end;
  end else
  begin
    reportf('Нет ключа "DSPRPCTUMS"');
    ShowMessage('Завершение работы из-за обнаружения ошибки при инициализации программы. (Создан файл ошибок DSP.RPT)');
    Application.Terminate; exit;
  end;
  Left := 0; Top := 0;
  mem_page := false;

  if CreateKOK then begin err := true; reportf('Ошибка порта кнопки ОК.'); Beep; end;
  if InitKOK then begin err := true; reportf('Ошибка инициализации порта кнопки ОК.'); Beep; end;

  if not InitpSD then begin err := true; reportf('Ошибка инициализации структуры безопасности'); Beep; end;
  if not InitEventPipes then begin err := true; reportf('Ошибка инициализации системных ресурсов'); Beep; end;

  CreateKanalSrv;
  InitKanalSrv(1);
  InitKanalSrv(2);

  Tablo1 := TBitmap.Create;
  Tablo2 := TBitmap.Create;
  ImageList.BkColor   := bkgndcolor;
  ImageListRU.BkColor := bkgndcolor;

  screen.Cursors[curTablo1]   := LoadCursor(HInstance, IDC_ARROW);
  screen.Cursors[curTablo1ok] := LoadCursor(HInstance, 'CURSOR1OK');

  // Сбросить все пункты меню
  DspMenu.Ready := false; DspMenu.WC := false; DspMenu.obj := -1;

  // сбросить все сообщения с подтверждением
  FixMessage.Count := 0; FixMessage.MarkerLine := 1; FixMessage.StartLine  := 1;

  // Подготовка структура вспомогательного перевода стрелок
  VspPerevod.Active := false;

  SyncTime := false;
  OperatorDirect := 0;
  StartObj := 1;
  cntObjZav := 1; cntObjView := 1; cntObjUprav := 1; cntOVBuffer := 1;
  ObjHintIndex := 0;
  LockHint := false;
  StartRM := true; // выполняются процедуры старта системы

  ObjectWav := TStringList.Create;
  ObjectWav.Add(config.path+'media\sound1.wav');
  ObjectWav.Add(config.path+'media\sound2.wav');
  ObjectWav.Add(config.path+'media\sound3.wav');
  ObjectWav.Add(config.path+'media\sound4.wav');
  ObjectWav.Add(config.path+'media\sound5.wav');
  ObjectWav.Add(config.path+'media\sound6.wav');
  IpWav := TStringList.Create;
  IpWav.Add(config.path+'media\ip1.wav');
  IpWav.Add(config.path+'media\ip2.wav');

  // Загрузка базы данных
  if not LoadBase(database) then err := true;
  // если количества мониторов не достаточно для включения РМ-ДСП - завершить по ошибке
{$IFNDEF DEBUG}
  if (DesktopSize.X < configru[config.ru].TabloSize.X) or (DesktopSize.Y < configru[config.ru].TabloSize.Y) then
  begin
    reportf('Размер табло ['+ IntToStr(configru[config.ru].TabloSize.X)+ 'x'+ IntToStr(configru[config.ru].TabloSize.Y) + '] превышает размер рабочего стола Windows!');
    err := true;
  end;
{$ENDIF}
  SetParamTablo;
  // загрузка коротких сообщений РМ-ДСП
  if not LoadLex(config.path + 'LEX.SDB') then err := true;
  if not LoadLex2(config.path + 'LEX2.SDB') then err := true;
  if not LoadLex3(config.path + 'LEX3.SDB') then err := true;
  if not LoadMsg(config.path + 'MSG.SDB') then err := true;
  // Загрузка структуры АКНР - выполняется после инициализации объектов зависимостей
  if not LoadAKNR(config.path + 'AKNR.SDB') then err := true;

  // Установить начальные данные режима работы АРМа
  ResetTrace;

  StateRU  := 0; // сбросить состояник УВК в исходное
  ArmState := 0;
  WorkMode.RazdUpr := false;
  WorkMode.MarhUpr := true;
  WorkMode.MarhOtm := false;
  WorkMode.VspStr  := false;
  WorkMode.InpOgr  := false;
  WorkMode.OtvKom  := false;
  WorkMode.Podsvet := false;
  WorkMode.GoTracert  := false;
  WorkMode.GoOtvKom   := false;
  WorkMode.GoMaketSt  := false;
  WorkMode.Upravlenie := false;
  WorkMode.LockCmd    := true;

  MsgFormDlg   := TMsgFormDlg.Create(nil);
  TimeInputDlg := TTimeInputDlg.Create(nil);
  PasswordDlg  := TPasswordDlg.Create(nil);
  // Установить позицию вывода запросов
  PasswordPos.X := configRU[config.ru].MsgLeft+1; PasswordPos.Y := configRU[config.ru].MsgTop+1;
  TimeInputPos.X := configRU[config.ru].MsgLeft+1; TimeInputPos.Y := configRU[config.ru].MsgTop+1;

  hWaitKanal := CreateEvent(nil,false,false,nil);
  IsBreakKanalASU := false;

  if err then
  begin
    ShowMessage('Завершение работы из-за обнаружения ошибки при инициализации программы. (Создан файл ошибок DSP.RPT)');
    Application.Terminate;
  end;
  AppStart := true;
end;

//------------------------------------------------------------------------------
// Активизация табло
procedure TTabloMain.FormActivate(Sender: TObject);
begin
  if not AppStart then exit;
  InsNewArmCmd($7ffb,0);
{$IFNDEF DEBUG}
  ShowWindow(Application.Handle,SW_HIDE);
{$ENDIF}
  AppStart := false;
  // первичная инициализация главного окна
  PresetObjParams;     // Установить параметры объектов зависимостей в исходное состояние
  Cursor := curTablo1;
  FindCursor := false; FindCursorCnt := 0;
  DrawTablo(Tablo1);
  DrawTablo(Tablo2);

  if config.cur_id = 1 then
  begin
  // Проверить разрешение на тестовую проверку
    if FileExists('a:0000000-1a47jdv-kbmndws.ini') then
    begin // Получено подтверждение режима тестовой проверки
      asTestMode := $aa;
    end else asTestMode := $55;
  end else asTestMode := $55;

  if (ConfigPortOK <> '') and (PortOK <> nil) then
  begin
    if not PortOK.OpenPort then
    begin
      reportf('Ошибка инициализации порта кнопки ОК.');
    end else // выдать DTR на кнопку ОК
    begin PortOK.DTROnOff(true); end;
  end;

  if (ConfigPortOK1 <> '') and (PortOK1 <> nil) then
  begin
    if not PortOK1.OpenPort then
    begin
      reportf('Ошибка инициализации первичного порта кнопки ОК.');
    end else
    begin PortOK1.StrToComm('тест первичного порта КОК'); end;
  end;

  if (ConfigPortOK2 <> '') and (PortOK2 <> nil) then
  begin
    if not PortOK2.OpenPort then
    begin
      reportf('Ошибка инициализации вторичного порта кнопки ОК.');
    end else
    begin PortOK2.StrToComm('тест вторичного порта КОК'); end;
  end;

  if DspToDspEnabled then
  begin // старт процесса обработки программного канала АСУ1
    case DspToDspType of
      1 : DspToDspThread := CreateThread(nil,0,@DspToDspClientProc,DspToDspParam,0,DspToDspThreadID); // клиент АСУ1
    else
      DspToDspThread := CreateThread(nil,0,@DspToDspServerProc,DspToDspParam,0,DspToDspThreadID);     // сервер АСУ1
    end;
    if DspToDspThread = INVALID_HANDLE_VALUE then
      reportf('Ошибка создания процесса обработки канала АСУ1.')
    else
      reportf('Начало обработки канала АСУ1.');
  end;


  if DspToArcEnabled then
  begin // старт процесса обработки программного канала хранилища архива
    DspToArcThread := CreateThread(nil,0,@DspToARCProc,DspToArcParam,0,DspToArcThreadID); // клиент АСУ1
    if DspToArcThread = INVALID_HANDLE_VALUE then
      reportf('Ошибка создания процесса обработки канала хранилища удаленного архива.')
    else
      reportf('Начало обработки канала хранилища удаленного архива.');
  end;

  if config.Master then ASU.Interval := 799 else // Установить периодичность обработки событий канала АСУ в зависимости от приоритета РМ
  if config.Slave then ASU.Interval := 1099;

  MainTimer.Enabled := true;
  LastRcv := Date+Time;
  if config.auto then SendCommandToSrv(WorkMode.DirectStateSoob,cmdfr3_directdef,0); // автозапрос захвата управления
  CmdSendT := Date + Time;   // начать отсчет времени жизни каманды автозахвата управления
  StartTime := CmdSendT;     // момент запуска РМ-ДСП
  LastReper := Date + Time;  // начать отсчет 10-ти минутных архивов состояний
  SendRestartServera := false;
  LockTablo := false;
  LockCommandDsp := false;

  ConnectKanalSrv(1);
  ConnectKanalSrv(2);
end;

procedure TTabloMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
  var ec : cardinal;
begin
  if WorkMode.Upravlenie and ((StateRU and $40) = $40) then
  begin
    if (LastRcv + MaxTimeOutRecave) > LastTime then
    begin
      CanClose := false; // при включенном управлении с арма и включенном сервере нельзя завершать работу программы
      exit;
    end;
  end;
  if IsCloseRMDSP then
  begin // получено разрешение завершения работы РМ-ДСП
    DisconnectKanalSrv(1);
    DisconnectKanalSrv(2);
    MainTimer.Enabled := false;
    BeepTimer.Enabled := false;
    if DspToDspEnabled then
    begin
      if GetExitCodeThread(DspToDspThread,ec) then
      begin
        if ec = STILL_ACTIVE then canClose := false // запрет завершения до окончания работы потока ДСП-ДСП
        else canClose := true;
      end else canClose := false;
    end else canClose := true;
    if canClose then
    begin
      if DspToArcEnabled then
      begin
        if GetExitCodeThread(DspToArcThread,ec) then
        begin
          if ec = STILL_ACTIVE then canClose := false; // запрет завершения до окончания работы потока ДСП-АРХИВ
        end else canClose := false;
      end;
    end;

    exit;
  end else
  if not SendToSrvCloseRMDSP then
  begin
    ShowWindow(Application.Handle,SW_SHOW);
    if PasswordDlg.ShowModal = mrOk then
    begin
      PutShortMsg(2,LastX,LastY,'Ожидается завершение работы РМ-ДСП');
      SendCommandToSrv(WorkMode.DirectStateSoob,cmdfr3_logoff,0);
      SendToSrvCloseRMDSP := true;
      KanalSrv[1].State := PIPE_EXIT; // Завершение приема из программного канала
      if DspToDspEnabled and (DspToDspThread <> INVALID_HANDLE_VALUE) then begin DspToDspBreak := true; ResumeThread(DspToDspThread); end;
      if DspToArcEnabled and (DspToArcThread <> INVALID_HANDLE_VALUE) then begin DspToArcBreak := true; ResumeThread(DspToArcThread); end;

      IsBreakKanalASU := true;
      InsNewArmCmd($7ffa,0);
    end;
    ShowWindow(Application.Handle,SW_HIDE);
  end;
  CanClose := false;
end;

procedure TTabloMain.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
  if not mem_page then Canvas.Draw(-shiftscr,0,tablo2) else Canvas.Draw(-shiftscr,0,tablo1);
end;

//------------------------------------------------------------------------------
// прорисовка на экране элементов управления
procedure TTabloMain.FormPaint(Sender: TObject);
  var i,x,y : integer; p : TPoint; n : boolean;
begin
  n := false;
  for i := 1 to 20 do if stellaj[i] then begin n := true; break; end;
  if IkonkaMove or n then begin GetCursorPos(p); canvas.DrawFocusRect(rect(p.X+IkonkaDeltaX,p.Y+IkonkaDeltaY,p.X+12++IkonkaDeltaX,p.Y+12++IkonkaDeltaY)); end;

  // Прорисовка фокуса на объектах
  if (cur_obj > 0) and (cur_obj < 20000) and (ObjUprav[cur_obj].MenuID > 0) then
    with canvas do
    begin
      Pen.Color := clRed; Pen.Mode := pmCopy; Pen.Width := 1; MoveTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Top);
      LineTo(ObjUprav[cur_obj].Box.Right-shiftscr, ObjUprav[cur_obj].Box.Top); LineTo(ObjUprav[cur_obj].Box.Right-shiftscr, ObjUprav[cur_obj].Box.Bottom);
      LineTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Bottom); LineTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Top);
    end;

  if not LockHint then
  begin
    // вывод описания объекта
    if ((LastTime - LastMove) > (1/86400)) and (ObjUprav[cur_obj].Hint <> '') and (ObjHintIndex = 0) then
    begin
      ObjHintIndex := cur_obj; i := LastX div configru[config.ru].MonSize.X + 1; shortmsg[i] := ObjUprav[cur_obj].Hint; shortmsgcolor[i] := bkgndcolor;
    end else
    if (ObjHintIndex > 0) and ((LastTime - LastMove) > (30/86400)) then
    begin
      ResetShortMsg; ObjHintIndex := 0;
    end;
  end;

  x := configRU[config.ru].MonSize.X; if configRU[config.ru].TabloSize.X < x then x := configRU[config.ru].TabloSize.X;
  y := configRU[config.ru].TabloSize.Y;
  for i := 1 to High(shortmsg) do
  begin
    canvas.Brush.Style := bsSolid; canvas.Font.Style := [];
    if shortmsg[i] <> '' then
    begin
    // Вывести короткие сообщения
      canvas.Brush.Color := shortmsgcolor[i]; canvas.FillRect(rect((i-1)*X, Y-15, i*X-32, Y));
      canvas.Font.Color  := clBlack; canvas.Font.Size := 10; canvas.TextOut((i-1)*X+3, Y-15,shortmsg[i]);
      canvas.Brush.Color := clWhite; canvas.FillRect(rect(i*X-32, Y-15, i*X-16, Y));
      canvas.Brush.Color := clRed; canvas.FillRect(rect(i*X-31, Y-14, i*X-27, Y-1));
      canvas.Brush.Color := clGreen; canvas.FillRect(rect(i*X-26, Y-14, i*X-22, Y-1));
      canvas.Brush.Color := clBlue; canvas.FillRect(rect(i*X-21, Y-14, i*X-17, Y-1));
      if ObjHintIndex > 0 then
      begin
        ImageList16.Draw(canvas,i*X-16, Y-15,1);
      end else
      if ShowWarning then
      begin
        if mem_page then ImageList16.Draw(canvas,i*X-16, Y-15,5) else ImageList16.Draw(canvas,i*X-16, Y-15,0);
      end else
      begin
        ilGlobus.Draw(canvas,i*X-16, Y-15,GlobusIndex div 8);
      end;
    end else
    begin
      canvas.Brush.Color := bkgndcolor; canvas.FillRect(rect((i-1)*X, Y-15, i*X-32, Y));
      canvas.Brush.Color := clWhite; canvas.FillRect(rect(i*X-32, Y-15, i*X-16, Y));
      canvas.Brush.Color := clRed; canvas.FillRect(rect(i*X-31, Y-14, i*X-27, Y-1));
      canvas.Brush.Color := clGreen; canvas.FillRect(rect(i*X-26, Y-14, i*X-22, Y-1));
      canvas.Brush.Color := clBlue; canvas.FillRect(rect(i*X-21, Y-14, i*X-17, Y-1));
      ilGlobus.Draw(canvas,i*X-16, Y-15,GlobusIndex div 8);
    end;
  end;

  // поиск курсора на табло
  if FindCursor then
  begin
    GetCursorPos(p);
    if FindCursorCnt >= 100 then
    begin // продолжить бег кругов
      canvas.Pen.Color := clWhite; canvas.Pen.Width := 3; canvas.Pen.Mode := pmNotMask;
      canvas.Pen.Style := psSolid; canvas.Brush.Style := bsClear;
      canvas.Ellipse(p.X-FindCursorCnt,p.Y-FindCursorCnt,p.X+FindCursorCnt,p.Y+FindCursorCnt);
      FindCursorCnt := FindCursorCnt - 100;
      if FindCursorCnt > 0 then
      begin
        canvas.Ellipse(p.X-FindCursorCnt,p.Y-FindCursorCnt,p.X+FindCursorCnt,p.Y+FindCursorCnt); FindCursorCnt := FindCursorCnt - 100;
        if FindCursorCnt > 0 then
        begin
          canvas.Ellipse(p.X-FindCursorCnt,p.Y-FindCursorCnt,p.X+FindCursorCnt,p.Y+FindCursorCnt); FindCursorCnt := FindCursorCnt - 100;
          if FindCursorCnt > 0 then
          begin
            canvas.Ellipse(p.X-FindCursorCnt,p.Y-FindCursorCnt,p.X+FindCursorCnt,p.Y+FindCursorCnt);
            if FindCursorCnt < 100 then
            begin FindCursor := false; FindCursorCnt := 0; end;
          end;
        end else begin FindCursor := false; FindCursorCnt := 0; end;
      end else begin FindCursor := false; FindCursorCnt := 0; end;
    end else
    begin // завершить бег кругов
      FindCursor := false; FindCursorCnt := 0;
    end;
  end;
end;

procedure TTabloMain.DrawTablo(tablo: TBitmap);
  var i,x,y,c : integer;
begin
  Tablo.Canvas.Lock;
  Tablo.Canvas.Brush.Color := bkgndcolor;
  Tablo.canvas.FillRect(rect(0, 0, tablo.width, tablo.height));

  if FixMessage.Count > 0 then
  begin
  // Нарисовать фиксированные сообщения
    x := configRU[config.ru].MsgLeft; y := configRU[config.ru].MsgTop;
    c := FixMessage.Count - FixMessage.StartLine;
    Tablo.Canvas.Font.Size := 8;
    if c > 4 then c := 4;
    for i := FixMessage.StartLine to FixMessage.StartLine + c do
    begin
      Tablo.Canvas.Font.Color := FixMessage.Color[i];
      Tablo.Canvas.Brush.Style := bsSolid;
      if i = FixMessage.MarkerLine then
        Tablo.Canvas.Brush.Color := focuscolor
      else
        Tablo.Canvas.Brush.Color := bkgndcolor;
      Tablo.Canvas.FillRect(rect(x,y,configRU[config.ru].MsgRight,y+16));
      Tablo.Canvas.TextOut(x+2,y,FixMessage.Msg[i]);
      y := y + 16;
    end;
    if FixMessage.Count > 5 then
    begin // отобразить полосу прокрутки фиксированных сообщений
      Tablo.Canvas.Brush.Color := bkgndcolor; Tablo.Canvas.Brush.Style := bsSolid;
      Tablo.Canvas.Rectangle(configRU[config.ru].MsgRight-10,configRU[config.ru].MsgTop,configRU[config.ru].MsgRight,configRU[config.ru].MsgBottom);
      Tablo.Canvas.Brush.Color := Tablo.Canvas.Pen.Color;
      if FixMessage.StartLine > 1 then
      begin
        Tablo.Canvas.Polygon([Point(configRU[config.ru].MsgRight-9,configRU[config.ru].MsgTop+10),
                              Point(configRU[config.ru].MsgRight-1,configRU[config.ru].MsgTop+10),
                              Point(configRU[config.ru].MsgRight-5,configRU[config.ru].MsgTop+1)]);
      end;
      if (FixMessage.Count - FixMessage.StartLine) > 4 then
      begin
        Tablo.Canvas.Polygon([Point(configRU[config.ru].MsgRight-9,configRU[config.ru].MsgBottom-10),
                              Point(configRU[config.ru].MsgRight-1,configRU[config.ru].MsgBottom-10),
                              Point(configRU[config.ru].MsgRight-5,configRU[config.ru].MsgBottom-1)]);
      end;
    end;
  end;

  // прорисовка полки со значками
  Tablo.Canvas.Pen.Color := armcolor8; Tablo.Canvas.Brush.Color := armcolor18; Tablo.Canvas.Pen.Style := psSolid; Tablo.Canvas.Pen.Width := 2;
  Tablo.Canvas.Rectangle(configRU[config.ru].BoxLeft,configRU[config.ru].BoxTop,configRU[config.ru].BoxLeft+12*20+7,configRU[config.ru].BoxTop+16);
  for i := 0 to 19 do
  begin
    x := 1 + i * 12; if i > 10 then x := x + 3;
    case i of
        1 : y := 14;  2 : y := 15;  3 : y := 16;  4 : y := 17;  5 : y := 18;
        6 : y := 19;  7 : y := 20;  8 : y := 21;  9 : y := 22; 10 : y := 23;
       11 : y := 4;  12 : y := 5;  13 : y := 6;  14 : y := 7;  15 : y := 8;
       16 : y := 9;  17 : y := 10; 18 : y := 11; 19 : y := 12;
    else
      y := 13;
    end;
    ImageList.Draw(Tablo.Canvas,configRU[config.ru].BoxLeft+x, configRU[config.ru].BoxTop+1,y, Stellaj[i+1]);
  end;

  // прорисовка иконок в поле
  case config.ru of
    1 : begin // RU1
      for i := 1 to High(Ikonki) do
      begin
        if (Ikonki[i,1] > 0) then ImageList.Draw(Tablo.Canvas,Ikonki[i,2],Ikonki[i,3],Ikonki[i,1],true);
      end;
    end;
    2 : begin // RU2
      for i := 1 to High(Ikonki2) do
      begin
        if (Ikonki2[i,1] > 0) then ImageList.Draw(Tablo.Canvas,Ikonki2[i,2],Ikonki2[i,3],Ikonki2[i,1],true);
      end;
    end;
  end;

  // Из за ошибки в драйвере видеоадаптера WinXP приходится проделывать следующие действия:
  Tablo.Canvas.Brush.Style := bsClear; Tablo.Canvas.Font.Color := clRed; Tablo.Canvas.Font.Color := clBlack;
  // конец программы, устраняющей ошибку WinXP

  // прорисовка всех отображающих объектов табло
  c := 0;
  for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
  begin
    if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 0) then DisplayItemTablo(@ObjView[i], Tablo.Canvas);
    inc(c); if c > 300 then begin SyncReady; WaitForSingleObject(hWaitKanal,3); c := 0; end;
  end;

  for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
  begin
    if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 1) then DisplayItemTablo(@ObjView[i], Tablo.Canvas);
    inc(c);if c > 300 then begin SyncReady; WaitForSingleObject(hWaitKanal,3); c := 0; end;
  end;

  for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
  begin
    if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 2) then DisplayItemTablo(@ObjView[i], Tablo.Canvas);
    inc(c);if c > 300 then begin SyncReady; WaitForSingleObject(hWaitKanal,3); c := 0; end;
  end;

  // Прорисовка границ экранов
  with Tablo.Canvas do
  begin
    Pen.Color := clDkGray; Pen.Width := 1; Brush.Style := bsClear; Pen.Style := psSolid;
    Rectangle(configRU[config.ru].MsgLeft, configRU[config.ru].MsgTop, configRU[config.ru].MsgRight, configRU[config.ru].MsgBottom);
    Pen.Color := armcolor12; Pen.Width := 2;
    for i := 1 to (configRU[config.ru].TabloSize.X div configRU[config.ru].MonSize.X)-1 do
    begin
      MoveTo(i*configRU[config.ru].MonSize.X,0); LineTo(i*configRU[config.ru].MonSize.X,configRU[config.ru].MonSize.Y);
    end;
    Pen.Color := clDkGray; Pen.Width := 1; MoveTo(0,Tablo.Height-1); LineTo(Tablo.Width,Tablo.Height-1);
  end;

// вывод количества не переданых квитанций
{
canvas.TextOut(0,0,IntToStr(LastCRC[1]));
if MaxLastCRC < LastCRC[1] then MaxLastCRC := LastCRC[1];
canvas.TextOut(0,20,IntToStr(LastCRC[2]));
}

  Tablo.Canvas.UnLock;
end;

procedure TTabloMain.FormMouseMove(Sender: TObject; Shift: TShiftState; X, Y: Integer);
  var i,o: integer;
begin
  OperatorDirect := LastTime + 1/86400;

  if PopupMenuCmd.PopupComponent <> nil then exit; // прервать есль кувсор находится в меню

  if IkonkaDown > 0 then
    if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then
    begin
      for i := 1 to High(Stellaj) do stellaj[i] := false; IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0;
    end else
      IkonkaMoved := true;


  if ObjHintIndex > 0 then begin ResetShortMsg; ObjHintIndex := 0; end;
  LastMove := Date+Time; LastX := x; LastY := y;
  // Номер объекта зависимостей
  o := cur_obj; cur_obj := -1;
  for i := configRU[config.ru].OUmin to configRU[config.ru].OUmax do
  begin
    if (x+shiftscr >= ObjUprav[i].Box.Left) and (y >= ObjUprav[i].Box.Top) and (x+shiftscr <= ObjUprav[i].Box.Right) and (y <= ObjUprav[i].Box.Bottom) then
    begin
      cur_obj := i; ID_obj := ObjUprav[i].IndexObj; ID_menu := ObjUprav[i].MenuID; break;
    end;
  end;

  if o <> cur_obj then
  begin
    canvas.Pen.Width := 1;
    if (o > 0) and (o < 20000) then
    with canvas do
    begin
      Pen.Color := bkgndcolor; Pen.Mode := pmCopy; MoveTo(ObjUprav[o].Box.Left-shiftscr, ObjUprav[o].Box.Top);
      LineTo(ObjUprav[o].Box.Right-shiftscr, ObjUprav[o].Box.Top); LineTo(ObjUprav[o].Box.Right-shiftscr, ObjUprav[o].Box.Bottom);
      LineTo(ObjUprav[o].Box.Left-shiftscr, ObjUprav[o].Box.Bottom); LineTo(ObjUprav[o].Box.Left-shiftscr, ObjUprav[o].Box.Top);
    end;
    if (cur_obj > 0) and (ObjUprav[cur_obj].MenuID > 0) then
    with canvas do
    begin
      Pen.Color := clRed; Pen.Mode := pmCopy; MoveTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Top);
      LineTo(ObjUprav[cur_obj].Box.Right-shiftscr, ObjUprav[cur_obj].Box.Top); LineTo(ObjUprav[cur_obj].Box.Right-shiftscr, ObjUprav[cur_obj].Box.Bottom);
      LineTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Bottom); LineTo(ObjUprav[cur_obj].Box.Left-shiftscr, ObjUprav[cur_obj].Box.Top);
    end else
    begin
      ID_obj := -1; ID_menu := -1;
    end;
  end;
end;

//------------------------------------------------------------------------------
// Обработка события выбора пункта в меню
procedure TTabloMain.DspPopupHandler(Sender: TObject);
  var i : integer;
begin
  with Sender as TMenuItem do begin
    for i := 1 to Length(DspMenu.Items) do
      if DspMenu.Items[i].ID = Command then
      begin
        DspCommand.Command := DspMenu.Items[i].Command; DspCommand.Obj := DspMenu.Items[i].Obj; DspCommand.Active  := true; SelectCommand;
        if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-2, ObjUPrav[cur_obj].Box.Bottom-2);
        exit;
      end;
  end;
end;

procedure ResetCommands;
begin
  DspMenu.Ready := false; DspMenu.WC := false;
  DspCommand.Active := false; DspMenu.obj := -1;
  ResetTrace; // Сбросить набираемый маршрут
  WorkMode.GoTracert := false; WorkMode.GoMaketSt := false;
  WorkMode.GoOtvKom := false; Workmode.MarhOtm := false;
  Workmode.VspStr := false; Workmode.InpOgr := false;
  if OtvCommand.Active then
  begin
    InsNewArmCmd(0,0); OtvCommand.Active := false; showShortMsg(156,LastX,LastY,'');
  end else
  if VspPerevod.Active then
  begin
    InsNewArmCmd(0,0);
    VspPerevod.Cmd := 0; VspPerevod.Strelka := 0; VspPerevod.Reper := 0;
    VspPerevod.Active := false; showShortMsg(149,LastX,LastY,'');
  end else
    ResetShortMsg; // Сбросить все короткие сообщения
end;

//------------------------------------------------------------------------------
// работа с ярлыками на табло
procedure TTabloMain.SetPlakat(X,Y : integer);
  var i,j,dx : integer; uu : boolean;
begin
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then begin for i := 1 to High(Stellaj) do stellaj[i] := false; IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0; exit; end;
  uu := false;
  for j := 0 to 19 do
  begin
    dx := j * 12 + 1; if j > 10 then dx := dx + 3;
    if (X >= configRU[config.ru].BoxLeft+dx) and (Y >= configRU[config.ru].BoxTop) and
       (X < configRU[config.ru].BoxLeft+dx+12) and (Y < configRU[config.ru].BoxTop+13) then
    begin
      for i := 1 to 20 do if i <> (j+1) then stellaj[i] := false else stellaj[i] := not stellaj[i]; uu := true; IkonkaDeltaX := 0; IkonkaDeltaY := 0;
    end;
  end;
  if ((X+12+IkonkaDeltaX) >= configRU[config.ru].BoxLeft) and ((Y+12+IkonkaDeltaY) >= configRU[config.ru].BoxTop) and
     ((X+IkonkaDeltaX) < configRU[config.ru].BoxLeft+12*20+7) and ((Y+IkonkaDeltaY) < configRU[config.ru].BoxTop+16) then
  begin // проверить зону чувствительности полки с плакатами
    uu := true; for i := 1 to 20 do stellaj[i] := false;
  end;
  if not uu then
  begin
    // поиск вида плаката
    j := 0;
    for i := 1 to High(Stellaj) do if stellaj[i] then begin j := i; break; end;
    if j > 0 then
    begin // установить плакат
      case config.ru of
        1 : begin // RU1
          for i := 1 to High(Ikonki) do
          begin
            if Ikonki[i,1] = 0 then
            begin // поместить иконку на табло
              IkonNew := true; IkonSend := true;
              case j of
                1  : Ikonki[i,1] := 13;
                2  : Ikonki[i,1] := 14;
                3  : Ikonki[i,1] := 15;
                4  : Ikonki[i,1] := 16;
                5  : Ikonki[i,1] := 17;
                6  : Ikonki[i,1] := 18;
                7  : Ikonki[i,1] := 19;
                8  : Ikonki[i,1] := 20;
                9  : Ikonki[i,1] := 21;
                10 : Ikonki[i,1] := 22;
                11 : Ikonki[i,1] := 23;
                12 : Ikonki[i,1] := 4;
                13 : Ikonki[i,1] := 5;
                14 : Ikonki[i,1] := 6;
                15 : Ikonki[i,1] := 7;
                16 : Ikonki[i,1] := 8;
                17 : Ikonki[i,1] := 9;
                18 : Ikonki[i,1] := 10;
                19 : Ikonki[i,1] := 11;
                20 : Ikonki[i,1] := 12;
              end;
              Ikonki[i,2] := X; Ikonki[i,3] := Y; uu := true; break;
            end;
          end;
        end;
        2 : begin // RU2
          for i := 1 to High(Ikonki) do
          begin
            if Ikonki2[i,1] = 0 then
            begin // поместить иконку на табло
              IkonNew := true; IkonSend := true;
              case j of
                1  : Ikonki2[i,1] := 13;
                2  : Ikonki2[i,1] := 14;
                3  : Ikonki2[i,1] := 15;
                4  : Ikonki2[i,1] := 16;
                5  : Ikonki2[i,1] := 17;
                6  : Ikonki2[i,1] := 18;
                7  : Ikonki2[i,1] := 19;
                8  : Ikonki2[i,1] := 20;
                9  : Ikonki2[i,1] := 21;
                10 : Ikonki2[i,1] := 22;
                11 : Ikonki2[i,1] := 23;
                12 : Ikonki2[i,1] := 4;
                13 : Ikonki2[i,1] := 5;
                14 : Ikonki2[i,1] := 6;
                15 : Ikonki2[i,1] := 7;
                16 : Ikonki2[i,1] := 8;
                17 : Ikonki2[i,1] := 9;
                18 : Ikonki2[i,1] := 10;
                19 : Ikonki2[i,1] := 11;
                20 : Ikonki2[i,1] := 12;
              end;
              Ikonki2[i,2] := X; Ikonki2[i,3] := Y; uu := true; break;
            end;
          end;
        end;
      end;
      if not uu then Beep;
    end;
    for i := 1 to High(Stellaj) do stellaj[i] := false; // сброс на полке
  end;
end;

procedure TTabloMain.GetPlakat(X,Y : integer);
  var i,j,dx : integer; uu : boolean;
begin
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then begin for i := 1 to High(Stellaj) do stellaj[i] := false; IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0; exit; end;
  uu := false;
  for j := 0 to 19 do
  begin // проверить зону чувствительности полки с плакатами
    dx := j * 12 + 1; if j > 10 then dx := dx + 3;
    if (X >= configRU[config.ru].BoxLeft+dx) and (Y >= configRU[config.ru].BoxTop) and
       (X < configRU[config.ru].BoxLeft+dx+12) and (Y < configRU[config.ru].BoxTop+13) then
    begin
      for i := 1 to 20 do if i <> (j+1) then stellaj[i] := false else stellaj[i] := not stellaj[i]; uu := true;
    end;
  end;
  if not uu then
  begin
    case config.ru of
      1 : begin // RU1
        for i := High(Ikonki) downto 1 do
          if Ikonki[i,1] > 0 then
          begin
            if (Ikonki[i,2] <= X) and (Ikonki[i,2]+12 >= X) and (Ikonki[i,3] <= Y) and (Ikonki[i,3]+12 >= Y) then
            begin
              IkonkaDown := i; IkonkaMove := true; IkonkaMoved := false; IkonkaDeltaX := Ikonki[i,2] - X; IkonkaDeltaY := Ikonki[i,3] - Y; break;
            end;
          end;
      end;
      2 : begin // RU2
        for i := High(Ikonki2) downto 1 do
          if Ikonki2[i,1] > 0 then
          begin
            if (Ikonki2[i,2] <= X) and (Ikonki2[i,2]+12 >= X) and (Ikonki2[i,3] <= Y) and (Ikonki2[i,3]+12 >= Y) then
            begin
              IkonkaDown := i; IkonkaMove := true; IkonkaMoved := false; IkonkaDeltaX := Ikonki2[i,2] - X; IkonkaDeltaY := Ikonki2[i,3] - Y; break;
            end;
          end;
      end;
    end;
  end;
end;

procedure TTabloMain.DrawPlakat(X,Y : integer);
  var i,j,dx : integer; uu : boolean;
begin
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then begin for i := 1 to High(Stellaj) do stellaj[i] := false; IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0; exit; end;
  uu := false;
  if ((X+12+IkonkaDeltaX) >= configRU[config.ru].BoxLeft) and ((Y+12+IkonkaDeltaY) >= configRU[config.ru].BoxTop) and
     ((X+IkonkaDeltaX) < configRU[config.ru].BoxLeft+12*20+7) and ((Y+IkonkaDeltaY) < configRU[config.ru].BoxTop+16) then
  begin // проверить зону чувствительности полки с плакатами
    for j := 0 to 19 do
    begin // проверить зону чувствительности иконок
      dx := j * 12 + 1; if j > 10 then dx := dx + 3;
      if (X >= configRU[config.ru].BoxLeft+dx) and (Y >= configRU[config.ru].BoxTop) and
         (X < configRU[config.ru].BoxLeft+dx+12) and (Y < configRU[config.ru].BoxTop+13) then
      begin
        for i := 1 to 20 do if i <> (j+1) then stellaj[i] := false else stellaj[i] := not stellaj[i];
      end;
    end;
    uu := true;
  end;
  IkonNew := true; IkonSend := true;
  if not uu then
  begin // переместить плакат на табло
    case config.ru of
      1 : begin
        Ikonki[IkonkaDown,2] := X + IkonkaDeltaX; Ikonki[IkonkaDown,3] := Y + IkonkaDeltaY;
      end;
      2 : begin
        Ikonki2[IkonkaDown,2] := X + IkonkaDeltaX; Ikonki2[IkonkaDown,3] := Y + IkonkaDeltaY;
      end;
    end;
  end else
  begin // убрать плакат с табло
    case config.ru of
      1 : begin
        Ikonki[IkonkaDown,1] := 0;
      end;
      2 : begin
        Ikonki2[IkonkaDown,1] := 0;
      end;
    end;
  end;
end;

procedure ResetAllPlakat;
  var i : integer;
begin
  for i := 1 to High(Ikonki) do
  begin
    Ikonki[i,1] := 0; Ikonki[i,2] := 0; Ikonki[i,3] := 0;
  end;
  for i := 1 to High(Ikonki2) do
  begin
    Ikonki2[i,1] := 0; Ikonki2[i,2] := 0; Ikonki2[i,3] := 0;
  end;
  IkonNew := true; IkonSend := true;
end;

//------------------------------------------------------------------------------
// Реакция на мышку
procedure TTabloMain.FormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  GetPlakat(X,Y); // получить параметры перемещаемого плаката (если есть)
  OperatorDirect := LastTime + 1/86400;
end;

var LastID_Obj : integer;

procedure TTabloMain.FormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
  var n,i : integer;
begin
  if PopupMenuCmd.PopupComponent <> nil then exit;
  if LockCommandDsp then
  begin
    SingleBeep := true; exit;
  end;
  if Button = mbLeft then
  begin // нажата левая кнопка мышки
    if IkonkaMove and IkonkaMoved and (IkonkaDown > 0) and (cur_obj < 0) then
      DrawPlakat(X,Y); // переместить плакат в новое место
    IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0;
    if (X+shiftscr > configRU[config.ru].MsgLeft) and (X+shiftscr < configRU[config.ru].MsgRight) and
       (Y > configRU[config.ru].MsgTop) and (Y < configRU[config.ru].MsgBottom) then
    begin // работа в поле фиксированных сообщений
      if (X+shiftscr > configRU[config.ru].MsgRight-10) and (FixMessage.Count > 5) then
      begin // прокрутить список?
        if (Y < (configRU[config.ru].MsgTop+10)) and (FixMessage.MarkerLine > 1) then
        begin // прокрутка вверх (новые данные)
          dec(FixMessage.MarkerLine);
          if FixMessage.MarkerLine < FixMessage.StartLine then FixMessage.StartLine := FixMessage.MarkerLine;
        end else
        if (Y > (configRU[config.ru].MsgBottom-10)) and (FixMessage.MarkerLine < FixMessage.Count) then
        begin // прокрутка вниз (старые данные)
          inc(FixMessage.MarkerLine);
          if (FixMessage.MarkerLine - FixMessage.StartLine) > 4 then FixMessage.StartLine := FixMessage.MarkerLine - 4;
        end else
      end else
      begin // пометить строку
        n := (Y - configRU[config.ru].MsgTop) div 16 + FixMessage.StartLine; // Номер выбраной строки
        if n <= FixMessage.Count then FixMessage.MarkerLine := n;
      end;
      for i := 1 to High(Stellaj) do stellaj[i] := false; // сбросить полку с плакатами
      exit;
    end else

    if cur_obj < 0 then
    begin // проверить точку
      if WorkMode.GoTracert {and not MarhTracert[1].Finish} then
      begin // проверить не выбраны ли остряки стрелки при трассировке
        n := -1;
        for i := configRU[config.ru].OVmin to configRU[config.ru].OVmax do
          if ObjView[i].TypeObj = 11 then
          begin
            if (ObjView[i].Points[2].X-8 < X) and (ObjView[i].Points[2].Y-8 < Y) and
               (ObjView[i].Points[2].X+8 > X) and (ObjView[i].Points[2].Y+8 > Y) then
            begin
              n := ObjView[i].ObjConstI[4]; break;
            end;
          end;
        if n > 0 then
        begin
          for i := 1 to WorkMode.LimitObjZav do
            if (ObjZav[i].TypeObj >= 1) and (ObjZav[i].TypeObj <= 8) and (ObjZav[i].VBufferIndex = n) then
            begin // обнаружена стрелка
              if MarhTracert[1].Finish then
              begin // не продолжать трассу
                if LastID_Obj = i then
                begin // завершить набор, подтвердить трассу
                  LastID_Obj := -1; ID_Menu := KeyMenu_EndTrace; cur_obj := 20000; CreateDspMenu(ID_menu, X, Y); SelectCommand; break;
                end else
                begin // сбросить трассу
                  ResetTrace; LastID_Obj := -1; exit;
                end;
              end else
              begin // продолжать трассу
                LastID_Obj := i; ID_Obj := i; ID_Menu := IDMenu_Tracert; cur_obj := 20002; CreateDspMenu(ID_menu, X, Y); break;
              end;
            end;
        end else
        begin // проверить не нажат ли ярлык
          SetPlakat(X,Y); ResetCommands; exit;
        end;
      end else
      begin // проверить не нажат ли ярлык
        SetPlakat(X,Y); ResetCommands; exit;
      end;
    end else
    begin
      for i := 1 to High(Stellaj) do stellaj[i] := false; // сбросить полку с плакатами
    end;

    if (cur_obj > 0) and (DspMenu.WC or DspMenu.Ready) then
    begin // Команда для одиночного объекта
      if (DspMenu.Obj > 0) and (cur_obj <> DspMenu.Obj) then
      begin
        if WorkMode.OtvKom then begin ResetCommands; exit; end else
        if (MarhTracert[1].Finish or // запрос выдачи маршрутной команды
           (MarhTracert[1].GonkaStrel and (MarhTracert[1].GonkaList > 0))) and // запрос перевода стрелок по трассе
           (ObjUprav[cur_obj].IndexObj = 20000) then // нажата кнопка "Конец набора"
        begin
          MarhTracert[1].Finish := false; DspCommand.Active := true; SelectCommand;
        end else ResetCommands;
        exit;
      end;
    end;

    if DspMenu.WC then
    begin // подтверждение команды
      DspCommand.Active := true; SelectCommand;
    end else
    if DspMenu.Ready then
    begin // выбор команды
      DspMenu.WC := true;
    end else
    begin
      if ID_menu > 0 then
      begin
        CreateDspMenu(ID_menu, X, Y);
        if not DspMenu.WC then SelectCommand; // Выполнить команду сразу после вывода сообщения
        // иначе Ждать нажатия кнопки
      end else SingleBeep := true;
    end;
  end else
  begin // нажата правая кнопка мышки
    IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IkonkaDeltaX := 0; IkonkaDeltaY := 0; InsNewArmCmd(0,0); UnLockHint; ResetCommands;
    // Сбросить фиксируемое сообщение
    if (X+shiftscr > configRU[config.ru].MsgLeft) and (X+shiftscr < configRU[config.ru].MsgRight) and
       (Y > configRU[config.ru].MsgTop) and (Y < configRU[config.ru].MsgBottom) then
    begin
      n := (Y - configRU[config.ru].MsgTop) div 16 + FixMessage.StartLine; // Номер выбраной строки
      if n <= FixMessage.Count then begin FixMessage.MarkerLine := n; ResetFixMessage; end else SingleBeep := true;
    end;
  end;
end;

var
  LastKey : Word;        // Код последней нажатой клавиши
  LastKeyPress : Double; // Время нажатия последней клавиши

//------------------------------------------------------------------------------
// Обработка нажатия на клавиатуре
procedure TTabloMain.FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
begin
  OperatorDirect := LastTime + 1/86400;

  // проверить длительность нажатия клавиши
  if LastKey = Key then
  begin
    if (LastKeyPress+10/80000) < Date+Time then
    begin
      LastKeyPress := Date+Time;
      case Key of
        VK_RETURN : sMsg := 'ENTER';
        VK_ESCAPE : sMsg := 'ESC';
        VK_SPACE : sMsg := 'ПРОБЕЛ';
        VK_BACK : sMsg := 'ЗАБОЙ';
        VK_F1 : sMsg := 'F1';
        VK_F2 : sMsg := 'F2';
        VK_F3 : sMsg := 'F3';
        VK_F4 : sMsg := 'F4';
        VK_F5 : sMsg := 'F5';
        VK_F6 : sMsg := 'F6';
        VK_F7 : sMsg := 'F7';
        VK_F8 : sMsg := 'F8';
        VK_F9 : sMsg := 'F9';
        VK_F10 : sMsg := 'F10';
        VK_F11 : sMsg := 'F11';
        VK_F12 : sMsg := 'F12';
        VK_INSERT : sMsg := 'INSERT';
        VK_DELETE : sMsg := 'DELETE';
        VK_HOME : sMsg := 'HOME';
        VK_END : sMsg := 'END';
        VK_NEXT : sMsg := 'Page Down';
        VK_PRIOR : sMsg := 'Page Up';
        VK_LEFT : sMsg := 'Стрелка влево';
        VK_RIGHT : sMsg := 'Стрелка вправо';
        VK_DOWN : sMsg := 'Стрелка вниз';
        VK_UP : sMsg := 'Стрелка вверх';
        VK_SHIFT : sMsg := 'SHIFT';
        VK_MENU : sMsg := 'ALT';
        VK_CONTROL : sMsg := 'CTRL';
      else
        sMsg := Char(Key);
      end;
      PutShortMsg(1,LastX,LastY,'Внимание! Длительно нажата клавиша <'+ sMsg+ '>');
      Beep;
    end;
  end else
  begin // сохранить код клавиши и время нажатия
    LastKey := Key;
    LastKeyPress := Date+Time;
  end;
end;

//------------------------------------------------------------------------------
// Отпускание кнопки на клавиатуре
procedure TTabloMain.FormKeyUp(Sender: TObject; var Key: Word; Shift: TShiftState);
  var i,o,p : integer; x,y : integer; t : TPoint;
{$IFDEF SAVEKANAL} sl : TStringList; {$ENDIF}
begin
  LastKey := 0; // сбросить код нажатой клавиши при отпускании

  if PopupMenuCmd.PopupComponent <> nil then exit;
  case Key of

{$IFDEF SAVEKANAL}
    VK_F8 : begin
      sl := TStringList.Create;
      sl.Text := trmkvit;
      sl.SaveToFile('trm.txt');
      sl.Text := rcvkvit;
      sl.SaveToFile('rsv.txt');
      trmkvit := '';
      rcvkvit := '';
      sl.Free;
    end;
{$ENDIF}

    VK_DELETE :
    begin
      if Shift = [ssShift] then
      begin // сброс информационных тпбличек (всех)
        ResetAllPlakat;
        PutShortMsg(2,LastX,LastY,'Информационные таблички сброшены');
      end else
      if Shift = [] then
      begin // сброс трассы маршрута
        InsNewArmCmd(0,KeyMenu_ClearTrace);
        Cmd_ChangeMode(KeyMenu_ClearTrace);
      end;
    end;

    VK_LEFT :
    begin
      if cur_obj > 0 then
      begin
        if ObjUPrav[cur_obj].Neighbour[1] > 0 then cur_obj := ObjUPrav[cur_obj].Neighbour[1];
      end else
        cur_obj := StartObj;
      if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_RIGHT :
    begin
      if cur_obj > 0 then
      begin
        if ObjUPrav[cur_obj].Neighbour[2] > 0 then cur_obj := ObjUPrav[cur_obj].Neighbour[2];
      end else
        cur_obj := StartObj;
      if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_UP :
    begin
      if Shift = [ssCtrl] then
      begin
        if (FixMessage.Count > 0) and (FixMessage.MarkerLine > 1) then
        begin
          dec(FixMessage.MarkerLine); if FixMessage.MarkerLine < FixMessage.StartLine then FixMessage.StartLine := FixMessage.MarkerLine;
        end else
          Beep;
      end else
      if Shift = [] then
      begin
        if cur_obj > 0 then
        begin
          if ObjUPrav[cur_obj].Neighbour[3] > 0 then cur_obj := ObjUPrav[cur_obj].Neighbour[3];
        end else
          cur_obj := StartObj;
        if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
      end;
    end;

    VK_DOWN :
    begin
      if Shift = [ssCtrl] then
      begin
        if (FixMessage.Count > 0) and (FixMessage.MarkerLine < FixMessage.Count) then
        begin
          inc(FixMessage.MarkerLine); if FixMessage.MarkerLine > FixMessage.StartLine + 4 then FixMessage.StartLine := FixMessage.MarkerLine - 4;
        end else
          Beep;
      end else
      if Shift = [] then
      begin
        if cur_obj > 0 then
        begin
          if ObjUPrav[cur_obj].Neighbour[4] > 0 then cur_obj := ObjUPrav[cur_obj].Neighbour[4];
        end else
          cur_obj := StartObj;
        if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
      end;
    end;

    VK_RETURN :
    begin
      if LockCommandDsp then
      begin
        SingleBeep := true; exit;
      end;
      if DspMenu.WC then
      begin // Ожидается подтверждение команды
        if cur_obj = DspMenu.obj then
        begin // Выбрать команду
          DspCommand.Active := true; SelectCommand;
        end else
        begin // Сброс ранее подготовленной команды
          ResetShortMsg; // Сбросить все короткие сообщения
          DspMenu.Ready := false; DspMenu.WC := false; DspMenu.obj := -1;
        end;
      end else
      if DspMenu.Ready then
      begin // Ожидается выбор команды
        SelectCommand;
      end else
      begin
        if cur_obj < 0 then Beep else
        begin
          x := ObjUprav[cur_obj].Box.Left; y := ObjUprav[cur_obj].Box.Top;
          if ID_menu > 0 then
          begin
            CreateDspMenu(ID_menu, X, Y);
            if not DspMenu.WC then SelectCommand;
          end else Beep;
        end;
      end;
    end;

    VK_ESCAPE :
    begin
      UnLockHint; ResetCommands; InsNewArmCmd(0,0);
      if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_SPACE :
    begin
{$IFDEF DEBUG}
      if Shift = [ssCtrl] then // Технологическая функция
      begin
        WorkMode.PushOK := not WorkMode.PushOK;
      end else
      begin
{$ENDIF}
        UnLockHint; ResetCommands; InsNewArmCmd(0,0);
        if cur_obj > 0 then SetCursorPos(ObjUPrav[cur_obj].Box.Right-shiftscr-2, ObjUPrav[cur_obj].Box.Bottom-2);
{$IFDEF DEBUG}
      end;
{$ENDIF}
    end;

    VK_END :
    begin
      if WorkMode.MarhUpr then
      begin
        if Shift = [] then
        begin // Конец набора маршрута
          DspMenu.obj := 0;
          CreateDSPMenu(KeyMenu_EndTrace,LastX,LastY);
          SelectCommand;
        end else
        if Shift = [ssShift] then
        begin // Сброс набора
          DspMenu.obj := 0;
          CreateDSPMenu(KeyMenu_ClearTrace,LastX,LastY);
          SelectCommand;
        end;
      end;
    end;

    VK_HOME :
    begin
      if Shift = [] then
      begin // Перевести курсор на кнопки выбора режима работы
        for i := 1 to High(ObjUprav) do
        begin
          if config.ru = ObjUprav[i].RU then
          begin
            if WorkMode.MarhUpr and (ObjUprav[i].MenuID = KeyMenu_MarshRejim) then
            begin
              SetCursorPos(ObjUPrav[i].Box.Right-shiftscr-60, ObjUPrav[i].Box.Bottom-8);
              break;
            end else
            if WorkMode.RazdUpr and (ObjUprav[i].MenuID = KeyMenu_RazdeRejim) then
            begin
              SetCursorPos(ObjUPrav[i].Box.Right-shiftscr-60, ObjUPrav[i].Box.Bottom-8);
              break;
            end;
          end;
        end;
        GetCursorPos(t);
        FindCursorCnt := configRU[config.ru].TabloSize.X + configRU[config.ru].TabloSize.Y - t.X - t.Y;
        FindCursor := true;
      end;
    end;

    VK_F1 :
    begin
      if Shift = [] then
      begin
        if WorkMode.MarhUpr then
        begin // Установить раздельный режим управления
          DspMenu.obj := 0;
          CreateDSPMenu(KeyMenu_RazdeRejim,LastX,LastY);
          SelectCommand;
        end else
        begin // Установить маршрутный режим управления
          DspMenu.obj := 0;
          CreateDSPMenu(KeyMenu_MarshRejim,LastX,LastY);
          SelectCommand;
        end;
      end;
    end;

    VK_F2 :
    begin
      if Shift = [] then // Ввод времени
      begin
        DspMenu.obj := 0;
        CreateDSPMenu(KeyMenu_DateTime,LastX,LastY);
        SelectCommand;
      end;
    end;

    VK_F3 :
    begin
      if Shift = [] then // ввод номера поезда
      begin
        DspMenu.obj := 20002;
        CreateDSPMenu(KeyMenu_VvodNomeraPoezda,LastX,LastY);
        SelectCommand;
      end;
    end;

    VK_F4 :
    begin // Сброс фиксируемых сообщений
      if Shift = [] then
      begin
        sound := false; ResetFixMessage;
      end;
    end;

    VK_F5 :
    begin // Просмотр сообщений по неисправностям устройств
      if Shift = [] then
      begin
        if WorkMode.Upravlenie then exit;
        OpenMsgForm := true;
      end;
    end;

    VK_F6 :
    begin
{$IFDEF DEBUG}
// Включить управление для проверки без сервера
{      if Shift = [] then
      begin
        WorkMode.Upravlenie := true; WorkMode.LockCmd := false; StartRM := false;
      end;}
{$ENDIF}
    end;

    VK_PRIOR :
    begin
{$IFDEF DEBUG}
    // сдвиг экранов табло влево
      if shiftscr >= 800 then shiftscr := shiftscr - 800 else begin shiftscr := 0; Beep; end;
{$ENDIF}
    end;

    VK_NEXT :
    begin
{$IFDEF DEBUG}
    // сдвиг экранов табло вправо
      if shiftscr < (configru[config.ru].TabloSize.X - configru[config.ru].MonSize.X) then shiftscr := shiftscr + 800 else Beep;
{$ENDIF}
    end;

    VK_F7 :
    begin // сброс разрешения управления с РМ-ДСП (аварийный при отсутствии другой возможности)
      if (Shift = [ssShift,ssAlt,ssCtrl]) and not WorkMode.Upravlenie then
      begin
        if config.ru <> 1 then
        begin
{$IFNDEF DEBUG}
          if (DesktopSize.X >= configru[1].TabloSize.X) and (DesktopSize.Y >= configru[1].TabloSize.Y) then
{$ENDIF}
          begin
//            AddDspMenuItem(GetShortMsg(1,226, ''), CmdMenu_RU1,ID_Obj);
            NewRegion := 1; ChRegion := true;
          end;
        end;
        if config.ru <> 2 then
        begin
{$IFNDEF DEBUG}
          if (DesktopSize.X >= configru[2].TabloSize.X) and (DesktopSize.Y >= configru[2].TabloSize.Y) then
{$ENDIF}
          begin
//            AddDspMenuItem(GetShortMsg(1,227, ''), CmdMenu_RU2,ID_Obj);
            NewRegion := 2; ChRegion := true;
          end;
        end;
      end;
    end;


    VK_F8 :
    begin // переключатель подсветки номера поездов
      DspMenu.obj := 20003;
      CreateDSPMenu(KeyMenu_PodsvetkaNomerov,LastX,LastY);
      SelectCommand;
    end;


    VK_F9 :
    begin // переключатель подсветки положения стрелок, района местного управления
      DspMenu.obj := 20001;
      CreateDSPMenu(KeyMenu_PodsvetkaStrelok,LastX,LastY);
      SelectCommand;
    end;

    VK_F10 :
    begin // технологическая функция - выдача команд в обход интерфейса оператора
      if (asTestMode = $55) then exit;
      if Shift = [ssShift] then
      begin // Ввод маршрутной команды
        sMsg := InputBox('Ввод маршрутной команды', 'Введите маршрут', '');
        if sMsg <> '' then
        begin
          if (sMsg[1] = 'м') or (sMsg[1] = 'М') or (sMsg[1] = 'm') or (sMsg[1] = 'M') then MarhTracert[1].MarhCmd[10] := cmdfr3_marshrutmanevr else
          if (sMsg[1] = 'п') or (sMsg[1] = 'П') or (sMsg[1] = 'p') or (sMsg[1] = 'P') then MarhTracert[1].MarhCmd[10] := cmdfr3_marshrutpoezd else
            MarhTracert[1].MarhCmd[10] := cmdfr3_marshrutlogic;
          i := 3; sPar := '';
          while i <= Length(sMsg) do begin if sMsg[i] = ' ' then break else sPar := sPar + sMsg[i]; inc(i); end; inc(i);
          try x := StrToInt(sPar) except x := 0 end;
          MarhTracert[1].MarhCmd[1] := x - (x div 256) * 256;
          MarhTracert[1].MarhCmd[2] := x div 256;
          sPar := '';
          while i <= Length(sMsg) do begin if sMsg[i] = ' ' then break else sPar := sPar + sMsg[i]; inc(i); end; inc(i);
          try x := StrToInt(sPar) except x := 0 end;
          MarhTracert[1].MarhCmd[3] := x - (x div 256) * 256;
          MarhTracert[1].MarhCmd[4] := x div 256;
          sPar := '';
          while i <= Length(sMsg) do begin if sMsg[i] = ' ' then break else sPar := sPar + sMsg[i]; inc(i); end;
          MarhTracert[1].MarhCmd[5] := Length(sPar);
          MarhTracert[1].MarhCmd[6] := 0;
          MarhTracert[1].MarhCmd[7] := 0;
          MarhTracert[1].MarhCmd[8] := 0;
          MarhTracert[1].MarhCmd[9] := 0;
          o := 1; p := 0;
          for i := 1 to Length(sPar) do begin if sPar[i] = '1' then p := p + o; o := o * 2; end;
          i := p and $ff;MarhTracert[1].MarhCmd[6] := i; i := p and $ff00;
          i := i shr 8;MarhTracert[1].MarhCmd[7] := i; i := p and $ff0000;
          i := i shr 16;MarhTracert[1].MarhCmd[8] := i; i := p and $ff000000;
          i := i shr 24;MarhTracert[1].MarhCmd[9] := i;
          CmdSendT := LastTime;
          WorkMode.MarhRdy := true;
        end;
      end else
      begin // Ввод раздельной команды
        sMsg := InputBox('Ввод раздельной команды', 'Введите команду', '');
        if sMsg <> '' then
        begin
          i := 1; sPar := '';
          while i <= Length(sMsg) do begin if sMsg[i] = ' ' then break else sPar := sPar + sMsg[i]; inc(i); end; inc(i);
          try x := StrToInt(sPar) except x := 0 end;
          sPar := '';
          while i <= Length(sMsg) do begin if sMsg[i] = ' ' then break else sPar := sPar + sMsg[i]; inc(i); end;
          try y := StrToInt(sPar) except y := 0 end;
          if (x > 0) and (x < 190) and (y > 0) and (y < 1100) then begin CmdBuff.Cmd := x; CmdBuff.Index := y; CmdCnt := 1; CmdSendT := LastTime; end else Beep;
        end;
      end;
    end;

    VK_F11 :
    begin // подсветить положение курсора
      GetCursorPos(t);
      FindCursorCnt := configRU[config.ru].TabloSize.X + configRU[config.ru].TabloSize.Y - t.X - t.Y;
      if (t.X + t.Y) > FindCursorCnt then FindCursorCnt := t.X + t.Y;
      FindCursor := true;
    end;

    VK_F12 :
    begin // Сброс фиксируемого звонка
      sound := false;
    end;

    VK_INSERT :
    begin // Просмотр списка значений FR3, FR4
      if Shift = [ssShift,ssCtrl] then
      begin
        FrForm.Show;
      end;
    end;

  else
    inherited;
  end;
end;

var
  cntrsv : array[1..2] of integer;
//------------------------------------------------------------------------------
// обработчик главного таймера табло
procedure TTabloMain.MainTimerTimer(Sender: TObject);
  var gts : TSystemTime; st,i : integer; bh,bl,b : byte; ec: cardinal;
begin
  if Application.MainForm <> nil then
  begin
    if Application.MainForm.WindowState = wsMinimized then
      Application.MainForm.WindowState := wsNormal;
    if not Application.MainForm.Visible then
    begin
      Application.MainForm.Visible := true;
      SetForegroundWindow(Application.Handle);
    end;
  end;

  inc(GlobusIndex); if GlobusIndex > (31*8) then GlobusIndex := 0; // прокрутить глобус

  LastTime := Date+Time;                   // Сохранить момент последнего чтения из канала
  if CanFocus then DspMenu.Ready := false; // Управление курсором направить в табло

  try
    SyncReady; // Ожидание новых данных и синхронизации канала сервер-арм
    if LastTime - LastSync > RefreshTimeOut then // интервал 0,33 сек.
    begin // Пора рисовать на табло
    // проверить обмен по 1-му каналу
      inc(cntrsv[1]);
      if (KanalSrv[1].rcvcnt < 70) and (KanalSrv[1].cnterr > 4) then KanalSrv[1].iserror := true;
      if cntrsv[1] > 3 then begin cntrsv[1] := 0; KanalSrv[1].rcvcnt := 0; end else inc(KanalSrv[1].cnterr);
    // проверить обмен по 2-му каналу
      inc(cntrsv[2]);
      if (KanalSrv[2].rcvcnt < 70) and (KanalSrv[2].cnterr > 4) then KanalSrv[2].iserror := true;
      if cntrsv[2] > 3 then begin cntrsv[2] := 0; KanalSrv[2].rcvcnt := 0; end else inc(KanalSrv[2].cnterr);

    // Перерисовка экрана
      if not RefreshTablo then begin DateTimeToString(sMsg, 'dd/mm/yy h:nn:ss.zzz', LastTime); reportf('Сбой регенерации табло '+ sMsg); end;
    // Подготовить байт состояния режима управления
      if WorkMode.RazdUpr   then b := 1 else b := 0; // раздельное управление
      if WorkMode.MarhUpr   then b := b + 2;         // маршрутное управление
      if WorkMode.MarhOtm   then b := b + 4;         // отмена
      if WorkMode.InpOgr    then b := b + 8;         // ввод ограничений
      if WorkMode.VspStr    then b := b + $10;       // вспомогательный перевод
      if WorkMode.OtvKom    then b := b + $20;       // нажата КОК
      if WorkMode.Podsvet   then b := b + $40;       // подсветка стрелок
      if WorkMode.GoTracert then b := b + $80;       // ввод трассы маршрута
      if (ArmState <> b) and (WorkMode.ArmStateSoob > 0) then
      begin // если есть изменения - сохранить в архиве
        ArmState := b; FR3inp[WorkMode.ArmStateSoob] := char(b);
        bl := WorkMode.ArmStateSoob and $ff; bh := WorkMode.ArmStateSoob shr 8; NewFR[1] := NewFR[1] + char(bl) + char(bh) + char(ArmState);
      end;
    // сохранить канальную новизну и команды меню в архив
      if (NewFR[1] <> '') or (NewFR[2] <> '') or (NewCmd[1] <> '') or (NewCmd[2] <> '') or (NewMenuC <> '') or (NewMsg <> '') then
      begin
        SaveArch(1);
        if DspToArcEnabled then // ДСП-АРХИВ
        begin // передать архив по каналу ДСП-АРХИВ
          if DspToArcConnected and DspToArcAdresatEn and not DspToArcPending and (LenArc > 0) and (BuffArc[1] > 0) then
          begin
            SendDspToArc(@BuffArc[1],LenArc); LenArc := 0;
          end;
        end;
      end;

      if OpenMsgForm then
      begin
        OpenMsgForm := false;
        MsgFormDlg.Left := 0; MsgFormDlg.Top := 0;
        MsgFormDlg.Width := configru[config.ru].MonSize.X;
        MsgFormDlg.Height := configru[config.ru].MonSize.Y;
        MsgFormDlg.Show;
        UpdateMsgQuery := true;
      end;

      if (MsgFormDlg <> nil) and MsgFormDlg.Visible then
      begin
        if NewNeisprav then
        begin
          NewNeisprav := false; MsgFormDlg.BtnUpdate.Enabled := true;
        end;
        if UpdateMsgQuery then
        begin
          MsgFormDlg.BtnUpdate.Enabled := false;
          UpdateMsgQuery := false;
          st := 1; i := 0; while st <= Length(ListNeisprav) do begin if ListNeisprav[st] = #10 then inc(i); if i < 700 then inc(st) else begin SetLength(ListNeisprav,st); break; end; end; MsgFormDlg.Memo.Lines.Text := ListNeisprav;
          st := 1; i := 0; while st <= Length(ListDiagnoz) do begin if ListDiagnoz[st] = #10 then inc(i); if i < 700 then inc(st) else begin SetLength(ListDiagnoz,st); break; end; end; MsgFormDlg.MemoUVK.Lines.Text := ListDiagnoz;
        end;
      end;

      if (LastReper + (600 / 86400)) < LastTime then
      begin // сохранить 10-ти минутный архив состояний
        SaveArch(2);
        if DspToArcEnabled then // ДСП-АРХИВ
        begin // передать архив по каналу ДСП-АРХИВ
          if DspToArcConnected and DspToArcAdresatEn and not DspToArcPending and (LenArc > 0) then
          begin
            SendDspToArc(@BuffArc[1],LenArc); LenArc := 0;
          end;
        end;
      end;

// Технологическая функция - сохранить данные, полученные из канала
if savearc then SaveKanal;



    {обработать состояние системы}

    // проверка контрольной суммы записей массива ObjZav
      st := cntObjZav + 10;
      while cntObjZav < st do
      begin
        if cntObjZav <= WorkMode.LimitObjZav then
        begin
          if not CalcCRC_OZ(cntObjZav) then
          begin
            InsNewArmCmd($7ffc,0);
            PutShortMsg(1,LastX,LastY,'Искажение константной части описания объекта зависимостей в строке '+ IntToStr(cntObjZav));
            Beep;
          end;
          inc(cntObjZav);
        end else
        begin
          cntObjZav := 1; break; // переместить на начало
        end;
      end;
    // проверка контрольной суммы записей массива ObjView
      st := cntObjView + 20;
      while cntObjView < st do
      begin
        if cntObjView <= WorkMode.LimitObjView then
        begin
          if ObjView[cntObjView].TypeObj <> 0 then
          begin
            if not CalcCRC_OV(cntObjView) then
            begin
              InsNewArmCmd($7ffc,0);
              PutShortMsg(1,LastX,LastY,'Искажение константной части описания объекта табло в строке '+ IntToStr(cntObjView));
              Beep;
            end;
          end;
          inc(cntObjView);
        end else
        begin
          cntObjView := 1; break; // переместить на начало
        end;
      end;
    // проверка контрольной суммы записей массива ObjUprav
      st := cntObjUprav + 20;
      while cntObjUprav < st do
      begin
        if cntObjUprav <= WorkMode.LimitObjUprav then
        begin
          if ObjUprav[cntObjUprav].RU <> 0 then
          begin
            if not CalcCRC_OU(cntObjUprav) then
            begin
              InsNewArmCmd($7ffc,0);
              PutShortMsg(1,LastX,LastY,'Искажение константной части описания объекта управления в строке '+ IntToStr(cntObjUprav));
              Beep;
            end;
          end;
          inc(cntObjUprav);
        end else
        begin
          cntObjUprav := 1; break; // переместить на начало
        end;
      end;
    // проверка контрольной суммы записей массива OVBuffer
      st := cntOVBuffer + 20;
      while cntOVBuffer < st do
      begin
        if cntOVBuffer <= High(OVBuffer) then
        begin
          if (OVBuffer[cntOVBuffer].TypeRec <> 0) or (OVBuffer[cntOVBuffer].Jmp1 <> 0) or (OVBuffer[cntOVBuffer].Jmp2 <> 0) then
          begin
            if not CalcCRC_VB(cntOVBuffer) then
            begin
              InsNewArmCmd($7ffc,0);
              PutShortMsg(1,LastX,LastY,'Искажение константной части описания буфера отображения в строке '+ IntToStr(cntOVBuffer));
              Beep;
            end;
          end;
          inc(cntOVBuffer);
        end else
        begin
          cntOVBuffer := 1; break; // переместить на начало
        end;
      end;



      if WorkMode.ServerSync then
      begin // Выполнить синхронизацию времени серверов
        st := (config.RMID - 3) * 12;
        GetSystemTime(gts);
        if SyncTime then
        begin
          if not ((gts.wMinute = st) and (gts.wSecond = 28)) then SyncCmd := true;
        end else
        begin // Проверить необходимость выдачи синхронизации времени серверов
          if (gts.wMinute = st) and (gts.wSecond = 28) then SyncTime := true;
        end;
      end;

{$IFDEF DEBUG} WorkMode.OtvKom := WorkMode.PushOK;
{$ELSE}
      KeyOtvKomState;
      if ((StateRU and $20) = $20) and WorkMode.PushOK then
      begin // Есть разрешение ОК
        WorkMode.OtvKom := true;
      end else
      begin // Нет разрешения ОК
        WorkMode.OtvKom := false;
      end;
{$ENDIF}

      if WorkMode.OtvKom then begin Cursor := curTablo1ok; end else begin Cursor := curTablo1; end;

      if ChRegion then ChangeRegion(NewRegion); // выполнить изменение района управления
      if ChDirect then ChangeDirectState(StDirect); // выполнить изменение состояния управления

      if WorkMode.Upravlenie and WorkMode.OU[0] then
      begin // дать короткий звонок требуемой тональности
        if Ip1Beep then begin Ip1Beep := false;  PlaySound(PAnsiChar(IpWav.Strings[0]),0,SND_ASYNC); end else // 1 приближение
        if Ip2Beep then begin Ip2Beep := false; PlaySound(PAnsiChar(IpWav.Strings[1]),0,SND_ASYNC); end else  // 2 приближение
        if SingleBeep  then begin SingleBeep := false;  PlaySound(PAnsiChar(ObjectWav.Strings[0]),0,SND_ASYNC); end else
        if SingleBeep2 then begin SingleBeep2 := false; PlaySound(PAnsiChar(ObjectWav.Strings[1]),0,SND_ASYNC); end else
        if SingleBeep4 then begin SingleBeep4 := false; PlaySound(PAnsiChar(ObjectWav.Strings[3]),0,SND_ASYNC); end else
        if SingleBeep5 then begin SingleBeep5 := false; PlaySound(PAnsiChar(ObjectWav.Strings[4]),0,SND_ASYNC); end else
        if SingleBeep6 then begin SingleBeep6 := false; PlaySound(PAnsiChar(ObjectWav.Strings[5]),0,SND_ASYNC); end;
      end;
      if MsgStateRM <> '' then
      begin // вывод сообщения об изменении статуса
        PutShortMsg(MsgStateClr,LastX,LastY,MsgStateRM); MsgStateRM := '';
      end;


    end;
    MySync[1] := false; MySync[2] := false;

    if SendToSrvCloseRMDSP then
      if CmdSendT + (3/86400) < LastTime then
      begin // Закрыть главное окно РМ-ДСП
        IsCloseRMDSP := true; Close;
      end;

    if (CmdCnt = 0) and WorkMode.CmdReady and SendRestartServera then
    begin // отменить ожидание квитанции на команду перезапуска серверов
      SendRestartServera := false; WorkMode.CmdReady := false;
    end;

    if ((CmdCnt > 0) or (WorkMode.MarhRdy) or (WorkMode.CmdReady)) then
    begin // буфер команд заполнен
      if WorkMode.LockCmd then
      begin // есть блокировка команд - отложить выдачу команд на сервер
        if StartRM then
        begin
          if CmdSendT + (10/86400) < LastTime then
          begin // превышено время ожидания команды автоматического захвата управления
            InsNewArmCmd($7ffd,0); CmdCnt := 0; WorkMode.MarhRdy := false; WorkMode.CmdReady := false; StartRM := false; // сброс команд
          end;
        end else
        if not SendToSrvCloseRMDSP then CmdSendT := LastTime;
      end else
      begin // проверить время ожидания выдачи команды на сервер
        if CmdSendT + (4/86400) < LastTime then
        begin // превышено время ожидания команды
          InsNewArmCmd($7ffd,0); CmdCnt := 0; WorkMode.MarhRdy := false; WorkMode.CmdReady := false; // сброс команд
          if not StartRM then AddFixMessage(GetShortMsg(1,296,GetNameObjZav(CmdBuff.LastObj)),4,1);
        end;
      end;
    end;

    if StartRM and (StartTime < LastTime - 10/86400) then
    begin // процедуры старта системы
      if (KanalSrv[1].issync or KanalSrv[2].issync) and not WorkMode.LockCmd then StartRM := false;
    end;

    if LockCommandDsp then
    begin
      if TimeLockCmdDsp < (LastTime - 0.5/86400) then LockCommandDsp := false;
    end;

    // Отработать команду синхронизации времени
    SetDateTimeARM(DateTimeSync);



// обработка канала ДСП - ДСП

    // сформировать байт приоритета массива ярлыков
    IkonPri := $1f;
    if not StartRM then IkonPri := IkonPri and $ff-$10;
    if IkonNew then IkonPri := IkonPri and $ff-$8;
    if WorkMode.Upravlenie then IkonPri := IkonPri and $ff-$4;
    if config.ru = config.def_ru then IkonPri := IkonPri and $ff-$2;
    if config.Master then IkonPri := IkonPri and $ff-$1;

    // потоки АСУ
    if DspToDspEnabled then // ДСП-ДСП
    begin
      if GetExitCodeThread(DspToDspThread,ec) then
      begin // проверить исполнение потока обработки трубы АСУ1
        if (ec <> STILL_ACTIVE) and not IsBreakKanalASU then
        begin
          AddFixMessage('Разрыв связи по каналу АСУ1',4,6);
          case DspToDspType of
            1 : DspToDspThread := CreateThread(nil,0,@DspToDspClientProc,DspToDspParam,0,DspToDspThreadID); // клиент АСУ1
          else
            DspToDspThread := CreateThread(nil,0,@DspToDspServerProc,DspToDspParam,0,DspToDspThreadID);     // сервер АСУ1
          end;
          if DspToDspThread = INVALID_HANDLE_VALUE then
            reportf('Ошибка создания процесса обработки канала АСУ1. '+ DateTimeToStr(LastTime));
        end else
        begin // обработать данные по приему из канала АСУ1
          // принять таблички из канала ДСП-ДСП2 (АСУ1)
          if DspToDspInputBufPtr > 0 then ExtractPacketASU(@DspToDspInputBuf[0],@DspToDspInputBufPtr);
          DspToDspSucces := false; // сбросить буфер приема
        end;
      end;
      if IkonSend then
      begin // передать таблички по каналу ДСП-ДСП2 (АСУ1)
        if DspToDspConnected and DspToDspAdresatEn and not DspToDspPending then
        begin
          IkonNew := false; IkonSend := false;
          IkonkiPack; // сформировать массив табличек
          SendDspToDsp(@IkonkiOut[0],1007); // передать таблички на другой РМ-ДСП
        end;
      end;
    end;
    if DspToArcEnabled then // ДСП-АРХИВ
    begin
      if GetExitCodeThread(DspToArcThread,ec) then
      begin // проверить исполнение потока обработки трубы АРХИВ
        if (ec <> STILL_ACTIVE) and not IsBreakKanalASU then
        begin
          AddFixMessage('Разрыв связи по каналу удаленного архива',4,6);
          DspToArcThread := CreateThread(nil,0,@DspToARCProc,DspToArcParam,0,DspToArcThreadID); // клиент АРХИВ
          if DspToArcThread = INVALID_HANDLE_VALUE then
            reportf('Ошибка создания процесса обработки канала хранилища удаленного архива. '+ DateTimeToStr(LastTime));
        end;
      end;
    end;

  finally
    MainTimer.Enabled := true;
  end;
end;

//------------------------------------------------------------------------------
// Обновление образа табло
function TTabloMain.RefreshTablo : Boolean;
  var Delta : Double; a,b : boolean;
begin
  LastSync := LastTime;
  PrepareOZ;

  if OtvCommand.Active then // Ожидание выдача исполнительной ответственной команды
  begin
    // проверить сообщения из сервера
    if OtvCommand.State <> GetFR3(OtvCommand.Check,a,b) then // состояние контролируемого датчика изменилось
      OtvCommand.Ready := false; // есть подтверждение предв.ком - разрешить исполнительную

    // обработать состояние ОтвК
    Delta := OtvCommand.Reper - LastTime; // Остаток времени
    if Delta > 0 then
    begin
      if not WorkMode.OtvKom then
      begin // отпустили кнопку ответственных команд
        OtvCommand.Active := false; WorkMode.GoOtvKom := false; OtvCommand.Ready := false;
        ShowShortMsg(156, LastX, LastY, '');
        InsArcNewMsg(0,156);
        SingleBeep := true;
      end else
      if OtvCommand.Second > 0 then
      begin
        if OtvCommand.Ready then
        begin // преждевременная исполнительная команда
          OtvCommand.Active := false; WorkMode.GoOtvKom := false;
          ResetShortMsg;
          AddFixMessage(GetShortMsg(1,155,''),1,1);
          InsArcNewMsg(0,155);
        end else
        if (OtvCommand.Second = OtvCommand.Cmd) and (OtvCommand.Obj = OtvCommand.SObj) then
        begin // Обнаружена исполнительная команда
          OtvCommand.Active := false; WorkMode.GoOtvKom := false; OtvCommand.Ready := false;
          DspCommand.Command := OtvCommand.Second;
          DspCommand.Obj     := OtvCommand.SObj;
          DspCommand.Active  := true;
          SelectCommand;
        end else
        begin // Вторая команда не соответствует ожидаемой
          OtvCommand.Active := false; WorkMode.GoOtvKom := false; OtvCommand.Ready := false;
          ResetShortMsg;
          AddFixMessage(GetShortMsg(1,153,''),1,1);
          InsArcNewMsg(0,153);
        end;
      end else
      begin // продолжить ожидание исполнительной команды
        if not (OtvCommand.Ready or (OtvCommand.SObj > 0)) then
        begin // вывод отсчета таймера
          delta := delta * 84000;
          PutShortMsg(7, LastX, LastY, GetShortMsg(1,154, '') + FloatToStrF(delta,ffFixed,2,0));
        end;
      end;
    end else
    begin // Сброс ожидания по превышению времени
      OtvCommand.Active := false; WorkMode.GoOtvKom := false; OtvCommand.Ready := false;
      ShowShortMsg(152, LastX, LastY, ObjZav[ObjZav[DspCommand.Obj].BaseObject].Liter + sMsg);
      InsArcNewMsg(0,152); SingleBeep := true;
    end;
  end else
  if VspPerevod.Active then // Ожидание нажатия кнопки выключения контроля при вспомогательном переводе
  begin
    Delta := VspPerevod.Reper - LastTime; // Остаток времени
    if Delta > 0 then
    begin
      if ObjZav[ObjZav[ObjZav[VspPerevod.Strelka].BaseObject].ObjConstI[13]].bParam[1] then
      begin // Нажата кнопка вспомогательного перевода - выдать команду
        VspPerevod.Active := false; WorkMode.VspStr := false;
        DspCommand.Command := VspPerevod.Cmd;
        DspCommand.Obj     := VspPerevod.Strelka;
        DspCommand.Active  := true;
        SelectCommand;
      end else
      begin // Продолжить ожидание
        delta := delta * 84000;
        case VspPerevod.Cmd of
          CmdStr_ReadyVPerevodPlus : begin
            sMsg := GetShortMsg(1,99, ObjZav[ObjZav[VspPerevod.Strelka].BaseObject].Liter) + FloatToStrF(delta,ffFixed,2,0);
            PutShortMsg(7, LastX, LastY, sMsg); InsArcNewMsg(VspPerevod.Strelka,99);
          end;
          CmdStr_ReadyVPerevodMinus : begin
            sMsg := GetShortMsg(1,100, ObjZav[ObjZav[VspPerevod.Strelka].BaseObject].Liter) + FloatToStrF(delta,ffFixed,2,0);
            PutShortMsg(7, LastX, LastY, sMsg); InsArcNewMsg(VspPerevod.Strelka,100);
          end;
        end;
      end;
    end else
    begin // Сбросить команду ВСП
      VspPerevod.Active := false; WorkMode.VspStr := false;
      ShowShortMsg(148, LastX, LastY, ObjZav[ObjZav[DspCommand.Obj].BaseObject].Liter); InsArcNewMsg(VspPerevod.Strelka,148);
    end;
  end;

  mem_page := not mem_page;
  if not mem_page then tab_page := not tab_page;

  if mem_page then DrawTablo(Tablo2) // Подготовка табло2 для отображения
  else DrawTablo(Tablo1);            // Подготовка табло1 для отображения
  Invalidate;                        // Перерисовка экрана
  result := true;
end;

//------------------------------------------------------------------------------
// обработчик таймера выработки звукового сигнала
procedure TTabloMain.BeepTimerTimer(Sender: TObject);
begin
  if WorkMode.Upravlenie and WorkMode.OU[0] and Sound then PlaySound(PAnsiChar(ObjectWav.Strings[2]),0,SND_ASYNC);
end;

//------------------------------------------------------------------------------
// Инкремент счетчика ответственных команд
procedure IncrementKOK;
begin
  inc(KOKCounter);
  reg.RootKey := HKEY_LOCAL_MACHINE;
  if Reg.OpenKey(KeyName, false) then
  begin
    if reg.ValueExists('KOK') then reg.WriteInteger('KOK',KOKCounter);
    reg.CloseKey;
  end;
end;

//------------------------------------------------------------------------------
// Проверка состояния кнопки ОК
{$IFNDEF DEBUG}
procedure TTabloMain.KeyOtvKomState;
begin
  if (ConfigPortOK = '') and ((ConfigPortOK1 = '') or (ConfigPortOK2 = '')) then exit;
  if (PortOK = nil) and ((PortOK1 = nil) or (PortOK2 = nil)) then exit;

  if ConfigPortOK2 = '' then
  begin // кнопка с DTR
    case GetKOKStateKvit of
      0 : begin // неисправность КОК
        MsgStateRM := GetShortMsg(1,314,''); MsgStateClr := 1; SingleBeep := true;
      end;
    end;
  end else
  begin // Кнопка с Tx
    case GetKOKStateTx of
      0 : begin // неисправность КОК
        MsgStateRM := GetShortMsg(1,314,''); MsgStateClr := 1; SingleBeep := true;
      end;
      254 : begin // КОК подключена неправильно
        AddFixMessage(GetShortMsg(1,340,''),4,3);
      end;
    end;
  end;
end;
{$ENDIF}

//------------------------------------------------------------------------------
// установить начальные значения переменных в объектах
procedure PresetObjParams;
  var i : integer;
begin
  for i := 1 to High(ObjZav) do
    case ObjZav[i].TypeObj of
      2 : begin // стрелка
        ObjZav[i].bParam[4] := false;
        ObjZav[i].bParam[5] := false;
      end;
      3 : begin // Секция
        ObjZav[i].bParam[1] := true;  // СП
        ObjZav[i].bParam[2] := true;  // з
        ObjZav[i].bParam[4] := true;  // МСП
        ObjZav[i].bParam[5] := true;  // МИ
        ObjZav[i].bParam[8] := true;  // Предварительное замыкание
        ObjZav[i].bParam[10] := true; // Аварийное замыкание
      end;
      4 : begin // Путь
        ObjZav[i].bParam[1] := true; // П(ч)
        ObjZav[i].bParam[2] := true; // ЧИ
        ObjZav[i].bParam[3] := true; // НИ
        ObjZav[i].bParam[5] := true; // МИ(ч)
        ObjZav[i].bParam[6] := true; // МИ(н)
        ObjZav[i].bParam[8] := true; // Предварительное замыкание
        ObjZav[i].bParam[10] := true; // Аварийное замыкание
        ObjZav[i].bParam[16] := true; // П(н)
      end;
      5 : begin // Светофор
        ObjZav[i].iParam[1] := 0;
        ObjZav[i].iParam[2] := 0;
        ObjZav[i].iParam[3] := 0;
      end;
      15 : begin // АБ
        ObjZav[i].bParam[6] := false;
        ObjZav[i].bParam[9] := true;
      end;
      34 : begin // Питание
        ObjZav[i].bParam[1] := true;
        ObjZav[i].bParam[2] := true;
      end;
      38 : begin // контроль надвига
        ObjZav[i].bParam[1] := false;
      end;

    end;
{$IFDEF DEBUG}
// Установить признак активности объектов FR3s
  for i := 1 to FR_LIMIT do FR3s[i] := LastTime;
  for i := 1 to FR_LIMIT do FR4s[i] := LastTime;
{$ENDIF}
end;

//------------------------------------------------------------------------------
// Выполнить переключение Управление -> резерв -> Управление
procedure ChangeDirectState(State : Boolean);
  var i: Integer; f : Byte;
begin
  ChDirect := false;

  for i := 1 to High(ObjZav) do
  begin // Сбросить признаки трассировки
    case ObjZav[i].TypeObj of
      1 : begin // хвост стрелки
        ObjZav[i].bParam[6]  := false;
        ObjZav[i].bParam[7]  := false;
        ObjZav[i].bParam[14] := false;
      end;
      2 : begin // стрелка
        ObjZav[i].bParam[6]  := false;
        ObjZav[i].bParam[7]  := false;
        ObjZav[i].bParam[10] := false;
        ObjZav[i].bParam[11] := false;
        ObjZav[i].bParam[12] := false;
        ObjZav[i].bParam[13] := false;
        ObjZav[i].bParam[14] := false;
        ObjZav[i].iParam[1] := 0;
      end;
      3 : begin // участок
        ObjZav[i].bParam[14] := false;
        ObjZav[i].iParam[1] := 0;
        ObjZav[i].bParam[8] := true;
        ObjZav[i].bParam[19] := false;
        ObjZav[i].bParam[22] := false;
        ObjZav[i].bParam[23] := false;
      end;
      4 : begin // путь
        ObjZav[i].bParam[14] := false;
        ObjZav[i].iParam[1] := 0;
        ObjZav[i].bParam[8] := true;
        ObjZav[i].bParam[19] := false;
        ObjZav[i].bParam[21] := false;
        ObjZav[i].bParam[22] := false;
        ObjZav[i].bParam[23] := false;
      end;
      5 : begin // светофор
        ObjZav[i].bParam[14] := false;
        ObjZav[i].iParam[1] := 0;
        ObjZav[i].bParam[7] := false;
        ObjZav[i].bParam[9] := false;
        ObjZav[i].bParam[19] := false;
        ObjZav[i].bParam[20] := false;
        ObjZav[i].bParam[22] := false;
        ObjZav[i].bParam[23] := false;
      end;
      15 : begin // АБ
        ObjZav[i].bParam[14] := false;
        ObjZav[i].bParam[15] := false;
      end;
      25 : begin // маневровая колонка
        ObjZav[i].bParam[14] := false;
        ObjZav[i].bParam[8] := false;
      end;
    end;
  end;

  // сбросить список фиксированных сообщений
  FixMessage.Count := 0;
  FixMessage.MarkerLine := 0;
  FixMessage.StartLine := 0;

  if State then
  begin // Получено разрешение на включение управления
    if not WorkMode.Upravlenie then
    begin // Выполнить переключение Резерв -> Управление
      maket_strelki_index := 0;
      maket_strelki_name  := '';
      for i := 1 to High(ObjZav) do
      begin // Установить состояния объектов
      // применить FR4
        case ObjZav[i].TypeObj of
          1 : begin // хвост стрелки
            f := fr4[ObjZav[i].ObjConstI[1] div 8]; // буфер FR4
            ObjZav[i].bParam[19] := (f and $2) = $2; // макет
            if ObjZav[i].bParam[19] and (ObjZav[i].RU = config.ru) then
            begin // повесить литер стрелки на макетный шильдик
              maket_strelki_index := i;
              maket_strelki_name  := ObjZav[i].Liter;
            end;
            if ObjZav[i].ObjConstI[8] > 0 then
            begin // ограничения для ближней стрелки
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[15] := ObjZav[i].bParam[19]; // макет
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[16] := (f and $4) = $4; // закр.движ.
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[18] := (f and $1) = $1; // откл.упр.
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[17] := (f and $10) = $10; // закр.противош.движ.
            end;
            if ObjZav[i].ObjConstI[9] > 0 then
            begin // ограничения для дальней стрелки
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[15] := ObjZav[i].bParam[19]; // макет
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[16] := (f and $8) = $8; // закр.движ.
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[18] := (f and $1) = $1; // откл.упр.
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[17] := (f and $20) = $20; // закр.противош.движ.
            end;
          end;
          3 : begin // участок
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[25] := (f and $1) = $1; // закр.движ.пост.т
            ObjZav[i].bParam[26] := (f and $2) = $2; // закр.движ.пер.т
            ObjZav[i].bParam[13] := (f and $4) = $4; // закр.движ.
            ObjZav[i].bParam[24] := (f and $8) = $8; // закр.движ.э.т
          end;
          4 : begin // путь
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[25] := (f and $1) = $1; // закр.движ.пост.т
            ObjZav[i].bParam[26] := (f and $2) = $2; // закр.движ.пер.т
            ObjZav[i].bParam[13] := (f and $4) = $4; // закр.движ.
            ObjZav[i].bParam[24] := (f and $8) = $8; // закр.движ.э.т
          end;
          5 : begin // светофор
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[13] := (f and $4) = $4; // заблокирован
          end;
          15 : begin // АБ
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[25] := (f and $1) = $1; // закр.движ.пост.т
            ObjZav[i].bParam[26] := (f and $2) = $2; // закр.движ.пер.т
            ObjZav[i].bParam[13] := (f and $4) = $4; // закр.движ.
            ObjZav[i].bParam[24] := (f and $8) = $8; // закр.движ.э.т
          end;
          24 : begin // Увязка с ЭЦ
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[25] := (f and $1) = $1; // закр.движ.пост.т
            ObjZav[i].bParam[26] := (f and $2) = $2; // закр.движ.пер.т
            ObjZav[i].bParam[15] := (f and $4) = $4; // закр.движ.
            ObjZav[i].bParam[24] := (f and $8) = $8; // закр.движ.э.т
          end;
          26 : begin // РПБ
            f := fr4[ObjZav[i].ObjConstI[1]]; // буфер FR4
            ObjZav[i].bParam[25] := (f and $1) = $1; // закр.движ.пост.т
            ObjZav[i].bParam[26] := (f and $2) = $2; // закр.движ.пер.т
            ObjZav[i].bParam[13] := (f and $4) = $4; // закр.движ.
            ObjZav[i].bParam[24] := (f and $8) = $8; // закр.движ.э.т
          end;
        end;
      end;
      SingleBeep := false; Sound := false;
      SingleBeep3 := false; SingleBeep4 := false; SingleBeep5 := false; SingleBeep6 := false;
      AddFixMessage(GetShortMsg(1,273,''),0,2);
      WorkMode.Upravlenie := true;
      MsgFormDlg.Hide; // Скрыть окно просмотра сообщений
    end;
  end else
  begin // Получена команда на отключение управления
    if WorkMode.Upravlenie then
    begin // Выполнить переключение Управление -> Резерв
      ResetCommands;
      WorkMode.Upravlenie := false;
      for i := 1 to High(ObjZav) do
      begin // Установить состояния объектов
      // сбросить FR4
        case ObjZav[i].TypeObj of
          1 : begin // хвост стрелки
            ObjZav[i].bParam[19] := false; // макет
            if ObjZav[i].ObjConstI[8] > 0 then
            begin // ограничения для ближней стрелки
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[15] := false; // макет
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[16] := false; // закр.движ.
              ObjZav[ObjZav[i].ObjConstI[8]].bParam[18] := false; // откл.упр.
            end;
            if ObjZav[i].ObjConstI[9] > 0 then
            begin // ограничения для дальней стрелки
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[15] := false; // макет
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[16] := false; // закр.движ.
              ObjZav[ObjZav[i].ObjConstI[9]].bParam[18] := false; // откл.упр.
            end;
          end;
          3 : begin // участок
            ObjZav[i].bParam[13] := false; // закр.движ.
            ObjZav[i].bParam[24] := false; // закр.движ.э.т
            ObjZav[i].bParam[25] := false; // закр.движ.пост.т
            ObjZav[i].bParam[26] := false; // закр.движ.пер.т
          end;
          4 : begin // путь
            ObjZav[i].bParam[13] := false; // закр.движ.
            ObjZav[i].bParam[24] := false; // закр.движ.э.т
            ObjZav[i].bParam[25] := false; // закр.движ.пост.т
            ObjZav[i].bParam[26] := false; // закр.движ.пер.т
          end;
          5 : begin // светофор
            ObjZav[i].bParam[13] := false; // заблокирован
          end;
          15 : begin // АБ
            ObjZav[i].bParam[13] := false; // закр.движ.
            ObjZav[i].bParam[24] := false; // закр.движ.э.т
            ObjZav[i].bParam[25] := false; // закр.движ.пост.т
            ObjZav[i].bParam[26] := false; // закр.движ.пер.т
          end;
          24 : begin // Увязка с ЭЦ
            ObjZav[i].bParam[15] := false; // закр.движ.
            ObjZav[i].bParam[24] := false; // закр.движ.э.т
            ObjZav[i].bParam[25] := false; // закр.движ.пост.т
            ObjZav[i].bParam[26] := false; // закр.движ.пер.т
          end;
          26 : begin // РПБ
            ObjZav[i].bParam[13] := false; // закр.движ.
            ObjZav[i].bParam[24] := false; // закр.движ.э.т
            ObjZav[i].bParam[25] := false; // закр.движ.пост.т
            ObjZav[i].bParam[26] := false; // закр.движ.пер.т
          end;
        end;
      end;
      AddFixMessage(GetShortMsg(1,274,''),0,2);
    end;
  end;
end;

//------------------------------------------------------------------------------
// Изменить район управления
procedure ChangeRegion(RU : Byte);
  var i: Integer; f : Byte;
begin
  ChRegion := false;
  if ChDirect or WorkMode.Upravlenie or SendToSrvCloseRMDSP then exit;
  config.ru := RU;
  // Установить позицию вывода запроса пароля
  PasswordPos.X := configRU[config.ru].MsgLeft+1; PasswordPos.Y := configRU[config.ru].MsgTop+1;
  for i := 1 to High(ObjZav) do
  begin // Установить состояния объектов
    case ObjZav[i].TypeObj of
      1 : begin // хвост стрелки
        f := fr4[ObjZav[i].ObjConstI[1] div 8]; // буфер FR4
        if ((f and $2) = $2) and (ObjZav[i].RU = config.ru) then
        begin // повесить литер стрелки на макетный шильдик
          maket_strelki_index := i;
          maket_strelki_name  := ObjZav[i].Liter;
        end;
      end;
    end;
  end;
  ResetAllPlakat;
  SetParamTablo;
end;

//------------------------------------------------------------------------------
// Блокировка запроса на изменение размеров окна
procedure TTabloMain.FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer; var Resize: Boolean);
begin
  Resize := false;
end;

//------------------------------------------------------------------------------
// Обработка каналов АСУ (сеть верхнего уровня)
procedure TTabloMain.ASUTimer(Sender: TObject);
begin
  IkonSend := true; // сформировать принудительный обмен по каналам АСУ в случае простоя
end;

end.

