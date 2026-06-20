#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/neighbour.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>

#define BUF_SIZE 32768
#define MAX_INTERFACES 256

// GLOBAL FILTER
static int af_filter = AF_UNSPEC;
static int use_color = 1;
static uint16_t nud_filter = 0;
static uint32_t route_table_filter = 0;
static int route_proto_filter = -1;
static int route_scope_filter = -1;
static const char *p_name = "ip";
static struct {
    int family;
    int bitlen;
    unsigned char addr[16];
} addr_filter = { AF_UNSPEC, 0, {0} };

// ===================== COLOR HELPERS =====================

static const char *c_ifname(void) { return use_color ? "\033[36m" : ""; } /* Cyan */
static const char *c_ip(void)     { return use_color ? "\033[35m" : ""; } /* Magenta */
static const char *c_mac(void)    { return use_color ? "\033[32m" : ""; } /* Green */
static const char *c_reset(void)  { return use_color ? "\033[0m"  : ""; } /* Reset */

// ===================== NETLINK HELPERS =====================

static int nl_send(int sock, struct nlmsghdr *nlh) {
    struct sockaddr_nl addr = { .nl_family = AF_NETLINK };
    struct iovec iov = { .iov_base = nlh, .iov_len = nlh->nlmsg_len };
    struct msghdr msg = {
        .msg_name = &addr,
        .msg_namelen = sizeof(addr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    return sendmsg(sock, &msg, 0);
}

// ===================== LINK =====================

static void ip_link_show(const char *filter_dev) {
    struct ifaddrs *ifaddr, *ifa;
    char seen[MAX_INTERFACES][IFNAMSIZ];
    int seen_count = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == NULL)
            continue;

        if (filter_dev && strlen(filter_dev) > 0 && strcmp(filter_dev, ifa->ifa_name) != 0)
            continue;

        /* Check if already processed */
        int already_seen = 0;
        for (int i = 0; i < seen_count; i++) {
            if (strncmp(seen[i], ifa->ifa_name, IFNAMSIZ) == 0) {
                already_seen = 1;
                break;
            }
        }

        if (already_seen)
            continue;

        /* Add to seen list */
        if (seen_count < MAX_INTERFACES) {
            strncpy(seen[seen_count], ifa->ifa_name, IFNAMSIZ - 1);
            seen[seen_count][IFNAMSIZ - 1] = '\0';
            seen_count++;
        }

        /* Optimized Output formatting */
        printf("%s%s%s: <", c_ifname(), ifa->ifa_name, c_reset());
        int first = 1;
        unsigned int flags = ifa->ifa_flags;
        
        #define PFLAG(f, name) if (flags & f) { if (!first) printf(","); printf(name); first = 0; }
        PFLAG(IFF_UP, "UP")
        PFLAG(IFF_BROADCAST, "BROADCAST")
        PFLAG(IFF_DEBUG, "DEBUG")
        PFLAG(IFF_LOOPBACK, "LOOPBACK")
        PFLAG(IFF_POINTOPOINT, "POINTOPOINT")
        PFLAG(IFF_RUNNING, "RUNNING")
        PFLAG(IFF_NOARP, "NOARP")
        PFLAG(IFF_PROMISC, "PROMISC")
        PFLAG(IFF_MULTICAST, "MULTICAST")
        #undef PFLAG
        
        printf(">\n");
    }

    freeifaddrs(ifaddr);
}

static int do_link(int argc, char **argv) {
    if (argc > 0 && strcmp(argv[0], "help") == 0) {
        fprintf(stderr, "Usage: %s link show [dev]\n", p_name);
        return 0;
    }
    if (argc == 0 || strcmp(argv[0], "show") == 0 || strcmp(argv[0], "lst") == 0 || strcmp(argv[0], "list") == 0) {
        const char *dev = (argc > 1) ? argv[1] : NULL;
        ip_link_show(dev);
        return 0;
    }
    fprintf(stderr, "Command \"%s\" is unknown, try \"%s link help\".\n", argv[0], p_name);
    return -1;
}

static void ip_addr_show(const char *dev);

