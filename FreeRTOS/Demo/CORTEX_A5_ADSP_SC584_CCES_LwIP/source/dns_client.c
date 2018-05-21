/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#include <lwip/sockets.h>
#include <lwip/inet.h>
#include "dns_client.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

/*! DNS server address
 * NOTE: This address needs to be changed as per your network.
 */
#define DNS_HOST_ADDR "10.64.53.110"

static char host_name_to_query[] = "analog.com";
int dns_query_hostname(char *host_name);
extern SemaphoreHandle_t g_semLWIPBootComplete;


/*! DNS receiver port */
#define DNS_RCV_PORT (6002)

/*! DNS server port */
#define DNS_PORT     (53)

/*! DNS packet size */
#define DNS_PACKET_SIZE  (512)

static int dns_id = 1;
static char dns_packet[DNS_PACKET_SIZE];

#if defined(_DEBUG_) || defined(_DEBUG)
#define DNS_ASSERT(str_,val_) do {                                              \
        							if(val_) { printf("%s\n",str_); return -1; }\
								 }  while(0)
#else
#define DNS_ASSERT(str_,val_)
#endif /* _DEBUG_ */

static mx_record g_mx_records[5] = {{0}};

/*! DnsClientTask queries the domain ip address */
void DnsClientTask(void *p_arg)
{
    /* Wait for lwip to finish booting */
	if (pdFALSE == xSemaphoreTake(g_semLWIPBootComplete, portMAX_DELAY))
    {
        printf("DnsClientTask: failed to wait on lwip boot complete semaphore\n");
        return;
    }

    /*
     * LWIP subsystem has initialized and posted the semaphore. *All* threads blocking on this
     * semaphore must be released, so we must give it back to release the next waiting thread.
     */
    if (pdFALSE == xSemaphoreGive(g_semLWIPBootComplete))
    {
    	printf("DnsClientTask: failed to re-post boot semaphore\n");
        return;
    }

    /* start the application task */
    dns_query_hostname(host_name_to_query);

    /* Terminate the DNS client task */
    vTaskDelete(NULL);
}

/*
 * Replaces each . (dot) in the host name with the dns required form.
 * each . is replaced with the number of characters followed by it.
 * including the first string, out_string must be greater than or
 * equal to the length of the host_name.
 *
 */
int construct_dns_string(char *host_name, char *out_string)
{
int i=0,count=0;
char *src,*dst;

        src = host_name;
        dst = out_string;
        if(host_name && out_string && strlen(host_name) > 0)
        {
                for(i=0; i <= strlen(host_name); i++)
                {
                        if(*src != '.')
                        {
                                count++;
                                *++dst  = *src++;
                        }
                        else
                        {
                                *(dst - count) = count;
                                src++; dst++;
                                count=0;
                        }
                }
                *(dst - count) = count-1;
        }
        return 1;
}
/*
 * constructs the dns query packet and returns in dns_pkt. memory is expected
 * to be allocated for the packet
 */
int dns_make_query(int id,char *dns_pkt,dns_q_type_e qtype,char *host_name)
{
char *ptr;
        ptr = dns_pkt;

        if(qtype == DNS_REQ)
        {
                *ptr++ = (char)(id >> 8); // first 2 bytes is id  0
                *ptr++ = (char)(id & 0xFF); // 1
                *ptr++ = (char)1; // flags 1 means query packet. //2
                *ptr++ = (char)0;  //3
                *ptr++ = (char)0; // 4
                *ptr++ = (char)1; // number of questions 1 //5

                *ptr++ = (char)0; // answers 0 //6
                *ptr++ = (char)0; // answers 0 //7

                *ptr++ = (char)0; //8
                *ptr++ = (char)0; //9

                *ptr++ = (char)0;//10
                *ptr++ = (char)0;//11

        construct_dns_string(host_name,ptr);
        ptr += strlen(host_name)+1;

        *ptr++ = 0;
        *ptr++ = 0;

        *ptr++ = 1;
        *ptr++ = 0;
        *ptr++ = 1;
        }

        return 1;
}

