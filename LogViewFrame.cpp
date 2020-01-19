/*
 * This file is part of phdlogview
 *
 * Copyright (C) 2016-2020 Andy Galasso
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, visit the http://fsf.org website.
 */

#include "LogViewFrame.h"
#include "LogViewApp.h"
#include "AnalysisWin.h"
#include "logparser.h"

#include <wx/aboutdlg.h>
#include <wx/busyinfo.h>
#include <wx/clipbrd.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/graphics.h>
#include <wx/grid.h>
#include <wx/log.h>
#include <wx/splitter.h>
#include <wx/stopwatch.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/wfstream.h>
#include <wx/wupdlock.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <sstream>

#define MAX_HSCALE_GUIDE 100.0
#define MIN_HSCALE_GUIDE 0.1
#define MAX_HSCALE_CAL 400.0
#define MIN_HSCALE_CAL 5.0
#define DECEL 0.03
#define MIN_SHOW 25

#define APP_NAME "PHD2 Log Viewer"
#define APP_VERSION_STR "0.6.4"

PointArray s_tmp;
Settings s_settings;

static GuideLog s_log;

enum DragMode
{
    DRAG_PAN,
    DRAG_EXCLUDE,
    DRAG_INCLUDE,
};

struct DragInfo
{
    bool m_dragging;
    DragMode m_dragMode;

    // for DRAG_EXCLUDE / DRAG_INCLUDE
    wxPoint m_anchorPoint;
    wxPoint m_endPoint;
    bool dragMoved;

    DragDirection drag_direction;
    wxPoint m_lastMousePos;
    wxPoint m_mousePos[2];
    wxLongLong_t m_mouseTime[2];
    double decel;
    wxRealPoint m_rate; // pixels per millisecond
};
static DragInfo s_drag;

static int s_analyze_pos;

struct ScatterPlot
{
    wxBitmap *bitmap;
    bool valid;
    ScatterPlot() : bitmap(0), valid(false) { }
    ~ScatterPlot() { delete bitmap; }
    void Invalidate() { valid = false; }
};
static ScatterPlot s_scatter;

struct FileDropTarget : public wxFileDropTarget
{
    LogViewFrame *m_lvf;
    FileDropTarget(LogViewFrame *lvf) : m_lvf(lvf) { }
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
        if (filenames.size() == 1)
        {
            m_lvf->OpenLog(filenames[0]);
            return true;
        }
        return false;
    }
};

enum
{
    ID_TIMER = 10001,
    ID_INCLUDE_ALL,
    ID_INCLUDE_NONE,
    ID_EXCLUDE_SETTLE,
    ID_ANALYZE_GA,
    ID_ANALYZE_ALL,
    ID_ANALYZE_ALL_NORA,
};

wxBEGIN_EVENT_TABLE(LogViewFrame, LogViewFrameBase)
  EVT_MENU(wxID_OPEN, LogViewFrame::OnFileOpen)
  EVT_MENU(wxID_SETTINGS, LogViewFrame::OnFileSettings)
  EVT_MENU(wxID_EXIT, LogViewFrame::OnFileExit)
  EVT_MENU(wxID_HELP, LogViewFrame::OnHelp)
  EVT_MENU(wxID_ABOUT, LogViewFrame::OnHelpAbout)
  EVT_MENU_RANGE(ID_INCLUDE_ALL, ID_EXCLUDE_SETTLE, LogViewFrame::OnMenuInclude)
  EVT_MENU(ID_ANALYZE_GA, LogViewFrame::OnMenuAnalyzeGA)
  EVT_MENU_RANGE(ID_ANALYZE_ALL, ID_ANALYZE_ALL_NORA, LogViewFrame::OnMenuAnalyzeAll)
  EVT_MOUSEWHEEL(LogViewFrame::OnMouseWheel)
  EVT_TIMER(ID_TIMER, LogViewFrame::OnTimer)
wxEND_EVENT_TABLE()

inline static bool vscale_locked()
{
    return s_settings.vscale != 0.0;
}

static void _update_vscale_setting(double vscale)
{
    s_settings.vscale = vscale;
    Config->Write("/vscale", vscale);
}

inline static void update_vscale_setting(double vscale, double pixelScale)
{
    _update_vscale_setting(vscale / pixelScale);
}

inline static double get_vscale_setting(double pixelScale)
{
    return s_settings.vscale * pixelScale;
}

static void InitLegends(bool radec, wxTextCtrl *ra, wxTextCtrl *dec)
{
    if (radec)
    {
        ra->ChangeValue(wxT("\u2015RA"));
        dec->ChangeValue(wxT("\u2015Dec"));
    }
    else
    {
        ra->ChangeValue(wxT("\u2015dx"));
        dec->ChangeValue(wxT("\u2015dy"));
    }
}

void SaveGeometry(const wxFrame *win, const wxString& key)
{
    Config->Write(key, wxString::Format("%d;%d;%d;%d;%d",
                                        win->IsMaximized() ? 1 : 0,
                                        win->GetSize().x, win->GetSize().y,
                                        win->GetPosition().x, win->GetPosition().y));
}

void LoadGeometry(wxFrame *win, const wxString& key)
{
    wxString geometry = Config->Read(key, wxEmptyString);
    if (!geometry.IsEmpty())
    {
        wxArrayString f = wxSplit(geometry, ';');
        if (f[0] == "1")
            win->Maximize();
        long w, h, x, y;
        f[1].ToLong(&w);
        f[2].ToLong(&h);
        f[3].ToLong(&x);
        f[4].ToLong(&y);
        win->SetSize(w, h);
        win->SetPosition(wxPoint(x, y));
    }
}

LogViewFrame::LogViewFrame()
    :
    LogViewFrameBase(0),
    m_sessionIdx(-1),
    m_session(nullptr),
    m_calibration(nullptr),
    m_timer(this, ID_TIMER),
    m_analysisWin(nullptr)
{
    SetTitle(APP_NAME);

    m_graph->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_graph->Connect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEventHandler(LogViewFrame::OnCaptureLost), NULL, this);

    Bind(wxEVT_CHAR_HOOK, &LogViewFrame::OnKeyDown, this);

    SetDropTarget(new FileDropTarget(this));

    LoadGeometry(this, "/geometry");

    // load the two splitter positions
    {
        long val;
        wxString s;
        s = Config->Read("/geometry.splitter1", wxEmptyString);
        if (!s.ToLong(&val))
            val = 215;
        m_splitter1->SetSashPosition(val);
        s = Config->Read("/geometry.splitter2", wxEmptyString);
        if (!s.ToLong(&val))
            val = 330;
        m_splitter2->SetSashPosition(val);
    }

    s_settings.excludeByServer = Config->ReadBool("/settle/excludeByServer", true);
    s_settings.excludeParametric = Config->ReadBool("/settle/excludeParametric", false);
    s_settings.settle.pixels = Config->ReadDouble("/settle/pixels", 1.0);
    s_settings.settle.seconds = Config->ReadDouble("/settle/seconds", 10.0);
    s_settings.raColor = wxColor(Config->Read("/color/ra", wxColor(100, 100, 255).GetAsString(wxC2S_HTML_SYNTAX)));
    s_settings.decColor = wxColor(Config->Read("/color/dec", wxRED->GetAsString(wxC2S_HTML_SYNTAX)));
    s_settings.vscale = Config->ReadDouble("/vscale", 0.0);

    m_raLegend->SetForegroundColour(s_settings.raColor);
    m_decLegend->SetForegroundColour(s_settings.decColor);

    InitLegends(true, m_raLegend, m_decLegend);

    m_vlock->SetValue(vscale_locked());
}

LogViewFrame::~LogViewFrame()
{
    if (m_analysisWin)
        m_analysisWin->Destroy();
}

static wxString durStr(double dur)
{
    wxString s;
    double hrs = floor(dur / 3600.0);
    if (hrs > 0.0)
    {
        s += wxString::Format("%.fh", hrs);
        dur -= hrs * 3600.0;
    }
    double mins = floor(dur / 60.0);
    if (mins > 0.0)
    {
        s += wxString::Format("%.fm", mins);
        dur -= mins * 60.0;
    }
    s += wxString::Format("%.fs", dur);
    return s;
}

inline static void IncludeRange(GuideSession::EntryVec& entries, bool include, unsigned int i = 0, unsigned int i1 = (unsigned int)-1)
{
    for (auto it = entries.begin() + i; i < i1 && it != entries.end(); ++it, ++i)
        it->included = include;
}

inline static void IncludeAll(GuideSession::EntryVec& entries)
{
    IncludeRange(entries, true);
}

inline static void IncludeNone(GuideSession::EntryVec& entries)
{
    IncludeRange(entries, false);
}

static void ExcludeSettlingByAPI(GuideSession *session)
{
    bool settling = false;
    int start_idx = 0;
    auto& infos = session->infos;
    auto& entries = session->entries;

    for (auto it = infos.begin(); it != infos.end(); ++it)
    {
        if (settling)
        {
            if (it->info.find("Settling complete") != wxString::npos || it->info.find("Settling fail") != wxString::npos)
            {
                settling = false;
                IncludeRange(entries, false, start_idx, it->idx);
            }
        }
        else
        {
            if (it->info.find("Settling start") != wxString::npos)
            {
                settling = true;
                start_idx = it->idx;
            }
        }
    }
    if (settling)
        IncludeRange(entries, false, start_idx);
}

