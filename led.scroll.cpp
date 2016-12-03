//****************************************************************************
//  Copyright (c) 2014  Daniel D Miller
//  lcd.scroll.exe - create tile-based scrolling-text application.
//  This project differs from Nic Wilson's LCD Tiles project
//  in that it uses bit-mapped raster fonts instead of 
//  static bitmap files.
//
//  Written by:  Dan Miller
//****************************************************************************
//  This is free software.  It is distributed under the terms of the
//  Code Project Open License (CPOL) V1.02 :
//  http://www.codeproject.com/info/cpol10.aspx
//
//  The main points subject to the terms of the License are:
//  
//  > Source Code and Executable Files can be used in commercial applications;
//  > Source Code and Executable Files can be redistributed; and
//  > Source Code can be modified to create derivative works.
//  > No claim of suitability, guarantee, or any warranty whatsoever is provided.
//    The software is provided "as-is".
//  > The Article accompanying the Work may not be distributed or republished 
//    without the Author's consent
//****************************************************************************
//  Revision history
//  1.00    01/31/14    Initial release
//  1.01    02/03/14    Modified MatrixStatic constructor to take a
//                      data structor which specfies all required params.
//                      This hides the sequence of ini operations from user.
//  1.02    02/04/14    Code cleanup, remove unused code
//  1.03    02/05/14    Sync lrender class with another utility
//  1.04    02/07/14    Build wShowFont utility into this program
//****************************************************************************

static char const * const Version = "LED Scrolling, Version 1.04" ;

#include <windows.h>
// #include <tchar.h>
#include <string>
#include <time.h>
using namespace std;

#include "resource.h"
#include "common.h"
#include "statbar.h"
#include "fontmgr.h"
#include "lrender.h"
#include "matrixstatic.h"

//  wshowfont.cpp
extern bool show_font_viewer(void);

//****************************************************************************
HINSTANCE g_hinst = 0;
static uint main_timer_id = 0 ;

static uint cxClient = 0 ;
static uint cyClient = 0 ;

// HWND hwndStatusBar ;
static CStatusBar *MainStatusBar = NULL;

//  local class instances
static CMatrixStatic *m_lcdleft;
static CMatrixStatic *m_lcdright;
static CMatrixStatic *m_lcdstaticsmall;
static CMatrixStatic *m_lcdup;
static CMatrixStatic *m_lcddown;
static CMatrixStatic *m_lcdstatic;
static CMatrixStatic *m_lcd_lrender;
static CMatrixStatic *m_lcd_clock;

static led_data_t led_left = {
   IDC_LCDLEFT, "Scrl Left..", "..\\fntcol\\readable.f08", 3, 1, 1, SQUARE_PIXELS, 
   13, 1, 10, MBD_LEFT, true, '!', RGB(0, 0, 0), RGB(255, 60, 0), RGB(103, 30, 0)
} ;

static led_data_t led_right = {
   IDC_LCDRIGHT, "Scrl Right..", "..\\fntcol\\readable.f08", 3, 1, 1, ROUND_PIXELS, 
   13, 1, 3, MBD_RIGHT, true, 0, RGB(0, 103, 30), RGB(0, 255, 50), RGB(0, 103, 30)
} ;

static led_data_t led_ssmall = {
   IDC_MATRIXSMALL, " !\"#$%&'()*+,-./0123456789;:<=>?"
                    "@ABCDEFGHIJKLMNO PQRSTUVWXYZ[\\]^"
                    "_`abcdefghijklmnopqrstuvwxyz{|}~", "..\\fntcol\\cp437.f14",
                     2, 0, 0, SQUARE_PIXELS, 32, 3, 4, MBD_RIGHT, false, 0,
                     DEFAULT_BGND, DEFAULT_SET, DEFAULT_CLEAR
} ;

