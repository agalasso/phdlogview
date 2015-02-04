#include "LogViewFrame.h"
#include "LogViewApp.h"
#include "logparser.h"

#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/grid.h>
#include <wx/dcbuffer.h>
#include <wx/busyinfo.h>
#include <wx/dnd.h>
#include <wx/wupdlock.h>
#include <wx/aboutdlg.h>

#include <algorithm>
#include <fstream>

static GuideLog s_log;

#define MAX_HSCALE_GUIDE 100.0
#define MIN_HSCALE_GUIDE 0.1
#define MAX_HSCALE_CAL 400.0
#define MIN_HSCALE_CAL 5.0
#define DECEL 0.19

#define APP_NAME "PHD2 Log Viewer"
#define APP_VERSION_STR "0.2"

struct DragInfo
{
    bool m_dragging;
    int drag_direction; // 0 = unknown, 1 = horizontal, 2 = vertical
    wxPoint m_lastMousePos;
    wxPoint m_mousePos[2];
    wxLongLong_t m_mouseTime[2];
    wxRealPoint m_rate; // pixels per millisecond
};
static DragInfo s_drag;

struct PointArray
{
    wxPoint *pts;
    unsigned int size;
    PointArray() : pts(0), size(0) { }
    ~PointArray() { delete[] pts; }
};
static PointArray s_tmp;

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

enum { ID_TIMER = 10001, };

wxBEGIN_EVENT_TABLE(LogViewFrame, LogViewFrameBase)
  EVT_MENU(wxID_OPEN, LogViewFrame::OnFileOpen)
  EVT_MENU(wxID_ABOUT, LogViewFrame::OnHelpAbout)
  EVT_MENU(wxID_EXIT, LogViewFrame::OnFileExit)
  EVT_MOUSEWHEEL(LogViewFrame::OnMouseWheel)
  EVT_TIMER(ID_TIMER, LogViewFrame::OnTimer)
wxEND_EVENT_TABLE()

LogViewFrame::LogViewFrame()
    :
    LogViewFrameBase(0),
    m_sessionIdx(-1),
    m_session(0),
    m_calibration(0),
    m_timer(this, ID_TIMER)
{
    SetTitle(APP_NAME);

    m_graph->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_graph->Connect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEventHandler(LogViewFrame::OnCaptureLost), NULL, this);

    SetDropTarget(new FileDropTarget(this));

    wxString geometry = Config->Read("/geometry", wxEmptyString);
    if (!geometry.IsEmpty())
    {
        wxArrayString f = wxSplit(geometry, ';');
        if (f[0] == "1")
            Maximize();
        else {
            long w, h, x, y;
            f[1].ToLong(&w);
            f[2].ToLong(&h);
            f[3].ToLong(&x);
            f[4].ToLong(&y);
            SetSize(w, h);
            SetPosition(wxPoint(x, y));
        }
    }
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

void LogViewFrame::OpenLog(const wxString& filename)
{
    std::ifstream ifs(filename.fn_str());
    if (!ifs.good())
    {
        wxLogError("Cannot open file '%s'.", filename);
        return;
    }

    wxFileName fn(filename);
    SetTitle(wxString::Format(APP_NAME " - %s", fn.GetFullName()));

    // must do this before calling parser since parse will Yield and call OnPaint
    m_sessionIdx = -1;
    m_session = 0;
    m_sessionInfo->Clear();
    m_stats->ClearGrid();
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
            m_sessions->SetCellValue(row, 2, "Guiding");
            m_sessions->SetCellValue(row, 3, durStr(static_cast<GuideSession*>(s)->duration));
        }
        m_sessions->SetCellValue(row, 1, s->date);
    }
    m_sessions->GoToCell(0, 0);
    m_sessions->EndBatch();

    m_graph->Refresh();
}

void LogViewFrame::OnFileExit(wxCommandEvent& event)
{
    Close();
}

void LogViewFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog
        openFileDialog(this, _("Open PHD2 Guide Log"), "", "",
        "Text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    OpenLog(openFileDialog.GetPath());
}

