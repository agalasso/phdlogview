///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  8 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LOGVIEWFRAMEBASE_H__
#define __LOGVIEWFRAMEBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/scrolbar.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/stattext.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_SETTINGS 1000

///////////////////////////////////////////////////////////////////////////////
/// Class LogViewFrameBase
///////////////////////////////////////////////////////////////////////////////
class LogViewFrameBase : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxSplitterWindow* m_splitter2;
		wxGrid* m_sessions;
		wxTextCtrl* m_sessionInfo;
		wxBoxSizer* m_mainSizer;
		wxTextCtrl* m_rowInfo;
		wxPanel* m_graph;
		wxScrollBar* m_scrollbar;
		wxTextCtrl* m_raLegend;
		wxTextCtrl* m_decLegend;
		wxButton* m_hminus;
		wxButton* m_hplus;
		wxButton* m_hreset;
		wxButton* m_launch;
		wxButton* m_vplus;
		wxButton* m_vminus;
		wxButton* m_vreset;
		wxToggleButton* m_vpan;
		wxToggleButton* m_vlock;
		wxBoxSizer* m_guideControlsSizer;
		wxRadioBox* m_device;
		wxRadioBox* m_units;
		wxRadioBox* m_axes;
		wxCheckBox* m_corrections;
		wxCheckBox* m_grid;
		wxCheckBox* m_ra;
		wxCheckBox* m_dec;
		wxCheckBox* m_mass;
		wxCheckBox* m_snr;
		wxCheckBox* m_events;
		wxCheckBox* m_limits;
		wxCheckBox* m_scatter;
		wxNotebook* m_statsnb;
		wxGrid* m_stats;
		wxHtmlWindow* m_stats2;
		wxMenuBar* m_menubar;
		wxStatusBar* m_statusBar1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnIconize( wxIconizeEvent& event ) { event.Skip(); }
		virtual void OnCellSelected( wxGridEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaintGraph( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnRightUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnSizeGraph( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnHMinus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHPlus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHReset( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLaunchEditor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVPlus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVMinus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVReset( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVPan( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVLock( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDevice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUnits( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAxes( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCorrectionsChecked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRAChecked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDecChecked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStatsChar( wxKeyEvent& event ) { event.Skip(); }
		
	
	public:
		
		LogViewFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 970,599 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~LogViewFrameBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class HelpDialogBase
///////////////////////////////////////////////////////////////////////////////
class HelpDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxHtmlWindow* m_html;
	
	public:
		
		HelpDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("PHD Log Viewer Quick Help"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 567,523 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~HelpDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDialogBase
///////////////////////////////////////////////////////////////////////////////
class SettingsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText5;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRAColor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDecColor( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxCheckBox* m_excludeApi;
		wxCheckBox* m_excludeByParam;
		wxTextCtrl* m_settlePixels;
		wxTextCtrl* m_settleSeconds;
		wxButton* m_raColorBtn;
		wxButton* m_decColorBtn;
		
		SettingsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~SettingsDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AnalyzeFrameBase
///////////////////////////////////////////////////////////////////////////////
class AnalyzeFrameBase : public wxFrame 
{
	private:
	
	protected:
		wxButton* m_vplus;
		wxButton* m_vminus;
		wxButton* m_vreset;
		wxButton* m_hminus;
		wxButton* m_hplus;
		wxButton* m_hreset;
		wxStatusBar* m_statusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnBtnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnClickDrift( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFFT( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaintGraph( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnSizeGraph( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnVPlus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVMinus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnVReset( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHMinus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHPlus( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHReset( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxToggleButton* m_toggleDrift;
		wxToggleButton* m_toggleFFT;
		wxPanel* m_graph;
		wxCheckBox* m_ra;
		wxCheckBox* m_dec;
		
		AnalyzeFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Analysis"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 949,540 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~AnalyzeFrameBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame5
///////////////////////////////////////////////////////////////////////////////
class MyFrame5 : public wxFrame 
{
	private:
	
	protected:
	
	public:
		
		MyFrame5( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~MyFrame5();
	
};

#endif //__LOGVIEWFRAMEBASE_H__