static int do_addr(int argc, char **argv) {
    if (argc > 0 && strcmp(argv[0], "help") == 0) {
        fprintf(stderr, "Usage: %s addr show [dev]\n", p_name);
        return 0;
    }
    if (argc == 0 || strcmp(argv[0], "show") == 0 || strcmp(argv[0], "lst") == 0 || strcmp(argv[0], "list") == 0) {
        const char *dev = (argc > 1) ? argv[1] : NULL;
        ip_addr_show(dev);
        return 0;
    }
    fprintf(stderr, "Command \"%s\" is unknown, try \"%s addr help\".\n", argv[0], p_name);
    return -1;
}

// ===================== ROUTE =====================

static const char *get_table_name(uint32_t id) {
    switch (id) {
        case RT_TABLE_UNSPEC: return "unspec";
        case RT_TABLE_COMPAT: return "compat";
        case RT_TABLE_DEFAULT: return "default";
        case RT_TABLE_MAIN: return "main";
        case RT_TABLE_LOCAL: return "local";
        default: return NULL;
    }
}

static const char *get_proto_name(unsigned char proto) {
    switch (proto) {
        case RTPROT_UNSPEC: return "unspec";
        case RTPROT_REDIRECT: return "redirect";
        case RTPROT_KERNEL: return "kernel";
        case RTPROT_BOOT: return "boot";
        case RTPROT_STATIC: return "static";
#ifdef RTPROT_GATED
        case RTPROT_GATED: return "gated";
#endif
#ifdef RTPROT_RA
        case RTPROT_RA: return "ra";
#endif
#ifdef RTPROT_MRT
        case RTPROT_MRT: return "mrt";
#endif
#ifdef RTPROT_ZEBRA
        case RTPROT_ZEBRA: return "zebra";
#endif
#ifdef RTPROT_BIRD
        case RTPROT_BIRD: return "bird";
#endif
#ifdef RTPROT_DNROUTED
        case RTPROT_DNROUTED: return "dnrouted";
#endif
#ifdef RTPROT_XORP
        case RTPROT_XORP: return "xorp";
#endif
#ifdef RTPROT_NTK
        case RTPROT_NTK: return "ntk";
#endif
#ifdef RTPROT_DHCP
        case RTPROT_DHCP: return "dhcp";
#endif
#ifdef RTPROT_MROUTED
        case RTPROT_MROUTED: return "mrouted";
#endif
#ifdef RTPROT_BABEL
        case RTPROT_BABEL: return "babel";
#endif
        default: return NULL;
    }
}

static const char *get_scope_name(unsigned char scope) {
    switch (scope) {
        case RT_SCOPE_UNIVERSE: return "global";
        case RT_SCOPE_NOWHERE: return "nowhere";
        case RT_SCOPE_HOST: return "host";
        case RT_SCOPE_LINK: return "link";
        case RT_SCOPE_SITE: return "site";
        default: return NULL;
    }
}

static uint32_t parse_table(const char *name) {
    if (strcmp(name, "main") == 0) return RT_TABLE_MAIN;
    if (strcmp(name, "local") == 0) return RT_TABLE_LOCAL;
    if (strcmp(name, "default") == 0) return RT_TABLE_DEFAULT;
    if (strcmp(name, "compat") == 0) return RT_TABLE_COMPAT;
    if (strcmp(name, "unspec") == 0) return RT_TABLE_UNSPEC;
    return (uint32_t)strtoul(name, NULL, 0);
}

static int parse_proto(const char *name) {
    if (strcmp(name, "kernel") == 0) return RTPROT_KERNEL;
    if (strcmp(name, "boot") == 0) return RTPROT_BOOT;
    if (strcmp(name, "static") == 0) return RTPROT_STATIC;
    if (strcmp(name, "redirect") == 0) return RTPROT_REDIRECT;
    return atoi(name);
}

static int parse_scope(const char *name) {
    if (strcmp(name, "global") == 0 || strcmp(name, "universe") == 0) return RT_SCOPE_UNIVERSE;
    if (strcmp(name, "nowhere") == 0) return RT_SCOPE_NOWHERE;
    if (strcmp(name, "host") == 0) return RT_SCOPE_HOST;
    if (strcmp(name, "link") == 0) return RT_SCOPE_LINK;
    if (strcmp(name, "site") == 0) return RT_SCOPE_SITE;
    return atoi(name);
}

