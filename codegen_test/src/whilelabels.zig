const ifj = @import("ifj24.zig");

pub fn main() void {
  var i: i32 = 0;
    nazev: while (i < 2000000){
        b:while (i < 10) : ({
            if (i == 6) {
                break :nazev;
            }
        }) {
            ifj.write(i); ifj.write("\n");
            i = i + 2;
            continue :b;
        }else{
            ifj.write("i na konci whilu a: "); ifj.write(i); ifj.write("\n");
        }
    }

    b:while (i < 10){
        i = i + 1;
        continue : b;
    }else{
        ifj.write("i na konci whilu b: "); ifj.write(i); ifj.write("\n");
    }
}
