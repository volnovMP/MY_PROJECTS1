unit TabloDSP;
{$INCLUDE D:\sapr2012\CfgProject} //--------------------------------- ��������� ����������
{$UNDEF SAVEKANAL}     //-------------------------------- ��������� ������ ������� � �����
//**************************************************************************************\\
//                       ������� ���� ��������� ��-���                                  **
//**************************************************************************************//
interface

uses
  Windows,
  Messages,
  SysUtils,
  Classes,
  Graphics,
  Controls,
  Forms,
  Dialogs,
  ExtCtrls,
  ImgList,
  Registry,
  Menus,
  MMSystem;

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
    ilClock: TImageList;
    Timer1: TTimer;
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
    procedure ASUTimer(Sender: TObject);
    procedure Edit1Change(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X,
      Y: Integer);


  private
    function RefreshTablo : Boolean; //--------------------------- ���������� ������ �����
    procedure SetPlakat(X,Y : integer);
    procedure GetPlakat(X,Y : integer);
    procedure DrawPlakat(X,Y : integer);
  public
    PopupMenuCmd  : TPopupMenu;
    FindCursor    : Boolean; //------------------------------------ �������� ����� �������
    IskCnt        : Integer; //-------------------------------- ������� ��� ������ �������
    IDX        : Integer; //------------------------------ X-�������� ����� ������� ������
    IDY        : Integer; //------------------------------ Y-�������� ����� ������� ������
  end;

var
  TabloMain: TTabloMain;
  RefreshTimeOut : Double;       //--- ������������ ����� �������� ������������� �� ������
  StartTime      : Double;       //--------------------------------- ����� ������� �������
  TimeLockCmdDsp : Double;       //------------- ����� ������������ �������� ������ ������
  IsCloseRMDSP   : Boolean;      //------------ ��������� �������� �������� ���� ���������
  AppStart       : Boolean;      //---------------- ������� ������ �������� ���� ���������
  SendToSrvCloseRMDSP : Boolean; //-- ������� ����������� ������� � ���������� ������ ����
  SendRestartServera  : Boolean; //---------------- ������ ������� �� ������������ �������
  OpenMsgForm         : Boolean; //----------------------- ������ �������� ����� ���������
  Sdvig               : integer; //---------------------------------------- ����� ��������
  GlobusIndex : integer; //

procedure ChangeDirectState(State : Boolean);
procedure ChangeRegion(RU : Byte);
procedure PresetObjParams;
procedure IncrementKOK;

const
  CurTablo1    = 1;
  CurTablo1ok  = 2;
  CurTabloGlas = 3;
  MigInterval : double = 0.5;

  RepFileName = 'Dsp.rpt';
  KeyName : string = '\Software\DSPRPCTUMS';

implementation

uses
  aclapi,
  accctrl,
  TypeALL,
  crccalc,
  Load,
  KanalArmSrvDSP,
  Objsost,
  Commands,
  Marshrut,
  MainLoop,
  CMenu,
  Commons,
  Comport,
  Password,
  PipeProc,
  MsgForm,
  TimeInput,
  ViewOzOv,
  ViewFr;

{$R *.DFM}
{$R CURSOR.RES}

var
  sMsg0,sPar    : string; //---------------- ��������� ���������� ��� ���� ��������� �����
  dMigTablo     : Double; //���������� ��� ������������ ��������� �������� ��������� �����
  LastID_Obj    : integer;
  CntSyncCh     : integer;
  CntSyncTO     : integer;
//  fix           : integer;
  LastKey       : Word;    //------------------------------- ��� ��������� ������� �������
  LastKeyPress  : Double;  //----------------------------- ����� ������� ��������� �������
  lastCurOK     : boolean;
  FixVspPerevod : boolean;


function SetPrivilege(aPrivilegeName : string; aEnabled : boolean ): boolean;
var
  TPPrev, TP : TTokenPrivileges;
  Token      : THandle;
  dwRetLen   : DWord;
begin
  Result := False;
  OpenProcessToken(GetCurrentProcess,TOKEN_ADJUST_PRIVILEGES or TOKEN_QUERY, Token );
  TP.PrivilegeCount := 1;
  if(LookupPrivilegeValue(nil, PChar( aPrivilegeName ), TP.Privileges[ 0 ].LUID ) ) then
  begin
    if( aEnabled )then TP.Privileges[0].Attributes:= SE_PRIVILEGE_ENABLED else TP.Privileges[0].Attributes:= 0;
    dwRetLen := 0;
    Result := AdjustTokenPrivileges(Token,False,TP, SizeOf( TPPrev ), TPPrev,dwRetLen );
  end;
  CloseHandle( Token );
end; 

