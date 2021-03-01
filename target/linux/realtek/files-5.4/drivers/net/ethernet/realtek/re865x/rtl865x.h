/*
 *  Realtek RTL865X built-in switch driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Based on Realtek RE865X asic driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __RTL865X_H
#define __RTL865X_H

#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/switch.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/regmap.h>

#define RTL865X_MAX_PHYS						5
#define RTL865X_MAX_PORTS						6
#define RTL865X_NUM_PHY_PORTS						RTL865X_MAX_PHYS

#ifdef CONFIG_SOC_RTL8197D
#define RTL865X_MAX_VLANS						2048
#else
#define RTL865X_MAX_VLANS						4096
#endif

#define RTL819X_SYSC_REG_CLK_MANAGE		0x10
#define CM_PROTECT						BIT(27)
#define CM_ACTIVE_SWCORE				BIT(11)

#define RTL819X_SYSC_REG_PIN_MUX_SEL	0x40
#define PMS_P0MDIO_SHIFT				3
#define PMS_P0MDIO_MASK					0x3
#define PMS_P0MDIO_MDIO					0x0
#define PMS_P0MDIO_GPIO					0x3
#define PMS_FCS1N_SHIFT					12
#define PMS_FCS1N_MASK					0x3
#define PMS_FCS1N_SF_CS1				0x0
#define PMS_FCS1N_NAND_CE				0x1
#define PMS_FCS1N_GPIOA1				0x3

struct rtl865x_vlan_cache {
	u16 vid;
	u8 members;
	u8 untag_members;
};

struct rtl865x {
	struct device *parent;
	struct net_device *dev;
	struct mii_bus *mii_sw;
	struct switch_dev swdev;

	void __iomem 	*swcore_base;
	void __iomem 	*swtbl_base;
#if defined(CONFIG_SOC_RTL8197D)
	struct regmap	*sysctl;
#endif

	//int mii_irq[PHY_MAX_ADDR];

	int num_ports;
	bool vlan_enabled;
	s16 vlan_id[RTL865X_MAX_VLANS];
	u8 vlan_members[RTL865X_MAX_VLANS];
	u8 vlan_untag_members[RTL865X_MAX_VLANS];
	u16 pvid[RTL865X_MAX_PORTS + 1];

	int num_vlan_caches;
	struct rtl865x_vlan_cache vlan_caches[RTL865X_MAX_VLANS];
};

int rtl865x_switch_probe(struct rtl865x *rsw);
int rtl865x_switch_remove(struct rtl865x *rsw);

bool rtl865x_get_vlan_port_members(struct rtl865x *rsw, u16 vid, u8 *pmembers, u8 *puntag_members);
bool rtl865x_cpu_port_is_tagged(struct rtl865x *rsw, u16 vid);

void rtl865x_port_init(struct rtl865x *rsw, struct device_node *np);

#define RTL865X_SW_MIB_BASE						0x1000
#define RTL865X_SW_GLOBAL_PORT_CTRL_BASE				0x4000
#define RTL865X_SW_PORT_CONFIG_BASE					0x4100
#define RTL865X_SW_MISC_BASE						0x4200
#define RTL865X_SW_ALE_BASE						0x4400
#define RTL865X_SW_VLAN_BASE						0x4a00
#define RTL865X_SW_TABLE_ACC_BASE					0x4d00


#define RTL865X_SW_REG_MIB_CONTROL					(RTL865X_SW_MIB_BASE + 0x00)

#define RTL865X_SW_SYS_COUNTER_RESTART					(1 << 18)
#define RTL865X_SW_PORT_IN_COUNTER_RESTART(p)				BIT((p) * 2 + 1)
#define RTL865X_SW_PORT_OUT_COUNTER_RESTART(p)				BIT((p) * 2)

#define RTL865X_SW_ALL_PORT_COUNTER_RESTART				\
		(RTL865X_SW_SYS_COUNTER_RESTART |			\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(0) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(0) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(1) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(1) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(2) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(2) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(3) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(3) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(4) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(4) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(5) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(5) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(6) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(6) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(7) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(7) |		\
		 RTL865X_SW_PORT_OUT_COUNTER_RESTART(8) |		\
		 RTL865X_SW_PORT_IN_COUNTER_RESTART(8))


#define RTL865X_SW_PORT_MIB_SCALE					0x80
#define RTL865X_SW_REG_PORT_MIB(p, r)					(RTL865X_SW_MIB_BASE + (p) * RTL865X_SW_PORT_MIB_SCALE + (r))

#define RX_BYTES_LO							0x100
#define RX_BYTES_HI							0x104
#define RX_UNICAST_PACKETS						0x108
#define RX_MULTICAST_PACKETS						0x13c
#define RX_BROADCAST_PACKETS						0x140

#define TX_BYTES_LO							0x800
#define TX_BYTES_HI							0x804
#define TX_UNICAST_PACKETS						0x808
#define TX_MULTICAST_PACKETS						0x80c
#define TX_BROADCAST_PACKETS						0x810


#define RTL865X_SW_REG_CHIP_ID						(RTL865X_SW_MISC_BASE + 0x00)

#define RTL865X_SW_CHIP_ID_SHIFT					16
#define RTL865X_SW_CHIP_ID_MASK						0xffff

#define RTL865X_SW_CHIP_VER_SHIFT					0
#define RTL865X_SW_CHIP_VER_MASK					0xffff


#define RTL865X_SW_REG_CHIP_ID_MGMT					(RTL865X_SW_MISC_BASE + 0x08)

#define RTL865X_SW_CHIP_PROJECT_ID_SHIFT				16
#define RTL865X_SW_CHIP_PROJECT_ID_MASK					0xffff


#define RTL865X_SW_REG_RESET						(RTL865X_SW_MISC_BASE + 0x04)

#define RTL865X_SW_RESET_FULL						BIT(2)
#define RTL865X_SW_RESET_SEMI						BIT(1)
#define RTL865X_SW_TRX_READY						BIT(0)


#define RTL865X_SW_REG_MAC_CONFIG					(RTL865X_SW_GLOBAL_PORT_CTRL_BASE + 0x00)

#define RTL865X_SW_SELIPG_SHIFT						18
#define RTL865X_SW_SELIPG_MASK						0x3
#define RTL865X_SW_SELIPG_7						0
#define RTL865X_SW_SELIPG_8						1
#define RTL865X_SW_SELIPG_11						2
#define RTL865X_SW_SELIPG_12						3

#define RTL865X_SW_CF_RXIPG_SHIFT					0
#define RTL865X_SW_CF_RXIPG_MASK					0xf


#define RTL865X_SW_REG_MDIO_CMD						(RTL865X_SW_GLOBAL_PORT_CTRL_BASE + 0x04)

#define RTL865X_SW_MDIO_CMD_WRITE					BIT(31)

#define RTL865X_SW_MDIO_PHY_ADDR_SHIFT					24
#define RTL865X_SW_MDIO_PHY_ADDR_MASK					0x1f

#define RTL865X_SW_MDIO_REG_ADDR_SHIFT					16
#define RTL865X_SW_MDIO_REG_ADDR_MASK					0x1f

#define RTL865X_SW_MDIO_WRITE_DATA_SHIFT				0
#define RTL865X_SW_MDIO_WRITE_DATA_MASK					0xffff


#define RTL865X_SW_REG_MDIO_STATUS					(RTL865X_SW_GLOBAL_PORT_CTRL_BASE + 0x08)

#define RTL865X_SW_MDIO_STATUS_BUSY					BIT(31)

#define RTL865X_SW_MDIO_READ_DATA_SHIFT					0
#define RTL865X_SW_MDIO_READ_DATA_MASK					0xffff


#define RTL865X_SW_REG_PORT_CONFIG(p)					(RTL865X_SW_PORT_CONFIG_BASE + 0x04 + (p) * 4)

#define RTL865X_SW_96C_98_PORT_EXT_PHY_ID_SHIFT				26
#define RTL865X_SW_96C_98_PORT_EXT_PHY_ID_MASK				0x1f

#define RTL865X_SW_96C_98_PORT_FORCED_MODE				BIT(25)

#define RTL865X_SW_PORT_EXT_PHY_ID_SHIFT				24
#define RTL865X_SW_PORT_EXT_PHY_ID_MASK					0x1f

#define RTL865X_SW_96C_98_PORT_MAC_RESET_L				BIT(3)

#define RTL865X_SW_PORT_PACKET_LENGTH_SHIFT				1
#define RTL865X_SW_PORT_PACKET_LENGTH_MASK				0x3
#define RTL865X_SW_PORT_PACKET_LENGTH_1536				0
#define RTL865X_SW_PORT_PACKET_LENGTH_1552				1
#define RTL865X_SW_PORT_PACKET_LENGTH_9K				2
#define RTL865X_SW_PORT_PACKET_LENGTH_16K				3

#define RTL865X_SW_PORT_ENABLE_PHY_IF					BIT(0)


#define RTL865X_SW_REG_PORT_STATUS(p)					(RTL865X_SW_PORT_CONFIG_BASE + 0x28 + (p) * 4)

#define RTL865X_SW_PORT_LINK_DOWN_EVENT					BIT(8)
#define RTL865X_SW_PORT_ANEG_ENABLED					BIT(7)
#define RTL865X_SW_PORT_RX_PAUSE					BIT(6)
#define RTL865X_SW_PORT_TX_PAUSE					BIT(5)
#define RTL865X_SW_PORT_LINK_UP						BIT(4)
#define RTL865X_SW_PORT_DUPLEX						BIT(3)

#define RTL865X_SW_PORT_SPEED_SHIFT					0
#define RTL865X_SW_PORT_SPEED_MASK					0x3
#define RTL865X_SW_PORT_SPEED_10M					0
#define RTL865X_SW_PORT_SPEED_100M					1
#define RTL865X_SW_PORT_SPEED_1000M					2


#define RTL865X_SW_REG_MODULE_SWITCH_CONTROL				(RTL865X_SW_ALE_BASE + 0x10)

#define RTL865X_SW_STP_ENABLE						BIT(5)
#define RTL865X_SW_INGRESS_ACL_ENABLE					BIT(4)
#define RTL865X_SW_EGRESS_ACL_ENABLE					BIT(3)
#define RTL865X_SW_L4_ENGINE_ENABLE					BIT(2)
#define RTL865X_SW_L3_ENGINE_ENABLE					BIT(1)
#define RTL865X_SW_L2_ENGINE_ENABLE					BIT(0)



#define RTL865X_SW_REG_TABLE_CONTROL_0					(RTL865X_SW_ALE_BASE + 0x18)

#define RTL865X_SW_TLU_STOPPED						BIT(19)
#define RTL865X_SW_TLU_STOP						BIT(18)

#define RTL865X_SW_LAN_DECISION_POLICY_SHIFT				16
#define RTL865X_SW_LAN_DECISION_POLICY_MASK				0x3
#define RTL865X_SW_LAN_DECISION_POLICY_BY_VLAN				0
#define RTL865X_SW_LAN_DECISION_POLICY_BY_PORT				1
#define RTL865X_SW_LAN_DECISION_POLICY_BY_MAC				2

#define RTL865X_SW_UNKNOWN_VLAN_FRAME_TO_CPU				BIT(15)
#define RTL865X_SW_UNKNOWN_NAPT_FRAME_TO_CPU				BIT(14)


#define RTL865X_SW_REG_FRAME_FORWARDING_CONFIG				(RTL865X_SW_ALE_BASE + 0x28)

#define RTL865X_SW_UNICAST_TO_CPU					BIT(1)
#define RTL865X_SW_MULTICAST_TO_CPU					BIT(0)


#define RTL865X_SW_REG_VLAN_CONTROL_0					(RTL865X_SW_VLAN_BASE + 0x00)

#define RTL865X_SW_IGNORE_VLAN_TAG					BIT(31)

#define RTL965X_SW_PORT_VLAN_FILTER_TYPE_SHIFT(p)			(9 + (p) * 2)
#define RTL965X_SW_PORT_VLAN_FILTER_TYPE_MASK				0x3
#define RTL965X_SW_PORT_VLAN_FILTER_TYPE_ALL				0
#define RTL965X_SW_PORT_VLAN_FILTER_TYPE_TAGGED				1
#define RTL965X_SW_PORT_VLAN_FILTER_TYPE_UNTAGGED			2

#define RTL965X_SW_VLAN_INGRESS_FILTER_EN(p)				BIT(p)
#define RTL965X_SW_VLAN_INGRESS_FILTER_ALL_EN_MASK			0x1ff


#define RTL865X_SW_REG_PORT_VLAN_CONFIG(p)				(RTL865X_SW_VLAN_BASE + 0x08 + (p) * 4)
#define RTL865X_SW_REG_PORT_VLAN_CONFIG_1_0				(RTL865X_SW_VLAN_BASE + 0x08)
#define RTL865X_SW_REG_PORT_VLAN_CONFIG_3_2				(RTL865X_SW_VLAN_BASE + 0x0c)
#define RTL865X_SW_REG_PORT_VLAN_CONFIG_5_4				(RTL865X_SW_VLAN_BASE + 0x10)
#define RTL865X_SW_REG_PORT_VLAN_CONFIG_7_6				(RTL865X_SW_VLAN_BASE + 0x14)

#define RTL865X_SW_PORT_PRIORITY_ODD_SHIFT				28
#define RTL865X_SW_PORT_PRIORITY_ODD_MASK				0x7

#define RTL865X_SW_PORT_VLAN_ODD_SHIFT					16
#define RTL865X_SW_PORT_VLAN_ODD_MASK					0xfff

#define RTL865X_SW_PORT_PRIORITY_EVEN_SHIFT				12
#define RTL865X_SW_PORT_PRIORITY_EVEN_MASK				0x7

#define RTL865X_SW_PORT_VLAN_EVEN_SHIFT					0
#define RTL865X_SW_PORT_VLAN_EVEN_MASK					0xfff


#define RTL865X_SW_REG_TABLE_ACC_CONTROL				(RTL865X_SW_TABLE_ACC_BASE + 0x00)

#define RTL865X_SW_TABLE_CMD_FORCE					BIT(3)
#define RTL865X_SW_TABLE_CMD_MODIFY					BIT(2)
#define RTL865X_SW_TABLE_CMD_ADD					BIT(1)
#define RTL865X_SW_TABLE_ACTION						BIT(0)


#define RTL865X_SW_REG_TABLE_ACC_STATUS					(RTL865X_SW_TABLE_ACC_BASE + 0x04)

#define RTL865X_SW_TABLE_FAIL						BIT(0)


#define RTL865X_SW_REG_TABLE_ACC_ADDRESS				(RTL865X_SW_TABLE_ACC_BASE + 0x08)


#define RTL865X_SW_REG_TABLE_ACC_DATA(x)				(RTL865X_SW_TABLE_ACC_BASE + 0x20 + (x) * 4)


#define RTL865X_LUT_TABLE_NUM_DATA_REGS					8
#define RTL865X_LUT_TABLE_SIZE						(RTL865X_LUT_TABLE_NUM_DATA_REGS * sizeof (u32))


#define RTL865X_TABLE_SIZE						0x10000


enum rtl865x_table_type
{
	TYPE_L2_SWITCH_TABLE = 0,
	TYPE_ARP_TABLE,
	TYPE_L3_ROUTING_TABLE,
	TYPE_MULTICAST_TABLE,
	TYPE_NETINTERFACE_TABLE,
	TYPE_EXT_INT_IP_TABLE,
	TYPE_VLAN_TABLE,
	TYPE_VLAN1_TABLE,
	TYPE_SERVER_PORT_TABLE,
	TYPE_L4_TCP_UDP_TABLE,
	TYPE_L4_ICMP_TABLE,
	TYPE_PPPOE_TABLE,
	TYPE_ACL_RULE_TABLE,
	TYPE_NEXT_HOP_TABLE,
	TYPE_RATE_LIMIT_TABLE,
	TYPE_ALG_TABLE,
};

union rtl865x_table_vlan
{
	u32 data[8];
	struct
	{
		u32 reserved_bits : 12;
		u32 fid : 2;
		u32 ext_egress_untag : 3;
		u32 egress_untag : 6;
		u32 ext_member_port : 3;
		u32 member_port : 6;

		u32 reserved[7];
	};
};

static inline u32 rtl865x_lut_address(struct rtl865x *rsw, enum rtl865x_table_type type)
{
	return (size_t) rsw->swtbl_base + type * RTL865X_TABLE_SIZE;
}

static inline u32 rtl865x_vlan_table_address(struct rtl865x *rsw, u32 vid)
{
	return rtl865x_lut_address(rsw, TYPE_VLAN_TABLE) + vid * RTL865X_LUT_TABLE_SIZE;
}

#endif /* __RTL865X_H */
