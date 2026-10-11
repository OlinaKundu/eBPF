#include <linux/in.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>
#include <bpf/bpf_endian.h>
#include <linux/ip.h>
#define FILTER_PORT 4040

struct{
	__uint(type,BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, __u16);
}config_map SEC(".maps");

SEC("xdp")
int xdp_filter(struct xdp_md *ctx)
{
	__u32 key=0;
	__u16 *port =bpf_map_lookup_elem(&config_map, &key);
	if (!port)
		return XDP_PASS;
	void *data = (void *)(long)ctx->data;
	void *data_end = (void *)(long)ctx->data_end;
	struct ethhdr *eth=data;
	if ((void *)(eth + 1) > data_end)
		return XDP_PASS;
	if(eth->h_proto != __constant_htons(ETH_P_IP))
		return XDP_PASS;
	struct iphdr *ip = (void *)(eth +1);
	if ((void *)(ip + 1) > data_end)
		return XDP_PASS;
	if (ip->version !=4 || ip->ihl < 5)
		return XDP_PASS;
	__u32 ip_header_len = ip->ihl * 4;
	if ((void *)ip + ip_header_len >data_end)
		return XDP_PASS;
	if (ip->tot_len < ip_header_len)
		return XDP_PASS;
	if ((void *)ip + bpf_ntohs(ip->tot_len) > data_end)
		return XDP_PASS;
	if (ip->protocol != IPPROTO_TCP)
		return XDP_PASS;
	struct tcphdr *tcp = (void *)ip + ip->ihl * 4;
	if ((void *)(tcp + 1) > data_end)
		return XDP_PASS;
	if (tcp->doff < 5)
		return XDP_PASS;
	if ((void *)tcp + tcp ->doff * 4 > data_end)
		return XDP_PASS;
	if (tcp->dest == bpf_htons(*port))
		return XDP_DROP;
	return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
