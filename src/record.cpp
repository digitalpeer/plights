/*
 * plights
 *
 * Copyright (C) 2006, Joshua D. Henderson <www.digitalpeer.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "wx/wx.h"
#include "track.hpp"

#include <time.h>

static CTrack tracks[8];
struct timespec start;

#if !wxUSE_LOG
#   error You must set wxUSE_LOG to 1 in setup.h
#endif

/**
 */
class CRecorderApp : public wxApp
{
public:
   virtual bool OnInit();
};

class RecorderFrame : public wxFrame
{
public:
   RecorderFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
		 long style = wxDEFAULT_FRAME_STYLE);

   ~RecorderFrame() { delete m_logTarget; }

   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);
   void OnClear(wxCommandEvent& event);
   void OnSave(wxCommandEvent& event);
   //   void OnSkip(wxCommandEvent& event);
   // void OnShowRaw(wxCommandEvent& event);

   void OnSize(wxSizeEvent& event);

private:
   wxLog *m_logTarget;

   class TextWindow *m_winText;
   wxListBox *m_lboxLog;

   // any class wishing to process wxWidgets events must use this macro
   DECLARE_EVENT_TABLE()
};

class LboxLogger : public wxLog
{
public:
   LboxLogger(wxListBox *lbox, wxLog *logOld)
   {
      m_lbox = lbox;
      //m_lbox->Disable(); -- looks ugly under MSW
      m_logOld = logOld;
   }

   virtual ~LboxLogger()
   {
      wxLog::SetActiveTarget(m_logOld);
   }

private:
   // implement sink functions
   virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
   {
      // don't put trace messages into listbox or we can get into infinite
      // recursion
      if ( level == wxLOG_Trace )
      {
	 if ( m_logOld )
	 {
	    // cast is needed to call protected method
	    ((LboxLogger *)m_logOld)->DoLog(level, szString, t);
	 }
      }
      else
      {
	 wxLog::DoLog(level, szString, t);
      }
   }

   virtual void DoLogString(const wxChar *szString, time_t WXUNUSED(t))
   {
      wxString msg;
      TimeStamp(&msg);
      msg += szString;

#ifdef __WXUNIVERSAL__
      m_lbox->AppendAndEnsureVisible(msg);
#else // other ports don't have this method yet
      m_lbox->Append(msg);
      m_lbox->SetFirstItem(m_lbox->GetCount() - 1);
#endif
   }

   // the control we use
   wxListBox *m_lbox;

    // the old log target
   wxLog *m_logOld;
};

class TextWindow : public wxWindow
{
public:
   TextWindow(wxWindow *parent)
      : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		 wxRAISED_BORDER)
   {
      //m_skip = true;
      //m_showRaw = false;

      SetBackgroundColour(*wxBLUE);
   }

   //void SetSkip(bool skip) { m_skip = skip; }
   //void SetShowRaw(bool show) { m_showRaw = show; }

protected:
   void OnKeyDown(wxKeyEvent& event) { LogEvent(_T("Key down"), event); }
   void OnKeyUp(wxKeyEvent& event) { LogEvent(_T("Key up"), event); }
   //   void OnChar(wxKeyEvent& event) { LogEvent(_T("Char"), event); }

   void OnPaint(wxPaintEvent& WXUNUSED(event))
   {
      wxPaintDC dc(this);
      dc.SetTextForeground(*wxWHITE);
      dc.DrawLabel(_T("Start Record"), GetClientRect(), wxALIGN_CENTER);
   }

private:
   static inline wxChar GetChar(bool on, wxChar c) { return on ? c : _T('-'); }

   void LogEvent(const wxChar *name, wxKeyEvent& event);

   //bool m_skip;
   //bool m_showRaw;

   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TextWindow, wxWindow)
   EVT_KEY_DOWN(TextWindow::OnKeyDown)
   EVT_KEY_UP(TextWindow::OnKeyUp)
//   EVT_CHAR(TextWindow::OnChar)

   EVT_PAINT(TextWindow::OnPaint)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
   // menu items
   Keyboard_Quit = 1,

   Keyboard_Clear,
   Keyboard_Save,
   //Keyboard_Skip,
   //Keyboard_ShowRaw,

   // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Keyboard_About = wxID_ABOUT
};

BEGIN_EVENT_TABLE(RecorderFrame, wxFrame)
    EVT_MENU(Keyboard_Quit,  RecorderFrame::OnQuit)
    EVT_MENU(Keyboard_About, RecorderFrame::OnAbout)

    EVT_MENU(Keyboard_Clear, RecorderFrame::OnClear)
    EVT_MENU(Keyboard_Save, RecorderFrame::OnSave)
#if 0
    EVT_MENU(Keyboard_Skip, RecorderFrame::OnSkip)
    EVT_MENU(Keyboard_ShowRaw, RecorderFrame::OnShowRaw)
#endif

    EVT_SIZE(RecorderFrame::OnSize)
END_EVENT_TABLE()

IMPLEMENT_APP(CRecorderApp)

