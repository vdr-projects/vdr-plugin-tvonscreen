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

#define clrGrey       0xFF5F5F5F
#define clrBackground clrGray50 // this should be tied somehow into current theme

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

int getTimelineWidth()
{
    int Width;
    int Height;
    int tWidth;
    double Aspect;
    GetOsdSize(Width, Height, Aspect);
    tWidth = floor((((Width * 90) / 100) * 7) / 100);
    return tWidth;
}

int getScheduleWidth()
{
    int Width;
    int Height;
    int sWidth;
    double Aspect;
    GetOsdSize(Width, Height, Aspect);
    sWidth = floor((((Width * 90) / 100) * 31) / 100);
    return sWidth;
}

int getScheduleHeight()
{
    int Width;
    int Height;
    int sHeight;
    double Aspect;
    GetOsdSize(Width, Height, Aspect);
    sHeight = floor((((Height * 90) / 100) * 85) / 100);
    return sHeight;
}

int getTL_YSTART()
{
    int Width;
    int Height;
    int TL_START;
    double Aspect;
    GetOsdSize(Width, Height, Aspect);
//	TL_START = 1 + (47 * round(Height / 480));
//	TL_START = round( 48 *(( Height / 480)*( Height / 480)));
    TL_START = 37+((Height/480)*13.33333);
    return TL_START;
}

static tArea Areas[NUMBER_OF_AREAS] =
{
#ifndef MULTINAMES
    { 0,0,0,0,8 }, // NAMES_AREA
#else
    { 0,0,0,0,8 }, // NAME1_AREA
    { 0,0,0,0,8 }, // NAME2_AREA
    { 0,0,0,0,8 }, // NAME3_AREA
#endif
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

magazine::magazine(class cPlugin *p)
{
    parent=p;
    const cSchedules* Schedules = cSchedules::Schedules(_schedulesLock);

    curmode=SHOW;
    EDIT_curEvent=0;

    f1=NULL;
    f2=NULL;
    f3=NULL;
    f4=NULL;
    f5=NULL;
    me=NULL;
    met=NULL;
    mes=NULL;

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

    ev1=NULL;
    ev2=NULL;
    ev3=NULL;
    fullHours=NULL;
    fullHours_tmp1=NULL;
    fullHours_tmp2=NULL;
    fullHours_tmp3=NULL;

    currentFirstTime=time(NULL);
}
magazine::~magazine(void)
{
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

    delete [] fullHours_tmp3;
    delete [] fullHours_tmp2;
    delete [] fullHours_tmp1;
    delete [] fullHours;
    delete [] ev1;
    delete [] ev2;
    delete [] ev3;

    delete f1;
    delete f2;
    delete f3;
    delete f4;
    delete f5;

    free(schedArray);
    delete osd;
}

void magazine::printLogo(const cSchedule *s,int p)
{
    cChannel* channel;
    const char *txt;
    int x=getScheduleWidth()*p+p*4;
    int currentChannel;

#ifdef MULTINAMES
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
    x=0;
#else
    int a=NAMES_AREA;
#endif
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
        DrawXpm(fname,osd,x+Areas[a].x1,Areas[a].y1,0,tvonscreenCfg.bwlogos);
    }
}
void magazine::printHead(const cSchedule *s,int p)
{
    cChannel* channel;
    const char *txt;
    int x=getScheduleWidth()*p+p*4;
    int currentChannel;
    int wmin=0,yoff=0;

    if (tvonscreenCfg.showLogos)
        wmin=64;

    int gTL_YSTART = getTL_YSTART();

#ifdef MULTINAMES
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
    x=0;
#else
    int a=NAMES_AREA;
#endif
    if (gTL_YSTART>24)
    {

        yoff=28;
        osd->DrawRectangle(x+Areas[a].x1+getScheduleWidth()-4,0+Areas[a].y1+28,x+Areas[a].x1+
                           getScheduleWidth()-2,Areas[a].y1+28+20,clrGrey);
        // nie zu sehen, ausser wenn kein Logo gefunden wird
        osd->DrawRectangle(x+Areas[a].x1,0+Areas[a].y1+28,x+
                           Areas[a].x1+2,Areas[a].y1+28+20,clrGrey);

        if (p==2) DrawXpm(TVonscreen,osd,x+Areas[a].x1+getScheduleWidth()-112,0+Areas[a].y1);

        if (tvonscreenCfg.showChannels || !tvonscreenCfg.showLogos)
            osd->DrawRectangle(x+Areas[a].x1,yoff+Areas[a].y1,x+Areas[a].x1+getScheduleWidth()-2,
                               Areas[a].y1+gTL_YSTART-1,clrBlue);
    }
    else
    {
        osd->DrawRectangle(x+Areas[a].x1,yoff+Areas[a].y1,x+Areas[a].x1+getScheduleWidth()-2,
                           Areas[a].y1+gTL_YSTART-1,clrBlue);
    }

    if (tvonscreenCfg.showChannels || !tvonscreenCfg.showLogos)
    {
        if (s!=NULL)
        {
            tColor col=clrWhite;

            currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
            channel = Channels.GetByChannelID(s->ChannelID(), true);
            if (currentChannel==channel->Number())
            {
                col=clrCyan;
            }
            txt=channel->ShortName(true);

            if (!tvonscreenCfg.XLfonts || f3->LargeWidth(txt)>=getScheduleWidth()-wmin)
                f3->Text(wmin+x+Areas[a].x1+(getScheduleWidth()-wmin-f3->Width(txt))/2,
                         Areas[a].y1+yoff-1,txt,col,clrBlue);
            else
                f3->LargeText(wmin+x+Areas[a].x1+(getScheduleWidth()-wmin-f3->LargeWidth(txt))/2,
                              Areas[a].y1+yoff-1,txt,col,clrBlue);
        }
    }
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

    osd->DrawRectangle(Areas[CONTROL_AREA].x1, Areas[CONTROL_AREA].y1, Areas[CONTROL_AREA].x2+1,
                       Areas[CONTROL_AREA].y2+1, clrTransparent);
    if (tvonscreenCfg.noInfoLine)
        return;

    txt[0]=0;
    if (curmode==SHOW)
    {
        sprintf(txt,"%s",tr("Press 1 for help"));
    }
    f4->Text(Areas[CONTROL_AREA].x1+(8+getTimelineWidth()+(getScheduleWidth()+4)*2+
                                     getScheduleWidth()-101-f4->Width(txt))/2,
             Areas[CONTROL_AREA].y1,txt,
             clrWhite,clrBackground);
}

