///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  8 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LogViewFrameBase.h"

///////////////////////////////////////////////////////////////////////////

LogViewFrameBase::LogViewFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter1->SetSashGravity( 0 );
	m_splitter1->SetMinimumPaneSize( 100 );
	
	wxPanel* panel5;
	panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter2 = new wxSplitterWindow( panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter2->SetMinimumPaneSize( 100 );
	
	wxPanel* panel7;
	panel7 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( panel7, wxID_ANY, wxT("Log sections") ), wxHORIZONTAL );
	
	m_sessions = new wxGrid( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxBORDER_STATIC );
	
	// Grid
	m_sessions->CreateGrid( 8, 4 );
	m_sessions->EnableEditing( false );
	m_sessions->EnableGridLines( true );
	m_sessions->SetGridLineColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW ) );
	m_sessions->EnableDragGridSize( true );
	m_sessions->SetMargins( 0, 0 );
	
	// Columns
	m_sessions->SetColSize( 0, 24 );
	m_sessions->SetColSize( 1, 114 );
	m_sessions->SetColSize( 2, 64 );
	m_sessions->SetColSize( 3, 72 );
	m_sessions->AutoSizeColumns();
	m_sessions->EnableDragColMove( false );
	m_sessions->EnableDragColSize( true );
	m_sessions->SetColLabelSize( 0 );
	m_sessions->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_sessions->EnableDragRowSize( false );
	m_sessions->SetRowLabelSize( 0 );
	m_sessions->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_sessions->SetDefaultCellBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_sessions->SetDefaultCellTextColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	m_sessions->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_sessions->Hide();
	
	sbSizer1->Add( m_sessions, 1, wxALL|wxEXPAND|wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5 );
	
	
	panel7->SetSizer( sbSizer1 );
	panel7->Layout();
	sbSizer1->Fit( panel7 );
	wxPanel* panel8;
	panel8 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( panel8, wxID_ANY, wxT("Section heading") ), wxVERTICAL );
	
	m_sessionInfo = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_sessionInfo->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial") ) );
	m_sessionInfo->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sbSizer2->Add( m_sessionInfo, 1, wxALL|wxEXPAND, 5 );
	
	
	panel8->SetSizer( sbSizer2 );
	panel8->Layout();
	sbSizer2->Fit( panel8 );
	m_splitter2->SplitVertically( panel7, panel8, -1 );
	bSizer8->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	
	panel5->SetSizer( bSizer8 );
	panel5->Layout();
	bSizer8->Fit( panel5 );
	wxPanel* panel6;
	panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_rowInfo = new wxTextCtrl( panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer10->Add( m_rowInfo, 0, wxALL|wxEXPAND, 5 );
	
	
	m_mainSizer->Add( bSizer10, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_graph = new wxPanel( panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_graph->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	m_graph->SetMinSize( wxSize( 650,90 ) );
	
	bSizer9->Add( m_graph, 1, wxEXPAND | wxALL, 5 );
	
	m_scrollbar = new wxScrollBar( panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
	bSizer9->Add( m_scrollbar, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_raLegend = new wxTextCtrl( panel6, wxID_ANY, wxT("―RA"), wxDefaultPosition, wxSize( 45,-1 ), wxTE_CENTRE|wxTE_READONLY|wxBORDER_NONE );
	m_raLegend->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );
	m_raLegend->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	bSizer5->Add( m_raLegend, 0, wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_decLegend = new wxTextCtrl( panel6, wxID_ANY, wxT("―Dec"), wxDefaultPosition, wxSize( 48,-1 ), wxTE_CENTRE|wxTE_READONLY|wxBORDER_NONE );
	m_decLegend->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );
	m_decLegend->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	bSizer5->Add( m_decLegend, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_hminus = new wxButton( panel6, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_hminus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_hminus->SetToolTip( wxT("Zoom out") );
	
	bSizer5->Add( m_hminus, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_hplus = new wxButton( panel6, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_hplus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_hplus->SetToolTip( wxT("Zoom in") );
	
	bSizer5->Add( m_hplus, 0, wxALL, 5 );
	
	m_hreset = new wxButton( panel6, wxID_ANY, wxT("R"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_hreset->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_hreset->SetToolTip( wxT("Reset zoom") );
	
	bSizer5->Add( m_hreset, 0, wxALL, 5 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_launch = new wxButton( panel6, wxID_ANY, wxT("Text"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_launch->SetToolTip( wxT("Open the log file in your text editor") );
	
	bSizer5->Add( m_launch, 0, wxALL, 5 );
	
	
	bSizer9->Add( bSizer5, 0, wxEXPAND, 5 );
	
	
	bSizer2->Add( bSizer9, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_vplus = new wxButton( panel6, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 30,25 ), 0 );
	m_vplus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_vplus->SetToolTip( wxT("Zoom in") );
	
	bSizer3->Add( m_vplus, 0, wxALL, 5 );
	
	m_vminus = new wxButton( panel6, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 30,25 ), 0 );
	m_vminus->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_vminus->SetToolTip( wxT("Zoom out") );
	
	bSizer3->Add( m_vminus, 0, wxALL, 5 );
	
	m_vreset = new wxButton( panel6, wxID_ANY, wxT("R"), wxDefaultPosition, wxSize( 30,25 ), 0 );
	m_vreset->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
	m_vreset->SetToolTip( wxT("Reset zoom") );
	
	bSizer3->Add( m_vreset, 0, wxALL, 5 );
	
	m_vpan = new wxToggleButton( panel6, wxID_ANY, wxT("P/&Z"), wxDefaultPosition, wxSize( 30,25 ), 0 );
	m_vpan->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_vpan->SetToolTip( wxT("Pan / Zoom") );
	
	bSizer3->Add( m_vpan, 0, wxALL, 5 );
	
	m_vlock = new wxToggleButton( panel6, wxID_ANY, wxT("L"), wxDefaultPosition, wxSize( 30,25 ), 0 );
	m_vlock->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_vlock->SetToolTip( wxT("Lock vertical scale, use the same scale for all guiding sessions") );
	
	bSizer3->Add( m_vlock, 0, wxALL, 5 );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	
	m_mainSizer->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_guideControlsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( panel6, wxID_ANY, wxT("Plot options") ), wxHORIZONTAL );
	
	wxString m_deviceChoices[] = { wxT("Mount"), wxT("AO") };
	int m_deviceNChoices = sizeof( m_deviceChoices ) / sizeof( wxString );
	m_device = new wxRadioBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Device"), wxDefaultPosition, wxDefaultSize, m_deviceNChoices, m_deviceChoices, 1, wxRA_SPECIFY_COLS );
	m_device->SetSelection( 0 );
	m_device->SetToolTip( wxT("Display corrections for this device") );
	
	sbSizer4->Add( m_device, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5 );
	
	wxString m_unitsChoices[] = { wxT("arc-sec"), wxT("pixels") };
	int m_unitsNChoices = sizeof( m_unitsChoices ) / sizeof( wxString );
	m_units = new wxRadioBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Units"), wxDefaultPosition, wxDefaultSize, m_unitsNChoices, m_unitsChoices, 1, wxRA_SPECIFY_COLS );
	m_units->SetSelection( 0 );
	m_units->SetToolTip( wxT("Vertical axis units") );
	
	sbSizer4->Add( m_units, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_axesChoices[] = { wxT("RA/Dec"), wxT("dx/dy") };
	int m_axesNChoices = sizeof( m_axesChoices ) / sizeof( wxString );
	m_axes = new wxRadioBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Axes"), wxDefaultPosition, wxDefaultSize, m_axesNChoices, m_axesChoices, 1, wxRA_SPECIFY_COLS );
	m_axes->SetSelection( 0 );
	m_axes->SetToolTip( wxT("Select camera axes (dx/dy) or Mount axes (RA/Dec)") );
	
	sbSizer4->Add( m_axes, 0, wxALIGN_LEFT|wxALIGN_RIGHT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_corrections = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Corrections"), wxDefaultPosition, wxDefaultSize, 0 );
	m_corrections->SetValue(true); 
	m_corrections->SetToolTip( wxT("Plot guide pulses") );
	
	bSizer12->Add( m_corrections, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_grid = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Grid"), wxDefaultPosition, wxDefaultSize, 0 );
	m_grid->SetValue(true); 
	m_grid->SetToolTip( wxT("Plot grid lines") );
	
	bSizer12->Add( m_grid, 0, wxALL, 5 );
	
	
	sbSizer4->Add( bSizer12, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_ra = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ra->SetValue(true); 
	m_ra->SetToolTip( wxT("Plot guide star RA (or dx) offset") );
	
	bSizer7->Add( m_ra, 0, wxALL, 5 );
	
	m_dec = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Dec"), wxDefaultPosition, wxDefaultSize, 0 );
	m_dec->SetValue(true); 
	m_dec->SetToolTip( wxT("Plot guide star Dec (or dy) offset") );
	
	bSizer7->Add( m_dec, 0, wxALL, 5 );
	
	
	sbSizer4->Add( bSizer7, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );
	
	m_mass = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Star mass"), wxDefaultPosition, wxDefaultSize, 0 );
	m_mass->SetToolTip( wxT("Plot star mass value (guide star intensity)") );
	
	bSizer101->Add( m_mass, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_snr = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("SNR"), wxDefaultPosition, wxDefaultSize, 0 );
	m_snr->SetToolTip( wxT("Plot star SNR (signal-to-noise ratio)") );
	
	bSizer101->Add( m_snr, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer4->Add( bSizer101, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_events = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_events->SetValue(true); 
	m_events->SetToolTip( wxT("Plot events, including guide parameter changes, server commands received, and star lost events") );
	
	bSizer11->Add( m_events, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_limits = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Limits"), wxDefaultPosition, wxDefaultSize, 0 );
	m_limits->SetToolTip( wxT("Plot the min-motion and max-duration levels") );
	
	bSizer11->Add( m_limits, 0, wxALL, 5 );
	
	
	sbSizer4->Add( bSizer11, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_scatter = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Scatter"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_scatter, 0, wxALL, 5 );
	
	
	sbSizer4->Add( bSizer21, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	m_guideControlsSizer->Add( sbSizer4, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	m_statsnb = new wxNotebook( panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxPanel* panel1;
	panel1 = new wxPanel( m_statsnb, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_stats = new wxGrid( panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_stats->CreateGrid( 3, 2 );
	m_stats->EnableEditing( false );
	m_stats->EnableGridLines( true );
	m_stats->EnableDragGridSize( true );
	m_stats->SetMargins( 0, 0 );
	
	// Columns
	m_stats->SetColSize( 0, 95 );
	m_stats->SetColSize( 1, 95 );
	m_stats->AutoSizeColumns();
	m_stats->EnableDragColMove( false );
	m_stats->EnableDragColSize( true );
	m_stats->SetColLabelSize( 20 );
	m_stats->SetColLabelValue( 0, wxT("RMS") );
	m_stats->SetColLabelValue( 1, wxT("Peak") );
	m_stats->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_stats->EnableDragRowSize( false );
	m_stats->SetRowLabelSize( 80 );
	m_stats->SetRowLabelValue( 0, wxT("RA") );
	m_stats->SetRowLabelValue( 1, wxT("Dec") );
	m_stats->SetRowLabelValue( 2, wxT("Total") );
	m_stats->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_stats->SetDefaultCellBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_SCROLLBAR ) );
	m_stats->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer23->Add( m_stats, 0, wxALL, 5 );
	
	
	panel1->SetSizer( bSizer23 );
	panel1->Layout();
	bSizer23->Fit( panel1 );
	m_statsnb->AddPage( panel1, wxT("Statistics"), true );
	wxPanel* panel2;
	panel2 = new wxPanel( m_statsnb, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_stats2 = new wxHtmlWindow( panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer24->Add( m_stats2, 1, wxALL|wxEXPAND, 5 );
	
	
	panel2->SetSizer( bSizer24 );
	panel2->Layout();
	bSizer24->Fit( panel2 );
	m_statsnb->AddPage( panel2, wxT("Drift"), false );
	
	bSizer22->Add( m_statsnb, 1, wxEXPAND | wxALL, 5 );
	
	
	m_guideControlsSizer->Add( bSizer22, 1, wxEXPAND, 5 );
	
	
	m_mainSizer->Add( m_guideControlsSizer, 0, 0, 5 );
	
	
	panel6->SetSizer( m_mainSizer );
	panel6->Layout();
	m_mainSizer->Fit( panel6 );
	m_splitter1->SplitHorizontally( panel5, panel6, -1 );
	bSizer32->Add( m_splitter1, 1, wxEXPAND, 0 );
	
	
	this->SetSizer( bSizer32 );
	this->Layout();
	m_menubar = new wxMenuBar( 0 );
	wxMenu* filemenu;
	filemenu = new wxMenu();
	wxMenuItem* fileOpen;
	fileOpen = new wxMenuItem( filemenu, wxID_OPEN, wxString( wxT("&Open") ) , wxEmptyString, wxITEM_NORMAL );
	filemenu->Append( fileOpen );
	
	wxMenuItem* fileSettings;
	fileSettings = new wxMenuItem( filemenu, wxID_SETTINGS, wxString( wxT("Settings...") ) , wxEmptyString, wxITEM_NORMAL );
	filemenu->Append( fileSettings );
	
	wxMenuItem* fileExit;
	fileExit = new wxMenuItem( filemenu, wxID_EXIT, wxString( wxT("E&xit") ) , wxEmptyString, wxITEM_NORMAL );
	filemenu->Append( fileExit );
	
	m_menubar->Append( filemenu, wxT("&File") ); 
	
	wxMenu* helpMenu;
	helpMenu = new wxMenu();
	wxMenuItem* helpAbout;
	helpAbout = new wxMenuItem( helpMenu, wxID_ABOUT, wxString( wxT("About...") ) , wxEmptyString, wxITEM_NORMAL );
	helpMenu->Append( helpAbout );
	
	wxMenuItem* helpHelp;
	helpHelp = new wxMenuItem( helpMenu, wxID_HELP, wxString( wxT("Quick Help...") ) , wxEmptyString, wxITEM_NORMAL );
	helpMenu->Append( helpHelp );
	
	m_menubar->Append( helpMenu, wxT("&Help") ); 
	
	this->SetMenuBar( m_menubar );
	
	m_statusBar1 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( LogViewFrameBase::OnActivate ) );
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( LogViewFrameBase::OnClose ) );
	this->Connect( wxEVT_ICONIZE, wxIconizeEventHandler( LogViewFrameBase::OnIconize ) );
	m_sessions->Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( LogViewFrameBase::OnCellSelected ), NULL, this );
	m_graph->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( LogViewFrameBase::OnLeftDown ), NULL, this );
	m_graph->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( LogViewFrameBase::OnLeftUp ), NULL, this );
	m_graph->Connect( wxEVT_MOTION, wxMouseEventHandler( LogViewFrameBase::OnMove ), NULL, this );
	m_graph->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LogViewFrameBase::OnMouseWheel ), NULL, this );
	m_graph->Connect( wxEVT_PAINT, wxPaintEventHandler( LogViewFrameBase::OnPaintGraph ), NULL, this );
	m_graph->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( LogViewFrameBase::OnRightUp ), NULL, this );
	m_graph->Connect( wxEVT_SIZE, wxSizeEventHandler( LogViewFrameBase::OnSizeGraph ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_hminus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHMinus ), NULL, this );
	m_hplus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHPlus ), NULL, this );
	m_hreset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHReset ), NULL, this );
	m_launch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnLaunchEditor ), NULL, this );
	m_vplus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVPlus ), NULL, this );
	m_vminus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVMinus ), NULL, this );
	m_vreset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVReset ), NULL, this );
	m_vpan->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVPan ), NULL, this );
	m_vlock->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVLock ), NULL, this );
	m_device->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnDevice ), NULL, this );
	m_units->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnUnits ), NULL, this );
	m_axes->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnAxes ), NULL, this );
	m_corrections->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_grid->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_ra->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnRAChecked ), NULL, this );
	m_dec->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnDecChecked ), NULL, this );
	m_mass->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_snr->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_events->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_limits->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_scatter->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_stats->Connect( wxEVT_CHAR, wxKeyEventHandler( LogViewFrameBase::OnStatsChar ), NULL, this );
}

