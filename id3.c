/*  Copyright (c) 2006-2008, Philip Busch <broesel@studcs.uni-sb.de>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "id3.h"

int id3_read(const char *_path, struct id3 *_id3)
{
	char tmp[TAG_LENGTH];

	assert(_path != NULL);
	assert(_id3 != NULL);

	if(id3_read_tag(_path, tmp) < 0)
	        return(-1);

	if(id3_check_tag(tmp) < 0)
	        return(-1);

	if(id3_extract_info(tmp, _id3) < 0)
	        return(-1);

	id3_normalize_info(_id3);

	return(0);
}

int id3_write(const char *_path, struct id3 *_id3)
{
	struct id3 tmp;
	FILE *fp;

	assert(_id3 != NULL);
	assert(_path != NULL);

	if((fp = fopen(_path, "r+")) == NULL)
	        return(-1);

	if(id3_read(_path, &tmp) == 0) {
		if(fseek(fp, -1 * TAG_LENGTH, SEEK_END) == -1)
		        return(-1);
	} else {
		if(fseek(fp, 0, SEEK_END) == -1)
		        return(-1);
	}

        id3_write_tag(fp, _id3);

	fclose(fp);

	return(0);
}

void id3_print(struct id3 *_id3)
{
	char genre[1024];

	assert(_id3 != NULL);

	id3_get_genre_as_string(_id3, genre);

	printf("Title..: %s\n", _id3->title);
	printf("Artist.: %s\n", _id3->artist);
	printf("Album..: %s\n", _id3->album);
	printf("Year...: %s\n", _id3->year);
	printf("Comment: %s\n", _id3->comment);
	printf("Genre..: %s (%d)\n", genre, _id3->genre);
	printf("Track..: %d\n", _id3->track);
}

int id3_read_tag(const char *_path, char *_tag)
{
	FILE *fp;
	int i;
	
	assert(_path != NULL);
	assert(_tag != NULL);

	if((fp = fopen(_path, "r")) == NULL)
	        return(-1);

	if(fseek(fp, -1 * TAG_LENGTH, SEEK_END) == -1)
	        return(-1);

	for(i = 0; i < TAG_LENGTH; i++) {
		_tag[i] = fgetc(fp);
	}

	fclose(fp);

	return(0);
}

int id3_write_tag(FILE *_fp, struct id3 *_id3)
{
	assert(_fp != NULL);
	assert(_id3 != NULL);

	fprintf(_fp, "TAG");
	write_with_padding(_fp, _id3->title, SIZE_INFO);
	write_with_padding(_fp, _id3->artist, SIZE_INFO);
	write_with_padding(_fp, _id3->album, SIZE_INFO);
	write_with_padding(_fp, _id3->year, SIZE_YEAR);
	write_with_padding(_fp, _id3->comment, SIZE_INFO);

	if(_id3->track != 0) {
		fseek(_fp, -1, SEEK_CUR);
		fprintf(_fp, "%c", _id3->track);
	}

	fprintf(_fp, "%c",   _id3->genre);

	return(0);
}

int id3_check_tag(const char *_tag)
{
	assert(_tag != NULL);

	if((_tag[0] != 'T') || (_tag[1] != 'A') || (_tag[2] != 'G'))
	        return(-1);

	return(0);
}


int id3_extract_info(const char *_tag, struct id3 *_id3)
{
	assert(_tag != NULL);
	assert(_id3 != NULL);

	memcpy(_id3->title,   _tag + OFFSET_TITLE,   SIZE_INFO);
	memcpy(_id3->artist,  _tag + OFFSET_ARTIST,  SIZE_INFO);
	memcpy(_id3->album,   _tag + OFFSET_ALBUM,   SIZE_INFO);
	memcpy(_id3->year,    _tag + OFFSET_YEAR,    SIZE_YEAR);
	memcpy(_id3->comment, _tag + OFFSET_COMMENT, SIZE_INFO);

	_id3->track = (_tag[OFFSET_TRACK] == '\0')?0:_tag[OFFSET_TRACK];
	_id3->genre = _tag[OFFSET_GENRE];

	_id3->title[30]   = '\0';
	_id3->artist[30]  = '\0';
	_id3->album[30]   = '\0';
	_id3->year[4]     = '\0';
	_id3->comment[30] = '\0';

	return(0);
}

static void remove_trailing_whitespaces(char *_str)
{
	int i, size;
	
	assert(_str != NULL);

	if(_str == NULL)
	        return;
	
	size = strlen(_str);
	
	for(i = size - 1; i >= 0; i--) {
		if(_str[i] == ' ') {
			_str[i] = '\0';
		} else break;
	}
}

void id3_normalize_info(struct id3 *_id3)
{
	assert(_id3 != NULL);

	remove_trailing_whitespaces(_id3->title);
	remove_trailing_whitespaces(_id3->artist);
	remove_trailing_whitespaces(_id3->album);
	remove_trailing_whitespaces(_id3->comment);
}

void id3_get_genre_as_string(struct id3 *_id3, char *_genre)
{
	assert(_id3 != NULL);
	assert(_genre != NULL);

	if(_id3->genre > GENRE_MAX) {
		strcpy(_genre, "Unknown");
	} else {
		strcpy(_genre, genres[_id3->genre]);
	}
}

static void write_with_padding(FILE *_fp, const char *_str, size_t _len)
{
	unsigned int i;

	assert(_fp != NULL);
	assert(_str != NULL);

	fprintf(_fp, "%s", _str);

	for(i = 0; i < _len - strlen(_str); i++) {
		fprintf(_fp, "%c", '\0');
	}
}

int main()
{
	struct id3 id3;
	struct id3 tmp;
	
	strcpy(tmp.title, "Englishman in New York");
	strcpy(tmp.artist, "Sting");
	strcpy(tmp.album, "Album");
	strcpy(tmp.year, "Year");
	strcpy(tmp.comment, "Comment");
	tmp.genre = 24;
	tmp.track = 4;

	id3_write("sting.mp3", &tmp);
	id3_read("sting.mp3", &id3);
	id3_print(&id3);

	system("PAUSE");
	return(0);
}