void magazine::showTimeline(void)
{
    int lh=-1;
    int lhc=0;
    tColor hgr[]={clrBlue,clrBlack};
    char dtxt[50];
    time_t t1;
    struct tm tm_r1;

    t1=currentFirstTime;
    localtime_r(&t1,&tm_r1);

    int gTL_YSTART = getTL_YSTART();

    osd->DrawRectangle(0+Areas[TIMELINE_AREA].x1,0+Areas[TIMELINE_AREA].y1,getTimelineWidth(),
                       getScheduleHeight()+gTL_YSTART,clrBlack);
    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height()+gTL_YSTART;
        if (lh!=fullHours[i] && fullHours[i]>=0)
        {
            lh=fullHours[i];
            lhc=lh&1;
        }
        osd->DrawRectangle(0+Areas[TIMELINE_AREA].x1,y+Areas[TIMELINE_AREA].y1,
                           getTimelineWidth(),y+f1->Height(),hgr[lhc]);
    }

    if (gTL_YSTART<=24)
    {
        strftime(dtxt,sizeof(dtxt),tr("%d-%m"),&tm_r1);
        f5->Text((getTimelineWidth()-f5->Width(dtxt))/2+Areas[TIMELINE_AREA].x1,
                 0+Areas[TIMELINE_AREA].y1,dtxt,clrWhite,clrBlack);
    }
    else
    {

        strcpy(dtxt,WeekDayName(tm_r1.tm_wday));
        osd->DrawRectangle(0+Areas[TIMELINE_AREA].x1,0+Areas[TIMELINE_AREA].y1,
                           Areas[TIMELINE_AREA].x1+getTimelineWidth(),
                           Areas[TIMELINE_AREA].y1+f5->Height()*2,clrWhite);

        osd->DrawRectangle(2+Areas[TIMELINE_AREA].x1,2+Areas[TIMELINE_AREA].y1,
                           Areas[TIMELINE_AREA].x1+getTimelineWidth()-2,
                           Areas[TIMELINE_AREA].y1+f5->Height(),clrBlack);

        f5->Text((getTimelineWidth()-f5->Width(dtxt))/2+Areas[TIMELINE_AREA].x1,
                 0+Areas[TIMELINE_AREA].y1+2,dtxt,clrWhite,clrBlack);

        strftime(dtxt,sizeof(dtxt),tr("%d-%m"),&tm_r1);

        f5->Text((getTimelineWidth()-f5->Width(dtxt))/2+Areas[TIMELINE_AREA].x1,
                 0+Areas[TIMELINE_AREA].y1+f5->Height()+1,dtxt,clrBlack,clrWhite);
    }

    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height()+gTL_YSTART;
        if (fullHours[i]>=0 && lh!=fullHours[i])
        {
            char txt[50];
            sprintf(txt,"%02d",fullHours[i]);
            lh=fullHours[i];
            f1->Text((getTimelineWidth()-f1->Width(txt))/2+Areas[TIMELINE_AREA].x1,
                     y+Areas[TIMELINE_AREA].y1,txt,clrWhite,clrBlack);
            if (i+1<evnum && (fullHours[i+1]==lh || fullHours[i+1]==-1))
            {
                strcpy(txt,tr("o'clock"));
                f2->Text((getTimelineWidth()-f2->Width(txt))/2+Areas[TIMELINE_AREA].x1,
                         y+f1->Height()+Areas[TIMELINE_AREA].y1,txt,clrWhite,clrBlack);
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

    tColor hgr[]={clrGrey,clrBackground};
    tColor col=clrWhite;

    osd->DrawRectangle(Areas[area].x1, Areas[area].y1, Areas[area].x2+1, Areas[area].y2+1, clrGray50);

    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height(); //+24;
        if (lh!=fullHours[i] && fullHours[i]>=0)
        {
            lh=fullHours[i];
            lhc=lh&1;
        }
        osd->DrawRectangle(Areas[area].x1,Areas[area].y1+y,Areas[area].x1+getScheduleWidth(),
                           Areas[area].y1+y+f1->Height(),hgr[lhc]);
    }
    for (int i=0;i<evnum;i++)
    {
        int y=i*f1->Height(); //+24;
        cev=ev[i];
        if (cev!=NULL)
        {
            if (oldev!=cev)
            {
                if (oldev)
                {
                    txt=oldev->ShortText();
                    int cc=f2->TextHeight(getScheduleWidth()-f1->Width("00:0"),txt);
                    if (cc<=i-j)
                    {
                        f2->Text(f1->Width("00:0")+Areas[area].x1,(j)*f1->Height()+Areas[area].y1,
                                 getScheduleWidth()-f1->Width("00:0"),i-j,txt,col,clrBackground);
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
                f1->Text(Areas[area].x1,y+Areas[area].y1,timetxt,col,clrBackground);
                j=i+f1->Text(f1->Width("00:00 ")+Areas[area].x1,
                             y+Areas[area].y1,getScheduleWidth()-f1->Width("00:00 "),
                             20,txt,col,clrBackground);
                oldev=cev;
            }
        }
    }
    if (oldev)
    {
        txt=oldev->ShortText();
        if (j+f2->TextHeight(f1->Width("00:00")+Areas[area].x1,txt)>=evnum)
            f2->Text(f1->Width("00:0")+Areas[area].x1,j*f1->Height()+Areas[area].y1,
                     getScheduleWidth()-f1->Width("00:0"),evnum-j,txt,col,clrBackground);
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
                               Areas[area].x1+getScheduleWidth(),Areas[area].y2+1,clrWhite);
            osd->DrawRectangle(Areas[area].x1,Areas[area].y2-f2->Height()-4,
                               Areas[area].x1+getScheduleWidth(),Areas[area].y2+1,clrYellow);
            const char *txt=tr("Timer conflict!");
            int x=(getScheduleWidth()-f2->Width(txt))/2;
            f2->Text(x+Areas[area].x1,Areas[area].y2-f2->Height()-4,txt,clrBackground,clrYellow);
        }
    }
}

