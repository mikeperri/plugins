import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';

final DynamicLibrary nativeLib =
  Platform.isAndroid
    ? DynamicLibrary.open("libandroid_tsf_player.so")
    : DynamicLibrary.executable();

final nPrepareMidi =
  nativeLib
    .lookupFunction<
      Int32 Function(Pointer<Utf8>),
      int Function(Pointer<Utf8>)
    >("prepareMidi");

final nChangeSound =
  nativeLib
    .lookupFunction<
      Int32 Function(Pointer<Utf8>, Int64),
      int Function(Pointer<Utf8>, int)
    >("changeSound");

final nUnmute =
  nativeLib
    .lookupFunction<
      Int32 Function(),
      int Function()
    >("unmute");

final nNoteOn =
  nativeLib
    .lookupFunction<
      Int32 Function(Uint8, Uint8, Int64, Float),
      int Function(int, int, int, double)
    >("noteOn");

final nNoteOff =
  nativeLib
    .lookupFunction<
      Int32 Function(Uint8, Uint8, Int64),
      int Function(int, int, int)
    >("noteOff");

class NativeBridge {
  static int prepareMidi(String soundfontPath) {
    var utf8Path = Utf8.toUtf8(soundfontPath);

    return nPrepareMidi(utf8Path);
  }

  static int changeSound(String soundfontPath) {
    var utf8Path = Utf8.toUtf8(soundfontPath);

    return nChangeSound(utf8Path, 0);
  }

  static int unmute() {
    return nUnmute();
  }

  static int playMidiNote(int patchIndex, int pitch, int delayMs, num velocity) {
    return nNoteOn(patchIndex, pitch, delayMs, velocity);
  }

  static int stopMidiNote(int patchIndex, int pitch, int delayMs) {
    return nNoteOff(patchIndex, pitch, delayMs);
  }
}
