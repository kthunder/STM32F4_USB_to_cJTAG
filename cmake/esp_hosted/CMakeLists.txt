cmake_minimum_required(VERSION 3.22)

project(esp_hosted_lib)

add_library(esp_hosted_lib INTERFACE)

set(ESP_HOSTED_DIR ${CMAKE_SOURCE_DIR}/libs/rt-thread_esp-hosted)

target_include_directories(esp_hosted_lib INTERFACE
    # esp-hosted common
    ${ESP_HOSTED_DIR}/esp-hosted/host
    ${ESP_HOSTED_DIR}/esp-hosted/common/proto
    ${ESP_HOSTED_DIR}/esp-hosted/common/include
    ${ESP_HOSTED_DIR}/esp-hosted/common/protobuf-c

    # esp-hosted host api
    ${ESP_HOSTED_DIR}/esp-hosted/host/api/include

    # esp-hosted host drivers
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/mempool
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/core
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/slaveif
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/wrap
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/serial
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/transport
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/transport/spi
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/virtual_serial_if
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/bt

    # esp-hosted host utils
    ${ESP_HOSTED_DIR}/esp-hosted/host/utils

    # porting components
    ${ESP_HOSTED_DIR}/porting/components/log
    ${ESP_HOSTED_DIR}/porting/components/utils
    ${ESP_HOSTED_DIR}/porting/components/esp_wifi/include
    ${ESP_HOSTED_DIR}/porting/components/esp_netif/include

    # porting port
    ${ESP_HOSTED_DIR}/porting/port/include

    # porting ota
    ${ESP_HOSTED_DIR}/porting/ota
)

target_sources(esp_hosted_lib INTERFACE
    # esp-hosted common proto
    ${ESP_HOSTED_DIR}/esp-hosted/common/proto/esp_hosted_rpc.pb-c.c

    # esp-hosted host api
    ${ESP_HOSTED_DIR}/esp-hosted/host/api/src/esp_hosted_api.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/api/src/esp_wifi_weak.c

    # esp-hosted host drivers - mempool
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/mempool/mempool.c

    # esp-hosted host drivers - rpc core
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/core/rpc_core.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/core/rpc_evt.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/core/rpc_req.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/core/rpc_rsp.c

    # esp-hosted host drivers - rpc slaveif
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/slaveif/rpc_slave_if.c

    # esp-hosted host drivers - rpc wrap
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/rpc/wrap/rpc_wrap.c

    # esp-hosted host drivers - serial
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/serial/serial_drv.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/serial/serial_ll_if.c

    # esp-hosted host drivers - transport
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/transport/transport_drv.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/transport/spi/spi_drv.c

    # esp-hosted host drivers - virtual serial if
    ${ESP_HOSTED_DIR}/esp-hosted/host/drivers/virtual_serial_if/serial_if.c

    # esp-hosted host utils
    ${ESP_HOSTED_DIR}/esp-hosted/host/utils/common.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/utils/stats.c
    ${ESP_HOSTED_DIR}/esp-hosted/host/utils/util.c

    # porting components - log
    ${ESP_HOSTED_DIR}/porting/components/log/esp_log.c
    ${ESP_HOSTED_DIR}/porting/components/log/log_linked_list.c
    ${ESP_HOSTED_DIR}/porting/components/log/tag_log_level.c

    # porting components - esp wifi remote
    ${ESP_HOSTED_DIR}/porting/components/esp_wifi_remote/esp_wifi_remote_net.c

    # porting port
    ${ESP_HOSTED_DIR}/porting/port/source/esp_hosted_config.c
    ${ESP_HOSTED_DIR}/porting/port/source/os_wrapper.c
    ${ESP_HOSTED_DIR}/porting/port/source/spi_wrapper.c

    # porting ota
    ${ESP_HOSTED_DIR}/porting/ota/esp_hosted_ota.c

    # porting wlan
    ${ESP_HOSTED_DIR}/porting/wlan/esp_wlan.c
)

# 蓝牙驱动选择
# SConscript 中有三种情况:
#   1. ESP_HOSTED_BT_USING_VHCI_DEVICE_DRIVER -> vhci_dev.c + bt/vhci/vhci.c
#   2. ESP_HOSTED_BT_USING_NIMBLE_STACK -> vhci_drv.c
#   3. 默认(无选择) -> vhci_drv.c (空实现)
# CMake 中通过 option 控制, 默认使用 vhci_drv.c (空实现)
option(ESP_HOSTED_BT_VHCI_DEVICE_DRIVER "Use VHCI device driver" OFF)

if(ESP_HOSTED_BT_VHCI_DEVICE_DRIVER)
    target_sources(esp_hosted_lib INTERFACE
        ${ESP_HOSTED_DIR}/porting/bt/vhci_dev.c
        ${ESP_HOSTED_DIR}/porting/bt/vhci/vhci.c
    )
else()
    # 默认或 NimBLE: 使用 vhci_drv.c (空实现)
    target_sources(esp_hosted_lib INTERFACE
        ${ESP_HOSTED_DIR}/porting/bt/vhci_drv.c
    )
endif()
