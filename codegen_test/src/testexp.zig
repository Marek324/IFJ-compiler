const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = true;
    if (!!a)
        ifj.write("ahoj");

    ifj.write("\n");
}
