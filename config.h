/*
 * magazine.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: config.h,v 1.1 2004/03/05 12:16:26 schmitzj Exp $
 *
 */


#ifndef TVONSCREEN_CONFIG_H
#define TVONSCREEN_CONFIG_H


#include <vdr/config.h>

#define MAX

class tvonscreenConfig
{

public:

  tvonscreenConfig(void)
  {
    *NameUser = 0;
  }

  char showChannels[];
};

#endif 


