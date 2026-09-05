# Test Button (Geode Mod)

Mod tes sederhana buat Geometry Dash pakai [Geode](https://geode-sdk.org/). Cuma
nambahin satu tombol bulat hijau di menu utama — diklik bakal muncul popup
"Tes Berhasil!". Cocok buat ngetes environment build Geode kamu sudah beres.

## Isi folder

```
geode-test-mod/
├── mod.json          # Manifest mod (ID, versi, target GD, dsb)
├── CMakeLists.txt    # Script build CMake
├── src/
│   └── main.cpp      # Logic mod: nambahin tombol ke MenuLayer
└── resources/        # (kosong, sediakan sendiri kalau mau sprite custom)
```

## Cara build (opsi A — GitHub Actions, RECOMMENDED buat Android64)

Ini cara paling gampang biar dapet `.geode` yang support semua platform
(Windows, macOS, Android32, **Android64**) tanpa perlu install NDK manual:

1. Buat repo baru di GitHub (public/private bebas).
2. Push seluruh isi folder ini ke repo tsb (dari PC atau dari Termux, lihat
   contoh command di bawah).
3. Buka tab **Actions** di repo GitHub kamu — workflow "Build Geode Mod"
   otomatis jalan begitu ada push.
4. Tunggu build selesai (~5-10 menit, ngebuild 4 platform sekaligus lewat
   `.github/workflows/build.yml`).
5. Buka run yang selesai → scroll ke bagian **Artifacts** → download.
   Di dalamnya ada file `.geode` combined yang sudah bawa binary
   `android64.so` — tinggal taruh di folder `mods/` Geode di HP kamu.

Contoh push dari Termux/terminal:
```bash
cd geode-test-mod
git init
git add .
git commit -m "init"
git branch -M main
git remote add origin https://github.com/USERNAME/NAMA-REPO.git
git push -u origin main
```
Ganti `USERNAME` dan `NAMA-REPO` sesuai punya kamu. Setelah push pertama,
workflow otomatis trigger — nggak perlu command tambahan lagi.

## Cara build (opsi B — lokal manual, PC only, nggak dapet Android64)

1. Install [Geode CLI](https://docs.geode-sdk.org/getting-started/geode-cli) kalau belum ada.
2. Install Geode SDK: `geode sdk install`
3. Dari dalam folder `geode-test-mod/`, jalankan:
   ```bash
   geode build
   ```
   atau kalau mau custom CMake manual:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build . --config Release
   ```
4. File `.geode` hasil build ada di `build/`. Tinggal double-click atau taruh
   di folder `mods/` Geode kamu, lalu jalankan Geometry Dash.

## Custom sprite (opsional)

Kalau mau pakai gambar sendiri buat ikon tombol, taruh file PNG (misal
`button.png`) di folder `resources/`, lalu di `src/main.cpp` ganti bagian:

```cpp
CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png")
```

jadi:

```cpp
CCSprite::create("button.png"_spr)
```

## Catatan

- Target versi GD di `mod.json` di-set ke `2.2074`. Sesuaikan kalau versi
  game kamu beda.
- ID mod (`com.clausewitz.testbutton`) unik untuk kamu — ganti kalau mau
  publish ke index Geode biar nggak bentrok sama mod orang lain.