LogViewFrameBase::~LogViewFrameBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( LogViewFrameBase::OnActivate ) );
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( LogViewFrameBase::OnClose ) );
	this->Disconnect( wxEVT_ICONIZE, wxIconizeEventHandler( LogViewFrameBase::OnIconize ) );
	m_sessions->Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( LogViewFrameBase::OnCellSelected ), NULL, this );
	m_graph->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( LogViewFrameBase::OnLeftDown ), NULL, this );
	m_graph->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( LogViewFrameBase::OnLeftUp ), NULL, this );
	m_graph->Disconnect( wxEVT_MOTION, wxMouseEventHandler( LogViewFrameBase::OnMove ), NULL, this );
	m_graph->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LogViewFrameBase::OnMouseWheel ), NULL, this );
	m_graph->Disconnect( wxEVT_PAINT, wxPaintEventHandler( LogViewFrameBase::OnPaintGraph ), NULL, this );
	m_graph->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( LogViewFrameBase::OnRightUp ), NULL, this );
	m_graph->Disconnect( wxEVT_SIZE, wxSizeEventHandler( LogViewFrameBase::OnSizeGraph ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_scrollbar->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( LogViewFrameBase::OnScroll ), NULL, this );
	m_hminus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHMinus ), NULL, this );
	m_hplus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHPlus ), NULL, this );
	m_hreset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnHReset ), NULL, this );
	m_launch->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnLaunchEditor ), NULL, this );
	m_vplus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVPlus ), NULL, this );
	m_vminus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVMinus ), NULL, this );
	m_vreset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVReset ), NULL, this );
	m_vpan->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVPan ), NULL, this );
	m_vlock->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnVLock ), NULL, this );
	m_device->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnDevice ), NULL, this );
	m_units->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnUnits ), NULL, this );
	m_axes->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( LogViewFrameBase::OnAxes ), NULL, this );
	m_corrections->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_grid->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_ra->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnRAChecked ), NULL, this );
	m_dec->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnDecChecked ), NULL, this );
	m_mass->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_snr->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_events->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_limits->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_scatter->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogViewFrameBase::OnCorrectionsChecked ), NULL, this );
	m_stats->Disconnect( wxEVT_CHAR, wxKeyEventHandler( LogViewFrameBase::OnStatsChar ), NULL, this );
	
}

