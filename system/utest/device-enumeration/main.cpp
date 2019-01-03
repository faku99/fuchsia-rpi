// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <fbl/algorithm.h>
#include <fbl/unique_fd.h>
#include <fuchsia/sysinfo/c/fidl.h>
#include <lib/devmgr-integration-test/fixture.h>
#include <lib/fdio/util.h>
#include <unittest/unittest.h>
#include <zircon/status.h>

using devmgr_integration_test::RecursiveWaitForFile;

namespace {

enum class Board {
    kQemu,
    kVim2,
    kAstro,
    kUnknown,
};

Board GetBoardType() {
    constexpr char kSysInfoPath[] = "/dev/misc/sysinfo";
    fbl::unique_fd sysinfo(open(kSysInfoPath, O_RDWR));
    if (!sysinfo) {
        return Board::kUnknown;
    }
    zx::channel channel;
    if (fdio_get_service_handle(sysinfo.release(), channel.reset_and_get_address()) != ZX_OK) {
        return Board::kUnknown;
    }

    char board_name[ZX_MAX_NAME_LEN];
    zx_status_t status;
    size_t actual_size;
    zx_status_t fidl_status = fuchsia_sysinfo_DeviceGetBoardName(channel.get(), &status, board_name,
                                                                sizeof(board_name), &actual_size);
    if (fidl_status != ZX_OK || status != ZX_OK) {
        return Board::kUnknown;
    }

    printf("Found board %s\n", board_name);

    if (!strcmp(board_name, "qemu")) {
        return Board::kQemu;
    } else if (!strcmp(board_name, "vim2")) {
        return Board::kVim2;
    } else if (!strcmp(board_name, "astro")) {
        return Board::kAstro;
    }

    return Board::kUnknown;
}

bool TestRunner(const char** device_paths, size_t paths_num) {
    BEGIN_HELPER;

    fbl::unique_fd devfs_root(open("/dev", O_RDWR));
    ASSERT_TRUE(devfs_root);

    fbl::unique_fd fd;
    for (size_t i = 0; i < paths_num; ++i) {
        EXPECT_EQ(RecursiveWaitForFile(devfs_root, device_paths[i],
                                       zx::deadline_after(zx::sec(1)), &fd),
                  ZX_OK, device_paths[i]);
    }
    END_HELPER;
}

bool qemu_enumeration_test() {
    BEGIN_TEST;
    static const char* kDevicePaths[] = {
        "sys/platform/qemu-bus",
        "sys/platform/00:00:6/rtc",
        "sys/pci/00:00.0",
    };

    ASSERT_TRUE(TestRunner(kDevicePaths, fbl::count_of(kDevicePaths)));

    END_TEST;
}

bool vim2_enumeration_test() {
    BEGIN_TEST;
    static const char* kDevicePaths[] = {
        "sys/platform/vim-bus",
        "sys/platform/05:03:1/aml-gxl-gpio",
        "sys/platform/00:00:8/gpio-test",
        "sys/platform/05:00:2/aml-i2c",
        "sys/platform/05:03:4/clocks",
        "sys/platform/05:00:e/aml-canvas",
        //"sys/platform/05:00:3/aml-uart/serial/bt-transport-uart/bcm-hci",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-000/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-001/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-002/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-003/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-004/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-005/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-006/block/fvm/blobfs-p-1/block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc/sdmmc-block/block/part-006/block/fvm/minfs-p-2/block/zxcrypt/block",
        //"sys/platform/05:00:6/aml-sd-emmc/sdio",
        "sys/platform/04:02:7/aml-ethernet/eth_phy/phy_null_device",
        "sys/platform/04:02:7/aml-ethernet/Designware MAC/ethernet",
        "sys/platform/00:00:2/xhci/usb",
        "sys/platform/05:03:d/aml-gpu",
        "sys/platform/04:02:9/aml-mailbox/aml-scpi/vim-thermal",
        "sys/platform/04:02:1/ProxyClient[7043414e]/aml-canvas-proxy",
        "sys/platform/04:02:1/display/vim2-display/display-controller",
        "sys/platform/05:03:c/ProxyClient[7043414e]/aml-canvas-proxy",
        "sys/platform/05:03:c/video",
        "sys/platform/00:00:b/led2472g",
        "sys/platform/00:00:e",
        "sys/platform/09:00:5/rtc",
    };

    ASSERT_TRUE(TestRunner(kDevicePaths, fbl::count_of(kDevicePaths)));

    END_TEST;
}

bool astro_enumeration_test() {
    BEGIN_TEST;
    static const char* kDevicePaths[] = {
        "sys/platform/aml-bus",
        "sys/platform/05:05:1/aml-axg-gpio",
        "sys/platform/00:00:13/hid-buttons/hid-device-000",
        "sys/platform/05:00:2/aml-i2c",
        "sys/platform/05:05:d/aml-gpu",
        "sys/platform/00:00:2/xhci/usb",
        "sys/platform/03:03:5/gt92xx HidDevice/hid-device-000",
        "sys/platform/10:01:1/ti-lp8556",
        "sys/platform/05:05:b/ProxyClient[7043414e]/aml-canvas-proxy",
        "sys/platform/05:05:b/display/astro-display/display-controller",
        "sys/platform/05:00:e/aml-canvas",
        "sys/platform/00:00:e/optee-tz",
        "sys/platform/05:05:c/ProxyClient[7043414e]/aml-canvas-proxy",
        "sys/platform/05:05:c/aml-video",
        "sys/platform/00:00:f/fallback-rtc",
        "sys/platform/05:00:d/aml-raw_nand/nand/tpl/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/fts/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/factory/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/zircon-b/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/zircon-a/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/zircon-r/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/fvm/ftl/block/fvm/blobfs-p-1/block",
        "sys/platform/05:00:d/aml-raw_nand/nand/fvm/ftl/block/fvm/minfs-p-2/block/zxcrypt/block",
        "sys/platform/05:00:d/aml-raw_nand/nand/sys-config/skip-block",
        "sys/platform/05:00:d/aml-raw_nand/nand/migration/skip-block",
        "sys/platform/05:00:6/aml-sd-emmc/sdmmc",
        "sys/platform/05:00:6/aml-sd-emmc/sdio",
        "sys/platform/0a:01:1/tcs-3400/hid-device-000",
        "sys/platform/05:05:f/clocks",
        "sys/platform/05:05:8/thermal",
        "sys/platform/05:05:10/astro-audio-out",
        "sys/platform/05:05:11/astro-audio-in",
        //"sys/platform/05:05:3/aml-uart/serial/bt-transport-uart/bcm-hci",
    };

    ASSERT_TRUE(TestRunner(kDevicePaths, fbl::count_of(kDevicePaths)));

    END_TEST;
}

#define MAKE_TEST_CASE(name) \
    BEGIN_TEST_CASE(name) \
    RUN_TEST(name ## _enumeration_test) \
    END_TEST_CASE(name); \
    test_case_element* test_case_ ## name = TEST_CASE_ELEMENT(name)

MAKE_TEST_CASE(qemu);
MAKE_TEST_CASE(vim2);
MAKE_TEST_CASE(astro);

#undef MAKE_TEST_CASE

} // namespace

int main(int argc, char** argv) {
    switch (GetBoardType()) {
        case Board::kQemu:
            return unittest_run_one_test(test_case_qemu, TEST_ALL) ? 0 : -1;
        case Board::kVim2:
            return unittest_run_one_test(test_case_vim2, TEST_ALL) ? 0 : -1;
        case Board::kAstro:
            return unittest_run_one_test(test_case_astro, TEST_ALL) ? 0 : -1;
        case Board::kUnknown:
            return 0;
    }
}