void LogViewFrame::OnHelpAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;

    aboutInfo.SetName(APP_NAME);
    aboutInfo.SetVersion(APP_VERSION_STR);
    aboutInfo.SetDescription(_("A tool for visualizing the data in your PHD2 log file"));
    aboutInfo.SetCopyright("(C) 2015 Andy Galasso <andy.galasso@gmail.com>");
    aboutInfo.SetWebSite("http://adgsoftware.com/phd2utils");
    aboutInfo.AddDeveloper("Andy Galasso");

    wxAboutBox(aboutInfo);
}

inline static void UpdateRange(GraphInfo *ginfo)
{
    ginfo->i0 = (double) ginfo->xofs / ginfo->hscale;
    ginfo->i1 = (double) (ginfo->xofs + ginfo->width) / ginfo->hscale;
}

void LogViewFrame::InitGraph()
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

    GraphInfo& ginfo = m_session->m_ginfo;
    ginfo.xofs = 0;
    ginfo.hscale = scale;
    ginfo.width = width;
    UpdateRange(&ginfo);

    // find max ra or dec
    double mxr = 0.0;
    double mxy = 0.0;
    int mxmass = 0;
    double mxsnr = 0.0;
    for (const auto& e : m_session->entries)
    {
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
    if (mxr > 0.0)
        ginfo.vscale = (double)(m_graph->GetSize().GetHeight() / 2 - 10) / mxr;
    else
        ginfo.vscale = 1.0;

    if (mxmass > 0)
        ginfo.massscale = (double)(m_graph->GetSize().GetHeight() / 2 - 10) / (double)mxmass;
    else
        ginfo.massscale = 1.0;

    if (mxsnr > 0.0)
        ginfo.snrscale = (double)(m_graph->GetSize().GetHeight() / 2 - 10) / mxsnr;
    else
        ginfo.snrscale = 1.0;

    ginfo.hscale0 = ginfo.hscale;
    ginfo.vscale0 = ginfo.vscale;
}

