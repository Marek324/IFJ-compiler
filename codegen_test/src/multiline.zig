const ifj = @import("ifj24.zig");
// Hlavná funkcia programu
pub fn main() void {

    const a = ifj.string(\\ line1
                        \\ line2
                        \\ line3
                        \\ line4
                        );
    ifj.write(a);
}