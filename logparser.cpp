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

#include "logparser.h"
#include "LogViewApp.h"

#include <algorithm>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/regex.h>

static std::string VERSION_PREFIX("PHD2 version ");
static std::string GUIDING_BEGINS("Guiding Begins at ");
static std::string GUIDING_HEADING("Frame,Time,mount");
static std::string MOUNT_KEY("Mount = ");
static std::string AO_KEY("AO = ");
static std::string PX_SCALE("Pixel scale = ");
static std::string GUIDING_ENDS("Guiding Ends");
static std::string INFO_KEY("INFO: ");
static std::string CALIBRATION_BEGINS("Calibration Begins at ");
static std::string CALIBRATION_HEADING("Direction,Step,dx,dy,x,y,Dist");
static std::string CALIBRATION_ENDS("Calibration complete");
static std::string XALGO("X guide algorithm = ");
static std::string YALGO("Y guide algorithm = ");
static std::string MINMOVE("Minimum move = ");

static char *nstrtok(char *str, const char *delims)
{
    static char *src;

    if (str)
        src = str;

    if (!src)
        return 0;

    char *p;
    char *ret = 0;

    if ((p = strpbrk(src, delims)) != 0)
    {
        *p = 0;
        ret = src;
        src = p + 1;
    }

    return ret;
}

inline static bool toLong(const char *s, long *p)
{
    if (s)
    {
        char *endptr;
        long l = strtol(s, &endptr, 10);
        if (endptr != s)
        {
            *p = l;
            return true;
        }
    }
    return false;
}

inline static bool toDouble(const char *s, double *p)
{
    if (s)
    {
        char *endptr;
        double d = strtod(s, &endptr);
        if (endptr != s)
        {
            *p = d;
            return true;
        }
    }
    return false;
}

inline static void toDouble(const char *s, double *d, double dflt)
{
    double t;
    *d = toDouble(s, &t) ? t : dflt;
}

static bool ParseEntry(const std::string& ln, GuideEntry& e)
{
    char buf[256];
    size_t len = ln.size();
    if (len > sizeof(buf) - 3)
        len = sizeof(buf) - 3;
    memcpy(buf, ln.c_str(), len);
    // sentinel for optional info column
    buf[len] = ',';
    // sentinel for end of string
    buf[len + 1] = ',';
    buf[len + 2] = '\0';

    const char *s;
    long l;
    double d;

    s = nstrtok(buf, ",");
    if (!toLong(s, &l)) return false;
    e.frame = l;

    s = nstrtok(0, ",");
    if (!toDouble(s, &d)) return false;
    e.dt = (float)d;

    s = nstrtok(0, ",");
    if (strcmp(s, "\"Mount\"") == 0)
        e.mount = MOUNT;
    else if (strcmp(s, "\"AO\"") == 0)
        e.mount = AO;
    else
    {
        // older logs have the mount name here; punt on AO for these old logs
        e.mount = MOUNT;
    }

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.dx = (float)d;
    }
    else e.dx = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.dy = (float)d;
    }
    else e.dy = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.raraw = (float)d;
    }
    else e.raraw = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.decraw = (float)d;
    }
    else e.decraw = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.raguide = (float)d;
    }
    else e.raguide = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.decguide = (float)d;
    }
    else e.decguide = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.radur = l;
    }
    else e.radur = 0;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (s[0] == 'E')
            ;
        else if (s[0] == 'W')
            e.radur = -e.radur;
        else
            return false;
    }

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.decdur = l;
    }
    else e.decdur = 0;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (*s == 'N')
            ;
        else if (*s == 'S')
            e.decdur = -e.decdur;
        else
            return false;
    }

    // x step
    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.radur = l;
    }

    // y step
    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.decdur = l;
    }

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.mass = l;
    }
    else e.mass = 0;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toDouble(s, &d)) return false;
        e.snr = (float)d;
    }
    else e.snr = 0.f;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        if (!toLong(s, &l)) return false;
        e.err = l;
    }
    else e.err = 0;

    s = nstrtok(0, ",");
    if (s && *s)
    {
        e.info = s;
        // chop quotes
        if (e.info.length() >= 2)
            e.info = e.info.substr(1, e.info.length() - 2);
    }

    return true;
}

inline static void GetDbl(const std::string& ln, const std::string& key, double *d, double dflt)
{
    size_t pos = ln.find(key);
    if (pos != std::string::npos)
    {
        std::string s = ln.substr(pos + key.length());
        toDouble(s.c_str(), d, dflt);
    }
    else
        *d = dflt;
}

