//***************************************************************************
//  Copyright (c) 2014  Daniel D Miller
//  Converted class to standard C++ and WinAPI, vs MFC implementation.
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
// SEE FEATURES:
//
// Single or multilined displays.
// Three fixed sizes for performance.
// Display can be any colors of your choice.
// CStatic objects can be auto sized.
// Horizontal left or right scrolling for single lined display .
// Vertical up or down scrolling for mulilined displays.
// Display characters can be customised.
//***************************************************************************

#include <windows.h>
#include <cstdio>
#include <string>
using namespace std;

#include "common.h"
#include "fontmgr.h"
#include "lrender.h"
#include "MatrixStatic.h"

//lint -esym(1714, CMatrixStatic::GetText)

//***************************************************************
// CMatrixStatic constructor
//***************************************************************
CMatrixStatic::CMatrixStatic(HWND hwnd, led_data_p led_data) :
   hwndParent(hwnd),
   hwndSelf(NULL),
   hdcSelf(NULL),
   m_cPadChar(' '),
   m_csStrText(),
   m_bModified(false), 
   m_bImmediateUpdate(false), 
   m_btimer(false),
   m_bAutoPad(false),
   m_crOffColor(RGB(23, 64,  103)),  
   m_crOnColor(RGB(63, 181, 255)), 
   m_crBackColor(RGB(0,  0,   0)),
   m_iImagematrix(0), 
   m_iTimer(0),
   m_icharwidth(0), 
   m_icharheight(0), 
   m_ixspacing(0), 
   m_iyspacing(0),
   m_iMaxYChars(0), 
   m_iMaxXChars(0), 
   m_iDirection(MBD_LEFT),
   scroll_rate(0),  //  in 100th/second
   scroll_count(0),
   field_is_drawn(false),
   render(NULL)
{
   m_csStrText.clear() ;
   hwndSelf = GetDlgItem(hwnd, led_data->ControlID) ;
   hdcSelf = GetDC(hwndSelf) ;
   
   SetText(led_data->msg);
   SelectFontFile(led_data->font_name);
   SetLRenderParams(led_data->diameter, led_data->bit_gap, led_data->char_gap, 
                    WHOLE_SPACE, led_data->pixel_type) ;
   SetMatrixDimens(led_data->columns, led_data->rows, led_data->new_scroll_rate, led_data->scroll_dir);
   SetAutoPadding(led_data->set_auto_padding, led_data->auto_padding_char) ;
   SetDisplayColors(led_data->bgnd, led_data->set_bit, led_data->clear_bit) ;
}

//***************************************************************
CMatrixStatic::~CMatrixStatic()
{
   if (render != NULL) {
      delete render ;   //lint !e1551  Function may throw exception
   }
   ReleaseDC(hwndSelf, hdcSelf) ;
   hdcSelf = NULL ;
   hwndSelf = NULL ;
   hwndParent = NULL ;
}

//***************************************************************
//  additional interfaces to lrender class
//***************************************************************
void CMatrixStatic::SelectFontFile(char *font_name)
{
   render = new lrender(hwndSelf, font_name) ;
}

void CMatrixStatic::SetLRenderParams(unsigned new_diameter, unsigned new_bit_gap, unsigned char_gap,
   unsigned new_nspace, unsigned new_pixel_type)
{
   if (render != NULL) {
      render->set_attr(m_crOnColor, m_crOffColor, m_crBackColor);
      render->set_dimens(new_diameter, new_bit_gap, char_gap) ;  //  diameter, bit gap, inter-char gap
      render->set_options(new_nspace, new_pixel_type) ;
   }
}

//***************************************************************
void CMatrixStatic::SetText(char *lpszText)
{
   if (m_csStrText != lpszText) {
      m_csStrText = lpszText;
		m_bModified = true;
      field_is_drawn = false ;
	}
}

//***************************************************************
void CMatrixStatic::SetAutoPadding(bool pad, char ch)
{
   m_bAutoPad = pad;
   m_cPadChar = (ch == 0) ? ' ' : ch;
}