static void parse_route(struct nlmsghdr *nlh, const char *filter_dev) {
    struct rtmsg *rtm = NLMSG_DATA(nlh);

    if (af_filter != AF_UNSPEC && rtm->rtm_family != af_filter)
        return;

    if (route_proto_filter != -1 && rtm->rtm_protocol != route_proto_filter)
        return;

    if (route_scope_filter != -1 && rtm->rtm_scope != route_scope_filter)
        return;

    struct rtattr *rta = RTM_RTA(rtm);
    int rtl = RTM_PAYLOAD(nlh);

    char dst[INET6_ADDRSTRLEN] = "";
    char gw[INET6_ADDRSTRLEN] = "";
    char dev[IF_NAMESIZE] = "";
    
    // Fetch initial table ID from route message header
    uint32_t table_id = rtm->rtm_table;

    for (; RTA_OK(rta, rtl); rta = RTA_NEXT(rta, rtl)) {
        switch (rta->rta_type) {
            case RTA_DST:
                inet_ntop(rtm->rtm_family, RTA_DATA(rta), dst, sizeof(dst));
                break;
            case RTA_GATEWAY:
                inet_ntop(rtm->rtm_family, RTA_DATA(rta), gw, sizeof(gw));
                break;
            case RTA_OIF: {
                int idx;
                memcpy(&idx, RTA_DATA(rta), sizeof(idx));
                if_indextoname(idx, dev);
                break;
            }
            case RTA_TABLE: {
                // Extended table ID (takes priority over rtm->rtm_table)
                memcpy(&table_id, RTA_DATA(rta), sizeof(table_id));
                break;
            }
        }
    }

    if (route_table_filter != 0 && table_id != route_table_filter)
        return;

    if (filter_dev && strlen(filter_dev) > 0 && strcmp(filter_dev, dev) != 0)
        return;

    if (strlen(dst) == 0)
        printf("default ");
    else
        printf("%s%s/%d%s ", c_ip(), dst, rtm->rtm_dst_len, c_reset());

    if (strlen(gw))
        printf("via %s%s%s ", c_ip(), gw, c_reset());

    if (strlen(dev))
        printf("dev %s%s%s ", c_ifname(), dev, c_reset());

    // Print out the scope
    if (rtm->rtm_scope != RT_SCOPE_UNIVERSE) {
        const char *scope_name = get_scope_name(rtm->rtm_scope);
        if (scope_name)
            printf("scope %s ", scope_name);
        else
            printf("scope %u ", rtm->rtm_scope);
    }

    // Print out the protocol
    const char *proto_name = get_proto_name(rtm->rtm_protocol);
    if (proto_name)
        printf("proto %s ", proto_name);
    else
        printf("proto %u ", rtm->rtm_protocol);

    // Print out the table ID
    const char *table_name = get_table_name(table_id);
    if (table_name)
        printf("table %s\n", table_name);
    else
        printf("table %u\n", table_id);
}

static void ip_route_show(const char *dev) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) { perror("socket"); return; }

    struct {
        struct nlmsghdr nlh;
        struct rtmsg rtm;
    } req = {0};

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nlh.nlmsg_type = RTM_GETROUTE;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 1;

    req.rtm.rtm_family = af_filter;

    if (nl_send(sock, &req.nlh) < 0) {
        perror("sendmsg"); close(sock); return;
    }

    char buf[BUF_SIZE];

    while (1) {
        int len = recv(sock, buf, sizeof(buf), 0);
        if (len < 0) { perror("recv"); break; }

        struct nlmsghdr *nlh = (struct nlmsghdr *)buf;

        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_seq != 1) continue;

            if (nlh->nlmsg_type == NLMSG_DONE) {
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = NLMSG_DATA(nlh);
                errno = -err->error;
                perror("netlink");
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == RTM_NEWROUTE)
                parse_route(nlh, dev);
        }
    }

    close(sock);
}

