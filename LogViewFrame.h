/*
 * This file is part of phdlogview
 *
 * Copyright (C) 2016 Andy Galasso
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
    void OnVPan(wxCommandEvent& event);
    void OnVLock( wxCommandEvent& event);
    void OnHMinus(wxCommandEvent& event);
    void OnHPlus(wxCommandEvent& event);
    void OnHReset(wxCommandEvent& event);
    void OnDevice(wxCommandEvent& event);
    void OnUnits(wxCommandEvent& event);
    void OnAxes(wxCommandEvent& event);
    void OnCorrectionsChecked(wxCommandEvent& event);
    void OnDecChecked(wxCommandEvent& event);
    void OnRAChecked(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& evt);
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnSizeGraph(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnStatsChar(wxKeyEvent& event);

    void InitGraph();
    void InitCalDisplay();
    void UpdateScrollbar();

    wxDECLARE_EVENT_TABLE();
};

class SettingsDialog : public SettingsDialogBase
{
public:
    wxColour m_raColor;
    wxColour m_decColor;

    SettingsDialog(wxWindow *parent) : SettingsDialogBase(parent) { }
    void OnRAColor(wxCommandEvent& event);
    void OnDecColor(wxCommandEvent& event);
};

#endif // __LogViewFrame__
