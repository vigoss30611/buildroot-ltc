package com.infotm.dv.camera;

import com.infotm.dv.camera.InfotmCamera;

public abstract interface ICameraStateFetcher {
  public abstract void destroy();

  public abstract void fetchState(InfotmCamera iCamera);

  public abstract boolean init();

  public abstract void reset();
}