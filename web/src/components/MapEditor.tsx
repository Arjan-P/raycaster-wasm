import { forwardRef, useEffect, useImperativeHandle, useRef } from "react";

export interface MapEditorHandle {
  getMap(): Uint8Array;
}

interface MapEditorProps {
  mapWidth: number;
  mapHeight: number;
}

export const MapEditor = forwardRef<MapEditorHandle, MapEditorProps>(
  ({ mapWidth, mapHeight }, ref) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const mapRef = useRef(
      new Uint8Array(mapWidth * mapHeight)
    );

    const cellSize = 20;

    useImperativeHandle(ref, () => ({
      getMap() {
        return mapRef.current;
      }
    }), []);

    useEffect(() => {
      const canvas = canvasRef.current;
      if (!canvas) return;

      canvas.width = mapWidth * cellSize;
      canvas.height = mapHeight * cellSize;
      const ctx = canvas.getContext("2d")!;
      ctx.clearRect(0, 0, canvas.width, canvas.height);

      // draw grid
      ctx.strokeStyle = "#e9e9e9";
      ctx.lineWidth = 1;

      for (let x = 0; x <= mapWidth; x++) {
        ctx.beginPath();
        ctx.moveTo(x * cellSize, 0);
        ctx.lineTo(x * cellSize, canvas.height);
        ctx.stroke();
      }

      for (let y = 0; y <= mapHeight; y++) {
        ctx.beginPath();
        ctx.moveTo(0, y * cellSize);
        ctx.lineTo(canvas.width, y * cellSize);
        ctx.stroke();
      }
      let isPainting = false;
      let lastIdx = -1;

      function paintCell(cx: number, cy: number) {
        if (cx < 0 || cy < 0 || cx >= mapWidth || cy >= mapHeight) return;

        const idx = cy * mapWidth + cx;
        if (idx === lastIdx) return;

        lastIdx = idx;
        mapRef.current[idx] ^= 1;

        ctx.fillStyle = mapRef.current[idx] ? "#326496" : "#4a5565";
        ctx.fillRect(
          cx * cellSize + 1,
          cy * cellSize + 1,
          cellSize - 2,
          cellSize - 2
        );
      }

      function getCellFromPoint(x: number, y: number) {
        if (!canvas) return;

        const rect = canvas.getBoundingClientRect();
        const px = x - rect.left;
        const py = y - rect.top;

        return {
          cx: Math.floor(px / cellSize),
          cy: Math.floor(py / cellSize),
        };
      }

      function onPointerDown(e: PointerEvent) {
        e.preventDefault();
        if (!canvas) return;
        canvas.setPointerCapture(e.pointerId);

        isPainting = true;
        lastIdx = -1;

        const cell = getCellFromPoint(e.clientX, e.clientY);
        if (!cell) return;

        paintCell(cell.cx, cell.cy);
      }

      function onPointerMove(e: PointerEvent) {
        if (!isPainting) return;

        const cell = getCellFromPoint(e.clientX, e.clientY);
        if (!cell) return;

        paintCell(cell.cx, cell.cy);
      }

      function onPointerUp(_e: PointerEvent) {
        isPainting = false;
        lastIdx = -1;
      }
      canvas.addEventListener("pointerdown", onPointerDown);
      canvas.addEventListener("pointermove", onPointerMove);
      canvas.addEventListener("pointerup", onPointerUp);
      canvas.addEventListener("pointercancel", onPointerUp);
      canvas.addEventListener("pointerleave", onPointerUp);
      return () => {
        canvas.removeEventListener("pointerdown", onPointerDown);
        canvas.removeEventListener("pointermove", onPointerMove);
        canvas.removeEventListener("pointerup", onPointerUp);
        canvas.removeEventListener("pointercancel", onPointerUp);
        canvas.removeEventListener("pointerleave", onPointerUp);
      };

    }, [mapWidth, mapHeight]);

    return (
      <div className="w-full overflow-auto rounded-lg border border-white/10">
        <canvas ref={canvasRef} style={{ touchAction: "none" }} />
      </div>
    );
  }
);

