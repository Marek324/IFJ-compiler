const ifj = @import("ifj24.zig");

pub fn main() void {
    var i: ?i32 = 2;
    while (i) |ic|: (i = if (ic > 0) ic - 1 else null) {
        ifj.write(ic); ifj.write("\n");
    }
}
