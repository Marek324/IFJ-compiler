const ifj = @import("ifj24.zig");

pub fn main() void {
    var i: i32 = 0;
    nazev: while (i < 2000000){
        while (i < 10) : ({
            if (i == 6)
                break :nazev;
        }) {
            ifj.write(i); ifj.write("\n");
            i = i + 2;
        }else{
            ifj.write("i na konci whilu: "); ifj.write(i); ifj.write("\n");
        }
    }
}