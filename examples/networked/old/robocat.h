#ifndef ROBOCAT_H
#define ROBOCAT_H

typedef struct {
    u32 health;
    u32 meowCount;
    char name[128];
} RoboCat;

void robocat_write(RoboCat *r, XStreamMemOut *s) {
    xstream_mem_out_write_u32(s, r->health);
    xstream_mem_out_write_u32(s, r->meowCount);
    xstream_mem_out_write(s, r->name, 128);
}

void robocat_read(RoboCat *r, XStreamMemIn *s) {
    xstream_mem_in_read_u32(&r->health);
    xstream_mem_in_read_u32(&r->meowCount);
    xstream_mem_in_read(&r->name, 128);
}

void robocat_send(RoboCat *r, SOCKET sock) {
    XStreamMemOut s;
    robocat_write(r, &s);
    char *data = xstream_mem_out_get_ptr(&s);
    u32 length = xstream_mem_out_get_length(&s);
    send(sock, data, length, 0);
}

void robocat_recv(RoboCat *r, SOCKET sock) {
    char *inBuffer = xalloc(XMAX_PACKET_SIZE);
    size_t recvLength = recv(sock, inBuffer, XMAX_PACKET_SIZE, 0);
    if (recvLength > 0) {
        XStreamMemIn s = xstream_mem_in_create(recvLength);
        robocat_read(r, &s);
        xstream_mem_in_destroy(&s); // This deallocates inBuffer for us
    }
    else
    {
        /* If didn't receive anything, we gotta deallocate inBuffer */
        xfree(inBuffer);
    }
}

#endif