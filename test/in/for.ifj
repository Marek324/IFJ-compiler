const ifj = @import("ifj24.zig");
pub fn main() void {
    const str: []u8 = ifj.string("rete zec");
    for (str) |temp| {
        const character: i32 = @as(i32, temp);
        if (character == 32) {
            ifj.write("Nemam rad mezery.\n");
            continue;
        } else {}
        ifj.write("Ordinalni hodnota: ");
        ifj.write(character);
        ifj.write(".\n");
    }
    ifj.write("koteseni\n");
}
