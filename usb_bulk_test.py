import usb.core
import time
# 查找设备（替换为实际 VID/PID）
dev = usb.core.find(idVendor=0xEFFF, idProduct=0xFFFF)
if dev is None:
    raise ValueError("设备未找到")
else:
    # 配置设备（需根据设备协议配置端点）
    dev.set_configuration()
    cfg = dev.get_active_configuration()
    for _ in cfg:
        # print("**************")
        # print(_)
        if _.bInterfaceClass == 0xFF:
            intf = _
    # 获取端点（Bulk 传输为例）
    ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: 
                                    e.bEndpointAddress & 0x80 == 0
                                    and e.bmAttributes & 0x02 == 0x02)
    ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: 
                                    e.bEndpointAddress & 0x80 != 0
                                    and e.bmAttributes & 0x02 == 0x02)

def speed_test():
    bytelen = 64 * 10000 #直接使用大数据块，因为py循环比较慢
    data = b'\xAA' * bytelen  # 数据块
    start = time.time()
    ep_out.write(data)
    duration = time.time() - start
    print(f"速度: {bytelen / duration / 1024:.2f} KB/s")

if __name__ == "__main__":
    print(usb.util.get_string(dev, dev.iProduct))  # 产品名称
    print(hex(dev.bcdUSB))  # U
    ep_out.write(bytearray([5,0x80,0x08,0x00,0xff]))
    ep_out.write(bytearray([6,0x0,0x09,0x00,0x55,0x55]))
    ep_out.write(bytearray([6,0x1,0x0A,0x00,0xAA,0xAA]))
    ep_out.write(bytearray([5,0xFF,0x08,0x00,0xff]))
    data = ep_in.read(64, timeout=1000)
    print(f"接收数据: {data.tobytes()}")
    # speed_test()
    # for _ in range(10):
    #     bb = bytearray(30)
    #     bb[0] = _
    #     # 发送数据
    #     ep_out.write(bb)  # 自定义协议数据
    #     # 接收数据
    #     # data = ep_in.read(64, timeout=1000)
    #     # print(f"接收数据: {data.tobytes()}")