//========================================================================================
procedure TTabloMain.FormDestroy(Sender: TObject);
begin
  if LoopHandle > 0 then
  begin LoopSync := false; CloseHandle(LoopHandle); end; //������� ����� ������ ���-������

  DateTimeToString(sMsg0, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
  RepF('������������� �� ������   = '+ IntToStr(CntSyncCh));
  RepF('������������� �� �������� = '+ IntToStr(CntSyncTO));
  RepF('���������� ������ ��������� '+ sMsg0);
  InsNewArmCmd($7ffa,0); //-------------------------- ��������� � ������ ���������� ������
  reg.Free;
  DestroyKanalSrv;
  if Assigned(Tablo1) then Tablo1.Free;
  if Assigned(Tablo2) then Tablo2.Free;
  if Assigned(IpWav) then IpWav.Free;
{$IFNDEF DEBUG}
  if ReBoot then
  begin
    if SetPrivilege('SeShutdownPrivilege', true) then
    begin
      if not ExitWindowsEx(EWX_FORCE or EWX_REBOOT, 0) then SimpleBeep;
      SetPrivilege('SeShutdownPrivilege', False);
    end;
    MessageBeep(MB_ICONHAND);
  end;
{$ENDIF}
end;

//========================================================================================
//------------------------------------- ��������� �������� �������� ���� ��������� ��� ���
procedure TTabloMain.FormCreate(Sender: TObject);
  var
    err: boolean;
    i,h,aiElements,aColors : integer;

begin
  Caption := '�����';

  GlobusIndex := 0; //---------------------------------- ������ ��������� �������� �������
  ilGlobus.BkColor := ACVT15; //-------------------------������� ���� �������� �������
  ilClock.BkColor := ACVT15; //--------------------- ������� ���� �������� ������� ���

  LockTablo := false; //----------------------------- ���������� ����� �� ����� ����������
  FormStyle := fsStayOnTop; //-------- ���� ��� ������� - ���������� ���� ������ ���������
  IsCloseRMDSP := false; //---------- ������� "��������� �������� �������� ���� ���������"
  SendToSrvCloseRMDSP := false; //--- ������� "���� ��������� ������� � ����� ������ ����"
  reg := TRegistry.Create; //-------------------------------- ������ ��� ������� � �������
  reg.RootKey := HKEY_LOCAL_MACHINE; //----------------------------- �������� ���� �������
  //---------------------------------------- ��������� ����� ����� ��������� ������ ������
  if FileExists(RepFileName) then    //---------------------RepFileName = 'Dsp.rpt';
  begin
    h := FileOpen(RepFileName,fmOpenRead);
    if h > 0 then
    begin
      i := FileSeek(h,0,2); //------------------------------------------ ����� ����� �����
      if i > 199999 then FileSeek(h,0,0); //-- ���� ���� ������ ���������, ������ � ������
      FileClose(h);//-------------------------------------------------------- ������� ����
    end;
  end;
  DateTimeToString(sMsg0, 'dd/mm/yy h:nn:ss.zzz', Date+Time);
  RepF('@');
  RepF('������ ������ ��������� '+ sMsg0);

  PopupMenuCmd := TPopupMenu.Create(self);
  PopupMenuCmd.AutoPopup := false;
  Sdvig := 0;

  err := false;
  if Reg.OpenKey(KeyName, false) then   //--- ��� ��� ��� KeyName = '\Software\DSPRPCTUMS'
  begin
    if reg.ValueExists('databasepath') then
    database    := reg.ReadString('databasepath')
    else begin err := true; RepF('��� ����� "databasepath"');end;

    if reg.ValueExists('path') then config.path := reg.ReadString('path')
    else begin err := true; RepF('��� ����� "path"'); end;

    if reg.ValueExists('arcpath') then config.arcpath := reg.ReadString('arcpath')
    else begin err := true; RepF('��� ����� "arcpath"'); end;

    if reg.ValueExists('ru') then config.ru := reg.ReadInteger('ru')
    else begin err := true; RepF('��� ����� "ru"'); end;

    if reg.ValueExists('avtodst') then config.avtodst := reg.ReadInteger('avtodst')
    else begin err := true; RepF('��� ����� "avtodst"'); end;

    {
      if (config.ru < 1) or (config.ru > 3) then config.ru := 1;
      DirState[1] := config.ru;
      config.def_ru := config.ru; //------------------------------- ����� �� �������������

      if reg.ValueExists('auto') then config.auto := reg.ReadBool('auto')
      else begin err := true; RepF('��� ����� "auto"'); end;
    }
    if reg.ValueExists('RMID') then config.RMID := reg.ReadInteger('RMID')
    else begin err := true; RepF('��� ����� "RMID"'); end;

    if reg.ValueExists('Master') then config.Master := true  //----------- ������ ���� ���
    else if reg.ValueExists('Slave') then config.Slave := true;

    if reg.ValueExists('KOK') then KOKCounter  := reg.ReadInteger('KOK' ) //----- ���� ���
    else begin err := true; RepF('��� ����� "KOK"'); end;

    if reg.ValueExists('configkanal') then sMsg0 := reg.ReadString('configkanal')
    else begin err := true; RepF('��� ����� "configkanal"'); end;

    KanalSrv[1].config := sMsg0;
    KanalSrv[2].config := sMsg0;

    //------------------------------------------------------------- ����� ��� ����� � STAN
    if reg.ValueExists('namepipein') then KanalSrv[1].nPipe:= reg.ReadString('namepipein')
    else sMsg0 := '';

    if reg.ValueExists('namepipeout')then KanalSrv[2].nPipe:=reg.ReadString('namepipeout')
    else sMsg0 := '';

    if (KanalSrv[1].nPipe = '') and (KanalSrv[2].nPipe = '')  then KanalType := 0
    else
    if (KanalSrv[1].nPipe <> 'null') or (KanalSrv[2].nPipe <> 'null') then KanalType := 1
    else
      begin
        err := true;
        RepF('������� ��������� ��� ������ ����� � ��������');
      end;

    if reg.ValueExists('AnsverTimeOut') then
    AnsverTimeOut := reg.ReadDateTime('AnsverTimeOut')
    else
      begin err := true; RepF('��� ����� "AnsverTimeOut"'); end;

    if reg.ValueExists('RefreshTimeOut') then
      RefreshTimeOut := reg.ReadDateTime('RefreshTimeOut')
    else
      begin err := true; RepF('��� ����� "RefreshTimeOut"'); end;

    if reg.ValueExists('TimeOutRdy') then
      MaxTimeOutRecave := reg.ReadDateTime('TimeOutRdy')
    else
      begin err := true; RepF('��� ����� "TimeOutRdy"'); end;

    if reg.ValueExists('IntervalAutoMarsh') then
      IntervalAutoMarsh := reg.ReadInteger('IntervalAutoMarsh')
    else IntervalAutoMarsh := 15;

    if reg.ValueExists('DiagnozUVK') then DiagnozON := reg.ReadBool('DiagnozUVK')
    else DiagnozON := false;

    if reg.ValueExists('configKRU') then config.configKRU := reg.ReadInteger('configKRU')
    else config.configKRU := 0;

    if reg.ValueExists('N_UVK') then config.N_UVK := reg.ReadInteger('N_UVK')
    else
      begin config.N_UVK :=0; err := true; RepF('��� ����� "N_UVK"'); end;

    if reg.ValueExists('ServerSync') then WorkMode.ServerSync := true
    else WorkMode.ServerSync := false;

    if reg.ValueExists('SetIkonRez') then SetIkonRezNonOK := true;
    if reg.ValueExists('ASUpipe1')  then sMsg0 := reg.ReadString('ASUpipe1')
    else sMsg0 := '';
 {
    if sMsg0  = '' then //------------------------------ ����������� ����� ���1 �� ���������
    begin  DspToDspEnabled := false; nDspToDspPipe := '';  end
    else //---------------------------------------------- ����������� ����� ���1 ���������
    begin
      if sMsg[1] = '0' then DspToDspType := 0 //---------------------���(�������)-- ������
      else
      if sMsg[1] = '1' then DspToDspType := 1 //--------------------���(�������) -- ������
      else
      begin err := true; RepF('�������� �������� ������������ ������ ���1'); SimpleBeep; end;

      if not err then DspToDspEnabled := true;
      nDspToDspPipe := '';
      for i := 3 to Length(sMsg0 ) do nDspToDspPipe := nDspToDspPipe + sMsg[i];
    end;
}
    if reg.ValueExists('ARCpipe')  then sMsg0 := reg.ReadString('ARCpipe')
    else sMsg0 := '';
{
    if sMsg0  = '' then //------------------ ����������� ����� ��������� ������ �� ���������
    begin
      DspToArcEnabled := false;
      nDspToArcPipe := '';
    end else
    begin //------------------------------------------- ����������� ����� ��������� ������
      DspToArcEnabled := true;
      //---------------------------------- ������������ ����� ��� ������ "����� ���-�����"
      nDspToArcPipe := '';
      for i := 1 to Length(sMsg0 ) do nDspToArcPipe := nDspToArcPipe + sMsg[i];
    end;
 }
    //------------------------------------------------------------ ��������������� �������
    if reg.ValueExists('SaveArc') then savearc := true;

    Stol.X := Screen.DesktopWidth; //-------------- �������� ������ �������� ����� Windows
    Stol.Y := Screen.DesktopHeight;

    if reg.ValueExists('kanal1') then
    begin
      i := reg.ReadInteger('kanal1');
      KanalSrv[1].Index := i;
    end  else
    begin
      KanalSrv[1].Index := 0;
      err := true;
      RepF('��� ����� "kanal1"');
    end;

    if reg.ValueExists('kanal2') then
    begin
      i := reg.ReadInteger('kanal2');
      KanalSrv[2].Index := i;
    end  else
    begin
      KanalSrv[2].Index := 0;
      err := true;
      RepF('��� ����� "kanal2"');
    end;

    if reg.ValueExists('cur_id') then //-------- ��������� �������� ����� ��������� ������
    config.cur_id := reg.ReadInteger('cur_id')
    else config.cur_id := 0;//------------------ ��������� �������� ����� ��������� ������
    reg.CloseKey;

    if not FileExists(database) then
    begin
      err := true;
      RepF('���� ������������ ���� ������ ������� �� ������.');
    end;
  end else
  begin
    RepF('��� ����� "DSPRPCTUMS"');
    ShowMessage('���������� ������ ��-�� ����������� ������ ��� ������������� ���������. (������ ���� ������ DSP.RPT)');
    Application.Terminate;
    exit;
  end;
  Left := 0;
  Top := 0;
  mem_page := false;


  if not InitpSD then //------------ ���� �� ������� ������� ������ ��������� ������������
  begin
    err := true;
    RepF('������ ������������� ��������� ������������');
    SimpleBeep;
  end;

  if not InitEventPipes then //-- ������� ������� R/W � ������ � ������.��������� ��� ����
  begin err := true; RepF('������ ������������� ��������� ��������'); SimpleBeep; end;

  CreateKanalSrv; //-- ������� 1-� � 2-� ������ ����� � �������� ����� ���-����� ��� �����
  InitKanalSrv(1); //----------------------- ������������ ����� 1 ����� ���-���� ��� �����
//  InitKanalSrv(2);

  Tablo1 := TBitmap.Create;  //------------------------- ������� ������� ����� ��� ����� 1
  Tablo2 := TBitmap.Create;  //------------------------- ������� ������� ����� ��� ����� 2
  ImageList.BkColor   := bkgndcolor; //----------------- ���������� ���� ���� ��� ��������
  ImageListRU.BkColor := bkgndcolor; //-------- ���������� ���� ���� ��� ������ ����������

  screen.Cursors[curTablo1]   := LoadCursor(HInstance, IDC_ARROW);//-- ����������� �������
  screen.Cursors[curTablo1ok] := LoadCursor(HInstance, 'CURSOR1OK'); //- ���� ������� � ��

  //--------------------------------------------------------- �������� ��� ������ ���-����
  DspMenu.Ready := false;
  DspMenu.WC := false;
  DspMenu.obj := -1;

  //---------------------------------------------- �������� ��� ��������� � ��������������
  FixMessage.Count:= 0; FixMessage.ActLine:= 1; FixMessage.StartLine:= 1;

  //------------------------------- ���������� ��������� ���������������� �������� �������
  VspPerevod.Active := false;
  SyncTime := false;
  OperatorDirect := 0;
  StartObj := 1;
  cntObjZav := 1; cntObjView := 1; cntUpr := 1; cntOVBuffer := 1;
  ObjHintIndex := 0;
  LockHint := false;

  StartRM := true; //-------------------------------- ����������� ��������� ������ �������

  //------------------------------------------------------- ������� ������ �������� ������
  ObjWav[1] := PAnsiChar(config.path+'media\sound1.wav');
  ObjWav[2] := PAnsiChar(config.path+'media\sound2.wav');
  ObjWav[3] := PAnsiChar(config.path+'media\sound3.wav');
  ObjWav[4] := PAnsiChar(config.path+'media\sound4.wav');
  ObjWav[5] := PAnsiChar(config.path+'media\sound5.wav');
  ObjWav[6] := PAnsiChar(config.path+'media\sound6.wav');

  IpWav := TStringList.Create; //--- ������� ������ ��� ������ ������ �������� �����������
  IpWav.Add(config.path+'media\ip1.wav');
  IpWav.Add(config.path+'media\ip2.wav');

  //----------------------------------------------------------------- �������� ���� ������
  if not LoadBase(database) then err := true;
  if (Stol.X < configru[config.ru].T_S.X) or (Stol.Y < configru[config.ru].T_S.Y) then
  begin //------------- ���� ��������� �� ���������� ��� ���� ��-��� - ��������� �� ������
    RepF('������ ����� ['+ IntToStr(configru[config.ru].T_S.X)+ 'x'+
    IntToStr(configru[config.ru].T_S.Y)+'] ������ ������� �������� ����� Windows!');
    err := true;
  end;
  SetParamTablo; //---------------------------------------------- ���������� ������� �����

  //--------------------------------------------------- �������� �������� ��������� ��-���
  if not LoadLex(config.path + 'LEX.SDB')   then err := true;
  if not LoadLex2(config.path + 'LEX2.SDB') then err := true;
  if not LoadLex3(config.path + 'LEX3.SDB') then err := true;
  if not LoadMsg(config.path + 'MSG.SDB')   then err := true;

  //------ �������� ��������� ���� - ����������� ����� ������������� �������� ������������
  if not LoadAKNR(config.path + 'AKNR.SDB') then err := true;

  GetMYTHX;//---------------------------------------------- ��������� ������� �������� MYT

  //--------------------------------------- ���������� ��������� ������ ������ ������ ����
  ResetTrace;

  StateRU  := 0; //------------------------------------- �������� ��������� ��� � ��������
  ArmState := 0; //------------------------------------- �������� ��������� ��� � ��������
  WorkMode.RazdUpr := false;//-------------------------- �������� ���������� �� ����������
  WorkMode.MarhUpr := true; //----------------------------- �������� ���������� ����������
  WorkMode.MarhOtm := false; //--------------------------------------- ��� ������ ��������
  WorkMode.VspStr  := false; //--------------------- ��� ���������������� �������� �������
  WorkMode.InpOgr  := false; //------------------------------------- ��� ����� �����������
  WorkMode.OtvKom  := false; //--------------------------- ��� ������ ������������� ������
  WorkMode.Podsvet := false; //--------------------------- ��� ��������� ��������� �������
  WorkMode.GoTracert  := false; //--------------------------------- ��� ������ �����������
  WorkMode.GoOtvKom   := false;//��� ���������� ��������������� ���� ������������� �������
  WorkMode.GoMaketSt  := false;//-------------------------- ��� ��������� ������� �� �����
  WorkMode.Upravlenie := false;//----------------------------- ��� �� �������� �����������
  WorkMode.LockCmd    := true; //------------------------ ��� ���������� ���������� �� ���

  MsgFormDlg   := TMsgFormDlg.Create(nil); //-------- ������� ����� ��� ���������� �������
  TimeInputDlg := TTimeInputDlg.Create(nil); //----------- ������� ����� ��� ����� �������
  PasswordDlg  := TPasswordDlg.Create(nil);  //------------ ������� ����� ��� ����� ������
  //--------------------------------------------------- ���������� ������� ������ ��������
  PasswordPos.X := configRU[config.ru].MsgLeft+1;//--------------- ������� ���� ��� ������
  PasswordPos.Y := configRU[config.ru].MsgTop+1;
  TimeInputPos.X := configRU[config.ru].MsgLeft+1; //------ ������� ���� ��� ����� �������
  TimeInputPos.Y := configRU[config.ru].MsgTop+1;

  //hWaitKanal := CreateEvent(nil,false,false,nil);  //----- ������� ������� �������� ������
  IsBreakKanalASU := false; //------- �������� ������� ���������� ������������ ������� ���

  if err then //-------���� �������� ������ ��� ������ ������� ��� ������������� ���������
  begin
    ShowMessage('����� ������ ��-�� ������ ��� �������������.������ ���� ������ DSP.RPT');
    Application.Terminate;
  end;

  aiElements := COLOR_MENU;
  aColors := ACVT7;
  SetSysColors(1,aiElements,aColors);
  AppStart := true;
end;

//========================================================================================
//---------------------------------------------------------------------- ����������� �����
procedure TTabloMain.FormActivate(Sender: TObject);
  var Dummy : Cardinal;
begin
  if not AppStart then exit; // ���� ��� �������� ������ �������� ���� ���������, �� �����
  InsNewArmCmd($7ffb,0); //--------- ����� ������ � ����� ��� $7ffb (������ ������ �� ���)
  ShowWindow(Application.Handle,SW_HIDE);//----------------------------------- ������ ����
  AppStart := false; //-------------------------------- �������� ������� ������ ����������
  //------------------------------------------------ ��������� ������������� �������� ����
  PresetObjParams; //----- ���������� ��������� �������� ������������ � �������� ���������
  Screen.Cursor := curTablo1; //-------------- ���������� ������ �������� ���� (���������)
  FindCursor := false; //-------------------------------- ��������� ������� ������ �������
  IskCnt := 0; //---------------------------------- �������� ������� ������ �������
  DrawTablo(Tablo1); //------------------------------------------------ ���������� ����� 1
  DrawTablo(Tablo2); //------------------------------------------------ ���������� ����� 2

  if config.cur_id = 1 then //------------------------------ ���� ������ ����� ������� ���
  begin
  //-------------------------------------------- ��������� ���������� �� �������� ��������
    if FileExists('c:/0000000-1a47jdv-kbmndws.ini') then //���� ���������� ����� ���� �� �:
    begin //------------------------------ �������� ������������� ������ �������� ��������
      asTestMode := $aa;
    end
    else asTestMode := $55;
  end
  else
  asTestMode := $55;
  //--------------------------------------------------------------------------------------
  if DspToDspEnabled then
  begin //------------------------------ ����� �������� ��������� ������������ ������ ���1
    case DspToDspType of //------------------------- ������������� ����������� ���� �� ���
      1 :                //---------------------------------------- ���� ������ ���-������
        DspToDspThread := //----------------------------- ������� ����� ��� ������ �������
        CreateThread(nil,0,@DspToDspClientProc,DspToDspParam,0,DspToDspThreadID);//������1
      else              //----------------------------------------- ���� ������ ���-������
        DspToDspThread := //----------------------------- ������� ����� ��� ������ �������
        CreateThread(nil,0,@DspToDspServerProc,DspToDspParam,0,DspToDspThreadID);//������1
    end;
    if DspToDspThread = 0 then RepF('������ �������� �������� ��������� ������ ���1.')
    else RepF('������ ��������� ������ ���1.');
  end;


  if DspToArcEnabled then  //------------------------ ���� ��������� ������� ��� � �������
  begin //------------------ ����� �������� ��������� ������������ ������ ��������� ������
    DspToArcThread := //------------------------------------------------------ ������ ���1
    CreateThread(nil,0,@DspToARCProc,DspToArcParam,0,DspToArcThreadID);
    if DspToArcThread = 0 then
      RepF('������ �������� �������� ��������� ������ ��������� ���������� ������.')
    else
      RepF('������ ��������� ������ ��������� ���������� ������.');
  end;

  //- ���������� ������������� ��������� ������� ������ ��� � ����������� �� ���������� ��
  if config.Master then ASU.Interval := 799 else  //---------- ���� ��� ������� � ���� ���
  if config.Slave then ASU.Interval := 1099; //-------------------------- ���� ��� �������

  MainTimer.Enabled := true; //---------------------------------- ��������� ������� ������
  LastRcv := Date+Time; //----------------------------- ��������� ������ ���������� ������
  dMigTablo := Date+Time;  //------------------------- ��������� ������ ���������� �������
  { //---------------------------------------------------- ��� ������ � ��������� ��������
  if config.auto then
  SendCommandToSrv(WorkMode.DirectStateSoob,_directdef,0);//���������� �� ����������
   }
  CmdSendT := Date + Time;   // ������ ������ ������� ����� ������� ����������� ����������
  StartTime := CmdSendT;     //------------------------------------- ������ ������� ��-���
  LastReper := Date + Time;  //------------ ������ ������ 10-�� �������� ������� ���������
  SendRestartServera := false; //------------------------- ����� �������� �������� �������
  LockTablo := false;   //--------------------- ����� ���������� ����� �� ����� ����������
  LockComDsp := false;//--- ����� ����. �����.��������� �� ����� ���������� ��������������

  //---------------- ������� ����� ������������ ������ ��� - ������ � ������ �������������

  LoopSync := true;  //--------- ���������� ������� ������������ ������� ������ � ��������

  //------------------------------------------- ������� �����, ������ ������������ �������
  LoopHandle := CreateThread(nil,0,@SyncReadyThread,nil,0,Dummy);
  if LoopHandle > 0 then  //------------------ ���� ����� ������ ������ � ������� � ������
  begin
    RepF('����� ��������� ������ ���-������ �������. ThreadID = '+IntToStr(Dummy));
    ConnectKanalSrv(1); //-------------------------------- ������� ����������� �� ������ 1
  end else
  begin
    RepF('������ ������������� ������ ������ ���-������.��������� ���������� ���������.');
    Application.Terminate;
  end;
  ReBoot := true;
end;

//========================================================================================
//-------- ��������� ���������� � ������� � ��������������� �������� ����� ��������� �����
procedure TTabloMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
var
  ec : cardinal;
begin
  if (WorkMode.Upravlenie and ((StateRU and $40) = $40)) or WorkMode.PushRU
  then
  begin
  //-- ��� ���������� ���������� � ���� � ���������� ������� �� ��������� ������ ���������
    if (LastRcv + MaxTimeOutRecave) > LastTime then
    begin CanClose := false;  exit; end;
  end;

  if IsCloseRMDSP then
  begin //----------------------------------- �������� ���������� ���������� ������ ��-���
    DisconnectKanalSrv(1);
    //DisconnectKanalSrv(2);
    MainTimer.Enabled := false; //------------------------------ ���������� ������� ������
    BeepTimer.Enabled := false; //-------------------- ���������� ������ �������� ��������

    if DspToDspEnabled then
    begin
      if GetExitCodeThread(DspToDspThread,ec) then //---- ��������� ����� ������ � �������
      begin
        if ec=STILL_ACTIVE then canClose:=false //�� ���������,���� �������� ����� ���-���
        else canClose := true;
      end
      else canClose := false;
    end
    else canClose := true;

    if canClose then
    begin
      if DspToArcEnabled then
      begin
        if GetExitCodeThread(DspToArcThread,ec) then //-- ��������� ����� ������ � �������
        begin
          if ec=STILL_ACTIVE
          then canClose:=false;//-------------- �� ��������� ���� �������� ����� ���-�����
        end
        else canClose := false;
      end;
    end;

  // MainLoopState := 1; //---------------------- ��������� ��������� �������� ����� ?????
    ReBoot := not CanClose;
    exit;
  end
  else
  if not SendToSrvCloseRMDSP then //------------------ ���� ������� �� ����������� �������
  begin
    ShowWindow(Application.Handle,SW_SHOW); // �������������� � ���������� ���� ����������
    if PasswordDlg.ShowModal = mrOk then
    begin
      InsNewMsg(0,89,7,''); //----------------------- "��������� ���������� ������ �� ���"
      showSMsg (89,LastX,LastY,'');
      SendCommandToSrv(WorkMode.DirectStateSoob,_logoff,0);  //--- ���������� ������
      SendToSrvCloseRMDSP := true; //------- ��������� �������� �������� � ��������� �����
      StopAll := 15;
    end;
    ShowWindow(Application.Handle,SW_HIDE);
  end;
  CanClose := false;
end;

procedure TTabloMain.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
  if mem_page then Canvas.Draw(-Sdvig,0,tablo1) else Canvas.Draw(-Sdvig,0,tablo2);
end;

//========================================================================================
//---------------------------------------------- ���������� �� ������ ��������� ����������
procedure TTabloMain.FormPaint(Sender: TObject);
var
  i,x,y : integer;
  p : TPoint;
  OldColor : TColor;
  OldStyle : TFontStyles;
  n : Boolean;
begin
  n := false;

  for i := 1 to 20 do if stellaj[i] then begin n := true; break; end;

  with canvas do
  begin
    if IkonkaMove or n then
    begin GetCursorPos(p);DrawFocusRect(rect(p.X+IDX,p.Y+IDY,p.X+12+IDX,p.Y+12+IDY));end;

    //------------------------------------------------------ ���������� ������ �� ��������
    if (cur_obj > 0) and (cur_obj < 20000) and (ObjUprav[cur_obj].MenuID > 0) then
    begin
      Pen.Color := clRed; Pen.Mode := pmCopy; Pen.Width := 1;
      with ObjUprav[cur_obj].Box do
      begin
        MoveTo(Left - Sdvig,Top);
        LineTo(Right - Sdvig,Top);   LineTo(Right - Sdvig,Bottom);
        LineTo(Left  - Sdvig,Bottom);LineTo(Left  - Sdvig,Top);
      end;
    end;
  end;

  with ObjUprav[cur_obj] do
  begin
    if not LockHint then
    begin
      //----------------------------------------------------------- ����� �������� �������
      if ((LastTime - LastMove) > (1/86400)) and  (Hint <> '') and (ObjHintIndex = 0) then
      begin
        ObjHintIndex := cur_obj;  i := LastX div configru[config.ru].MonSize.X + 1;
        sMsg[i] := Hint;     sMsgCvet[i] := bkgndcolor;
      end else
      if (ObjHintIndex > 0) and ((LastTime - LastMove) > (30/86400)) then
      begin
        if tab_page then begin RSTMsg;ObjHintIndex:=cur_obj; end else
        begin i:=LastX div configru[config.ru].MonSize.X+1;sMsg[i]:=Hint;
          sMsgCvet[i] := bkgndcolor;
        end;
      end;
    end;
  end;

  y := configRU[config.ru].T_S.Y;  x := configRU[config.ru].MonSize.X;
  if configRU[config.ru].T_S.X < x then x := configRU[config.ru].T_S.X;


  with canvas do
  begin
    OldColor := Font.Color; OldStyle := Font.Style;  Font.Style := [fsBold];

    for i := 1 to High(sMsg) do
    begin
      Brush.Style := bsSolid;
      if sMsg[i] <> '' then //--------------------- ������� �������� ��������� ��� �������
      begin
        Brush.Color := sMsgCvet[i]; if Brush.Color = 0 then Brush.Color := bkgndcolor;
        if Brush.Color <> 255 then Font.Color := clBlack else Font.Color := clWhite;
        FillRect(rect((i-1)*X, Y-20, i*X-32, Y));
        Font.Height := 20; TextOut((i-1)*X+3, Y-20,sMsg[i]);//------------ ����� ���������

        if (ObjHintIndex > 0) and ((LastTime - LastMove) < (30/86400)) then
        begin ImageList16.Draw(canvas,i*X-16, Y-15,1); end  else
        if ShowWarning then
        begin
          if tab_page then ImageList16.Draw(canvas,i*X-16, Y-15,5)
          else ImageList16.Draw(canvas,i*X-16, Y-15,0);
        end else begin ilGlobus.Draw(canvas, i*X-16, Y-15, GlobusIndex); end;
      end else
      begin
        Brush.Color := bkgndcolor;  FillRect(rect((i-1)*X, Y-15, i*X-32, Y));
        Brush.Color := clWhite;     FillRect(rect(i*X-32, Y-15, i*X-16, Y));
        Brush.Color := clRed;       FillRect(rect(i*X-31, Y-14, i*X-27, Y-1));
        Brush.Color := clGreen;     FillRect(rect(i*X-26, Y-14, i*X-22, Y-1));
        Brush.Color := clBlue;      FillRect(rect(i*X-21, Y-14, i*X-17, Y-1));
        ilGlobus.Draw(canvas, i*X-16, Y-15, GlobusIndex);
      end;
    end;

    Font.Color := OldColor;
    Font.Style := OldStyle;
  end;

  //--------------------------------------------------------------- ����� ������� �� �����
  if FindCursor then
  with canvas do
  begin
    GetCursorPos(p); //------------------- �������� ������� ������� � ������� ����� p(x,y)
    Pen.Color := clWhite; Pen.Width := 3; Pen.Mode := pmNotMask; Pen.Style := psSolid;
    Brush.Style := bsClear;
    if IskCnt >= 100 then
    begin //------------------------------------------------------------------- ��� ������
      Ellipse(p.X-IskCnt,p.Y-IskCnt,p.X+IskCnt,p.Y+IskCnt); IskCnt := IskCnt - 100;
      Ellipse(p.X-IskCnt,p.Y-IskCnt,p.X+IskCnt,p.Y+IskCnt); IskCnt := IskCnt - 100;
      Ellipse(p.X-IskCnt,p.Y-IskCnt,p.X+IskCnt,p.Y+IskCnt); IskCnt := IskCnt - 100;
    end;
    if IskCnt< 100 then begin FindCursor:=false; IskCnt := 0; end;
  end;
end;

//========================================================================================
//------------------------------------ ������� ���������� ������ ����� ��� ������ �� �����
procedure TTabloMain.DrawTablo(tablo: TBitmap);
var
  i,x,y,c : integer;
  OldColor : TColor;
begin
  if not Assigned(tablo) then
  begin
    RepF('�� ��������������� ��������� ����� ������� ��������� [DrawTablo]');
    Application.Terminate;
    exit;
  end;

  Tablo.Canvas.Lock; //--------------------------- ��������� ����� ����� �� ������ �������

  with tablo.Canvas do
  begin
    Brush.Color := bkgndcolor; Brush.Style := bsSolid;
    FillRect(rect(0, 0, tablo.width, tablo.height));

    //************************ ���� ���� ��������� ��� ������� �������� ���� *************
    if FixMessage.Count > 0 then
    begin
      //----------------------------------------------- ���������� ������������� ���������
      x := configRU[config.ru].MsgLeft; y := configRU[config.ru].MsgTop;
      c := FixMessage.Count - FixMessage.StartLine;  Font.Size := 8;
      if c > 4 then c := 4;

      //--------------------------------- ����� �� 1 �� 4-� ��������� � ���� ������ ������
      for i := FixMessage.StartLine to FixMessage.StartLine + c do
      begin
        Font.Color := FixMessage.Color[i];
        if i=FixMessage.ActLine then Brush.Color:= focuscvt else Brush.Color:= bkgndcolor;
        FillRect(rect(x,y,configRU[config.ru].MsgRight,y+16));
        TextOut(x+2,y,FixMessage.Msg[i]);
        y := y + 16;
      end;

      if FixMessage.Count > 5 then
      begin //------------------------ ���������� ������ ��������� ������������� ���������
        Brush.Color := bkgndcolor;
        Rectangle(configRU[config.ru].MsgRight-10, configRU[config.ru].MsgTop,
        configRU[config.ru].MsgRight, configRU[config.ru].MsgBottom);

        if FixMessage.StartLine > 1 then
        begin
          Polygon([Point(configRU[config.ru].MsgRight-9,configRU[config.ru].MsgTop+10),
          Point(configRU[config.ru].MsgRight-1, configRU[config.ru].MsgTop+10),
          Point(configRU[config.ru].MsgRight-5, configRU[config.ru].MsgTop+1)]);
        end;

        if (FixMessage.Count - FixMessage.StartLine) > 4 then
        begin
          Polygon([Point(configRU[config.ru].MsgRight-9,configRU[config.ru].MsgBottom-10),
          Point(configRU[config.ru].MsgRight-1,configRU[config.ru].MsgBottom-10),
          Point(configRU[config.ru].MsgRight-5,configRU[config.ru].MsgBottom-1)]);
        end;
      end;
    end;

    //------------------------------------------------------- ���������� ����� �� ��������
    Pen.Color := ACVT8;  Brush.Color := ACVT18;  Pen.Style := psSolid;
    Canvas.Pen.Width := 2;

    Rectangle(configRU[config.ru].BoxLeft,configRU[config.ru].BoxTop,
    configRU[config.ru].BoxLeft+12*20+7,configRU[config.ru].BoxTop+16);

    for i := 0 to 19 do
    begin
      x := 1 + i * 12; if i > 10 then x := x + 3;
      case i of
        1 : y := 14;  2 : y := 15;  3 : y := 16;  4 : y := 17;  5 : y := 18;
        6 : y := 19;  7 : y := 20;  8 : y := 21;  9 : y := 22; 10 : y := 23;
       11 : y := 4;  12 : y := 5;  13 : y := 6;  14 : y := 7;  15 : y := 8;
       16 : y := 9;  17 : y := 10; 18 : y := 11; 19 : y := 12;
       else y := 13;
      end;
      ImageList.Draw(Tablo.Canvas,configRU[config.ru].BoxLeft+x,
      configRU[config.ru].BoxTop+1,y, Stellaj[i+1]);
    end;

    //----------------------------------------------------------- ���������� ������ � ����
    case config.ru of
      1 :
      begin //------------------------------------------------------------------------ RU1
        for i := 1 to High(Ikonki) do
        begin
          if (Ikonki[i,1] > 0) then
          ImageList.Draw(Tablo.Canvas,Ikonki[i,2],Ikonki[i,3],Ikonki[i,1],true);
        end;
      end;

      2 :
      begin //------------------------------------------------------------------------ RU2
        for i := 1 to High(Ikonki2) do
        begin
          if (Ikonki2[i,1] > 0) then
          ImageList.Draw(Tablo.Canvas,Ikonki2[i,2],Ikonki2[i,3],Ikonki2[i,1],true);
        end;
      end;
    end;

    
    //�� �� ������ � �������� ������������� WinXP ���������� ����������� ��������� ��������:
    Tablo.Canvas.Brush.Style:= bsClear;
    Tablo.Canvas.Font.Color:= clRed; Tablo.Canvas.Font.Color := clBlack;
    //------------------------------------------ ����� ���������, ����������� ������ WinXP

    //---------------------------------------- ���������� ���� ������������ �������� �����
    c := 0;
    for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
    begin
      if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 0)
      then DisplayItemTablo(i, Tablo.Canvas);
      inc(c);
      if c > 300 then
      begin
        SyncReady;
        WaitForSingleObject(hWaitKanal,INFINITE); //ChTO);
        c := 0;
      end;
    end;

    for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
    begin
      if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 1) then
      DisplayItemTablo(i, Tablo.Canvas);
      inc(c);
      if c > 300 then
      begin
        SyncReady;
        WaitForSingleObject(hWaitKanal,ChTO);
        c := 0;
      end;
    end;

    for i := configRU[config.RU].OVmin to configRU[config.RU].OVmax do
    begin
      if (ObjView[i].TypeObj > 0) and (ObjView[i].Layer = 2) then
      DisplayItemTablo(i, Tablo.Canvas);
      inc(c);
      if c > 300 then
      begin
        SyncReady;
        WaitForSingleObject(hWaitKanal,ChTO);
        c := 0;
      end;
    end;

    //---------------------------------------------------------- ���������� ������ �������
    Pen.Color := clDkGray; Pen.Width := 1; Brush.Style := bsClear; Pen.Style := psSolid;
    Rectangle(configRU[config.ru].MsgLeft, configRU[config.ru].MsgTop,
    configRU[config.ru].MsgRight, configRU[config.ru].MsgBottom);
    Pen.Color := ACVT12;  Pen.Width := 2;

    for i := 1 to (configRU[config.ru].T_S.X div configRU[config.ru].MonSize.X)-1 do
    begin
      MoveTo(i*configRU[config.ru].MonSize.X,0);
      LineTo(i*configRU[config.ru].MonSize.X,configRU[config.ru].MonSize.Y);
    end;
    Pen.Color := clDkGray; Pen.Width := 1;
    MoveTo(0,Tablo.Height-1); LineTo(Tablo.Width,Tablo.Height-1);
    UnLock;
  end;
