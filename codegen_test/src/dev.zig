const ifj = @import("ifj24.zig");

pub fn main() void {
    const A: ?i32 = 3;
    if (A) |B| {
        const A: ?i32 = 3;
    }
    if (A) {

    }
}
