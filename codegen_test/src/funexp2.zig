const ifj = @import("ifj24.zig");

pub fn main() void {
  ifj.write(ifj.concat(ifj.substring(ifj.string("abcd"), 0, 2).?, ifj.string("cd\n"))); 
}
