/*
 * magazine.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "magazine.h"
#include "TVonscreen.h"
#include "math.h"
#include <vdr/osd.h>
#include <vdr/device.h>

void GetOsdSize(int &Width, int &Height, double &Aspect)
{
#if (APIVERSNUM>10707)
    cDevice::PrimaryDevice()->GetOsdSize(Width, Height, Aspect);
#else
    Width=720;
    if (cDevice::PrimaryDevice()->GetVideoSystem()==vsPAL)
    {
        Height=576;
    }
    else
    {
        Height=480;
    }
    Aspect=1;
#endif
}

static tArea Areas[NUMBER_OF_AREAS] =
{
    { 0,0,0,0,8 }, // NAME1_AREA
    { 0,0,0,0,8 }, // NAME2_AREA
    { 0,0,0,0,8 }, // NAME3_AREA
    { 0,0,0,0,8 }, // DATETIME_AREA
    { 0,0,0,0,8 }, // TIMELINE_AREA
    { 0,0,0,0,8 }, // SCHED1_AREA
    { 0,0,0,0,8 }, // SCHED2_AREA
    { 0,0,0,0,8 }, // SCHED3_AREA
    { 0,0,0,0,4 }, // CONTROL_AREA
};

static int CompareSchedules(const void *p1, const void *p2)
{

    int c1nr = 9999; // there should be no one with more than 9999 channels
    int c2nr = 9999;

    cChannel* c1 = Channels.GetByChannelID((*(const cSchedule**)p1)->ChannelID(), true);
    if (c1)
        c1nr = c1->Number();

    cChannel* c2 = Channels.GetByChannelID((*(const cSchedule**)p2)->ChannelID(), true);
    if (c2)
        c2nr = c2->Number();

    return c1nr - c2nr;
}

void magazine::setTransparency(unsigned int *color)
{
    if (!color) return;

    unsigned int alpha,delta,value;

    alpha=47;
    delta=208;
    value=100-tvonscreenCfg.transparency;
    value*=delta;
    value/=100;
    value+=alpha;
    value<<=24;
    *color=(*color & 0x00FFFFFF) | value;
}

void magazine::setcurrentFirstTime(time_t now)
{
    struct tm tm_r;
    localtime_r(&now,&tm_r);
    tm_r.tm_min=0;
    tm_r.tm_sec=0;
    currentFirstTime=mktime(&tm_r);
}

magazine::magazine(class cPlugin *p)
{
    parent=p;
    const cSchedules* Schedules = cSchedules::Schedules(_schedulesLock);

    curmode=SHOW;
    EDIT_curEvent=0;

    /* please edit only these colors.
        use colors with NO ALPHA
        (0xFF......), e.g. clrGreen */
    clrTimeline1=clrBlue;
    clrTimeline2=clrBlack;
    /* ---------------------------- */

    clrSched1=clrTimeline1;
    clrSched2=clrTimeline2;
    clrNames=clrTimeline1;

    setTransparency(&clrSched1);
    setTransparency(&clrSched2);
    setTransparency(&clrTimeline1);
    setTransparency(&clrTimeline2);

    f1=f2=f3=f4=f5=NULL;
    me=NULL;
    met=NULL;
    mes=NULL;
    ev1=ev2=ev3=NULL;
    fullHours=NULL;

    timeline_tested=false;
    timeline_found_conflict=false;

    // Create and sort entries for menu items
    // --------------------------------------

    const cSchedule* Schedule = Schedules->First();

    int num = 0;
    schedArray = NULL;
    while (Schedule)
    {
        schedArray = (const cSchedule **)realloc(schedArray, (num + 1) * sizeof(cSchedule*));

        if (Schedule->GetPresentEvent())
        {
            // if there exist an event -> add to array
            // check if we can get a channel NAME ( XXX )
            // I thougt that this must always work , but doesn't
            cChannel *c = Channels.GetByChannelID(Schedule->ChannelID(), true);
            if (c)
            {
                schedArray[num] = Schedule;
                ++num;
            }
        }
        Schedule = (const cSchedule*)Schedules->Next(Schedule);
    }
    schedArrayNum=num;
    qsort(schedArray, num, sizeof(cSchedule*), CompareSchedules);
    currentFirst=0;

    cChannel *cchannel=Channels.GetByNumber(cDevice::CurrentChannel());
    if (cchannel)
    {
        int currentChannel = cchannel->Number();
        for (int i=0;i<schedArrayNum;i++)
        {
            cChannel *channel = Channels.GetByChannelID(schedArray[i]->ChannelID(), true);
            if (channel->Number()==currentChannel)
            {
                currentFirst=i-1;
                if (currentFirst<0)
                    currentFirst=0;
                break;
            }
        }
    }

    setcurrentFirstTime(time(NULL));
}
magazine::~magazine(void)
{
    if (me) delete me;
    if (met) delete met;
    if (mes) delete mes;
    if (fullHours) delete [] fullHours;
    if (ev1) delete [] ev1;
    if (ev2) delete [] ev2;
    if (ev3) delete [] ev3;
    if (f1) delete f1;
    if (f2) delete f2;
    if (f3) delete f3;
    if (f4) delete f4;
    if (f5) delete f5;
    if (schedArray) free(schedArray);
    if (osd) delete osd;

    me=NULL;
    met=NULL;
    mes=NULL;
    fullHours=NULL;
    ev1=ev2=ev3=NULL;
    f1=f2=f3=f4=f5=NULL;
    schedArray=NULL;
    osd=NULL;
}

