const ifj = @import("ifj24.zig");

pub fn main() void {
    const A: ?i32 = 3;
    const C: ?i32 = 3;
    if (A) |B| {
        const S: ?i32 = 3;
    }
    if (A) |K| {
        const D: ?i32 = 3;
    }
}