static void ExcludeSettlingByDistance(GuideSession *session, const SettleParams& params)
{
    auto& infos = session->infos;
    auto& entries = session->entries;
    double lim2 = params.pixels * params.pixels;

    for (auto it = infos.begin(); it != infos.end(); ++it)
    {
        if (it->info.find("DITHER") != wxString::npos)
        {
            if (it->idx >= (int)entries.size())
                break;
            int start_idx = it->idx;
            auto eit = entries.begin() + it->idx;
            double start_time = eit->dt;
            bool close = false;
            bool settled = false;
            int end_idx = start_idx;

            for (; eit != entries.end(); ++eit, ++end_idx)
            {
                double dx = eit->dx;
                double dy = eit->dy;
                double d2 = dx * dx + dy * dy;
                double t = eit->dt;
                if (d2 < lim2)
                {
                    if (!close)
                    {
                        close = true;
                        start_time = t;
                    }
                    else
                    {
                        double elapsed = t - start_time;
                        if (elapsed > params.seconds)
                        {
                            settled = true;
                            break;
                        }
                    }
                }
                else
                {
                    close = false;
                }
            }
            if (settled)
                IncludeRange(entries, false, start_idx, end_idx);
        }
    }
}

static void ExcludeSettling(GuideSession *session)
{
    if (s_settings.excludeByServer)
        ExcludeSettlingByAPI(session);

    if (s_settings.excludeParametric)
        ExcludeSettlingByDistance(session, s_settings.settle);
}

void LogViewFrame::OpenLog(const wxString& filename)
{
    m_filename.clear();

    std::ifstream ifs(filename.fn_str());
    if (!ifs.good())
    {
        wxLogError("Cannot open file '%s'.", filename);
        return;
    }

    m_sessions->Hide();

    m_filename = filename;

    wxFileName fn(filename);
    SetTitle(wxString::Format(APP_NAME " - %s", fn.GetFullName()));

    // must do this before calling parser since parse will Yield and call OnPaint
    m_sessionIdx = -1;
    m_session = 0;
    m_sessionInfo->Clear();
    m_stats->ClearGrid();
    m_stats2->SetPage(wxEmptyString);
    m_sessions->BeginBatch();
    if (m_sessions->GetNumberRows() > 8)
        m_sessions->DeleteRows(8, m_sessions->GetNumberRows() - 8);
    m_sessions->ClearGrid();
    m_sessions->EndBatch();
    m_rowInfo->Clear();
    wxGetApp().Yield();

    {
        wxWindowDisabler disableAll;
        wxBusyInfo wait("Please wait, working...");
        LogParser().Parse(ifs, s_log);
    }

    if (!s_log.phd_version.empty())
        SetTitle(wxString::Format(APP_NAME " - %s - PHD2 %s", fn.GetFullName(), s_log.phd_version.c_str()));

    // load the grid
    m_sessions->BeginBatch();
    int row = 0;
    for (auto it = s_log.sections.begin(); it != s_log.sections.end(); ++it, ++row)
    {
        if (row >= m_sessions->GetNumberRows())
            m_sessions->AppendRows(1, false);
        m_sessions->SetCellValue(row, 0, wxString::Format("%d", row + 1));
        LogSection *s;
        if (it->type == CALIBRATION_SECTION)
        {
            s = &s_log.calibrations[it->idx];
            m_sessions->SetCellValue(row, 2, "Calibration");
            m_sessions->SetCellValue(row, 3, wxEmptyString);
        }
        else
        {
            s = &s_log.sessions[it->idx];
            GuideSession *session = static_cast<GuideSession*>(s);
            IncludeAll(session->entries);
            ExcludeSettling(session);
            session->CalcStats();
            m_sessions->SetCellValue(row, 2, "Guiding");
            m_sessions->SetCellValue(row, 3, durStr(session->duration));
        }
        m_sessions->SetCellValue(row, 1, s->date);
    }
    m_sessions->GoToCell(0, 0);
    m_sessions->AutoSize();
    m_sessions->EndBatch();

    if (s_log.sections.empty())
    {
        m_sessionInfo->SetValue("(empty log file)");
    }
    else
    {
        // FIXME
        // Hack to cause graph scrollbars be displayed when grid's
        // rows do not fit in the panel
        {
            int x = m_splitter1->GetSashPosition();
            m_splitter1->SetSashPosition(x + 1);
            m_splitter1->SetSashPosition(x);
        } // end hack

        m_sessions->Show();
    }

    s_scatter.Invalidate();
    m_graph->Refresh();
}

void LogViewFrame::OnFileExit(wxCommandEvent& event)
{
    Close();
}

void LogViewFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Open PHD2 Guide Log"),
        Config->Read("/FileOpenDir", wxEmptyString),
        wxEmptyString,
        "PHD2 Guide Logs (*PHD2_GuideLog*.txt)|*PHD2_GuideLog*.txt|"
        "Text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    // save the location
    Config->Write("/FileOpenDir", wxFileName(openFileDialog.GetPath()).GetPath());

    OpenLog(openFileDialog.GetPath());
}

static void OnColor(wxWindow *parent, wxButton *btn, wxColor *color)
{
    wxColourDialog dlg(parent);
    dlg.GetColourData().SetColour(*color);
    if (dlg.ShowModal() == wxID_OK)
    {
        *color = dlg.GetColourData().GetColour();
        btn->SetForegroundColour(*color);
    }
}

void SettingsDialog::OnRAColor(wxCommandEvent& event)
{
    OnColor(this, m_raColorBtn, &m_raColor);
}

void SettingsDialog::OnDecColor(wxCommandEvent& event)
{
    OnColor(this, m_decColorBtn, &m_decColor);
}

void LogViewFrame::OnFileSettings(wxCommandEvent& event)
{
    SettingsDialog dlg(this);

    dlg.m_excludeApi->SetValue(s_settings.excludeByServer);
    dlg.m_excludeByParam->SetValue(s_settings.excludeParametric);
    double pixels = s_settings.settle.pixels;
    dlg.m_settlePixels->SetValidator(wxFloatingPointValidator<double>(2, &pixels, 0));
    double seconds = s_settings.settle.seconds;
    dlg.m_settleSeconds->SetValidator(wxFloatingPointValidator<double>(1, &seconds, 0));
    dlg.m_raColor = s_settings.raColor;
    dlg.m_decColor = s_settings.decColor;
    dlg.m_raColorBtn->SetForegroundColour(dlg.m_raColor);
    dlg.m_decColorBtn->SetForegroundColour(dlg.m_decColor);

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    s_settings.excludeByServer = dlg.m_excludeApi->GetValue();
    s_settings.excludeParametric = dlg.m_excludeByParam->GetValue();
    s_settings.settle.pixels = pixels;
    s_settings.settle.seconds = seconds;
    s_settings.raColor = dlg.m_raColor;
    s_settings.decColor = dlg.m_decColor;

    // in case color changed
    m_graph->Refresh();
    m_raLegend->SetForegroundColour(s_settings.raColor);
    m_decLegend->SetForegroundColour(s_settings.decColor);

    Config->Write("/settle/excludeByServer", s_settings.excludeByServer);
    Config->Write("/settle/excludeParametric", s_settings.excludeParametric);
    Config->Write("/settle/pixels", s_settings.settle.pixels);
    Config->Write("/settle/seconds", s_settings.settle.seconds);
    Config->Write("/color/ra", s_settings.raColor.GetAsString(wxC2S_HTML_SYNTAX));
    Config->Write("/color/dec", s_settings.decColor.GetAsString(wxC2S_HTML_SYNTAX));
}

inline static int IdxFromScreen(const GraphInfo& ginfo, wxCoord x)
{
    return (int)(ginfo.i0 + 0.5 + x / ginfo.hscale);
}

void LogViewFrame::OnRightUp(wxMouseEvent& event)
{
    if (!m_session)
    {
        event.Skip();
        return;
    }

    wxMenu *menu = new wxMenu();

    menu->Append(ID_INCLUDE_ALL, _("Include all frames"));
    menu->Append(ID_INCLUDE_NONE, _("Exclude all frames"));
    menu->Append(ID_EXCLUDE_SETTLE, _("Exclude frames settling"));
    menu->AppendSeparator();

    wxMenuItem *mi1 = menu->Append(ID_ANALYZE_ALL, _("Analyze selected frames"));
    wxMenuItem *mi2 = menu->Append(ID_ANALYZE_ALL_NORA, _("Analyze selected, raw RA"));
    if (!AnalysisWin::CanAnalyzeAll(*m_session))
    {
        mi1->Enable(false);
        mi2->Enable(false);
    }

    wxMenuItem *mi = menu->Append(ID_ANALYZE_GA, _("Analyze unguided section"));

    {
        GraphInfo& ginfo = m_session->m_ginfo;
        int i = IdxFromScreen(ginfo, event.GetPosition().x);
        if (i >= 0 && AnalysisWin::CanAnalyzeGA(*m_session, i))
        {
            s_analyze_pos = i;
        }
        else
        {
            mi->Enable(false);
        }
    }

    wxWindow *w = wxDynamicCast(event.GetEventObject(), wxWindow);
    if (w)
        PopupMenu(menu, ScreenToClient(w->ClientToScreen(event.GetPosition())));

    delete menu;
}