static void ip_route_get(const char *dst_str, const char *src_str) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) { perror("socket"); return; }

    struct {
        struct nlmsghdr nlh;
        struct rtmsg rtm;
        char buf[256];
    } req = {0};

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nlh.nlmsg_type = RTM_GETROUTE;
    req.nlh.nlmsg_flags = NLM_F_REQUEST;
    req.nlh.nlmsg_seq = 4;

    unsigned char addr[16];
    int family = AF_INET;
    if (inet_pton(AF_INET, dst_str, addr) > 0) {
        family = AF_INET;
    } else if (inet_pton(AF_INET6, dst_str, addr) > 0) {
        family = AF_INET6;
    } else {
        fprintf(stderr, "Invalid address: %s\n", dst_str);
        close(sock);
        return;
    }

    req.rtm.rtm_family = (unsigned char)family;
    req.rtm.rtm_dst_len = (unsigned char)((family == AF_INET) ? 32 : 128);

    struct rtattr *rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nlh.nlmsg_len));
    rta->rta_type = RTA_DST;
    rta->rta_len = (unsigned short)RTA_LENGTH((family == AF_INET) ? 4 : 16);
    memcpy(RTA_DATA(rta), addr, (size_t)((family == AF_INET) ? 4 : 16));
    req.nlh.nlmsg_len = (uint32_t)(NLMSG_ALIGN(req.nlh.nlmsg_len) + RTA_ALIGN(rta->rta_len));

    if (src_str) {
        unsigned char saddr[16];
        if (inet_pton(family, src_str, saddr) > 0) {
            struct rtattr *rta_src = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nlh.nlmsg_len));
            rta_src->rta_type = RTA_SRC;
            rta_src->rta_len = (unsigned short)RTA_LENGTH((family == AF_INET) ? 4 : 16);
            memcpy(RTA_DATA(rta_src), saddr, (size_t)((family == AF_INET) ? 4 : 16));
            req.nlh.nlmsg_len = (uint32_t)(NLMSG_ALIGN(req.nlh.nlmsg_len) + RTA_ALIGN(rta_src->rta_len));
            req.rtm.rtm_src_len = (unsigned char)((family == AF_INET) ? 32 : 128);
        } else {
            fprintf(stderr, "Invalid source address: %s\n", src_str);
        }
    }

    if (nl_send(sock, &req.nlh) < 0) {
        perror("sendmsg"); close(sock); return;
    }

    char res_buf[BUF_SIZE];
    int len = (int)recv(sock, res_buf, sizeof(res_buf), 0);
    if (len < 0) { perror("recv"); close(sock); return; }

    struct nlmsghdr *nlh = (struct nlmsghdr *)res_buf;
    for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
        if (nlh->nlmsg_seq != 4) continue;
        if (nlh->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = NLMSG_DATA(nlh);
            if (err->error < 0)
                fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
            break;
        }
        if (nlh->nlmsg_type == RTM_NEWROUTE) {
            parse_route(nlh, NULL);
        }
    }

    close(sock);
}

// ===================== ADDR =====================

static void parse_addr(struct nlmsghdr *nlh, const char *filter_dev) {
    struct ifaddrmsg *ifa = NLMSG_DATA(nlh);

    if (af_filter != AF_UNSPEC && ifa->ifa_family != af_filter)
        return;
    struct rtattr *rta = IFA_RTA(ifa);
    int rtl = IFA_PAYLOAD(nlh);

    char addr[INET6_ADDRSTRLEN] = "";
    char dev[IF_NAMESIZE] = "";

    if_indextoname(ifa->ifa_index, dev);

    for (; RTA_OK(rta, rtl); rta = RTA_NEXT(rta, rtl)) {
        if (rta->rta_type == IFA_ADDRESS || rta->rta_type == IFA_LOCAL) {
            inet_ntop(ifa->ifa_family, RTA_DATA(rta), addr, sizeof(addr));
        }
    }

    if (filter_dev && strcmp(filter_dev, dev) != 0)
        return;

    if (strlen(addr))
        printf("%s%s%s %s%s/%d%s\n", c_ifname(), dev, c_reset(), c_ip(), addr, ifa->ifa_prefixlen, c_reset());
}

static void ip_addr_show(const char *dev) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) { perror("socket"); return; }

    struct {
        struct nlmsghdr nlh;
        struct ifaddrmsg ifa;
    } req = {0};

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.nlh.nlmsg_type = RTM_GETADDR;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 2;

    req.ifa.ifa_family = af_filter;

    if (nl_send(sock, &req.nlh) < 0) {
        perror("sendmsg"); close(sock); return;
    }

    char buf[BUF_SIZE];

    while (1) {
        int len = recv(sock, buf, sizeof(buf), 0);
        if (len < 0) { perror("recv"); break; }

        struct nlmsghdr *nlh = (struct nlmsghdr *)buf;

        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_seq != 2) continue;

            if (nlh->nlmsg_type == NLMSG_DONE) {
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = NLMSG_DATA(nlh);
                errno = -err->error;
                perror("netlink");
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == RTM_NEWADDR)
                parse_addr(nlh, dev);
        }
    }

    close(sock);
}