void magazine::printLogo(const cSchedule *s,int p)
{
    cChannel* channel;
    const char *txt;
    int currentChannel;

    int a=0;
    switch (p)
    {
    case 0:
    default:
        a=NAME1_AREA;
        break;
    case 1:
        a=NAME2_AREA;
        break;
    case 2:
        a=NAME3_AREA;
        break;
    }
    if (s!=NULL)
    {
        currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
        channel = Channels.GetByChannelID(s->ChannelID(), true);
        txt=channel->ShortName(true);
        // logo: 64x48px

        const char *ConfigDirectory=cPlugin::ConfigDirectory("logos");
        if (tvonscreenCfg.logos && strlen(tvonscreenCfg.logos)>0)
            ConfigDirectory=tvonscreenCfg.logos;
        char *fname=new char[strlen(ConfigDirectory) + 1 + strlen(txt) + strlen(".xpm") + 1];
        sprintf(fname,"%s/%s.xpm",ConfigDirectory,txt);
        DrawXpm(fname,osd,Areas[a].x1,Areas[a].y1,0,tvonscreenCfg.bwlogos);
        delete [] fname;
    }
}
void magazine::printHead(const cSchedule *s,int p)
{
    cChannel* channel;
    const char *txt;
    int currentChannel;

    int a;
    switch (p)
    {
    case 0:
    default:
        a=NAME1_AREA;
        break;
    case 1:
        a=NAME2_AREA;
        break;
    case 2:
        a=NAME3_AREA;
        break;
    }

    osd->DrawRectangle(Areas[a].x1,Areas[a].y1,Areas[a].x2,Areas[a].y2,clrTransparent);

    if (tvonscreenCfg.showChannels || !tvonscreenCfg.showLogos)
    {
        osd->DrawRectangle(Areas[a].x1,Areas[a].y2-f3->Height(),Areas[a].x2,Areas[a].y2,clrNames);
        if (s)
        {
            tColor col=clrWhite;

            currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
            channel = Channels.GetByChannelID(s->ChannelID(), true);
            if (currentChannel==channel->Number())
            {
                col=clrCyan;
            }
            txt=channel->ShortName(true);
            int middlepos=(Areas[a].Width()-f3->Width(txt))/2;
            if ((middlepos<70) && (tvonscreenCfg.showLogos)) middlepos=70; // prevent overlap with icon
            f3->Text(Areas[a].x1+middlepos,Areas[a].y2-f3->Height(),txt,col,clrNames);
        }
    }
    if (p==2) DrawXpm(TVonscreen,osd,Areas[a].x1+ScheduleWidth-112,Areas[a].y1);
}

void magazine::showHeads(bool onlyBG)
{
    const cSchedule *s1=NULL,*s2=NULL,*s3=NULL;

    if (!onlyBG)
    {
        s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
        s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
        s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;
    }

    printHead(s1,0);
    printHead(s2,1);
    printHead(s3,2);

    if (tvonscreenCfg.showLogos)
    {
        printLogo(s1,0);
        printLogo(s2,1);
        printLogo(s3,2);
    }
}

void magazine::showKeys(void)
{
    char txt[100];

    osd->DrawRectangle(Areas[CONTROL_AREA].x1, Areas[CONTROL_AREA].y1, Areas[CONTROL_AREA].x2,
                       Areas[CONTROL_AREA].y2, clrTransparent);
    if (tvonscreenCfg.noInfoLine)
        return;

    txt[0]=0;
    if (curmode==SHOW) sprintf(txt,"%s",tr("Press 1 for help"));
    f4->Text(Areas[CONTROL_AREA].x1,Areas[CONTROL_AREA].y1,txt,
             clrWhite,clrTransparent);
}

void magazine::showDatetime(void)
{
    struct tm tm_r1;
    localtime_r(&currentFirstTime,&tm_r1);
    char dtxt[50];

    strcpy(dtxt,WeekDayName(tm_r1.tm_wday));
    osd->DrawRectangle(Areas[DATETIME_AREA].x1,Areas[DATETIME_AREA].y1,
                       Areas[DATETIME_AREA].x2,Areas[DATETIME_AREA].y2,clrWhite);

    osd->DrawRectangle(2+Areas[DATETIME_AREA].x1,2+Areas[DATETIME_AREA].y1,
                       Areas[DATETIME_AREA].x1+TimelineWidth-2,
                       Areas[DATETIME_AREA].y1+f5->Height(),clrBlack);

    f5->Text((TimelineWidth-f5->Width(dtxt))/2+Areas[DATETIME_AREA].x1,
             Areas[DATETIME_AREA].y1+2,dtxt,clrWhite,clrBlack);

    strftime(dtxt,sizeof(dtxt),tr("%d-%m"),&tm_r1);

    f5->Text((TimelineWidth-f5->Width(dtxt))/2+Areas[DATETIME_AREA].x1,
             Areas[DATETIME_AREA].y1+f5->Height()+1,dtxt,clrBlack,clrWhite);
}