static led_data_t led_scrl_up = {
   IDC_MATRIXUP, "    LCDMATRIX   "
                 "                "
                 " by  Nic Wilson "
                 " and Dan Miller "
                 "----------------"
                 "                ", "..\\fntcol\\script1.f14", 
                  1, 0, 0, SQUARE_PIXELS, 16, 3, 6, MBD_UP, false, 0,
                  RGB(165, 181, 66), RGB(0, 0, 0), RGB(148, 156, 66)
} ;

static led_data_t led_scrl_down = {
   IDC_MATRIXDOWN, "                "
                   "   Scroll Down  "
                   "                ", "..\\fntcol\\pc.f14", 
                  2, 0, 1, SQUARE_PIXELS, 16, 3, 6, MBD_DOWN, false, 0,
                  RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23)
} ;

static led_data_t led_static1 = {
   IDC_LCDSTATIC, " derelict ", "..\\fntcol\\roman2.f19",
   2, 0, 1, SQUARE_PIXELS, 0, 0, 0, MBD_STATIC, false, 0,
   RGB(0, 0, 192), RGB(0, 240, 240), RGB(0, 0, 192)
} ;

static led_data_t led_lrender = {
   IDC_LRENDER, "lrender class ", "script2.f19",
   1, 0, 1, SQUARE_PIXELS, 0, 0, 10, MBD_RIGHT, false, 0,
   RGB(0, 0, 0), RGB(0, 240, 240), RGB(0, 0, 192)
} ;

static led_data_t led_clock = {
   IDC_LRENDER2, " 00:00:00 ", "script2.f19", 
   2, 0, 1, SQUARE_PIXELS, 0, 0, 0, MBD_STATIC, false, 0,
   RGB(0, 0, 0), RGB(255, 128, 0), RGB(128, 64, 0)
} ;

//***********************************************************************
static void create_lcd_objects(HWND hwnd)
{
   m_lcdleft        = new CMatrixStatic(hwnd, &led_left) ;
   m_lcdright       = new CMatrixStatic(hwnd, &led_right) ;
   m_lcdstaticsmall = new CMatrixStatic(hwnd, &led_ssmall) ;
   m_lcdup          = new CMatrixStatic(hwnd, &led_scrl_up) ;
   m_lcddown        = new CMatrixStatic(hwnd, &led_scrl_down) ;
   m_lcdstatic      = new CMatrixStatic(hwnd, &led_static1) ;
   m_lcd_lrender    = new CMatrixStatic(hwnd, &led_lrender) ;
   m_lcd_clock      = new CMatrixStatic(hwnd, &led_clock) ;
}

//***********************************************************************
static void update_lcd_objects(void)
{
   m_lcdleft->OnTimer() ;
   m_lcdright->OnTimer() ;
   m_lcdstaticsmall->OnTimer() ;
   m_lcdup->OnTimer() ;
   m_lcddown->OnTimer() ;
   m_lcdstatic->OnTimer() ;
   m_lcd_lrender->OnTimer() ;
}

//***********************************************************************
static void restore_lcd_objects(void)
{
   m_lcdleft->DialogRestored() ;
   m_lcdright->DialogRestored() ;
   m_lcdstaticsmall->DialogRestored() ;
   m_lcdup->DialogRestored() ;
   m_lcddown->DialogRestored() ;
   m_lcdstatic->DialogRestored() ;
   m_lcd_lrender->DialogRestored() ;
   m_lcd_clock->DialogRestored() ;
}

//***********************************************************************
static void update_clock(void)
{
   static uint dsec_timer_counter = 0 ;
   if (dsec_timer_counter % 10 == 0) {
      time_t ttm ;
      struct tm *gtm ;
      char tstr[30] ;

      time(&ttm) ;
      gtm = localtime(&ttm) ;
      wsprintf(tstr, " %02u:%02u:%02u ", gtm->tm_hour, gtm->tm_min, gtm->tm_sec) ;

      m_lcd_clock->SetText(tstr) ;
      m_lcd_clock->OnTimer() ;
   }
   dsec_timer_counter++ ;
}