std::string FormatNum(double n)
{
    char buf[20];
#ifdef _MSC_VER
# define snprintf _snprintf
#endif
    snprintf(buf, sizeof(buf), "%+.2f", n);
    return buf;
}

static void InitStats(wxGrid *stats, wxHtmlWindow *stats2, const GuideSession *session)
{
    stats->BeginBatch();
    stats->SetCellValue(0, 0, wxString::Format("% .2f\" (%.2f px)", session->pixelScale * session->rms_ra, session->rms_ra));
    stats->SetCellValue(1, 0, wxString::Format("% .2f\" (%.2f px)", session->pixelScale * session->rms_dec, session->rms_dec));
    double tot = sqrt(session->rms_ra * session->rms_ra + session->rms_dec * session->rms_dec);
    stats->SetCellValue(2, 0, wxString::Format("% .2f\" (%.2f px)", session->pixelScale * tot, tot));
    stats->SetCellValue(0, 1, wxString::Format("% .2f\" (% .2f px)", session->pixelScale * session->peak_ra, session->peak_ra));
    stats->SetCellValue(1, 1, wxString::Format("% .2f\" (% .2f px)", session->pixelScale * session->peak_dec, session->peak_dec));
    stats->SetCellValue(2, 1, wxString::Format(" Elong.: %.f%%", session->elongation * 100.));
    stats->AutoSize();
    stats->EndBatch();

    std::ostringstream os;
    os << "<span style='font-family:monospace;font-size:8;'><table cellspacing=1 cellpadding=1 border=0>"
            "<tr><td>RA Drift</td><td>" << FormatNum(session->drift_ra * session->pixelScale) << "\"/min, " << FormatNum(session->drift_ra) << " px/min</td></tr>"
       <<  "<tr><td>Dec Drift</td><td>" << FormatNum(session->drift_dec * session->pixelScale) << "\"/min, " << FormatNum(session->drift_dec) << " px/min</td></tr>"
       <<  "<tr><td>Polar Alignment Error  </td><td>" << std::fixed << std::setprecision(1) << session->paerr << "'</td></tr>"
       << "</table></span>";
    stats2->SetPage(os.str());
}

static void UpdateStats(wxGrid *stats, wxHtmlWindow *stats2, GuideSession *session)
{
    session->CalcStats();
    InitStats(stats, stats2, session);
}

void LogViewFrame::OnMenuInclude(wxCommandEvent& event)
{
    if (!m_session)
        return;

    if (event.GetId() == ID_INCLUDE_ALL)
    {
        IncludeAll(m_session->entries);
        UpdateStats(m_stats, m_stats2, m_session);
        s_scatter.Invalidate();
        m_graph->Refresh();
    }
    else if (event.GetId() == ID_INCLUDE_NONE)
    {
        IncludeNone(m_session->entries);
        UpdateStats(m_stats, m_stats2, m_session);
        s_scatter.Invalidate();
        m_graph->Refresh();
    }
    else if (event.GetId() == ID_EXCLUDE_SETTLE)
    {
        ExcludeSettling(m_session);
        UpdateStats(m_stats, m_stats2, m_session);
        s_scatter.Invalidate();
        m_graph->Refresh();
    }
}

void LogViewFrame::OnMenuAnalyzeGA(wxCommandEvent& event)
{
    if (!m_analysisWin)
        m_analysisWin = new AnalysisWin(this);

    m_analysisWin->AnalyzeGA(*m_session, s_analyze_pos);

    if (m_analysisWin->IsShown())
    {
        m_analysisWin->m_graph->Refresh();
        m_analysisWin->Raise();
    }
    else
        m_analysisWin->Show();
}

void LogViewFrame::OnMenuAnalyzeAll(wxCommandEvent& event)
{
    if (!m_analysisWin)
        m_analysisWin = new AnalysisWin(this);

    bool undo_ra_corrections = event.GetId() == ID_ANALYZE_ALL_NORA;

    m_analysisWin->AnalyzeAll(*m_session, undo_ra_corrections);

    if (m_analysisWin->IsShown())
    {
        m_analysisWin->m_graph->Refresh();
        m_analysisWin->Raise();
    }
    else
        m_analysisWin->Show();
}

void LogViewFrame::OnHelpAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;

    aboutInfo.SetName(APP_NAME);
    aboutInfo.SetVersion(APP_VERSION_STR);
    aboutInfo.SetDescription(_("A tool for visualizing PHD2 guide log data"));
    aboutInfo.SetCopyright("(C) 2018-2020 Andy Galasso <andy.galasso@gmail.com>");
    aboutInfo.SetWebSite("http://adgsoftware.com/phd2utils");
    aboutInfo.AddDeveloper("Andy Galasso");

    wxAboutBox(aboutInfo);
}

class HelpDialog : public HelpDialogBase
{
public:
    HelpDialog(wxWindow *parent);
    ~HelpDialog();
};

static HelpDialog *s_help;

HelpDialog::HelpDialog(wxWindow *parent) : HelpDialogBase(parent)
{
    static const wxString BR = "<br>";
    wxString s;
    s << "<html>"
        << "<h3>Opening a guide log</h3>"
        << "There are three ways to open a PHD2 Log File:" << BR
        << "1. Menu: File => Open" << BR
        << "2. Drag and drop from Windows explorer" << BR
        << "3. As an argument on the command-line" << BR
        << "<h3>Viewing calibration</h3>"
        << "Drag with the mouse to pan" << BR
        << "Use the mouse wheel or the lower +/- buttons to zoom in and out" << BR
        << "<h3>Viewing guiding data</h3>"
        << "Drag left or right with the mouse to pan, or use the horizontal scrollbar" << BR
        << "The mouse wheel or the lower +/- buttons will zoom the horizontal scale" << BR
        << "Drag up or down with the mouse or use the +/- buttons on the right to change the vertical scale" << BR
        << "<h3>Statistics</h3>"
        << "To exclude a range of points from the statistics, hold down CTRL and drag the mouse," << BR
        << "To include a range of points from the statistics, hold down Shift and drag the mouse," << BR
        << "CTRL-Click on an excluded range to un-exclude the range of points." << BR
        << "Right-click on the graph for some more options." << BR
        << "</html>";

    m_html->SetPage(s);
    s_help = this;
}

HelpDialog::~HelpDialog()
{
    s_help = 0;
}

void LogViewFrame::OnHelp(wxCommandEvent& event)
{
    if (!s_help)
        s_help = new HelpDialog(this);
    s_help->Show();
}

inline static void UpdateRange(GraphInfo *ginfo)
{
    ginfo->i0 = (double) ginfo->xofs / ginfo->hscale;
    ginfo->i1 = (double) (ginfo->xofs + ginfo->width) / ginfo->hscale;
}

void LogViewFrame::InitGraph()
{
    GraphInfo& ginfo = m_session->m_ginfo;

    // find max ra or dec
    double mxr = 0.0;
    double mxy = 0.0;
    int mxmass = 0;
    double mxsnr = 0.0;
    //for (const auto& e : m_session->entries)
    for (auto it = m_session->entries.begin(); it != m_session->entries.end(); ++it)
    {
        const auto& e = *it;

        double val = fabs(e.raraw);
        if (val > mxr)
            mxr = val;
        val = fabs(e.decraw);
        if (val > mxr)
            mxr = val;

        val = fabs(e.dx);
        if (val > mxy)
            mxy = val;
        val = fabs(e.dy);
        if (val > mxy)
            mxy = val;

        if (e.mass > mxmass)
            mxmass = e.mass;
        if (e.snr > mxsnr)
            mxsnr = e.snr;
    }

    ginfo.max_ofs = mxr;
    ginfo.max_mass = mxmass;
    ginfo.max_snr = mxsnr;
    ginfo.yofs = 0;
}

void LogViewFrame::InitCalDisplay()
{
    CalDisplay& disp = m_calibration->display;
    disp.xofs = disp.yofs = 0;

    disp.firstWest = -1;
    disp.firstNorth = -1;
    double maxv = 0.0;
    int i = 0;

    for (auto it = m_calibration->entries.begin(); it != m_calibration->entries.end(); ++it)
    {
        const auto& p = *it;
        if (p.direction == WEST)
        {
            if (disp.firstWest == -1) disp.firstWest = i;
            disp.lastWest = i;
        }
        else if (p.direction == NORTH)
        {
            if (disp.firstNorth == -1) disp.firstNorth = i;
            disp.lastNorth = i;
        }
        double d = fabs(p.dx);
        if (d > maxv) maxv = d;
        d = fabs(p.dy);
        if (d > maxv) maxv = d;
        ++i;
    }

    if (maxv == 0.0)
        maxv = 25.0;
    int size = wxMin(m_graph->GetSize().GetWidth(), m_graph->GetSize().GetHeight());
    size = std::max(size, 40); // prevent -ive scale
    disp.scale = (double)(size - 30) / (2.0 * maxv);
    disp.min_scale = std::min(disp.scale, MIN_HSCALE_CAL);

    disp.valid = true;
}

