const ifj = @import("ifj24.zig");

pub fn main() void {
  const y  = 24;
  const z  = 0;
  
  ifj.write(y/z); 
}
