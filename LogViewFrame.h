#ifndef __LogViewFrame__
#define __LogViewFrame__

#include "LogViewFrameBase.h"

#include <wx/timer.h>

struct GuideSession;
struct Calibration;

class LogViewFrame : public LogViewFrameBase
{
    int m_sessionIdx;
    GuideSession *m_session;
    Calibration *m_calibration;
    wxTimer m_timer;

public:
    LogViewFrame();
    void OpenLog(const wxString& filename);

private:
    void OnFileOpen(wxCommandEvent& event);
    void OnFileSettings(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnHelpAbout(wxCommandEvent& event);
    void OnMenuInclude(wxCommandEvent& event);
    // Handlers for LogViewFrameBase events.
    void OnCellSelected(wxGridEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);
    void OnMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);
    void OnPaintGraph(wxPaintEvent& event);
    void OnVPlus(wxCommandEvent& event);
    void OnVMinus(wxCommandEvent& event);
    void OnVReset(wxCommandEvent& event);
    void OnVLock(wxCommandEvent& event);
    void OnHMinus(wxCommandEvent& event);
    void OnHPlus(wxCommandEvent& event);
    void OnHReset(wxCommandEvent& event);
    void OnUnits(wxCommandEvent& event);
    void OnAxes(wxCommandEvent& event);
    void OnCorrectionsChecked(wxCommandEvent& event);
    void OnDecChecked(wxCommandEvent& event);
    void OnRAChecked(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& evt);
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnSizeGraph(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);

    void InitGraph();
    void InitCalDisplay();
    void UpdateScrollbar();

    wxDECLARE_EVENT_TABLE();
};

#endif // __LogViewFrame__
