/*
 * uart_interface.c
 *
 * Created on: Jan 24, 2022 22:13
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/uart_interface.h"

static void GenericUartCallback(const struct device* uart_dev,
                                struct uart_event* evt,
                                void* device_descriptor);

void GetUartSbusConfig(struct uart_config* cfg) {
  cfg->baudrate = 100000;
  cfg->parity = UART_CFG_PARITY_EVEN;
  cfg->stop_bits = UART_CFG_STOP_BITS_2;
  cfg->data_bits = UART_CFG_DATA_BITS_8;
  cfg->flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
}

bool ConfigureUart(UartDescriptor* dd, struct uart_config config) {
  if (!dd->active) return false;

  dd->config = config;
  int ret = uart_configure(dd->device, &dd->config);
  return (ret == 0);
}

bool SetupUartAsyncMode(UartDescriptor* dd) {
  if (!dd->active) return false;

  if (k_sem_init(&dd->rx_sem, 0, 1) != 0 ||
      k_sem_init(&dd->tx_sem, 0, 1) != 0) {
    return false;
  }

  uart_callback_set(dd->device, GenericUartCallback, (void*)dd);

  ring_buf_init(&dd->ring_buffer, sizeof(dd->ring_buffer_mem),
                dd->ring_buffer_mem);

  return true;
}

bool StartUartAsyncSend(UartDescriptor* dd, const uint8_t* buf, size_t len,
                        int32_t timeout) {
  if (!dd->active) return false;
  int ret = uart_tx(dd->device, buf, len, timeout);
  return (ret == 0);
}

void StartUartAsyncReceive(UartDescriptor* dd) {
  if (!dd->active) return;

  dd->double_buffer_index = 0;
  uart_rx_enable(dd->device, dd->rx_double_buffer[dd->double_buffer_index],
                 UART_RX_BUF_SIZE, UART_RX_TIMEOUT);
}

void StopUartAsyncReceive(UartDescriptor* dd) {
  if (!dd->active) return;

  uart_rx_disable(dd->device);
}

void GenericUartCallback(const struct device* uart_dev, struct uart_event* evt,
                         void* device_descriptor) {
  UartDescriptor* dd = (UartDescriptor*)device_descriptor;

  switch (evt->type) {
    // Received data is ready for processing.
    case UART_RX_RDY:
      ring_buf_put(&dd->ring_buffer, evt->data.rx.buf + evt->data.rx.offset,
                   evt->data.rx.len);
      k_sem_give(&dd->rx_sem);
      break;
    // Driver requests next buffer for continuous reception.
    case UART_RX_BUF_REQUEST:
      dd->double_buffer_index = (dd->double_buffer_index + 1) % 2;
      uart_rx_buf_rsp(dd->device, dd->rx_double_buffer[dd->double_buffer_index],
                      UART_RX_BUF_SIZE);
      break;
    // Buffer is no longer used by UART driver.
    case UART_RX_BUF_RELEASED:
      break;
    // RX has been disabled and can be reenabled.
    case UART_RX_DISABLED:
      dd->double_buffer_index = 0;
      StartUartAsyncReceive(dd);
      break;
    // RX has stopped due to external event.
    case UART_RX_STOPPED:
      break;
    // Whole TX buffer was transmitted.
    case UART_TX_DONE:
      k_sem_give(&dd->tx_sem);
      break;
    // Transmitting aborted due to timeout or uart_tx_abort call
    case UART_TX_ABORTED:
      break;
    default:
      break;
  }
}