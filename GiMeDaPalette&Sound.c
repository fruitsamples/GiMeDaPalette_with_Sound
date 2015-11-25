/*	File:		GiMeDaPalette&Sound.c	Contains:	This is the original version of GiMeDaPalette with the change to it to make it 				possible to play a sound at the same time as the colors are being animated.  				The method which is being used to play the sound is to use Double Buffering.  				When ever you select some form of Animation from the menu, there will be sound 				to go with it.	Written by: RICHARD P. COLLYER   		Copyright:	Copyright � 1990-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				8/2/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*/#include	<CType.h>#include	<Quickdraw.h>#include	<Windows.h>#include	<desk.h>#include	<dialogs.h>#include	<Events.h>#include 	<GestaltEqu.h>#include	<Memory.h>#include	<Menus.h>#include	<OSEvents.h>#include	<Palettes.h>#include	<SegLoad.h>#include	<Sound.h>#include	<Resources.h>#include	<ToolUtils.h>#include	<StdIO.h>#include	<QDOffscreen.h>extern _DataInit();#define pmInhibitG2		0x0100#define pmInhibitC2		0x0200#define pmInhibitG4		0x0400#define pmInhibitC4		0x0800#define pmInhibitG8		0x1000#define pmInhibitC8		0x2000#define	TRUE			0xFF#define	FALSE			0#define Gestalttest		0xA1AD#define NoTrap			0xA89F#define	appleID			128			#define	appleMenu		0#define	aboutMeCommand	1#define	fileID			129#define	quitCommand 	1#define	PaletteID		130#define	CourCommand 	1#define	TolCommand 		2#define	ExpCommand 		3#define	AniCommand 		4#define	TolExpCommand 	6#define	TolAniCommand 	7#define	ExpAniCommand 	8#define	TEACommand 		10#define	PictID			128#define	clutID			150#define	aboutMeDLOG		128#define	okButton		1#define	authorItem		2#define	languageItem	3#define	numcolor		256#define kDoubleBufferSize	0x1000struct LocalVars {    long	bytesTotal;    long	bytesCopied;    Ptr		dataPtr;};typedef struct LocalVars LocalVars;typedef LocalVars *LocalVarsPtr;Rect					TotalRect, minRect, WinMinusScroll, InitWindowSize;WindowPtr				myWindow;CTabHandle				mycolors;PaletteHandle			srcPalette;Boolean					DoneFlag;MenuHandle				mymenu1, mymenu2, mymenu0;PicHandle				ThePict;GWorldPtr				offscreenGWorld;SndDoubleBufferHeader	doubleHeader;SndDoubleBufferPtr		doubleBuffer;SCStatus				Stats;SndChannelPtr			chan;SoundHeaderPtr			Head;Handle					SoundData;LocalVars				myVars;SndDoubleBackUPP        gSndDoubleBackUPP;pascal void MyDoubleBackProc (SndChannelPtr channel, SndDoubleBufferPtr doubleBufferPtr);void CheckMenu(int whichMenu);void SoundSetUp();void SoundOff();void Draw();void SetInhibited(int Usage);void showAboutMeDialog();void doCommand(long mResult);void init();/*______________________________________________________*//*              Sound Double Back Proc                  *//*______________________________________________________*/pascal void MyDoubleBackProc (SndChannelPtr chan,SndDoubleBufferPtr doubleBuffer){	#pragma unused(chan)LocalVarsPtr	myVarsPtr;long			bytesToCopy;myVarsPtr = (LocalVarsPtr) doubleBuffer->dbUserInfo[0];bytesToCopy = myVarsPtr->bytesTotal - myVarsPtr->bytesCopied;if (bytesToCopy > kDoubleBufferSize)	bytesToCopy = kDoubleBufferSize;	BlockMove (myVarsPtr->dataPtr, &doubleBuffer->dbSoundData[0], bytesToCopy);doubleBuffer->dbNumFrames = bytesToCopy;doubleBuffer->dbFlags = (doubleBuffer->dbFlags) | dbBufferReady;myVarsPtr->dataPtr = (Ptr) ((myVarsPtr->dataPtr) + bytesToCopy);myVarsPtr->bytesCopied = myVarsPtr->bytesCopied + bytesToCopy;if (myVarsPtr->bytesCopied == myVarsPtr->bytesTotal) {	doubleBuffer->dbFlags = (doubleBuffer->dbFlags) | dbLastBuffer;	}	return;}/*______________________________________________________*//*                Check Correct Menu                    *//*______________________________________________________*/void CheckMenu(int whichMenu)/* This procedure checks and unchecks the appropriate menus */{			CheckItem(mymenu2,CourCommand,FALSE);			CheckItem(mymenu2,TolCommand,FALSE);			CheckItem(mymenu2,ExpCommand,FALSE);			CheckItem(mymenu2,AniCommand,FALSE);			CheckItem(mymenu2,TolExpCommand,FALSE);			CheckItem(mymenu2,TolAniCommand,FALSE);			CheckItem(mymenu2,ExpAniCommand,FALSE);			CheckItem(mymenu2,TEACommand,FALSE);	switch (whichMenu) {		case CourCommand:			CheckItem(mymenu2,CourCommand,TRUE);			break;		case TolCommand:			CheckItem(mymenu2,TolCommand,TRUE);			break;		case ExpCommand:			CheckItem(mymenu2,ExpCommand,TRUE);			break;		case AniCommand:			CheckItem(mymenu2,AniCommand,TRUE);			break;		case TolExpCommand:			CheckItem(mymenu2,TolExpCommand,TRUE);			break;		case TolAniCommand:			CheckItem(mymenu2,TolAniCommand,TRUE);			break;		case ExpAniCommand:			CheckItem(mymenu2,ExpAniCommand,TRUE);			break;		case TEACommand:			CheckItem(mymenu2,TEACommand,TRUE);			break;		defalut:			break;		}}/*______________________________________________________*//*                     Start Sound                      *//*______________________________________________________*/void SoundSetUp(){	int				i;	OSErr			err;		chan = nil;	err = SndNewChannel (&chan, sampledSynth, 0, nil);	if (err != noErr)		Debugger();		Head = (SoundHeaderPtr) NewPtrClear (sizeof(SoundHeader));	Head->samplePtr = *SoundData;	Head->length = 45838;	Head->sampleRate = 0x56EE8BA3;		//recorded at 22KHz	Head->loopStart = 0;	Head->loopEnd = 0;	Head->encode = 0;	Head->baseFrequency = 60;	Head->sampleArea[0] = 0;	myVars.bytesTotal = Head->length;	myVars.bytesCopied = 0;	myVars.dataPtr = Head->samplePtr;	doubleHeader.dbhNumChannels = 1;	doubleHeader.dbhSampleSize = 8;	doubleHeader.dbhCompressionID = 0;	doubleHeader.dbhPacketSize = 0;	doubleHeader.dbhSampleRate = Head->sampleRate;		doubleHeader.dbhDoubleBack = gSndDoubleBackUPP;	for (i = 0; i <= 1; ++i) {		doubleBuffer = (SndDoubleBufferPtr) NewPtrClear (sizeof(SndDoubleBuffer) + kDoubleBufferSize);			if (doubleBuffer == nil || MemError() != 0)			Debugger();				doubleBuffer->dbNumFrames = 0;		doubleBuffer->dbFlags = 0;		doubleBuffer->dbUserInfo [0] = (long) &myVars;			MyDoubleBackProc (chan, doubleBuffer);			// initialize the buffers			doubleHeader.dbhBufferPtr [i] = doubleBuffer;		}}/*______________________________________________________*//*                     Start Sound                      *//*______________________________________________________*/void SoundOff(){	OSErr		err;		err = SndPlayDoubleBuffer (chan, &doubleHeader);	if (err != noErr)		Debugger();}/*______________________________________________________*//*             Set Up Usage of Palette                  *//*______________________________________________________*/void Draw(){	RGBColor		black = {0,0,0}, White = {65535,65535,65535};			RGBForeColor (&black);	RGBBackColor (&White);	CopyBits ((BitMap *) *offscreenGWorld->portPixMap, &myWindow->portBits, 					&InitWindowSize, &WinMinusScroll, srcCopy, nil);}/*______________________________________________________*//*             Set Up Usage of Palette                  *//*______________________________________________________*/void SetInhibited(int Usage)/* When the usage of the palette changes I need to rebuild the palette,so I call this routine.  I need to call SetEntryUsage on all of the entriesof the palette, but I want to make sure that if the monitor is set to a bit depthlow than 8 bits that the palette is limited in its colors.  So the first two colors(not including white) are set to work at all depths except 1 bit mode.  The next 12 entries are set to work on all depths except 1 & 2 bit modes.  The rest of the colors are set to only work in 8 bit mode.  I have also set the tolerance tohex 1500.  With this tolerance I will be able to get most of the colors I need and those that I don't get exact will be very close. */{	int		i;		CTab2Palette (mycolors, srcPalette, Usage, 0x1500);	for (i = 1; i <= 2; ++i) {		SetEntryUsage (srcPalette, i, Usage, 0x1500);		}	for (i = 3; i <= 14; ++i) {		SetEntryUsage (srcPalette, i, Usage+pmInhibitG2+pmInhibitC2, 0x1500);		}	for (i = 15; i < numcolor; ++i) {		SetEntryUsage (srcPalette, i, Usage+pmInhibitG2+pmInhibitC2+						pmInhibitG4+pmInhibitC4, 0x1500);		}}/*______________________________________________________*//*                 About Prog Dialog                    *//*______________________________________________________*/void showAboutMeDialog(){	GrafPtr 	savePort;	DialogPtr	theDialog;	short		itemHit;	GetPort(&savePort);	theDialog = GetNewDialog(aboutMeDLOG, nil, (WindowPtr) -1);	SetPort(theDialog);	do {		ModalDialog(nil, &itemHit);	} while (itemHit != okButton);	CloseDialog(theDialog);	SetPort(savePort);	return;}/*______________________________________________________*//*                 Do Menu Function                     *//*______________________________________________________*/void doCommand(long mResult){	int 					theMenu, theItem;	Str255					daName;	GrafPtr 				savePort;	theItem = LoWord(mResult);	theMenu = HiWord(mResult);		switch (theMenu) {/*______________________________________________________*//*                    Do Apple Menu                     *//*______________________________________________________*/		case appleID:			if (theItem == aboutMeCommand)				showAboutMeDialog();			else {				GetMenuItemText(mymenu0, theItem, daName);				GetPort(&savePort);				(void) OpenDeskAcc(daName);				SetPort(savePort);			}			break;/*______________________________________________________*//*                     Do File Menu                     *//*______________________________________________________*/		case fileID:			switch (theItem) {				case quitCommand:					DoneFlag = TRUE;					break;				default:					break;				}			break;/*______________________________________________________*//*                     Do File Menu                     *//*______________________________________________________*/		case PaletteID:			switch (theItem) {				case CourCommand:					CheckMenu(CourCommand);					SetInhibited(pmCourteous);					break;				case TolCommand:					CheckMenu(TolCommand);					SetInhibited(pmTolerant);					break;				case ExpCommand:					CheckMenu(ExpCommand);					SetInhibited(pmExplicit);					break;				case AniCommand:					CheckMenu(AniCommand);					SetInhibited(pmAnimated);					SoundOff();					break;				case TolExpCommand:					CheckMenu(TolExpCommand);					SetInhibited(pmTolerant+pmExplicit);					break;				case TolAniCommand:					CheckMenu(TolAniCommand);					SetInhibited(pmTolerant+pmAnimated);					SoundOff();					break;				case ExpAniCommand:					CheckMenu(ExpAniCommand);					SetInhibited(pmAnimated+pmExplicit);					SoundOff();					break;				case TEACommand:					CheckMenu(TEACommand);					SetInhibited(pmTolerant+pmExplicit+pmAnimated);					SoundOff();					break;				default:					break;				}			SetPalette ((WindowPtr) myWindow, srcPalette, TRUE);			ActivatePalette ((WindowPtr) myWindow);			break;		}	HiliteMenu(0);	return;}/*______________________________________________________*//*               Initialization traps                   *//*______________________________________________________*/void init(){	RgnHandle			tempRgn;	Rect				BaseRect;	OSErr				err;	long				QDfeature, OSfeature;	GDHandle			SaveGD;	CGrafPtr			SavePort;	RGBColor			black = {0,0,0}, White = {65535,65535,65535};	//UnloadSeg(_DataInit);	InitGraf(&qd.thePort);	FlushEvents(everyEvent, 0);	InitMenus();	InitWindows();	InitDialogs(nil);	InitCursor();		/* set up the UPP for MyDoubleBackProc */	gSndDoubleBackUPP = NewSndDoubleBackProc(MyDoubleBackProc);		DoneFlag = FALSE;	ThePict = GetPicture(PictID);	if (ThePict == nil)		DoneFlag = TRUE;/*______________________________________________________*//*            Use Gestalt to find 32BQD                 *//*______________________________________________________*/	err = Gestalt(gestaltQuickdrawVersion, &QDfeature);	if (err && (QDfeature & 0x0f00) < 0x0200)		DoneFlag = TRUE;	err = Gestalt(gestaltSystemVersion, &OSfeature);	if (err)		DoneFlag = TRUE;	if (!DoneFlag && (QDfeature & 0x0f00) != 0x0200 && OSfeature < 0x0605)		DoneFlag = TRUE;/*______________________________________________________*//*                     Set Rects                        *//*______________________________________________________*/if (DoneFlag == FALSE) {	SetRect(&BaseRect, 40, 60, 472, 282);	SetRect(&WinMinusScroll, BaseRect.left-40, BaseRect.top-60, BaseRect.right-60, 				BaseRect.bottom - 80);	SetRect(&InitWindowSize, WinMinusScroll.left, WinMinusScroll.top, 							WinMinusScroll.right, WinMinusScroll.bottom);	tempRgn = GetGrayRgn();	HLock ((Handle) tempRgn);	TotalRect = (**tempRgn).rgnBBox;	SetRect(&minRect, 80, 80, (**tempRgn).rgnBBox.right - 40, 				(**tempRgn).rgnBBox.bottom - 40);	HUnlock ((Handle) tempRgn);/*______________________________________________________*//*        Open Window & set Palette & Picture           *//*______________________________________________________*/	GetGWorld (&SavePort, &SaveGD);	mycolors = GetCTable (clutID);	(*mycolors)->ctFlags |= 0x4000;	myWindow = NewCWindow(nil, &BaseRect, "\p", TRUE, zoomDocProc, 							(WindowPtr) -1, TRUE, 150);	SetGWorld((CGrafPtr)myWindow, SaveGD);	DrawGrowIcon (myWindow);	srcPalette = NewPalette (numcolor, mycolors, pmCourteous, 0);	SetInhibited(pmCourteous);	SetPalette ((WindowPtr) myWindow, srcPalette, TRUE);		DrawPicture (ThePict, &InitWindowSize);	GetGWorld (&SavePort, &SaveGD);	err = NewGWorld (&offscreenGWorld, 8, &InitWindowSize, mycolors, nil, nil);	if (err)		Debugger();	SetGWorld (offscreenGWorld, nil);	EraseRect (&InitWindowSize);	DrawPicture (ThePict, &InitWindowSize);	SetGWorld (SavePort, SaveGD);		/*______________________________________________________*//*                    Set menus                         *//*______________________________________________________*/	mymenu0 = GetMenu(appleID);	AppendResMenu(mymenu0, 'DRVR');	InsertMenu(mymenu0,0);	mymenu1 = GetMenu(129);	InsertMenu(mymenu1,0);	mymenu2 = GetMenu(130);	InsertMenu(mymenu2,0);	DrawMenuBar();/*______________________________________________________*//*                  Set Up Sound                        *//*______________________________________________________*/	SoundData = GetResource ('snd ', 100);	if (ResError() != noErr || SoundData == nil)		Debugger();	HLock (SoundData);	SoundSetUp();	}		return;}main(){	char			key;	Boolean			track;	long			growResult;	EventRecord 	myEvent;	WindowPtr		whichWindow;	int				yieldTime;	RGBColor		changecolor;	CTabHandle		StoreCTab;	short			Usage, Tolerance;	OSErr			err;/*______________________________________________________*//*                   Main Event loop                    *//*______________________________________________________*/	init();	yieldTime = 0;	StoreCTab = GetCTable (clutID);	for ( ;; ) {		if (DoneFlag) {			ExitToShell();			}					/* If the usage is set to animate than animate the colors one step for eash event loop */		GetEntryUsage(srcPalette, 1, &Usage, &Tolerance);		Usage &= 0x0004;		if (Usage == 0x0004) {			err = SndChannelStatus (chan, sizeof (Stats), &Stats);			if (err != noErr)				Debugger();			if (!Stats.scChannelBusy) {				err = SndDisposeChannel (chan,FALSE);				if (err != noErr)					Debugger();						SoundSetUp();				SoundOff();				}			GetEntryColor (srcPalette, 1, &changecolor);			AnimatePalette (myWindow, StoreCTab, 2, 1, numcolor - 2);			AnimateEntry (myWindow, numcolor - 1, &changecolor);			Palette2CTab (srcPalette, StoreCTab);			}					if (WaitNextEvent(everyEvent, &myEvent, yieldTime, nil)) {			switch (myEvent.what) {				case mouseDown:					switch (FindWindow(myEvent.where, &whichWindow)) {						case inSysWindow:							SystemClick(&myEvent, whichWindow);							break;						case inMenuBar:							doCommand(MenuSelect(myEvent.where));							break;						case inContent:							break;						case inDrag:							DragWindow (whichWindow, myEvent.where, &TotalRect);							Draw();							DrawGrowIcon (whichWindow);							break;						case inGrow:							growResult = GrowWindow (whichWindow, myEvent.where,													&minRect);							SizeWindow(whichWindow, LoWord(growResult), 									HiWord(growResult), TRUE);							EraseRect(&whichWindow->portRect);							SetRect(&WinMinusScroll, whichWindow->portRect.left, 									whichWindow->portRect.top, 									whichWindow->portRect.right-20, 									whichWindow->portRect.bottom - 20);							Draw();							DrawGrowIcon (whichWindow);							break;						case inGoAway:							track = TrackGoAway (whichWindow, myEvent.where);							if (track) {								CloseWindow (whichWindow);								DoneFlag = TRUE;								}							break;						case inZoomIn:							track = TrackBox (whichWindow, myEvent.where, inZoomIn);							if (track) {								ZoomWindow (whichWindow, inZoomIn, TRUE);								EraseRect(&whichWindow->portRect);								SetRect(&WinMinusScroll, whichWindow->portRect.left, 										whichWindow->portRect.top, 										whichWindow->portRect.right-20, 										whichWindow->portRect.bottom - 20);								Draw();								DrawGrowIcon (whichWindow);								}							break;						case inZoomOut:							track = TrackBox (whichWindow, myEvent.where, inZoomOut);							if (track) {								ZoomWindow (whichWindow, inZoomOut, TRUE);								EraseRect(&whichWindow->portRect);								SetRect(&WinMinusScroll, whichWindow->portRect.left, 										whichWindow->portRect.top, 										whichWindow->portRect.right-20, 										whichWindow->portRect.bottom - 20);								Draw();								DrawGrowIcon (whichWindow);								}							break;						default:							break;						}					break;				case keyDown:				case autoKey:					key = myEvent.message & charCodeMask;					if ( myEvent.modifiers & cmdKey )						if ( myEvent.what == keyDown )							doCommand(MenuKey(key));					break;				case updateEvt:					if ((WindowPtr) myEvent.message == myWindow) {						BeginUpdate((WindowPtr) myWindow);						EndUpdate((WindowPtr) myWindow);						Draw();						}					break;				case diskEvt:					break;				case activateEvt:					break;				case app4Evt:					if ((myEvent.message << 31) == 0) { /* Suspend */						yieldTime = 30;						HideWindow((WindowPtr) myWindow);						}					else { /* Resume */						yieldTime = 0;						ShowWindow((WindowPtr) myWindow);						SetPort((WindowPtr) myWindow);						}					break;				default:					break;				}			}		}}