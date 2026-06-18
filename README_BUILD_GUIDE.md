# UMKMudah — Panduan Build di Visual Studio

## Tentang Proyek
Aplikasi Android untuk UMKM dengan **logika bisnis ditulis dalam bahasa C** menggunakan Android NDK.

- **Bahasa inti:** C (file `umkmudah.c`)
- **Bridge:** JNI (Java Native Interface)
- **UI:** Java + XML Layouts
- **Output:** `umkmudah.apk`

---

## Prasyarat

| Tool | Versi | Download |
|------|-------|----------|
| Visual Studio | 2019 / 2022 | visualstudio.microsoft.com |
| Android SDK | API 21+ | via Android Studio atau SDK Manager |
| Android NDK | r25+ | via SDK Manager → SDK Tools |
| JDK | 17 atau 11 | adoptium.net |

---

## Langkah Setup Visual Studio

### 1. Install Workload Android
Buka Visual Studio Installer → centang:
- ✅ **Mobile development with C++**
- ✅ **Mobile development with .NET** (opsional, tapi membantu)

### 2. Install Android NDK via SDK Manager
Buka Android Studio → SDK Manager → SDK Tools:
- ✅ NDK (Side by side) versi 25.x
- ✅ CMake (walaupun kita pakai `ndk-build`, tetap install)

### 3. Konfigurasi `local.properties`
Edit file `local.properties` di root proyek:
```
sdk.dir=C:\Users\NamaKamu\AppData\Local\Android\Sdk
ndk.dir=C:\Users\NamaKamu\AppData\Local\Android\Sdk\ndk\25.2.9519653
```

---

## Build Menggunakan Visual Studio

### Cara A: Gradle dari Command Prompt
```cmd
cd UMKMudah
gradlew assembleDebug
```
APK output: `app\build\outputs\apk\debug\umkmudah.apk`

### Cara B: Buka sebagai Gradle Project di Visual Studio
1. Visual Studio → **Open → Folder** → pilih folder `UMKMudah`
2. VS otomatis deteksi proyek Gradle
3. Klik kanan `build.gradle` (app) → **Build**

### Cara C: Deploy Langsung ke Emulator/HP
1. Sambungkan HP (USB Debugging ON) atau jalankan emulator
2. Visual Studio → klik **▶ Run** → pilih device
3. Otomatis build + install APK

---

## Cara Kerja Kode C

```
umkmudah.c  ←── Semua logika bisnis ada di sini
     ↕ JNI
NativeLib.java  ←── Bridge deklarasi native methods
     ↕
Fragment Java  ←── Tampilkan data ke UI
```

File C utama: `app/src/main/jni/umkmudah.c`

Fungsi-fungsi C yang tersedia:
```c
// Pelanggan
Java_com_umkmudah_NativeLib_tambahPelanggan()
Java_com_umkmudah_NativeLib_getDaftarPelanggan()   // return JSON
Java_com_umkmudah_NativeLib_hapusPelanggan()
Java_com_umkmudah_NativeLib_getJumlahPelanggan()

// Stok Barang
Java_com_umkmudah_NativeLib_tambahBarang()
Java_com_umkmudah_NativeLib_updateStok()
Java_com_umkmudah_NativeLib_getDaftarBarang()      // return JSON
Java_com_umkmudah_NativeLib_getBarangStokRendah()

// Laporan Keuangan
Java_com_umkmudah_NativeLib_catatTransaksi()
Java_com_umkmudah_NativeLib_getLaporanKeuangan()   // return JSON
Java_com_umkmudah_NativeLib_getRiwayatTransaksi()  // return JSON
```

---

## Struktur File

```
UMKMudah/
├── app/src/main/
│   ├── jni/
│   │   ├── umkmudah.c        ← KODE C UTAMA
│   │   ├── Android.mk        ← NDK build config
│   │   └── Application.mk    ← Target ABI
│   ├── java/com/umkmudah/
│   │   ├── NativeLib.java    ← JNI bridge
│   │   ├── MainActivity.java
│   │   ├── DashboardFragment.java
│   │   ├── PelangganFragment.java
│   │   ├── StokFragment.java
│   │   └── LaporanFragment.java
│   ├── res/
│   │   ├── layout/           ← XML UI layouts
│   │   ├── drawable/         ← Gradient shapes
│   │   ├── values/           ← Colors, strings, themes
│   │   └── menu/             ← Bottom nav menu
│   └── AndroidManifest.xml
├── build.gradle              ← Gradle + NDK config
├── settings.gradle
├── local.properties          ← PATH ke SDK & NDK (wajib diisi)
└── README_BUILD_GUIDE.md     ← File ini
```

---

## Fitur Aplikasi

### 🏠 Dashboard
- Ringkasan pendapatan, pengeluaran, laba bersih
- Statistik jumlah pelanggan, produk, stok rendah
- Data real-time dari kode C

### 👥 Manajemen Pelanggan
- Tambah pelanggan (nama, telepon, alamat)
- Lihat total belanja & jumlah transaksi per pelanggan
- Hapus pelanggan

### 📦 Pencatatan Stok Barang
- Tambah produk (nama, kategori, harga beli/jual, stok, satuan)
- Update stok (+10 / -1)
- Warning merah jika stok di bawah minimum

### 📊 Laporan Keuangan
- Rekap pendapatan, pengeluaran, laba bersih, nilai stok
- Riwayat transaksi lengkap (terbaru dulu)
- Warna hijau = penjualan masuk, merah = pembelian stok

---

## Troubleshooting

| Error | Solusi |
|-------|--------|
| `NDK not found` | Cek `ndk.dir` di `local.properties` |
| `UnsatisfiedLinkError` | Pastikan nama fungsi di `.c` match dengan package Java |
| `minSdk` error | Ubah ke `minSdk 21` di `build.gradle` |
| Build gagal di VS | Coba `gradlew clean assembleDebug` dari CMD |

---

*UMKMudah v1.0 — Logika bisnis 100% ditulis dalam C via Android NDK*
