typedef struct
{
	struct sockaddr addr;
} XSocketAddress;

#define IPV4(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|d)

function XSocketAddress
xsocket_address_ipv4(u32 address, u16 port)
{
	XSocketAddress result;
    
	struct sockaddr_in *p = (struct sockaddr_in *)&result;
	p->sin_family = AF_INET;
	p->sin_addr.S_un.S_addr = htonl(address);
	p->sin_port = htons(port);
    
	return result;
}