bool CRecorderApp::OnInit()
{
   RecorderFrame *frame = new RecorderFrame(_T("plights-record Version 0.1"),
					    wxPoint(50, 50), wxSize(450, 340));

   frame->Show(true);

   for (int x = 0; x < 8;x++)
      tracks[x].SetOutput(x);

   (void)clock_gettime(CLOCK_REALTIME,&start);

   return true;
}

RecorderFrame::RecorderFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
   : wxFrame(NULL, wxID_ANY, title, pos, size, style),
     m_winText(NULL)
{
   // create a menu bar
   wxMenu *menuFile = new wxMenu;
   menuFile->Append(Keyboard_Clear, _T("&Clear\tCtrl-L"));
   menuFile->Append(Keyboard_Save, _T("&Save\tCtrl-S"));
   menuFile->AppendSeparator();
   menuFile->Append(Keyboard_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

   // wxMenu *menuKeys = new wxMenu;
   //menuKeys->AppendCheckItem(Keyboard_ShowRaw, _T("Show &raw keys\tCtrl-R"));
   //menuKeys->AppendSeparator();
   //menuKeys->AppendCheckItem(Keyboard_Skip, _T("&Skip key down\tCtrl-S"));

   // the "About" item should be in the help menu
   wxMenu *menuHelp = new wxMenu;
   menuHelp->Append(Keyboard_About, _T("&About...\tF1"), _T("Show about dialog"));

   // now append the freshly created menu to the menu bar...
   wxMenuBar *menuBar = new wxMenuBar();
   menuBar->Append(menuFile, _T("&File"));
   //menuBar->Append(menuKeys, _T("&Keys"));
   menuBar->Append(menuHelp, _T("&Help"));

   // attach menu bar to the frame
   SetMenuBar(menuBar);

   //menuBar->Check(Keyboard_Skip, true);

   //#ifndef wxHAS_RAW_KEY_CODES
   //menuBar->Enable(Keyboard_ShowRaw, false);
   //#endif // !wxHAS_RAW_KEY_CODES

   m_winText = new TextWindow(this);
   m_lboxLog = new wxListBox(this, wxID_ANY);

   m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
   wxLog::SetActiveTarget(m_logTarget);

   //#if wxUSE_STATUSBAR
   // create a status bar just for fun (by default with 1 pane only)
   //CreateStatusBar(2);
   //SetStatusText(_T(""));
   //#endif // wxUSE_STATUSBAR
}

void RecorderFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
   for (int x = 0; x < 8;x++)
   {
      wxString filename;
      filename.Printf(_T("%d.track"), x);
      if (!tracks[x].Save(filename.c_str()))
      {
	 wxMessageBox("", _T("Failed to save track file"), wxOK | wxICON_INFORMATION, this);
      }
   }
}

void RecorderFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(true);
}

void RecorderFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
   wxString msg = _T("Record parallel port tracks using keys 0-7\n")
      _T("(c) 2006 Joshua D. Henderson");

   wxMessageBox(msg, _T("About plights-record"), wxOK | wxICON_INFORMATION, this);
}

void RecorderFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
   m_lboxLog->Clear();
}

#if 0
void RecorderFrame::OnSkip(wxCommandEvent& event)
{
   m_winText->SetSkip(event.IsChecked());
}

void RecorderFrame::OnShowRaw(wxCommandEvent& event)
{
   m_winText->SetShowRaw(event.IsChecked());
}
#endif

void RecorderFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
   if ( m_winText )
   {
      wxSize size = GetClientSize();
      m_winText->SetSize(0, 0, size.x, 50);
      m_lboxLog->SetSize(0, 51, size.x, size.y - 50);
   }
}

void TextWindow::LogEvent(const wxChar *name, wxKeyEvent& event)
{
   wxString key;
   long keycode = event.GetKeyCode();

   if (keycode >= 49 && keycode <= 56)
   {
      struct timespec now;
      (void)clock_gettime(CLOCK_REALTIME,&now);

      // get difference between now and start time in milliseconds
      unsigned int diff = (unsigned int)((double)(now.tv_sec-start.tv_sec) * 1000.0) +
	 (unsigned int)((double)(now.tv_nsec-start.tv_nsec) / 1000000.0);

      tracks[keycode - 49].Add(diff);

      key.Printf(_T("'%c'"), (unsigned char)keycode);
   }
   else
   {
      key.Printf(_T("unknown (%ld)"), keycode);
   }

   wxString msg;
   msg.Printf(_T("%s event: %s (flags = %c%c%c%c)"),
	      name,
	      key.c_str(),
	      GetChar(event.ControlDown(), _T('C')),
	      GetChar(event.AltDown(), _T('A')),
	      GetChar(event.ShiftDown(), _T('S')),
	      GetChar(event.MetaDown(), _T('M')));

   //    if (m_showRaw)
   //{
   msg += wxString::Format(_T(" (raw key code/flags: %lu and 0x%lx)"),
			   (unsigned long)event.GetRawKeyCode(),
			   (unsigned long)event.GetRawKeyFlags());
   //}

   wxLogMessage(msg);

   //    if (m_skip)
   //{
   //  event.Skip();
   //}
}
