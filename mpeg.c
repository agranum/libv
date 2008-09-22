/* (C) Copyright 2001 by Felix Opatz <felix@zotteljedi.de>
 *
 * Copyright (c) 2003
 *    Felix Opatz <felix@zotteljedi.de>.  All rights reserved.
 *
 * Copyright (c) 2007
 *    Philip Busch <philip@0xe3.com>. All rights reserved.
 *
 * Reads MPEG frame header information.
 * See http://www.dv.co.yu/mpgscript/mpeghdr.htm for MPEG frame header specs.
 * See http://www.zotteljedi.de/kleinkram/src/mp3info.c for original version.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer as
 *    the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FELIX OPATZ BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpeg.h"

/* some magic numbers, see docs */
static const int bitrates[2][3][15] = {
	{
		/* layer 3 */
		{
			0, 32, 40, 48, 56, 64, 80,  96, 112, 128, 160, 192, 224, 256, 320
		},

		/* layer 2 */
		{
			0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384
		},

		/* layer 1 */
		{
			0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448
		}
	},

	{
		/* layer 3 */
		{
			8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160
		},

		/* layer 2 */
		{
			8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160
		},

		/* layer 1 */
		{
			0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256
		}
	}
};

static const int freqs[3][3] = {
	/* MPEG 1 */
	{
		44100, 48000, 32000
	},
	/* MPEG 2 */
	{
		22050, 24000, 16000
	},
	/* MPEG 2.5 */
	{
		11025, 12000, 8000
	}
};


/* our own ntohl() implementation for strict ANSI compliance */
#ifndef ntohl
	#define ntohl own_ntohl
#endif

static unsigned long int own_ntohl(unsigned long int in)
{
	unsigned char a[4], t;

	memcpy(a, &in, 4);

	t = a[3];
	a[3] = a[0];
	a[0] = t;

	t = a[2];
	a[2] = a[1];
	a[1] = t;

	memcpy(&in, a, 4);

	return in;
}


int mpeg_read(const char *path_, struct mpeg *mpeg_)
{
	FILE *fp;
	unsigned long int header;

	if ((fp = fopen(path_, "rb")) == NULL) {
		return(-1);
	}

	if((header = mpeg_seek_nextheader_(fp)) == (unsigned long int)-1) {
		return(-1);
	}

	if(mpeg_extract_info(header, mpeg_) == -1) {
		return(-1);
	}

	fclose(fp);
	return(0);
}

unsigned long int mpeg_seek_nextheader_(FILE *fp_)
{
	unsigned long int header;
	int c;

	while((c = fgetc(fp_)) != 0xff && c != EOF);

	if (feof(fp_))
		return(-1);

	ungetc(c, fp_);
	fread(&header, 1, sizeof(header), fp_);
	header = ntohl(header);

	return(header);
}

int mpeg_extract_info(unsigned long int header_, struct mpeg *mpeg_)
{
	if ((header_ & MASK_SYNC) != MASK_SYNC)
		return(-1);

	mpeg_->mpeg_version = ((header_ & MASKmpeg_) >> SHIFTmpeg_);

	mpeg_->layer_desc = ((header_ & MASK_LAYER) >> SHIFT_LAYER);

	mpeg_->bit_prot = ((header_ & MASK_PROT) >> SHIFT_PROT);

	/* causes pain for MPEG_VERSION_2_5 */
	mpeg_->bitrate = bitrates[(mpeg_->mpeg_version == MPEG_VERSION_1)?0:1][mpeg_->layer_desc - 1][((header_ & MASK_BITRATE) >> SHIFT_BITRATE)];

	switch(mpeg_->mpeg_version)
	{
		case MPEG_VERSION_1:
			mpeg_->freq = freqs[0][((header_ & MASK_FREQ) >> SHIFT_FREQ)];
			break;
		case MPEG_VERSION_2:
			mpeg_->freq = freqs[1][((header_ & MASK_FREQ) >> SHIFT_FREQ)];
			break;
		case MPEG_VERSION_2_5:
			mpeg_->freq = freqs[2][((header_ & MASK_FREQ) >> SHIFT_FREQ)];
			break;
	}

	mpeg_->bit_padding = ((header_ & MASK_PADDING) >> SHIFT_PADDING);

	mpeg_->bit_priv = ((header_ & MASK_PRIV) >> SHIFT_PRIV);

	mpeg_->chan = (header_ & MASK_CHAN) >> SHIFT_CHAN;

	mpeg_->mode_ext = (header_ & MASK_MODE_EXT) >> SHIFT_MODE_EXT;

	mpeg_->bit_copyright = (header_ & MASK_COPYRIGHT) >> SHIFT_COPYRIGHT;

	mpeg_->bit_orig = (header_ & MASK_ORIG) >> SHIFT_ORIG;

	mpeg_->emphasis = (header_ & MASK_EMPHASIS) >> SHIFT_EMPHASIS;

	return(0);
}

