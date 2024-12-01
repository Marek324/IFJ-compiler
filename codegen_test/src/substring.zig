const ifj = @import("ifj24.zig");

pub fn main() void {
  ifj.write(ifj.substring(ifj.string("a"), 0, 1).?); 
}
