const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.string("abc");
    const c = ifj.substring(a, 1,2);

    if (c) |aa| {
        ifj.write(aa); 
        ifj.write("\n");
    }
}
