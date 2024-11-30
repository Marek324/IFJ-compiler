const ifj = @import("ifj24.zig");

pub fn main() void {
    const A: ?i32 = null;
    const C: ?i32 = 3;
    if (A) |B| {
        const P = B;
    }
    else {
        const P = C;
    }
    brk: while (A) |B| : (const K: ?i32 = 3) {
        const D: ?i32 = 3;
    }
    else {
        
    }
    
    while (A) |B| {
        const D: ?i32 = 3;
    }
}
pub fn fu( ast: ?i32,) ?i32 {
    const A: ?i32 = 3;
    return A;
}