// ===================== NEIGHBOR =====================

static int parse_prefix(const char *prefix) {
    char *slash = strchr(prefix, '/');
    char buf[INET6_ADDRSTRLEN] = {0};
    if (slash) {
        size_t len = (size_t)(slash - prefix);
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        strncpy(buf, prefix, len);
    } else {
        strncpy(buf, prefix, sizeof(buf) - 1);
    }
    
    if (inet_pton(AF_INET, buf, addr_filter.addr) > 0) {
        addr_filter.family = AF_INET;
        addr_filter.bitlen = slash ? atoi(slash + 1) : 32;
    } else if (inet_pton(AF_INET6, buf, addr_filter.addr) > 0) {
        addr_filter.family = AF_INET6;
        addr_filter.bitlen = slash ? atoi(slash + 1) : 128;
    } else {
        return -1;
    }
    return 0;
}

static uint16_t parse_nud_state(const char *state) {
    if (strcasecmp(state, "permanent") == 0) return NUD_PERMANENT;
    if (strcasecmp(state, "noarp") == 0) return NUD_NOARP;
    if (strcasecmp(state, "reachable") == 0) return NUD_REACHABLE;
    if (strcasecmp(state, "stale") == 0) return NUD_STALE;
    if (strcasecmp(state, "delay") == 0) return NUD_DELAY;
    if (strcasecmp(state, "probe") == 0) return NUD_PROBE;
    if (strcasecmp(state, "failed") == 0) return NUD_FAILED;
    if (strcasecmp(state, "incomplete") == 0) return NUD_INCOMPLETE;
    if (strcasecmp(state, "none") == 0) return NUD_NONE;
    return 0;
}