//***************************************************************
int CMatrixStatic::GetText(char *string, int size) const 
{
   int length = m_csStrText.length();
   if (m_csStrText.empty() || size < length)
		return 0;
   // _tcscpy((LPTSTR)string, m_csStrText.c_str());
   strcpy((char *) string, m_csStrText.c_str());
	return length;
}

//********************************************************************
// OnPaint() is called by OnTimer() to render the character images
//********************************************************************
void CMatrixStatic::OnPaint(void) 
{
   RECT m_rect;

   if (m_iDirection == MBD_STATIC) {
      if (field_is_drawn)
         return ;
      field_is_drawn = true ;
   }

   GetClientRect(hwndSelf, &m_rect);
   int x = 0, y = 0;
   int slen = m_csStrText.length();
   if (slen == 0)
      return;

   //  clear entire background of current field
   HBRUSH hbBkBrush = CreateSolidBrush(m_crBackColor);
   FillRect(hdcSelf, &m_rect, hbBkBrush);
   DeleteObject(hbBkBrush);

   int charcount = 0;
   int linecount = 1;
   for (int ix = 0; ix < slen; ix++) {
      if (render != NULL) {
         render->write_led_char(x, y, (char)m_csStrText[ix]) ;
      }

      x += m_icharwidth + m_ixspacing;
      charcount++;
      //  for single-line display, stop rendering when frame is full.
      if ((charcount == m_iMaxXChars) && m_iMaxYChars == 1)
         break;
      //  for multi-line display, update row number as required
      if ((charcount == m_iMaxXChars) && m_iMaxYChars > 1) {
         if (linecount == m_iMaxYChars)
            break;
         x = charcount = 0;
         y += m_icharheight + m_iyspacing;
         linecount++;
      }
   }
}

//************************************************************************************
//  OnTimer() does the string management, then calls OnPaint() to render the images
//************************************************************************************
void CMatrixStatic::OnTimer(void) 
{ 
   uint row_len = m_iMaxXChars ;
   uint idx ;
   uint row_idx = 0 ;
   string stemp ;

   if (m_iMaxXChars == 0)
      return ;

   if (m_iDirection != MBD_STATIC) {
      if (scroll_rate == 0)
         return ;
      if (++scroll_count < scroll_rate) {
         return ;
      } 
      scroll_count = 0 ;
   }

   int slen = m_csStrText.length();
   //  Handle single-row fields
   if (m_iMaxYChars == 1) {
      if (m_bAutoPad) {
         if (slen < m_iMaxXChars) {
            int diff =  m_iMaxXChars - slen;
            // syslog("pt A: padding with %c\n", m_cPadChar) ;
				for (;diff > 0; diff--)
					m_csStrText += m_cPadChar;
            slen = m_iMaxXChars ;
			}
		}
      switch (m_iDirection) {
      case MBD_LEFT:
         // m_csStrText = m_csStrText.Mid(1) + m_csStrText[0]; //Move the first character to the end.
         m_csStrText = m_csStrText.substr(1, slen-1) + m_csStrText.substr(0,1) ;
         break;

      case MBD_STATIC:   //  string is not modified
         // m_csStrText = m_csStrText.substr(1, slen-1) + m_csStrText.substr(0,1) ;
         break;

      default:
         // int len = m_csStrText.GetLength();
         // m_csStrText = m_csStrText.Right(1) + m_csStrText.Left(slen-1); //Move the end character to the start.
         m_csStrText = m_csStrText.substr(slen-1, 1) + m_csStrText.substr(0,slen-1) ;
         break;
      }  //lint !e788  enum constant not used within defaulted switch
	}
   //  Handle multi-row fields
   else if (m_iMaxYChars > 1) {
      if (m_bAutoPad) {
         if (slen < (m_iMaxXChars * m_iMaxYChars)) {
            int diff = m_iMaxXChars * m_iMaxYChars - slen;
            // syslog("pt B: padding with %c\n", m_cPadChar) ;
				for (;diff > 0; diff--)
					m_csStrText += m_cPadChar;
            slen = m_iMaxXChars ;
			}
		}
      if (m_iDirection  == MBD_DOWN)
		{
         //  move row from end to beginning
         // string tmp  = m_csStrText.Right(m_iMaxXChars);
         // m_csStrText = tmp + m_csStrText.Left(slen - m_iMaxXChars);
         m_csStrText = m_csStrText.substr(slen-m_iMaxXChars, m_iMaxXChars) + m_csStrText.substr(0,slen-m_iMaxXChars) ;
		}
      else if (m_iDirection == MBD_UP)
		{
         //  move row from beginning to end
         // string tmp  = m_csStrText.Left(m_iMaxXChars);
         // m_csStrText = m_csStrText.Mid(m_iMaxXChars) + tmp;
         m_csStrText = m_csStrText.substr(m_iMaxXChars, slen-m_iMaxXChars) + m_csStrText.substr(0, m_iMaxXChars) ;
		}
      else if (m_iDirection  == MBD_LEFT)
		{
         //  this is doing a right-or-left rotate of characters 
         //  on each row of a multi-row display.
         uint loop = slen / m_iMaxXChars;
         if (loop == 0 || slen < m_iMaxXChars)
				loop = 1;
         row_len = m_iMaxXChars ;
         row_idx = 0 ;
         for (idx=0; idx<loop; idx++) {
            // m_csStrText = m_csStrText.substr(1, slen-1) + m_csStrText.substr(0,1) ;
            stemp += m_csStrText.substr(row_idx+1, row_len-1) + m_csStrText.substr(row_idx+0,1) ;
            row_idx += row_len ;
         }
         m_csStrText = stemp ;
      }
      else if (m_iDirection  == MBD_RIGHT)
		{
         //  this is doing a right-or-left rotate of characters 
         //  on each row of a multi-row display.
         uint loop = slen / m_iMaxXChars;  //  get number of rows
         if (loop == 0 || slen < m_iMaxXChars)
				return;
         row_len = m_iMaxXChars ;
         row_idx = 0 ;
         for (idx=0; idx<loop; idx++) {
            // m_csStrText = m_csStrText.substr(slen-1, 1) + m_csStrText.substr(0,slen-1) ;
            stemp += m_csStrText.substr(row_idx+row_len-1, 1) + m_csStrText.substr(row_idx+0,row_len-1) ;
            row_idx += row_len ;
         }
         m_csStrText = stemp ;
      }
	}
   OnPaint() ;
}  //lint !e715