static void ParseMount(const std::string& ln, Mount& mount)
{
    mount.isValid = true;

    GetDbl(ln, ", xAngle = ", &mount.xAngle, 0.0);
    GetDbl(ln, ", xRate = ", &mount.xRate, 1.0);
    GetDbl(ln, ", yAngle = ", &mount.yAngle, M_PI_2);
    GetDbl(ln, ", yRate = ", &mount.yRate, 1.0);

    // older log files had rates in px/ms, newer logs are px/sec
    if (mount.xRate < 0.05)
        mount.xRate *= 1000.0;
    if (mount.yRate < 0.05)
        mount.yRate *= 1000.0;
}

static void GetMinMo(const std::string& ln, Limits *lim)
{
    GetDbl(ln, MINMOVE, &lim->minMo, 0.0);
}

inline static bool StartsWith(const std::string& s, const std::string& pfx)
{
    return s.length() >= pfx.length() &&
        s.compare(0, pfx.length(), pfx) == 0;
}

inline static bool EndsWith(const std::string& s, const std::string& sfx)
{
    return s.length() >= sfx.length() &&
        s.compare(s.length() - sfx.length(), sfx.length(), sfx) == 0;
}

inline static std::string BeforeLast(const std::string& s, char ch)
{
    return s.substr(0, s.rfind(ch));
}

inline static bool IsEmpty(const std::string& s)
{
    return s.find_first_not_of(" \t\r\n") == std::string::npos;
}

static void ParseInfo(const std::string& ln, GuideSession *s)
{
    InfoEntry e;
    e.idx = s->entries.size();
    e.repeats = 1;
    e.info = ln.substr(INFO_KEY.length());

    // trim some useless prefixes
    if (StartsWith(e.info, "SETTLING STATE CHANGE, "))
        e.info = e.info.substr(23);
    else if (StartsWith(e.info, "Guiding parameter change, "))
        e.info = e.info.substr(26);

    // trim extra dither info
    if (StartsWith(e.info, "DITHER"))
    {
        size_t pos = e.info.find(", new lock pos");
        if (pos != std::string::npos)
            e.info = e.info.substr(0, pos);
    }

    // strip extra trailing zeroes after last "."
    if (EndsWith(e.info, "00"))
    {
        wxRegEx re("\\.[0-9]+?(0+)$", wxRE_ADVANCED);
        if (re.Matches(e.info))
        {
            size_t start, len;
            re.GetMatch(&start, &len, 1);
            e.info = e.info.substr(0, start);
        }
    }

    if (s->infos.size() > 0)
    {
        InfoEntry& prev = *s->infos.rbegin();

        // coalesce repeated events, like star lost
        if (e.info == prev.info && e.idx >= prev.idx && e.idx <= prev.idx + prev.repeats)
        {
            ++prev.repeats;
            return;
        }

        if (prev.idx == e.idx)
        {
            // coalesce parameter changes
            if (prev.info.find('=') != std::string::npos && StartsWith(e.info, BeforeLast(prev.info, '=')))
            {
                prev = e;
                return;
            }
            // coalesce set lock pos and dither
            if (StartsWith(e.info, "DITHER") && StartsWith(prev.info, "SET LOCK POS"))
            {
                prev = e;
                return;
            }
        }
    }

    s->infos.push_back(e);
}

static bool ParseCalibration(const std::string& ln, CalibrationEntry& e)
{
    char buf[256];
    size_t len = ln.size();
    if (len > sizeof(buf) - 1)
        len = sizeof(buf) - 1;
    memcpy(buf, ln.c_str(), len);
    buf[len] = '\0';

    const char *s;
    long l;
    double d;

    s = nstrtok(buf, ",");
    if (strcmp(s, "West") == 0 || strcmp(s, "Left") == 0)
        e.direction = WEST;
    else if (strcmp(s, "East") == 0)
        e.direction = EAST;
    else if (strcmp(s, "Backlash") == 0)
        e.direction = BACKLASH;
    else if (strcmp(s, "North") == 0 || strcmp(s, "Up") == 0)
        e.direction = NORTH;
    else if (strcmp(s, "South") == 0)
        e.direction = SOUTH;
    else
        return false;

    s = nstrtok(0, ",");
    if (!toLong(s, &l)) return false;
    e.step = l;

    s = nstrtok(0, ",");
    if (!toDouble(s, &d)) return false;
    e.dx = d;

    s = nstrtok(0, ",");
    if (!toDouble(s, &d)) return false;
    e.dy = d;

    return true;
}

static void rtrim(std::string& ln)
{
    auto end = ln.find_last_not_of(" \r\n\t");
    if (end != std::string::npos && end + 1 < ln.size())
        ln = ln.substr(0, end + 1);
}

