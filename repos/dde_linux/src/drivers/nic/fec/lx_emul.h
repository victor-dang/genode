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

typedef unsigned long atomic_long_t;
typedef int clockid_t;

enum {
	HZ        = 100UL,
	PAGE_SIZE = 4096UL
};

struct list_head;
void INIT_LIST_HEAD(struct list_head *list);

#include <lx_emul/compiler.h>
#include <lx_emul/printf.h>
#include <lx_emul/types.h>
#include <lx_emul/irq.h>
#include <lx_emul/kernel.h>
#include <lx_emul/jiffies.h>
#include <lx_emul/time.h>
#include <lx_emul/timer.h>
#include <lx_emul/mutex.h>
#include <lx_emul/bitops.h>
#include <lx_emul/work.h>
#include <lx_emul/spinlock.h>
#include <lx_emul/errno.h>
#include <lx_emul/string.h>
#include <lx_emul/module.h>

typedef __s64 time64_t;

typedef enum {
	/*PHY_INTERFACE_MODE_NA,
	PHY_INTERFACE_MODE_MII,
	PHY_INTERFACE_MODE_GMII,
	PHY_INTERFACE_MODE_SGMII,
	PHY_INTERFACE_MODE_TBI,
	PHY_INTERFACE_MODE_REVMII,
	PHY_INTERFACE_MODE_RMII,
	PHY_INTERFACE_MODE_RGMII,
	PHY_INTERFACE_MODE_RGMII_ID,
	PHY_INTERFACE_MODE_RGMII_RXID,
	PHY_INTERFACE_MODE_RGMII_TXID,
	PHY_INTERFACE_MODE_RTBI,
	PHY_INTERFACE_MODE_SMII,
	PHY_INTERFACE_MODE_XGMII,
	PHY_INTERFACE_MODE_MOCA,
	PHY_INTERFACE_MODE_QSGMII,
	PHY_INTERFACE_MODE_MAX,*/
	DUMMY
} phy_interface_t;

#include <lx_emul/completion.h>

#include <uapi/linux/net_tstamp.h>
#include <uapi/linux/ptp_clock.h>

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

struct completion {};

struct napi_struct {};

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

struct device {
	void * driver_data;
};

struct platform_device_id {
	char name[20];
	unsigned long driver_data;
};

struct platform_device {
	struct device dev;
	const struct platform_device_id * id_entry;
};

#define platform_get_device_id(pdev) ((pdev)->id_entry)

static inline void *platform_get_drvdata(const struct platform_device *pdev)
{
	return pdev->dev.driver_data;
}

void udelay(unsigned long usecs);

struct net_device
{
	void *priv;
};

static inline void *netdev_priv(const struct net_device *dev) { return dev->priv; }

#define CLOCKSOURCE_MASK(bits) (cycle_t)((bits) < 64 ? ((1ULL<<(bits))-1) : -1)

static inline u64 div_u64(u64 dividend, u32 divisor) { return dividend / divisor; }

struct pps_event_time {
	struct timespec64 ts_real;
};

size_t copy_from_user(void *to, void const *from, size_t len);
size_t copy_to_user(void *dst, void const *src, size_t len);

/*struct sockaddr {
	unsigned short sa_family;
	char           sa_data[14];
};*/

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

#include <lx_emul/extern_c_end.h>

#endif /* _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_ */