void LogViewFrame::OnCellSelected(wxGridEvent& event)
{
    int row = event.GetRow();
    m_sessions->SelectRow(row);

    if (row == m_sessionIdx)
        goto out;

    m_sessionIdx = row;

    if (row < (int) s_log.sections.size())
    {
        LogSection *section;
        const LogSectionLoc &loc = s_log.sections[m_sessionIdx];
        bool enableMount = false;
        bool enableAO = false;
        if (loc.type == CALIBRATION_SECTION)
        {
            m_session = 0;
            section = m_calibration = &s_log.calibrations[loc.idx];
            if (m_calibration->device == MOUNT)
                enableMount = true;
            else
                enableAO = true;
        }
        else
        {
            m_calibration = 0;
            section = m_session = &s_log.sessions[loc.idx];
            if (m_session->mount.isValid)
                enableMount = true;
            if (m_session->ao.isValid)
                enableAO = true;
        }

        m_device->Enable(0, enableMount);
        m_device->Enable(1, enableAO);

        if (enableMount && !enableAO)
            m_device->SetSelection(0);
        else if (enableAO && !enableMount)
            m_device->SetSelection(1);

        {
            wxWindowUpdateLocker lck(m_sessionInfo);
            m_sessionInfo->Clear();
            for (auto it = section->hdr.begin(); it != section->hdr.end(); ++it)
                *m_sessionInfo << *it << "\n";
            m_sessionInfo->SetInsertionPoint(0);
        }

        if (m_session)
        {
            bool first = !m_session->m_ginfo.IsValid();
            if (first)
                InitGraph();

            if (!m_mainSizer->IsShown(m_guideControlsSizer))
            {
                bool enable = true;
                m_vplus->Enable(enable);
                m_vminus->Enable(enable);
                m_vreset->Enable(enable);
                m_vpan->Enable(enable);
                m_vlock->Enable(enable);
                m_scrollbar->Enable(enable);

                m_mainSizer->Show(m_guideControlsSizer);
                m_mainSizer->Layout();
            }

            InitStats(m_stats, m_stats2, m_session);

            if (first)
            {
                wxCommandEvent dummy;
                OnHReset(dummy);

                if (!vscale_locked())
                    OnVReset(dummy);
            }
            else
                UpdateScrollbar();
        }
        else
        {
            // do this first so InitCalDisplay has the right window sizes
            if (m_mainSizer->IsShown(m_guideControlsSizer))
            {
                bool enable = false;
                m_vplus->Enable(enable);
                m_vminus->Enable(enable);
                m_vreset->Enable(enable);
                m_vpan->Enable(enable);
                m_vlock->Enable(enable);
                m_scrollbar->Enable(enable);

                m_mainSizer->Hide(m_guideControlsSizer);
                m_mainSizer->Layout();
            }

            m_stats->ClearGrid();
            if (!m_calibration->display.valid)
                InitCalDisplay();
        }
    }
    else
    {
        m_session = 0;
        m_calibration = 0;
        m_sessionInfo->Clear();
        m_stats->ClearGrid();
    }

    m_rowInfo->Clear();
    s_scatter.Invalidate();
    m_graph->Refresh();

out:
    m_graph->SetFocus();
}

inline static wxLongLong_t now()
{
    return ::wxGetUTCTimeMillis().GetValue();
}

void LogViewFrame::OnLeftDown(wxMouseEvent& event)
{
    if (!m_session && !m_calibration)
    {
        event.Skip();
        return;
    }

    s_drag.m_dragging = true;

    if (m_session && event.ControlDown())
    {
        s_drag.m_dragMode = DRAG_EXCLUDE;
        s_drag.m_anchorPoint = s_drag.m_endPoint = event.GetPosition();
        s_drag.dragMoved = false;
    }
    else if (m_session && event.ShiftDown())
    {
        s_drag.m_dragMode = DRAG_INCLUDE;
        s_drag.m_anchorPoint = s_drag.m_endPoint = event.GetPosition();
        s_drag.dragMoved = false;
    }
    else
    {
        s_drag.m_dragMode = DRAG_PAN;
        s_drag.drag_direction = DRAGDIR_UNKNOWN;
        s_drag.m_lastMousePos = event.GetPosition();
        s_drag.m_mousePos[0] = s_drag.m_mousePos[1] = event.GetPosition();
        s_drag.m_mouseTime[0] = s_drag.m_mouseTime[1] = now(); /*evt.GetTimestamp();*/
    }

    s_drag.m_rate.x = 0.0;
    s_drag.m_rate.y = 0.0;
    m_timer.Stop();

    m_graph->CaptureMouse();
    event.Skip();
}

void LogViewFrame::OnLeftUp(wxMouseEvent& event)
{
    if (!s_drag.m_dragging)
    {
        event.Skip();
        return;
    }

    s_drag.m_dragging = false;
    m_graph->ReleaseMouse();

    if (s_drag.m_dragMode == DRAG_PAN)
    {
        wxLongLong_t now = ::now();
        long dt = now - s_drag.m_mouseTime[0];
        if (dt > 0)
        {
            s_drag.m_rate.x = (double)(event.GetPosition().x - s_drag.m_mousePos[0].x) / (double)dt;
            s_drag.m_rate.y = (double)(event.GetPosition().y - s_drag.m_mousePos[0].y) / (double)dt;
            s_drag.decel = DECEL;
            if (fabs(s_drag.m_rate.x) > DECEL || fabs(s_drag.m_rate.y) > DECEL)
            {
                s_drag.m_mousePos[1] = event.GetPosition();
                s_drag.m_mouseTime[1] = now;
                m_timer.Start(20, true);
            }
        }
    }
    else // DRAG_EXCLUDE or DRAG_INCLUDE
    {
        if (m_session)
        {
            GraphInfo& ginfo = m_session->m_ginfo;

            if (s_drag.dragMoved)
            {
                // include/exclude a range
                bool include = s_drag.m_dragMode == DRAG_INCLUDE;
                wxRect rect(s_drag.m_anchorPoint, s_drag.m_endPoint);
                GuideSession::EntryVec& entries = m_session->entries;
                int i0 = (int)(ginfo.i0 + 0.5 + rect.GetLeft() / ginfo.hscale);
                int i1 = (int)(ginfo.i0 + 0.5 + rect.GetRight() / ginfo.hscale);
                if (i1 >= 0 && i0 < (int)entries.size())
                {
                    if (i0 < 0)
                        i0 = 0;
                    if (i1 >= (int) m_session->entries.size())
                        i1 = m_session->entries.size() - 1;

                    for (int j = i0; j <= i1; j++)
                        entries[j].included = include;
                    s_scatter.Invalidate();
                    m_graph->Refresh();
                    UpdateStats(m_stats, m_stats2, m_session);
                }
            }
            else
            {
                // delete excluded range
                int i = IdxFromScreen(ginfo, event.GetPosition().x);
                GuideSession::EntryVec& entries = m_session->entries;
                if (i >= 0 && i < (int)entries.size() && !entries[i].included)
                {
                    for (int j = i; j >= 0; --j)
                    {
                        if (entries[j].included)
                            break;
                        entries[j].included = true;
                    }
                    for (int j = i + 1; j < (int)entries.size(); j++)
                    {
                        if (entries[j].included)
                            break;
                        entries[j].included = true;
                    }
                    s_scatter.Invalidate();
                    m_graph->Refresh();
                    UpdateStats(m_stats, m_stats2, m_session);
                }
            }
        }
    }

    event.Skip();
}