end;

//========================================================================================
//----------------------------------------- ��������� ��������� ������� ������ ������ ����
procedure TTabloMain.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  GetPlakat(X,Y); //----------------- �������� ��������� ������������� ������� (���� ����)
  OperatorDirect := LastTime + 1/86400;
  if MarhTrac.TraSRazdel then ResetTrace;//----- �������� ������ ����������� ������
end;

//=======================================================================================
//------------------------------------------ ��������� ��������� ����������� ������� �����
procedure TTabloMain.FormMouseMove(Sender: TObject; Shift: TShiftState; X, Y: Integer);
var
  i,o: integer;
begin
  OperatorDirect := LastTime + 1/86400;

  if PopupMenuCmd.PopupComponent <> nil then exit;// �������� ���� �� ������ �������� ����

  if IkonkaDown > 0 then   //------------------------- ���� ������� ������ ��� �����������
  if not SetIkonRezNonOK and //------- ���� ��������� ��������� ������ �� ��������� ���� �
  not WorkMode.OtvKom and //----------------------- ��� �� � ������ ������������� ������ �
  not WorkMode.Upravlenie then //------------------ ��� �� � ������ ��������� ������������
  begin //---------------------------------------------- ��� ��������� � �������� ��������
    for i := 1 to High(Stellaj) do stellaj[i] := false;
    IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0;
    IDX := 0;
    IDY := 0;
  end
  else  IkonkaMoved := true; //--------------------------- ����� ��������� �������� ������

  if ObjHintIndex > 0 then //------------------------ ���� ����� ������ �������� �� ������
  begin RSTMsg; ObjHintIndex := 0; end;

  LastMove := Date+Time; //----------------------------------- ��������� ����� �����������
  LastX := x;   LastY := y;
  o := cur_obj;//---------------------------------------------- ����� ������� ������������
  cur_obj := -1; //-------------------------------------------- ����� ������� ��� ��������

  //-------------------------------------------------- ������ �� ���� ����������� ��������
  for i := configRU[config.ru].OUmin to configRU[config.ru].OUmax do
  with ObjUprav[i] do
  begin //--------------------------------------------- ���� ������ � ���� ������� �������
    if(x+Sdvig>= Box.Left) and (y >=Box.Top) and (x+Sdvig<= Box.Right) and (y<=Box.Bottom)
    then
    begin
      cur_obj := i;
      ID_obj := ObjUprav[i].IndexObj;
      ID_menu := ObjUprav[i].MenuID;
      break;
    end;
  end;

  if o <> cur_obj then
  begin
    canvas.Pen.Width := 1;
    if (o > 0) and (o < 20000) then
    with canvas do  //---------------- ������� ������������� � �������� ������� ����������
    begin
      with ObjUprav[o] do
      begin
        Pen.Color := bkgndcolor; Pen.Mode := pmCopy;
        MoveTo(Box.Left -Sdvig, Box.Top);
        LineTo(Box.Right-Sdvig, Box.Top);    LineTo(Box.Right-Sdvig, Box.Bottom);
        LineTo(Box.Left -Sdvig, Box.Bottom); LineTo(Box.Left -Sdvig, Box.Top);
      end;
    end;

    if (cur_obj > 0) and (ObjUprav[cur_obj].MenuID > 0) then
    with canvas do //---------------- ���������� ������������� �� ����� ������� ����������
    begin
      with ObjUprav[cur_obj] do
      begin
        Pen.Color := clRed; Pen.Mode := pmCopy;
        MoveTo(Box.Left -Sdvig, Box.Top);
        LineTo(Box.Right-Sdvig, Box.Top);    LineTo(Box.Right-Sdvig, Box.Bottom);
        LineTo(Box.Left -Sdvig, Box.Bottom); LineTo(Box.Left -Sdvig, Box.Top);
      end;
    end else  begin  ID_obj := -1;  ID_menu := -1; end;
  end;
end;

//========================================================================================
//------------------------------------------------- ��������� ������� ������ ������ � ����
procedure TTabloMain.DspPopupHandler(Sender: TObject);
var
  i : integer;
begin
  with Sender as TMenuItem do
  begin
    for i := 1 to Length(DspMenu.Items) do
    if DspMenu.Items[i].ID = Command then
    begin
      DspCom.Com := DspMenu.Items[i].Command;
      DspCom.Obj := DspMenu.Items[i].Obj;
      DspCom.Active  := true;
      SelectCommand;

      if cur_obj > 0 then
      SetCursorPos(ObjUPrav[cur_obj].Box.Right-2, ObjUPrav[cur_obj].Box.Bottom-2);
      exit;
    end;
  end;
end;

//========================================================================================
procedure TTabloMain.Edit1Change(Sender: TObject);
begin
{$IFDEF TEST_DANN}
  testObj :=  StrToInt(TabloMain.Edit1.Text);
{$endif}
end;


//========================================================================================
//------------------------------------------------------------- ������ � �������� �� �����
procedure TTabloMain.SetPlakat(X,Y : integer);
  var i,j,dx : integer; uu : boolean;
