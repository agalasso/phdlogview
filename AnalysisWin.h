/*
 * This file is part of phdlogview
 *
 * Copyright (C) 2016-2018 Andy Galasso
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

#ifndef ANALYSISWIN_INCLUDED
#define ANALYSISWIN_INCLUDED

#include "LogViewFrame.h"

struct GuideSession;

class Spline
{
    void *spline;
    void *accel;
public:
    Spline() : spline(nullptr), accel(nullptr) { }
    Spline(const double *x, const double *y, size_t n) : spline(nullptr), accel(nullptr) { Init(x, y, n); }
    void Init(const double *x, const double *y, size_t n);
    ~Spline();
    double Eval(double x) const;
};

struct GARun
{
    wxDateTime starts;
    double pixscale;
    size_t len;
    double *t;
    double *rac;  // drift-corrected RA
    double *decc; // drift-corrected Dec
    size_t nfft;
    double *fftx; // FFT period
    double *ffty; // FFT amplitude
    Spline ffts;  // FFT spline for graphing
    double fftymax;
    GARun() : len(0), t(nullptr), rac(nullptr), decc(nullptr), nfft(0), fftx(nullptr), ffty(nullptr) { }
    ~GARun();
    static bool CanAnalyze(const GuideSession& session, size_t begin, size_t end);
    void Analyze(const GuideSession& session, size_t begin, size_t end, bool undo_ra_corrections);
};

class AnalysisWin : public AnalyzeFrameBase
{
public:
    GARun m_garun;
    int m_cursor;

public:
    AnalysisWin(LogViewFrame *parent);
    ~AnalysisWin();

    static bool CanAnalyzeGA(const GuideSession& session, size_t pos);
    static bool CanAnalyzeAll(const GuideSession& session);

    void AnalyzeGA(const GuideSession& session, size_t pos);
    void AnalyzeAll(const GuideSession& session, bool undo_ra_corrections);
    void RefreshGraph();

private:
    void OnClose(wxCloseEvent& event) override;
    void OnSizeGraph(wxSizeEvent& event) override;
    void OnCheck(wxCommandEvent& event) override;
    void OnBtnLeftDown(wxMouseEvent& event) override;
    void OnClickDrift(wxCommandEvent& event) override;
    void OnClickFFT(wxCommandEvent& event) override;
    void OnLeftDown(wxMouseEvent& event) override;
    void OnLeftUp(wxMouseEvent& event) override;
    void OnMouseWheel(wxMouseEvent& event) override;
    void OnMove(wxMouseEvent& event) override;
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnPaintGraph(wxPaintEvent& event) override;
    void OnHMinus(wxCommandEvent& event) override;
    void OnHPlus(wxCommandEvent& event) override;
    void OnHReset(wxCommandEvent& event) override;
    void OnVMinus(wxCommandEvent& event) override;
    void OnVPlus(wxCommandEvent& event) override;
    void OnVReset(wxCommandEvent& event) override;

    wxDECLARE_EVENT_TABLE();
};

inline void AnalysisWin::RefreshGraph()
{
    m_graph->Refresh();
}

#endif
