#ifndef PACKET_H
#define PACKET_H

#define PACKET_MAX_SIZE 1440

typedef enum PacketType {
    PacketType_null,
    PacketType_state,
    PacketType_sync,
    PacketType_input,
    PacketType_replicate,
} PacketType;

typedef enum ReplicateType {
    ReplicateType_null,
    ReplicateType_move,
} ReplicateType;

typedef enum InputType {
    InputType_null,
    InputType_move,
} InputType;

static void   packet_write_type (Stream *s, PacketType t);
static void   packet_write_text (Stream *s, char text[256]);
static void   packet_read_type  (Stream *s, PacketType *t);
static void   packet_read_text  (Stream *s, char text[256], u8 *length);
static void
packet_write_type(Stream *s, PacketType type) {
    serialize_u32(s, type);
}

static void
packet_write_text(Stream *s, char text[256]) {
    /* Serialize the text into the byte stream */
    serialize_text(s, text);
}

static void
packet_read_type(Stream *s, PacketType *type) {
    deserialize_u32(s, (u32 *)type);
}

static void
packet_read_text(Stream *s, char text[256], u8 *length) {
    deserialize_text(s, text, length);
}

#endif //PACKET_H
