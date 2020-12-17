program ARCVIEW;

uses
  Windows,
  Forms,
  Objsost in '..\Commons\OBJSOST.PAS',
  typerpc in '..\Commons\TYPERPC.PAS',
  CRCCALC in '..\Commons\CRCCALC.PAS',
  comport in '..\Commons\COMPORT.PAS',
  Marshrut in '..\Commons\Marshrut.pas',
  MainLoop in '..\Commons\MainLoop.pas',
  CMenu in '..\Commons\CMenu.pas',
  Commands in '..\Commons\Commands.pas',
  PackArmSrv in 'PackArmSrv.pas',
  TabloForm in 'TabloForm.pas' {TabloMain},
  MsgForm in 'MsgForm.pas' {MsgFormDlg},
  ValueList in 'ValueList.pas' {ValueListDlg},
  DirectForm in 'DirectForm.pas' {DirectFormDlg},
  Load in '..\Commons\Load.pas',
  Commons in '..\Commons\Commons.pas',
  VarStruct in '..\Commons\VarStruct.pas';

{$R *.RES}

var h : cardinal;

begin
  h := FindWindowEx(0,0,PChar('TDSPMain'+#0),PChar('Архив системного журнала'+#0));
  if h > 0 then
  begin
    ShowWindow(h,SW_SHOW);
  end else
  begin
    Application.Initialize;
    Application.Title := 'Архив';
    Application.CreateForm(TTabloMain, TabloMain);
    Application.CreateForm(TValueListDlg, ValueListDlg);
    Application.CreateForm(TDirectFormDlg, DirectFormDlg);
    Application.Run;
  end;
end.
