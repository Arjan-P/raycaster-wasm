import { useNavigate } from "react-router-dom";
import { setMapBuffer } from "../wasm/mapStore";
import { useRef, useState } from "react";
import { MapEditor, type MapEditorHandle } from "../components/MapEditor";

export function Config() {
  const [mapHeight, setMapHeight] = useState<string | null>(null);
  const [mapWidth, setMapWidth] = useState<string | null>(null);
  const [showEditor, setShowEditor] = useState(false);
  const editorRef = useRef<MapEditorHandle>(null);
  const navigate = useNavigate();
  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (mapHeight && mapWidth) setShowEditor(true);
  }
  const handleClick = () => {
    const map = (editorRef.current?.getMap());
    if (!map) return;
    setMapBuffer(map, Number(mapWidth), Number(mapHeight));
    navigate("render");
  }
return (
  <section className="h-full w-full overflow-y-auto">
    <div
      className="
        min-h-full
        w-full
        flex
        flex-col
        items-center
        justify-center
        px-4
        lg:flex-row
        lg:items-start
        lg:justify-center
        gap-8
      "
    >
      <div
        className="
          w-full
          max-w-md
          p-6
          rounded-lg
          border
          border-white/10
          flex
          flex-col
          gap-5
          bg-gray-700
        "
      >
        {/* ===== Form ===== */}
        <form className="flex flex-col gap-4" onSubmit={handleSubmit}>
          <input
            type="number"
            min={1}
            placeholder="Enter Width"
            className="border rounded px-3 py-2"
            onChange={(e) => setMapWidth(e.target.value)}
          />
          <input
            type="number"
            min={1}
            placeholder="Enter Height"
            className="border rounded px-3 py-2"
            onChange={(e) => setMapHeight(e.target.value)}
          />
          <button className="buttonStyle" type="submit">
            Confirm
          </button>
        </form>

        {/* ===== Editor ===== */}
        {showEditor && (
          <div className="flex flex-col gap-4">
              <MapEditor
                ref={editorRef}
                mapHeight={Number(mapHeight)!}
                mapWidth={Number(mapWidth)!}
              />

            <button className="buttonStyle" onClick={handleClick}>
              Render
            </button>
          </div>
        )}
      </div>
    </div>
  </section>
);
}
