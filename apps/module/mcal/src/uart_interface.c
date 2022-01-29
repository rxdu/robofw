/*
 * uart_interface.c
 *
 * Created on: Jan 24, 2022 22:13
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "mcal/interface/uart_interface.h"

#include <assert.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>
#include <sys/printk.h>

#define initialize_uart(x, node_label, desc)                \
  {                                                         \
    desc.descriptor[x].device =                             \
        device_get_binding(DT_LABEL(DT_ALIAS(node_label))); \
    if (!desc.descriptor[x].device) {                       \
      printk("[ERROR] UART device driver not found\n");     \
      return false;                                         \
    }                                                       \
    desc.descriptor[x].active = true;                       \
  }

static UartDescription uart_desc;

bool InitUart() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_UART_NUM; ++i) uart_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart0), okay)
  initialize_uart(0, xuart0, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart1), okay)
  initialize_uart(1, xuart1, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart2), okay)
  initialize_uart(2, xuart2, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart3), okay)
  initialize_uart(3, xuart3, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart4), okay)
  initialize_uart(4, xuart4, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart5), okay)
  initialize_uart(5, xuart5, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart6), okay)
  initialize_uart(6, xuart6, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart7), okay)
  initialize_uart(7, xuart7, uart_desc);
#endif

  printk("[INFO] Initialized UART\n");
  PrintUartInitResult();

  return true;
}

UartDescription* GetUartDescription() { return &uart_desc; }

UartDescriptor* GetUartDescriptor(UartList dev_id) {
  assert(dev_id < DD_UART_NUM);
  return &uart_desc.descriptor[dev_id];
}

void PrintUartInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_UART_NUM; ++i) {
    if (uart_desc.descriptor[i].active) {
      count++;
      printk(" - [xUART%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}

static void GenericUartCallback(const struct device* uart_dev,
                                struct uart_event* evt, void* device_id);

void GetUartSbusConfig(struct uart_config* cfg) {
  cfg->baudrate = 100000;
  cfg->parity = UART_CFG_PARITY_EVEN;
  cfg->stop_bits = UART_CFG_STOP_BITS_2;
  cfg->data_bits = UART_CFG_DATA_BITS_8;
  cfg->flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
}

bool ConfigureUart(UartList dev_id, struct uart_config config) {
  if (!uart_desc.descriptor[dev_id].active) {
    printk("[xUART] Device inactive\n");
    return false;
  }

  uart_desc.descriptor[dev_id].config = config;
  int ret = uart_configure(uart_desc.descriptor[dev_id].device,
                           &uart_desc.descriptor[dev_id].config);
  return (ret == 0);
}

bool SetupUartAsyncMode(UartList dev_id) {
  if (!uart_desc.descriptor[dev_id].active) {
    printk("[xUART] Device inactive\n");
    return false;
  }

  if (k_sem_init(&uart_desc.descriptor[dev_id].rx_sem, 0, 1) != 0 ||
      k_sem_init(&uart_desc.descriptor[dev_id].tx_sem, 0, 1) != 0) {
    return false;
  }

  uart_callback_set(uart_desc.descriptor[dev_id].device, GenericUartCallback,
                    (void*)dev_id);

  ring_buf_init(&uart_desc.descriptor[dev_id].ring_buffer,
                sizeof(uart_desc.descriptor[dev_id].ring_buffer_mem),
                uart_desc.descriptor[dev_id].ring_buffer_mem);

  return true;
}

bool StartUartAsyncSend(UartList dev_id, const uint8_t* buf, size_t len,
                        int32_t timeout) {
  if (!uart_desc.descriptor[dev_id].active) {
    printk("[xUART] Device inactive\n");
    return false;
  }
  int ret = uart_tx(uart_desc.descriptor[dev_id].device, buf, len, timeout);
  return (ret == 0);
}

void StartUartAsyncReceive(UartList dev_id) {
  if (!uart_desc.descriptor[dev_id].active) {
    printk("[xUART] Device inactive\n");
    return;
  }

  uart_desc.descriptor[dev_id].double_buffer_index = 0;
  uart_rx_enable(
      uart_desc.descriptor[dev_id].device,
      uart_desc.descriptor[dev_id]
          .rx_double_buffer[uart_desc.descriptor[dev_id].double_buffer_index],
      UART_RX_BUF_SIZE, UART_RX_TIMEOUT);
}

void StopUartAsyncReceive(UartList dev_id) {
  if (!uart_desc.descriptor[dev_id].active) {
    printk("[xUART] Device inactive\n");
    return;
  }

  uart_rx_disable(uart_desc.descriptor[dev_id].device);
}

void GenericUartCallback(const struct device* uart_dev, struct uart_event* evt,
                         void* device_id) {
  UartList dev_id = (UartList)device_id;

  switch (evt->type) {
    // Received data is ready for processing.
    case UART_RX_RDY:
      ring_buf_put(&uart_desc.descriptor[dev_id].ring_buffer,
                   evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len);
      k_sem_give(&uart_desc.descriptor[dev_id].rx_sem);
      break;
    // Driver requests next buffer for continuous reception.
    case UART_RX_BUF_REQUEST:
      uart_desc.descriptor[dev_id].double_buffer_index =
          (uart_desc.descriptor[dev_id].double_buffer_index + 1) % 2;
      uart_rx_buf_rsp(uart_desc.descriptor[dev_id].device,
                      uart_desc.descriptor[dev_id].rx_double_buffer
                          [uart_desc.descriptor[dev_id].double_buffer_index],
                      UART_RX_BUF_SIZE);
      break;
    // Buffer is no longer used by UART driver.
    case UART_RX_BUF_RELEASED:
      break;
    // RX has been disabled and can be reenabled.
    case UART_RX_DISABLED:
      uart_desc.descriptor[dev_id].double_buffer_index = 0;
      StartUartAsyncReceive(dev_id);
      break;
    // RX has stopped due to external event.
    case UART_RX_STOPPED:
      break;
    // Whole TX buffer was transmitted.
    case UART_TX_DONE:
      k_sem_give(&uart_desc.descriptor[dev_id].tx_sem);
      break;
    // Transmitting aborted due to timeout or uart_tx_abort call
    case UART_TX_ABORTED:
      break;
    default:
      break;
  }
}