void magazine::showTimeline(void)
{
    int lh=-1;
    int lhc=0;
    tColor hgr[2];
    hgr[0]=clrTimeline1;
    hgr[1]=clrTimeline2;

    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height();
        if (lh!=fullHours[i] && fullHours[i]>=0)
        {
            lh=fullHours[i];
            lhc=lh&1;
        }
        osd->DrawRectangle(Areas[TIMELINE_AREA].x1,Areas[TIMELINE_AREA].y1+y,
                           TimelineWidth,Areas[TIMELINE_AREA].y1+y+f1->Height(),hgr[lhc]);
    }
    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height();
        if (fullHours[i]>=0 && lh!=fullHours[i])
        {
            char txt[50];
            sprintf(txt,"%02d",fullHours[i]);
            lh=fullHours[i];
            f1->Text((TimelineWidth-f1->Width(txt))/2+Areas[TIMELINE_AREA].x1,
                     Areas[TIMELINE_AREA].y1+y,txt,clrWhite);
            if (i+1<evnum && (fullHours[i+1]==lh || fullHours[i+1]==-1))
            {
                strcpy(txt,tr("o'clock"));
                f2->Text((TimelineWidth-f2->Width(txt))/2+Areas[TIMELINE_AREA].x1,
                         Areas[TIMELINE_AREA].y1+y+f1->Height(),txt,clrWhite);
            }
        }
    }
}

void magazine::showSched(cEvent **ev,tMagazineArea area)
{
    cEvent *oldev=NULL,*cev=NULL;

    int j=0;
    const char *txt;
    cString timetxt;
    int lh=-1;
    int lhc=0;

    tColor hgr[2];
    hgr[0]=clrSched1;
    hgr[1]=clrSched2;

    tColor col=clrWhite;

    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height();
        if (lh!=fullHours[i] && fullHours[i]>=0)
        {
            lh=fullHours[i];
            lhc=lh&1;
        }
        osd->DrawRectangle(Areas[area].x1,Areas[area].y1+y,Areas[area].x1+ScheduleWidth,
                           Areas[area].y1+y+f1->Height(),hgr[lhc]);
    }
    /*
        for (int i=0;i<evnum;i++)
        {
            int y=i*f1->Height();

            if (ev[i])
            {
                timetxt=ev[i]->GetTimeString();
                txt=ev[i]->Title();

                f1->Text(Areas[area].x1,y+Areas[area].y1,*timetxt,col);
                f1->Text(f1->Width("00:00 ")+Areas[area].x1,
                         y+Areas[area].y1,ScheduleWidth-f1->Width("00:00 "),
                         20,txt,col);
                mzlog(0," out ev[%d]=%s '%s'",i,*timetxt,txt);
            }

        }
        mzlog(0," ***");
    */
    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height();
        cev=ev[i];
        if (cev!=NULL)
        {
            if (oldev!=cev)
            {
                if (oldev)
                {
                    txt=oldev->ShortText();
                    int cc=f2->TextHeight(ScheduleWidth-f1->Width("00:00 "),txt);
                    if (cc<=i-j)
                    {
                        f2->Text(f1->Width("00:00 ")+Areas[area].x1,(j)*f1->Height()+Areas[area].y1,
                                 ScheduleWidth-f1->Width("00:00 "),i-j,txt,col);
                    }
                }
                col=clrWhite;
                if (EDIT_curEvent==cev->EventID())
                {
                    col=clrYellow;
                    EDIT_curEVI=i;
                }
                timetxt=cev->GetTimeString();
                txt=cev->Title();
                if (i+f1->TextHeight(f1->Width("00:00 ")+Areas[area].x1,txt)>=evnum)
                    break;
                f1->Text(Areas[area].x1,y+Areas[area].y1,timetxt,col);
                j=i+f1->Text(f1->Width("00:00 ")+Areas[area].x1,
                             y+Areas[area].y1,ScheduleWidth-f1->Width("00:00 "),
                             20,txt,col);
                oldev=cev;
            }
        }
    }
    if (oldev)
    {
        txt=oldev->ShortText();
        if (j+f2->TextHeight(f1->Width("00:00 ")+Areas[area].x1,txt)>=evnum)
            f2->Text(f1->Width("00:00 ")+Areas[area].x1,j*f1->Height()+Areas[area].y1,
                     ScheduleWidth-f1->Width("00:00 "),evnum-j,txt,col);
    }
    if (!EDIT_curEvent)
    {
        if (!timeline_tested)
        {
            timeline_tested=true;
            cPlugin *p = cPluginManager::GetPlugin("timeline");
            if (p)
            {
                char *args[]={(char *) "timeline_command_interface",(char *) "conflicts"};
                timeline_found_conflict=p->ProcessArgs(1,args);
            }
        }
        if (timeline_found_conflict)
        {
            osd->DrawRectangle(Areas[area].x1,Areas[area].y2-f2->Height()-6,
                               Areas[area].x1+ScheduleWidth,Areas[area].y2+1,clrWhite);
            osd->DrawRectangle(Areas[area].x1,Areas[area].y2-f2->Height()-4,
                               Areas[area].x1+ScheduleWidth,Areas[area].y2+1,clrYellow);
            const char *txt=tr("Timer conflict!");
            int x=(ScheduleWidth-f2->Width(txt))/2;
            f2->Text(x+Areas[area].x1,Areas[area].y2-f2->Height()-4,txt,clrGreen,clrYellow);
        }
    }
}

