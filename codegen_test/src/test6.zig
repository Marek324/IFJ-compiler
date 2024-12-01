const ifj = @import("ifj24.zig");

pub fn main() void {

    const a = ifj.string("abcd");
        ifj.write(foo(a));
    ifj.write("\n");
}

pub fn foo(a:[]u8) []u8{
    const b = 12;
    const c = 0-10;
    return (b - 1) * 8 + ifj.ord(a, 0) - ifj.ord(ifj.string("a"), 0);
    
}
