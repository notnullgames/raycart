import raycart from "./raycart.mjs";

export default async function (carts = [], canvas) {
  // TODO: put carts into file in emscripten FS and set arguments to name
  const host = await raycart({ canvas });

  // TODO: load cart main.wasm and connect to host

  return host;
}
