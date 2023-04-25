#ifndef SERIALIZATION_H
#define SERIALIZATION_H

typedef struct Stream {
    char *storage;
    u32 at;
} Stream;

static void deserialize_construct (Stream *s, char *readData);
static void serialize_construct (Stream *s, u32 byteSize);
static void serialize_destruct  (Stream *s);
static void serialize_u8        (Stream *s, u8 value);
static void serialize_u32       (Stream *s, u32 value);
static void serialize_text      (Stream *s, char text[256]);
static void serialize_v4        (Stream *s, v4 value);
static void deserialize_u8      (Stream *s, u8 *value);
static void deserialize_u32     (Stream *s, u32 *value);
static void deserialize_text    (Stream *s, char text[256], u8 *length);
static void deserialize_v4      (Stream *s, v4 value);




static void
deserialize_construct (Stream *s, char *readData) {
    *s = (Stream){0};
    s->storage = readData;
}

static void
serialize_construct(Stream *stream, u32 byteSize) {
    *stream = (Stream){0};
    stream->storage = xalloc(byteSize);
    stream->at = 0;
}

static void
serialize_destruct(Stream *stream) {
    xfree(stream->storage);
    stream->storage = 0;
    stream->at = 0;
}

static void
serialize_u8(Stream *stream, u8 value) {
    memcpy(stream->storage + stream->at, &value, sizeof(u8));
    stream->at += sizeof(u8);
}

static void
serialize_u32(Stream *stream, u32 value) {
    memcpy(stream->storage + stream->at, &value, sizeof(u32));
    stream->at += sizeof(u32);
}

static void
serialize_text(Stream *stream, char text[256]) {
    u8 length = (u8)strlen(text);
    serialize_u8(stream, length);
    memcpy(stream->storage + stream->at, text, sizeof(char)*length);
    stream->at += sizeof(char)*length;
}

static void
serialize_v4(Stream *s, v4 value) {
    u32 r255 = (u32)((value[0] * 255.0f) + 0.5f);
    u32 g255 = (u32)((value[1] * 255.0f) + 0.5f);
    u32 b255 = (u32)((value[2] * 255.0f) + 0.5f);
    u32 a255 = (u32)((value[3] * 255.0f) + 0.5f);
    serialize_u32(s, r255);
    serialize_u32(s, g255);
    serialize_u32(s, b255);
    serialize_u32(s, a255);
}

static void
deserialize_u8(Stream *s, u8 *value) {
    memcpy(value, s->storage + s->at, sizeof(u8));
    s->at += sizeof(u8);
}

static void
deserialize_u32(Stream *s, u32 *value) {
    memcpy(value, s->storage + s->at, sizeof(u32));
    s->at += sizeof(u32);
}

static void
deserialize_text(Stream *s, char text[256], u8 *length) {
    deserialize_u8(s, length);
    assert(*length >= 0 && *length <= 256);
    memcpy(text, s->storage + s->at, sizeof(char)*(*length));
    s->at += sizeof(char)*(*length);
}

static void
deserialize_v4(Stream *s, v4 value) {
    u32 r255 = 0;
    u32 g255 = 0;
    u32 b255 = 0;
    u32 a255 = 0;
    deserialize_u32(s, &r255);
    deserialize_u32(s, &g255);
    deserialize_u32(s, &b255);
    deserialize_u32(s, &a255);
    v4_copy((v4) {
                (f32)r255 / 255.0f,
                (f32)g255 / 255.0f,
                (f32)b255 / 255.0f,
                (f32)a255 / 255.0f,
            }, value);
    
}

#endif //SERIALIZATION_H