HelpDialogBase::HelpDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_html = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer13->Add( m_html, 1, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer13 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

HelpDialogBase::~HelpDialogBase()
{
}

SettingsDialogBase::SettingsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Dither") ), wxVERTICAL );
	
	m_excludeApi = new wxCheckBox( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Exclude settling by server API events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_excludeApi->SetValue(true); 
	sbSizer5->Add( m_excludeApi, 0, wxALL, 5 );
	
	m_excludeByParam = new wxCheckBox( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Exclude settling parametrically"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_excludeByParam, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer20->Add( 60, 0, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Settle at <"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer15->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_settlePixels = new wxTextCtrl( sbSizer5->GetStaticBox(), wxID_ANY, wxT("1.00"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	bSizer15->Add( m_settlePixels, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText4 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, wxT("pixels"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer15->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer19->Add( bSizer15, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, wxT("For"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer151->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_settleSeconds = new wxTextCtrl( sbSizer5->GetStaticBox(), wxID_ANY, wxT("10"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer151->Add( m_settleSeconds, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText5 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, wxT("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer151->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer19->Add( bSizer151, 1, wxEXPAND, 5 );
	
	
	bSizer20->Add( bSizer19, 0, 0, 5 );
	
	
	sbSizer5->Add( bSizer20, 1, wxEXPAND, 5 );
	
	
	bSizer14->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Colors") ), wxHORIZONTAL );
	
	m_raColorBtn = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, wxT("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_raColorBtn->SetForegroundColour( wxColour( 255, 0, 0 ) );
	m_raColorBtn->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	sbSizer6->Add( m_raColorBtn, 0, wxALL, 5 );
	
	m_decColorBtn = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Dec"), wxDefaultPosition, wxDefaultSize, 0 );
	m_decColorBtn->SetForegroundColour( wxColour( 0, 0, 255 ) );
	m_decColorBtn->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	sbSizer6->Add( m_decColorBtn, 0, wxALL, 5 );
	
	
	bSizer14->Add( sbSizer6, 0, 0, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer22->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxButton* okbtn;
	okbtn = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( okbtn, 0, wxALL, 5 );
	
	wxButton* cancelbtn;
	cancelbtn = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( cancelbtn, 0, wxALL, 5 );
	
	
	bSizer14->Add( bSizer22, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer14 );
	this->Layout();
	bSizer14->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_raColorBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialogBase::OnRAColor ), NULL, this );
	m_decColorBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialogBase::OnDecColor ), NULL, this );
}

SettingsDialogBase::~SettingsDialogBase()
{
	// Disconnect Events
	m_raColorBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialogBase::OnRAColor ), NULL, this );
	m_decColorBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SettingsDialogBase::OnDecColor ), NULL, this );
	
}

AnalyzeFrameBase::AnalyzeFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	m_toggleDrift = new wxToggleButton( this, wxID_ANY, wxT("Drift-corrected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggleDrift->SetValue( true ); 
	bSizer27->Add( m_toggleDrift, 0, wxALL, 5 );
	
	m_toggleFFT = new wxToggleButton( this, wxID_ANY, wxT("Frequency Analysis"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( m_toggleFFT, 0, wxALL, 5 );
	
	
	bSizer25->Add( bSizer27, 0, 0, 5 );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	m_graph = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_graph->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	bSizer29->Add( m_graph, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );
	
	m_vplus = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer30->Add( m_vplus, 0, wxALL, 5 );
	
	m_vminus = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer30->Add( m_vminus, 0, wxALL, 5 );
	
	m_vreset = new wxButton( this, wxID_ANY, wxT("R"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer30->Add( m_vreset, 0, wxALL, 5 );
	
	
	bSizer29->Add( bSizer30, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer25->Add( bSizer29, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ra = new wxCheckBox( this, wxID_ANY, wxT("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ra->SetValue(true); 
	bSizer281->Add( m_ra, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_dec = new wxCheckBox( this, wxID_ANY, wxT("Dec"), wxDefaultPosition, wxDefaultSize, 0 );
	m_dec->SetValue(true); 
	bSizer281->Add( m_dec, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer28->Add( bSizer281, 0, wxEXPAND, 5 );
	
	
	bSizer28->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer271;
	bSizer271 = new wxBoxSizer( wxHORIZONTAL );
	
	m_hminus = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer271->Add( m_hminus, 0, wxALL, 5 );
	
	m_hplus = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer271->Add( m_hplus, 0, wxALL, 5 );
	
	m_hreset = new wxButton( this, wxID_ANY, wxT("R"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer271->Add( m_hreset, 0, wxALL, 5 );
	
	
	bSizer28->Add( bSizer271, 1, wxEXPAND, 5 );
	
	
	bSizer28->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizer25->Add( bSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer25 );
	this->Layout();
	m_statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( AnalyzeFrameBase::OnClose ) );
	m_toggleDrift->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnBtnLeftDown ), NULL, this );
	m_toggleDrift->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnClickDrift ), NULL, this );
	m_toggleFFT->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnBtnLeftDown ), NULL, this );
	m_toggleFFT->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnClickFFT ), NULL, this );
	m_graph->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnLeftDown ), NULL, this );
	m_graph->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( AnalyzeFrameBase::OnLeftUp ), NULL, this );
	m_graph->Connect( wxEVT_MOTION, wxMouseEventHandler( AnalyzeFrameBase::OnMove ), NULL, this );
	m_graph->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( AnalyzeFrameBase::OnMouseWheel ), NULL, this );
	m_graph->Connect( wxEVT_PAINT, wxPaintEventHandler( AnalyzeFrameBase::OnPaintGraph ), NULL, this );
	m_graph->Connect( wxEVT_SIZE, wxSizeEventHandler( AnalyzeFrameBase::OnSizeGraph ), NULL, this );
	m_vplus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVPlus ), NULL, this );
	m_vminus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVMinus ), NULL, this );
	m_vreset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVReset ), NULL, this );
	m_ra->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnCheck ), NULL, this );
	m_dec->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnCheck ), NULL, this );
	m_hminus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHMinus ), NULL, this );
	m_hplus->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHPlus ), NULL, this );
	m_hreset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHReset ), NULL, this );
}

