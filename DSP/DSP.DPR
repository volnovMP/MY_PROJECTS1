program DSP;

uses
  Windows,
  Forms,
  Registry,
  Objsost in '..\Commons\OBJSOST.PAS',
  typerpc in '..\Commons\TYPERPC.PAS',
  comport in '..\Commons\COMPORT.PAS',
  CRCCALC in '..\Commons\CRCCALC.PAS',
  Marshrut in '..\Commons\Marshrut.pas',
  ViewFr in '..\Commons\ViewFr.pas' {FrForm},
  MainLoop in '..\Commons\MainLoop.pas',
  CMenu in '..\Commons\CMenu.pas',
  PipeProc in '..\Commons\PipeProc.pas',
  Commands in '..\Commons\Commands.pas',
  Load in '..\Commons\Load.pas',
  Commons in '..\Commons\Commons.pas',
  VarStruct in '..\Commons\VarStruct.pas',
  KanalArmSrv in 'KanalArmSrv.pas',
  ButtonOK in 'ButtonOK.pas',
  TabloForm in 'TabloForm.pas' {TabloMain},
  TimeInput in 'TimeInput.pas' {TimeInputDlg},
  MsgForm in 'MsgForm.pas' {MsgForm1Dlg},
  Password in 'Password.pas' {PasswordDlg},
  ASUProc in 'ASUProc.pas';

{$R *.RES}

var h,o : THandle; reg : TRegistry;

begin
  reg := TRegistry.Create;
  reg.RootKey := HKEY_LOCAL_MACHINE;
  findSrv := false;
  if Reg.OpenKey('\Software\DSPRPCTUMS', false) then
  begin
    if reg.ValueExists('FindServerForRun') then findSrv := reg.ReadBool('FindServerForRun');
    reg.CloseKey;
  end;

  h := FindWindowEx(0,0,PChar('TStan'+#0),PChar('Stan'+#0));
  if not findSrv or (h > 0) then
  begin // если запущена программа сервера или не требуется проверка сервера
    h := FindWindowEx(0,0,PChar('TTabloMain'+#0),PChar('Табло'+#0));
    if h > 0 then
    begin
      o := GetWindow(h,GW_OWNER); if o > 0 then begin SetForegroundWindow(o); ShowWindow(o,SW_SHOWNORMAL); end;
    end else
    begin
      Application.Initialize;
      Application.Title := 'РМ ДСП';
      Application.CreateForm(TTabloMain, TabloMain);
      Application.CreateForm(TPasswordDlg, PasswordDlg);
      Application.CreateForm(TFrForm, FrForm);
      Application.Run;
    end;
  end;
end.
