# USB转cJtag通信工具

## 简介

基于STM32F401的USB转cJtag通信工具，理论上芯片支持GPIO和USB即可

## 特性

- 支持Win USB免驱

- 支持USB 2.0 Bulk传输，使用端点0x01、0x81来输入和输出

- 支持USB转cJtag通信

## 通信报文帧格式

```

+-----+-----+-----+-----+-----+-----+-----+-----+-----+

| len | cmd |   bits    |  data0, data1 ... dataN     |

+-----+-----+-----+-----+-----+-----+-----+-----+-----+

```

| field | len    | comment                            |
| ----- | ------ | ---------------------------------- |
| len   | 1 Byte | 报文长度 1(len)+1(cmd)+2(bits)+n(data) |
| cmd   | 1 Byte | 命令码,可选项：[0x00, 0x01, 0xFF]         |
| bits  | 2 Byte | 数据bit长度                            |
| data  | N Byte | 原始数据                               |

### 1. 输出Oscan1格式的TDI序列，并接收TDO（CMD=0x00 or 0x80）

硬件接收到报文后将`data`按小端序逐位输出到TDI，LSB在前,共`bits`位，输出最后一位时，TMS为1，其余TMS都为0，用于IR/DR SCAN

硬件接收TDO序，以小端序，LSB在前的格式通过USB端点输出到上位机，不包含帧格式，所有字节均为数据，有效bit数等同于输出的`bits`数

### 2. 输出Oscan1格式的TMS序列（CMD=0x01）

硬件接收到报文后将`data`按小端序逐位输出到TMS，LSB优先,共`bits`位，硬件仅输出TMS序列，忽略TDI和TDO，用于JTAG状态机转移

### 3. 输出Oscan1格式激活序列（CMD=0xFF）

硬件接收到报文后输出Escape序列、线激活码（OAC）、扩展码（EC）和检查包（CP），以激活Oscan1格式数据传输

## 上位机DEMO

```C
/* libusb */

//初始化
struct libusb_context *ctx;
libusb_device_handle *dev_handle;

libusb_init(&ctx);
dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
libusb_claim_interface(dev_handle, 0);
libusb_set_interface_alt_setting(dev_handle, 0, 0);

//传输
libusb_bulk_transfer(dev_handle, ED_IN, data, length, &actual_length, 1000);
libusb_bulk_transfer(dev_handle, ED_OUT, data, length, &actual_length, 1000);
```