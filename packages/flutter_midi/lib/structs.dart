import 'dart:ffi';
import 'dart:convert';

class Utf8 extends Struct<Utf8> {
  @Uint8()
  int char;

  static String fromUtf8(Pointer<Utf8> ptr) {
    final units = List<int>();
    var len = 0;
    while (true) {
      final char = ptr.elementAt(len++).load<Utf8>().char;
      if (char == 0) break;
      units.add(char);
    }
    return Utf8Decoder().convert(units);
  }

  static Pointer<Utf8> toUtf8(String s) {
    final units = Utf8Encoder().convert(s);
    final ptr = Pointer<Utf8>.allocate(count: units.length + 1);
    for (var i = 0; i < units.length; i++) {
      ptr.elementAt(i).load<Utf8>().char = units[i];
    }
    // Add the C string null terminator '\0'
    ptr.elementAt(units.length).load<Utf8>().char = 0;
    return ptr;
  }
}