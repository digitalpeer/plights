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
#include "pio.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>

PIO::PIO(unsigned int port)
   : mValue(0),mPort(port), mInit(false)
{;}

bool PIO::Init()
{
   bool result = true;
   if (mInit || ioperm(mPort, 1, 1) != 0)
   {
      result = false;
   }
   else
   {
      Reset();
      Flush();
   }
   return (mInit = result);
}

bool PIO::DeInit()
{
   bool result = true;
   if (!mInit || ioperm(mPort, 3, 0))
   {
      result = false;
   }
   mInit = (!result && mInit);
   return result;
}

void PIO::Toggle(unsigned int x)
{
#if 0
   if (on)
   {
      printf("on %d\n",x);
      mValue |= (1 << x);
   }
   else
   {
      printf("off %d\n",x);
      mValue &= ~(1 << x);
   }
#else
   mValue ^= (1 << x);
#endif
}

bool PIO::Status(unsigned int x)
{
   return (mValue & (1 << x));
}

void PIO::Reset()
{
   mValue = 255;
}

void PIO::Flush()
{
   if (mInit)
   {
      //printf("flushing %d to %d\n",mValue,mPort);
      outb(mValue, mPort);
   }
}

PIO::~PIO()
{
   (void)DeInit();
}
