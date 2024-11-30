const ifj = @import("ifj24.zig");

pub fn main() void {
  const y  = true;
  const z  = false;
  
  const x = y or z;

  if (x){
    ifj.write(x);
  } else {}
  ifj.write("\n");
}
