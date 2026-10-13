#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#define DEFAULT_PORT 4040
struct{
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, __u16);
}config_map SEC(".maps");
SEC("cgroup/connect4")
int process_connect4(struct bpf_sock_addr *ctx)
{
	__u32 key =0;
	__u16 *allowed_port =  bpf_map_lookup_elem(&config_map, &key);
	if (!allowed_port)
		return 0;
	if (*allowed_port ==0)
		return 0;
	if (ctx->user_port != bpf_htons(*allowed_port))
		return 0;
	return 1;
}
char LICENSE[] SEC("license") = "GPL";

