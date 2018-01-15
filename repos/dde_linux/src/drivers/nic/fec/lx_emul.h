/*
 * \brief  Freescale ethernet driver Linux emulation environment
 * \author Stefan Kalkowski
 * \date   2017-11-01
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

#ifndef _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_
#define _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_

#include <base/fixed_stdint.h>
#include <stdarg.h>

#include <lx_emul/extern_c_begin.h>

#include <lx_emul/compiler.h>
#include <lx_emul/printf.h>
#include <lx_emul/types.h>

typedef int clockid_t;

#define PAGE_SIZE 4096UL

enum { HZ = 100UL, };

struct list_head;

typedef __u16 __le16;
typedef __u32 __le32;
typedef __u64 __le64;
typedef __u16 __be16;
typedef __u32 __be32;
typedef __u64 __be64;
typedef __s64 time64_t;

#include <lx_emul/irq.h>
#include <lx_emul/kernel.h>
#include <lx_emul/jiffies.h>
#include <lx_emul/time.h>
#include <lx_emul/timer.h>
#include <lx_emul/mutex.h>
#include <lx_emul/bitops.h>
#include <lx_emul/atomic.h>
#include <lx_emul/work.h>
#include <lx_emul/spinlock.h>
#include <lx_emul/errno.h>
#include <lx_emul/string.h>
#include <lx_emul/module.h>
#include <lx_emul/bug.h>
#include <lx_emul/gfp.h>
#include <lx_emul/barrier.h>
#include <uapi/linux/swab.h>
#include <lx_emul/byteorder.h>
#include <lx_emul/completion.h>
#include <lx_emul/ioport.h>
#include <uapi/linux/net_tstamp.h>
#include <uapi/linux/ptp_clock.h>
#include <lx_emul/pm.h>

struct ethhdr { };

enum {
	ETH_HLEN     = 14,
	ETH_ALEN     = 6,      /* octets in one ethernet addr */
	ETH_P_8021Q  = 0x8100, /* 802.1Q VLAN Extended Header  */
	ETH_P_IP     = 0x0800,
	VLAN_HLEN    = 4,
};

#include <uapi/linux/ethtool.h>

#define dev_info(  dev, format, arg...) lx_printf("dev_info: "   format , ## arg)
#define dev_warn(  dev, format, arg...) lx_printf("dev_warn: "   format , ## arg)
#define dev_WARN(  dev, format, arg...) lx_printf("dev_WARN: "   format , ## arg)
#define dev_err(   dev, format, arg...) lx_printf("dev_error: "  format , ## arg)
#define dev_notice(dev, format, arg...) lx_printf("dev_notice: " format , ## arg)
#define dev_crit(  dev, format, arg...) lx_printf("dev_crit: "   format , ## arg)

#define pr_debug(fmt, ...)      printk(KERN_INFO fmt,   ##__VA_ARGS__)
#define pr_info(fmt, ...)       printk(KERN_INFO fmt,   ##__VA_ARGS__)
#define pr_err(fmt, ...)        printk(KERN_ERR fmt,    ##__VA_ARGS__)
#define pr_warn(fmt, ...)       printk(KERN_ERR fmt,    ##__VA_ARGS__)
#define pr_info_once(fmt, ...)  printk(KERN_INFO fmt,   ##__VA_ARGS__)
#define pr_notice(fmt, ...)     printk(KERN_NOTICE fmt, ##__VA_ARGS__)

#define netdev_err(dev, fmt, args...)  lx_printf("nedev_err:  " fmt, ##args)
#define netdev_warn(dev, fmt, args...) lx_printf("nedev_warn: " fmt, ##args)
#define netdev_info(dev, fmt, args...) lx_printf("nedev_info: " fmt, ##args)

struct completion {};

struct napi_struct
{
	struct net_device * dev;
};

#define writel(value, addr) (*(volatile uint32_t *)(addr) = (value))
#define readl(addr)         (*(volatile uint32_t *)(addr))

typedef u64 cycle_t;

struct timespec64 {
	time64_t tv_sec;  /* seconds */
	long     tv_nsec; /* nanoseconds */
};

extern struct timespec64 ns_to_timespec64(const s64 nsec);