begin
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then
  begin
    for i := 1 to High(Stellaj) do stellaj[i] := false;
    IkonkaMove := false;
    IkonkaMoved := false;
    IkonkaDown := 0;
    IDX := 0;
    IDY := 0;
    exit;
  end;
  uu := false;

  for j := 0 to 19 do
  begin
    dx := j * 12 + 1; if j > 10 then dx := dx + 3;
    if (X >= configRU[config.ru].BoxLeft+dx) and
    (Y >= configRU[config.ru].BoxTop) and
    (X < configRU[config.ru].BoxLeft+dx+12) and
    (Y < configRU[config.ru].BoxTop+13) then
    begin
      for i := 1 to 20 do
      if i <> (j+1) then stellaj[i] := false
      else stellaj[i] := not stellaj[i];
      uu := true;
      IDX := 0;
      IDY := 0;
    end;
  end;
  if ((X+12+IDX) >= configRU[config.ru].BoxLeft) and
  ((Y+12+IDY) >= configRU[config.ru].BoxTop) and
  ((X+IDX) < configRU[config.ru].BoxLeft+12*20+7) and
  ((Y+IDY) < configRU[config.ru].BoxTop+16) then
  begin //------------------------------ ��������� ���� ���������������� ����� � ���������
    uu := true;
    for i := 1 to 20 do stellaj[i] := false;
  end;
  if not uu then
  begin
    //----------------------------------------------------------------- ����� ���� �������
    j := 0;

    for i := 1 to High(Stellaj) do //------------------------ �������� �� ������� ��������
    if stellaj[i] then //---------------------------- ���� ���� ���������� ��������-������
    begin
      j := i;  //-------------------------------------- �������� ����� ����������� �������
      break;
    end;

    if j > 0 then //------------------------------------------- ���� ���� ��������� ������
    begin //------------------------------------------------------------ ���������� ������
      case config.ru of
        1 :
        begin //---------------------------------------------------------------------- RU1
          for i := 1 to High(Ikonki) do  //----- ���� � ������� ������ ��������� ���������
          begin
            if Ikonki[i,1] = 0 then
            begin //-------------------------------------------- ��������� ������ �� �����
              IkonNew := true;
              IkonSend := true;
              case j of         //--------------------- ������� ��� ����������� ��� ������
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
              Ikonki[i,2] := X; Ikonki[i,3] := Y; //-------------------- ������ ����������
              uu := true;
              break;
            end;
          end;
        end;

        2 :
        begin //---------------------------------------------------------------------- RU2
          for i := 1 to High(Ikonki) do
          begin
            if Ikonki2[i,1] = 0 then
            begin //-------------------------------------------- ��������� ������ �� �����
              IkonNew := true;
              IkonSend := true;
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
              Ikonki2[i,2] := X; Ikonki2[i,3] := Y;
              uu := true;
              break;
            end;
          end;
        end;
      end;
      if not uu then SimpleBeep;
    end;

    for i := 1 to High(Stellaj)
    do stellaj[i] := false; //--------------------------------------------- ����� �� �����
  end;
end;

//========================================================================================
procedure TTabloMain.Timer1Timer(Sender: TObject);
begin
  vremia_zapisi := true;
end;

//========================================================================================
procedure TTabloMain.GetPlakat(X,Y : integer);
var
  i,j,dx : integer;
  uu : boolean;
begin
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then
  begin
    for i := 1 to High(Stellaj) do stellaj[i] := false;
    IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0;
    IDX := 0; IDY := 0;
    exit;
  end;
  uu := false;
  for j := 0 to 19 do
  begin //---------------------------- ��������� ���� ���������������� ����� � ���������
    dx := j * 12 + 1;
    if j > 10 then dx := dx + 3;
    if (X >= configRU[config.ru].BoxLeft+dx) and
    (Y >= configRU[config.ru].BoxTop) and
    (X < configRU[config.ru].BoxLeft+dx+12) and
    (Y < configRU[config.ru].BoxTop+13) then
    begin  //---------------------------------- ���� �� ������ ������ �� ��������-������
      for i := 1 to 20 do //------------------ �������� ��� �� ��������� ������ ��������
      if i <> (j+1) then stellaj[i] := false
      else stellaj[i] := not stellaj[i];   //---------- ��������� �������� "�����������"
      uu := true;   //------------------------------- ���������� ������� ������ ��������
    end;
  end;

  if not uu then  //-------------------------------------- ���� �� ���� ������� ��������
  begin
    case config.ru of
      1 :
      begin //---------------------------------------------------------------------- RU1
        for i := High(Ikonki) downto 1 do   //----------- �������� �� ������ ���� ������
        if Ikonki[i,1] > 0 then
        begin
          if (Ikonki[i,2] <= X) and (Ikonki[i,2]+12 >= X) and
          (Ikonki[i,3] <= Y) and (Ikonki[i,3]+12 >= Y) then//������ ����� ������ �� ������
          begin
            IkonkaDown := i;
            IkonkaMove := true;
            IkonkaMoved := false;
            IDX := Ikonki[i,2] - X;
            IDY := Ikonki[i,3] - Y;
            break;
          end;
        end;
      end;

      2 :
      begin //---------------------------------------------------------------------- RU2
        for i := High(Ikonki2) downto 1 do
        if Ikonki2[i,1] > 0 then
        begin
          if (Ikonki2[i,2] <= X) and (Ikonki2[i,2]+12 >= X) and
          (Ikonki2[i,3] <= Y) and (Ikonki2[i,3]+12 >= Y) then
          begin
            IkonkaDown := i;
            IkonkaMove := true;
            IkonkaMoved := false;

            IDX := Ikonki2[i,2] - X;
            IDY := Ikonki2[i,3] - Y;
            break;
          end;
        end;
      end;
    end;
  end;
end;

//========================================================================================
procedure TTabloMain.DrawPlakat(X,Y : integer);
  var i,j,dx : integer; uu : boolean;
begin
try
  if not SetIkonRezNonOK and not WorkMode.OtvKom and not WorkMode.Upravlenie then
  begin
    for i := 1 to High(Stellaj) do stellaj[i] := false;
    IkonkaMove := false;
    IkonkaMoved := false;
    IkonkaDown := 0;
    IDX := 0;
    IDY := 0;
    exit;
  end;
  uu := false;
  if ((X+12+IDX) >= configRU[config.ru].BoxLeft) and
  ((Y+12+IDY) >= configRU[config.ru].BoxTop) and
  ((X+IDX) < configRU[config.ru].BoxLeft+12*20+7) and
  ((Y+IDY) < configRU[config.ru].BoxTop+16) then
  begin //------------------------------ ��������� ���� ���������������� ����� � ���������
    for j := 0 to 19 do
    begin //--------------------------------------- ��������� ���� ���������������� ������
      dx := j * 12 + 1; if j > 10 then dx := dx + 3;
      if (X >= configRU[config.ru].BoxLeft+dx) and (Y >= configRU[config.ru].BoxTop) and
      (X < configRU[config.ru].BoxLeft+dx+12) and (Y < configRU[config.ru].BoxTop+13) then
      begin
        for i := 1 to 20 do
        if i <> (j+1) then stellaj[i] := false
        else stellaj[i] := not stellaj[i];
      end;
    end;
    uu := true;
  end;
  IkonNew := true; IkonSend := true;
  if not uu then
  begin //---------------------------------------------------- ����������� ������ �� �����
    case config.ru of
      1 :
      begin
        Ikonki[IkonkaDown,2] := X + IDX;
        Ikonki[IkonkaDown,3] := Y + IDY;
      end;
      2 :
      begin
        Ikonki2[IkonkaDown,2] := X + IDX;
        Ikonki2[IkonkaDown,3] := Y + IDY;
      end;
    end;
  end else
  begin //---------------------------------------------------------- ������ ������ � �����
    case config.ru of
      1 : Ikonki[IkonkaDown,1] := 0;
      2 : Ikonki2[IkonkaDown,1] := 0;
    end;
  end;
except
  RepF('������ [TabloForm.TTabloMain.DrawPlakat]'); Application.Terminate;
end;
end;

procedure ResetAllPlakat;
var i : integer;
begin
  try
    for i := 1 to High(Ikonki) do
    begin
      Ikonki[i,1] := 0;
      Ikonki[i,2] := 0;
      Ikonki[i,3] := 0;
    end;
    for i := 1 to High(Ikonki2) do
    begin
      Ikonki2[i,1] := 0;
      Ikonki2[i,2] := 0;
      Ikonki2[i,3] := 0;
    end;
    IkonNew := true; IkonSend := true;
  except
    RepF('������ [TabloForm.ResetAllPlakat]'); Application.Terminate;
  end;
end;
{
//========================================================================================
//------------------------------------------------ ������� �� ������� ������ ������� �����
procedure TTabloMain.FormMouseDown(Sender : TObject; Button : TMouseButton;
Shift : TShiftState; X,Y : Integer);
begin
  GetPlakat(X,Y); //----------------- �������� ��������� ������������� ������� (���� ����)
  OperatorDirect := LastTime + 1/86400;
  if MarhTrac[1].TraSRazdel then ResetTrace;//----- �������� ������ ����������� ������
end;
 }

//========================================================================================
//------------------------------------------------------------ ������� �� ���������� �����
procedure TTabloMain.FormMouseUp(Sender: TObject; Button: TMouseButton;
Shift: TShiftState; X, Y: Integer);
var
  n,i : integer;
begin
  if PopupMenuCmd.PopupComponent <> nil then exit; //���� �� ����� �������� ����, �� �����

  if LockComDsp then
  begin  SBeep[1] := true; exit; end;//------------------------- ���� �������������, �����

  if Button = mbLeft then  //------------------------- ���� ���� ������ ����� ������ �����
  begin
    if IkonkaMove and  IkonkaMoved and  //---------------- ���� ������ � ��������� �������
    (IkonkaDown > 0) and (cur_obj < 0)
    then  DrawPlakat(X,Y); //---------------------------- ����������� ������ � ����� �����

     //----------------------------------------------- ������������ �� ������ � ����������
    IkonkaMove := false; IkonkaMoved := false; IkonkaDown := 0; IDX := 0; IDY := 0;

    with configRU[config.ru] do
    if (X+Sdvig> MsgLeft) and (X+Sdvig< MsgRight) and (Y> MsgTop) and (Y< MsgBottom) then
    begin //---------------------------------------- ������ � ���� ������������� ���������
      if (X+Sdvig > MsgRight-10) and (FixMessage.Count > 5) then
      begin //--------------------------------------------------------- ���������� ������?
        if (Y < MsgTop+10) and (FixMessage.ActLine > 1) then
        begin //------------------------------------------- ��������� ����� (����� ������)
          dec(FixMessage.ActLine);
          if FixMessage.ActLine < FixMessage.StartLine
          then FixMessage.StartLine := FixMessage.ActLine;
        end else
        if (Y > (MsgBottom-10)) and (FixMessage.ActLine < FixMessage.Count) then
        begin //------------------------------------------- ��������� ���� (������ ������)
          inc(FixMessage.ActLine); if(FixMessage.ActLine - FixMessage.StartLine) > 4
          then FixMessage.StartLine := FixMessage.ActLine - 4;
        end;
      end else
      begin //------------------------------------------------------------ �������� ������
        n := (Y - MsgTop) div 16 + FixMessage.StartLine; //--------------------- �  ������
        if n <= FixMessage.Count then FixMessage.ActLine := n;
      end;
      for i := 1 to High(Stellaj) do stellaj[i] := false; //--- �������� ����� � ���������
      exit;
    end  else // --------------------------- ���� ������ �� � ���� ������������� ���������
    if cur_obj < 0 then  //------------------- ���� ������ �� ������ � �����-���� ��������
    begin //-------------------------------------------------------------- ��������� �����
      if WorkMode.GoTracert then //--------------------------------- ���� ���� �����������
      //________________________� � � � � � � � � � � ____________________________________
      begin //-------------------- ��������� �� ������� �� ������� ������� ��� �����������
        n := -1;
        for i := OVmin to OVmax do //------------------------------- �� ���� �������������
        if ObjView[i].TypeObj = 11 then //------------- ���� ������� ����������� = �������
        begin   //-------------------------------- ���� ��� ������� � ���� ������� �������
          if (ObjView[i].P[2].X-8 < X) and  (ObjView[i].P[2].Y-8 < Y) and
          (ObjView[i].P[2].X+8 > X) and    (ObjView[i].P[2].Y+8 > Y) then
          begin n := ObjView[i].ObCI[4]; break; end;//----- ����� ����� ������ ��� �������
        end;

        if n > 0 then //------------------------------------------- ���� ���������� ������
        with ObjZv[i] do
        begin
          for i := 1 to WorkMode.LimitObjZav do //- ������ ������ �� �������� ������������
          //-- ���� ������� � �� ���������� ���� ������� ������ ������
          if (TypeObj >= 1) and (TypeObj <= 8) and (VBufInd = n) then
          begin //---------------------------------- ��, �������������, ���������� �������
            if MarhTrac.Finish then //------------- ���� ����� ������ ��� ������, ��
            begin //------------------------------------------------- �� ���������� ������
              if LastID_Obj = i then //--- ���� ��� ������� � ���� ��������� ������ ������
              begin //---------------------------- �� ��������� ����� � ����������� ������
                LastID_Obj := -1;
                ID_Menu := Key_EndTrace; //--------------------- Key_EndTrace = 1008 <End>
                cur_obj := 20000;  //-------------- �������� ������� ������ "����� ������"
                NewMenu_(ID_menu, X, Y);
                SelectCommand;
                break;
              end
              else begin ResetTrace;LastID_Obj:=-1; exit; end;//����� ����� ������ � �����
            end  else //------------------------------------- ���� ������ ��� �� ���������
            begin
              LastID_Obj := i; ID_Obj := i;
              ID_Menu    := ID_Tracert;    //------- ���������� ������ �� �������� �������
              cur_obj    := 20002;
              NewMenu_(ID_menu, X, Y); break;
            end;
          end;
        end else //---------------------------------------------- �� ������ ����������, ��
        begin SetPlakat(X,Y);ResetCommands;exit;end; //-- ��������� ������� ������ � �����
      end else

      //------------------------------------------------ ���� ���� �� ����������� ��������
      begin  SetPlakat(X,Y);ResetCommands; exit; end; //------ ��������� �� ����� �� �����

    end else //------------------------------------------- ���� ������ ��� � ���-�� ������
    for i := 1 to High(Stellaj) do stellaj[i] := false; //----- �������� ����� � ���������

    //--- ���� ���� ������ ��� �������� �  ������� ������� ��� � ������������� ����������
    if (cur_obj > 0) and (DspMenu.WC or DspMenu.Ready) then
    begin   //-- ���� ���� ������ ��� ���������� � ������ ������� �� ���, ��� ������ �����
      if (DspMenu.Obj > 0) and (cur_obj <> DspMenu.Obj) then
      begin //---------------------- ��� ������ ������������� ������ ����� ������� � �����
        if WorkMode.OtvKom then begin ResetCommands; exit; end else
        //����� ���� ������ ������ ���������� ������� ��� ��������� ����� ������� ��������
        //� ���� ������� ��������(������� ������� �� ������)� ������ ������ "����� ������"
        if (MarhTrac.Finish or (MarhTrac.GonkaStrel and (MarhTrac.GonkaList > 0))) and
        (ObjUprav[cur_obj].IndexObj = 20000) then
        begin MarhTrac.Finish := false;  DspCom.Active := true;  SelectCommand; end
        else  ResetCommands;
        exit;//----------------------------------------------- ����� ����� ������� � �����
      end;
    end
    else for i:=1 to High(sMsgCvet) do sMsgCvet[i] := 0;

    //-------------------------- ���� ���� ������������� �� ���������, �� �������� �������
    if DspMenu.WC then
    begin
      DspCom.Active := true;
      SelectCommand;
    end
    else
    //-- �����, ����  ��������� ����� �������, �� ������� ������� � �������� �������������
    if DspMenu.Ready then DspMenu.WC := true
    else
    begin //------------------------------------------------------------------ ����� ....
      if ID_menu > 0 then  //--------------------------- ���� ���� ��������� �������, ��..
      begin
        NewMenu_(ID_menu, X, Y); //-------------------- ������� ���� ��� ���� �������
        //--------------------------------- ��������� ������� ����� ����� ������ ���������
        if not DspMenu.WC then SelectCommand; //--- ���� �� ���� �������������, �� �������
      end else SBeep[1] := true; //------------------------------- ����� ������� � �����
    end;
  end else
  begin //----------------------------------------------------- ������ ������ ������ �����
    //------------------------------------- ����� ������, ������������� �� ����� � �������
    n := 1;
    case config.ru of  //----------------------------- ������������� �� ������� ����������
      1 :
      begin //------------------------------------------------------------------------ RU1
        for i := High(Ikonki) downto 1 do
        if (Ikonki[i,1] > 0) and (Ikonki[i,2] <= X) and
        (Ikonki[i,2]+12 >= X) and (Ikonki[i,3] <= Y)and
        (Ikonki[i,3]+12 >= Y) then
        begin Ikonki[i,1] := 0; n := 0; break; end;
      end;

      2 :
      begin //------------------------------------------------------------------------ RU2
        for i := High(Ikonki2) downto 1 do
        if (Ikonki2[i,1] > 0) and (Ikonki2[i,2] <= X) and
        (Ikonki2[i,2]+12 >= X) and (Ikonki2[i,3] <= Y) and
        (Ikonki2[i,3]+12 >= Y) then
        begin Ikonki2[i,1] := 0; n := 0; break; end;
      end;
    end;
    IkonkaMove := false;
    IkonkaMoved := false;
    IkonkaDown := 0;
    IDX := 0;
    IDY := 0;
    InsNewArmCmd(0,0);
    UnLockHint;
    ResetCommands;
    //----------------------------------------------------- �������� ����������� ���������
    if (n > 0) and (X+Sdvig > configRU[config.ru].MsgLeft) and
    (X+Sdvig < configRU[config.ru].MsgRight) and
    (Y > configRU[config.ru].MsgTop) and (Y < configRU[config.ru].MsgBottom) then
    begin
      n := (Y - configRU[config.ru].MsgTop) div 16 + FixMessage.StartLine;//- ����� ������
      if n <= FixMessage.Count then
      begin
        FixMessage.ActLine := n;
        ResetFixMessage;
      end
      else SBeep[1] := true;
    end;
  end;
