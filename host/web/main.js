import loadRaycart from './index.js'

const cartName = new URLSearchParams(location.search).get('cart') ?? 'basic.wasm'
loadRaycart(new URL(`./carts/${cartName}`, import.meta.url))
