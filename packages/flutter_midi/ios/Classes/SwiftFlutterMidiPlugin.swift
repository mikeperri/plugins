import Flutter
import UIKit
import AVFoundation

var au: AudioUnitMIDISynth?

@_cdecl("prepareMidi")
public func prepareMidi(filenamePtr: UnsafePointer<CChar>) -> Int32 {
    if let filename = String(validatingUTF8: filenamePtr) {
        let url = URL(fileURLWithPath: filename)
        au = AudioUnitMIDISynth(soundfont: url)

        return 0
    } else {
        return 1
    }
}

@_cdecl("changeSound")
public func changeSound(filenamePtr: UnsafePointer<CChar>) -> Int32 {
    if let filename = String(validatingUTF8: filenamePtr), let au = au {
        let url = URL(fileURLWithPath: filename)
        au.prepare(soundfont: url)

        return 0
    } else {
        return 1
    }
}

@_cdecl("unmute")
public func unmute() -> Int32 {
    do {
        try AVAudioSession.sharedInstance().setCategory(AVAudioSessionCategoryPlayback)
    } catch {
        print(error)
        return 1
    }

    return 0
}

@_cdecl("noteOn")
public func noteOn(patchIndex: UInt8, pitch: UInt8, delayMs: Int16, velocity: Float) -> Int32 {
    if let au = au {
        au.playPitch(midi: Int(pitch))

        return 0
    } else {
        return 1
    }
}

@_cdecl("noteOff")
public func noteOff(patchIndex: UInt8, pitch: UInt8, delayMs: Int64) -> Int32 {
    if let au = au {
        au.stopPitch(midi: Int(pitch))

        return 0
    } else {
        return 1
    }
}