AnalyzeFrameBase::~AnalyzeFrameBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( AnalyzeFrameBase::OnClose ) );
	m_toggleDrift->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnBtnLeftDown ), NULL, this );
	m_toggleDrift->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnClickDrift ), NULL, this );
	m_toggleFFT->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnBtnLeftDown ), NULL, this );
	m_toggleFFT->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnClickFFT ), NULL, this );
	m_graph->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnalyzeFrameBase::OnLeftDown ), NULL, this );
	m_graph->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( AnalyzeFrameBase::OnLeftUp ), NULL, this );
	m_graph->Disconnect( wxEVT_MOTION, wxMouseEventHandler( AnalyzeFrameBase::OnMove ), NULL, this );
	m_graph->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( AnalyzeFrameBase::OnMouseWheel ), NULL, this );
	m_graph->Disconnect( wxEVT_PAINT, wxPaintEventHandler( AnalyzeFrameBase::OnPaintGraph ), NULL, this );
	m_graph->Disconnect( wxEVT_SIZE, wxSizeEventHandler( AnalyzeFrameBase::OnSizeGraph ), NULL, this );
	m_vplus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVPlus ), NULL, this );
	m_vminus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVMinus ), NULL, this );
	m_vreset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnVReset ), NULL, this );
	m_ra->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnCheck ), NULL, this );
	m_dec->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnCheck ), NULL, this );
	m_hminus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHMinus ), NULL, this );
	m_hplus->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHPlus ), NULL, this );
	m_hreset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnalyzeFrameBase::OnHReset ), NULL, this );
	
}

MyFrame5::MyFrame5( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	
	this->Centre( wxBOTH );
}

MyFrame5::~MyFrame5()
{
}