static inline s64 timespec64_to_ns(const struct timespec64 *ts)
{
	return ((s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

#define ktime_to_ns(kt) ((kt).tv64)

ktime_t ns_to_ktime(u64 ns);

struct device_node {};

struct device_driver {
	const char * name;
	const struct of_device_id*of_match_table;
	const struct dev_pm_ops *pm;
};

struct device {
	struct device * parent;
	struct device_driver *driver;
	void * platform_data;
	void * driver_data;
	struct device_node *of_node;
};

struct platform_device_id {
	char name[20];
	unsigned long driver_data;
};

struct platform_device {
	const char * name;
	struct device dev;
	const struct platform_device_id * id_entry;
};

#define platform_get_device_id(pdev) ((pdev)->id_entry)

static inline void *platform_get_drvdata(const struct platform_device *pdev)
{
	return pdev->dev.driver_data;
}

void udelay(unsigned long usecs);

enum netdev_tx {
	NETDEV_TX_OK = 0x00,
	NETDEV_TX_BUSY = 0x10,
	NETDEV_TX_LOCKED = 0x20,
};
typedef enum netdev_tx netdev_tx_t;

struct sk_buff;
struct ifreq;

typedef u64 netdev_features_t;

struct net_device_ops {
	int (*ndo_open) (struct net_device *dev);
	int (*ndo_stop) (struct net_device *dev);
	netdev_tx_t (*ndo_start_xmit) (struct sk_buff *skb, struct net_device *dev);
	void (*ndo_set_rx_mode) (struct net_device *dev);
	int (*ndo_change_mtu) (struct net_device *dev, int new_mtu);
	int (*ndo_validate_addr) (struct net_device *dev);
	void (*ndo_tx_timeout) (struct net_device *dev);
	int (*ndo_set_mac_address)(struct net_device *dev, void *addr);
	int (*ndo_do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
	int (*ndo_set_features)(struct net_device *dev, netdev_features_t features);
};

struct net_device_stats {
	unsigned long rx_packets;
	unsigned long tx_packets;
	unsigned long rx_bytes;
	unsigned long tx_bytes;
	unsigned long collisions;
	unsigned long rx_errors;
	unsigned long tx_errors;
	unsigned long rx_dropped;
	unsigned long tx_dropped;
	unsigned long rx_length_errors;
	unsigned long rx_over_errors;
	unsigned long rx_crc_errors;
	unsigned long rx_frame_errors;
	unsigned long rx_fifo_errors;
	unsigned long rx_missed_errors;
	unsigned long tx_aborted_errors;
	unsigned long tx_carrier_errors;
	unsigned long tx_fifo_errors;
	unsigned long tx_heartbeat_errors;
	unsigned long tx_window_errors;
};

struct netdev_hw_addr {
	struct list_head list;
	unsigned char addr[32];
};

struct netdev_hw_addr_list {
	struct list_head list;
	int count;
};

struct net_device
{
	netdev_features_t features;
	struct net_device_stats stats;
	netdev_features_t hw_features;
	const struct net_device_ops *netdev_ops;
	const struct ethtool_ops *ethtool_ops;
	unsigned int flags;
	unsigned char addr_len;
	struct netdev_hw_addr_list mc;
	unsigned char *dev_addr;
	int watchdog_timeo;
	void *priv;
	struct device dev;
	u16 gso_max_segs;
};

static inline void *netdev_priv(const struct net_device *dev) { return dev->priv; }

#define CLOCKSOURCE_MASK(bits) (cycle_t)((bits) < 64 ? ((1ULL<<(bits))-1) : -1)

static inline u64 div_u64(u64 dividend, u32 divisor) { return dividend / divisor; }

struct pps_event_time {
	struct timespec64 ts_real;
};

size_t copy_from_user(void *to, void const *from, size_t len);
size_t copy_to_user(void *dst, void const *src, size_t len);

struct ifreq {
	void * ifr_data;
};

struct of_device_id {
	char compatible[128];
	const void *data;
};

int snprintf(char *buf, size_t size, const char *fmt, ...);

struct clk {};

unsigned long clk_get_rate(struct clk * clk);

#define module_param_array(macaddr, byte, arg1, arg2);

# define swab32p __swab32p
# define swab32s __swab32s

u64 local_clock(void);

#define do_div(n,base) ({ \
	unsigned long __base = (base); \
	unsigned long __rem; \
	__rem = ((uint64_t)(n)) % __base; \
	(n)   = ((uint64_t)(n)) / __base; \
	__rem; \
})

enum {
	MSEC_PER_SEC  = 1000L,
	USEC_PER_SEC  = MSEC_PER_SEC * 1000L,
};

static inline int rcu_read_lock_held(void) { return 1; }
static inline int rcu_read_lock_bh_held(void) { return 1; }

unsigned int jiffies_to_usecs(const unsigned long j);

struct rb_node {};

#define __aligned(x) __attribute__((aligned(x)))

#define kmemcheck_bitfield_begin(name)
#define kmemcheck_bitfield_end(name)

typedef __u32 __wsum;

enum { NUMA_NO_NODE = -1 };

struct ts_config {};

struct flow_dissector_key_control
{
	u16 thoff;
	u16 addr_type;
	u32 flags;
};

struct flow_keys
{
	struct flow_dissector_key_control control;
};

struct flow_dissector_key {};

struct flow_dissector {};

extern struct flow_dissector flow_keys_dissector;
extern struct flow_dissector flow_keys_buf_dissector;

struct flowi4 {};
struct flowi6 {};

__u32 __get_hash_from_flowi6(const struct flowi6 *fl6, struct flow_keys *keys);

bool flow_keys_have_l4(struct flow_keys *keys);

__u32 __get_hash_from_flowi4(const struct flowi4 *fl4, struct flow_keys *keys);

bool gfpflags_allow_blocking(const gfp_t gfp_flags);

struct lock_class_key { };

#define lockdep_set_class(lock, key)

struct page
{
	void *addr;
} __attribute((packed));

static inline struct page *compound_head(struct page *page) { return page; }

bool page_is_pfmemalloc(struct page *page);

void __free_page_frag(void *addr);

struct page *alloc_pages_node(int nid, gfp_t gfp_mask, unsigned int order);

void get_page(struct page *page);
void put_page(struct page *page);

static inline void *page_address(struct page *page) { return page->addr; };

struct page_frag
{
	struct page *page;
	__u16        offset;
	__u16        size;
};

enum dma_data_direction { DMA_FROM_DEVICE = 2 };

dma_addr_t dma_map_page(struct device *dev, struct page *page, size_t offset, size_t size, enum dma_data_direction dir);

void dma_sync_single_for_cpu(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir);

#define L1_CACHE_BYTES  32

struct iov_iter {};

size_t csum_and_copy_from_iter(void *addr, size_t bytes, __wsum *csum, struct iov_iter *i);

__wsum csum_block_add(__wsum csum, __wsum csum2, int offset);
__wsum csum_sub(__wsum csum, __wsum addend);
__wsum csum_partial(const void *buff, int len, __wsum sum);

typedef struct poll_table_struct { } poll_table;

size_t copy_from_iter(void *addr, size_t bytes, struct iov_iter *i);
size_t copy_to_iter(void *addr, size_t bytes, struct iov_iter *i);

struct socket {};

struct file {};

struct msghdr
{
	struct iov_iter msg_iter;
};

struct splice_pipe_desc {};

struct timespec ktime_to_timespec(const ktime_t kt);

typedef __u16 __sum16;

__sum16 csum_fold(__wsum csum);
__wsum csum_add(__wsum csum, __wsum addend);
__wsum remcsum_adjust(void *ptr, __wsum csum, int start, int offset);

#define htons(x) __cpu_to_be16(x)

struct iphdr {
	__u8 ihl:4;
	__u8 version:4;
	__sum16 check;
};

struct sk_buff;
struct iphdr *ip_hdr(const struct sk_buff *skb);

typedef unsigned short ushort;

dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size, enum dma_data_direction dir);

enum { DMA_TO_DEVICE = 1 };

int dma_mapping_error(struct device *dev, dma_addr_t dma_addr);

void dma_unmap_single(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir);

void dev_kfree_skb_any(struct sk_buff *);

int net_ratelimit(void);

unsigned int tcp_hdrlen(const struct sk_buff *skb);

struct netdev_queue *netdev_get_tx_queue(const struct net_device *dev, unsigned int index);
void netif_tx_stop_queue(struct netdev_queue *dev_queue);
void netif_tx_wake_queue(struct netdev_queue *dev_queue);
bool netif_queue_stopped(const struct net_device *dev);

#define CONFIG_ARCH_MXC 1

void rtnl_lock(void);
void rtnl_unlock(void);

int netif_device_present(struct net_device *);
int netif_running(const struct net_device *dev);
void netif_wake_queue(struct net_device *);
void netif_tx_lock_bh(struct net_device *dev);
void netif_tx_unlock_bh(struct net_device *dev);

void napi_enable(struct napi_struct *n);
void napi_disable(struct napi_struct *n);

static inline void __read_once_size(const volatile void *p, void *res, int size)
{
	switch (size) {
		case 1: *(__u8  *)res = *(volatile __u8  *)p; break;
		case 2: *(__u16 *)res = *(volatile __u16 *)p; break;
		case 4: *(__u32 *)res = *(volatile __u32 *)p; break;
		case 8: *(__u64 *)res = *(volatile __u64 *)p; break;
		default:
			barrier();
			__builtin_memcpy((void *)res, (const void *)p, size);
			barrier();
	}
}

#define READ_ONCE(x) \
({                                               \
	union { typeof(x) __val; char __c[1]; } __u; \
	__read_once_size(&(x), __u.__c, sizeof(x));  \
	__u.__val;                                   \
})

extern unsigned long find_next_bit(const unsigned long *addr, unsigned long
                                   size, unsigned long offset);
#define find_first_bit(addr, size) find_next_bit((addr), (size), 0)

#define prefetch(x)  __builtin_prefetch(x)

#define ntohs(x) __be16_to_cpu(x)

struct vlan_hdr
{
	__be16 h_vlan_TCI;
};

__be16 eth_type_trans(struct sk_buff *, struct net_device *);

void __vlan_hwaccel_put_tag(struct sk_buff *skb, __be16 vlan_proto, u16 vlan_tci);

enum gro_result {
	GRO_MERGED,
	GRO_MERGED_FREE,
	GRO_HELD,
	GRO_NORMAL,
	GRO_DROP,
};
typedef enum gro_result gro_result_t;

gro_result_t napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb);

