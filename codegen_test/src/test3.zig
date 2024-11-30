const ifj = @import("ifj24.zig");

pub fn main() void {
  const a = 12;
  const b = 6;
  while (true){
  if (a < b){
    const c = a+b;
    ifj.write(c);
  }
  if (a > b){
    const c = a-b;
    ifj.write(c);
  }
  if (a == b){
    const c = a*b;
    ifj.write(c);
  }
  ifj.write("\n");
  break;
  }
}