//***************************************************************
void CMatrixStatic::DialogRestored(void) 
{
   field_is_drawn = false ;
   scroll_count = scroll_rate ;
}

//***************************************************************
void CMatrixStatic::SetMatrixDimens(int columns, int rows, uint new_scroll_rate, mbmd_t dir)
{
   if (render != NULL) {
      unsigned diameter, bit_gap, char_gap ;
      render->get_dimens(&diameter, &bit_gap, &char_gap) ;

      m_icharwidth  = render->get_char_width();
      m_icharheight = render->get_char_height();

      m_ixspacing   = char_gap;
      m_iyspacing   = char_gap;
   }

   if (columns == 0)
      m_iMaxXChars = m_csStrText.length();
   else
      m_iMaxXChars = columns;

   if (rows == 0) {
      if (m_iMaxXChars != 0)
         m_iMaxYChars = (int) m_csStrText.length() / m_iMaxXChars;
      else
         m_iMaxYChars = 1;
   } else {
      m_iMaxYChars = rows;
   }

   RECT rc;
   GetClientRect(hwndSelf, &rc);
   int width  = ((m_icharwidth  + m_ixspacing) * m_iMaxXChars);
   int height = ((m_icharheight + m_iyspacing) * m_iMaxYChars);
   SetWindowPos(hwndSelf, NULL, 0, 0, width+4, height+4, SWP_NOMOVE);   

   // DoScroll()
   m_iDirection = dir;
   scroll_rate = new_scroll_rate ;
   scroll_count = new_scroll_rate ;
}

//***************************************************************
void CMatrixStatic::SetDisplayColors(COLORREF bk, COLORREF on, COLORREF off)
{
   if (render != NULL) {
      render->set_attr(on, off, bk);
   } 
   m_crBackColor = bk;
   m_crOffColor = off;
   m_crOnColor = on;
}