void dma_sync_single_for_device(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir);

bool napi_schedule_prep(struct napi_struct *n);

void __napi_schedule(struct napi_struct *n);

void napi_complete(struct napi_struct *n);

void *dev_get_platdata(const struct device *dev);

int is_valid_ether_addr(const u8 *);

const void *of_get_mac_address(struct device_node *np);

void eth_hw_addr_random(struct net_device *dev);

int pm_runtime_get_sync(struct device *dev);

void reinit_completion(struct completion *x);

void pm_runtime_mark_last_busy(struct device *dev);

int pm_runtime_put_autosuspend(struct device *dev);

int clk_prepare_enable(struct clk *);

void clk_disable_unprepare(struct clk *);

struct phy_device *of_phy_connect(struct net_device *dev, struct device_node *phy_np, void (*hndlr)(struct net_device *), u32 flags, int iface);

const char *dev_name(const struct device *dev);

void *kmalloc(size_t size, gfp_t flags);
void kfree(const void *);
void *kzalloc(size_t size, gfp_t flags);

struct mii_bus;
struct device_node *of_get_child_by_name( const struct device_node *node, const char *name);
void of_node_put(struct device_node *node);
int of_mdiobus_register(struct mii_bus *mdio, struct device_node *np);

struct resource *platform_get_resource(struct platform_device *, unsigned, unsigned);

