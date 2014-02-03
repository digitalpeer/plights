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

#include <assert.h>
#include <fstream>
using namespace std;

#define err(format, arg...)						\
   do {									\
      fprintf(stderr,"error:%d " format "\n", __LINE__, ## arg);	\
   } while (0);


CTrack::CTrack() : mStep(0),mEnabled(false),mOutput(0)
{;}

/*
 * Track file is in the format:
 *
 * output
 * millisecond delta
 * millisecond delta
 * millisecond delta
 * ...
 */
bool CTrack::Load(const char* file)
{
  ifstream stream(file);
  if (stream.is_open())
  {
     mSteps.clear();

     stream >> mOutput;

     unsigned int step;
     while (stream >> step)
	mSteps.push_back(step);

     stream.close();
     return true;
  }
  else
  {
     err("unable to open file %s",file);
  }

  return false;
}

bool CTrack::Save(const char* file)
{
  ofstream stream(file);
  if (stream.is_open())
  {
     stream << mOutput << endl;

     for (vector<unsigned int>::iterator i = mSteps.begin();
	  i != mSteps.end();++i)
     {
	stream << *i << endl;
     }

     stream.close();
     return true;
  }
  else
  {
     err("unable to open file %s",file);
  }

  return false;
}

unsigned int CTrack::operator[](unsigned int index)
{
   assert(index < mSteps.size());
   return mSteps[index];
}

bool CTrack::Enabled()
{
   return mEnabled;
}

bool CTrack::Finished()
{
   return mStep >= mSteps.size();
}

void CTrack::Add(unsigned int ms)
{
   mSteps.push_back(ms);
}

bool CTrack::Fire(unsigned int ms, FireCallback callback)
{
   bool result = false;

   if (mStep < mSteps.size() && mSteps.size())
   {
      //      err("step %d current %d",mSteps[mStep],ms);
      if (mSteps[mStep] <= ms)
      {
	 mEnabled = !mEnabled;
	 callback(mOutput,mEnabled);
	 mStep++;

	 result = true;
      }
   }

   return result;
}
