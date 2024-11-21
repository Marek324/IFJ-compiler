const ifj = @import("ifj24.zig");

pub fn main() void {
  const y : bool = (a+b*c)==12 and true;
  y = a * add(2,3);
  ifj.write(y);
  ifj.write("\n");
}