end;

//========================================================================================
//-------------------------------------------------------- ��������� ������� �� ����������
procedure TTabloMain.FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
begin
  OperatorDirect := LastTime + 1/86400;
  //----------------------------------------------- ��������� ������������ ������� �������
  if LastKey = Key then
  begin
    if (LastKeyPress+10/80000) < Date+Time then //---- ���� ������� ������ ����� 10 ������
    begin
      LastKeyPress := Date+Time;
      case Key of //------------------------- ������������ ����� ��� �������������� ������
        VK_RETURN : sMsg0 := 'ENTER';
        VK_ESCAPE : sMsg0 := 'ESC';
        VK_SPACE : sMsg0 := '������';
        VK_BACK : sMsg0 := '�����';
        VK_F1 : sMsg0 := 'F1'; //-------- ���������� ����������/���������� ����� ����������
        VK_F2 : sMsg0 := 'F2'; //--------------------------------------------- ���� �������
        VK_F3 : sMsg0 := 'F3'; //--------------------------------------- ���� ������ ������
        VK_F4 : sMsg0 := 'F4'; //------------------------------ ����� ����������� ���������
        VK_F5 : sMsg0 := 'F5'; //-- �������� ��������� �� �������� ���������(��� ���������)
        VK_F6 : sMsg0 := 'F6'; //--- ��������� ���������� � ������ ���������� �������������
        VK_F7 : sMsg0 := 'F7'; //---------- ����� ���������� ���������� � ��-���(���������)
        VK_F8 : sMsg0 := 'F8'; //------------------- ������������� ��������� ������ �������
        VK_F9 : sMsg0 := 'F9'; //---------------- ������������� ��������� ��������� �������
        VK_F10 : sMsg0 := 'F10'; //------------- ������ ������ � ����� ���������� ���������
        VK_F11 : sMsg0 := 'F11'; //--------------------------- ���������� ��������� �������
        VK_F12 : sMsg0 := 'F12'; //------------------------------ ����� ������������ ������
        VK_INSERT : sMsg0 := 'INSERT'; //�������� ������ �������� FR3, FR4 (��� Ctrl+Shift)
        VK_DELETE : sMsg0 := 'DELETE'; //--- ����� ���� �������������� �������� (��� Shift)
        VK_HOME : sMsg0 := 'HOME';  //----- ��������� ������ �� ������ ������ ������ ������
        VK_END : sMsg0 := 'END'; //-- ����� ������ ��������(��� ����� ��������, ���� Shift)
        VK_NEXT : sMsg0 := 'Page Down'; //----- ����� ������� ����� ������ (� ������ DEBUG)
        VK_PRIOR : sMsg0 := 'Page Up';  //------ ����� ������� ����� ����� (� ������ DEBUG)
        VK_LEFT : sMsg0 := '������� �����';   //- ����������� ������ �� ������ ������ �����
        VK_RIGHT : sMsg0 := '������� ������';//- ����������� ������ �� ������ ������ ������
        VK_DOWN : sMsg0 := '������� ����'; //----- ����������� ������ �� ������ ������ ����
        VK_UP : sMsg0 := '������� �����';//------ ����������� ������ �� ������ ������ �����
        VK_SHIFT : sMsg0 := 'SHIFT';
        VK_MENU : sMsg0 := 'ALT';
        VK_CONTROL : sMsg0 := 'CTRL';
        else  sMsg0 := Char(Key); //------------------- ��� ���� ��������� ������ ����� ���
      end;
      InsNewArmCmd($7fe7,0);
      AddFixMes(GetSmsg(1,141,'<' + sMsg0 + '>',0),4,3);
      showSMsg (141,LastX,LastY,'<'+ sMsg0 + '>');//---------- "��������� ������ �������"
    end;
  end else
  begin //------------------------------------------ ��������� ��� ������� � ����� �������
    LastKey := Key;
    LastKeyPress := Date+Time;
  end;
end;

//========================================================================================
//-------------------------------------------------------- ���������� ������ �� ����������
procedure TTabloMain.FormKeyUp(Sender: TObject; var Key: Word; Shift: TShiftState);
//------------------------------------------------- Key - ��� ������� � ���������� �������
//----------------------------------------------- Shift - ��� ��������� ����������� ������
var
  i,o,p,x,y : integer;
  t : TPoint;
  {$IFDEF SAVEKANAL} sl : TStringList; {$ENDIF}
begin
  LastKey := 0; //---------------------------- �������� ��� ������� ������� ��� ����������

  if PopupMenuCmd.PopupComponent <> nil then exit;

  case Key of
    VK_INSERT : //--------------------------------------------------- ������ ������� <Ins>
    begin
      if Shift = [ssShift,ssCtrl] //----------- ���� ����� ������������ ������ �����������
      then FrForm.Show;//------------------------------- �������� ������ �������� FR3, FR4

      if Shift = [ssShift,ssAlt] //----------- ���� ����� ������������ ������ �����������
      then ViewBaza.Show;//----------------------------- �������� ������ �������� FR3, FR4
    end;

    VK_DELETE : //---------------------------------------------- ���� ������ ������� <Del>
    begin
      if Shift = [ssShift] then //------------ � ������������ � ��� ������ ������� <Shift>
      begin //--------------------------------------- ����� �������������� �������� (����)
        ResetAllPlakat;
        showSMsg (142,LastX,LastY,''); //----------- "�������������� �������� ��������"
      end else
      if Shift = [] then
      begin //------------------------------------------------------ ����� ������ ��������
        InsNewArmCmd(0,Key_ClearTrace);
        Cmd_ChangeMode(Key_ClearTrace); //?? ������ �� ������
      end;
    end;

    VK_LEFT : //--------------------------------------------------- ������ ��������� �����
    begin
      if cur_obj > 0 then
      begin
        if ObjUPrav[cur_obj].Sosed[1] > 0
        then cur_obj := ObjUPrav[cur_obj].Sosed[1];
      end
      else  cur_obj := StartObj;

      if cur_obj > 0
      then SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_RIGHT : //------------------------------------------------- ������ ��������� ������
    begin
      if cur_obj > 0 then
      begin
        if ObjUPrav[cur_obj].Sosed[2] > 0
        then cur_obj := ObjUPrav[cur_obj].Sosed[2];
      end
      else cur_obj := StartObj;

      if cur_obj > 0
      then SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_UP : //----------------------------------------------------- ������ ��������� �����
    begin
      if Shift = [ssCtrl] then //----------------------------- ���� ��� ���� ������ <Ctrl>
      begin //------------------------ ����������� ��������� ������ ����� � ���� ���������
        if (FixMessage.Count > 0) and (FixMessage.ActLine > 1) then
        begin
          dec(FixMessage.ActLine);
          if FixMessage.ActLine < FixMessage.StartLine
          then FixMessage.StartLine := FixMessage.ActLine;
        end
        else  SimpleBeep;
      end
      else
      if Shift = [] then
      begin
        if cur_obj > 0 then
        begin
          if ObjUPrav[cur_obj].Sosed[3] > 0
          then cur_obj := ObjUPrav[cur_obj].Sosed[3];
        end
        else cur_obj := StartObj;
        if cur_obj > 0 then
        SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
      end;
    end;

    VK_DOWN :  //--------------------------------------------------- ������ ��������� ����
    begin
      if Shift = [ssCtrl] then //----------------------------- ���� ��� ���� ������ <Ctrl>
      begin  //----------------------- ����������� ���������� ������ ���� � ���� ���������
        if (FixMessage.Count > 0) and (FixMessage.ActLine < FixMessage.Count) then
        begin
          inc(FixMessage.ActLine);
          if FixMessage.ActLine > FixMessage.StartLine + 4
          then FixMessage.StartLine := FixMessage.ActLine - 4;
        end
        else  SimpleBeep;
      end else
      if Shift = [] then
      begin
        if cur_obj > 0 then
        begin
          if ObjUPrav[cur_obj].Sosed[4] > 0
          then cur_obj := ObjUPrav[cur_obj].Sosed[4];
        end
        else cur_obj := StartObj;
        if cur_obj > 0  then
        SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
      end;
    end;

    VK_RETURN : //------------------------------------------------- ������ ������� <Enter>
    begin
      //------------------------------------- ���� ������� �������������, �� ������� � ���
      if LockComDsp then
      begin  SBeep[1] := true;  exit;  end;

      if DspMenu.WC then //------------------- ���� ���� �������� ������������� ����������
      begin
        if cur_obj = DspMenu.obj then // ���� ������ �� ��� �������, ��� �������� ��������
        begin
          DspCom.Active := true;
          SelectCommand; //----------------------------------------------- ������� �������
        end
        else
        begin //------------------ ����� ����� �������������� �������, ���� ���� � �������
          RSTMsg; //------------------------------- �������� ��� �������� ���������
          DspMenu.Ready := false; DspMenu.WC := false; //------ �������� �������� ��������
          DspMenu.obj := -1; //----------------------------- ������ ��������� ����� ������
        end;
      end
      else //-------------------------------------------------- ���� �� ���� �������������
      //------------------------------------------------------- �� ��������� ����� �������
      if DspMenu.Ready then SelectCommand
      else
      begin //------------------------------------------- ���� ��� �������� ������ �������
        if cur_obj < 0 then SimpleBeep //- ���� ������ ����� �� �� �������, ������ �������
        else
        begin
          x := ObjUprav[cur_obj].Box.Left;
          y := ObjUprav[cur_obj].Box.Top;
          if ID_menu > 0 then //---------------------- ���� ��� ������� ���� ��� ���� ����
          begin
            NewMenu_(ID_menu, X, Y);  //---------------------------- ����������� ����
            if not DspMenu.WC then SelectCommand; //- ���� ��� �������������, �� ���������
          end else SimpleBeep;
        end;
      end;
    end;

    VK_ESCAPE : //---------------------------------------------- ���� ������ ������� <Esc>
    begin
      UnLockHint;
      ResetCommands;
      InsNewArmCmd(0,0);
      if cur_obj > 0 then //----------------- ���� ���� ��������� ������, �� ������ �� ���
      SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_SPACE :                        
    begin
      UnLockHint;
      ResetCommands;
      InsNewArmCmd(0,0);
      if cur_obj > 0 then
      SetCursorPos(ObjUPrav[cur_obj].Box.Right-Sdvig-2,ObjUPrav[cur_obj].Box.Bottom-2);
    end;

    VK_END :
    begin
      if WorkMode.MarhUpr then
      begin
        if Shift = [] then
        begin //---------------------------------------------------- ����� ������ ��������
          DspMenu.obj := 0;
          NewMenu_(Key_EndTrace,LastX,LastY);
          SelectCommand;
        end else
        if Shift = [ssShift] then
        begin //------------------------------------------------------------- ����� ������
          DspMenu.obj := 0;
          NewMenu_(Key_ClearTrace,LastX,LastY);
          SelectCommand;
        end;
      end;
    end;

    VK_HOME :
    begin
      if Shift = [] then
      begin //---------------------------- ��������� ������ �� ������ ������ ������ ������
        for i := 1 to High(ObjUprav) do
        begin
          if config.ru = ObjUprav[i].RU then
          begin
            if WorkMode.MarhUpr and (ObjUprav[i].MenuID = Key_MarshRejim) then
            begin
              SetCursorPos(ObjUPrav[i].Box.Right-Sdvig-60, ObjUPrav[i].Box.Bottom-8);
              break;
            end else
            if WorkMode.RazdUpr and (ObjUprav[i].MenuID = Key_RazdeRejim) then
            begin
              SetCursorPos(ObjUPrav[i].Box.Right-Sdvig-60, ObjUPrav[i].Box.Bottom-8);
              break;
            end;
          end;
        end;
        GetCursorPos(t);//------------ �������� ������� �������� ������� � ������� ����� t
        IskCnt:=configRU[config.ru].T_S.X+configRU[config.ru].T_S.Y-t.X-t.Y;
        FindCursor := true;
      end;
    end;

    VK_PRIOR :
    begin
{$IFDEF DEBUG}
      //-------------------------------------------------------- ����� ������� ����� �����
      if Sdvig >= 800 then Sdvig := Sdvig - 800
      else begin Sdvig := 0; SimpleBeep; end;
{$ENDIF}
    end;

    VK_NEXT :
    begin
{$IFDEF DEBUG}
      //------------------------------------------------------- ����� ������� ����� ������
      if Sdvig < (configru[config.ru].T_S.X - configru[config.ru].MonSize.X)
      then Sdvig := Sdvig + 800
      else SimpleBeep;
{$ENDIF}
    end;

    VK_F1 :
    begin
      if Shift = [] then
      begin
        if WorkMode.MarhUpr then
        begin //----------------------------------- ���������� ���������� ����� ����������
          DspMenu.obj := 0;
          NewMenu_(Key_RazdeRejim,LastX,LastY);
          SelectCommand;
        end else
        begin //----------------------------------- ���������� ���������� ����� ����������
          DspMenu.obj := 0;
          NewMenu_(Key_MarshRejim,LastX,LastY);
          SelectCommand;
        end;
      end;
    end;

    VK_F2 :
    begin
      if Shift = [] then //-------------------------------------------------- ���� �������
      begin
        DspMenu.obj := 0;
        NewMenu_(Key_DateTime,LastX,LastY);
        SelectCommand;
      end;
    end;

    VK_F3 :
    begin
      if Shift = [] then //-------------------------------------------- ���� ������ ������
      begin
        DspMenu.obj := 20002;
        NewMenu_(Key_VvodNomeraPoezda,LastX,LastY);
        SelectCommand;
      end;
    end;

    VK_F4 :
    begin //-------------------------------------------------- ����� ����������� ���������
      if Shift = [] then
      begin
        Zvuk := false;
        ResetFixMessage;
      end;
    end;

    VK_F5 :
    begin //------------------------------- �������� ��������� �� �������������� ���������
      if Shift = [] then
      begin
        if WorkMode.Upravlenie then exit;
        OpenMsgForm := true;
      end;
    end;

    VK_F6 :
    begin
      if Shift = [ssShift,ssAlt,ssCtrl] then
      begin //--------------------- ��������� ���������� � ������ ���������� �������������
        if Application.MessageBox('����������� ���������� ������','��-���',MB_OKCANCEL) = IDOK then
        begin
          ReBoot := false;
          Application.Terminate;
        end;
{$IFDEF DEBUG}
      end else
      //------------------------------------- �������� ���������� ��� �������� ��� �������
      if Shift = [] then
      begin
        WorkMode.Upravlenie := true;
        WorkMode.LockCmd := false;
        StartRM := false;
{$ENDIF}
      end;
    end;

    VK_F7 : //---- ����� ���������� ���������� � ��-���(��� ���������� ������ �����������)
    begin
      if (Shift = [ssShift,ssAlt,ssCtrl]) and not WorkMode.Upravlenie then
      begin
        if config.ru <> 1 then
        begin
          if (Stol.X >= configru[1].T_S.X) and (Stol.Y >= configru[1].T_S.Y) then
          begin
            InsNewArmCmd($7fec,0);
            NewRegion := 1;
            ChRegion := true;
          end;
        end;
        if config.ru <> 2 then
        begin
{$IFNDEF DEBUG}
          if (Stol.X >= configru[2].T_S.X) and
          (Stol.Y >= configru[2].T_S.Y) then
{$ENDIF}
          begin
            InsNewArmCmd($7feb,0);
            NewRegion := 2;
            ChRegion := true;
          end;
        end;
      end;
    end;


    VK_F8 :
    begin //--------------------------------------- ������������� ��������� ������ �������
      if Shift = [] then
      begin
        DspMenu.obj := 20003;
        NewMenu_(Key_PodsvetkaNomerov,LastX,LastY);
        SelectCommand;
{$IFDEF SAVEKANAL}
      end else
      if Shift = [ssCtrl] then
      begin
        sl := TStringList.Create;
        sl.Text := trmkvit;
        sl.SaveToFile('trm.txt');
        sl.Text := rcvkvit;
        sl.SaveToFile('rsv.txt');
        trmkvit := '';
        rcvkvit := '';
        sl.Free;
{$ENDIF}
      end;
    end;


    VK_F9 :
    begin //-------- ������������� ��������� ��������� �������, ������ �������� ����������
      if Shift = [] then
      begin
        DspMenu.obj := 20001;
        NewMenu_(Key_PodsvetkaStrelok,LastX,LastY);
        SelectCommand;
      end;
    end;

    VK_F10 :
    begin //--------- ��������������� ������� - ������ ������ � ����� ���������� ���������
      if (asTestMode = $55) then exit;
      TabloMain.Visible := false;
      if Shift = [ssShift] then
      begin //---------------------------------------------------- ���� ���������� �������

        sMsg0 := InputBox('���� ���������� �������', '������� �������', '');
        if sMsg0 <> '' then
        begin
          if (sMsg[1] = '�') or (sMsg[1] = '�') or (sMsg[1] = 'm') or (sMsg[1] = 'M')
          then MarhTrac.MarhCmd[10] := _marshrutmanevr
          else
          if (sMsg[1] = '�') or (sMsg[1] = '�') or (sMsg[1] = 'p') or (sMsg[1] = 'P')
          then MarhTrac.MarhCmd[10] := _marshrutpoezd
          else MarhTrac.MarhCmd[10] := _marshrutlogic;

          i := 3; sPar := '';
          while i <= Length(sMsg0 ) do
          begin
            if sMsg[i] = ' ' then break
            else sPar := sPar + sMsg[i];
            inc(i);
          end;
          inc(i);
          try x := StrToInt(sPar) except x := 0 end;

          MarhTrac.MarhCmd[1] := x - (x div 256) * 256;
          MarhTrac.MarhCmd[2] := x div 256;
          sPar := '';

          while i <= Length(sMsg0 ) do
          begin
            if sMsg[i] = ' ' then break
            else sPar := sPar + sMsg[i];
            inc(i);
          end;
          inc(i);
          try  x := StrToInt(sPar) except x := 0 end;

          MarhTrac.MarhCmd[3] := x - (x div 256) * 256;
          MarhTrac.MarhCmd[4] := x div 256;
          sPar := '';

          while i <= Length(sMsg0 ) do
          begin
            if sMsg[i] = ' ' then break
            else sPar := sPar + sMsg[i];
            inc(i);
          end;
          MarhTrac.MarhCmd[5] := Length(sPar);
          MarhTrac.MarhCmd[6] := 0;
          MarhTrac.MarhCmd[7] := 0;
          MarhTrac.MarhCmd[8] := 0;
          MarhTrac.MarhCmd[9] := 0;

          o := 1; p := 0;
          for i := 1 to Length(sPar) do
          begin
            if sPar[i] = '1'
            then p := p + o;
            o := o * 2;
          end;
          i := p and $ff;
          MarhTrac.MarhCmd[6] := i;
          i := p and $ff00;
          i := i shr 8; MarhTrac.MarhCmd[7] := i;
          i := p and $ff0000;
          i := i shr 16; MarhTrac.MarhCmd[8] := i;
          i := p and $ff000000;
          i := i shr 24; MarhTrac.MarhCmd[9] := i;
          CmdSendT := LastTime;
          WorkMode.MarhRdy := true;
        end;
      end else
      begin //---------------------------------------------------- ���� ���������� �������
        sMsg0 := InputBox('���� ���������� �������', '������� �������', '');
        if sMsg0 <> '' then
        begin
          i := 1;
          sPar := '';
          while i <= Length(sMsg0 ) do
          begin
            if sMsg[i] = ' ' then break
            else sPar := sPar + sMsg[i];
            inc(i);
          end;
          inc(i);
          try x := StrToInt(sPar) except x := 0 end;

          sPar := '';
          while i <= Length(sMsg0 ) do
          begin
            if sMsg[i] = ' ' then break
            else sPar := sPar + sMsg[i];
            inc(i);
          end;
          try y := StrToInt(sPar) except y := 0 end;

          if (x > 0) and (x < 190) and (y > 0) and (y < 1100) then
          begin
            CmdBuff.Cmd := x;
            CmdBuff.Index := y;
            CmdCnt := 1;
            CmdSendT := LastTime;
          end
          else SimpleBeep;
        end;
      end;
    end;

    VK_F11 :
    begin //------------------------------------------------- ���������� ��������� �������
      GetCursorPos(t);
      IskCnt := configRU[config.ru].T_S.X + configRU[config.ru].T_S.Y - t.X - t.Y;
      if (t.X + t.Y) > IskCnt
      then IskCnt := t.X + t.Y;
      FindCursor := true;
    end;

    VK_F12 ://-------------------------------------------------- ����� ������������ ������
    begin
      InsNewArmCmd($7fe8,0);
      Zvuk := false;
    end;

    else inherited;
  end;
