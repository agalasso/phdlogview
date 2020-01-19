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

#ifndef LOGPARSER_INCLUDED
#define LOGPARSER_INCLUDED

#include <wx/arrstr.h>
#include <wx/datetime.h>
#include <wx/string.h>

#include <iostream>
#include <math.h>
#include <string>
#include <vector>

enum WhichMount { MOUNT, AO, };

// Frame,Time,mount,dx,dy,RARawDistance,DECRawDistance,RAGuideDistance,DECGuideDistance,RADuration,RADirection,DECDuration,DECDirection,
//   XStep,YStep,StarMass,SNR,ErrorCode
struct GuideEntry
{
    int frame;
    float dt;
    WhichMount mount;
    bool included;
    bool guiding;
    float dx;
    float dy;
    float raraw;
    float decraw;
    float raguide;
    float decguide;
    int radur;  // or xstep
    int decdur; // or ystep
    int mass;
    float snr;
    int err;
    std::string info;
};

inline static bool StarWasFound(int err)
{
    // reproduces PHD2's function Star::WasFound
    switch (err) {
        case 0: // STAR_OK
        case 1: // STAR_SATURATED
            return true;
        default:
            return false;
    }
}

struct InfoEntry
{
    int idx;  // index of following frame
    int repeats;
    std::string info;
};

enum CalDirection
{
    WEST,
    EAST,
    BACKLASH,
    NORTH,
    SOUTH,
};

struct CalibrationEntry
{
    CalDirection direction;
    int step;
    float dx;
    float dy;
};

struct Limits
{
    double minMo;
    double maxDur;

    Limits() : minMo(0.0), maxDur(0.0) { }
};

struct Mount
{
    bool isValid;
    double xRate;
    double yRate;
    double xAngle;
    double yAngle;
    Limits xlim;
    Limits ylim;

    Mount() : isValid(false), xRate(1.0), yRate(1.0), xAngle(0.0), yAngle(M_PI_2) { }
};

struct GraphInfo
{
    double hscale;        // pixels per entry
    double vscale;
    double max_ofs;
    double max_snr;
    int max_mass;
    int xofs;             // view offset relative to 0th entry
    int yofs;
    int xmin;
    int xmax;
    int width;
    double i0;
    double i1;

    GraphInfo() : width(0) { }
    bool IsValid() const { return width != 0; }
};

struct LogSection
{
    wxString date;
    wxDateTime starts;
    wxArrayString hdr;

    LogSection(const wxString& dt) : date(dt) { }
};

struct GuideSession : public LogSection
{
    typedef std::vector<GuideEntry> EntryVec;
    typedef std::vector<InfoEntry> InfoVec;

    double duration;
    double pixelScale;
    double declination;
    EntryVec entries;
    InfoVec infos;
    Mount ao;
    Mount mount;

    // computed stats
    double rms_ra;
    double rms_dec;
    double avg_ra, avg_dec; // mean ra and dec
    double theta;  // angle of elongation elipse x-axis
    double lx, ly; // ellipse axes
    double elongation;
    double peak_ra;
    double peak_dec;
    double drift_ra;    // pixels per minute
    double drift_dec;   // pixels per minute
    double paerr;       // polar alignment error, arc-minutes

    GraphInfo m_ginfo;

    GuideSession(const wxString& dt) : LogSection(dt), duration(0.), pixelScale(1.), declination(0.), rms_ra(0.), rms_dec(0.), drift_ra(0.), drift_dec(0.) { }
    void CalcStats();
};

struct CalDisplay
{
    bool valid;
    int xofs;
    int yofs;
    double scale;
    double min_scale;
    int firstWest, lastWest, firstNorth, lastNorth;
    CalDisplay() : valid(false) { }
};

struct Calibration : public LogSection
{
    typedef std::vector<CalibrationEntry> EntryVec;

    WhichMount device;
    EntryVec entries;
    CalDisplay display;

    Calibration(const wxString& dt) : LogSection(dt), device(MOUNT) { }
};

enum SectionType { CALIBRATION_SECTION, GUIDING_SECTION };

struct LogSectionLoc
{
    SectionType type;
    int idx;

    LogSectionLoc(SectionType t, int ix) : type(t), idx(ix) { }
};

struct GuideLog
{
    typedef std::vector<GuideSession> SessionVec;
    typedef std::vector<Calibration> CalibrationVec;
    typedef std::vector<LogSectionLoc> SectionLocVec;

    std::string phd_version;
    SessionVec sessions;
    CalibrationVec calibrations;
    SectionLocVec sections;
};

class LogParser
{
public:
    bool Parse(std::istream& is, GuideLog& log);
};

#endif
