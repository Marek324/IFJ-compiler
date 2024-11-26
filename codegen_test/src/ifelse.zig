const ifj = @import("ifj24.zig");

pub fn main() void {
  const y = 12;
  const x = 3;
  if(x > y) {
    if (true and true){
        if(true){
            if(true == true){
                ifj.write(x);
            }
        }
    } 
  } else {
    ifj.write(y); 
  }
  ifj.write("\n");
}