static void parse_neigh(struct nlmsghdr *nlh, const char *filter_dev) {
    struct ndmsg *ndm = NLMSG_DATA(nlh);

    if (af_filter != AF_UNSPEC && ndm->ndm_family != af_filter)
        return;

    if (nud_filter && !(ndm->ndm_state & nud_filter))
        return;

    int attr_len = nlh->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm));
    struct rtattr *rta = (struct rtattr *)((char *)ndm + NLMSG_ALIGN(sizeof(*ndm)));

    struct rtattr *tb[NDA_MAX + 1];
    memset(tb, 0, sizeof(tb));

    while (RTA_OK(rta, attr_len)) {
        if (rta->rta_type <= NDA_MAX)
            tb[rta->rta_type] = rta;
        rta = RTA_NEXT(rta, attr_len);
    }

    if (addr_filter.family != AF_UNSPEC) {
        if (ndm->ndm_family != addr_filter.family)
            return;
        if (tb[NDA_DST]) {
            unsigned char *neigh_addr = RTA_DATA(tb[NDA_DST]);
            int bytes = addr_filter.bitlen / 8;
            int bits = addr_filter.bitlen % 8;
            if (memcmp(neigh_addr, addr_filter.addr, (size_t)bytes) != 0)
                return;
            if (bits > 0) {
                unsigned char mask = (unsigned char)(0xFF << (8 - bits));
                if ((neigh_addr[bytes] & mask) != (addr_filter.addr[bytes] & mask))
                    return;
            }
        } else {
            return;
        }
    }

    char ip[INET6_ADDRSTRLEN] = {0};
    char ifname[IF_NAMESIZE] = {0};

    if_indextoname(ndm->ndm_ifindex, ifname);

    if (filter_dev && strlen(filter_dev) > 0 && strcmp(filter_dev, ifname) != 0)
        return;

    if (tb[NDA_DST]) {
        if (ndm->ndm_family == AF_INET) {
            inet_ntop(AF_INET, RTA_DATA(tb[NDA_DST]), ip, sizeof(ip));
        } else if (ndm->ndm_family == AF_INET6) {
            inet_ntop(AF_INET6, RTA_DATA(tb[NDA_DST]), ip, sizeof(ip));
        }
    }

    if (ip[0]) 
        printf("%s%s%s ", c_ip(), ip, c_reset());
        
    printf("dev %s%s%s ", c_ifname(), ifname[0] ? ifname : "?", c_reset());

    if (tb[NDA_LLADDR]) {
        unsigned char *mac = RTA_DATA(tb[NDA_LLADDR]);
        int maclen = RTA_PAYLOAD(tb[NDA_LLADDR]);

        printf("lladdr %s", c_mac());
        for (int i = 0; i < maclen; i++) {
            printf("%02x%s", mac[i], (i + 1 < maclen) ? ":" : "");
        }
        printf("%s ", c_reset());
    }

    if (ndm->ndm_flags & NTF_ROUTER)
        printf("router ");

    // Map common neighbor states to standard ip output
    const char *state_color = c_reset();
    if (use_color) {
        if (ndm->ndm_state & NUD_PERMANENT) state_color = "\033[32m";       // Green
        else if (ndm->ndm_state & NUD_REACHABLE) state_color = "\033[32m";  // Green
        else if (ndm->ndm_state & NUD_STALE) state_color = "\033[33m";      // Yellow
        else if (ndm->ndm_state & NUD_DELAY) state_color = "\033[33m";      // Yellow
        else if (ndm->ndm_state & NUD_PROBE) state_color = "\033[33m";      // Yellow
        else if (ndm->ndm_state & NUD_FAILED) state_color = "\033[31m";     // Red
        else if (ndm->ndm_state & NUD_INCOMPLETE) state_color = "\033[31m"; // Red
    }

    printf("%s", state_color);
    if (ndm->ndm_state & NUD_PERMANENT) printf("PERMANENT ");
    else if (ndm->ndm_state & NUD_NOARP) printf("NOARP ");
    else if (ndm->ndm_state & NUD_REACHABLE) printf("REACHABLE ");
    else if (ndm->ndm_state & NUD_STALE) printf("STALE ");
    else if (ndm->ndm_state & NUD_DELAY) printf("DELAY ");
    else if (ndm->ndm_state & NUD_PROBE) printf("PROBE ");
    else if (ndm->ndm_state & NUD_FAILED) printf("FAILED ");
    else if (ndm->ndm_state & NUD_INCOMPLETE) printf("INCOMPLETE ");
    else if (ndm->ndm_state == NUD_NONE) printf("NONE ");
    else printf("state 0x%x ", ndm->ndm_state);

    printf("%s\n", c_reset());
}

static void ip_neigh_show(const char *dev) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) { perror("socket"); return; }

    struct {
        struct nlmsghdr nlh;
        struct ndmsg ndm;
    } req = {0};

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg));
    req.nlh.nlmsg_type = RTM_GETNEIGH;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 3; // Distinct sequence ID

    req.ndm.ndm_family = af_filter;

    if (nl_send(sock, &req.nlh) < 0) {
        perror("sendmsg"); close(sock); return;
    }

    char buf[BUF_SIZE];

    while (1) {
        int len = recv(sock, buf, sizeof(buf), 0);
        if (len < 0) { perror("recv"); break; }

        struct nlmsghdr *nlh = (struct nlmsghdr *)buf;

        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_seq != 3) continue;

            if (nlh->nlmsg_type == NLMSG_DONE) {
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = NLMSG_DATA(nlh);
                if (err->error) errno = -err->error;
                perror("netlink");
                close(sock);
                return;
            }

            if (nlh->nlmsg_type == RTM_NEWNEIGH)
                parse_neigh(nlh, dev);
        }
    }

    close(sock);
}


static void ip_route_show(const char *dev);
static void ip_route_get(const char *dst, const char *src);

