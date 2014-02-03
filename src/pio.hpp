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
#ifndef PIO_HPP
#define PIO_HPP
/**
 * @file
 * @author Joshua D. Henderson
 */

/**
 * @class PIO
 * @brief Parallel Port I/O
 */
class PIO
{
public:

   /**
    * Possible parallel port numbers.
    */
   enum
      {
	 LP1 = 0x278,
	 LP2 = 0x378,
	 LP3 = 0x3BC
      };

   /**
    * Create an instance of an interface to a
    * specific parallel port.
    * @param port The parallel port number.
    */
   PIO(unsigned int port = LP3);

   /**
    * Initializes the API and gains access to
    * the parallel port.
    */
   bool Init();

   /**
    * Deinitializes the API.
    */
   bool DeInit();

   /**
    *  Change a single output to on.
    *  @param x The output in range 0-7.
    */
   void Toggle(unsigned int x);

   /**
    * Get the current status of an output.
    * @param x The output in range 0-7.
    */
   bool Status(unsigned int x);

   /**
    * Set all outputs to off.
    */
   void Reset();

   /**
    * Flushes a change to the parallel port.
    */
   void Flush();

   ~PIO();

private:

   unsigned int mValue;
   unsigned int mPort;
   bool mInit;
};

#endif
