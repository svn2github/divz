/*
 * Libavformat API example: Output a media file in any supported
 * libavformat format. The default codecs are used.
 *
 * Copyright (c) 2003 Fabrice Bellard
 * Converted to C++ 'class' format by Josiah Bryan, 2010.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdio.h>

#include "VideoEncoderThread.h"

int main(int argc, char **argv)
{
	char *filename;
	
	if (argc != 2) 
	{
		printf("usage: %s output_file\n"
			"API example program to output a media file with libavformat.\n"
			"The output format is automatically guessed according to the file extension.\n"
			"Raw images can also be output by using '%%d' in the filename\n"
			"\n", argv[0]);
		exit(1);
	}
	
	filename = argv[1];
	
	VideoEncoderThread thread(filename);
	thread.run();	
	
	return 0;
}