void LogViewFrame::OnMove(wxMouseEvent& event)
{
    if (m_session)
    {
        GraphInfo& ginfo = m_session->m_ginfo;

        if (!s_drag.m_dragging)
        {
            int i = IdxFromScreen(ginfo, event.GetPosition().x);
            const GuideSession::EntryVec& entries = m_session->entries;
            if (i >= 0 && i < (int)entries.size())
            {
                const GuideEntry& ent = entries[i];
                wxDateTime t(m_session->starts + wxTimeSpan(0, 0, 0, (wxLongLong)(ent.dt * 1000.0)));
                m_rowInfo->SetValue(wxString::Format("%s Frame %d t=%.2f (x,y)=(%.2f,%.2f) (RA,Dec)=(%.2f,%.2f) guide (%.2f,%.2f) corr (%d,%d) m=%d SNR=%.1f%s %s",
                    t.FormatISOCombined(' '), ent.frame, ent.dt, ent.dx, ent.dy, ent.raraw, ent.decraw, ent.raguide, ent.decguide, ent.radur, ent.decdur, ent.mass, ent.snr, ent.err == 1 ? " SAT" : "", ent.info));
            }

            event.Skip();
            return;
        }

        if (s_drag.m_dragMode == DRAG_PAN)
        {
            const wxPoint& pos = event.GetPosition();
            int dx = pos.x - s_drag.m_lastMousePos.x;
            int dy = pos.y - s_drag.m_lastMousePos.y;
            s_drag.m_lastMousePos = pos;

            bool vpan = m_vpan->GetValue();

            if (!vpan)
            {
                DragDirection prior_direction = s_drag.drag_direction;
                if (dx == 0)
                {
                    s_drag.drag_direction = DRAGDIR_VERT;
                }
                else
                {
                    s_drag.drag_direction = DRAGDIR_HORZ;
                    dy = 0;
                }

                if (prior_direction != s_drag.drag_direction)
                {
                    event.Skip();
                    return;
                }
            }

            if (dx != 0)
            {
                int newpos = ginfo.xofs - dx;
                if (newpos < ginfo.xmin)
                    newpos = ginfo.xmin;
                else if (newpos > ginfo.xmax)
                    newpos = ginfo.xmax;
                ginfo.xofs = newpos;
                UpdateRange(&ginfo);

                wxLongLong_t now = ::now();
                long dt = now /*evt.GetTimestamp()*/ - s_drag.m_mouseTime[1];
                if (dt >= 20)
                {
                    s_drag.m_mousePos[0] = s_drag.m_mousePos[1];
                    s_drag.m_mouseTime[0] = s_drag.m_mouseTime[1];
                    s_drag.m_mousePos[1] = pos;
                    s_drag.m_mouseTime[1] = now; //evt.GetTimestamp();
                }

                UpdateScrollbar();
            }

            if (dy != 0)
            {
                if (vpan)
                {
                    ginfo.yofs -= dy;
                }
                else
                {
                    double vscale = vscale_locked() ? get_vscale_setting(m_session->pixelScale) : ginfo.vscale;
                    vscale *= (dy < 0 ? 1.05 : 1.0 / 1.05);
                    if (vscale_locked())
                        update_vscale_setting(vscale, m_session->pixelScale);
                    else
                        ginfo.vscale = vscale;

                    s_scatter.Invalidate();
                }
            }

            m_graph->Refresh();
        }
        else // DRAG_EXCLUDE / DRAG_INCLUDE
        {
            s_drag.m_endPoint = event.GetPosition();
            wxPoint d = s_drag.m_endPoint - s_drag.m_anchorPoint;
            if (d.x > 2 || d.x < -2 || d.y > 2 || d.y < -2)
                s_drag.dragMoved = true;
            m_graph->Refresh();
        }
    }
    else if (m_calibration && s_drag.m_dragging)
    {
        const wxPoint& pos = event.GetPosition();
        int dx = pos.x - s_drag.m_lastMousePos.x;
        int dy = pos.y - s_drag.m_lastMousePos.y;
        s_drag.m_lastMousePos = pos;

        m_calibration->display.xofs -= dx;
        m_calibration->display.yofs -= dy;

        wxLongLong_t now = ::now();
        long dt = now - s_drag.m_mouseTime[1];
        if (dt >= 20)
        {
            s_drag.m_mousePos[0] = s_drag.m_mousePos[1];
            s_drag.m_mouseTime[0] = s_drag.m_mouseTime[1];
            s_drag.m_mousePos[1] = pos;
            s_drag.m_mouseTime[1] = now;
        }

        m_graph->Refresh();
    }

    event.Skip();
}

void LogViewFrame::OnCaptureLost(wxMouseCaptureLostEvent& evt)
{
    s_drag.m_dragging = false;
    evt.Skip();
}

void LogViewFrame::OnTimer(wxTimerEvent& evt)
{
    wxLongLong_t now = ::now();
    long dt = now - s_drag.m_mouseTime[1];
    int dx = (int)floor(s_drag.m_rate.x * dt);
    int dy = (int)floor(s_drag.m_rate.y * dt);

    wxPoint pos(s_drag.m_mousePos[1].x + dx, s_drag.m_mousePos[1].y + dy);

    if (m_session)
    {
        GraphInfo& ginfo = m_session->m_ginfo;
        ginfo.xofs -= dx;
        if (ginfo.xofs < ginfo.xmin)
            ginfo.xofs = ginfo.xmin;
        else if (ginfo.xofs > ginfo.xmax)
            ginfo.xofs = ginfo.xmax;
        UpdateRange(&ginfo);
        UpdateScrollbar();
        m_graph->Refresh();
    }
    else if (m_calibration)
    {
        CalDisplay& disp = m_calibration->display;
        disp.xofs -= dx;
        disp.yofs -= dy;
        m_graph->Refresh();
    }

    double decel = s_drag.decel;
    s_drag.decel += DECEL;
    if (s_drag.m_rate.x > decel)
        s_drag.m_rate.x -= decel;
    else if (s_drag.m_rate.x < -decel)
        s_drag.m_rate.x += decel;
    else
        s_drag.m_rate.x = 0.0;

    if (s_drag.m_rate.y > decel)
        s_drag.m_rate.y -= decel;
    else if (s_drag.m_rate.y < -decel)
        s_drag.m_rate.y += decel;
    else
        s_drag.m_rate.y = 0.0;

    if (s_drag.m_rate.x != 0.0 || s_drag.m_rate.y != 0.0)
    {
        s_drag.m_mousePos[1] = pos;
        s_drag.m_mouseTime[1] = now;
        m_timer.Start(20, true);
    }
}

void LogViewFrame::OnScroll(wxScrollEvent& event)
{
    if (!m_session)
    {
        event.Skip();
        return;
    }

    int t = event.GetEventType();
    if (t == wxEVT_SCROLL_THUMBRELEASE ||
        t == wxEVT_SCROLL_CHANGED)
    {
        event.Skip();
        return;
    }

    int pos = event.GetPosition();

    GraphInfo& ginfo = m_session->m_ginfo;
    if (ginfo.xofs < 0)
        ginfo.xofs = 0;
    else
        ginfo.xofs = pos;
    if (ginfo.xofs < ginfo.xmin)
        ginfo.xofs = ginfo.xmin;
    else if (ginfo.xofs > ginfo.xmax)
        ginfo.xofs = ginfo.xmax;

    UpdateRange(&ginfo);
    m_graph->Refresh();
    UpdateScrollbar();

    event.Skip();
}

