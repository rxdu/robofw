/* 
 * main.c
 * 
 * Created on: Feb 24, 2021 11:33
 * Description: 
 * 
 * Copyright (c) 2021 Weston Robot
 */ 
#include <logging/log.h>
LOG_MODULE_REGISTER(net_socket_can_sample, LOG_LEVEL_DBG);

#include <zephyr.h>

#include <net/socket.h>
#include <net/socket_can.h>

#define PRIORITY k_thread_priority_get(k_current_get())
#define STACKSIZE 1024
#define SLEEP_PERIOD K_SECONDS(1)

#define CLOSE_PERIOD 15

static const struct zcan_filter zfilter = {
	.id_type = CAN_EXTENDED_IDENTIFIER,
	.rtr = CAN_DATAFRAME,
	// .std_id = 0x1,
	.rtr_mask = 1,
	.std_id_mask = 0x000, //CAN_STD_ID_MASK
};

static struct can_filter filter;

struct net_if *net_if_get_second_by_type(const struct net_l2 *l2)
{
	int count = 0;
	Z_STRUCT_SECTION_FOREACH(net_if, iface)
	{
		if (IS_ENABLED(CONFIG_NET_OFFLOAD) && !l2 &&
		    net_if_offload(iface)) {
			return iface;
		}
		if (net_if_l2(iface) == l2) {
			if (count++ == 1)
				return iface;
		}
	}

	return NULL;
}

static int create_socket(int i)
{
	struct sockaddr_can can_addr;
	int fd, ret;

	fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
	if (fd < 0) {
		printk("ERROR: Cannot create CAN socket (%d)\n", fd);
		return fd;
	}

	struct net_if *iface;
	if (i == 0) {
		iface = net_if_get_first_by_type(&NET_L2_GET_NAME(CANBUS_RAW));
	} else {
		iface = net_if_get_second_by_type(&NET_L2_GET_NAME(CANBUS_RAW));
	}
	if (!iface) {
		printk("ERROR: No CANBUS network interface found!\n");
		return -ENOENT;
	}

	can_addr.can_ifindex = net_if_get_by_iface(iface);
	can_addr.can_family = PF_CAN;

	ret = bind(fd, (struct sockaddr *)&can_addr, sizeof(can_addr));
	if (ret < 0) {
		printk("ERROR: Cannot bind %s CAN socket (%d)\n", "2nd",
		       -errno);
		(void)close(fd);
		return ret;
	}

	printk("INFO: CAN socket created (fd: %d)\n", fd);

	return fd;
}

static void tx_task(int *can_fd)
{
	int fd = POINTER_TO_INT(can_fd);
	struct zcan_frame msg = { 0 };
	struct can_frame frame = { 0 };
	int ret, i;

	msg.dlc = 8U;
	msg.id_type = CAN_STANDARD_IDENTIFIER;
	msg.std_id = 0x1;
	msg.rtr = CAN_DATAFRAME;

	for (i = 0; i < msg.dlc; i++) {
		msg.data[i] = 0xF0 | i;
	}

	can_copy_zframe_to_frame(&msg, &frame);

	printk("INFO: TX task running ... [%d]\n", fd);

	while (1) {
		ret = send(fd, &frame, sizeof(frame), 0);
		if (ret < 0) {
			printk("ERROR: Cannot send CAN message (%d)\n", -errno);
		}

		k_sleep(SLEEP_PERIOD);
	}
}

static void rx_task(int *can_fd, int *do_close_period,
		    const struct can_filter *filter)
{
	int close_period = POINTER_TO_INT(do_close_period);
	int fd = POINTER_TO_INT(can_fd);
	struct sockaddr_can can_addr;
	socklen_t addr_len;
	struct zcan_frame msg;
	struct can_frame frame;
	volatile int ret;

	printk("INFO: RX task running ... [%d]\n", fd);

	while (1) {
		uint8_t *data;

		memset(&frame, 0, sizeof(frame));
		addr_len = sizeof(can_addr);

		ret = recvfrom(fd, &frame, sizeof(struct can_frame), 0,
			       (struct sockaddr *)&can_addr, &addr_len);
		if (ret < 0) {
			printk("ERROR: [%d] Cannot receive CAN message (%d)\n",
			       fd, -errno);
			continue;
		}

		can_copy_frame_to_zframe(&frame, &msg);

		printk("INFO: [%d] CAN msg: type 0x%x RTR 0x%x EID 0x%x DLC 0x%x\n",
		       fd, msg.id_type, msg.rtr, msg.ext_id, msg.dlc);
		printk("INFO: [%d] CAN data: ", fd);
		for (int i = 0; i < msg.dlc; ++i)
			printk("%x ", msg.data[i]);
		printk("\n");

		if (!msg.rtr) {
			if (msg.dlc > 8) {
				data = (uint8_t *)msg.data_32;
			} else {
				data = msg.data;
			}

			LOG_HEXDUMP_INF(data, msg.dlc, "Data");
		} else {
			printk("INFO: [%d] EXT Remote message received\n", fd);
		}

		// if (POINTER_TO_INT(do_close_period) > 0) {
		// 	close_period--;
		// 	if (close_period <= 0) {
		// 		(void)close(fd);

		// 		k_sleep(K_SECONDS(1));

		// 		fd = create_socket(filter);
		// 		if (fd < 0) {
		// 			printk("ERROR: Cannot get socket (%d)\n",
		// 			       -errno);
		// 			return;
		// 		}

		// 		close_period = POINTER_TO_INT(do_close_period);
		// 	}
		// }
	}
}