static int do_route(int argc, char **argv) {
    if (argc > 0 && strcmp(argv[0], "help") == 0) {
        fprintf(stderr, "Usage: %s route show [dev] [table TABLE] [proto PROTO] [scope SCOPE]\n", p_name);
        fprintf(stderr, "       %s route get ADDR [from ADDR]\n", p_name);
        return 0;
    }
    if (argc == 0 || strcmp(argv[0], "show") == 0 || strcmp(argv[0], "lst") == 0 || strcmp(argv[0], "list") == 0) {
        const char *dev = NULL;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "dev") == 0 && i + 1 < argc) {
                dev = argv[++i];
            } else if (strcmp(argv[i], "table") == 0 && i + 1 < argc) {
                route_table_filter = parse_table(argv[++i]);
            } else if (strcmp(argv[i], "proto") == 0 && i + 1 < argc) {
                route_proto_filter = parse_proto(argv[++i]);
            } else if (strcmp(argv[i], "scope") == 0 && i + 1 < argc) {
                route_scope_filter = parse_scope(argv[++i]);
            } else if (dev == NULL) {
                dev = argv[i];
            }
        }
        ip_route_show(dev);
        return 0;
    }
    if (strcmp(argv[0], "get") == 0) {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s route get ADDR [from ADDR]\n", p_name);
            return -1;
        }
        const char *dst = argv[1];
        const char *src = NULL;
        if (argc >= 4 && strcmp(argv[2], "from") == 0) {
            src = argv[3];
        }
        ip_route_get(dst, src);
        return 0;
    }
    fprintf(stderr, "Command \"%s\" is unknown, try \"%s route help\".\n", argv[0], p_name);
    return -1;
}

static void ip_neigh_show(const char *dev);

static int do_neigh(int argc, char **argv) {
    if (argc > 0 && strcmp(argv[0], "help") == 0) {
        fprintf(stderr, "Usage: %s neigh show [dev] [nud STATE] [to PREFIX]\n", p_name);
        return 0;
    }
    if (argc == 0 || strcmp(argv[0], "show") == 0 || strcmp(argv[0], "lst") == 0 || strcmp(argv[0], "list") == 0) {
        const char *dev = NULL;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "dev") == 0 && i + 1 < argc) {
                dev = argv[++i];
            } else if (strcmp(argv[i], "nud") == 0 && i + 1 < argc) {
                nud_filter |= parse_nud_state(argv[++i]);
            } else if (strcmp(argv[i], "to") == 0 && i + 1 < argc) {
                parse_prefix(argv[++i]);
            } else if (dev == NULL && strchr(argv[i], '.') == NULL && strchr(argv[i], ':') == NULL) {
                dev = argv[i];
            } else {
                parse_prefix(argv[i]);
            }
        }
        ip_neigh_show(dev);
        return 0;
    }
    fprintf(stderr, "Command \"%s\" is unknown, try \"%s neigh help\".\n", argv[0], p_name);
    return -1;
}

static void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS] OBJECT { COMMAND | help }\n", p_name);
    fprintf(stderr, "where  OBJECT := { link | addr | route | neigh }\n");
    fprintf(stderr, "       OPTIONS := { -4 | -6 | -c | -nc }\n");
}

static int do_help(int argc, char **argv) {
    usage();
    return 0;
}

// ===================== MAIN =====================

struct cmd {
    const char *cmd;
    int (*func)(int argc, char **argv);
};

static const struct cmd cmds[] = {
    { "address", do_addr },
    { "addr",    do_addr },
    { "link",    do_link },
    { "route",   do_route },
    { "neighbor", do_neigh },
    { "neigh",   do_neigh },
    { "help",    do_help },
    { NULL,      NULL }
};

int main(int argc, char *argv[]) {
    if (argc > 0) p_name = argv[0];
    int argi = 1;

    // Parse options: -4, -6, -c, --color, -nc, --no-color
    while (argi < argc) {
        if (strcmp(argv[argi], "-4") == 0) {
            af_filter = AF_INET;
            argi++;
        } else if (strcmp(argv[argi], "-6") == 0) {
            af_filter = AF_INET6;
            argi++;
        } else if (strcmp(argv[argi], "-c") == 0 || strcmp(argv[argi], "-color") == 0 || strcmp(argv[argi], "--color") == 0) {
            use_color = 1;
            argi++;
        } else if (strcmp(argv[argi], "-nc") == 0 || strcmp(argv[argi], "--no-color") == 0) {
            use_color = 0;
            argi++;
        } else if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0) {
            usage();
            return 0;
        } else {
            break;
        }
    }

    if (argi >= argc) {
        usage();
        return 1;
    }

    const char *object = argv[argi++];
    for (const struct cmd *c = cmds; c->cmd; c++) {
        if (strcmp(object, c->cmd) == 0) {
            return c->func(argc - argi, argv + argi);
        }
    }

    fprintf(stderr, "Object \"%s\" is unknown, try \"%s help\".\n", object, p_name);
    return 1;
}