void magazine::showScheds()
{
    const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
    const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
    const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

    if (s1) showSched(ev1,SCHED1_AREA);
    if (s2) showSched(ev2,SCHED2_AREA);
    if (s3) showSched(ev3,SCHED3_AREA);
}

const cEvent *magazine::getNext(const cSchedule *s,const cEvent *e)
{
    if (e == NULL)
        return NULL;

    const cEvent *pe = NULL;
    time_t ref = e->StartTime();

    time_t delta = INT_MAX;

    for (int i=0;i< (s->Events())->Count();i++)
    {
        const cEvent *p = (s->Events())->Get(i);
        if (p!=e)
        {
            time_t dt = p->StartTime() - ref;
            if (dt > 0 && dt < delta)
            {
                delta = dt;
                pe = p;
            }
        }
    }
    return pe;
}

const cEvent *magazine::getPrev(const cSchedule *s,const cEvent *e)
{
    if (e == NULL)

        return NULL;

    const cEvent *pe = NULL;
    time_t ref = e->StartTime();
    time_t delta = INT_MAX;


    for (int i=0;i< (s->Events())->Count();i++)
    {
        const cEvent *p = (s->Events())->Get(i);

        if (p!=e)
        {
            time_t dt = ref - p->StartTime();
            if (dt > 0 && dt < delta)
            {
                delta = dt;
                pe = p;
            }
        }
    }
    return pe;
}

void magazine::calcScheds()
{
    const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
    const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
    const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

    struct tm tm_r;
    localtime_r(&currentFirstTime,&tm_r);
    int hour=tm_r.tm_hour;
    int fh=0;
    for (int i=0;i<evnum;i++)
    {
        ev1[i]=NULL;
        ev2[i]=NULL;
        ev3[i]=NULL;
        if (fh==i)
        {
            fullHours[i]=hour;
            fh+=8;
            hour++;
            if (hour==24) hour=0;
        }
        else
        {
            fullHours[i]=-1;
        }
    }

    if (s1) calcSched(s1,ev1);
    if (s2) calcSched(s2,ev2);
    if (s3) calcSched(s3,ev3);
}

const cEvent *magazine::GetEventAfter(const cSchedule *s, time_t StartTime, time_t EndTime) const
{
    const cEvent *p = NULL;
    for (p = s->Events()->First(); p; p = s->Events()->Next(p))
    {
        if ((p->StartTime()>=StartTime) && (p->StartTime()<=EndTime)) break;
    }
    return p;
}

void magazine::calcSched(const cSchedule *s,cEvent **ev)
{
    const cEvent *cev=s->GetEventAround(currentFirstTime);
    if (!cev) return;
    for (;;)
    {
        time_t t=cev->StartTime();
        if (t>=currentFirstTime)
        {
            struct tm tm_r;
            localtime_r(&t,&tm_r);
            int i=(tm_r.tm_hour-fullHours[0])*8;
            if (i<0) return;
            int offs=(tm_r.tm_min*8)/60;
            if (i+offs>=evnum) return;
            if (ev[i+offs]) offs++;
            if (i+offs>=evnum) return;
            ev[i+offs]=(cEvent *)cev;
            //mzlog(0," ev[%d]=%d:%d '%s'",i+offs,tm_r.tm_hour,tm_r.tm_min,cev->Title());
        }
        cev=s->Events()->Next(cev);
        if (!cev) return;
    }
}

void magazine::colorworkaround(cBitmap *b)
{
    b->Reset();
    // This is an ugly work around for color problems with
    //  DVB driver or hardware or vdr handling, who knows
    int c=0;
    b->SetColor(c++,clrTransparent);
    b->SetColor(c++,clrBlack);
    b->SetColor(c++,clrWhite);
    b->SetColor(c++,clrCyan);

    b->SetColor(c++,clrSched1);
    b->SetColor(c++,clrSched2);
    b->SetColor(c++,clrTimeline1);
    b->SetColor(c++,clrTimeline2);

    for (int i=c;i<128;i++)
    {
        b->SetColor(i,0x01000000|i);
    }
    // End work around
}

void magazine::output(void)
{
    showHeads();
    showKeys();
    calcScheds();
    showScheds();
    showDatetime();
    showTimeline();

    osd->Flush();
}

void magazine::outputLR(void)
{
    showHeads();
    calcScheds();
    searchcEvt();
    showScheds();
    showDatetime();
    showTimeline();

    osd->Flush();
}
void magazine::gotoUsertime(int u)
{
    struct tm tm_r;
    localtime_r(&currentFirstTime,&tm_r);
    tm_r.tm_hour=u/100;
    tm_r.tm_min=0;
    tm_r.tm_sec=0;
    setcurrentFirstTime(mktime(&tm_r));
    if (currentFirstTime<time(NULL))
    {
        if (tvonscreenCfg.thenshownextday)
            setcurrentFirstTime(currentFirstTime+60*60*24);
        else
            setcurrentFirstTime(time(NULL));
    }
    output();
}