static k_tid_t tx_tid1;
static K_THREAD_STACK_DEFINE(tx_stack1, STACKSIZE);
static struct k_thread tx_data1;

static k_tid_t tx_tid2;
static K_THREAD_STACK_DEFINE(tx_stack2, STACKSIZE);
static struct k_thread tx_data2;

int setup_tx_task1()
{
	int ret = -1;

	int fd = create_socket(0);
	if (fd < 0) {
		printk("Failed to create socket for tx task (%d)\n", fd);
		return -1;
	}

	tx_tid1 = k_thread_create(&tx_data1, tx_stack1,
				  K_THREAD_STACK_SIZEOF(tx_stack1),
				  (k_thread_entry_t)tx_task, INT_TO_POINTER(fd),
				  NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	if (!tx_tid1) {
		ret = -ENOENT;
		errno = -ret;
		printk("Cannot create TX thread!\n");
		(void)close(fd);
	}

	printk("Started socket CAN TX thread\n");

	return 0;
}

int setup_tx_task2()
{
	int ret = -1;

	int fd = create_socket(1);
	if (fd < 0) {
		printk("Failed to create socket for tx task (%d)\n", fd);
		return -1;
	}

	tx_tid2 = k_thread_create(&tx_data2, tx_stack2,
				  K_THREAD_STACK_SIZEOF(tx_stack2),
				  (k_thread_entry_t)tx_task, INT_TO_POINTER(fd),
				  NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	if (!tx_tid2) {
		ret = -ENOENT;
		errno = -ret;
		printk("Cannot create TX thread!\n");
		(void)close(fd);
	}

	printk("Started socket CAN TX thread\n");

	return 0;
}

static k_tid_t rx_tid1;
static K_THREAD_STACK_DEFINE(rx_stack1, STACKSIZE);
static struct k_thread rx_data1;

static k_tid_t rx_tid2;
static K_THREAD_STACK_DEFINE(rx_stack2, STACKSIZE);
static struct k_thread rx_data2;

int setup_rx_task1()
{
	int ret = -1;

	int fd = create_socket(0);
	if (fd < 0) {
		printk("Failed to create socket for rx task (%d)\n", fd);
		return -1;
	}

	// set filter
	can_copy_zfilter_to_filter(&zfilter, &filter);
	ret = setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter,
			 sizeof(filter));
	if (ret < 0) {
		ret = -errno;
		printk("ERROR: Cannot set CAN sockopt (%d)\n", ret);
		close(fd);
		return ret;
	}

	rx_tid1 = k_thread_create(&rx_data1, rx_stack1,
				  K_THREAD_STACK_SIZEOF(rx_stack1),
				  (k_thread_entry_t)rx_task, INT_TO_POINTER(fd),
				  INT_TO_POINTER(CLOSE_PERIOD), &filter,
				  PRIORITY, 0, K_NO_WAIT);
	if (!rx_tid1) {
		ret = -ENOENT;
		errno = -ret;
		printk("Cannot create RX thread!\n");
		(void)close(fd);
	}

	printk("Started socket CAN RX thread\n");

	return 0;
}

int setup_rx_task2()
{
	int ret = -1;

	int fd = create_socket(1);
	if (fd < 0) {
		printk("Failed to create socket for rx task (%d)\n", fd);
		return -1;
	}

	// set filter
	can_copy_zfilter_to_filter(&zfilter, &filter);
	ret = setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter,
			 sizeof(filter));
	if (ret < 0) {
		ret = -errno;
		printk("ERROR: Cannot set CAN sockopt (%d)\n", ret);
		close(fd);
		return ret;
	}

	rx_tid2 = k_thread_create(&rx_data2, rx_stack2,
				  K_THREAD_STACK_SIZEOF(rx_stack2),
				  (k_thread_entry_t)rx_task, INT_TO_POINTER(fd),
				  INT_TO_POINTER(CLOSE_PERIOD), &filter,
				  PRIORITY, 0, K_NO_WAIT);
	if (!rx_tid2) {
		ret = -ENOENT;
		errno = -ret;
		printk("Cannot create RX thread!\n");
		(void)close(fd);
	}

	printk("Started socket CAN RX thread\n");

	return 0;
}

void main(void)
{
	printk("Running app with board: %s\n", CONFIG_BOARD);

	/* Let the device start before doing anything */
	k_sleep(K_SECONDS(2));

	/* Create TX and RX tasks */
	(void)setup_tx_task1();
	(void)setup_tx_task2();
	(void)setup_rx_task1();
	(void)setup_rx_task2();
}
