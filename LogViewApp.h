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

#ifndef LOGVIEWAPP_INCLUDED
#define LOGVIEWAPP_INCLUDED

#include <wx/app.h>
#include <wx/config.h>

class LogViewFrame;

extern wxConfigBase *Config;

class LogViewApp : public wxApp
{
    LogViewFrame *m_frame;
    wxString m_openFile;

public:
    LogViewApp();
    LogViewFrame *LVFrame() const { return m_frame; }

private:
    bool OnInit();
    int OnExit();
    void OnInitCmdLine(wxCmdLineParser& parser);
    bool OnCmdLineParsed(wxCmdLineParser& parser);
};

wxDECLARE_APP(LogViewApp);

#endif
