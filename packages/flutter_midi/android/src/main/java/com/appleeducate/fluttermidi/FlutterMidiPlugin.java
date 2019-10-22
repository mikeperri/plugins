package com.appleeducate.fluttermidi;

import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.MethodChannel.MethodCallHandler;
import io.flutter.plugin.common.MethodChannel.Result;
import io.flutter.plugin.common.PluginRegistry.Registrar;
// import android.content.res.AssetManager;

/** FlutterMidiPlugin */
public class FlutterMidiPlugin implements MethodCallHandler {
  // static {
  //   System.loadLibrary("oboe_bridge");
  // }

  /** Plugin registration. */
  public static void registerWith(Registrar registrar) {
    final MethodChannel channel = new MethodChannel(registrar.messenger(), "flutter_midi");
    channel.setMethodCallHandler(new FlutterMidiPlugin());
  }

  @Override
  public void onMethodCall(MethodCall call, Result result) {
    if (call.method.equals("setupAssetManager")) {
      // native_onStart(assetManager);
      result.success(null);
    } else {
      result.notImplemented();
    }
  }

  // private native void native_onStart(AssetManager assetManager);
}