//***********************************************************************
static LRESULT CALLBACK TermProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   char msgstr[81] ;

   switch(iMsg) {
   case WM_INITDIALOG:
      wsprintfA(msgstr, "%s", Version) ;
      SetWindowTextA(hwnd, msgstr) ;

      SetClassLongA(hwnd, GCL_HICON,   (LONG) LoadIcon(g_hinst, (LPCSTR)LCDTICO));
      SetClassLongA(hwnd, GCL_HICONSM, (LONG) LoadIcon(g_hinst, (LPCSTR)LCDTICO));

      {  //  create local context
      RECT myRect ;
      GetClientRect(hwnd, &myRect) ;
      cxClient = (myRect.right - myRect.left) ;
      cyClient = (myRect.bottom - myRect.top) ;

      //****************************************************************
      //  create/configure status bar first
      //****************************************************************
      MainStatusBar = new CStatusBar(hwnd) ;
      MainStatusBar->MoveToBottom(cxClient, cyClient) ;

      //  re-position status-bar parts
      {
      int sbparts[3];
      sbparts[0] = (int) (6 * cxClient / 10) ;
      sbparts[1] = (int) (8 * cxClient / 10) ;
      sbparts[2] = -1;
      MainStatusBar->SetParts(3, &sbparts[0]);
      }
      }

      create_lcd_objects(hwnd) ;
      main_timer_id = SetTimer(hwnd, IDT_TIMER_MAIN, 100, (TIMERPROC) NULL) ;
      return true;

   //********************************************************************
   case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER_MAIN:
         update_lcd_objects() ;
         update_clock() ;
         break;

      default:
         break;
         // return DefWindowProcA(hwnd, iMsg, wParam, lParam);
      }
      break;

   case WM_SIZE:
      if (wParam == SIZE_RESTORED) {
         restore_lcd_objects() ;
      } 
      break;

   case WM_ERASEBKGND:
      restore_lcd_objects() ;
      return false;

   case WM_COMMAND:  //  for keyboard handling
      {  //  create local context
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;

      switch (cmd) {
      case BN_CLICKED:
         switch(target) {
         case IDB_FONT_VIEWER:
            show_font_viewer() ;
            break;
            
         case IDOK:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
         } //lint !e744  switch target
         return true;
      } //lint !e744  switch cmd
      break;
      }  //lint !e438 !e10  end local context

   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      if (main_timer_id != 0) {
         KillTimer(hwnd, main_timer_id) ;
         main_timer_id = 0 ;
      }
      delete m_lcdleft        ;
      delete m_lcdright       ;
      delete m_lcdstaticsmall ;
      delete m_lcdup          ;
      delete m_lcddown        ;
      delete m_lcdstatic      ;
      delete m_lcd_lrender    ;
      delete m_lcd_clock      ;

      //  any of these close the child, but I cannot then close myself!!
      // if (hwndShowFontMain != NULL) {
      //    syslog("close child window\n") ;
      //    // SendMessage(hwndShowFontMain, WM_CLOSE, 0, 0) ;
      //    // SendMessage(hwndShowFontMain, WM_DESTROY, 0, 0) ;
      //    DestroyWindow(hwndShowFontMain) ;
      // }
      PostQuitMessage(0);
      break;

   default:
      return false;
   }  //  switch(iMsg) 

   return true;
}  //lint !e715

//***********************************************************************
//lint -esym(1784, WinMain)
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
   {
   g_hinst = hInstance;

   HWND hwnd = CreateDialog(g_hinst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC) TermProc) ;
   if (hwnd == NULL) {
      syslog("CreateDialog: %s\n", get_system_message()) ;
      return 0;
   }

   MSG Msg;
   while(GetMessage(&Msg, NULL,0,0)) {
      if(!IsDialogMessage(hwnd, &Msg)) {
          TranslateMessage(&Msg);
          DispatchMessage(&Msg);
      }
   }

   return (int) Msg.wParam ;
}  //lint !e715

