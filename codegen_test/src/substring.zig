const ifj = @import("ifj24.zig");

pub fn main() void {
  ifj.write(ifj.substring(ifj.string("abcd"), 0, 2).?); 
}