int ethtool_op_get_ts_info(struct net_device *, struct ethtool_ts_info *);

int  device_set_wakeup_enable(struct device *dev, bool enable);

bool device_may_wakeup(struct device *dev);

int enable_irq_wake(unsigned int irq);
int disable_irq_wake(unsigned int irq);

struct ethtool_ops {
	int(*get_settings)(struct net_device *, struct ethtool_cmd *);
	int(*set_settings)(struct net_device *, struct ethtool_cmd *);
	void(*get_drvinfo)(struct net_device *, struct ethtool_drvinfo *);
	int(*get_regs_len)(struct net_device *);
	void(*get_regs)(struct net_device *, struct ethtool_regs *, void *);
	void(*get_wol)(struct net_device *, struct ethtool_wolinfo *);
	int(*set_wol)(struct net_device *, struct ethtool_wolinfo *);
	int(*nway_reset)(struct net_device *);
	u32(*get_link)(struct net_device *);
	int(*get_coalesce)(struct net_device *, struct ethtool_coalesce *);
	int(*set_coalesce)(struct net_device *, struct ethtool_coalesce *);
	void(*get_pauseparam)(struct net_device *, struct ethtool_pauseparam*);
	int(*set_pauseparam)(struct net_device *, struct ethtool_pauseparam*);
	void(*get_strings)(struct net_device *, u32 stringset, u8 *);
	void(*get_ethtool_stats)(struct net_device *, struct ethtool_stats *, u64 *);
	int(*get_sset_count)(struct net_device *, int);
	int(*get_ts_info)(struct net_device *, struct ethtool_ts_info *);
	int(*get_tunable)(struct net_device *,
			       const struct ethtool_tunable *, void *);
	int(*set_tunable)(struct net_device *,
			       const struct ethtool_tunable *, const void *);
};

