program ARMSHN;

uses
  Windows,
  Forms,
  Objsost in '..\Commons\OBJSOST.PAS',
  comport in '..\Commons\COMPORT.PAS',
  CRCCALC in '..\Commons\CRCCALC.PAS',
  typerpc in '..\Commons\TYPERPC.PAS',
  Marshrut in '..\Commons\Marshrut.pas',
  MainLoop in '..\Commons\MainLoop.pas',
  CMenu in '..\Commons\CMenu.pas',
  Commands in '..\Commons\Commands.pas',
  Load in '..\Commons\Load.pas',
  Commons in '..\Commons\Commons.pas',
  VarStruct in '..\Commons\VarStruct.pas',
  ViewFr in '..\Commons\ViewFr.pas' {FrForm},
  TabloForm in 'TabloForm.pas' {TabloMain},
  MsgForm in 'MsgForm.pas' {MsgFormDlg},
  Password in 'Password.pas' {PasswordDlg},
  ValueList in 'ValueList.pas' {ValueListDlg},
  ViewObj in 'ViewObj.pas' {ViewObjForm},
  Notify in 'Notify.pas' {NotifyForm},
  KanalArmSrv in '..\Commons\KanalArmSrv.pas',
  KanalArmDc in 'KanalArmDc.pas',
  Clock in 'Clock.pas' {ClockForm};

{$R *.RES}

var h,o : THandle;

begin
  h := FindWindowEx(0,0,PChar('TTabloMain'+#0),PChar('АРМ ШН - Контроль станции в реальном времени'+#0));
  if h > 0 then
  begin
    o := GetWindow(h,GW_OWNER); if o > 0 then begin SetForegroundWindow(o); ShowWindow(o,SW_SHOWNORMAL); end;
  end else
  begin
    Application.Initialize;
    Application.Title := 'АРМ ШН';
    Application.CreateForm(TTabloMain, TabloMain);
  Application.CreateForm(TValueListDlg, ValueListDlg);
  Application.CreateForm(TPasswordDlg, PasswordDlg);
  Application.CreateForm(TFrForm, FrForm);
  Application.CreateForm(TViewObjForm, ViewObjForm);
  Application.CreateForm(TMsgFormDlg, MsgFormDlg);
  Application.CreateForm(TNotifyForm, NotifyForm);
  Application.CreateForm(TClockForm, ClockForm);
  Application.Run;
  end;
end.
