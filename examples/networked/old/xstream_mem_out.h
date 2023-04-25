#ifndef XSTREAM_MEM_OUT_H
#define XSTREAM_MEM_OUT_H

typedef struct
{
	char *storage;
	u32 length;
	u32 at;
    
} XStreamMemOut;

function void  xstream_mem_out_destroy    (XStreamMemOut *s);
function void  xstream_mem_out_realloc    (XStreamMemOut *s, u32 newLength);
function void  xstream_mem_out_write      (XStreamMemOut *s, void *data, u32 length);
function void  xstream_mem_out_write_u32  (XStreamMemOut *s, u32 data);
function void  xstream_mem_out_write_s32  (XStreamMemOut *s, s32 data);
function char *xstream_mem_out_get_ptr    (XStreamMemOut *s);
function u32   xstream_mem_out_get_length (XStreamMemOut *s);




void  xstream_mem_out_destroy(XStreamMemOut *s) {
	xfree(s->storage);
	s->length = 0;
	s->at = 0;
	s->storage = 0;
}

function void
xstream_mem_out_realloc(XStreamMemOut *s, u32 newLength) {
	char *newStorage = xalloc(newLength);
    
	/* If there is storage allocated already */
	if (s->storage)
	{
		/* Copy current content */
		xcopy(newStorage, s->storage, s->length);
		/* Free old array */
		xfree(s->storage);
	}
    
	/* Store new ptr and length */
	s->storage = newStorage;
	s->length = newLength;
}

function void
xstream_mem_out_write(XStreamMemOut *s, void *data, u32 length) {
	u32 resultAt = s->at + length;
	if (resultAt > s->length)
		xstream_mem_out_realloc(s, max(s->length*2, resultAt));
	xcopy(s->storage + s->at, data, length);
	s->at = resultAt;
}

function void
xstream_mem_out_write_u32(XStreamMemOut *s, u32 data) {
	xstream_mem_out_write(s, &data, sizeof(u32));
}

function void
xstream_mem_out_write_s32(XStreamMemOut *s, s32 data) {
	xstream_mem_out_write(s, &data, sizeof(s32));
}

function char *
xstream_mem_out_get_ptr(XStreamMemOut *s) {
	return s->storage;
}

function u32
xstream_mem_out_get_length(XStreamMemOut *s) {
	return s->at;
}

#endif