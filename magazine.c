/*
 * magazine.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: magazine.c,v 1.7 2004/03/05 12:16:26 schmitzj Exp $
 *
 */

#include "magazine.h"

static int CompareSchedules(const void *p1, const void *p2) {

  int c1nr = 9999; // there should be no one with more than 9999 channels
  int c2nr = 9999;
  
  cChannel* c1 = Channels.GetByChannelID((*(const cSchedule**)p1)->GetChannelID(), true);
  if (c1)
    c1nr = c1->Number();

  cChannel* c2 = Channels.GetByChannelID((*(const cSchedule**)p2)->GetChannelID(), true);
  if (c2)
    c2nr = c2->Number();

  return c1nr - c2nr; 
}


magazine::magazine(class cPlugin *p)
{
	parent=p;
	
	const cSchedules* Schedules = cSIProcessor::Schedules(_mutexLock);

	curmode=SHOW;
	EDIT_curEvent=0;

	f1=NULL;
	f2=NULL;
	f3=NULL;
	f4=NULL;
	me=NULL;
	met=NULL;
	
	// Create and sort entries for menu items
	// --------------------------------------

	const cSchedule* Schedule = Schedules->First();

	int num = 0;
	schedArray = NULL;
	while (Schedule) 
	{
		schedArray = (const cSchedule **)realloc(schedArray, (num + 1) * sizeof(cSchedule*));

		if (Schedule->GetPresentEvent()) 
		{ // if there exist an event -> add to array
			// check if we can get a channel NAME ( XXX )
			// I thougt that this must always work , but doesn't 
			cChannel *c = Channels.GetByChannelID(Schedule->GetChannelID(), true);
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

	int currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
	for(int i=0;i<schedArrayNum;i++)
	{
		cChannel *channel = Channels.GetByChannelID(schedArray[i]->GetChannelID(), true);
		if (channel->Number()==currentChannel)
		{
			currentFirst=i-1;
			if (currentFirst<0)
				currentFirst=0;
			break;
		}
	}

	ev1=NULL;
	ev2=NULL;
	ev3=NULL;
	fullHours=NULL;

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

	delete [] fullHours;
	delete [] ev1;
	delete [] ev2;
	delete [] ev3;

	delete f1;
	delete f2;
	delete f3;
	delete f4;
	
	free(schedArray);
	delete osd;
}
/* void magazine::printHead(const cSchedule *s,tWindowHandle sched)
{
	cChannel* channel;
	const char *txt;

	osd->Clear(sched);
	osd->Fill(0,0,184,20,clrBlue,sched);
	if (s!=NULL)
	{
		channel = Channels.GetByChannelID(s->GetChannelID(), true);
		txt=channel->Name();
		if (f3->LargeWidth(txt)>184)
			f3->Text((184-f3->Width(txt))/2,0,txt,clrWhite,clrBlue,sched);
		else
			f3->LargeText((184-f3->LargeWidth(txt))/2,0,txt,clrWhite,clrBlue,sched);
	}
	osd->Fill(0,21,184,23,clrTransparent,sched);
} */
void magazine::printHead(const cSchedule *s,int p)
{
	cChannel* channel;
	const char *txt;
	int x=184*p+p*4;
	int currentChannel;
	
	osd->Fill(x,0,x+188,20,clrBlue,names);
	if (s!=NULL)
	{
		enum eDvbColor col=clrWhite;
		
		currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
		channel = Channels.GetByChannelID(s->GetChannelID(), true);
		if (currentChannel==channel->Number())
		{
			col=clrCyan;
		}
		txt=channel->Name();
		if (f3->LargeWidth(txt)>184)
			f3->Text(x+(184-f3->Width(txt))/2,-1,txt,col,clrBlue,names);
		else
			f3->LargeText(x+(184-f3->LargeWidth(txt))/2,-1,txt,col,clrBlue,names);
	}
}
void magazine::showHeads()
{
	const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
	const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
	const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

	printHead(s1,0);
	printHead(s2,1);
	printHead(s3,2);
}
void magazine::showKeys(void)
{
	char txt[100];
	osd->Clear(control);

	if (curmode==SHOW)
	{
		sprintf(txt,"%s",tr("Keys: Up/Down/Left/Right/Red/Blue/Green/Yellow/Ok/Back/7/8/9/0"));
	}
	else if (curmode==EDIT)
	{
		sprintf(txt,"%s",tr("Keys: Up/Down/Ok/Back/Rec"));
	}
	f4->Text((8+45+188*2+184-100-f4->Width(txt))/2,-1,txt,clrWhite,clrBackground,control);
}
void magazine::showTimeline(void)
{
	int lh=-1;
	int lhc=0;
	enum eDvbColor hgr[]={clrBlue,clrBlack};
	char dtxt[50];
	time_t t1;
	struct tm tm_r1;

	t1=currentFirstTime;
	localtime_r(&t1,&tm_r1);

	osd->Fill(0,0,48,424,clrBlack,timeline);
	for(int i=0;i<evnum;i++)
	{
		int y=i*f1->Height()+24;
		if (lh!=fullHours[i] && fullHours[i]>=0)
		{
			lh=fullHours[i];
			lhc=lh&1;
		}
		osd->Fill(0,y,48,y+f1->Height(),hgr[lhc],timeline);
	}

	strftime(dtxt,sizeof(dtxt),tr("%d-%m"),&tm_r1);
	f3->Text((48-f3->Width(dtxt))/2,0,dtxt,clrWhite,clrBlack,timeline);

	for(int i=0;i<evnum;i++)
	{
		int y=i*f1->Height()+24;
		if (fullHours[i]>=0 && lh!=fullHours[i])
		{
			char txt[50];
			sprintf(txt,"%02d",fullHours[i]);
			lh=fullHours[i];
			f1->Text((48-f1->Width(txt))/2,y,txt,clrWhite,clrBlack,timeline);
			if (i+1<evnum && (fullHours[i+1]==lh || fullHours[i+1]==-1))
			{
				strcpy(txt,tr("o'clock"));
				f2->Text((48-f2->Width(txt))/2,y+f1->Height(),txt,clrWhite,clrBlack,timeline);
			}
		}
	}
	osd->Flush();
}
void magazine::showSched(const cSchedule *s,cEventInfo **ev,tWindowHandle sched)
{
	cEventInfo *oldev=NULL,*cev=NULL;
	int j=0;
	const char *txt;
	int lh=-1;
	int lhc=0;
	enum eDvbColor hgr[]={clrGrey,clrBackground};
	enum eDvbColor col=clrWhite;

	osd->Clear(sched);

	for(int i=0;i<evnum;i++)
	{
		int y=i*f1->Height(); //+24;
		if (lh!=fullHours[i] && fullHours[i]>=0)
		{
			lh=fullHours[i];
			lhc=lh&1;
		}
		osd->Fill(0,y,184,y+f1->Height(),hgr[lhc],sched);
	}
	for(int i=0;i<evnum;i++)
	{
		int y=i*f1->Height(); //+24;
		cev=ev[i];
		if (cev!=NULL)
		{
			if (oldev!=cev)
			{
				if (oldev)
				{
					txt=oldev->GetSubtitle();
					int cc=f2->TextHeight(184-f1->Width("00:0"),txt);
					if (cc<=i-j)
						f2->Text(f1->Width("00:0"),(j)*f1->Height()/*+24*/,184-f1->Width("00:0"),i-j,txt,col,clrBackground,sched);
				}
				col=clrWhite;
				if (EDIT_curEvent==cev->GetEventID())
				{
					col=clrYellow;
					EDIT_curEVI=i;
				}
				txt=cev->GetTimeString();
				f1->Text(0,y,txt,col,clrBackground,sched);
				txt=cev->GetTitle();
				j=i+f1->Text(f1->Width("00:00 "),y,184-f1->Width("00:00 "),20,txt,col,clrBackground,sched);
				oldev=cev;
			}
		}
	}
	if (oldev)
	{
		txt=oldev->GetSubtitle();
		f2->Text(f1->Width("00:0"),j*f1->Height()/*+24*/,184-f1->Width("00:0"),evnum-j,txt,col,clrBackground,sched);
	}
}
void magazine::showScheds()
{
	const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
	const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
	const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

	if (s1!=NULL)
	{
		showSched(s1,ev1,sched1);
	}

	if (s2!=NULL)
	{
		showSched(s2,ev2,sched2);
	}

	if (s3!=NULL)
	{
		showSched(s3,ev3,sched3);
	}
}
const cEventInfo *magazine::getNext(const cSchedule *s,const cEventInfo *e)
{
	const cEventInfo *pe = NULL;
	time_t ref = e->GetTime();
	time_t delta = INT_MAX;
	for (int i=0;i<s->NumEvents();i++)
	{
		const cEventInfo *p = s->GetEventNumber(i);
		if (p!=e)
		{
			time_t dt = p->GetTime() - ref;
			if (dt > 0 && dt < delta)
			{
				delta = dt;
				pe = p;
			}
		}
	}
	return pe;
}
const cEventInfo *magazine::getPrev(const cSchedule *s,const cEventInfo *e)
{
	const cEventInfo *pe = NULL;
	time_t ref = e->GetTime();
	time_t delta = INT_MAX;
	for (int i=0;i<s->NumEvents();i++)
	{
		const cEventInfo *p = s->GetEventNumber(i);
		if (p!=e)
		{
			time_t dt = ref - p->GetTime();
			if (dt > 0 && dt < delta)
			{
				delta = dt;
				pe = p;
			}
		}
	}
	return pe;
}
void magazine::calcSched(const cSchedule *s,cEventInfo **ev)
{
	const cEventInfo *cev=NULL;
	const cEventInfo *cev2=NULL;
	int cc=0;

	for(int i=0;i<evnum;i++)
	{
		ev[i]=NULL;
		if (cev==NULL)
		{
			cev=s->GetEventAround(currentFirstTime);
			cev2=NULL;
			if (cev)
			{
				cc=f1->TextHeight(184-f1->Width("00:00 "),cev->GetTitle());
				time_t t=cev->GetTime();
				struct tm tm_r;
				localtime_r(&t,&tm_r);
				fullHours[i]=tm_r.tm_hour; //mzlog(0," fH[%d]=%d '%s'",i,tm_r.tm_hour,cev->GetTitle());
			}
		}
		else
		{
			if (!cev2)
				cev2=getNext(s,cev);
			if (cev2 && cc--<=1)
			{
				int z=(cev2->GetTime()-currentFirstTime)*6/60/60;
				if (z<i)
				{
					cev=cev2;
					cev2=NULL;
					cc=f1->TextHeight(184-f1->Width("00:00 "),cev->GetTitle());
					time_t t=cev->GetTime();
					struct tm tm_r;
					localtime_r(&t,&tm_r);
					fullHours[i]=tm_r.tm_hour; //mzlog(0," fH[%d]=%d '%s'",i,tm_r.tm_hour,cev->GetTitle());
				}
			}
		}
		ev[i]=(cEventInfo *)cev;
	}
}
void magazine::calcScheds()
{
	const cSchedule *s1=schedArrayNum>currentFirst?schedArray[currentFirst]:NULL;
	const cSchedule *s2=schedArrayNum>currentFirst+1?schedArray[currentFirst+1]:NULL;
	const cSchedule *s3=schedArrayNum>currentFirst+2?schedArray[currentFirst+2]:NULL;

	for(int i=0;i<evnum;i++)
	{
		fullHours[i]=-1;
	}

	if (s1!=NULL)
	{
		calcSched(s1,ev1);
	}

	if (s2!=NULL)
	{
		calcSched(s2,ev2);
	}

	if (s3!=NULL)
	{
		calcSched(s3,ev3);
	}
}

#include "fontosd/fontosd-arial18.c"
#include "fontosd/fontosd-verdana16.c"
#include "fontosd/fontosd-tahoma16.c"
#include "fontosd/fontosd-arial16.c"

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

	osd = cOsd::OpenRaw(50, 60);
  	if (osd)
	{
		delete [] fullHours;
		delete [] ev1;
		delete [] ev2;
		delete [] ev3;

		delete f1;
		delete f2;
		delete f3;
		delete f4;

		f1=new anyFont(osd,(cFont::tPixelData *)fontosd_arial18,FONTOSD_ARIAL18,1);		// Sendung
		f2=new anyFont(osd,(cFont::tPixelData *)fontosd_verdana16,FONTOSD_VERDANA16,1);	// Extra-Info
		f3=new anyFont(osd,(cFont::tPixelData *)fontosd_tahoma16,FONTOSD_TAHOMA16);		// Sender
		f4=new anyFont(osd,(cFont::tPixelData *)fontosd_arial16,FONTOSD_ARIAL16);		// Tasten
		timeline=osd->Create(0, 0, 8+40, 384+24, 2, false, false);
		names=osd->Create(8+45, 0, 188*2+184, 20, 2, false, false);
		sched1=osd->Create(8+45, 24, 184, 384, 2, true, false);
		sched2=osd->Create(8+45+188, 24, 184, 384, 2, true, false);
		sched3=osd->Create(8+45+188*2, 24, 184, 384, 2, true, false);
		control=osd->Create(50, 384+24+8, 8+45+188*2+184-100, 20, 1, true, false);

		osd->AddColor(clrBackground,sched1);
		osd->AddColor(clrBackground,sched2);
		osd->AddColor(clrBackground,sched3);
		osd->AddColor(clrWhite,timeline);
		osd->AddColor(clrWhite,sched1);
		osd->AddColor(clrWhite,sched2);
		osd->AddColor(clrWhite,sched3);
		osd->AddColor(clrWhite,names);
		osd->AddColor(clrBlack,timeline);
		osd->AddColor(clrBlue,names);
		osd->AddColor(clrYellow,names);
		osd->AddColor(clrGrey,sched1);
		osd->AddColor(clrGrey,sched2);
		osd->AddColor(clrGrey,sched3);
		osd->Clear();

		evnum=(384)/f1->Height();
		ev1=new (cEventInfo*)[evnum];
		ev2=new (cEventInfo*)[evnum];
		ev3=new (cEventInfo*)[evnum];
		fullHours=new int[evnum];

		showHeads();
		osd->Flush();

		showKeys();
		calcScheds();
		showScheds();
		showTimeline();
	}
}
cEventInfo **magazine::ev4ch(int p)
{
	cEventInfo **ev=NULL;
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
	cEventInfo **ev=ev4ch(EDIT_curChannel);
	for(int i=EDIT_curEVI;i>=0;i--)
	{
		if (ev[i])
		{
			EDIT_curEvent=ev[i]->GetEventID();
			break;
		}
	}
}
eOSState magazine::ProcessKey(eKeys Key)
{
	mzlog(10," ProcessKey(%d)",Key);
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
		else if (state == osUnknown)
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
				case kBlue: // Umschalten
					{
						cEventInfo **ev=ev4ch(EDIT_curChannel);
						cChannel *channel = Channels.GetByChannelID(ev[EDIT_curEVI]->GetChannelID(), true);
    					if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
						{
							delete me;
							me=NULL;
					        return osEnd;
						}
					}
					break;
				case kRed:
				case kRecord: // new 
					{
						delete me;
						me=NULL;
						cEventInfo **ev=ev4ch(EDIT_curChannel);
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
	else
	{
		state = cOsdObject::ProcessKey(Key);
	
		if (state == osUnknown)
		{
			if (curmode==SHOW && schedArrayNum>currentFirst)
			{
				switch (Key & ~k_Repeat)
				{
					case kOk:
						if (schedArrayNum>currentFirst)
						{
							curmode=EDIT;
							EDIT_curChannel=currentFirst;
							for(int i=0;i<evnum;i++)
							{
								if (ev1[i])
								{
									EDIT_curEvent=ev1[i]->GetEventID();
									break;
								}
							}
							showKeys();
							showHeads();
							calcScheds();
							showScheds();
							showTimeline();
						}
						break;
            		case kBack:
						return osBack;
					case kLeft:
						currentFirst--;
						if (currentFirst<0)
							currentFirst=0;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case kRight:
						currentFirst++;
						if (currentFirst>schedArrayNum-2)
							currentFirst=schedArrayNum-2;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case kUp:
						currentFirstTime-=60*30;
						if (currentFirstTime<time(NULL))
							currentFirstTime=time(NULL);
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case kDown:
						currentFirstTime+=60*30;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break; 
					case kRed: // -1 Tag
						currentFirstTime-=24*60*60;
						if (currentFirstTime<time(NULL))
							currentFirstTime=time(NULL);
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case kBlue: // +1 Tag
						currentFirstTime+=24*60*60;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case k7: // -1 Seite (3 Sender)
					case kGreen:
						currentFirst-=3;
						if (currentFirst<0)
							currentFirst=0;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;			
					case k9: // +1 Seite (3 Sender)
					case kYellow:
						currentFirst+=3;
						if (currentFirst>schedArrayNum-2)
							currentFirst=schedArrayNum-2;
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
					case k8: // zum aktuellen Sender
						{
							int currentChannel = Channels.GetByNumber(cDevice::CurrentChannel())->Number();
							for(int i=0;i<schedArrayNum;i++)
							{
								cChannel *channel = Channels.GetByChannelID(schedArray[i]->GetChannelID(), true);
								if (channel->Number()==currentChannel)
								{
									currentFirst=i-1;
									if (currentFirst<0)
										currentFirst=0;
									showHeads();
									calcScheds();
									showScheds();
									showTimeline();
									break;
								}
							}
						}
						break;
					case k0: // jetzt
						currentFirstTime=time(NULL);
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
						break;
		/*			case k0:
						break;			
					case k1:
						break;			
					case k2:
						break;			
					case k3:
						break;			
					case k4:
						break;			
					case k5:
						break;			
					case k6:
						break;			
					case k8:
						break;			*/
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
							delete osd;
							osd=NULL;
							cEventInfo **ev=ev4ch(EDIT_curChannel);
							me=new cMenuEvent(ev[EDIT_curEVI], true);
							me->Display();
							curmode=SHOW;
							EDIT_curEvent=0;
							return osContinue;
						}
						break;
            		case kBack:
						curmode=SHOW;
						EDIT_curEvent=0;
						showKeys();
						showHeads();
						calcScheds();
						showScheds();
						showTimeline();
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
						showHeads();
						calcScheds();
						searchcEvt();
						showScheds();
						showTimeline();
						break;
					case kRight:
							EDIT_curChannel++;
						if (EDIT_curChannel>schedArrayNum-1)
							EDIT_curChannel=schedArrayNum-1;
						if (EDIT_curChannel>currentFirst+2)
						{
							currentFirst++;
							if (currentFirst>schedArrayNum-3)
								currentFirst=schedArrayNum-3;
							EDIT_curChannel=currentFirst+2;
						}
						showHeads();
						calcScheds();
						searchcEvt();
						showScheds();
						showTimeline();
						break;
					case kUp:
						{
							cEventInfo **ev=ev4ch(EDIT_curChannel);
							const cEventInfo *e=getPrev(schedArray[EDIT_curChannel],ev[EDIT_curEVI]);

							if (e)
							{
								int found;
								do
								{
									found=0;
									for(int i=0;i<evnum;i++)
									{
										if (ev[i] && ev[i]->GetEventID()==e->GetEventID())
										{
											found=1;
											EDIT_curEvent=e->GetEventID();
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
							showHeads();
							calcScheds();
							showScheds();
							showTimeline();
						}
						break; 
					case kDown:
						{
							cEventInfo **ev=ev4ch(EDIT_curChannel);
							const cEventInfo *e=getNext(schedArray[EDIT_curChannel],ev[EDIT_curEVI]);

							if (e)
							{
								EDIT_curEvent=e->GetEventID();
								int found,cc=0;
								do
								{
									found=0;
									cc++;
									for(int i=0;i<evnum;i++)
									{
										if (ev[i] && ev[i]->GetEventID()==EDIT_curEvent)
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
							showHeads();
							calcScheds();
							showScheds();
							showTimeline();
						}
						break; 
/*					case kRed:
						break;
					case kBlue:
						break;
					case kGreen:
						break;			
					case kYellow:
						break;			*/
		/*			case k0:
						break;			
					case k1:
						break;			
					case k2:
						break;			
					case k3:
						break;			
					case k4:
						break;			
					case k5:
						break;			
					case k6:
						break;			
					case k7:
						break;
					case k8:
						break;						
					case k9:
						break;			*/
				case kRecord:
					{
						delete osd;
						osd=NULL;
						cEventInfo **ev=ev4ch(EDIT_curChannel);
						cTimer *timer = new cTimer(ev[EDIT_curEVI]);
						cTimer *t = Timers.GetTimer(timer);
						if (t)
						{
							delete timer;
							timer = t;
						}
						met=new cMenuEditTimer(timer, !t);
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
	auto char    t[BUFSIZ];
	auto va_list ap;
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
