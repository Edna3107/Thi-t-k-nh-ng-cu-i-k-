`timescale 1ns/1ps

module menu_controller #(
    parameter NUM_PAGES = 4,           // s? trang menu
    parameter NUM_ITEMS_PER_PAGE = 4,  // s? item m?i trang
    parameter PAGE_WIDTH = 2,           // bit width cho page
    parameter ITEM_WIDTH = 2            // bit width cho item
)(
    input  wire clk,
    input  wire rst_n,                  // ACTIVE LOW reset

    // Button inputs (1-cycle pulse t? AXI wrapper)
    input  wire btn_up,                 // item tr??c
    input  wire btn_down,               // item sau
    input  wire btn_left,               // trang tr??c
    input  wire btn_right,              // trang sau
    input  wire btn_select,             // ch?n item

    // Menu state outputs
    output reg [PAGE_WIDTH-1:0] current_page,
    output reg [ITEM_WIDTH-1:0] current_item,
    output reg [7:0] selected_id,       // page*NUM_ITEMS + item
    output reg item_selected,            // pulse khi select

    // LCD (ch?a dùng trong AXI, gi? l?i)
    output reg [7:0] lcd_data,
    output reg lcd_data_valid
);

    // ============================
    // Internal registers
    // ============================
    reg [PAGE_WIDTH-1:0] page_reg;
    reg [ITEM_WIDTH-1:0] item_reg;

    // ============================
    // MENU NAVIGATION (LEVEL BASED)
    // ============================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            page_reg      <= {PAGE_WIDTH{1'b0}};
            item_reg      <= {ITEM_WIDTH{1'b0}};
            current_page  <= {PAGE_WIDTH{1'b0}};
            current_item  <= {ITEM_WIDTH{1'b0}};
            selected_id   <= 8'h00;
            item_selected <= 1'b0;
        end else begin
            // default
            item_selected <= 1'b0;

            // -------- ITEM UP --------
            if (btn_up) begin
                if (item_reg == 0)
                    item_reg <= NUM_ITEMS_PER_PAGE - 1;
                else
                    item_reg <= item_reg - 1'b1;
            end

            // -------- ITEM DOWN --------
            else if (btn_down) begin
                if (item_reg == NUM_ITEMS_PER_PAGE - 1)
                    item_reg <= {ITEM_WIDTH{1'b0}};
                else
                    item_reg <= item_reg + 1'b1;
            end

            // -------- PAGE LEFT --------
            else if (btn_left) begin
                if (page_reg == 0)
                    page_reg <= NUM_PAGES - 1;
                else
                    page_reg <= page_reg - 1'b1;

                item_reg <= {ITEM_WIDTH{1'b0}}; // reset item
            end

            // -------- PAGE RIGHT --------
            else if (btn_right) begin
                if (page_reg == NUM_PAGES - 1)
                    page_reg <= {PAGE_WIDTH{1'b0}};
                else
                    page_reg <= page_reg + 1'b1;

                item_reg <= {ITEM_WIDTH{1'b0}}; // reset item
            end

            // -------- SELECT --------
            else if (btn_select) begin
                selected_id   <= (page_reg << ITEM_WIDTH) + item_reg;
                item_selected <= 1'b1;
            end

            // update outputs
            current_page <= page_reg;
            current_item <= item_reg;
        end
    end

    // ============================
    // LCD DATA (OPTIONAL / DEBUG)
    // ============================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            lcd_data       <= 8'h00;
            lcd_data_valid <= 1'b0;
        end else begin
            if (btn_up || btn_down || btn_left || btn_right) begin
                lcd_data <= {2'b00, page_reg, 2'b00, item_reg};
                lcd_data_valid <= 1'b1;
            end else if (btn_select) begin
                lcd_data <= selected_id;
                lcd_data_valid <= 1'b1;
            end else begin
                lcd_data_valid <= 1'b0;
            end
        end
    end

endmodule
