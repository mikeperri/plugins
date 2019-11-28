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
  // static const MethodChannel _channel =
  //   const MethodChannel('flutter_low_latency_audio');

  static String prepareMidi(String soundfontPath) {
    // _setupAssetManager();
    var utf8Path = Utf8.toUtf8(soundfontPath);

    var result = nPrepareMidi(utf8Path);
    print('prepare midi result: ' + result.toString());

    return '';
  }

  static String changeSound(String soundfontPath) {

    return '';
  }

  static String unmute() {

    return '';
  }

  static String playMidiNote(int patchIndex, int pitch, int delayMs, num velocity) {
    nNoteOn(patchIndex, pitch, delayMs, velocity);
    return '';
  }

  static String stopMidiNote(int patchIndex, int pitch, int delayMs) {
    nNoteOff(patchIndex, pitch, delayMs);
    return '';
  }

  // Future<void> _setupAssetManager() async {
  //   return await _channel.invokeMethod('setupAssetManager');
  // }
}
