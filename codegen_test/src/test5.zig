const ifj = @import("ifj24.zig");

pub fn main() void {
   print_board(daco());
}

pub fn print_board(a:i32) void{
    ifj.write(a);
}

pub fn daco () i32{
    return 2;
}