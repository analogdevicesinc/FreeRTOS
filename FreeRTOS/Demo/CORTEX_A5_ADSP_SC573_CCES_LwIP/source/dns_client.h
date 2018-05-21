/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#ifndef _DNS_CLIENT_H_
#define _DNS_CLIENT_H_

#ifdef __cplusplus
extern  "C" {
#endif /* __cplusplus */

/*! DNS server port */
#define DNS_PORT  (53)

/*! DNS query enumeration */
typedef enum _dns_q_type_e
{
    DNS_REQ,
    DNS_RES
}dns_q_type_e;

/*! DNS record structure */
typedef struct _mx_record
{
    int ip_addr;
    struct _mx_record *next;
}mx_record;


/*! function prototypes */
int dns_make_query(int id, char *dns_pkt, dns_q_type_e qtype,
                   char *host_name);

int dns_query_hostname(char *host_name);

void DnsClientTask(void *p_arg);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DNS_CLIENT_H_ */
