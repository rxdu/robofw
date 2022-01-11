/*
 * can.c
 *
 * Created on: Feb 11, 2021 09:51
 * Description:
 *
 * Copyright (c) 2021 Weston Robot
 */

#include "driver/interface.h"

#include <stdio.h>
#include <strings.h>

#include <net/socket.h>
#include <net/socket_can.h>

typedef struct {
  int tx_socket_fd;
  int rx_socket_fd;
  const struct zcan_filter rx_filter;
  struct k_msgq *msgq;
} CANChnDef;

typedef struct {
  CANChnDef channel[2];
} CANChnMapping;

CAN_DEFINE_MSGQ(can_msgq1, 2048);
CAN_DEFINE_MSGQ(can_msgq2, 2048);

CANChnMapping can = {
    // CAN1
    .channel[0] =
        {
            .rx_filter = {.id_type = CAN_EXTENDED_IDENTIFIER,
                          .rtr = CAN_DATAFRAME,
                          .rtr_mask = 1,
                          .ext_id_mask = 0},
        },
    // CAN2
    .channel[1] = {
        .rx_filter = {.id_type = CAN_EXTENDED_IDENTIFIER,
                      .rtr = CAN_DATAFRAME,
                      .rtr_mask = 1,
                      .ext_id_mask = 0},
    }};

#define SOCKET_CAN_INTERFACE_1 0
#define SOCKET_CAN_INTERFACE_2 1

#define PRIORITY k_thread_priority_get(k_current_get())
#define STACKSIZE 1024
#define CLOSE_PERIOD 15

static k_tid_t rx_tid1;
static K_THREAD_STACK_DEFINE(rx_stack1, STACKSIZE);
static struct k_thread rx_data1;

static k_tid_t rx_tid2;
static K_THREAD_STACK_DEFINE(rx_stack2, STACKSIZE);
static struct k_thread rx_data2;

struct net_if *net_if_get_by_type(const struct net_l2 *l2, const char *socket_can) {
  Z_STRUCT_SECTION_FOREACH(net_if, iface) {
    if (IS_ENABLED(CONFIG_NET_OFFLOAD) && !l2 && net_if_offload(iface)) {
      return iface;
    }
    if (net_if_l2(iface) == l2) {
      if (iface->if_dev->dev->name == socket_can)
        return iface;
    }
  }

  return NULL;
}

static int create_socket(CanIndex chn) {
  struct sockaddr_can can_addr;
  int fd, ret;

  fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
  if (fd < 0) {
    printk("ERROR: Cannot create CAN socket (%d)\n", fd);
    return fd;
  }

  struct net_if *iface;
  if (chn == CAN_MESSENGER_CHN) {
    iface = net_if_get_by_type(&NET_L2_GET_NAME(CANBUS_RAW), "SOCKET_CAN_1");
  } else if (chn == CAN_INTERACTOR_CHN) {
    iface = net_if_get_by_type(&NET_L2_GET_NAME(CANBUS_RAW), "SOCKET_CAN_2");
  }
  printf("%s\n", iface->if_dev->dev->name);
  if (!iface) {
    printk("ERROR: No CANBUS network interface found!\n");
    return -ENOENT;
  }

  can_addr.can_ifindex = net_if_get_by_iface(iface);
  can_addr.can_family = PF_CAN;

  ret = bind(fd, (struct sockaddr *)&can_addr, sizeof(can_addr));
  if (ret < 0) {
    printk("ERROR: Cannot bind %s CAN socket (%d)\n", "2nd", -errno);
    (void)close(fd);
    return ret;
  }

  printk("INFO: CAN socket created (fd: %d)\n", fd);

  return fd;
}

