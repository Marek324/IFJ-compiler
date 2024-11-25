const ifj = @import("ifj24.zig");

pub fn main() void {
  const y = 12;
  const x = 3;
  if(x < y) {
    ifj.write(x); 
  } 
  ifj.write("\n");
}
