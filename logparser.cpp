#include "logparser.h"
#include "LogViewApp.h"

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

inline static void GetDbl(const wxString& ln, const wxString& key, double *d, double dflt)
{
    size_t pos = ln.find(key);
    if (pos != wxString::npos)
    {
        wxString s = ln.substr(pos + key.length());
        toDouble(s, d, dflt);
    }
    else
        *d = dflt;
}

static void ParseMount(const wxString& ln, Mount& mount)
{
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

static void GetMinMo(const wxString& ln, Limits *lim)
{
    GetDbl(ln, "Minimum move = ", &lim->minMo, 0.0);
}

static double rms(unsigned int nr, double sx, double sx2)
{
    if (nr == 0)
        return 0.0;
    double const n = (double) nr;
    return sqrt(n * sx2 - sx * sx) / n;
}

inline static bool StartsWith(const std::string& s, const std::string& pfx)
{
    return s.compare(0, pfx.length(), pfx) == 0;
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
    if (e.info.StartsWith("SETTLING STATE CHANGE, "))
        e.info = e.info.substr(23);
    else if (e.info.StartsWith("Guiding parameter change, "))
        e.info = e.info.substr(26);

    // trim extra dither info
    if (e.info.StartsWith("DITHER"))
    {
        size_t pos = e.info.find(", new lock pos");
        if (pos != wxString::npos)
            e.info = e.info.substr(0, pos);
    }

    // strip extra trailing zeroes after last "."
    if (e.info.EndsWith("00"))
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
            if (prev.info.find('=') != wxString::npos && e.info.StartsWith(prev.info.BeforeLast('=')))
            {
                prev = e;
                return;
            }
            // coalesce set lock pos and dither
            if (e.info.StartsWith("DITHER") && prev.info.StartsWith("SET LOCK POS"))
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

void GuideSession::CalcStats()
{
    double sum_ra = 0.0;
    double sum_ra2 = 0.0;
    double sum_dec = 0.0;
    double sum_dec2 = 0.0;
    double peak_r = 0.0;
    double peak_d = 0.0;
    int cnt = 0;

    for (auto it = entries.begin(); it != entries.end(); ++it)
    {
        const GuideEntry& e = *it;
        if (e.included)
        {
            ++cnt;
            sum_ra += e.raraw;
            sum_ra2 += e.raraw * e.raraw;
            if (fabs(e.raraw) > fabs(peak_r))
                peak_r = e.raraw;
            sum_dec += e.decraw;
            sum_dec2 += e.decraw * e.decraw;
            if (fabs(e.decraw) > fabs(peak_d))
                peak_d = e.decraw;
        }
    }

    rms_ra = rms(cnt, sum_ra, sum_ra2);
    rms_dec = rms(cnt, sum_dec, sum_dec2);
    peak_ra = peak_r;
    peak_dec = peak_d;
}

static std::string rtrim(const std::string& ln)
{
    auto end = ln.find_last_not_of(" \r\n\t");
    if (end != std::string::npos)
        return ln.substr(0, end + 1);
    return ln;
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
    GuideSession *s = 0;
    Calibration *cal = 0;
    unsigned int nr = 0;

    std::string ln;
    while (std::getline(is, ln))
    {
        ++nr;
        if (nr % 200 == 0)
            wxGetApp().Yield();

redo:
        if (st == SKIP)
        {
            if (StartsWith(ln, GUIDING_BEGINS))
            {
                st = GUIDING_HDR;
                hdrst = GLOBAL;
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
            }
            else if (StartsWith(ln, YALGO))
            {
                GetMinMo(ln, hdrst == MOUNT ? &s->mount.ylim : &s->ao.ylim);
            }
            else if (ln.find("Max RA duration = ") != std::string::npos)
            {
                // Max RA duration = 2000, Max DEC duration = 2000
                Mount& mnt = hdrst == MOUNT ? s->mount : s->ao;
                GetDbl(ln, "Max RA duration = ", &mnt.xlim.maxDur, 0.0);
                GetDbl(ln, "Max DEC duration = ", &mnt.ylim.maxDur, 0.0);
            }

            s->hdr.push_back(rtrim(ln));
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

                if (e.err && e.mass == 0)
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

                s->entries.push_back(e);
                continue;
            }

            if (StartsWith(ln, INFO_KEY))
            {
                ParseInfo(ln, s);
            }
        }
        else if (st == CAL_HDR)
        {
            if (StartsWith(ln, CALIBRATION_HEADING))
            {
                st = CALIBRATING;
                continue;
            }
            cal->hdr.push_back(rtrim(ln));
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

            if (StartsWith(ln, WEST_KEY) ||
                StartsWith(ln, EAST_KEY) ||
                StartsWith(ln, BACKLASH_KEY) ||
                StartsWith(ln, NORTH_KEY) ||
                StartsWith(ln, SOUTH_KEY) ||
                StartsWith(ln, LEFT_KEY) ||
                StartsWith(ln, UP_KEY))
            {
                CalibrationEntry e;
                if (ParseCalibration(ln, e))
                    cal->entries.push_back(e);
            }
            else
            {
                cal->hdr.push_back(rtrim(ln));
            }
        }
    }

    if (s)
    {
        const auto& p = s->entries.rbegin();
        if (p != s->entries.rend())
            s->duration = p->dt;
    }

    return true;
}
