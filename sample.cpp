
//  in resource file, within dialog-box declaration
    LTEXT           "",     IDC_LCDRIGHT,     10,  42, 270, 18, 0, WS_EX_CLIENTEDGE

//  In C++ code, Window/Dialog message handler
//  local class instances
static CMatrixStatic *m_lcdleft;

   //***************************************************************************
   //  instantiate the class (typically in WM_INITDIALOG / WM_CREATE handler)
   //  
   //  All of these actions must be done in this sequence,
   //  except for those labelled (optional)
   //***************************************************************************
   case WM_INITDIALOG:
      m_lcdleft = new CMatrixStatic(hwnd, IDC_LCDLEFT) ;
      //  specify the display string.
      m_lcdleft->SetText("Scrl Left..");
      //  specify which font file to use
      m_lcdleft->SelectFontFile("..\\fntcol\\readable.f08");    
      //  specify LED Matrix parameters;
      //  bit diameter, bit gap, inter-char gap, spacing, pixel type
      m_lcdleft->SetLRenderParams(3, 1, 1, WHOLE_SPACE, SQUARE_PIXELS) ;
      //  specify framing parameters.
      //  scroll rate (in 1/100-second increments), rows, columns, scroll direction
      m_lcdleft->SetMatrixDimens(13, 1, 10, MBD_LEFT);
      m_lcdleft->SetAutoPadding(true, '!');   // (optional) auto padding with different character
      m_lcdleft->SetDisplayColors(RGB(0, 0, 0), RGB(255, 60, 0), RGB(103, 30, 0)); // (optional)

      //  start the timer for display update
      main_timer_id = SetTimer(hwnd, IDT_TIMER_MAIN, 100, (TIMERPROC) NULL) ;

   //***************************************************************************
   //  update the scrolling display(s), in WM_TIMER
   //***************************************************************************
   case WM_TIMER:
      m_lcdleft->OnTimer() ;
      return true;

   //***************************************************************************
   //  re-draw the control(s), if image was minimized and maximized
   //***************************************************************************
   case WM_SIZE:
      if (wParam == SIZE_RESTORED) {
         m_lcdleft->DialogRestored() ;
      } 
      break;

   //***************************************************************************
   //  re-draw the control(s), if image was covered and then restored
   //***************************************************************************
   case WM_ERASEBKGND:
      m_lcdleft->DialogRestored() ;
      return false;


