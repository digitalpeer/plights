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
#ifndef TRACK_HPP
#define TRACK_HPP
/**
 * @file
 * @author Joshua D. Henderson
 */

#include <vector>

/**
 * @class CTrack
 *
 * A track runs a single output.  A track consists of events in
 * relation to time to tell when to turn the output on and off.
 * A track is not tied to a specific output, that is done by
 * the mix.
 */
class CTrack
{
public:

   /**
    * Construct a new track by name.
    */
   CTrack();

   inline void SetOutput(unsigned int output)
   {
      mOutput = output;
   }

   /**
    *  Load a track from a file.
    */
   bool Load(const char* file);

   /**
    * Save a track to a file.
    */
   bool Save(const char* file);

   unsigned int operator[](unsigned int index);

   /**
    * Is this output currently enabled?
    */
   bool Enabled();

   bool Finished();

   void Add(unsigned int ms);

   inline void Restart()
   {
      mStep = 0;
   }

   inline int Output() const
   {
      return mOutput;
   }

   inline int Size() const
   {
      return mSteps.size();
   }

   typedef void (*FireCallback)(unsigned int output, bool on);

   bool Fire(unsigned int ms, FireCallback callback);

private:

   /**
    *  All steps for this track.
    */
   std::vector<unsigned int> mSteps;
   unsigned int mStep;

   bool mEnabled;
   int mOutput;
};

#endif