void magazine::showHelp()
{
    anyFont *usef=f2;
    int j=0;
    const char *txt;
    int lines=ScheduleHeight/usef->Height();
    int width=ScheduleWidth;

    char *helptext[]=
    {
        trNOOP((char *) "NORMAL MODE:"),
        trNOOP((char *) "arrows\n\tmove view"),
        trNOOP((char *) "back\n\tclose TV OnScreen"),
        trNOOP((char *) "red/blue\n\t-/+ one day"),
        trNOOP((char *) "green/yellow\n\tone page left/right"),
        trNOOP((char *) "7/9\n\tone page left/right"),
        trNOOP((char *) "8\n\tgoto current channel"),
        trNOOP((char *) "0\n\tgoto now"),
        trNOOP((char *) "4/5/6\n\tgoto configured time"),
        trNOOP((char *) "ok\n\tswitch to edit mode\n"),
        trNOOP((char *) "EDIT MODE:"),
        trNOOP((char *) "back\n\tback to normal mode"),
        trNOOP((char *) "arrows\n\tmove selected schedule"),
        trNOOP((char *) "record\n\tcreate timer"),
        trNOOP((char *) "ok\n\tshow details"),
        (char *) "|\n(c) 2004 Juergen Schmitz\n\thttp://www.js-home.org/vdr",
        NULL
    };

    int area=SCHED1_AREA;

    osd->DrawRectangle(Areas[SCHED1_AREA].x1, Areas[SCHED1_AREA].y1+1,
                       Areas[SCHED1_AREA].x2+1, Areas[SCHED1_AREA].y2+1, clrSched1);
    osd->DrawRectangle(Areas[SCHED2_AREA].x1, Areas[SCHED2_AREA].y1+1,
                       Areas[SCHED2_AREA].x2+1, Areas[SCHED2_AREA].y2+1, clrSched1);
    osd->DrawRectangle(Areas[SCHED3_AREA].x1, Areas[SCHED3_AREA].y1+1,
                       Areas[SCHED3_AREA].x2+1, Areas[SCHED3_AREA].y2+1, clrSched1);

    osd->DrawRectangle(Areas[SCHED1_AREA].x1+2, Areas[SCHED1_AREA].y1,
                       Areas[SCHED1_AREA].x2+1-3, Areas[SCHED1_AREA].y2+1-3, clrSched2);
    osd->DrawRectangle(Areas[SCHED2_AREA].x1+2, Areas[SCHED2_AREA].y1,
                       Areas[SCHED2_AREA].x2+1-3, Areas[SCHED2_AREA].y2+1-3, clrSched2);
    osd->DrawRectangle(Areas[SCHED3_AREA].x1+2, Areas[SCHED3_AREA].y1,
                       Areas[SCHED3_AREA].x2+1-3, Areas[SCHED3_AREA].y2+1-3, clrSched2);

    osd->DrawRectangle(Areas[TIMELINE_AREA].x1,Areas[TIMELINE_AREA].y1+YSTART,
                       Areas[TIMELINE_AREA].x1+TimelineWidth,
                       Areas[TIMELINE_AREA].y1+ScheduleHeight+YSTART,clrBlack);
    showHeads(true);

    do
    {
        for (int i=0;i<lines;)
        {
            int y=i*usef->Height();
            if (helptext[j]==NULL)
                break;
            if (helptext[j][0]=='|')
                txt=helptext[j]+1;
            else
                txt=tr(helptext[j]);

            if (i+usef->TextHeight(width,txt)>=lines)
                break;

            i+=usef->Text(Areas[area].x1+4,y+Areas[area].y1,width-8,lines-i,txt,clrWhite);
            j++;
        }
        if (area==SCHED1_AREA)
            area=SCHED2_AREA;
        else if (area==SCHED2_AREA)
            area=SCHED3_AREA;
        else if (area==SCHED3_AREA)
            break;
    }
    while (helptext[j]);
    osd->Flush();
}

void magazine::autoTimer(const class cEvent *cev)
{
    FILE *f;
    if ((f=fopen(tvonscreenCfg.vdradminfile,"a")))
    {
        const char *title;
        int chan;

        title=cev->Title();
        cChannel *cc=Channels.GetByChannelID(cev->ChannelID());
        chan=cc->Number();

        fprintf(f,"1:%s:1:::1:40:7:%d:\n",title,chan);
        fclose(f);
    }
}