void LogViewFrame::OnMouseWheel(wxMouseEvent& evt)
{
    if (evt.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
    {
        wxCommandEvent dummy;
        if (evt.GetWheelRotation() > 0)
            OnHPlus(dummy);
        else
            OnHMinus(dummy);
    }
    evt.Skip();
}

void LogViewFrame::UpdateScrollbar()
{
    const GraphInfo& ginfo = m_session->m_ginfo;
    const GuideSession::EntryVec& entries = m_session->entries;
    // screen coords:
    int p0 = std::min(-ginfo.xofs, 0);
    int end1 = (int)(ginfo.hscale * (double)entries.size()) - ginfo.xofs; // end of curve, screen coords
    int end2 = ginfo.width;
    int p1 = std::max(end1, end2);
    // scrollbar coords pos = pscreen - p0 = -p0
    m_scrollbar->SetScrollbar(-p0, ginfo.width, p1 - p0, ginfo.width);
}

static void PaintCalibration(wxDC& dc, const Calibration *cal, wxWindow *graph)
{
    dc.SetTextForeground(wxColour(80, 80, 80));
#if defined(__WXOSX__)
    dc.SetFont(wxSMALL_FONT->Smaller());
#else
    dc.SetFont(wxSWISS_FONT->Smaller());
#endif

    int size = wxMin(graph->GetSize().GetWidth(), graph->GetSize().GetHeight());
    int x0 = graph->GetSize().GetWidth() / 2 - cal->display.xofs;
    int y0 = graph->GetSize().GetHeight() / 2 - cal->display.yofs;

    double const scale = cal->display.scale;

    int tsize = (int)(25.0 * 2.0 * scale);

    dc.SetPen(*wxGREY_PEN);
    dc.DrawLine(x0 - tsize / 2, y0, x0 + tsize / 2, y0);
    dc.DrawLine(x0, y0 - tsize / 2, x0, y0 + tsize / 2);

    // 5-pixel circles
    wxPen GreyDashPen(wxColour(100, 100, 100), 1, wxPENSTYLE_DOT);
    dc.SetPen(GreyDashPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    double dr = 5.0;
    if ((int)(dr * scale) >= size / 2)
        dr = 1.0;
    double r = dr;
    while (true)
    {
        int ir = (int)(r * scale);
        if (r > 25)
            break;
        dc.DrawCircle(x0, y0, ir);
        dc.DrawText(wxString::Format("%g", r), x0 + ir + 5, y0 - 15);
        r += dr;
    }

    for (auto p = cal->entries.rbegin(); p != cal->entries.rend(); ++p)
    {
        switch (p->direction) {
        case WEST:
            dc.SetPen(wxPen(s_settings.raColor));
            break;
        case EAST:
            dc.SetPen(wxPen(s_settings.raColor.ChangeLightness(30)));
            break;
        case NORTH:
            dc.SetPen(wxPen(s_settings.decColor));
            break;
        case BACKLASH:
        case SOUTH:
            dc.SetPen(wxPen(s_settings.decColor.ChangeLightness(30)));
            break;
        }
        dc.DrawCircle(x0 + (int)(p->dx * scale), y0 - (int)(p->dy * scale), 7);

        char ch = "WEBNS"[p->direction];
        dc.DrawText(wxString::Format("%c%d", ch, p->step), x0 + (int)(p->dx * scale) + 8, y0 - (int)(p->dy * scale) + 8);
    }

    if (cal->display.firstWest != -1 && cal->display.lastWest != cal->display.firstWest)
    {
        const auto& p0 = cal->entries[cal->display.firstWest];
        const auto& p1 = cal->entries[cal->display.lastWest];
        dc.SetPen(wxPen(s_settings.raColor, 2));
        dc.DrawLine(x0 + (int)(p0.dx * scale), y0 - (int)(p0.dy * scale),
            x0 + (int)(p1.dx * scale), y0 - (int)(p1.dy * scale));
    }
    if (cal->display.firstNorth != -1 && cal->display.lastNorth != cal->display.firstNorth)
    {
        const auto& p0 = cal->entries[cal->display.firstNorth];
        const auto& p1 = cal->entries[cal->display.lastNorth];
        dc.SetPen(wxPen(s_settings.decColor, 2));
        dc.DrawLine(x0 + (int)(p0.dx * scale), y0 - (int)(p0.dy * scale),
            x0 + (int)(p1.dx * scale), y0 - (int)(p1.dy * scale));
    }
}

void LogViewFrame::OnPaintGraph(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(m_graph);

    dc.Clear();

    if (m_calibration)
    {
        PaintCalibration(dc, m_calibration, m_graph);
        return;
    }

    if (!m_session)
        return;

    const GuideSession::EntryVec& entries = m_session->entries;
    const GraphInfo& ginfo = m_session->m_ginfo;
    double const vscale = vscale_locked() ? get_vscale_setting(m_session->pixelScale) : ginfo.vscale;

    unsigned int i0, i1;
    if (entries.size())
    {
        i0 = (unsigned int)std::max(ginfo.i0, 0.0);
        i1 = std::min((size_t)ceil(ginfo.i1), entries.size() - 1);
    }
    else
    {
        i0 = 1;
        i1 = 0;
    }
    double x0 = (double)i0 * ginfo.hscale - (double)ginfo.xofs;
    int fullw = m_graph->GetSize().GetWidth();
    int y00 = m_graph->GetSize().GetHeight() / 2;
    int y0 = y00 - ginfo.yofs;
    unsigned int cnt = i1 >= i0 ? i1 - i0 + 1 : 0;
    s_tmp.alloc(cnt);

    bool const radec = m_axes->GetSelection() == 0;

    dc.SetTextForeground(*wxLIGHT_GREY);
#if defined(__WXOSX__)
    dc.SetFont(wxSMALL_FONT->Smaller());
#else
    dc.SetFont(wxSWISS_FONT->Smaller());
#endif

    dc.SetPen(*wxGREY_PEN);
    dc.DrawLine(0, y0, fullw, y0);

    if (m_grid->IsChecked())
    {
        // horizontal grid lines
        double vsc = vscale;
        bool const arcsecs = m_units->GetSelection() == 0;
        if (arcsecs) // arc-seconds
            vsc /= m_session->pixelScale;
        double v = (double)m_graph->GetSize().GetHeight() * (0.5 / 6.0) / vsc;
        double m = pow(10, ceil(log10(v)));
        double t;
        if (v < (t = .25 * m))
            v = t;
        else if (v < (t = .5 * m))
            v = t;
        else
            v = m;
        int iv = (int)(v * vsc);

        if (iv > 0)
        {
            dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxPENSTYLE_DOT));
            double dy = v;
            wxString format = arcsecs ? "%g\"" : "%g";
            for (int y = y0 - iv; y > 0; y -= iv, dy += v)
            {
                dc.DrawLine(0, y, fullw, y);
                dc.DrawText(wxString::Format(format, dy), 3, y + 2);
            }
            dy = -v;
            for (int y = y0 + iv; y < m_graph->GetSize().GetHeight(); y += iv, dy -= v)
            {
                dc.DrawLine(0, y, fullw, y);
                dc.DrawText(wxString::Format(format, dy), 3, y + 2);
            }
        }

        // vertical ticks
        if (i1 > i0)
        {
            double dt0 = m_session->entries[i0].dt;
            double dt1 = m_session->entries[i1].dt;
            double tspan = dt1 - dt0;
            int x0 = (int)(((double)i0 + 0.5) * ginfo.hscale) - ginfo.xofs;
            int x1 = (int)(((double)i1 + 0.5) * ginfo.hscale) - ginfo.xofs;
            double r = (double)(x1 - x0) / tspan; // pixels per second
            int secs = (int)(ceil(80.0 / (r * 60.0)) * 60.0); // seconds per tick
            wxDateTime ti0(m_session->starts + wxTimeSpan(0, 0, 0, (wxLongLong)(dt0 * 1000.0)));
            wxDateTime t0(ti0);
            time_t ticks = ((t0.GetTicks() + secs - 1) / secs) * secs;
            t0.Set(ticks); // time of first tick
            double t = (double)(t0 - ti0).GetMilliseconds().GetValue() / 1000.0;
            wxDateTime t1(m_session->starts + wxTimeSpan(0, 0, 0, (wxLongLong)(dt1 * 1000.0)));
            double tend = (double)(t1 - ti0).GetMilliseconds().GetValue() / 1000.0;

            dc.SetPen(*wxGREY_PEN);
            for (; t < tend; t += secs)
            {
                int x = (int)((double) x0 + r * t);
                dc.DrawLine(x, 0, x, 10);
                wxDateTime wxt(ti0 + wxTimeSpan(0, 0, 0, (wxLongLong)(t * 1000.0)));
                dc.DrawText(wxt.Format("%H:%M"), x + 3, 1);
            }
        }
    }

    // limits
    if (m_limits->IsChecked())
    {
        if (m_ra->IsChecked())
        {
            if (m_session->mount.xlim.maxDur > 0.0)
            {
                // max ra (milliseconds) * xRate (px/sec)
                int y = (int)(m_session->mount.xlim.maxDur * m_session->mount.xRate / 1000.0 * vscale);
                dc.SetPen(wxPen(s_settings.raColor, 1, wxPENSTYLE_DOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
            if (m_session->mount.xlim.minMo > 0.0)
            {
                // minMo (pixels)
                int y = (int)(m_session->mount.xlim.minMo * vscale);
                dc.SetPen(wxPen(s_settings.raColor, 1, wxPENSTYLE_DOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
        }
        if (m_dec->IsChecked())
        {
            if (m_session->mount.ylim.maxDur > 0.0)
            {
                // max dec (milliseconds) * yRate (px/sec)
                int y = (int)(m_session->mount.ylim.maxDur * m_session->mount.yRate / 1000.0 * vscale);
                dc.SetPen(wxPen(s_settings.decColor, 1, wxPENSTYLE_DOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
            if (m_session->mount.ylim.minMo > 0.0)
            {
                // minMo (pixels)
                int y = (int)(m_session->mount.ylim.minMo * vscale);
                dc.SetPen(wxPen(s_settings.decColor, 1, wxPENSTYLE_DOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
        }
    }

    // corrections
    int cwid = ((int)ginfo.hscale * .8);

    WhichMount device = m_device->GetSelection() == 0 ? MOUNT : AO;

    // ra corrections
    if (m_corrections->IsChecked() && m_ra->IsChecked() && cwid >= 1)
    {
        wxString lblE(_("GuideEast"));
        static wxSize szE;
        if (szE.x == 0)
            szE = dc.GetTextExtent(lblE);
        wxColor prev = dc.GetTextForeground();
        dc.SetTextForeground(s_settings.raColor.ChangeLightness(75));
        dc.DrawText(lblE, fullw - szE.GetWidth() - 4, m_graph->GetSize().GetHeight() - 2 * szE.GetHeight() - 2);
        dc.SetTextForeground(prev);

        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(s_settings.raColor.ChangeLightness(60)));

        double xRate[2];
        xRate[MOUNT] = m_session->mount.xRate / 1000.0; // pixels per millisec
        xRate[AO] = m_session->ao.xRate / 1000.0;

        double xx = x0 - 0.4 * ginfo.hscale;
        for (unsigned int i = i0; i <= i1; i++)
        {
            int x = (int) xx;
            const auto& e = entries[i];

            if (e.mount == device)
            {
                int height = (int)(e.radur * xRate[e.mount] * vscale);
                if (height > 0)
                    dc.DrawRectangle(x, y0, cwid, height);
                else if (height < 0)
                    dc.DrawRectangle(x, y0 + height, cwid, -height);
            }

            xx += ginfo.hscale;
        }
    }

    // dec corrections
    if (m_corrections->IsChecked() && m_dec->IsChecked() && cwid >= 1)
    {
        wxString lblN(_("GuideNorth"));
        static wxSize szN;
        if (szN.x == 0)
            szN = dc.GetTextExtent(lblN);
        wxColor prev = dc.GetTextForeground();
        dc.SetTextForeground(s_settings.decColor.ChangeLightness(75));
        dc.DrawText(lblN, fullw - szN.GetWidth() - 4, 0 /*topEdge*/ + szN.GetHeight() + 2);
        dc.SetTextForeground(prev);

        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(s_settings.decColor.ChangeLightness(60)));

        double yRate[2];
        yRate[MOUNT] = m_session->mount.yRate / 1000.0; // pixels per millisec
        yRate[AO] = m_session->ao.yRate / 1000.0;

        double xx = x0 - 0.2 * ginfo.hscale;
        for (unsigned int i = i0; i <= i1; i++)
        {
            int x = (int) xx;
            const auto& e = entries[i];

            if (e.mount == device)
            {
                int height = -(int)(e.decdur * yRate[e.mount] * vscale);
                if (height > 0)
                    dc.DrawRectangle(x, y0, cwid, height);
                else if (height < 0)
                    dc.DrawRectangle(x, y0 + height, cwid, -height);
            }

            xx += ginfo.hscale;
        }
    }

    if (m_mass->IsChecked())
    {
        double massscale;
        if (ginfo.max_mass > 0)
            massscale = (double)(m_graph->GetSize().GetHeight() / 2 - 10) / (double)ginfo.max_mass;
        else
            massscale = 1.0;

        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            s_tmp.pts[ix].x = (int)x;
            s_tmp.pts[ix].y = y00 - (int)(entries[i].mass * massscale);

            ++ix;
            x += ginfo.hscale;
        }
        dc.SetPen(*wxYELLOW_PEN);
        dc.DrawLines(ix, s_tmp.pts);
    }

    if (m_snr->IsChecked())
    {
        double snrscale;
        if (ginfo.max_snr > 0.0)
            snrscale = (double)(m_graph->GetSize().GetHeight() / 2 - 10) / ginfo.max_snr;
        else
            snrscale = 1.0;

        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            s_tmp.pts[ix].x = (int)x;
            s_tmp.pts[ix].y = y00 - (int)(entries[i].snr * snrscale);

            ++ix;
            x += ginfo.hscale;
        }
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLines(ix, s_tmp.pts);
    }

    // Ra
    if (m_ra->IsChecked())
    {
        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            wxASSERT(ix < s_tmp.size);
            s_tmp.pts[ix].x = (int)x;
            double val = radec ? entries[i].raraw : entries[i].dx;
            s_tmp.pts[ix].y = y0 + (int)(val * vscale);

            ++ix;
            x += ginfo.hscale;
        }
        wxPen raOrDxPen(s_settings.raColor, ginfo.hscale < 2.0 ? 1 : 2);
        dc.SetPen(raOrDxPen);
        dc.DrawLines(ix, s_tmp.pts);
    }

    // Dec
    if (m_dec->IsChecked())
    {
        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            s_tmp.pts[ix].x = (int)x;
            double val = radec ? -entries[i].decraw : entries[i].dy;
            s_tmp.pts[ix].y = y0 + (int)(val * vscale);

            ++ix;
            x += ginfo.hscale;
        }
        wxPen decOrDyPen(s_settings.decColor, ginfo.hscale < 2.0 ? 1 : 2);
        dc.SetPen(decOrDyPen);
        dc.DrawLines(ix, s_tmp.pts);
    }

    // excluded sections
    if (i1 >= i0)
    {
        bool included = entries[i0].included;
        bool prev_included = included;
        unsigned int e0 = i0;

        wxGraphicsContext *gc = 0;
        for (unsigned int i = i0 + 1; i <= i1; i++)
        {
            bool included = entries[i].included;
            if (included && !prev_included)
            {
                // end of an excluded range, draw it
                if (!gc)
                {
                    gc = wxGraphicsContext::Create(dc);
                    if (gc)
                        gc->SetBrush(wxColour(192, 192, 192, 64));
                }
                if (gc)
                {
                    int x0 = (int)((double)(e0 - 0.25) * ginfo.hscale) - ginfo.xofs;
                    int x1 = (int)((double)(i - 1.0 + 0.25) * ginfo.hscale) - ginfo.xofs;
                    gc->DrawRectangle(x0, 0, x1 - x0 + 1, m_graph->GetSize().GetHeight());
                }
            }
            else if (!included && prev_included)
            {
                // start of excluded range
                e0 = i;
            }
            prev_included = included;
        }
        if (!prev_included)
        {
            if (!gc)
            {
                gc = wxGraphicsContext::Create(dc);
                if (gc)
                    gc->SetBrush(wxColour(192, 192, 192, 64));
            }
            if (gc)
            {
                int x0 = (int)((double)(e0 - 0.25) * ginfo.hscale) - ginfo.xofs;
                int x1 = (int)((double)(i1 + 0.25) * ginfo.hscale) - ginfo.xofs;
                gc->DrawRectangle(x0, 0, x1 - x0 + 1, m_graph->GetSize().GetHeight());
            }
        }

        delete gc;
    }

    // events
    if (m_events->IsChecked())
    {
        int i1 = std::min((int)entries.size(), (int)floor(ginfo.i1));
        const GuideSession::InfoVec& infos = m_session->infos;
        int prev_end = -999999;
        int row = 1;
        for (auto it = infos.begin(); it != infos.end(); ++it)
        {
            const auto& info = *it;
            if (info.idx > i1)
                break;
            wxString s = info.repeats > 1 ? wxString::Format("%d x %s", info.repeats, info.info) : wxString(info.info);
            int width = dc.GetTextExtent(s).x;
            int xpos = info.idx * ginfo.hscale - ginfo.xofs;
            if (info.idx <= (int)i0)
            {
                if (xpos + width <= 0)
                    continue;
            }
            if (xpos < prev_end + 10)
                ++row;
            else
                row = 1;
            if (xpos + width > prev_end)
                prev_end = xpos + width;
            dc.DrawText(s, xpos, m_graph->GetSize().GetHeight() - 16 * row);
        }
    }

    if (s_drag.m_dragging &&
        (s_drag.m_dragMode == DRAG_EXCLUDE || s_drag.m_dragMode == DRAG_INCLUDE) &&
        s_drag.m_anchorPoint.x != s_drag.m_endPoint.x)
    {
        wxRect rect(s_drag.m_anchorPoint, s_drag.m_endPoint);

        wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
        if (gc)
        {
            if (s_drag.m_dragMode == DRAG_EXCLUDE)
                gc->SetBrush(wxColour(192, 192, 192, 64));
            else
                gc->SetBrush(wxColour(255, 255, 92, 64));

            gc->DrawRectangle(rect.x, 0, rect.width, m_graph->GetSize().GetHeight());
            delete gc;
        }
    }

    // scatter plot
    if (m_scatter->IsChecked())
    {
        int h = m_graph->GetSize().GetHeight() / 2 - 40;
        if (h < 140) h = 140;

        if (!s_scatter.valid)
        {
            if (!s_scatter.bitmap || s_scatter.bitmap->GetSize().GetWidth() != h)
            {
                delete s_scatter.bitmap;
                s_scatter.bitmap = new wxBitmap(h, h);
            }
            wxMemoryDC mdc(*s_scatter.bitmap);

            mdc.SetBrush(*wxBLACK_BRUSH);
            mdc.SetPen(*wxGREY_PEN);
            mdc.DrawRectangle(0, 0, h, h);
            mdc.DrawLine(h / 2, 0, h / 2, h);
            mdc.DrawLine(0, h / 2, h, h / 2);

            mdc.SetPen(*wxYELLOW_PEN);

            double scale = vscale * h * 0.5 / (double)(m_graph->GetSize().GetHeight() / 2 - 10);

            for (auto it = entries.begin(); it != entries.end(); ++it)
            {
                if (it->included)
                {
                    int x = (int)((double)(radec ? it->raraw : it->dx) * scale);
                    int y = (int)((double)(radec ? it->decraw : it->dy) * scale);
                    mdc.DrawPoint(h / 2 + x, h / 2 - y);
                }
            }

            // draw an ellipse showing the elongation
            {
                double const expand = 3.0;
                double ew = m_session->lx * scale * expand;
                double ehw = ew * 0.5;
                double eh = m_session->ly * scale * expand;
                double ehh = eh * 0.5;

                wxGraphicsContext *gc = wxGraphicsContext::Create(mdc);

                gc->Translate(h / 2 + m_session->avg_ra * scale, h / 2 - m_session->avg_dec * scale);
                gc->Rotate(-m_session->theta);

                gc->SetPen(wxPen(wxColour(255, 32, 255), 1));
                gc->DrawEllipse(-ehw, -ehh, ew, eh);
                gc->StrokeLine(-ehw * 1.5, 0.0, ehw * 1.5, 0.0);
                gc->StrokeLine(0.0, -ehh * 1.5, 0.0, ehh * 1.5);

                delete gc;
            }

            s_scatter.valid = true;
        }

        dc.DrawBitmap(*s_scatter.bitmap, m_graph->GetSize().GetWidth() - h, 0);
    }
}

void LogViewFrame::OnVPlus( wxCommandEvent& event )
{
    if (m_session)
    {
        if (vscale_locked())
            _update_vscale_setting(s_settings.vscale * 1.1);
        else
            m_session->m_ginfo.vscale *= 1.1;
        s_scatter.Invalidate();
        m_graph->Refresh();
    }
}

void LogViewFrame::OnVMinus( wxCommandEvent& event )
{
    if (m_session)
    {
        if (vscale_locked())
            _update_vscale_setting(s_settings.vscale / 1.1);
        else
            m_session->m_ginfo.vscale /= 1.1;
        s_scatter.Invalidate();
        m_graph->Refresh();
    }
}

void LogViewFrame::OnVReset( wxCommandEvent& event )
{
    if (m_session)
    {
        int height = m_graph->GetSize().GetHeight();
        auto& ginfo = m_session->m_ginfo;

        double vscale;
        if (ginfo.max_ofs > 0.0)
            vscale = (double)(height / 2 - 10) / ginfo.max_ofs;
        else
            vscale = 1.0;

        if (vscale_locked())
            update_vscale_setting(vscale, m_session->pixelScale);
        else
            ginfo.vscale = vscale;

        ginfo.yofs = 0;

        s_scatter.Invalidate();
        m_graph->Refresh();
    }
}

void LogViewFrame::OnVPan(wxCommandEvent& event)
{
    if (m_vpan->GetValue())
        m_vpan->SetLabel("&P/Z");
    else
        m_vpan->SetLabel("P/&Z");
}

void LogViewFrame::OnVLock(wxCommandEvent& event)
{
    if (m_vlock->GetValue())
    {
        update_vscale_setting(m_session->m_ginfo.vscale, m_session->pixelScale);
    }
    else
    {
        // set all GuideSession
        for (auto it = s_log.sections.begin(); it != s_log.sections.end(); ++it)
        {
            if (it->type == GUIDING_SECTION)
            {
                GuideSession &session = static_cast<GuideSession&>(s_log.sessions[it->idx]);
                session.m_ginfo.vscale = get_vscale_setting(session.pixelScale);
            }
        }
        _update_vscale_setting(0.0);
    }
}

static void Rescale(GuideSession *session, double f, unsigned int width)
{
    GraphInfo& ginfo = session->m_ginfo;
    double s = ginfo.hscale * f;
    if (s < MIN_HSCALE_GUIDE)
    {
        s = MIN_HSCALE_GUIDE;
        f = s / ginfo.hscale;
    }
    if (s > MAX_HSCALE_GUIDE)
    {
        s = MAX_HSCALE_GUIDE;
        f = s / ginfo.hscale;
    }

    // keep center stationary
    double hw = (double)(width / 2);
    ginfo.xofs = (int)(f * ((double)ginfo.xofs + hw) - hw);
    ginfo.hscale = s;
    ginfo.xmax = (int)(ginfo.hscale * session->entries.size()) - MIN_SHOW;
    if (ginfo.xofs < ginfo.xmin)
        ginfo.xofs = ginfo.xmin;
    if (ginfo.xofs > ginfo.xmax)
        ginfo.xofs = ginfo.xmax;
    UpdateRange(&ginfo);
}

void LogViewFrame::OnHMinus(wxCommandEvent& event)
{
    if (m_session)
    {
        if (m_session->m_ginfo.hscale <= MIN_HSCALE_GUIDE)
            return;
        Rescale(m_session, 1.0 / 1.1, m_graph->GetSize().GetWidth());
        m_graph->Refresh();
        UpdateScrollbar();
    }
    else if (m_calibration)
    {
        auto& display = m_calibration->display;
        if (display.scale <= display.min_scale)
            return;
        display.scale *= 1.0 / 1.1;
        m_graph->Refresh();
    }
}

void LogViewFrame::OnHPlus(wxCommandEvent& event)
{
    if (m_session)
    {
        if (m_session->m_ginfo.hscale >= MAX_HSCALE_GUIDE)
            return;
        Rescale(m_session, 1.1, m_graph->GetSize().GetWidth());
        m_graph->Refresh();
        UpdateScrollbar();
    }
    else if (m_calibration)
    {
        if (m_calibration->display.scale >= MAX_HSCALE_CAL)
            return;
        m_calibration->display.scale *= 1.1;
        m_graph->Refresh();
    }
}

void LogViewFrame::OnHReset(wxCommandEvent& event)
{
    if (m_session)
    {
        // horizontal scale, pixels per entry
        unsigned int width = m_graph->GetSize().GetWidth();
        unsigned int entries = m_session->entries.size();

        if (entries == 0)
            entries = 1;
        if (entries > width)
            entries = width;
        double scale = (double)width / (double)entries;
        if (scale > MAX_HSCALE_GUIDE)
            scale = MAX_HSCALE_GUIDE;

        auto& ginfo = m_session->m_ginfo;
        ginfo.hscale = scale;
        ginfo.width = width;
        ginfo.xofs = 0;
        ginfo.yofs = 0;
        ginfo.xmin = -ginfo.width + MIN_SHOW;
        ginfo.xmax = (int)(ginfo.hscale * m_session->entries.size()) - MIN_SHOW;
        UpdateRange(&ginfo);
        m_graph->Refresh();
        UpdateScrollbar();
    }
    else if (m_calibration)
    {
        InitCalDisplay();
        m_graph->Refresh();
    }
}

void LogViewFrame::OnSizeGraph(wxSizeEvent& event)
{
    if (m_calibration)
    {
        m_graph->Refresh();
        return;
    }
    if (!m_session)
    {
        event.Skip();
        return;
    }
    GraphInfo& ginfo = m_session->m_ginfo;
    ginfo.width = event.GetSize().GetWidth();
    ginfo.hscale = (double)ginfo.width / (ginfo.i1 - ginfo.i0);
    ginfo.xofs = (int)(ginfo.hscale * ginfo.i0);
    ginfo.xmin = -ginfo.width + MIN_SHOW;
    ginfo.xmax = (int)(ginfo.hscale * m_session->entries.size()) - MIN_SHOW;
    if (ginfo.xofs < ginfo.xmin)
        ginfo.xofs = ginfo.xmin;
    if (ginfo.xofs > ginfo.xmax)
        ginfo.xofs = ginfo.xmax;
    s_scatter.Invalidate();
    m_graph->Refresh();
    UpdateScrollbar();
}

void LogViewFrame::OnDevice( wxCommandEvent& event )
{
    m_graph->Refresh();
}

void LogViewFrame::OnUnits( wxCommandEvent& event )
{
    m_graph->Refresh();

    if (m_analysisWin)
        m_analysisWin->RefreshGraph();
}

void LogViewFrame::OnAxes( wxCommandEvent& event )
{
    InitLegends(m_axes->GetSelection() == 0, m_raLegend, m_decLegend);
    s_scatter.Invalidate();
    m_graph->Refresh();
}

void LogViewFrame::OnCorrectionsChecked( wxCommandEvent& event )
{
    m_graph->Refresh();
}

void LogViewFrame::OnDecChecked( wxCommandEvent& event )
{
    m_graph->Refresh();
}

void LogViewFrame::OnRAChecked( wxCommandEvent& event )
{
    m_graph->Refresh();
}

void LogViewFrame::OnIconize(wxIconizeEvent& event)
{
    event.Skip();
}

void LogViewFrame::OnActivate(wxActivateEvent& event)
{
    event.Skip();
}

void LogViewFrame::OnClose(wxCloseEvent& event)
{
    if (m_analysisWin)
        m_analysisWin->Close(true);
    ::SaveGeometry(this, "/geometry");
    // .. and save the two splitter positions
    Config->Write("/geometry.splitter1", wxString::Format("%d", m_splitter1->GetSashPosition()));
    Config->Write("/geometry.splitter2", wxString::Format("%d", m_splitter2->GetSashPosition()));
    event.Skip();
}

void LogViewFrame::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case 'P':
    case 'Z':
        if (m_vpan->IsEnabled())
        {
            m_vpan->SetValue(!m_vpan->GetValue());
            wxCommandEvent dummy;
            OnVPan(dummy);
            break;
        }
    default:
        event.Skip();
        break;
    }
}

static wxString StatsText(const wxGrid *g)
{
    int minRow = 99, maxRow = -1, minCol = 99, maxCol = -1;
    for (int r = 0; r < g->GetNumberRows(); r++)
        for (int c = 0; c < g->GetNumberCols(); c++)
            if (g->IsInSelection(r, c))
            {
                if (r < minRow) minRow = r;
                if (r > maxRow) maxRow = r;
                if (c < minCol) minCol = c;
                if (c > maxCol) maxCol = c;
            }

    wxString s;

    for (int r = minRow; r <= maxRow; r++)
    {
        if (r > minRow)
            s += "\n";

        for (int c = minCol; c <= maxCol; c++)
        {
            if (c > minCol)
                s += ",";
            if (g->IsInSelection(r, c))
                s += g->GetCellValue(r, c);
        }
    }

    return s;
}

void LogViewFrame::OnStatsChar(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case WXK_CONTROL_A:
        m_stats->SelectAll();
        return;
    case WXK_CONTROL_C:
        if (wxClipboard::Get()->Open())
        {
            wxClipboard::Get()->SetData(new wxTextDataObject(StatsText(m_stats)));
            wxClipboard::Get()->Close();
        }
        return;
    }

    event.Skip();
}

static void _shell_open(const wxString& loc)
{
#if defined(__WXMSW__)
    ::ShellExecute(NULL, _T("open"), loc.fn_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__WXOSX__)
    ::wxExecute("/usr/bin/open '" + loc + "'", wxEXEC_ASYNC);
#else
    ::wxExecute("xdg-open '" + loc + "'", wxEXEC_ASYNC);
#endif
}

void LogViewFrame::OnLaunchEditor(wxCommandEvent& event)
{
    if (!m_filename.empty())
        _shell_open(m_filename);
}
