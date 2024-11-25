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

pub fn fu(ast: i32) i32 {
    ifj.write("Int: ");
    const y : i32 = 3;
}