void magazine::Show(void)
{
    mzlog(10," magazine::Show()");

    if (me)
    {
        delete me;
        me=NULL;
    }
    if (met)
    {
        delete met;
        met=NULL;
    }
    if (mes)
    {
        delete mes;
        mes=NULL;
    }
    GetOsdSize(Width, Height, Aspect);

    int Left=Width/20;
    int Top=Height/20;
    Width-=(2*Left);
    Height-=(2*Top);

    if (fullHours)
    {
        delete [] fullHours;
        fullHours=NULL;
    }
    if (ev1)
    {
        delete [] ev1;
        ev1=NULL;
    }
    if (ev2)
    {
        delete [] ev2;
        ev2=NULL;
    }
    if (ev3)
    {
        delete [] ev3;
        ev3=NULL;
    }

    if (f1)
    {
        delete f1;
        f1=NULL;
    }
    if (f2)
    {
        delete f2;
        f2=NULL;
    }
    if (f3)
    {
        delete f3;
        f3=NULL;
    }
    if (f4)
    {
        delete f4;
        f4=NULL;
    }
    if (f5)
    {
        delete f5;
        f5=NULL;
    }

    osd = cOsdProvider::NewOsd(Left,Top);
    if (osd)
    {
        int h1=(int) round(Height/35);
        int h2=h1-2;
        h1+=tvonscreenCfg.fontdsize;
        h2+=tvonscreenCfg.fontdsize;

        f1=new anyFont(osd,h1,1); // Sendung
        f2=new anyFont(osd,h2,1); // Extra-Info
        f3=new anyFont(osd,h1,1); // Sender
        f4=new anyFont(osd,h2);   // Tasten
        f5=new anyFont(osd,h2);   // Datum

        YSTART=2*f5->Height();
        if (YSTART<48) YSTART=48;

        struct tm tm_r1;
        localtime_r(&currentFirstTime,&tm_r1);
        char dtxt[50];
        strftime(dtxt,sizeof(dtxt),tr("%d-%m"),&tm_r1);
        TimelineWidth=f5->Width(dtxt)+10;
        if (TimelineWidth<(Width/14))
            TimelineWidth=Width/14;
        ScheduleHeight=Height-YSTART;
        ScheduleWidth=(Width-TimelineWidth)/3;

        evnum=((YSTART+ScheduleHeight-1)-YSTART)/f1->Height();
        ScheduleHeight=evnum*f1->Height();

        Areas[DATETIME_AREA].x1 = 0;
        Areas[DATETIME_AREA].x2 = TimelineWidth;
        Areas[DATETIME_AREA].y1 = 0;
        Areas[DATETIME_AREA].y2 = YSTART;

        Areas[TIMELINE_AREA].x1 = 0;
        Areas[TIMELINE_AREA].x2 = TimelineWidth;
        Areas[TIMELINE_AREA].y1 = YSTART+5;
        Areas[TIMELINE_AREA].y2 = YSTART+ScheduleHeight;

        Areas[SCHED1_AREA].x1 = 8+TimelineWidth;
        Areas[SCHED1_AREA].x2 = 8+TimelineWidth+ScheduleWidth;
        Areas[SCHED1_AREA].y1 = YSTART+5;
        Areas[SCHED1_AREA].y2 = YSTART+ScheduleHeight;

        Areas[SCHED2_AREA].x1 = 12+TimelineWidth+ScheduleWidth;
        Areas[SCHED2_AREA].x2 = 12+TimelineWidth+(2*ScheduleWidth);
        Areas[SCHED2_AREA].y1 = YSTART+5;
        Areas[SCHED2_AREA].y2 = YSTART+ScheduleHeight;

        Areas[SCHED3_AREA].x1 = 16+TimelineWidth+(2*ScheduleWidth);
        Areas[SCHED3_AREA].x2 = 16+TimelineWidth+(3*ScheduleWidth);
        Areas[SCHED3_AREA].y1 = YSTART+5;
        Areas[SCHED3_AREA].y2 = YSTART+ScheduleHeight;

        Areas[CONTROL_AREA].x1 = 12+TimelineWidth+ScheduleWidth;
        Areas[CONTROL_AREA].x2 = 12+TimelineWidth+(2*ScheduleWidth);
        Areas[CONTROL_AREA].y1 = YSTART+ScheduleHeight+5;
        Areas[CONTROL_AREA].y2 = YSTART+ScheduleHeight+5+f5->Height();

        Areas[NAME1_AREA].x1 = 8+TimelineWidth;
        Areas[NAME1_AREA].x2 = 8+TimelineWidth+ScheduleWidth;
        Areas[NAME1_AREA].y1 = 0;
        Areas[NAME1_AREA].y2 = YSTART;

        Areas[NAME2_AREA].x1 = 12+TimelineWidth+ScheduleWidth;
        Areas[NAME2_AREA].x2 = 12+TimelineWidth+(2*ScheduleWidth);
        Areas[NAME2_AREA].y1 = 0;
        Areas[NAME2_AREA].y2 = YSTART;

        Areas[NAME3_AREA].x1 = 16+TimelineWidth+(2*ScheduleWidth);
        Areas[NAME3_AREA].x2 = 16+TimelineWidth+(3*ScheduleWidth);
        Areas[NAME3_AREA].y1 = 0;
        Areas[NAME3_AREA].y2 = YSTART;

        if (osd->SetAreas(Areas, sizeof(Areas)/sizeof(tArea))) return;

        ev1=new cEvent*[evnum+1];
        ev2=new cEvent*[evnum+1];
        ev3=new cEvent*[evnum+1];

        fullHours=new int[evnum+1];
        output();
    }
}

cEvent **magazine::ev4ch(int p)
{
    cEvent **ev=NULL;
    if (p==currentFirst)
        ev=ev1;
    else if (p==currentFirst+1)
        ev=ev2;
    if (p==currentFirst+2)
        ev=ev3;
    return ev;
}

void magazine::searchcEvt(void)
{
    cEvent **ev=ev4ch(EDIT_curChannel);
    for (int i=EDIT_curEVI;i>=0;i--)
    {
        if (ev[i])
        {
            EDIT_curEvent=ev[i]->EventID();
            break;
        }
    }
}

