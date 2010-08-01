/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c,v 1.15 2004/11/30 20:08:24 schmitzj Exp $
 *
 */

#include "i18n.h"

const tI18nPhrase tvoPhrases[] =
{
    { "TV-OnScreen", // English
        "TV-OnScreen", // Deutsch
        "", // Slovenski
        "", // Italiano
        "", // Nederlands
        "", // Portugu�s
        "", // Fran�ais
        "", // Norsk
        "TV-ohjelmat", // suomi
        "", // Polski
        "", // Espa�ol
        "", // ��������
        "", // Svenska
        "", // Romaneste
        "", // Magyar
        "", // Catal�
#if VDRVERSNUM >= 10302
        ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
        "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
        "", // Eesti
#if VDRVERSNUM >= 10316
        "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Shows the EPG info in form of a typical TV magazine", // English
      "", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "TV-ohjelmatiedot (OSD)", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "o'clock", // English
      "Uhr", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      " ", // suomi (empty space)
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "%d-%m", // English
      "%d.%m.", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "%d.%m.", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Press 1 for help", // English
      "F�r Hilfe die 1 dr�cken", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Paina '1' n�hd�ksesi opasteen", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Timer conflict!", // English
      "Timer Konflikt!", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      " ", // suomi (empty space)
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },

// Prefs
    { "show channel logos", // English
      "zeige Kanal-Logos", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "N�yt� kanavien logot", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "use XL fonts", // English
      "benutze XL Schrift", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "K�yt� XL-kirjasinta", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "hide info line", // English
      "verstecke Info Zeile", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Piilota inforivi", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "show channel names", // English
      "zeige Kanal-Namen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "N�yt� kanavien nimet", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "show logos in black&white", // English
      "zeige Kanal-Logos in Schwarz/Wei�", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "N�yt� logot mustavalkoisina", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "enable color problem work around", // English
      "Farbproblem Umgehung aktivieren", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Saksalainen v�rikorjaus", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "user point in time 1 (Key 4)", // English
      "Anwenderzeitpunkt 1 (Taste 4)", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Ajankohta #1 (n�pp�in '4')", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "user point in time 2 (Key 5)", // English
      "Anwenderzeitpunkt 2 (Taste 5)", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Ajankohta #2 (n�pp�in '5')", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "user point in time 3 (Key 6)", // English
      "Anwenderzeitpunkt 3 (Taste 6)", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Ajankohta #3 (n�pp�in '6')", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "jump to next day point if ago", // English
      "Gehe zum n�chsten Tag wenn vorbei", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Siirry tarvittaessa seur. p�iv��n", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Show search item itself", // English
      "Zeige auch Suchbegriff", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "N�yt� hakuehto", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },

// Help
    { "NORMAL MODE:", // English
      "NORMALER MODUS:", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "NORMAALITILA:\n", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "arrows\n\tmove view", // English
      "Pfeile\n\tAnsicht bewegen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Nuolin�pp�imet'\n\tohjaa n�kym��", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "back\n\tclose TV OnScreen", // English
      "back\n\tschlie�e TV OnScreen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Takaisin'\n\tsulje laajennos", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "red/blue\n\t-/+ one day", // English
      "rot/blau\n\t-/+ ein Tag", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Punainen'/'Sininen'\n\tyksi p�iv� eteen/taaksep�in", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "green/yellow\n\tone page left/right", // English
      "gr�n/gelb\n\teine Seite links/rechts", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Vihre�'/'Keltainen'\n\tseuraava/edellinen sivu", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "7/9\n\tone page left/right", // English
      "7/9\n\teine Seite links/rechts", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'7'/'9'\n\tseuraava/edellinen sivu", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "8\n\tgoto current channel", // English
      "8\n\tgehe zum aktuellen Kanal", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'8'\n\tsiirry nykyiselle kanavalle", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "0\n\tgoto now", // English
      "0\n\tgehe zur aktuellen Zeit", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'0'\n\tsiirry nykyhetkeen", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "4/5/6\n\tgoto configured time", // English
      "4/5/6\n\tgehe zu eingestellten Zeitpunkten", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'4'/'5'/'6'\n\tsiirry m��riteltyyn ajankohtaan", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "ok\n\tswitch to edit mode\n", // English
      "ok\n\taktiviere Editier-Modus\n", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'OK'\n\tvaihda muokkaustilaan\n", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "EDIT MODE:", // English
      "EDITIER MODUS:", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "MUOKKAUSTILA:\n", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "back\n\tback to normal mode", // English
      "back\n\tzur�ck zum normalen Modus", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Takaisin'\n\tvaihda normaalitilaan", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "arrows\n\tmove selected schedule", // English
      "Pfeile\n\tSendung ausw�hlen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Nuolin�pp�imet'\n\tvalitse ohjelma", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "record\n\tcreate timer", // English
      "record\n\terzeuge Timer", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'Tallenna'\n\tluo ajastin", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "ok\n\tshow details", // English
      "ok\n\tzeige Sendungsdetails", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "'OK'\n\tn�yt� lis�tiedot", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
// End Help

// Details
    { "Record", // English
      "Aufnehmen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Tallenna", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Search", // English
      "Suchen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Etsi", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "AutoTimer", // English
      "AutoTimer", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "AutoTimer", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Added AutoTimer to vdradmin.", // English
      "AutoTimer in vdradmin aufgenommen.", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "AutoTimer lis�tty VDRAdminiin.", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Startoption 'vdradminfile' not set!", // English
      "Startoption 'vdradminfile' nicht gesetzt!", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "K�ynnistysoptio 'vdradminfile' puuttuu!", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },

// Search
    { "Search for", // English
      "Suche nach", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Etsit��n termi�", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Searching...", // English
      "Suchen...", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Etsit��n...", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Nothing found!", // English
      "Nichts gefunden!", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "", // suomi
      "Ei l�ydetty!", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Back", // English
      "Zur�ck", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Takaisin", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "Details", // English
      "Details", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Lis�tiedot", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },
    { "in all", // English
      "in allen", // Deutsch
      "", // Slovenski
      "", // Italiano
      "", // Nederlands
      "", // Portugu�s
      "", // Fran�ais
      "", // Norsk
      "Etsi kaikki", // suomi
      "", // Polski
      "", // Espa�ol
      "", // ��������
      "", // Svenska
      "", // Romaneste
      "", // Magyar
      "", // Catal�
#if VDRVERSNUM >= 10302
      ""  // ������� (Russian)
#if VDRVERSNUM >= 10307
      "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
      "", // Eesti
#if VDRVERSNUM >= 10316
      "", // Dansk
#endif
#endif
#endif
#endif
    },

    { NULL }
};
