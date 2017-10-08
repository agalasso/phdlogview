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

#include "LogViewApp.h"
#include "LogViewFrame.h"

#include <wx/cmdline.h>

#if 1 // FIXME - testing - remove
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
static void gsl_test()
{
    int i;
    double xi, yi, x[10], y[10];

    printf("#m=0,S=17\n");

    for (i = 0; i < 10; i++)
    {
        x[i] = i + 0.5 * sin(i);
        y[i] = i + cos(i * i);
        printf("%g %g\n", x[i], y[i]);
    }

    printf("#m=1,S=0\n");

    {
        gsl_interp_accel *acc
            = gsl_interp_accel_alloc();
        gsl_spline *spline
            = gsl_spline_alloc(gsl_interp_akima, 10);

        gsl_spline_init(spline, x, y, 10);

        for (xi = x[0]; xi < x[9]; xi += 0.01)
        {
            yi = gsl_spline_eval(spline, xi, acc);
            printf("%g %g\n", xi, yi);
        }
        gsl_spline_free(spline);
        gsl_interp_accel_free(acc);
    }
}
#endif

wxConfigBase *Config = 0;

LogViewApp::LogViewApp()
    :
    m_frame(0)
{
    SetVendorName("adgsoftware");
    SetAppName("phdlogview");
    Config = wxConfig::Get();
}

bool LogViewApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxLog::SetActiveTarget(new wxLogStderr());

    m_frame = new LogViewFrame();
    m_frame->Show();

    if (!m_openFile.IsEmpty())
        m_frame->OpenLog(m_openFile);

#if 1 // FIXME- testing remove
    gsl_test();
#endif
    return true;
}

int LogViewApp::OnExit()
{
    return wxApp::OnExit();
}

void LogViewApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.AddParam("filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

bool LogViewApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.GetParamCount() > 1)
        return false;
    if (parser.GetParamCount() == 1)
        m_openFile = parser.GetParam(0);
    return true;
}

wxIMPLEMENT_APP(LogViewApp);