end;


//========================================================================================
//------------------------------------------------------ ���������� �������� ������� �����
procedure TTabloMain.MainTimerTimer(Sender: TObject);
var
  gts : TSystemTime;
  st,i : integer;
  bh,bl,b : byte;
  ec: cardinal;
begin
{$IFNDEF DEBUG} Sdvig := 0; {$ENDIF}
{$IFDEF DEBUG} WorkMode.LockCmd:=false;for i:=1 to 1024 do ObjZv[i].bP[31]:=true; {$ENDIF}

  if LoopHandle > 0 then //------------------------ ���� ���������� ����� ����� � ��������
  begin
    if GetExitCodeThread(LoopHandle,ec) then  //---------------------- ���� ����� � ������
    begin
      if(ec<>STILL_ACTIVE) and LoopSync then//����� �� �������,����� ������� �������������
      begin //---------------- ��������� ������������� ������ ��������� ������� ���-������
        RepF('���������� ������ DSP-STAN'); CloseHandle(LoopHandle); //----- ������� �����
        //--------------------------------------------- ������ ������������ ������� ������
        LoopHandle := CreateThread(nil,0,@SyncReadyThread,nil,0,ec);
        RepF('����� DSP-STAN �����������.ThreadID = ' + IntToStr(ec));
      end;
    end;
  end else //------------------------------------------------------------- ���� ������ ���
  if LoopSync then //-------------------------- ���� ����� � �������� ������ �������������
  begin
    RepF('������ ������ ������� ����� ���-������');
    LoopHandle := CreateThread(nil,0,@SyncReadyThread,nil,0,ec);//- ������ ������. �������
    RepF('����� ������ ���-������ �������. ThreadID = ' + IntToStr(ec));
  end;

  if LockTablo then exit;//---------------- ���� ����� ����������� �� ����������, �� �����

  if Assigned(Application.MainForm) then  //------------------ ���� ��������� ������� ����
  begin
    if Application.MainForm.WindowState = wsMinimized//- ���� ���� �������� ��������������
    then Application.MainForm.WindowState := wsNormal;//-- ���������� ������� ������� ����

    if not Application.MainForm.Visible then   //----------------���� ���� �������� ������
    begin
      Application.MainForm.Visible := true; //--------------------- ������� ��������� ����
      SetForegroundWindow(Application.Handle);//---------- ��������� ���� �� �������� ����
    end;
  end;

  inc(GlobusIndex); if GlobusIndex > 31 then GlobusIndex := 0; //------- ���������� ������
  LastTime := Date + Time;             //-------------------------- ������� ������ �������

  if dMigTablo < LastTime then //------------------------- ������������ �������� ���������
  begin tab_page := not tab_page;  dMigTablo := LastTime + MigInterval / 86400;  end;

  if CanFocus then DspMenu.Ready := false; //------- ���������� �������� ��������� � �����

  if MySync[1] or MySync[2] then inc(CntSyncCh)//���� ���� ������������� �� ������ 1 ��� 2
  else
  if (LastTime - LastSync > RefreshTimeOut) //------- ���� ������ ���������� ����� �������
  then inc(CntSyncTO); //---------------------------------- ��������� ������� �� ����-����

  if (LastTime - LastSync > RefreshTimeOut) or MySync[1] or MySync[2] then
  begin //----------------------- �������� ����������� ����� �����, ���� �������� �� �����
    MySync[1] := false; MySync[2] := false;
    //------------------------------- ���������� ��������� ������ �� ������ ���-������
    //---- ���� ������� ����� 70 �������� � ������ 1, �� ��������� ������� ������ ��������
    if KanalSrv[1].lastcnt < 70 then inc(KanalSrv[1].lostcnt)
    else KanalSrv[1].lostcnt := 0; //------------ ���� ������� 70 � �����, �� ����� ������

    //----- ���� ������ ����� 10, �� ���������� ������� ������ � ������ 1, �������� ������
    if KanalSrv[1].lostcnt > 10 then
    begin KanalSrv[1].iserror := true; KanalSrv[1].lostcnt := 0; end;

    //----------------------------------------------------------- �� �� ����� ��� ������ 2
    if KanalSrv[2].lastcnt< 70 then inc(KanalSrv[2].lostcnt) else KanalSrv[2].lostcnt:= 0;
    if KanalSrv[2].lostcnt > 10 then
    begin KanalSrv[2].iserror := true; KanalSrv[2].lostcnt := 0; end;

    //----------------------------------------------------------------- ����������� ������
    if not RefreshTablo then
    begin
      DateTimeToString(sMsg0, 'dd/mm/yy h:nn:ss.zzz', LastTime);
      RepF('���� ����������� ����� '+ sMsg0);
    end;

    //--------------------------------------- ����������� ���� ��������� ������ ����������
    b := 0;
    if WorkMode.RazdUpr   then b := b +1;     //-------------------- ���������� ����������
    if WorkMode.MarhUpr   then b := b + 2;    //-------------------- ���������� ����������
    if WorkMode.MarhOtm   then b := b + 4;    //----------------------------------- ������
    if WorkMode.InpOgr    then b := b + 8;    //------------------------- ���� �����������
    if WorkMode.VspStr    then b := b + $10;  //------------------ ��������������� �������
    if WorkMode.OtvKom    then b := b + $20;  //------------------------------- ������ ���
    if WorkMode.Podsvet   then b := b + $40;  //------------------------ ��������� �������
    if WorkMode.GoTracert then b := b + $80;  //--------------------- ���� ������ ��������

    if(ArmState <> b) and (WorkMode.ArmStateSoob > 0) then //-- ��������� ������� � ������
    begin
      ArmState := b;  FR3inp[WorkMode.ArmStateSoob] := char(b);
      bl := WorkMode.ArmStateSoob and $ff;   bh := WorkMode.ArmStateSoob shr 8;
      NewFR[1] := NewFR[1] + char(bl) + char(bh) + char(ArmState);
    end;

    //--------------------------------- ��������� ��������� ������� � ������� ���� � �����
    if (NewFR[1] <> '') or (NewFR[2] <> '') or //---- 1-� ��� 2-� ����� ������� ��� ������
    (NewCmd[1] <> '') or (NewCmd[2] <> '') or //-------- 1-� ����� ��� 2-� ����� ���������
    (NewMenuC <> '') or (NewMsg <> '') then //---- ����� ������ ����, ��� ��������� �� FR3
    begin
      if StartRM and (StartTime < LastTime - 9/86400) then SaveArch(2)
      else SaveArch(1);

      if DspToArcEnabled then //------------------------------------------------ ���-�����
      begin //----------------------------------------- �������� ����� �� ������ ���-�����
        if DspToArcConnected and DspToArcAdresatEn and //-------- ���� ���� ����� �� �����
        not DspToArcPending and //------ ��� ���������� ������ �� ����� �������� � ����� �
        (LenArc > 0) and (BuffArc[1] > 0) then //----- ����,��� ������, � ������ �� ������
        begin
          SendDspToArc(@BuffArc[1],LenArc);//----- ������� � ����� ������ ��������� ������
          LenArc := 0;
        end;
      end;
    end;

    if Assigned(MsgFormDlg) then  //--------------------- ���� ���������� ��������� ������
    begin
      if OpenMsgForm then //----------- ���� ���������� ������ �� �������� ����� ���������
      begin
        OpenMsgForm := false; MsgFormDlg.Left := 0; MsgFormDlg.Top := 0;
        MsgFormDlg.Width  := configru[config.ru].MonSize.X;
        MsgFormDlg.Height := configru[config.ru].MonSize.Y;
        MsgFormDlg.Show;  //----------------------- �������� ���� ����� ���������� �������
        UpdateMsgQuery := true;
      end;

      if MsgFormDlg.Visible then  //-------------------------- ���� ��������� ������ �����
      begin
        if NewNeisprav then //----------------- ���� ���� ����� ������������� ��� ��������
        begin
          NewNeisprav := false; //--------------------- ����� ������� ������� ��� ��������
          MsgFormDlg.BtnUpdate.Enabled := true; //-------------- ������� ������ "��������"
        end;

        if UpdateMsgQuery then //--------- ���� ���� ������ �� ���������� ������ ���������
        begin
          MsgFormDlg.BtnUpdate.Enabled := false; //------------- ������� ������ "��������"
          UpdateMsgQuery := false; //--------- ����� ������ �� ���������� ������ ���������

          st := 1;
          i := 0;
          while st <= Length(LstNN) do //------------- ������ �� ������ �������������� ���
          begin
            if LstNN[st] = #10 then inc(i);//---------- ����� ������, ���� ����� ���������
            //--- ���� ������ 700 ���� ���������� ��������, ����� 700 ���� �������� ������
            if i < 700 then inc(st) else begin SetLength(LstNN,st); break; end;
          end;
          MsgFormDlg.Memo.Lines.Text := LstNN;//----------- ������ �������������� �� �����

          st := 1;
          i := 0;
          while st <= Length(ListDiagnoz) do //------- ������ �� ������ �������������� ���
          begin
            if ListDiagnoz[st] = #10 then inc(i);//----- ���� ����� ������, ���� ���������
            if i < 700 then inc(st)  //---------- ���� ������ 700 ���� ���������� ��������
            else    //------------------------------------------------ ���� ����� 700 ����
            begin SetLength(ListDiagnoz,st); break; end; //--- �������� ������ � ���������
          end;
          MsgFormDlg.MemoUVK.Lines.Text :=ListDiagnoz;//--- ������ �������������� �� �����
        end;
      end;
    end;

    if (LastReper + (600/86400)) < LastTime then //--- �� ��������� ������ � ����� >10 ���
    begin //------------------------------------- ��������� 10-�� �������� ����� ���������
      SaveArch(2);
      if DspToArcEnabled then //-------------------- ���� ���������������� ����� ���-�����
      begin //----------------------------------------- �������� ����� �� ������ ���-�����
        if DspToArcConnected and //--------------------------- ���� ���� ����� ��������� �
        DspToArcAdresatEn and //----------------------- ���� �������� ������� ��� ������ �
        not DspToArcPending and (LenArc > 0) then //---- ��� ����������, ���� ��� ��������
        begin
          SendDspToArc(@BuffArc[1],LenArc);//----- ������� � ����� ������ ��������� ������
          LenArc := 0;
        end;
      end;
    end;
    //------------------------------------------------------- ���������� ��������� �������
    //-�������� ����������� ����� ������� ������� ObjZav �� 10 ���� � ������ ����� �������
    st := cntObjZav + 10;  //--------------------------- ����������� ��������� 10 ��������
    while cntObjZav < st do //------------------------------------ �������� �� ���� ������
    begin
      if cntObjZav <= WorkMode.LimitObjZav then //���� �� ����� �� ����� �������� �������.
      begin
        if not CalcCRC_OZ(cntObjZav) then //--------------- ���� �� ��� ������� �� �������
        begin
          InsNewArmCmd($7ffc,0); //----------- �������� � ����� ��� ��������� � ������� ��
          showSMsg (526,LastX,LastY,'������� ������������ � ������ '+
          IntToStr(cntObjZav));
          SimpleBeep;
        end;
        inc(cntObjZav); //------------------------------------ ������� �� ��������� ������
      end else begin cntObjZav := 1; break; end;
    end;

    //�������� ����������� ����� ������� ������� ObjView �� 20 ���� � ������ ����� �������
    st := cntObjView + 20; //--------------------------- ����������� ��������� 20 ��������
    while cntObjView < st do //------------------- �������� �� ���� �� ��������� ���������
    begin
      if (cntObjView <= WorkMode.LimitObjView) then
      begin
        if (ObjView[cntObjView].TypeObj <> 0) and not CalcCRC_OV(cntObjView) then
        begin
          InsNewArmCmd($7ffc,0); showSMsg (526,LastX,LastY,'������� ����� � ������ '+
          IntToStr(cntObjView)); SimpleBeep;
        end;
        inc(cntObjView);
      end else begin cntObjView := 1; break; end;
    end;

    //----- �������� ����������� ����� ������� ������� ObjUprav �� 20 ���� � ����� �������
    st := cntUpr + 20;
    while cntUpr < st do
    begin
      if cntUpr <= WorkMode.LimitObjUprav then
      begin
        if (ObjUprav[cntUpr].RU <> 0) and not CalcCRC_OU(cntUpr) then
        begin
          InsNewArmCmd($7ffc,0);  //-------------------------------------------������ � ��
          showSMsg(526,LastX,LastY,'������� ���������� � ������ '+IntToStr(cntUpr));
          SimpleBeep;
        end;
        inc(cntUpr);
      end else  begin  cntUpr := 1;  break; end;
    end;

    //----- �������� ����������� ����� ������� ������� OVBuffer �� 20 ���� � ����� �������
    st := cntOVBuffer + 20;
    while cntOVBuffer < st do
    begin
      if cntOVBuffer <= High(OVBuffer) then
      begin
        if((OVBuffer[cntOVBuffer].TypeRec <> 0) or (OVBuffer[cntOVBuffer].Jmp1 <> 0) or
        (OVBuffer[cntOVBuffer].Jmp2 <> 0)) and not CalcCRC_VB(cntOVBuffer) then
        begin
          InsNewArmCmd($7ffc,0);
          PutSMsg(526,LastX,LastY,'������ ����������� � ������ '+
          IntToStr(cntOVBuffer));  SimpleBeep;
        end;
        inc(cntOVBuffer);
      end else begin cntOVBuffer := 1; break; end;
    end;

    //------------------------------------------------------------------------------------
    //------ ����� ������������� ������ ������� ��������� ������� � ������������� ��������
    if WorkMode.ServerSync then
    begin //------------------------------------- ��������� ������������� ������� ��������
      st := (config.RMID - 3) * 12; //------------------ ���������� ����� ��� ������� ����
      GetSystemTime(gts); //------------------------------------- �������� ��������� �����
      if SyncTime then   //--- ������� ������������ �������
      begin
        if not ((gts.wMinute = st) and (gts.wSecond = 28)) then//���� ������ �������������
        SyncCmd := true; //--- ���������� ������� ������ ������������� �������� � ����� ��
      end else if (gts.wMinute = st) and (gts.wSecond = 28) then SyncTime := true;
    end;

    if ((StateRU and $20) = $20) then //------------------------------- ���� ���������� ��
    begin  WorkMode.KOK_TUMS := true;   WorkMode.OtvKom := true; end
    else begin WorkMode.OtvKom:= false; WorkMode.KOK_TUMS:= false; end;//��� ���������� ��

    if WorkMode.OtvKom <> lastCurOK then  //--------------------- ���������� ��������� ���
    begin
      lastCurOK := WorkMode.OtvKom;
      if WorkMode.OtvKom then InsNewArmCmd($7fe6,0) //----------------- ������� � ����� ��
      else InsNewArmCmd($7fe5,0); //----------------- ����� �� ������ ������������� ������
    end;

    //------------------ ���� ����.������� ���� ������ ���, ���������� ������� ���� ������
    if WorkMode.CmdReady or WorkMode.MarhRdy then Screen.Cursor := crAppStart
    else
    begin //-------------------------------------------- ��� ����������� ������ ���-������
      if WorkMode.OtvKom then Screen.Cursor := curTablo1ok //-------��� ������� ������ ���
      else Screen.Cursor := crArrow; //------------------------ ������� ������ - ���������
    end;

    //----------- ��� ������������� ����� ������� ������ �� - �������� ������ � ������ ��.
    //-------------------------------------------------------------- ������ ����� ��������
    {   if WorkMode.OtvKom then begin ShowCursor(false); ShowCursor(true); end;
      if ChRegion then ChangeRegion(NewRegion); //------------ ��������� ������ ����������
    }
    if ChDirect then ChangeDirectState(StDirect); //------- ��������� ��������� ����������

    if WorkMode.Upravlenie and WorkMode.OU[0] then
    begin//-------------------- ��� ����������� ���� �������� ������ ��������� �����������
      if(ObjWav[1]<>'') and (IpWav[1]<>'') then
      begin
        if IpBeep[1] then //------------------------------------------------ 1 �����������
        begin IpBeep[1] := false; PlaySound(PAnsiChar(IpWav.Strings[0]),0,SND_ASYNC);end
        else if IpBeep[2] then //------------------------------------------- 2 �����������
        begin IpBeep[2] := false; PlaySound(PAnsiChar(IpWav.Strings[1]),0,SND_ASYNC); end;
      end;

      for I := 1 to 6 do
      if SBeep[i] then
      begin SBeep[i]:=false;PlaySound(PAnsiChar(ObjWav[i]),0,SND_ASYNC);end;

      if MsgStateRM <> '' then //-------------------- ����� ��������� �� ��������� �������
      begin putSMsg(MsgStateClr,LastX,LastY,MsgStateRM);  MsgStateRM := ''; end;

      //--------- ��������� ������ ��� - ���, ������������ ���� ���������� ������� �������
      IkonPri := $1f;
      if (StartTime+15/86400) < LastTime then //-------- ������� 15 ������ �� ������� ����
      IkonPri := IkonPri and $ff-$10;//- �� ��������� ���� �������� ������� �� ������ ����
      if IkonNew then IkonPri := IkonPri and $ff-$8;
      if WorkMode.Upravlenie then IkonPri := IkonPri and $ff-$4;
      if config.ru = config.def_ru then IkonPri := IkonPri and $ff-$2;
     if config.Master then IkonPri := IkonPri and $ff-$1;
    end;

    //--------------------- ����� ��������� ��������, ����������� ����� ��������� ��������
    //------------------------- ������������� �� ������ ��� �� ���������� ������� ��������
    if SendToSrvCloseRMDSP and (CmdSendT + (3/86400)< LastTime) then //������� ���� ��-���
    begin IsCloseRMDSP := true; Close; end;

    //------------------------ �������� �������� ��������� �� ������� ����������� ��������
    if (CmdCnt = 0) and WorkMode.CmdReady and SendRestartServera then
    begin SendRestartServera := false;  WorkMode.CmdReady := false;  end;

    if ((CmdCnt > 0) or(WorkMode.MarhRdy) or (WorkMode.CmdReady)) then
    begin //-------------------------------------------------------- ����� ������ ��������
      if WorkMode.LockCmd then
      begin //------------------ ���� ���������� ������ - �������� ������ ������ �� ������
        if StartRM then
        begin
          if CmdSendT + (10/86400) < LastTime then
          begin //---- ��������� ����� �������� ������� ��������������� ������� ����������
            InsNewArmCmd($7ffd,0);  CmdCnt := 0;  WorkMode.MarhRdy := false;
            WorkMode.CmdReady := false; StartRM := false; //----------------- ����� ������
          end;
        end else
        if not SendToSrvCloseRMDSP then CmdSendT := LastTime;
      end else
      begin //-------------------------- ��������� ����� �������� ������ ������� �� ������
        if CmdSendT + (5/86400) < LastTime then
        begin //----------------------------------------- ��������� ����� �������� �������
          InsNewMsg(0,296,0,'');  //-------------------------------------- "�������� �������"
          CmdCnt := 0;WorkMode.MarhRdy := false;WorkMode.CmdReady := false; //����� ������
          //-------------------------------------------------------- ���� �� ����� �������
          if not StartRM then AddFixMes(GetSmsg(1,296,GetNameObj(CmdBuff.LastObj),0),4,1);
        end;
      end;
    end;

    if StartRM and (StartTime < LastTime - 10/86400) then
    begin //----------------------------------------------------- ��������� ������ �������
      if (KanalSrv[1].issync or KanalSrv[2].issync) and not WorkMode.LockCmd then
      StartRM := false;
    end;

    if(TimeLockCmdDsp<(LastTime-0.5/86400))and LockComDsp then LockComDsp := false;
    MainTimer.Enabled := true;
  end;