static bool is_monotonic(const GuideSession& session)
{
    const auto& entries = session.entries;

    if (entries.size() <= 1)
        return true;

    for (auto it = entries.begin() + 1; it != entries.end(); ++it)
        if (it->dt <= (it - 1)->dt)
            return false;

    return true;
}

static void insert_info(GuideSession& session, const GuideSession::EntryVec::iterator& entrypos,
                        const std::string& info)
{
    auto pos = session.infos.begin();
    while (pos != session.infos.end())
    {
        const auto& e = session.entries[pos->idx];
        if (e.frame >= entrypos->frame)
            break;
        ++pos;
    }
    int idx = entrypos - session.entries.begin();
    InfoEntry ie;
    ie.idx = idx;
    ie.repeats = 1;
    ie.info = info;
    session.infos.insert(pos, ie);
}

static void FixupNonMonotonic(GuideSession& session)
{
    if (is_monotonic(session))
        return;

    // get the median positive interval

    double med;

    {
        std::vector<double> v;
        for (auto it = session.entries.begin() + 1; it != session.entries.end(); ++it)
        {
            double d = it->dt - (it - 1)->dt;
            if (d > 0.)
                v.push_back(d);
        }
        if (v.size() < 1)
            return;
        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
        med = v[v.size() / 2];
    }

    // replace any negative interval with the median interval

    double corr = 0.;

    for (auto it = session.entries.begin() + 1; it != session.entries.end(); ++it)
    {
        double d = it->dt + corr - (it - 1)->dt;
        if (d <= 0.)
        {
            corr += med - d;
            insert_info(session, it, "Timestamp jumped backwards");
        }
        it->dt += corr;
    }
}

static void FixupNonMonotonic(GuideLog& log)
{
    for (auto& section : log.sections)
        if (section.type == GUIDING_SECTION)
            FixupNonMonotonic(log.sessions[section.idx]);
}

