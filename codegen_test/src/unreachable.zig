const ifj = @import("ifj24.zig");

pub fn main() void {
    const maybeVal : ?i32 = 42;
    const noVal : ?i32 = null;
    const x = maybeVal orelse unreachable;
    const y = maybeVal.?;
    const z = noVal orelse unreachable;

    ifj.write(y);
    ifj.write(z);
    ifj.write(x); 
}
