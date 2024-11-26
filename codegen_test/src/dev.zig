const ifj = @import("ifj24.zig");

pub fn main() void {
    const y = ifj.readi32();
    const i : i32 = 3 + 3;
    const f : ?i32 = 3 + 3;
    i = 1;
    ifj.write("s");
}

pub fn fu(ast: i32) i32 {
    const y : i32 = 1+3;
}