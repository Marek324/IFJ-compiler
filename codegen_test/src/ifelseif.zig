const ifj = @import("ifj24.zig");

pub fn main() void {
  const y = 12;
  const x = 3;
  const z = 4;
  if(x > y) {
    ifj.write(x); 
  } else if(z > y) {
    ifj.write(z);
  } else {
    ifj.write(y); 
  }
  ifj.write("\n");
}
