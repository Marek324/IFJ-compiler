const ifj = @import("ifj24.zig");

pub fn main() void {
    const A: ?i32 = null;
    var C: ?i32 = 3;
    C = fu(C);
    if (A) |B| {
        const P = B;
    }
    else if (A) {
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
    const A: ?i32 = null;
    return A;
}