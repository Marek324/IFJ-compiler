const ifj = @import("ifj24.zig");

pub fn main() void {
    const b = 2;
    const a = 1+nic(b,3,);
    ifj.write(a); ifj.write("\n");
}

pub fn nic(a:i32,b:i32,) i32 {
    return a+b;
}