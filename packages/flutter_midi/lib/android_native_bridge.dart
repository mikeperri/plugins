import 'dart:async';
import 'dart:ffi';

import 'package:flutter/services.dart';
import 'structs.dart';

final DynamicLibrary androidTsfPlayerLib =
  DynamicLibrary.open("libandroid_tsf_player.so");

final int Function(Pointer<Utf8>) nLoadSoundfont =
  androidTsfPlayerLib
    .lookup<NativeFunction<Int32 Function(Pointer<Utf8>)>>("load_soundfont")
    .asFunction();

final int Function(int, int, int, num) nNoteOn =
  androidTsfPlayerLib
    .lookup<NativeFunction<Int32 Function(Uint8, Uint8, Int64, Float)>>("note_on")
    .asFunction();

final int Function(int, int, int) nNoteOff =
  androidTsfPlayerLib
    .lookup<NativeFunction<Int32 Function(Uint8, Uint8, Int64)>>("note_off")
    .asFunction();

class AndroidNativeBridge {
  // static const MethodChannel _channel =
  //   const MethodChannel('flutter_low_latency_audio');

  static String prepareMidi(String soundfontPath) {
    // _setupAssetManager();
    nLoadSoundfont(Utf8.toUtf8(soundfontPath));

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