// returns the offset of the answers within the DNS packet.
//
int get_answer_offset(char *dns_pkt)
{
        char *ptr = dns_pkt;
        int offset=0;
        ptr +=12; // skip dns header

        // no question
        offset = *ptr & 0xFF;

        if(offset == 0)
        {
                return 0;
        }
        else
        {
                while(offset != 0 )
                {
                        ptr += (offset+1);
                        offset = (*ptr & 0xFF);
                }
                // type & class 2 bytes each
                ptr +=4;
        }

        return ptr - dns_pkt;
}

mx_record* dns_parse_response(char *dns_pkt, int length)
{

        int questions,answers,ans_offset,i=0;
#ifdef __ADSPARM__
        int size __attribute__((unused));
#else
        int size;
#endif
        char *ptr;

        questions = ( (dns_pkt[4] & 0xFF) << 8) | (dns_pkt[5] & 0xFF);
        answers =   ( (dns_pkt[6] & 0xFF) << 8) | (dns_pkt[7] & 0xFF);

        if(!questions || !answers)
                return (mx_record*)0;

        ptr = dns_pkt;
        ans_offset = get_answer_offset(dns_pkt);

        // move ptr to point to answer list.
        ptr += ans_offset;

        // skip domain name,class,time to live,data length
        ptr += 12;

        // size of the address
        //
        size = *ptr++;

        for(i=0; i<answers; i++)
        {
        g_mx_records[i].ip_addr = (*ptr & 0xFF);
        ptr++;
        g_mx_records[i].ip_addr |= (*ptr & 0xFF) << 8 ;
        ptr++;
        g_mx_records[i].ip_addr |= (*ptr & 0xFF) << 16  ;
        ptr++;
        g_mx_records[i].ip_addr |= (*ptr & 0xFF) << 24;

        // skip domain name,class,time to live,data length
        ptr += 12;
        size = *ptr++;
        }
        return g_mx_records;
}

int dns_query_hostname(char *host_name)
{
        int sock_id,ret;
        struct sockaddr_in sa, ra;
        struct in_addr addr;
        char *p;

        sock_id = socket(AF_INET, SOCK_DGRAM, 0);

        DNS_ASSERT("socket creation failure", sock_id < 0);

        memset(&sa, 0, sizeof(struct sockaddr_in)); // clear our address
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        sa.sin_port = htons(DNS_RCV_PORT);

        ret = bind(sock_id, (struct sockaddr *)&sa, sizeof(sa));

        DNS_ASSERT("bind failure",ret < 0);

        memset(dns_packet,0,sizeof(dns_packet));

        // Initilaize receiver's parameters
        memset((char *)&ra,0,sizeof(ra));
        ra.sin_family = AF_INET;
        ra.sin_addr.s_addr = inet_addr(DNS_HOST_ADDR);
        ra.sin_port = htons(DNS_PORT);

        dns_make_query(dns_id++,dns_packet,DNS_REQ,host_name);

        ret = sendto(sock_id,dns_packet,128, 0,(struct sockaddr*)&ra,sizeof(ra));

        DNS_ASSERT("sendto failed",ret < 0);

        memset(dns_packet,0,sizeof(dns_packet));
        ret = recv(sock_id,dns_packet,128,0);

        DNS_ASSERT("recv failed",ret < 0);

        dns_parse_response(dns_packet,ret);

        addr.s_addr = (u32_t)g_mx_records[0].ip_addr;

        p = inet_ntoa(addr);
        printf("IP Address of %s is : %s\n",host_name,p);

#if defined(ETHER_AUTO_TEST)
        if(strcmp(p,"0.0.0.0"))
        {
        	printf("Example passed\n");
        }
        else
        {
        	printf("Example failed\n");
        }
        exit(0);
#endif /* ETHER_AUTO_TEST */
	return(0);
}