eOSState magazine::ProcessKey(eKeys Key)
{
    //mzlog(10," ProcessKey(%d)",Key);
    eOSState state;
    if (me)
    {
        state=me->ProcessKey(Key);
        if (state==osBack)
        {
            state=osContinue;
            delete me;
            me=NULL;
            if (!osd)
                Show();
        }
        else if (state == osUnknown || state == osContinue)
        {
            switch (Key & ~k_Repeat)
            {
            case kOk:
            case kBack:
                delete me;
                me=NULL;
                if (!osd)
                    Show();
                break;
            case kBlue: // Umschalten - obsolete!
            {
                cEvent **ev=ev4ch(EDIT_curChannel);
                cChannel *channel = Channels.GetByChannelID(ev[EDIT_curEVI]->ChannelID(), true);
                if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
                {
                    delete me;
                    me=NULL;
                    return osEnd;
                }
            }
            break;
            case kGreen: // Suchen
            {
                // mzlog(10," Search");
                delete me;
                me=NULL;
                cEvent **ev=ev4ch(EDIT_curChannel);
                mes=new cSearchMenu(ev[EDIT_curEVI]);
                state=osContinue;
            }
            break;
            case kYellow: // add AutoTimer
            {
                // mzlog(10," AutoTimer");
                if (tvonscreenCfg.vdradminfile)
                {
                    me->helpLine(false);
                    state=osContinue;

                    cEvent **ev=ev4ch(EDIT_curChannel);
                    autoTimer(ev[EDIT_curEVI]);
                    me->printMsg(tr("Added AutoTimer to vdradmin."));
                }
                else
                {
                    me->printMsg(tr("Startoption 'vdradminfile' not set!"));
                    me->helpLine(false);
                    state=osContinue;
                }
            }
            break;
            case kRed:
            case kRecord: // new
            {
                delete me;
                me=NULL;
                cEvent **ev=ev4ch(EDIT_curChannel);
                cTimer *timer = new cTimer(ev[EDIT_curEVI]);
                cTimer *t = Timers.GetTimer(timer);
                if (t)
                {
                    delete timer;
                    timer = t;
                }
                met=new cMenuEditTimer(timer, !t);
                met->Display();
                state=osContinue;
            }
            break;
            }
        }
    }
    else if (met)
    {
        state=met->ProcessKey(Key);
        if (state==osBack)
        {
            state=osContinue;
            delete met;
            met=NULL;
            if (!osd)
                Show();
        }
        else if (state == osUnknown)
        {
            switch (Key & ~k_Repeat)
            {
            case kOk:
            case kBack:
                delete met;
                met=NULL;
                if (!osd)
                    Show();
                break;
            }
        }
    }
    else if (mes)
    {
        state=mes->ProcessKey(Key);
        if (state==osBack)
        {
            state=osContinue;
            delete mes;
            mes=NULL;
            if (!osd)
                Show();
        }
        else if (state == osUnknown)
        {
            switch (Key & ~k_Repeat)
            {
            case kOk:
            case kGreen:
            {
                const cEvent *ev=mes->currentSelected();
                if (ev)
                {
                    delete mes;
                    mes=NULL;
                    me=new tvOcMenuEvent(ev);
                    me->Display();
                }
            }
            state=osContinue;
            break;
            case kYellow:
                mes->searchIn(schedArray,schedArrayNum);
                state=osContinue;
                break;
            case kBack:
            case kBlue:
                delete mes;
                mes=NULL;
                if (!osd)
                    Show();
                break;
            case kRed:
                const cEvent *ev=mes->currentSelected();
                if (ev)
                {
                    delete mes;
                    mes=NULL;
                    cTimer *timer = new cTimer(ev);
                    cTimer *t = Timers.GetTimer(timer);
                    if (t)
                    {
                        delete timer;
                        timer = t;
                    }
                    met=new cMenuEditTimer(timer, !t);
                    met->Display();
                }
                state=osContinue;
                break;
            }
        }
    }
    else
    {
        state = cOsdObject::ProcessKey(Key);

        if (state == osUnknown && schedArrayNum>currentFirst)
        {
            if (curmode==SHOW)
            {
                switch (Key & ~k_Repeat)
                {
                case kOk:
                    if (schedArrayNum>currentFirst)
                    {
                        curmode=EDIT;
                        EDIT_curChannel=currentFirst;
                        for (int i=0;i<evnum;i++)
                        {
                            if (ev1[i])
                            {
                                EDIT_curEvent=ev1[i]->EventID();
                                break;
                            }
                        }
                        output();
                    }
                    break;
                case kBack:
                    return osBack;
                case kLeft:
                    currentFirst--;
                    if (currentFirst<0)
                        currentFirst=0;
                    output();
                    break;
                case kRight:
                    currentFirst++;
                    if (currentFirst>schedArrayNum-3)
                        currentFirst=schedArrayNum-3;
                    if (currentFirst<0)
                        currentFirst=0;
                    output();
                    break;
                case kUp:
                    setcurrentFirstTime(currentFirstTime-3600);
                    if (currentFirstTime<time(NULL))
                        setcurrentFirstTime(time(NULL));
                    output();
                    break;
                case kDown:
                    setcurrentFirstTime(currentFirstTime+3600);
                    output();
                    break;
                case kRed: // -1 Tag
                    setcurrentFirstTime(currentFirstTime-86400);
                    if (currentFirstTime<time(NULL))
                        setcurrentFirstTime(time(NULL));
                    output();
                    break;
                case kBlue: // +1 Tag
                    setcurrentFirstTime(currentFirstTime+86400);
                    output();
                    break;
                case k7: // -1 Seite (3 Sender)
                case kGreen:
                    currentFirst-=3;
                    if (currentFirst<0)
                        currentFirst=0;
                    output();
                    break;
                case k9: // +1 Seite (3 Sender)
                case kYellow:
                    currentFirst+=3;
                    if (currentFirst>schedArrayNum-3)
                        currentFirst=schedArrayNum-3;
                    if (currentFirst<0)
                        currentFirst=0;
                    output();
                    break;
                case k8: // zum aktuellen Sender
                {
                    int currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
                    for (int i=0;i<schedArrayNum;i++)
                    {
                        cChannel *channel = Channels.GetByChannelID(schedArray[i]->ChannelID(), true);
                        if (channel->Number()==currentChannel)
                        {
                            currentFirst=i-1;
                            if (currentFirst<0)
                                currentFirst=0;
                            output();
                            break;
                        }
                    }
                }
                break;
                case k0: // jetzt
                    setcurrentFirstTime(time(NULL));
                    output();
                    break;
                case k1:
                    showHelp();
                    break;
                case k4: // usertime1
                    gotoUsertime(tvonscreenCfg.usertime1);
                    break;
                case k5: // usertime2
                    gotoUsertime(tvonscreenCfg.usertime2);
                    break;
                case k6: // usertime3
                    gotoUsertime(tvonscreenCfg.usertime3);
                    break;
                default:
                    return state;
                }
            }
            else
            {
                switch (Key & ~k_Repeat)
                {
                case kOk:
                {
                    cEvent **ev=ev4ch(EDIT_curChannel);
                    if (ev[EDIT_curEVI] != NULL)
                    {
                        delete osd;
                        osd=NULL;
                        me=new tvOcMenuEvent(ev[EDIT_curEVI]);
                        me->Display();
                        curmode=SHOW;
                        EDIT_curEvent=0;
                        return osContinue;
                    }
                }
                break;
                case kBack:
                    curmode=SHOW;
                    EDIT_curEvent=0;
                    output();
                    break;
                case kLeft:
                    EDIT_curChannel--;
                    if (EDIT_curChannel<0)
                        EDIT_curChannel=0;
                    if (EDIT_curChannel<currentFirst)
                    {
                        currentFirst--;
                        if (currentFirst<0)
                            currentFirst=0;
                        EDIT_curChannel=currentFirst;
                    }
                    outputLR();
                    break;
                case kRight:
                    EDIT_curChannel++;
                    if (EDIT_curChannel>schedArrayNum-1)
                        EDIT_curChannel=schedArrayNum-1;
                    if (EDIT_curChannel<0)
                        EDIT_curChannel=0;
                    if (EDIT_curChannel>currentFirst+2)
                    {
                        currentFirst++;
                        if (currentFirst>schedArrayNum-3)
                            currentFirst=schedArrayNum-3;
                        if (currentFirst<0)
                            currentFirst=0;
                        EDIT_curChannel=currentFirst+2;
                    }
                    outputLR();
                    break;
                case kUp:
                {
                    cEvent **ev=ev4ch(EDIT_curChannel);
                    const cEvent *e=getPrev(schedArray[EDIT_curChannel],ev[EDIT_curEVI]);

                    if (e)
                    {
                        int found;
                        do
                        {
                            found=0;
                            for (int i=0;i<evnum;i++)
                            {
                                if (ev[i] && ev[i]->EventID()==e->EventID())
                                {
                                    found=1;
                                    EDIT_curEvent=e->EventID();
                                }
                            }
                            if (!found)
                            {
                                setcurrentFirstTime(currentFirstTime-3600);
                                if (currentFirstTime<time(NULL))
                                {
                                    setcurrentFirstTime(time(NULL));
                                    found=1;
                                }
                                calcScheds();
                            }
                        }
                        while (!found);
                    }
                    output();
                }
                break;
                case kDown:
                {
                    cEvent **ev=ev4ch(EDIT_curChannel);
                    const cEvent *e=getNext(schedArray[EDIT_curChannel],ev[EDIT_curEVI]);

                    if (e)
                    {
                        EDIT_curEvent=e->EventID();
                        int found,cc=0;
                        do
                        {
                            found=0;
                            cc++;
                            for (int i=0;i<evnum;i++)
                            {
                                if (ev[i] && ev[i]->EventID()==EDIT_curEvent)
                                {
                                    found=1;
                                }
                            }
                            if (!found)
                            {
                                setcurrentFirstTime(currentFirstTime+3600);
                                calcScheds();
                            }
                        }
                        while (!found && cc<48);
                    }
                    output();
                }
                break;
                case kRecord:
                {
                    cEvent **ev=ev4ch(EDIT_curChannel);
                    if (ev[EDIT_curEVI] != NULL)
                    {
                        delete osd;
                        osd=NULL;
                        cTimer *timer = new cTimer(ev[EDIT_curEVI]);
                        cTimer *t = Timers.GetTimer(timer);
                        if (t)
                        {
                            delete timer;
                            timer = t;
                        }
                        met=new cMenuEditTimer(timer, !t);
                    }
                }
                break;
                default:
                    return state;
                }
            }
            state = osContinue;
        }
    }
    return state;
}

#include <stdarg.h>

void mzlog(int level, const char *fmt, ...)
{
    char    t[BUFSIZ];
    va_list ap;
    int minlevel=0;

    va_start(ap, fmt);

    if (level>=minlevel)
    {
        vsnprintf(t + 10, sizeof t - 10, fmt, ap);
        memcpy(t, "tvonscreen: ", 10);
        syslog(LOG_DEBUG, "%s", t);
    }
    va_end(ap);
}