end;

//========================================================================================
//---------------------------------------------------------------- ���������� ������ �����
function TTabloMain.RefreshTablo : Boolean;
var
  Delta : Double;
  a,b : boolean;
  Xstr : SmallInt;
  Txt1 : String;
begin
  LastSync := LastTime;
  if SVAZ <> config.SVAZ_TUMS[1][1] then config.SVAZ_TUMS[1][1] := SVAZ else
  if SVAZ <> config.SVAZ_TUMS[1][2] then config.SVAZ_TUMS[1][2] := SVAZ else
  if SVAZ <> config.SVAZ_TUMS[1][3] then config.SVAZ_TUMS[1][3] := SVAZ;

  PrepareOZ; //------------------------------------- ���������� ���� �������� ������������

  //----------------------------------- ������ �� ���������� ������� ������������� �������
  if OtvCommand.Active then //------ ���� ���� ������ �������������� ������������� �������
  begin
    with OtvCommand do
    begin// �������������� ������ ��������� � ��������� "STAN",
      if State <> GetFR3(Check,a,b) then Ready := false; //------------- ��� �������������
      //-------------------------------------------------------- ���������� ��������� ����
      Delta := Reper - LastTime; //--------------------------------------- ������� �������
      if Delta > 0 then
      begin
        if not WorkMode.OtvKom then //--------- ���� ��������� ������ ������������� ������
        begin
          Active := false; WorkMode.GoOtvKom := false; Ready := false;
          showSMsg (156,LastX,LastY,'');InsNewMsg(0,156,1,'');//---- ���� ���.������� �������
        end else
        if Second > 0 then
        begin
          if Ready then
          begin //--------------------------------- ��������������� �������������� �������
            Active := false;   WorkMode.GoOtvKom := false;  RSTMsg;
            AddFixMes(GetSmsg(1,155,'',1),1,1);//--------------"������� ������� ������ ��"
            InsNewMsg(0,155,1,'');
          end else
          if (Second = Cmd) and (Obj = SObj) then // ��������� ������� �� ��������� ������
          begin //-------------------------------------- ���������� �������������� �������
            Active := false;    WorkMode.GoOtvKom := false;  Ready := false;
            DspCom.Com := Second; DspCom.Obj := SObj; DspCom.Active  := true;
            SelectCommand;  //------------------------------- ��������� ������� ����������
          end else
          begin //------------------------------ ������ ������� �� ������������� ���������
            Active := false;  WorkMode.GoOtvKom := false;  Ready := false; RSTMsg;
            AddFixMes(GetSmsg(1,153,'',1),1,1);//------- "������. ������������ ���������."
            InsNewMsg(0,153,1,'');
          end;
        end else
        begin //------------------------------- ���������� �������� �������������� �������
          if not(Ready or (SObj > 0)) then//���� ����� �� �����.,��� ���� ������ ��������.
          begin //-------------------------------------------------- ����� ������� �������
            delta := delta * 84000;
            putSMsg(7,LastX,LastY,GetSmsg(1,154,'',1)+FloatToStrF(delta,ffFixed,2,0));
          end;
        end;
      end else
      begin //--------------------------------------- ����� �������� �� ���������� �������
        Active := false;  WorkMode.GoOtvKom := false; Ready := false;
        showSMsg (152, LastX, LastY, '');//---------------------"��������� ����� ��������"
        InsNewMsg(0,152,1,'');
      end;
    end;
  end else
  with VspPerevod do
  begin //------------------- �������� ������� ������ ���������� �������� ��� ���.��������
    if Active and (Strelka > 0) and (Strelka <= WorkMode.LimitObjZav) then
    begin
      Xstr := ObjZv[Strelka].BasOb; //------------------ ��������� �� ������ ������ �������
      if (Xstr > 0) and (Xstr <= WorkMode.LimitObjZav) then
      begin
        Delta := Reper - LastTime; //------------------------------------- ������� �������
        if Delta > 0 then
        begin
          if ObjZv[ObjZv[Xstr].ObCI[13]].bP[1] then
          //---------------- ���� ������ ������ ���������������� �������� - ������ �������
          begin
            Active := false; WorkMode.VspStr := false;  FixVspPerevod := false;
            DspCom.Com := Cmd;  DspCom.Obj := Strelka;  DspCom.Active  := true;
            SelectCommand;  //------------------------------- ��������� ������� ����������
          end else
          begin //----------------------------------- �� ������ ������ ���������� ��������
            delta := delta * 84000;
            Txt1 := FloatToStrF(delta,ffFixed,2,0);
            case Cmd of
              CmdStr_ReadyVPerevodPlus : //----------- ��������������� ������� ������� � +
              begin //--------------------------------------- ��������� ������� - ��������
                sMsg0 := GetSmsg(1,99,ObjZv[Xstr].Liter,7) + Txt1;
                putSMsg(7, LastX, LastY, sMsg0 );
                if not FixVspPerevod then
                begin InsNewMsg(Xstr,99,7,''); FixVspPerevod := true; end;
              end;
              CmdStr_ReadyVPerevodMinus : //---------- ��������������� ������� ������� � -
              begin //---------------------------------------- ��������� �������- ��������
                sMsg0:=GetSMsg(1,100,ObjZv[Xstr].Liter,7)+ Txt1;
                PutSMsg(7, LastX, LastY, sMsg0);
                if not FixVspPerevod then
                begin  InsNewMsg(Xstr,100,7,''); FixVspPerevod := true; end;
              end;
            end;
          end;
        end else
        begin //----------------------------------------------------- �������� ������� ���
          Active := false;
          WorkMode.VspStr := false;
          FixVspPerevod := false;
          ShowSMsg(148, LastX, LastY, ObjZv[Xstr].Liter); // "��������� ����� ��� ���"
          InsNewMsg(Xstr,148,1,'');
        end;
      end else
      begin //---------------------------------- �������� ������� ��� ��� ����������� ����
        Active := false;
        WorkMode.VspStr := false;
        FixVspPerevod := false;
      end;
    end;
  end;
  mem_page := not mem_page; //----- ������������� ���������� � ����������� �����1 / �����2

  if mem_page then DrawTablo(Tablo2) //----------------- ���������� �����2 ��� �����������
  else DrawTablo(Tablo1);            //----------------- ���������� �����1 ��� �����������

  Invalidate;              //------------------------------------------ ����������� ������
  result := true;
