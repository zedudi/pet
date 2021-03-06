/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef PRINTFWRITER_H_
#define PRINTFWRITER_H_

namespace pet {

/**
 * Tarce writer that uses printf.
 * 
 * This writer is intended to be used in a hosted test environment, where
 * stdlibc io functions are present and are quite straightforward to use.
 */
class PrintfWriter {
protected:
	/** Writes a zero terminated string to the output */
	static void write(const char* val);
	
	/** Writes a zero short value to the output */
	static void write(short val);
	
	/** Writes a zero unsigned short value to the output */
	static void write(unsigned short val);
	
	/** Writes a zero int value to the output */
	static void write(int val);
	
	/** Writes a zero unsigned int value to the output */
	static void write(unsigned int val);
	
	/** Writes a zero long value to the output */
	static void write(long val);
	
	/** Writes a zero unsigned long value to the output */
	static void write(unsigned long val);
	
	/** Writes a zero float value to the output */
	static void write(float val);
	
	/** Writes a zero double value to the output */
	static void write(double val);
	
	/** Writes a zero long value to the output */
	static void write(long double val);
	
	/** Writes a zero pointer value to the output */
	static void write(const void* val);
};

}

#endif /* PRINTFWRITER_H_ */
