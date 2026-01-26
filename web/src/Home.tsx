import { useEffect, useRef } from "react";
import { createEngineRuntime } from "./wasm/engine";

export function Home() {
  const screenRef = useRef<HTMLCanvasElement>(null);
  const mapRef = useRef<HTMLCanvasElement>(null);
  const engineRef = useRef<any>(null);
  const map = new Uint8Array(
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1]
  )

  useEffect(() => {
    if (!screenRef.current || !mapRef.current) return;

    let alive = true;

    createEngineRuntime(screenRef.current, mapRef.current)
      .then(engine => {
        if (!alive) return;
        engineRef.current = engine;
        engine.initEngine({px: 4, py: 4, pa: 0});
        engine.setMap(map, 8, 9);
        engine.start();
      });

    return () => {
      alive = false;
      engineRef.current?.stop();
    };
  }, []);

  useEffect(() => {
    function onKey(e: KeyboardEvent) {
      const engine = engineRef.current;
      if (!engine) return;

      if (e.key === "w") engine.move( 0.5);
      if (e.key === "s") engine.move(-0.5);
      if (e.key === "a") engine.rotate(-10);
      if (e.key === "d") engine.rotate(10);
    }

    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, []);

  return (
    <div>
      <canvas ref={screenRef} />
      <canvas ref={mapRef} />
    </div>
  );
}
