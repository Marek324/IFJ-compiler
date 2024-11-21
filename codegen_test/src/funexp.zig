const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 2+add(2+add(1+4,2, 3, 4, 5),3);
    ifj.write(a); ifj.write("\n");
}

pub fn add(a:i32, b:i32) i32 {
    return a + b;
}