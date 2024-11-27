const ifj = @import("ifj24.zig");

pub fn main() void {
  const x : i32 = 24;
  const y = x;
  ifj.write(y);
  ifj.write("\n");
}
