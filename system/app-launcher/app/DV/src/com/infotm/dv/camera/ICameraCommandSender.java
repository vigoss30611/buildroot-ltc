
package com.infotm.dv.camera;

import android.util.Pair;
//import com.gopro.wsdk.domain.camera.constants.CameraModes;
//import com.gopro.wsdk.domain.camera.constants.CameraModes.ModeGroup;

public abstract interface ICameraCommandSender {
    public abstract boolean cancelOta();

    public abstract Pair<Boolean, Number> enterOtaMode();

    // public abstract boolean resetProtune(CameraModes paramCameraModes);

    public abstract boolean sendCommand(String paramString);

    public abstract boolean sendCommand(String paramString, int paramInt);

    public abstract boolean sendCommand(String paramString1, String paramString2,
            int paramParameterFlag);//ParameterFlag

    public abstract Pair<Boolean, Number> sendCommandHttpResponse(String paramString, int paramInt);

    public abstract boolean setCameraPower(boolean paramBoolean);

    // public abstract boolean setMode(CameraModes paramCameraModes);
    //
    // public abstract boolean setModeGroup(CameraModes.ModeGroup
    // paramModeGroup);

    public abstract boolean setWifiConfig(String paramString1, String paramString2);
}
