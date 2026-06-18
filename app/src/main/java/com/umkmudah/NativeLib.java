package com.umkmudah;

/**
 * Bridge antara Java/Android dan kode C (NDK)
 * Semua logika bisnis ada di umkmudah.c
 */
public class NativeLib {

    static {
        System.loadLibrary("umkmudah");
    }

    // ── Info ──────────────────────────────────────────
    public native String getVersi();
    public native void   resetData();
    public native void   seedDemoData();

    // ── Pelanggan ─────────────────────────────────────
    /** Tambah pelanggan baru. Return id baru, atau -1 jika gagal. */
    public native int     tambahPelanggan(String nama, String telepon, String alamat);
    /** Return JSON array pelanggan */
    public native String  getDaftarPelanggan();
    /** Hapus pelanggan berdasarkan id */
    public native boolean hapusPelanggan(int id);
    /** Jumlah pelanggan terdaftar */
    public native int     getJumlahPelanggan();

    // ── Stok Barang ───────────────────────────────────
    /** Tambah barang baru. Return id baru, atau -1 jika gagal. */
    public native int     tambahBarang(String nama, String kategori,
                                       double hargaBeli, double hargaJual,
                                       int stok, int stokMin, String satuan);
    /** Update stok (delta positif = tambah, negatif = kurangi) */
    public native boolean updateStok(int id, int delta);
    /** Return JSON array barang */
    public native String  getDaftarBarang();
    /** Jumlah barang dengan stok di bawah minimum */
    public native int     getBarangStokRendah();

    // ── Keuangan ──────────────────────────────────────
    /**
     * Catat transaksi penjualan/pembelian.
     * jenis: "keluar" = penjualan, "masuk" = pembelian/stok masuk
     * Return id transaksi, atau negatif jika gagal.
     */
    public native int    catatTransaksi(int pelangganId, int barangId,
                                        int jumlah, String jenis);
    /** Return JSON ringkasan keuangan */
    public native String getLaporanKeuangan();
    /** Return JSON array riwayat transaksi (terbaru dulu) */
    public native String getRiwayatTransaksi();
}
