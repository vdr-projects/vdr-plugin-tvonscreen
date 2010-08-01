/*
 * magazine.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: magazine.h,v 1.16 2006/06/18 13:59:36 schmitzj Exp $
 *
 */

#ifndef MAGAZINE_H
#define MAGAZINE_H

#include <vdr/plugin.h>
#include <time.h>
#include <vdr/device.h>
#include "gfxtools.h"
#include "config.h"
#include "timer.h"
#include "search.h"

void mzlog(int level, const char *fmt, ...);
#define TL_YSTART 	48
// else
//#define TL_YSTART	24

// #define MULTINAMES

typedef enum
{
#ifdef MULTINAMES
    NAME1_AREA = 0,
    NAME2_AREA,
    NAME3_AREA,
#else
    NAMES_AREA = 0,
#endif
    TIMELINE_AREA,
    SCHED1_AREA,
    SCHED2_AREA,
    SCHED3_AREA,
    CONTROL_AREA,
    NUMBER_OF_AREAS,
} tMagazineArea;

class magazine : public cOsdObject
{
    cPlugin *parent;
    cOsd *osd;

    class tvOcMenuEvent *me;
    class cMenuEditTimer *met;
    class cSearchMenu *mes;

    anyFont *f1,*f2,*f3,*f4;

    cSchedulesLock _schedulesLock;
    const cSchedule** schedArray;
    int schedArrayNum;
    int currentFirst;
    time_t currentFirstTime;

    int evnum;
    class cEvent **ev1;
    class cEvent **ev2;
    class cEvent **ev3;
    int *fullHours;
    int *fullHours_tmp1;
    int *fullHours_tmp2;
    int *fullHours_tmp3;

    enum modes {SHOW,EDIT};
    enum modes curmode;

    int EDIT_curEvent;
    int EDIT_curChannel;
    int EDIT_curEVI;

    bool timeline_tested;
    bool timeline_found_conflict;

public:
    static const class cEvent *getNext(const cSchedule *s,const cEvent *e);
    static const class cEvent *getPrev(const cSchedule *s,const cEvent *e);
private:
    cEvent **ev4ch(int);
    void searchcEvt();

    void printLogo(const cSchedule *s,int p);
    void printHead(const cSchedule *s,int p);
    void showKeys(void);
    void showTimeline(void);
    void showHeads(bool onlyBG=false);
    void showScheds(void);
    void showSched(const cSchedule *s,cEvent **ev, tMagazineArea area);
    void calcSched(const cSchedule *s,cEvent **ev);
    void calcScheds(void);

    void output(void);
    void outputLR(void);

    void gotoUsertime(int u);
    void showHelp(void);

    void autoTimer(const class cEvent *EventInfo);

public:
    magazine(class cPlugin *);
    virtual ~magazine();
    virtual void Show(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif
