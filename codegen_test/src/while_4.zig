const ifj = @import("ifj24.zig");

pub fn main() void {
    var a :i32 = 2;
    while (a > 0) {
        a = a - 1;
    } else {
        ifj.write(a); ifj.write("\n");
    }
}
