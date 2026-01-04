import WasiPreview1 from "@easywasm/wasi";
import raycart from "./raycart.mjs";

const debug = false;

export default async function (carts = [], canvas) {
  const wasi_snapshot_preview1 = new WasiPreview1();

  const host = await raycart({
    canvas,
    arguments: carts.map((c) => c[0]),
    preRun(h) {
      for (const c of carts) {
        h.FS.writeFile(c[0], c[1]);
      }
    },
  });

  host.imports = { wasi_snapshot_preview1, raycart: {} };

  function loadBytesFromCart(filename) {
    const sizePtr = host._malloc(4);
    const fPtr = host.stringToNewUTF8(filename);
    const bPtr = host._LoadFileData(fPtr, sizePtr);
    const size = host.HEAPU32[sizePtr / 4];
    host._free(fPtr);
    host._free(sizePtr);
    let fileBytes = new Uint8Array();
    if (bPtr) {
      fileBytes = host.HEAPU8.slice(bPtr, bPtr + size);
      host._free(bPtr);
    }
    return fileBytes;
  }

  // instead of using LoadFileData/physfs to get main.wasm, I could load a zip FS in host and share it with cart WASI
  // this is simpler & smaller, but does not allow cart to access files directly (outside of raylib functions)

  const cartBytes = loadBytesFromCart("main.wasm");

  return host;
}
