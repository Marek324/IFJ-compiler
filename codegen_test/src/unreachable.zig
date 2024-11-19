const ifj = @import("ifj24.zig");

pub fn main() void {
    const noVal : ?i32 = null;
    const x = noVal orelse 42;
    const y = ifj.readi32();
    var z = noVal orelse y orelse noVal orelse y.?;

    z = 2;

    ifj.write(x); ifj.write("\n");
}