u32 ethtool_op_get_link(struct net_device *);

void *dma_alloc_coherent(struct device *, size_t, dma_addr_t *, gfp_t);
void dma_free_coherent(struct device *, size_t size, void *vaddr, dma_addr_t bus);

enum {
	SIOCSHWTSTAMP = 0x89b0,
	SIOCGHWTSTAMP = 0x89b1,
};

void netif_tx_start_all_queues(struct net_device *dev);

int pinctrl_pm_select_default_state(struct device *dev);

int pinctrl_pm_select_sleep_state(struct device *dev);

void netif_tx_disable(struct net_device *dev);

enum {
	IFF_PROMISC          = 0x100,
	IFF_ALLMULTI         = 0x200,
};

#include <lx_emul/list.h>

#define netdev_hw_addr_list_for_each(ha, l) \
	list_for_each_entry(ha, &(l)->list, list)

#define netdev_for_each_mc_addr(ha, dev) \
	netdev_hw_addr_list_for_each(ha, &(dev)->mc)

void netif_tx_wake_all_queues(struct net_device *);

struct sockaddr {
	unsigned short sa_family;
	char sa_data[14];
};

int eth_validate_addr(struct net_device *);

void *dmam_alloc_coherent(struct device *dev, size_t size, dma_addr_t *dma_handle, gfp_t gfp);

int eth_change_mtu(struct net_device *dev, int new_mtu);
int eth_validate_addr(struct net_device *dev);

void netif_napi_add(struct net_device *dev, struct napi_struct *napi, int (*poll)(struct napi_struct *, int), int weight);

bool of_device_is_available(const struct device_node *device);
int of_property_read_u32(const struct device_node *np, const char *propname, u32 *out_value);

enum { NAPI_POLL_WEIGHT = 64 };

#define SET_NETDEV_DEV(net, pdev)        ((net)->dev.parent = (pdev))

struct net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs, unsigned int rxqs);

void *devm_ioremap_resource(struct device *dev, struct resource *res);

const struct of_device_id *of_match_device(const struct of_device_id *matches, const struct device *dev);

const void *of_get_property(const struct device_node *node, const char *name, int *lenp);

void platform_set_drvdata(struct platform_device *pdev, void *data);

struct device_node *of_parse_phandle(const struct device_node *np, const char *phandle_name, int index);

int of_phy_register_fixed_link(struct device_node *np);
bool of_phy_is_fixed_link(struct device_node *np);
struct device_node *of_node_get(struct device_node *node);
int of_get_phy_mode(struct device_node *np);

struct clk *devm_clk_get(struct device *dev, const char *id);
struct regulator *__must_check devm_regulator_get(struct device *dev, const char *id);

void pm_runtime_set_autosuspend_delay(struct device *dev, int delay);
void pm_runtime_use_autosuspend(struct device *dev);
void pm_runtime_get_noresume(struct device *dev);
int  pm_runtime_set_active(struct device *dev);
void pm_runtime_enable(struct device *dev);

int regulator_enable(struct regulator *);

int platform_get_irq(struct platform_device *, unsigned int);

void netif_carrier_off(struct net_device *dev);

int register_netdev(struct net_device *);
void unregister_netdev(struct net_device *);

void free_netdev(struct net_device *);

int device_init_wakeup(struct device *dev, bool val);

int regulator_disable(struct regulator *r);

void *dev_get_drvdata(const struct device *dev);

void netif_device_attach(struct net_device *);
void netif_device_detach(struct net_device *dev);

int devm_request_irq(struct device *dev, unsigned int irq, irq_handler_t handler, unsigned long irqflags, const char *devname, void *dev_id);

#define SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#define SET_RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn)

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
};

#define module_platform_driver(x)

#include <lx_emul/extern_c_end.h>

#endif /* _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_ */
