#ifndef LOGPARSER_INCLUDED
#define LOGPARSER_INCLUDED

#include <wx/arrstr.h>
#include <wx/datetime.h>
#include <wx/string.h>

#include <iostream>
#include <math.h>
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
    wxString info;
};

struct InfoEntry
{
    int idx;  // index of following frame
    int repeats;
    wxString info;
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
    double xRate;
    double yRate;
    double xAngle;
    double yAngle;
    Limits xlim;
    Limits ylim;

    Mount() : xRate(1.0), yRate(1.0), xAngle(0.0), yAngle(M_PI_2) { }
};

struct GraphInfo
{
    double hscale;        // pixels per entry
    double vscale;
    double max_ofs;
    double max_snr;
    int max_mass;
    int xofs;             // view offset relative to 0th entry
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
    EntryVec entries;
    InfoVec infos;
    Mount ao;
    Mount mount;
    double rms_ra;
    double rms_dec;
    double peak_ra;
    double peak_dec;

    GraphInfo m_ginfo;

    GuideSession(const wxString& dt) : LogSection(dt), duration(0.0), pixelScale(1.0), rms_ra(0.0), rms_dec(0.0) { }
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

    EntryVec entries;
    CalDisplay display;

    Calibration(const wxString& dt) : LogSection(dt) { }
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
