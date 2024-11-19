const ifj = @import("ifj24.zig");

pub fn main() void {
    var i: i32 = 0;
    while (i < 10) : (i = i + 1) {
        ifj.write(i); ifj.write("\n");
        i = i + 1;
    }else{
        ifj.write("i na konci whilu: "); ifj.write(i); ifj.write("\n");
    }
}