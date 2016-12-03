//***************************************************************************
//  Copyright (c) 2014  Daniel D Miller
//  1. Converted class to standard C++ and WinAPI, vs MFC implementation.
//  2. Eliminated use of static bitmap image files, replaced with raster fonts
//  License remains the same as specified by Nic Wilson (see below)
//  
//  01/29/14 Notes (Dan Miller)
//  This package has been modified to eliminate reference to Nic Wilson's
//  static bitmap files.  Instead, it generates in-memory font-character
//  images from old DOS bit-mapped files (see fntcol directory).
//***************************************************************************
// MatrixStatic.cpp : implementation file
//
// Copyright (C) 2002 by Nic Wilson  http://www.nicsoft.com.au
// Release Date and Version:
// Version: 1.0 November 2002
//
// See readme.txt file for copyright notes
//***************************************************************************

//lint -esym(1712, CMatrixStatic)
//lint -esym(1719, CMatrixStatic)
//lint -esym(1720, CMatrixStatic)
//lint -esym(1722, CMatrixStatic)

//lint -esym(769, LARGE, STATIC)

//lint -esym(578, string, max)

//lint -esym(1540, CMatrixStatic::hwndParent, CMatrixStatic::hwndSelf, CMatrixStatic::hdcSelf)

//lint -esym(14, DEFAULT_BGND, DEFAULT_SET, DEFAULT_CLEAR)
//lint -esym(765, DEFAULT_BGND, DEFAULT_SET, DEFAULT_CLEAR)
//  definitions from original MatrixStatic package
//    RGB(63, 181, 255),   // BLUE ON
//    RGB(23, 64,  103),   // BLUE OFF
//    RGB(0,  0,   0),     // BLACK
const COLORREF DEFAULT_BGND  = RGB(0, 0, 0) ;
const COLORREF DEFAULT_SET   = RGB(63, 181, 255) ;
const COLORREF DEFAULT_CLEAR = RGB(23, 64,  103) ;

// typedef enum mbms_e {MBS_SMALL = 0, MBS_LARGE, MBS_TINY, MBS_LRENDER} mbms_t ;
typedef enum mbmd_e {MBD_LEFT = 0, MBD_RIGHT, MBD_UP, MBD_DOWN, MBD_STATIC} mbmd_t ;

typedef struct led_data_s {
   uint   ControlID ;         //  Windows ID code used for this control
   char   *msg ;              //  displayed message string
   char   *font_name ;        //  Name of font file to use for display
   uint   diameter ;          //  diameter of pixels in font
   uint   bit_gap ;           //  gap between pixels in character
   uint   char_gap ;          //  gap between characters
   // uint nspace ;           //  half-space doesn't work 
   uint   pixel_type ;        //  square or round pixels
   int    columns ;           //  number of columns in display; 0 uses strlen(msg)
   int    rows ;              //  number of rows in display; 0 uses 1
   uint   new_scroll_rate ;   //  scroll rate in 0.1 seconds (or whatever timer runs at)
   mbmd_t scroll_dir ;        //  scroll direction (left, right, up, down, static)
   bool   set_auto_padding ;  //  Use auto-padding?
   char   auto_padding_char ; //  Padding character, if set; 0 uses ' '
   COLORREF bgnd ;            //  background color
   COLORREF set_bit ;         //  color of set bits
   COLORREF clear_bit ;       //  color of cleared bits
} led_data_t, *led_data_p ;

/////////////////////////////////////////////////////////////////////////////
// CMatrixStatic window
class CMatrixStatic {
private:

   HWND     hwndParent ;
   HWND     hwndSelf ;
   HDC      hdcSelf ;

   char     m_cPadChar;
   string   m_csStrText;
   bool     m_bModified, m_bImmediateUpdate, m_btimer, m_bAutoPad;   
   COLORREF m_crOffColor,  m_crOnColor, m_crBackColor;
   UINT     m_iImagematrix, m_iTimer;
   int      m_icharwidth, m_icharheight, m_ixspacing, m_iyspacing;
   int      m_iMaxYChars, m_iMaxXChars;

   //  scroll-management fields
   mbmd_t   m_iDirection;
   uint     scroll_rate ;  //  in 10th/second
   uint     scroll_count ;
   bool     field_is_drawn ;  // only required for (m_iDirection == STATIC)

   //  lrender class fields 
   lrender  *render ;   //  for MBS_LRENDER instance type

   //  private, internal-use functions
   // bool GetCharBmpOffset(RECT *rc, char ch) const;
   void OnPaint(void);
   
   //  disable copy and assignment operators
   CMatrixStatic operator=(const CMatrixStatic src) ;
   CMatrixStatic(const CMatrixStatic&);

public:
   CMatrixStatic(HWND hwnd, led_data_p led_data);
   // virtual ~CMatrixStatic();
   ~CMatrixStatic();

   // Implementation
//lint -esym(114, CMatrixStatic)  Inconsistent structure declaration for tag 'CMatrixStatic' ??
   void SetText(char *lpszText);
   void SetMatrixDimens(int columns, int rows, uint new_scroll_rate, mbmd_t dir);
   void SetAutoPadding(bool pad, char ch = ' ');   //lint !e114
   void SetDisplayColors(COLORREF bk,  COLORREF on,  COLORREF off);
   int  GetText(char *string,  int size) const ;
   void DialogRestored(void) ;

   //  add new functions for lrender class access
   void SelectFontFile(char *font_name);
   void SetLRenderParams(unsigned new_diameter, unsigned new_bit_gap, unsigned char_gap,
                         unsigned new_nspace, unsigned new_pixel_type);

   //  timer functions
   void OnTimer(void);
};