void LogViewFrame::InitCalDisplay()
{
    CalDisplay& disp = m_calibration->display;
    disp.xofs = disp.yofs = 0;

    disp.firstWest = -1;
    disp.firstNorth = -1;
    double maxv = 0.0;
    int i = 0;
    for (const auto& p : m_calibration->entries)
    {
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
        maxv = 1.0;
    int size = wxMin(m_graph->GetSize().GetWidth(), m_graph->GetSize().GetHeight());
    disp.scale = (double)(size - 30) / (2.0 * maxv);

    disp.valid = true;
}

void LogViewFrame::OnCellSelected( wxGridEvent& event )
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
        if (loc.type == CALIBRATION_SECTION)
        {
            m_session = 0;
            section = m_calibration = &s_log.calibrations[loc.idx];
        }
        else
        {
            m_calibration = 0;
            section = m_session = &s_log.sessions[loc.idx];
        }

        {
            wxWindowUpdateLocker lck(m_sessionInfo);
            m_sessionInfo->Clear();
            for (const auto& s : section->hdr)
                *m_sessionInfo << s << "\n";
            m_sessionInfo->SetInsertionPoint(0);
        }

        if (m_session)
        {
            m_stats->BeginBatch();
            m_stats->SetCellValue(0, 0, wxString::Format("% .2f\" (%.2f px)", m_session->pixelScale * m_session->rms_ra, m_session->rms_ra));
            m_stats->SetCellValue(1, 0, wxString::Format("% .2f\" (%.2f px)", m_session->pixelScale * m_session->rms_dec, m_session->rms_dec));
            double tot = sqrt(m_session->rms_ra * m_session->rms_ra + m_session->rms_dec * m_session->rms_dec);
            m_stats->SetCellValue(2, 0, wxString::Format("% .2f\" (%.2f px)", m_session->pixelScale * tot, tot));
            m_stats->SetCellValue(0, 1, wxString::Format("% .2f\" (%.2f px)", m_session->pixelScale * m_session->peak_ra, m_session->peak_ra));
            m_stats->SetCellValue(1, 1, wxString::Format("% .2f\" (%.2f px)", m_session->pixelScale * m_session->peak_dec, m_session->peak_dec));
            m_stats->EndBatch();

            if (!m_session->m_ginfo.IsValid())
                InitGraph();
            UpdateScrollbar();
        }
        else
        {
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
    s_drag.drag_direction = 0;
    s_drag.m_lastMousePos = event.GetPosition();
    s_drag.m_mousePos[0] = s_drag.m_mousePos[1] = event.GetPosition();
    s_drag.m_mouseTime[0] = s_drag.m_mouseTime[1] = now(); /*evt.GetTimestamp();*/
    s_drag.m_rate.x = 0.0;
    s_drag.m_rate.y = 0.0;
    m_timer.Stop();

    m_graph->CaptureMouse();
    event.Skip();
}

void LogViewFrame::OnLeftUp( wxMouseEvent& event )
{
    if (!s_drag.m_dragging)
    {
        event.Skip();
        return;
    }

    s_drag.m_dragging = false;
    m_graph->ReleaseMouse();

    wxLongLong_t now = ::now();
    long dt = now - s_drag.m_mouseTime[0];
    if (dt > 0)
    {
        s_drag.m_rate.x = (double)(event.GetPosition().x - s_drag.m_mousePos[0].x) / (double)dt;
        s_drag.m_rate.y = (double)(event.GetPosition().y - s_drag.m_mousePos[0].y) / (double)dt;
        if (fabs(s_drag.m_rate.x) > DECEL || fabs(s_drag.m_rate.y) > DECEL)
        {
            s_drag.m_mousePos[1] = event.GetPosition();
            s_drag.m_mouseTime[1] = now;
            m_timer.Start(20, true);
        }
    }

    event.Skip();
}

void LogViewFrame::OnMove( wxMouseEvent& event )
{
    if (m_session)
    {
        GraphInfo& ginfo = m_session->m_ginfo;

        if (!s_drag.m_dragging)
        {
            int x = event.GetPosition().x;
            int i = (int)(ginfo.i0 + x / ginfo.hscale);
            const GuideSession::EntryVec& entries = m_session->entries;
            if (i >= 0 && i < (int)entries.size())
            {
                const GuideEntry& ent = entries[i];
                wxDateTime t(m_session->starts + wxTimeSpan(0, 0, ent.dt));
                m_rowInfo->SetValue(wxString::Format("%s Frame %d t=%.2f (x,y)=(%.2f,%.2f) (RA,Dec)=(%.2f,%.2f) guide (%.2f,%.2f) corr (%d,%d) m=%d SNR=%.1f %s",
                    t.FormatISOCombined(' '), ent.frame, ent.dt, ent.dx, ent.dy, ent.raraw, ent.decraw, ent.raguide, ent.decguide, ent.radur, ent.decdur, ent.mass, ent.snr, ent.info));
            }

            event.Skip();
            return;
        }

        const wxPoint& pos = event.GetPosition();
        int dx = pos.x - s_drag.m_lastMousePos.x;
        int dy = pos.y - s_drag.m_lastMousePos.y;
        s_drag.m_lastMousePos = pos;

        int prior_direction = s_drag.drag_direction;
        if (dx == 0)
            s_drag.drag_direction = 2;
        else
            s_drag.drag_direction = 1;

        if (prior_direction != s_drag.drag_direction)
        {
            event.Skip();
            return;
        }

        if (s_drag.drag_direction == 1)
        {
            ginfo.xofs -= dx;
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
        else
            ginfo.vscale *= (dy < 0 ? 1.05 : 1.0 / 1.05);

        m_graph->Refresh();
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
    long dt = now /*evt.GetTimestamp()*/ - s_drag.m_mouseTime[1];
    int dx = (int)floor(s_drag.m_rate.x * dt);
    int dy = (int)floor(s_drag.m_rate.y * dt);

    wxPoint pos(s_drag.m_mousePos[1].x + dx, s_drag.m_mousePos[1].y + dy);

    if (m_session)
    {
        GraphInfo& ginfo = m_session->m_ginfo;
        ginfo.xofs -= dx;
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

    if (s_drag.m_rate.x > DECEL)
        s_drag.m_rate.x -= DECEL;
    else if (s_drag.m_rate.x < -DECEL)
        s_drag.m_rate.x += DECEL;
    else
        s_drag.m_rate.x = 0.0;

    if (s_drag.m_rate.y > DECEL)
        s_drag.m_rate.y -= DECEL;
    else if (s_drag.m_rate.y < -DECEL)
        s_drag.m_rate.y += DECEL;
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
    if (//t == wxEVT_SCROLL_TOP ||
        //t == wxEVT_SCROLL_BOTTOM ||
        //t == wxEVT_SCROLL_LINEUP ||
        //t == wxEVT_SCROLL_LINEDOWN ||
        //t == wxEVT_SCROLL_PAGEUP ||
        //t == wxEVT_SCROLL_PAGEDOWN ||
//        t == wxEVT_SCROLL_THUMBTRACK ||
        t == wxEVT_SCROLL_THUMBRELEASE ||
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

    UpdateRange(&ginfo);
    m_graph->Refresh();
    UpdateScrollbar();

    event.Skip();
}

void LogViewFrame::OnMouseWheel( wxMouseEvent& evt )
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
    wxPen GreyDashPen(wxColour(100, 100, 100), 1, wxDOT);
    dc.SetPen(GreyDashPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    double dr = 5.0;
    if ((int)(dr * scale) >= size / 2)
        dr = 1.0;
    double r = dr;
    while (true)
    {
        int ir = (int)(r * scale);
//        if (ir >= size / 2)
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
            dc.SetPen(wxPen(wxColour(100, 100, 255)));
            break;
        case EAST:
            dc.SetPen(wxPen(wxColour(100, 100, 255).ChangeLightness(30)));
            break;
        case NORTH:
            dc.SetPen(wxPen(wxColour(255, 0, 0)));
            break;
        case BACKLASH:
        case SOUTH:
            dc.SetPen(wxPen(wxColour(255, 0, 0).ChangeLightness(30)));
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
        dc.SetPen(wxPen(wxColour(100, 100, 255), 2));
        dc.DrawLine(x0 + (int)(p0.dx * scale), y0 - (int)(p0.dy * scale),
            x0 + (int)(p1.dx * scale), y0 - (int)(p1.dy * scale));
    }
    if (cal->display.firstNorth != -1 && cal->display.lastNorth != cal->display.firstNorth)
    {
        const auto& p0 = cal->entries[cal->display.firstNorth];
        const auto& p1 = cal->entries[cal->display.lastNorth];
        dc.SetPen(wxPen(wxColour(255, 0, 0), 2));
        dc.DrawLine(x0 + (int)(p0.dx * scale), y0 - (int)(p0.dy * scale),
            x0 + (int)(p1.dx * scale), y0 - (int)(p1.dy * scale));
    }
}

static const wxColour RA_COLOR(100, 100, 255);
static const wxColour DEC_COLOR(255, 0, 0);

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

    unsigned int i0, i1;
    if (entries.size())
    {
        i0 = (unsigned int)std::max(ginfo.i0, 0.0);
        i1 = std::min((unsigned int)ceil(ginfo.i1), entries.size() - 1);
    }
    else
    {
        i0 = 1;
        i1 = 0;
    }
    double x0 = (double)i0 * ginfo.hscale - (double)ginfo.xofs;
    int fullw = m_graph->GetSize().GetWidth();
    int y0 = m_graph->GetSize().GetHeight() / 2;
    unsigned int cnt = (unsigned int) (ceil(ginfo.i1) - floor(ginfo.i0)) + 1;
    if (s_tmp.size < cnt)
    {
        delete[] s_tmp.pts;
        s_tmp.pts = new wxPoint[cnt];
        s_tmp.size = cnt;
    }

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
        double vscale = ginfo.vscale;
        bool const arcsecs = m_units->GetSelection() == 0;
        if (arcsecs) // arc-seconds
            vscale /= m_session->pixelScale;
        double v = (double)m_graph->GetSize().GetHeight() * (0.5 / 6.0) / vscale;
        double m = pow(10, ceil(log10(v)));
        double t;
        if (v < (t = .25 * m))
            v = t;
        else if (v < (t = .5 * m))
            v = t;
        else
            v = m;
        int iv = (int)(v * vscale);

        dc.SetPen(wxPen(wxColour(100, 100, 100), 1, wxDOT));
        double dy = v;
        wxString format = arcsecs ? "%g\"" : "%g";
        for (int y = y0 - iv; y > 0; y -= iv, dy += v)
        {
            dc.DrawLine(0, y, fullw, y);
            dc.DrawText(wxString::Format(format, dy), 3, y + 2);
        }
        dy = -v;
        for (int y = y0 + iv; y < 2 * y0; y += iv, dy -= v)
        {
            dc.DrawLine(0, y, fullw, y);
            dc.DrawText(wxString::Format(format, dy), 3, y + 2);
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
                int y = (int)(m_session->mount.xlim.maxDur * m_session->mount.xRate / 1000.0 * ginfo.vscale);
                dc.SetPen(wxPen(RA_COLOR, 1, wxDOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
            if (m_session->mount.xlim.minMo > 0.0)
            {
                // minMo (pixels)
                int y = (int)(m_session->mount.xlim.minMo * ginfo.vscale);
                dc.SetPen(wxPen(RA_COLOR, 1, wxDOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
        }
        if (m_dec->IsChecked())
        {
            if (m_session->mount.ylim.maxDur > 0.0)
            {
                // max dec (milliseconds) * yRate (px/sec)
                int y = (int)(m_session->mount.ylim.maxDur * m_session->mount.yRate / 1000.0 * ginfo.vscale);
                dc.SetPen(wxPen(DEC_COLOR, 1, wxDOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
            if (m_session->mount.ylim.minMo > 0.0)
            {
                // minMo (pixels)
                int y = (int)(m_session->mount.ylim.minMo * ginfo.vscale);
                dc.SetPen(wxPen(DEC_COLOR, 1, wxDOT));
                dc.DrawLine(0, y0 - y, fullw, y0 - y);
                dc.DrawLine(0, y0 + y, fullw, y0 + y);
            }
        }
    }

    // corrections
    int cwid = ((int)ginfo.hscale * .8);

    // ra corrections
    if (m_corrections->IsChecked() && m_ra->IsChecked() && cwid >= 1)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(RA_COLOR.ChangeLightness(60)));

        double xRate[2];
        xRate[MOUNT] = m_session->mount.xRate / 1000.0; // pixels per millisec
        xRate[AO] = m_session->ao.xRate / 1000.0;

        double xx = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            int x = (int) xx;
            const auto& e = entries[i];
            int height = -(int)(e.radur * xRate[e.mount] * ginfo.vscale);
            if (height > 0)
                dc.DrawRectangle(x, y0, cwid, height);
            else if (height < 0)
                dc.DrawRectangle(x, y0 + height, cwid, -height);

            xx += ginfo.hscale;
        }
    }

    // dec corrections
    if (m_corrections->IsChecked() && m_dec->IsChecked() && cwid >= 1)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(DEC_COLOR.ChangeLightness(60)));

        double yRate[2];
        yRate[MOUNT] = m_session->mount.yRate / 1000.0; // pixels per millisec
        yRate[AO] = m_session->ao.yRate / 1000.0;

        double xx = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            int x = (int)(xx + 0.25 * ginfo.hscale);
            const auto& e = entries[i];
            int height = -(int)(e.decdur * yRate[e.mount] * ginfo.vscale);
            if (height > 0)
                dc.DrawRectangle(x, y0, cwid, height);
            else if (height < 0)
                dc.DrawRectangle(x, y0 + height, cwid, -height);

            xx += ginfo.hscale;
        }
    }

    if (m_mass->IsChecked())
    {
        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            s_tmp.pts[ix].x = (int)x;
            s_tmp.pts[ix].y = y0 - (int)(entries[i].mass * ginfo.massscale);

            ++ix;
            x += ginfo.hscale;
        }
        dc.SetPen(*wxYELLOW_PEN);
        dc.DrawLines(ix, s_tmp.pts);
    }

    if (m_snr->IsChecked())
    {
        unsigned int ix = 0;
        double x = x0;
        for (unsigned int i = i0; i <= i1; i++)
        {
            s_tmp.pts[ix].x = (int)x;
            s_tmp.pts[ix].y = y0 - (int)(entries[i].snr * ginfo.snrscale);

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
            s_tmp.pts[ix].y = y0 - (int)(val * ginfo.vscale);

            ++ix;
            x += ginfo.hscale;
        }
        wxPen raOrDxPen(RA_COLOR, ginfo.hscale < 2.0 ? 1 : 2);
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
            double val = radec ? entries[i].decraw : entries[i].dy;
            s_tmp.pts[ix].y = y0 - (int)(val * ginfo.vscale);

            ++ix;
            x += ginfo.hscale;
        }
        wxPen decOrDyPen(DEC_COLOR, ginfo.hscale < 2.0 ? 1 : 2);
        dc.SetPen(decOrDyPen);
        dc.DrawLines(ix, s_tmp.pts);
    }

    // events
    if (m_events->IsChecked())
    {
        int i1 = std::min((int)entries.size(), (int)floor(ginfo.i1));
        const GuideSession::InfoVec& infos = m_session->infos;
        int prev_end = -999999;
        int row = 1;
        for (const auto& info : infos)
        {
            if (info.idx > i1)
                break;
            int width = dc.GetTextExtent(info.info).x;
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
            wxString s = info.repeats > 1 ? wxString::Format("%d x %s", info.repeats, info.info) : info.info;
            dc.DrawText(s, xpos, m_graph->GetSize().GetHeight() - 16 * row);
        }
    }
}

void LogViewFrame::OnVPlus( wxCommandEvent& event )
{
    if (m_session)
    {
        m_session->m_ginfo.vscale *= 1.1;
        m_graph->Refresh();
    }
}

void LogViewFrame::OnVMinus( wxCommandEvent& event )
{
    if (m_session)
    {
        m_session->m_ginfo.vscale /= 1.1;
        m_graph->Refresh();
    }
}

void LogViewFrame::OnVReset( wxCommandEvent& event )
{
    if (m_session)
    {
        m_session->m_ginfo.vscale = m_session->m_ginfo.vscale0;
        m_graph->Refresh();
    }
}

static void Rescale(GraphInfo *ginfo, double f, unsigned int width)
{
    double s = ginfo->hscale * f;
    if (s < MIN_HSCALE_GUIDE)
    {
        s = MIN_HSCALE_GUIDE;
        f = s / ginfo->hscale;
    }
    if (s > MAX_HSCALE_GUIDE)
    {
        s = MAX_HSCALE_GUIDE;
        f = s / ginfo->hscale;
    }

    // keep center stationary
    double hw = (double)(width / 2);
    ginfo->xofs = (int)(f * ((double)ginfo->xofs + hw) - hw);
    ginfo->hscale = s;
    UpdateRange(ginfo);
}

void LogViewFrame::OnHMinus( wxCommandEvent& event )
{
    if (m_session)
    {
        if (m_session->m_ginfo.hscale <= MIN_HSCALE_GUIDE)
            return;
        Rescale(&m_session->m_ginfo, 1.0 / 1.1, m_graph->GetSize().GetWidth());
        m_graph->Refresh();
        UpdateScrollbar();
    }
    else if (m_calibration)
    {
        if (m_calibration->display.scale <= MIN_HSCALE_CAL)
            return;
        m_calibration->display.scale *= 1.0 / 1.1;
        m_graph->Refresh();
    }
}

void LogViewFrame::OnHPlus( wxCommandEvent& event )
{
    if (m_session)
    {
        if (m_session->m_ginfo.hscale >= MAX_HSCALE_GUIDE)
            return;
        Rescale(&m_session->m_ginfo, 1.1, m_graph->GetSize().GetWidth());
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

void LogViewFrame::OnHReset( wxCommandEvent& event )
{
    if (m_session)
    {
        m_session->m_ginfo.hscale = m_session->m_ginfo.hscale0;
        m_session->m_ginfo.xofs = 0;
        UpdateRange(&m_session->m_ginfo);
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
    double s0 = ginfo.width;
    ginfo.width = event.GetSize().GetWidth();
    ginfo.hscale = (double)ginfo.width / (ginfo.i1 - ginfo.i0);
    ginfo.xofs = (int)(ginfo.hscale * ginfo.i0);
    m_graph->Refresh();
    UpdateScrollbar();
}

void LogViewFrame::OnUnits( wxCommandEvent& event )
{
    m_graph->Refresh();
}

void LogViewFrame::OnAxes( wxCommandEvent& event )
{
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

void LogViewFrame::OnClose(wxCloseEvent& event)
{
    Config->Write("/geometry", wxString::Format("%d;%d;%d;%d;%d",
        IsMaximized() ? 1 : 0,
        GetSize().x, GetSize().y, GetPosition().x, GetPosition().y));
    event.Skip();
}
