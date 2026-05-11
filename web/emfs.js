// this shares the emscripten FS with the WASI fs so both the cart and raylib share one filesystem

const FILETYPE_REGULAR_FILE = 4
const FILETYPE_DIRECTORY = 3

function absPath(p) {
  return p.startsWith('/') ? p : '/' + p
}

export default function createEmFs(FS) {
  return {
    statSync(path) {
      try {
        const st = FS.stat(absPath(path))
        const isFile = FS.isFile(st.mode)
        const isDir = FS.isDir(st.mode)
        const now = Math.floor(Date.now() / 1000)
        return {
          isFile: () => isFile,
          isDirectory: () => isDir,
          isSymbolicLink: () => false,
          isCharacterDevice: () => false,
          isBlockDevice: () => false,
          isFIFO: () => false,
          dev: st.dev,
          ino: st.ino,
          filetype: isFile ? FILETYPE_REGULAR_FILE : isDir ? FILETYPE_DIRECTORY : 0,
          nlink: st.nlink,
          size: st.size,
          atimeMs: now,
          mtimeMs: now,
          ctimeMs: now
        }
      } catch {
        throw Object.assign(new Error(`ENOENT: no such file or directory '${path}'`), { code: 'ENOENT' })
      }
    },
    readFileSync(path) {
      return FS.readFile(absPath(path))
    },
    writeFileSync(path, content) {
      const abs = absPath(path)
      const parts = abs.split('/').filter(Boolean)
      for (let i = 1; i < parts.length; i++) {
        try {
          FS.mkdir('/' + parts.slice(0, i).join('/'))
        } catch {}
      }
      FS.writeFile(abs, content)
    }
  }
}
