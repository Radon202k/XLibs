#ifndef XSTREAM_MEM_IN_H
#define XSTREAM_MEM_IN_H

/* NOTE: Streams are ordered sets! That is the reason this is useful, since we
;        can write the fields sequentially and later on read them sequentially. */

typedef struct
{
	char *storage;
	u32 length;
	u32 at;
    
} XStreamMemIn;

function XStreamMemIn xstream_mem_in_create             (char *inBuffer, u32 length);
function void         xstream_mem_in_destroy            (XStreamMemIn *s);
function void         xstream_mem_in_read               (XStreamMemIn *s, void *outData, u32 length);
function void         xstream_mem_in_read_u32           (XStreamMemIn *s, u32 *outData);
function void         xstream_mem_in_read_s32           (XStreamMemIn *s, s32 *outData);
function u32          xstream_mem_in_get_remaining_size (XStreamMemIn *s);




function XStreamMemIn
xstream_mem_in_create(char *inBuffer, u32 length) {
	XStreamMemIn r = {
		inBuffer,
		length,
		0
	};
	return r;
}

function void
xstream_mem_in_destroy(XStreamMemIn *s) {
	xfree(s->storage);
	s->length = 0;
	s->at = 0;
	s->storage = 0;
}

function void
xstream_mem_in_read(XStreamMemIn *s, void *outData, u32 length) {
	u32 remainingSize = xstream_mem_in_get_remaining_size(s);
	assert(remainingSize <= length);
	xcopy(outData, s->storage + s->at, length);
	s->at += length;
}

function void
xstream_mem_in_read_u32(XStreamMemIn *s, u32 *outData) {
	xstream_mem_in_read(s, outData, sizeof *outData);
}

function void
xstream_mem_in_read_s32(XStreamMemIn *s, s32 *outData) {
	xstream_mem_in_read(s, outData, sizeof *outData);
}

function u32
xstream_mem_in_get_remaining_size (XStreamMemIn *s) {
	return s->length - s->at;
}

#endif