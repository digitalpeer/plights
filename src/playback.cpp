/*
 * plights
 *
 * Copyright (C) 2006, Joshua D. Henderson <www.digitalpeer.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "track.hpp"
#include "pio.hpp"

#include <vector>
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <getopt.h>
#include <time.h>
#include <utime.h>

using namespace std;

bool verbose = false;
bool simulate = false;
bool loop = false;

#define err(format, arg...)						\
   do {									\
	 fprintf(stderr,"error:%d " format "\n", __LINE__, ## arg);	\
   } while (0);

#define info(format, arg...)						\
   do {									\
      if (verbose)							\
	 printf(format "\n", ## arg);					\
   } while (0);


static vector<CTrack*> tracks;
static PIO pio;

static void toggle(unsigned int output, bool on)
{
   pio.Toggle(output);
}

static void play()
{
   // get start time
   struct timespec start;
   (void)clock_gettime(CLOCK_REALTIME,&start);

   bool more = false;

   do
   {
      more = false;
      bool changed = false;

      struct timespec now;
      (void)clock_gettime(CLOCK_REALTIME,&now);

      // get difference between now and start time in milliseconds
      unsigned int diff = (unsigned int)((double)(now.tv_sec-start.tv_sec) * 1000.0) +
	 (unsigned int)((double)(now.tv_nsec-start.tv_nsec) / 1000000.0);

      // fire each track with the current delta time
      for (int x = 0; x < tracks.size();x++)
      {
	 if (tracks[x]->Fire(diff,toggle))
	 {
	    if (verbose)
	       cout << "track " << tracks[x]->Output() << " " << (tracks[x]->Enabled() ? "ON" : "OFF") << endl;

	    changed = true;
	 }

	 more = (more || !tracks[x]->Finished());
      }

      if (changed)
      {
	 pio.Flush();
      }

      usleep(100);
   }
   while(more);
}

static void usage(const char* base)
{
   fprintf(stderr,
	   "Parallel Lights Playback Version 1.0\n"			\
	   "Usage: %s [OPTION] TRACK...\n"				\
	   "   -h,--help                  Show this menu.\n"		\
	   "   -v,--verbose               Show verbose information.\n"	\
	   "   -s,--simulate              Don't actually perform parallel I/O.\n" \
	   "   -l,--loop                  Loop tracks forever.\n" \
	   "\n",base);
}

const char short_options[] = "vhsl";

struct option long_options[] =
   {
      { "verbose",      0, 0, 'v' },
      { "help",         0, 0, 'h' },
      { "simulate",     0, 0, 's' },
      { "loop",         0, 0, 'l' },
      { 0,              0, 0, 0   }
   };

int main(int argc, char** argv)
{
   int result = 0;
   int n;
   int x;

   while((n=getopt_long(argc,argv,short_options,long_options, NULL)) != -1)
   {
      switch(n)
      {
      case 0:
	 break;
      case 'v':
	 verbose = true;
	 break;
      case 'l':
	 loop = true;
	 break;
      case 's':
	 simulate = true;
	 break;
      case 'h':
	 usage(argv[0]);
	 return 0;
      default:
	 usage(argv[0]);
	 return 1;
      }
   }

   if (argc - optind < 1)
   {
      err("need at least one track file");
      usage(argv[0]);
      return 1;
   }

   // load each of the track files
   for (x = optind; x < argc;x++)
   {
      CTrack* track = new CTrack;
      if (!track || !track->Load(argv[x]))
      {
	 err("failed to load track file %s",argv[x]);
	 goto done;
      }

      info("track %d contains %d steps",track->Output(),track->Size());
      tracks.push_back(track);
   }

   // setup parallel I/O
   if (!simulate && !pio.Init())
   {
      err("failed to initialize parallel I/O");
      goto done;
   }

   info("playing %d tracks ...",tracks.size());

 play:

   play();

   if (loop)
   {
      info("looping ...");

      for (int x = 0; x < tracks.size();x++)
      {
	 tracks[x]->Restart();
      }

      goto play;
   }

 done:

   for (vector<CTrack*>::iterator i = tracks.begin();
	i != tracks.end(); ++i)
   {
      delete (*i);
   }

   if (!simulate)
      (void)pio.DeInit();

   return result;
}
