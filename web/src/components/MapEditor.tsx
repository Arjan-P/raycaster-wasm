import { forwardRef, useEffect, useImperativeHandle, useRef } from "react";

export interface MapEditorHandle {
  getMap(): Uint8Array;
}

interface MapEditorProps {
  mapWidth: number;
  mapHeight: number;
}
let isPainting = false;
let lastIdx = -1;



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

        ctx.fillStyle = mapRef.current[idx] ? "#326496" : "#ffffff";
        ctx.fillRect(
          cx * cellSize + 1,
          cy * cellSize + 1,
          cellSize - 2,
          cellSize - 2
        );
      }

      function getCell(e: MouseEvent) {
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        return {
          cx: Math.floor(x / cellSize),
          cy: Math.floor(y / cellSize),
        };
      }

      function onMouseDown(e: MouseEvent) {
        isPainting = true;
        lastIdx = -1;
        const { cx, cy } = getCell(e);
        paintCell(cx, cy);
      }

      function onMouseMove(e: MouseEvent) {
        if (!isPainting) return;
        const { cx, cy } = getCell(e);
        paintCell(cx, cy);
      }

      function stopPainting() {
        isPainting = false;
        lastIdx = -1;
      }

      canvas.addEventListener("mousedown", onMouseDown);
      canvas.addEventListener("mousemove", onMouseMove);
      canvas.addEventListener("mouseup", stopPainting);
      canvas.addEventListener("mouseleave", stopPainting);
      return () => {
        canvas.removeEventListener("mousedown", onMouseDown);
        canvas.removeEventListener("mousemove", onMouseMove);
        canvas.removeEventListener("mouseup", stopPainting);
        canvas.removeEventListener("mouseleave", stopPainting);
      };
    }, [mapWidth, mapHeight]);

    return <canvas ref={canvasRef} />;
  }
);

