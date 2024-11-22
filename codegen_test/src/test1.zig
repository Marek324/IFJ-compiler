const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.string("abc");
    const b = ifj.length(a);
    ifj.write(b); ifj.write("\n");
}
