const ifj = @import("ifj24.zig");

pub fn main() void {
  const y : bool = true and false;
  ifj.write(y);
  ifj.write("\n");
}