static void rx_task(int *can_fd, CanIndex *chn,
                    const struct can_filter *filter) {
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
      printk("ERROR: [%d] Cannot receive CAN message (%d)\n", fd, -errno);
      continue;
    }

    can_copy_frame_to_zframe(&frame, &msg);
    if ((CanIndex)(chn) == CAN_MESSENGER_CHN) {
      while (k_msgq_put(&can_msgq1, &msg, K_NO_WAIT) != 0) {
        k_msgq_purge(&can_msgq1);
      }
    } else if ((CanIndex)(chn) == CAN_INTERACTOR_CHN) {
      while (k_msgq_put(&can_msgq2, &msg, K_NO_WAIT) != 0) {
        k_msgq_purge(&can_msgq2);
      }
    }
    // printk("INFO: [%d] CAN data: ", fd);
    // for (int i = 0; i < msg.dlc; ++i)
    //   printk("%x ", msg.data[i]);
    // printk("\n");

    if (!msg.rtr) {
      if (msg.dlc > 8) {
        data = (uint8_t *)msg.data_32;
      } else {
        data = msg.data;
      }
    } else {
      printk("INFO: [%d] EXT Remote message received\n", fd);
    }
  }
}

bool SetupCan(CanIndex chn, CANMode mode, uint32_t baudrate) {
  static struct can_filter filter;
  can_copy_zfilter_to_filter(&can.channel[chn].rx_filter, &filter);

  if (chn == CAN_MESSENGER_CHN) {
    can.channel[chn].tx_socket_fd = create_socket(chn); // tx socket
    if (can.channel[chn].tx_socket_fd < 0) {
      printf("CAN1: Device driver not found.\n");
      return false;
    }
    can.channel[chn].msgq = &can_msgq1;

    // setup rx
    can.channel[chn].rx_socket_fd = create_socket(chn); // rx socket
    rx_tid1 = k_thread_create(
        &rx_data1, rx_stack1, K_THREAD_STACK_SIZEOF(rx_stack1),
        (k_thread_entry_t)rx_task,
        INT_TO_POINTER(can.channel[chn].rx_socket_fd),
        ((void *)(CanIndex)(chn)), &filter, PRIORITY, 0, K_NO_WAIT);
    if (!rx_tid1) {
      printk("Cannot create RX thread!\n");
      (void)close(can.channel[chn].rx_socket_fd);
    }

  } else if (chn == CAN_INTERACTOR_CHN) {
    can.channel[chn].tx_socket_fd = create_socket(chn);
    if (can.channel[chn].tx_socket_fd < 0) {
      printf("CAN2: Device driver not found.\n");
      return false;
    }
    can.channel[chn].msgq = &can_msgq2;

    // setup rx
    can.channel[chn].rx_socket_fd = create_socket(chn); // rx socket
    rx_tid2 = k_thread_create(
        &rx_data2, rx_stack2, K_THREAD_STACK_SIZEOF(rx_stack2),
        (k_thread_entry_t)rx_task,
        INT_TO_POINTER(can.channel[chn].rx_socket_fd),
        ((void *)(CanIndex)(chn)), &filter, PRIORITY, 0, K_NO_WAIT);
    if (!rx_tid2) {
      printk("Cannot create RX thread!\n");
      (void)close(can.channel[chn].rx_socket_fd);
    }
  }
  int ret = 0;
  ret = setsockopt(can.channel[chn].rx_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                   &filter, sizeof(filter));
  if (ret < 0) {
    ret = -errno;
    printf("ERROR: Cannot set CAN sockopt (%d)\n", ret);
    close(can.channel[chn].rx_socket_fd);
    return ret;
  }
  return true;
}

CANMsgQueue *GetCanMessageQueue(CanIndex chn) {
  return can.channel[chn].msgq;
}

int SendCanFrame(CanIndex chn, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc) {
  struct zcan_frame zcan_frame;
  struct can_frame can_frame;

  if (is_std_id) {
    zcan_frame.id_type = CAN_STANDARD_IDENTIFIER;
    zcan_frame.std_id = id;
  } else {
    zcan_frame.id_type = CAN_EXTENDED_IDENTIFIER;
    zcan_frame.ext_id = id;
  }
  zcan_frame.rtr = CAN_DATAFRAME;
  zcan_frame.dlc = dlc;
  memcpy(zcan_frame.data, data, dlc);
  can_copy_zframe_to_frame(&zcan_frame, &can_frame);
  if (send(can.channel[chn].tx_socket_fd, &can_frame, sizeof(can_frame), 0) >= 0)
    return 0;     //successfully send
  else
    return -1;    //failed to send
}