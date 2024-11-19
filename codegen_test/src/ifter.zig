const ifj = @import("ifj24.zig");

pub fn main() void {
    const y = 12;
    const x = 3;
    const z = if (x > y) x else y; 
    ifj.write(z); 
    ifj.write("\n");
}