void magazine::showScheds()
{
    const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
    const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
    const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

    if (s1)
    {
        showSched(ev1,SCHED1_AREA);
    }

    if (s2)
    {
        showSched(ev2,SCHED2_AREA);
    }

    if (s3)
    {
        showSched(ev3,SCHED3_AREA);
    }
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

void magazine::calcSched(const cSchedule *s,cEvent **ev)
{
    const cEvent *cev=NULL;
    const cEvent *cev2=NULL;
    int cc=0;

    for (int i=0;i<evnum;i++)
    {
        ev[i]=NULL;
        if (cev==NULL)
        {
            cev=s->GetEventAround(currentFirstTime);
            cev2=NULL;
            if (cev)
            {
                cc=f1->TextHeight(getScheduleWidth()-f1->Width("00:00 "),cev->Title());
                time_t t=cev->StartTime();
                struct tm tm_r;
                localtime_r(&t,&tm_r);
                fullHours[i]=tm_r.tm_hour; //mzlog(0," fH[%d]=%d '%s'",i,tm_r.tm_hour,cev->Title());
            }
        }
        else
        {
            if (!cev2)
                cev2=getNext(s,cev);
            if (cev2 && cc--<=1)
            {
                int z=(cev2->StartTime()-currentFirstTime)*6/60/60;
                if (z<i)
                {
                    cev=cev2;
                    cev2=NULL;

                    cc=f1->TextHeight(getScheduleWidth()-f1->Width("00:00 "),cev->Title());
                    time_t t=cev->StartTime();

                    struct tm tm_r;
                    localtime_r(&t,&tm_r);
                    fullHours[i]=tm_r.tm_hour; //mzlog(0," fH[%d]=%d '%s'",i,tm_r.tm_hour,cev->Title());
                }
            }
        }
        ev[i]=(cEvent *)cev;
    }
}

void magazine::calcScheds()
{
    const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
    const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
    const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

    for (int i=0;i<evnum;i++)
    {
        fullHours[i]=-1;
        fullHours_tmp1[i]=-1;
        fullHours_tmp2[i]=-1;
        fullHours_tmp3[i]=-1;
    }

    if (s1!=NULL)
    {
        calcSched(s1,ev1);
        for (int i=0;i<evnum;i++)
        {
            fullHours_tmp1[i]=fullHours[i];
            fullHours[i]=-1;
        }
    }

    if (s2!=NULL)
    {
        calcSched(s2,ev2);
        for (int i=0;i<evnum;i++)
        {
            fullHours_tmp2[i]=fullHours[i];
            fullHours[i]=-1;
        }
    }

    if (s3!=NULL)
    {
        calcSched(s3,ev3);
        for (int i=0;i<evnum;i++)
        {
            fullHours_tmp3[i]=fullHours[i];
            fullHours[i]=-1;
        }
    }
    int lfh1,lfh2,lfh3;
    lfh1=lfh2=lfh3=-1;
    for (int i=0;i<evnum;i++)
    {
        if (fullHours_tmp1[i]>=0)
            lfh1=fullHours_tmp1[i];
        fullHours_tmp1[i]=lfh1;
        if (fullHours_tmp2[i]>=0)
            lfh2=fullHours_tmp2[i];
        fullHours_tmp2[i]=lfh2;
        if (fullHours_tmp3[i]>=0)
            lfh3=fullHours_tmp3[i];
        fullHours_tmp3[i]=lfh3;
        if (fullHours_tmp1[i]>fullHours[i])
            fullHours[i]=fullHours_tmp1[i];
        if (fullHours_tmp2[i]>fullHours[i])
            fullHours[i]=fullHours_tmp2[i];
        if (fullHours_tmp3[i]>fullHours[i])
            fullHours[i]=fullHours_tmp3[i];
    }
}

void magazine::output(void)
{
    cBitmap *b=osd->GetBitmap(NAMES_AREA);
    if (b)
    {
        b->Reset();
        if (tvonscreenCfg.colorworkaround)
        {
            // This is an ugly work around for color problems with
            //  DVB driver or hardware or vdr handling, who knows
            b->SetColor(0,clrTransparent);
            b->SetColor(1,clrBlue);
            b->SetColor(2,clrWhite);
            b->SetColor(3,clrGrey);
            b->SetColor(4,0xffff0000);
            b->SetColor(5,clrCyan);
            for (int i=6;i<128;i++)
            {
                b->SetColor(i,0x01000000|i);
            }
            // End work around
        }
    }

    showHeads();
    showKeys();
    calcScheds();
    showScheds();
    showTimeline();

    osd->Flush();
}

void magazine::outputLR(void)
{
    cBitmap *b=osd->GetBitmap(NAMES_AREA);
    if (b)
    {
        b->Reset();
        if (tvonscreenCfg.colorworkaround)
        {
            // This is an ugly work around for color problems with
            // DVB driver or hardware or vdr handling, who knows
            b->SetColor(0,clrTransparent);
            b->SetColor(1,clrBlue);
            b->SetColor(2,clrWhite);
            b->SetColor(3,clrGrey);
            b->SetColor(4,0xffff0000);
            b->SetColor(5,clrCyan);
            for (int i=6;i<128;i++)
            {
                b->SetColor(i,0x01000000|i);
            }
            // End work around
        }
    }

    showHeads();
    calcScheds();
    searchcEvt();
    showScheds();
    showTimeline();

    osd->Flush();
}
void magazine::gotoUsertime(int u)
{
    struct tm tm_r;
    localtime_r(&currentFirstTime,&tm_r);
    tm_r.tm_hour=u/100;
    tm_r.tm_min=u%100;
    tm_r.tm_sec=0;
    currentFirstTime=mktime(&tm_r);
    if (currentFirstTime<time(NULL))
    {
        if (tvonscreenCfg.thenshownextday)
            currentFirstTime+=(60*60*24);
        else
            currentFirstTime=time(NULL);
    }
    output();
}

void magazine::showHelp()
{
    anyFont *usef=f2;
    int j=0;
    const char *txt;
    int lines=getScheduleHeight()/usef->Height();
    int width=getScheduleWidth();

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

    int gTL_YSTART = getTL_YSTART();

    osd->DrawRectangle(Areas[SCHED1_AREA].x1, Areas[SCHED1_AREA].y1+1,
                       Areas[SCHED1_AREA].x2+1, Areas[SCHED1_AREA].y2+1, clrGrey);
    osd->DrawRectangle(Areas[SCHED2_AREA].x1, Areas[SCHED2_AREA].y1+1,
                       Areas[SCHED2_AREA].x2+1, Areas[SCHED2_AREA].y2+1, clrGrey);
    osd->DrawRectangle(Areas[SCHED3_AREA].x1, Areas[SCHED3_AREA].y1+1,
                       Areas[SCHED3_AREA].x2+1, Areas[SCHED3_AREA].y2+1, clrGrey);

    osd->DrawRectangle(Areas[SCHED1_AREA].x1+2, Areas[SCHED1_AREA].y1,
                       Areas[SCHED1_AREA].x2+1-3, Areas[SCHED1_AREA].y2+1-3, clrGray50);
    osd->DrawRectangle(Areas[SCHED2_AREA].x1+2, Areas[SCHED2_AREA].y1,
                       Areas[SCHED2_AREA].x2+1-3, Areas[SCHED2_AREA].y2+1-3, clrGray50);
    osd->DrawRectangle(Areas[SCHED3_AREA].x1+2, Areas[SCHED3_AREA].y1,
                       Areas[SCHED3_AREA].x2+1-3, Areas[SCHED3_AREA].y2+1-3, clrGray50);

    osd->DrawRectangle(0+Areas[TIMELINE_AREA].x1,0+Areas[TIMELINE_AREA].y1+gTL_YSTART,
                       Areas[TIMELINE_AREA].x1+getTimelineWidth(),
                       Areas[TIMELINE_AREA].y1+getScheduleHeight()+gTL_YSTART,clrBlack);
    showHeads(true);
//	osd->Flush();

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

            i+=usef->Text(Areas[area].x1+4,y+Areas[area].y1,width-8,lines-i,txt,clrWhite,clrBackground);
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
    osd = cOsdProvider::NewOsd(((Width-((Width * 90) / 100)) / 2), ((Height-((Height * 90) / 100)) / 2));

    int gTL_YSTART = getTL_YSTART();

    Areas[TIMELINE_AREA].x1 = 0;
    Areas[TIMELINE_AREA].x2 = (2+getTimelineWidth());
    Areas[TIMELINE_AREA].y1 = 0;
    Areas[TIMELINE_AREA].y2 = (gTL_YSTART+getScheduleHeight()-1);

    Areas[SCHED1_AREA].x1 = (8+getTimelineWidth());
    Areas[SCHED1_AREA].x2 = (7+getTimelineWidth()+getScheduleWidth());
    Areas[SCHED1_AREA].y1 = gTL_YSTART;
    Areas[SCHED1_AREA].y2 = (gTL_YSTART+getScheduleHeight()-1);

    Areas[SCHED2_AREA].x1 = (12+getTimelineWidth()+getScheduleWidth());
    Areas[SCHED2_AREA].x2 = (11+getTimelineWidth()+(2*getScheduleWidth()));
    Areas[SCHED2_AREA].y1 = gTL_YSTART;
    Areas[SCHED2_AREA].y2 = (gTL_YSTART+getScheduleHeight()-1);

    Areas[SCHED3_AREA].x1 = (16+getTimelineWidth()+(2*getScheduleWidth()));
    Areas[SCHED3_AREA].x2 = (15+getTimelineWidth()+(3*getScheduleWidth()));
    Areas[SCHED3_AREA].y1 = gTL_YSTART;
    Areas[SCHED3_AREA].y2 = (gTL_YSTART+getScheduleHeight()-1);

    Areas[CONTROL_AREA].x1 = 50;
    Areas[CONTROL_AREA].x2 = (getTimelineWidth()+(2*(4+getScheduleWidth()))+getScheduleWidth()-43);
    Areas[CONTROL_AREA].y1 = (8+gTL_YSTART+getScheduleHeight());
    Areas[CONTROL_AREA].y2 = (27+gTL_YSTART+getScheduleHeight()-1);

#ifndef MULTINAMES
    Areas[NAMES_AREA].x1 = (8+getTimelineWidth());
    Areas[NAMES_AREA].x2 = (7+getTimelineWidth()+((4+getScheduleWidth())*2)+getScheduleWidth());
    Areas[NAMES_AREA].y1 = 0;
    Areas[NAMES_AREA].y2 = (gTL_YSTART-1);
#else
    Areas[NAME1_AREA].x1 = (8+getTimelineWidth());
    Areas[NAME1_AREA].x2 = (7+getTimelineWidth()+getScheduleWidth());
    Areas[NAME1_AREA].y1 = 0;
    Areas[NAME1_AREA].y2 = (gTL_YSTART-1);

    Areas[NAME2_AREA].x1 = (12+getTimelineWidth()+getScheduleWidth());
    Areas[NAME2_AREA].x2 = (11+getTimelineWidth()+(2*getScheduleWidth()));
    Areas[NAME2_AREA].y1 = 0;
    Areas[NAME2_AREA].y2 = (gTL_YSTART-1);

    Areas[NAME3_AREA].x1 = (8+getTimelineWidth()+(2*(getScheduleWidth()+4)));
    Areas[NAME3_AREA].x2 = (7+getTimelineWidth()+((4+getScheduleWidth())*2)+getScheduleWidth());
    Areas[NAME3_AREA].y1 = 0;
    Areas[NAME3_AREA].y2 = (gTL_YSTART-1);
#endif

    if (osd && !osd->SetAreas(Areas, sizeof(Areas)/sizeof(tArea)))
    {
        delete [] fullHours;
        delete [] ev1;
        delete [] ev2;
        delete [] ev3;

        delete f1;
        delete f2;
        delete f3;
        delete f4;
        delete f5;

        f1=new anyFont(osd,(round(12*Width) / Height),1);	// Sendung
        f2=new anyFont(osd,(round(11*Width) / Height),1);	// Extra-Info
        f3=new anyFont(osd,(round(12*Width) / Height),1);	// Sender
        f4=new anyFont(osd,(round(11*Width) / Height));	// Tasten
        f5=new anyFont(osd,(round(11*Width) / Height));	// Datum

        for (int i=0; i < (int)(sizeof(Areas)/sizeof(tArea)); i++)
        {
            osd->DrawRectangle(Areas[i].x1, Areas[i].y1, Areas[i].x2+1, Areas[i].y2+1, clrGray50);
        }
        evnum=(Areas[SCHED1_AREA].y2-Areas[SCHED1_AREA].y1)/f1->Height();

        ev1=new cEvent*[evnum];
        ev2=new cEvent*[evnum];
        ev3=new cEvent*[evnum];

        fullHours=new int[evnum];
        fullHours_tmp1=new int[evnum];
        fullHours_tmp2=new int[evnum];
        fullHours_tmp3=new int[evnum];

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
//						mzlog(10," Search");
                delete me;
                me=NULL;
                cEvent **ev=ev4ch(EDIT_curChannel);
                mes=new cSearchMenu(ev[EDIT_curEVI]);
                state=osContinue;
            }
            break;
            case kYellow: // add AutoTimer
            {
//						mzlog(10," AutoTimer");
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
                    currentFirstTime-=60*30;
                    if (currentFirstTime<time(NULL))
                        currentFirstTime=time(NULL);
                    output();
                    break;
                case kDown:
                    currentFirstTime+=60*30;
                    output();
                    break;
                case kRed: // -1 Tag
                    currentFirstTime-=24*60*60;
                    if (currentFirstTime<time(NULL))
                        currentFirstTime=time(NULL);
                    output();
                    break;
                case kBlue: // +1 Tag
                    currentFirstTime+=24*60*60;
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
                    currentFirstTime=time(NULL);
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
                                currentFirstTime-=60*30;
                                if (currentFirstTime<time(NULL))
                                {
                                    currentFirstTime=time(NULL);
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
                                currentFirstTime+=60*30;
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
