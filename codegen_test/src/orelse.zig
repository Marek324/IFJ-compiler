const ifj = @import("ifj24.zig");

pub fn main() void {
    const noVal : ?i32 = null;
    const maybeVal : ?i32 = 123;
    //const x = noVal orelse 42;
    //const y = noVal orelse null;
    var z = noVal; //orelse maybeVal;

    z = z orelse 2;

    ifj.write(z); ifj.write("\n");
}