size_t mpeg_frame_length(struct mpeg *mpeg_)
{
	int f = (mpeg_->layer_desc == LAYER_VERSION_1)?12:36;

	return(f * mpeg_->bitrate * 1000 / mpeg_->freq + mpeg_->bit_padding);
}

size_t mpeg_frame_bytes(struct mpeg *mpeg_)
{
	return(4 * mpeg_frame_length(mpeg_));
}

void mpeg_print(struct mpeg *mpeg_)
{
	float ver;
	int layer;
	const char *layers[] = {"I", "II", "III"};
	const char *chan_modes[] = {
		"Stereo",
		"Joint stereo (Stereo)",
		"Dual channel (Stereo)",
		"Single channel (Mono)"
	};

	const char *emphasis[] = {
		"none",
		"50/15 ms",
		"reserved",
		"CCIT J.17"
	};

	if (mpeg_->mpeg_version == MPEG_VERSION_1)
		ver = 1;
	if (mpeg_->mpeg_version == MPEG_VERSION_2)
		ver = 2;
	if (mpeg_->mpeg_version == MPEG_VERSION_2_5)
		ver = 2.5;

	if (mpeg_->layer_desc == LAYER_VERSION_1)
		layer = 1;
	if (mpeg_->layer_desc == LAYER_VERSION_2)
		layer = 2;
	if (mpeg_->layer_desc == LAYER_VERSION_3)
		layer = 3;

	printf("MPEG Version %.0f / Layer %s (", ver, layers[layer-1]);

	if (mpeg_->mpeg_version != MPEG_VERSION_2_5)
		printf("%i KBit/s, ", mpeg_->bitrate);

	printf("%i Hz, ", mpeg_->freq);

	printf("%s)\n", chan_modes[mpeg_->chan]);

	printf("Mode extension: ");
	if(mpeg_->chan == CMODE_JOINT_STEREO) {
		if(mpeg_->layer_desc == LAYER_VERSION_3) {
			printf("intensity stereo %s, MS stereo %s\n",
				((mpeg_->mode_ext==1) || (mpeg_->mode_ext==3))?"on":"off",
				(mpeg_->mode_ext > 1)?"on":"off"
			);
		} else {
			printf("bands %d to 31\n", (mpeg_->mode_ext + 1) * 4);
		}
	} else {
		printf("no mode extension\n");
	}

	printf("Emphasis:\t%s\n", emphasis[mpeg_->emphasis]);
	printf("Protection:\t%s\n", mpeg_->bit_prot?"no":"yes");
	printf("Padding:\t%s\n", mpeg_->bit_padding?"yes":"no");
	printf("Private:\t%s\n", mpeg_->bit_priv?"yes":"no");
	printf("Copyright:\t%s\n", mpeg_->bit_copyright?"yes":"no");
	printf("Original media:\t%s\n", mpeg_->bit_orig?"yes":"no");
	
	printf("Frame size:\t%d slots (%d bytes)\n", mpeg_frame_length(mpeg_), mpeg_frame_bytes(mpeg_));
}

int main(int argc, char *argv[])
{
	struct mpeg mpeg_data;
	int i;

	if (argc < 2) {
		fprintf(stderr, "USAGE: %s <file(s)>\n", argv[0]);
		return 1;
	}

	for (i = 1; i < argc; i++) {
		if(mpeg_read(argv[i], &mpeg_data) < 0) {
			fprintf(stderr, "%s: invalid/missing header, ignoring\n",	argv[i]);
		} else {
			printf("MPEG frame header information for %s:\n", argv[i]);
			mpeg_print(&mpeg_data);
		}
	}

	return(0);
}
