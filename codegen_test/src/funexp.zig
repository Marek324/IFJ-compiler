const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 1+add(2+add(3+4,5),6);
    ifj.write(a); ifj.write("\n");
}

pub fn add(a:i32, b:i32) i32 {
    return a + b;
}