end;

//========================================================================================
//----------------------------------------- ���������� ������� ��������� ��������� �������
procedure TTabloMain.BeepTimerTimer(Sender: TObject);
begin
  if WorkMode.Upravlenie and WorkMode.OU[0] and Zvuk and (ObjWav[3]<>'') then
  PlaySound(PAnsiChar(ObjWav[3]),0,SND_ASYNC);
end;

//========================================================================================
//------------------------------------------------ ��������� �������� ������������� ������
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

//========================================================================================
//------------------------------------ ���������� ��������� �������� ���������� � ��������
procedure PresetObjParams;
var
  i : integer;
begin
  for i := 1 to High(ObjZv) do
  with ObjZv[i] do
  case TypeObj of
    2 : begin bP[4] := false; bP[5] := false; end; //--------------------------- �������
    3 :
    begin //------- ������, ��, �, ���, ��,��������������� ���������,��������� ���������
      bP[1]:= true;bP[2]:= true; bP[4]:= true;bP[5]:= true;bP[8]:= true; bP[10]:= true;
    end;
    4 :
    begin // ����: �(�),��,��,��(�),��(�),����. ���������,
      bP[1]:= true;bP[2]:= true;bP[3]:= true; bP[5]:= true; bP[6]:= true; bP[8]:= true;
      bP[10]:= true; bP[16]:= true; //------------------------ ��������� ���������, �(�)
    end;
    5  : begin iP[1]:= 0; iP[2]:= 0; iP[3]:= 0; end; //-------------------------- ������
    15 : begin bP[6] := false; bP[9] := false; end; //------------------------------- ��
    34 : begin bP[1] := true;  bP[2] := true; end; //--------------------------- �������
    38 : bP[1] := false; //-------------------------------------------- �������� �������
  end;
{$IFDEF DEBUG}
  //------------------------------------------ ���������� ������� ���������� �������� FR3s
  for i:= 1 to FR_LIMIT do begin FR3s[i]:= LastTime;FR4s[i]:= LastTime; end;
{$ENDIF}
end;

//========================================================================================
//------------------------------ ��������� ������������ ���������� -> ������ -> ����������
procedure ChangeDirectState(State : Boolean);
var
  i: Integer;
  f : Byte;
begin
  ChDirect := false;

  for i := 1 to 16 do
  begin
    config.SVAZ_TUMS[i][1] := 0; config.SVAZ_TUMS[i][2] := 0; config.SVAZ_TUMS[i][3] := 0;
  end;

  for i := 1 to High(ObjZv) do
  with ObjZv[i] do
  begin //-------------------------------------------------- �������� �������� �����������
    case TypeObj of
      1 ://------------------����� �������: �������� ��, ��,�������� ����������� ���������
      begin bP[6]:= false; bP[7]:= false; bP[14]:= false;  end;

      2 ://�������: �������� ��,��,�������� �������,����������� ���������, ������ ��������
      begin
        bP[6]:= false;bP[7]:=false;bP[10]:=false;bP[11]:=false;bP[12]:=false;
        bP[13] := false;bP[14] := false; iP[1] := 0;
      end;

      3 : //-------- �������:�������� �����.���������,�������� ���� �,���, ��������� �����
      begin //---- �������� �������� �������������,�������� ������ ��������� � �����������
        bP[14] := false; iP[1] := 0; iP[2] := 0; bP[8] := true;
        bP[19] := false; bP[22] := false; bP[23] := false;
      end;

      4 : //-------------- ����: �������� �����.���������,������ ��������, ��������� �����
      begin //�������� �������� �������������,�������� ������ ��������� � �����������,����
        bP[14] := false;  iP[1] := 0; bP[8] := true;
        bP[19] := false;  bP[21] := false; bP[22] := false; bP[23] := false;
      end;

      5 : //------------ ������: �������� �����.���������,������ ��������, ��������� �����
      begin //---- �������� �������� �������������, ���������� � ���������� ��������������
        bP[14] := false; iP[1] := 0; bP[7] := false; bP[9] := false;
        bP[19] := false; bP[20] := false; bP[22] := false; bP[23] := false;
      end;

      15 : begin bP[14] := false;  bP[15] := false; end;//--- �� ����� ��������� � ������

      25 : begin bP[14] := false; bP[8] := false; end; //��: ����� ��������� ������ ������
    end;
  end;

  for i := 1 to High(ObjZv) do
  with ObjZv[i] do
  begin //----------------------------------- ���������� ��������� ��������, ��������� FR4
    case TypeObj of
      1 : //---------------------------------------------------------------- ����� �������
      begin //----------------------------- ����� FR4
        f := fr4[ObCI[1] div 8]; bP[19] := (f and $2) = $2; //---------------------- �����
        if bP[19] and (RU = config.ru) then //- �������� ����� ������� �� �������� �������
        begin maket_strelki_index := i; maket_strelki_name := Liter; end;

        if ObCI[8] > 0 then//----------------------------- ����������� ��� ������� �������
        begin
          ObjZv[ObCI[8]].bP[15] := bP[19];//---------------------------------------- �����
          ObjZv[ObCI[8]].bP[16] := (f and $4) = $4; //------------------------- ����.����.
          ObjZv[ObCI[8]].bP[18] := (f and $1) = $1; //-------------------------- ����.���.
          ObjZv[ObCI[8]].bP[17] := (f and $10) = $10;//------------------------ ������� ��
        end;

        if ObCI[9] > 0 then //---------------------------- ����������� ��� ������� �������
        begin
          ObjZv[ObCI[9]].bP[15] := bP[19]; //--------------------------------------- �����
          ObjZv[ObCI[9]].bP[16] := (f and $8) = $8; //------------------------- ����.����.
          ObjZv[ObCI[9]].bP[18] := (f and $1) = $1; //-------------------------- ����.���.
          ObjZv[ObCI[9]].bP[17] := (f and $20) = $20;//------------------------ ������� ��
        end;
      end;

      3 : //---------------------------------------------------------------------- �������
      begin
        f := fr4[ObCI[1]]; //----------------------------------- ����� FR4
        bP[25] := (f and $1) = $1; //------------------------- ����.����.����.�
        bP[26] := (f and $2) = $2; //-------------------------- ����.����.���.�
        bP[13] := (f and $4) = $4; //------------------------------- ����.����.
        bP[24] := (f and $8) = $8; //---------------------------- ����.����.�.�
      end;

      4 :
      begin //--------------------------------------------------------------------- ����
        f := fr4[ObCI[1]]; //----------------------------------- ����� FR4
        bP[25] := (f and $1) = $1; //------------------------- ����.����.����.�
        bP[26] := (f and $2) = $2; //-------------------------- ����.����.���.�
        bP[13] := (f and $4) = $4; //------------------------------- ����.����.
        bP[24] := (f and $8) = $8; //---------------------------- ����.����.�.�
      end;

      5 :
      begin //----------------------------------------------------------------- ��������
        f := fr4[ObCI[1]]; //----------------------------------- ����� FR4
        bP[13] := (f and $4) = $4; //----------------------------- ������������
      end;

      15 :
      begin //----------------------------------------------------------------------- ��
        f := fr4[ObCI[3] div 8]; //----------------------------- ����� FR4
        bP[25] := (f and $1) = $1; //------------------------- ����.����.����.�
        bP[26] := (f and $2) = $2; //-------------------------- ����.����.���.�
        bP[13] := (f and $4) = $4; //------------------------------- ����.����.
        bP[24] := (f and $8) = $8; //---------------------------- ����.����.�.�
      end;

      24 :
      begin //-------------------------------------------------- ������ ����� ������� ��
        f := fr4[ObCI[8] div 8]; //----------------------------- ����� FR4
        bP[25] := (f and $1) = $1; //------------------------- ����.����.����.�
        bP[26] := (f and $2) = $2; //-------------------------- ����.����.���.�
        bP[15] := (f and $4) = $4; //------------------------------- ����.����.
        bP[24] := (f and $8) = $8; //---------------------------- ����.����.�.�
      end;

      26 :
      begin //---------------------------------------------------------------------- ���
        f := fr4[ObCI[13] div 8]; //---------------------------- ����� FR4
        bP[25] := (f and $1) = $1; //------------------------- ����.����.����.�
        bP[26] := (f and $2) = $2; //-------------------------- ����.����.���.�
        bP[13] := (f and $4) = $4; //------------------------------- ����.����.
        bP[24] := (f and $8) = $8; //---------------------------- ����.����.�.�
      end;

      32 :
      begin //------------------------------------------------------------------- ������
        f := fr4[ObCI[1]]; //----------------------------------- ����� FR4
        bP[13] := (f and $4) = $4; //----------------------------- ������������
      end;
    end;
  end;
  for i := 1 to 6 do SBeep[i] := false;
  IpBeep[1] := false; IpBeep[2] := false;

  if not WorkMode.RU[0] and WorkMode.OU[0] then //------------------ ���������� �� ��� ���
  begin
    if WorkMode.DU[0] then  //------------------------------ ���� ������������� ����������
    begin//----------------------------------------- "��-��� ��������� � ���������� �� ��"
      InsNewMsg(0,571,0,'');sMsg0:=GetSMsg(1,571,'',0);PutSMsg(9,LastX,LastY,sMsg0);
      AddFixMes(sMsg0,0,0);  WorkMode.Upravlenie := false;
    end else
    if not WorkMode.Upravlenie then //-------------------------- ���� ������� � ����������
    begin //- ������������ ������ -> ���������� "��-��� ��������� � ����������� ���������"
      InsNewMsg(0,273,2,'');sMsg0:=GetSMsg(1,273,'',2);PutSMsg(5,LastX,LastY,sMsg0);
      AddFixMes(sMsg0,0,0);  WorkMode.Upravlenie := true;
    end else
    if WorkMode.Upravlenie then //-------------------------------- ���� ������� � ������
    begin//------------------------------------------------- "��-��� ���������� � ������"
      WorkMode.Upravlenie := false; InsNewMsg(0,274,7,'');  sMsg0:=GetSMsg(1,274,'',7);
      PutSMsg(7,LastX, LastY, sMsg0); AddFixMes(sMsg0,0,0);
    end;
  end else
  begin  //------------------------------------------------- ��������� ���������� �� �����
    WorkMode.Upravlenie := false; InsNewMsg(0,570,1,''); sMsg0:= GetSMsg(1,570,'',1);
    AddFixMes(sMsg0,0,0); PutSMsg(1,LastX, LastY, sMsg0);
  end;
end;

//========================================================================================
//-------------------------------------------------------------- �������� ����� ����������
procedure ChangeRegion(RU : Byte);
var
  i: Integer;
  f : Byte;
begin
  try
    ChRegion := false;
    if ChDirect or WorkMode.Upravlenie or SendToSrvCloseRMDSP then exit;
    config.ru := RU;
    //------------------------------------------- ���������� ������� ������ ������� ������
    PasswordPos.X := configRU[config.ru].MsgLeft+1;
    PasswordPos.Y := configRU[config.ru].MsgTop+1;

    for i := 1 to High(ObjZv) do //----------------- ������ �� ���� ��������  ������������
    begin //------------------------------------------------ ���������� ��������� ��������
      case ObjZv[i].TypeObj of
        1 :
        begin //------------------------------------------------------------ ����� �������
          f := fr4[ObjZv[i].ObCI[1] div 8]; //-------- �������� ������ ��� ������ FR4
          if ((f and $2)=$2) and (ObjZv[i].RU = config.ru) then //- ���� ������� �� ������
          begin //----------------------------- �������� ����� ������� �� �������� �������
            maket_strelki_index := i;
            maket_strelki_name  := ObjZv[i].Liter;
          end;
        end;
      end;
    end;
    ResetAllPlakat; //------------------------------------------------- ������ ��� �������
    SetParamTablo;
  except
    RepF('������ [TabloForm.ChangeRegion]'); Application.Terminate;
  end;
end;
//========================================================================================
//------------------------------------------ ���������� ������� �� ��������� �������� ����
procedure TTabloMain.FormCanResize (Sender :TObject; var NewWidth, NewHeight : Integer;
var
  Resize : Boolean);
begin
  Resize := isChengeRegion;
end;

//========================================================================================
//------------------------------------------- ��������� ������� ��� (���� �������� ������)
procedure TTabloMain.ASUTimer(Sender: TObject);
begin
  IkonSend := true; //-- ������������ �������������� ����� �� ������� ��� � ������ �������
end;

end.

