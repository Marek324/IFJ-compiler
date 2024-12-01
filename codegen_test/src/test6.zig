const ifj = @import("ifj24.zig");

pub fn main() void {

    const a = ifj.string("abcd");
    ifj.write(foo(a));
    ifj.write("\n");
}

pub fn foo(a:[]u8) i32{
    const b = 12;
    return (b - 1) * 8 + ifj.ord(a, 0) - ifj.ord(ifj.string("a"), 0);
    
}