bool LogParser::Parse(std::istream& is, GuideLog& log)
{
    log.phd_version.clear();
    log.sessions.clear();
    log.calibrations.clear();
    log.sections.clear();

    enum State { SKIP, GUIDING_HDR, GUIDING, CAL_HDR, CALIBRATING, };
    State st = SKIP;
    enum HdrState { GLOBAL, AO, MOUNT, };
    HdrState hdrst;
    char axis = ' ';
    GuideSession *s = 0;
    Calibration *cal = 0;
    unsigned int nr = 0;
    bool mount_enabled = false;

    std::string ln;
    while (std::getline(is, ln))
    {
        ++nr;
        if (nr % 200 == 0)
            wxGetApp().Yield();

        rtrim(ln);

redo:
        if (st == SKIP)
        {
            if (StartsWith(ln, GUIDING_BEGINS))
            {
                st = GUIDING_HDR;
                hdrst = GLOBAL;
                mount_enabled = false;
                std::string datestr = ln.substr(GUIDING_BEGINS.length());
                log.sessions.push_back(GuideSession(datestr));
                log.sections.push_back(LogSectionLoc(GUIDING_SECTION, log.sessions.size() - 1));
                s = &log.sessions[log.sessions.size() - 1];
                s->starts.ParseISOCombined(datestr, ' ');
                goto redo;
            }

            if (StartsWith(ln, CALIBRATION_BEGINS))
            {
                st = CAL_HDR;
                std::string datestr = ln.substr(CALIBRATION_BEGINS.length());
                log.calibrations.push_back(Calibration(datestr));
                log.sections.push_back(LogSectionLoc(CALIBRATION_SECTION, log.calibrations.size() - 1));
                cal = &log.calibrations[log.calibrations.size() - 1];
                cal->starts.ParseISOCombined(datestr, ' ');
                goto redo;
            }

            if (StartsWith(ln, VERSION_PREFIX))
            {
                auto pos = VERSION_PREFIX.size();
                auto end = ln.find(", Log version ", pos);
                if (end == std::string::npos)
                {
                    end = ln.find_first_of(" \t\r\n", pos);
                    if (end == std::string::npos)
                        end = ln.size();
                }
                log.phd_version = ln.substr(pos, end - pos);
                // fall through and skip it
            }
        }
        else if (st == GUIDING_HDR)
        {
            if (StartsWith(ln, GUIDING_HEADING))
            {
                st = GUIDING;
                continue;
            }
            else if (StartsWith(ln, MOUNT_KEY))
            {
                ParseMount(ln, s->mount);
                hdrst = MOUNT;
                mount_enabled = ln.find(", guiding enabled, ") != std::string::npos;
            }
            else if (StartsWith(ln, AO_KEY))
            {
                ParseMount(ln, s->ao);
                hdrst = AO;
            }
            else if (StartsWith(ln, PX_SCALE))
            {
                GetDbl(ln, "Pixel scale = ", &s->pixelScale, 1.0);
            }
            else if (StartsWith(ln, XALGO))
            {
                GetMinMo(ln, hdrst == MOUNT ? &s->mount.xlim : &s->ao.xlim);
                axis = 'X';
            }
            else if (StartsWith(ln, YALGO))
            {
                GetMinMo(ln, hdrst == MOUNT ? &s->mount.ylim : &s->ao.ylim);
                axis = 'Y';
            }
            else if (StartsWith(ln, MINMOVE))
            {
                if (axis == 'X')
                    GetMinMo(ln, hdrst == MOUNT ? &s->mount.xlim : &s->ao.xlim);
                else if (axis == 'Y')
                    GetMinMo(ln, hdrst == MOUNT ? &s->mount.ylim : &s->ao.ylim);
            }
            else if (ln.find("Max RA duration = ") != std::string::npos)
            {
                // Max RA duration = 2000, Max DEC duration = 2000
                Mount& mnt = hdrst == MOUNT ? s->mount : s->ao;
                GetDbl(ln, "Max RA duration = ", &mnt.xlim.maxDur, 0.0);
                GetDbl(ln, "Max DEC duration = ", &mnt.ylim.maxDur, 0.0);
            }
            else if (StartsWith(ln, "RA = "))
            {
                double dec;
                GetDbl(ln, " hr, Dec = ", &dec, 0.);
                s->declination = dec * M_PI / 180.;
            }

            s->hdr.push_back(ln);
        }
        else if (st == GUIDING)
        {
            if (IsEmpty(ln) || StartsWith(ln, GUIDING_ENDS))
            {
                const auto& p = s->entries.rbegin();
                if (p != s->entries.rend())
                    s->duration = p->dt;
                s = 0;

                st = SKIP;
                continue;
            }

            if (ln.at(0) >= '1' && ln.at(0) <= '9')
            {
                GuideEntry e;
                if (!ParseEntry(ln, e))
                    continue;

                if (!StarWasFound(e.err))
                {
                    e.included = false;

                    // older logs did not give the error info
                    if (e.info.empty())
                        e.info = "Frame dropped";

                    // fake an info event
                    ln = "INFO: " + e.info;
                    ParseInfo(ln, s);
                }
                else
                {
                    e.included = true;
                }

                e.guiding = mount_enabled;

                s->entries.push_back(e);
                continue;
            }

            if (StartsWith(ln, INFO_KEY))
            {
                ParseInfo(ln, s);

                size_t pos = ln.find("MountGuidingEnabled = ");
                if (pos != std::string::npos)
                    mount_enabled = ln.compare(pos + 22, 4, "true") == 0;
            }
        }
        else if (st == CAL_HDR)
        {
            if (StartsWith(ln, CALIBRATION_HEADING))
            {
                st = CALIBRATING;
                continue;
            }
            cal->hdr.push_back(ln);
        }
        else if (st == CALIBRATING)
        {
            if (IsEmpty(ln) || StartsWith(ln, CALIBRATION_ENDS))
            {
                st = SKIP;
                continue;
            }

            static const std::string WEST_KEY("West,");
            static const std::string EAST_KEY("East,");
            static const std::string BACKLASH_KEY("Backlash,");
            static const std::string NORTH_KEY("North,");
            static const std::string SOUTH_KEY("South,");
            static const std::string LEFT_KEY("Left,");
            static const std::string UP_KEY("Up,");

            bool isCalEntry = false;

            if (StartsWith(ln, WEST_KEY) ||
                StartsWith(ln, EAST_KEY) ||
                StartsWith(ln, BACKLASH_KEY) ||
                StartsWith(ln, NORTH_KEY) ||
                StartsWith(ln, SOUTH_KEY))
            {
                isCalEntry = true;
                cal->device = WhichMount::MOUNT;
            }
            else if (StartsWith(ln, LEFT_KEY) ||
                     StartsWith(ln, UP_KEY))
            {
                isCalEntry = true;
                cal->device = WhichMount::AO;
            }

            if (isCalEntry)
            {
                CalibrationEntry e;
                if (ParseCalibration(ln, e))
                    cal->entries.push_back(e);
            }
            else
            {
                cal->hdr.push_back(ln);
            }
        }
    }

    if (s)
    {
        const auto& p = s->entries.rbegin();
        if (p != s->entries.rend())
            s->duration = p->dt;
    }

    FixupNonMonotonic(log);

    return true;
}
