const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("Int: ");
    const y = ifj.readi32();
    if(y) |i| {
        ifj.write(i); ifj.write("\n");
    } else {
        ifj.write("null\n");
    }
}
