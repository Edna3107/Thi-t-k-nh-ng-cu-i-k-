#include "xparameters.h"
#include "xgpio.h"
#include "menu.h"     // Thư viện IP Menu của bạn
#include "lcd1602.h"
#include "sleep.h"
#include "xil_printf.h" // Thư viện để in lên Terminal
#include <stdio.h>

/* ===== Cấu hình địa chỉ ===== */
#define MENU_BASE    XPAR_MENU_0_S00_AXI_BASEADDR
#define GPIO_ID      XPAR_AXI_GPIO_0_DEVICE_ID

/* ===== Mã lệnh (Khớp với logic nút bấm bạn yêu cầu) ===== */
#define CMD_PAGE_PREV  (1 << 2) // BTN0: Trang trước
#define CMD_PAGE_NEXT  (1 << 3) // BTN1: Trang sau
#define CMD_ITEM_UP    (1 << 0) // BTN2: Con trỏ lên
#define CMD_ITEM_DOWN  (1 << 1) // BTN3: Con trỏ xuống

#define TOTAL_ITEMS       16
#define ITEMS_PER_PAGE    4

XGpio Gpio;

const char *menu_text[TOTAL_ITEMS] = {
    "P1: Setting", "P1: Network", "P1: Display", "P1: Sound",
    "P2: WiFi",    "P2: Bluetooth","P2: IP Addr", "P2: Reset",
    "P3: Bright",  "P3: Contrast", "P3: Color",   "P3: Theme",
    "P4: Info",    "P4: Version",  "P4: Support", "P4: Exit"
};

/* Hàm gửi xung lệnh xuống IP */
void send_ip_command(u32 bit_mask) {
    MENU_mWriteReg(MENU_BASE, 0, bit_mask);
    usleep(10);
    MENU_mWriteReg(MENU_BASE, 0, 0);
}

/* Hàm cập nhật LCD và in trạng thái lên Terminal */
void refresh_system_status() {
    // 1. Đọc trạng thái từ Hardware (IP Menu)
    u32 page_id = MENU_mReadReg(MENU_BASE, 4);  // Đọc slv_reg1
    u32 item_id = MENU_mReadReg(MENU_BASE, 8);  // Đọc slv_reg2

    int base_index = page_id * ITEMS_PER_PAGE;
    int current_global_index = base_index + item_id;

    // 2. In thông tin lên Terminal để Debug
    xil_printf("\r\n--- MENU STATUS ---");
    xil_printf("\r\nPage ID  : %d", (int)page_id);
    xil_printf("\r\nItem ID  : %d", (int)item_id);
    xil_printf("\r\nSelected : %s", menu_text[current_global_index]);
    xil_printf("\r\n-------------------");

    // 3. Hiển thị lên LCD 16x2
    lcd_cmd(0x01); // Xóa LCD
    usleep(2000);

    // Thuật toán cửa sổ trượt để hiện 2 dòng từ 4 mục của 1 trang
    int display_offset = (item_id / 2) * 2;

    lcd_cmd(0x80); // Dòng 1
    lcd_print(item_id % 2 == 0 ? ">" : " ");
    lcd_print(menu_text[base_index + display_offset]);

    lcd_cmd(0xC0); // Dòng 2
    lcd_print(item_id % 2 == 1 ? ">" : " ");
    lcd_print(menu_text[base_index + display_offset + 1]);
}

int main() {
    u32 btn, last_btn = 0;

    // Khởi tạo hệ thống
    XGpio_Initialize(&Gpio, GPIO_ID);
    XGpio_SetDataDirection(&Gpio, 2, 0xF);
    lcd_init();

    xil_printf("\r\n====================================");
    xil_printf("\r\n   HE THONG SoC MENU KHOI DONG...   ");
    xil_printf("\r\n====================================");

    refresh_system_status();

    while (1) {
        btn = XGpio_DiscreteRead(&Gpio, 2);

        // BTN0: Lùi Trang
        if ((btn & 0x1) && !(last_btn & 0x1)) {
            xil_printf("\r\n[EVENT] Bam BTN0: Lui Trang");
            send_ip_command(CMD_PAGE_PREV);
            refresh_system_status();
        }
        // BTN1: Tới Trang
        if ((btn & 0x2) && !(last_btn & 0x2)) {
            xil_printf("\r\n[EVENT] Bam BTN1: Toi Trang");
            send_ip_command(CMD_PAGE_NEXT);
            refresh_system_status();
        }
        // BTN2: Con trỏ Lên
        if ((btn & 0x4) && !(last_btn & 0x4)) {
            xil_printf("\r\n[EVENT] Bam BTN2: Con tro LEN");
            send_ip_command(CMD_ITEM_UP);
            refresh_system_status();
        }
        // BTN3: Con trỏ Xuống
        if ((btn & 0x8) && !(last_btn & 0x8)) {
            xil_printf("\r\n[EVENT] Bam BTN3: Con tro XUONG");
            send_ip_command(CMD_ITEM_DOWN);
            refresh_system_status();
        }

        last_btn = btn;
        usleep(20000); // Tránh dội phím
    }
    return 0;
}
