import 'dart:async';
import 'dart:io';
import 'package:flutter/material.dart';

import 'package:flutter/services.dart';
import 'local_storage.dart';
import 'native_bridge.dart';

class FlutterMidi {
  /// Needed so that the sound font is loaded
  /// On iOS make sure to include the sound_font.SF2 in the Runner folder.
  /// This does not work in the simulator.
  static Future<String> prepare(
      {@required ByteData sf2, String name = "instrument.sf2"}) async {
    File _file = await writeToFile(sf2, name: name);

    NativeBridge.prepareMidi(_file.path);

    return '';
  }

  /// Needed so that the sound font is loaded
  /// On iOS make sure to include the sound_font.SF2 in the Runner folder.
  /// This does not work in the simulator.
  static Future<String> changeSound(
      {@required ByteData sf2, String name = "instrument.sf2"}) async {
    File _file = await writeToFile(sf2, name: name);

    NativeBridge.changeSound(_file.path);

    return '';
  }

  /// Unmute the device temporarly even if the mute switch is on or toggled in settings.
  static Future<String> unmute() async {
    NativeBridge.unmute();

    return '';
  }

  /// Use this when stopping the sound onTouchUp or to cancel a long file.
  /// Not needed if playing midi onTap.
  static Future<String> stopMidiNote({
    @required int midi,
  }) async {
    NativeBridge.stopMidiNote(0, midi, 0);

    return '';
  }

  /// Play a midi note from the sound_font.SF2 library bundled with the application.
  /// Play a midi note in the range between 0-256
  /// Multiple notes can be played at once as seperate calls.
  static Future<String> playMidiNote({
    @required int midi,
  }) async {
    NativeBridge.playMidiNote(0, midi, 0, 1.0);

    return '';
  }
}
