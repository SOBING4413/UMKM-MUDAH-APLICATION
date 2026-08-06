/*
 * UMKMudah - Aplikasi UMKM Android
 * Ditulis dalam bahasa C menggunakan Android NDK
 * Build via Visual Studio dengan Android NDK support
 *
 * Fitur:
 *   1. Manajemen Pelanggan
 *   2. Pencatatan Stok Barang
 *   3. Laporan Keuangan
 */

#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <android/log.h>

#define LOG_TAG "UMKMudah"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/* ============================================================
 * STRUKTUR DATA
 * ============================================================ */

#define MAX_PELANGGAN   200
#define MAX_BARANG      300
#define MAX_TRANSAKSI   500
#define MAX_STR         128

typedef struct {
    int     id;
    char    nama[MAX_STR];
    char    telepon[32];
    char    alamat[MAX_STR];
    double  total_belanja;
    int     jumlah_transaksi;
    char    bergabung[20];   /* "YYYY-MM-DD" */
} Pelanggan;

typedef struct {
    int     id;
    char    nama[MAX_STR];
    char    kategori[64];
    double  harga_beli;
    double  harga_jual;
    int     stok;
    int     stok_minimum;
    char    satuan[32];
} Barang;

typedef struct {
    int     id;
    int     pelanggan_id;
    int     barang_id;
    int     jumlah;
    double  subtotal;
    char    tanggal[20];
    char    jenis[16];   /* "masuk" | "keluar" */
} Transaksi;

/* ============================================================
 * STORAGE GLOBAL (in-memory, disimpan via JNI ke SharedPreferences)
 * ============================================================ */

static Pelanggan   g_pelanggan[MAX_PELANGGAN];
static int         g_jumlah_pelanggan = 0;

static Barang      g_barang[MAX_BARANG];
static int         g_jumlah_barang = 0;

static Transaksi   g_transaksi[MAX_TRANSAKSI];
static int         g_jumlah_transaksi = 0;

static int         g_next_id = 1;

/* ============================================================
 * UTILITAS
 * ============================================================ */

static int next_id() {
    return g_next_id++;
}

static void tanggal_sekarang(char *buf, int buf_size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info == NULL || strftime(buf, buf_size, "%Y-%m-%d", tm_info) == 0) {
        snprintf(buf, buf_size, "1970-01-01");
    }
}

static void copy_string(char *dest, size_t dest_size, const char *src) {
    if (dest_size == 0) return;
    if (src == NULL) src = "";
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* ============================================================
 * MODUL: MANAJEMEN PELANGGAN
 * ============================================================ */

JNIEXPORT jint JNICALL
Java_com_umkmudah_NativeLib_tambahPelanggan(
        JNIEnv *env, jobject obj,
        jstring j_nama, jstring j_telepon, jstring j_alamat) {

    if (g_jumlah_pelanggan >= MAX_PELANGGAN) {
        LOGE("Pelanggan penuh!");
        return -1;
    }

    if (j_nama == NULL || j_telepon == NULL || j_alamat == NULL) {
        return -1;
    }

    const char *nama    = (*env)->GetStringUTFChars(env, j_nama, NULL);
    const char *telepon = (*env)->GetStringUTFChars(env, j_telepon, NULL);
    const char *alamat  = (*env)->GetStringUTFChars(env, j_alamat, NULL);
    if (nama == NULL || telepon == NULL || alamat == NULL) {
        if (nama) (*env)->ReleaseStringUTFChars(env, j_nama, nama);
        if (telepon) (*env)->ReleaseStringUTFChars(env, j_telepon, telepon);
        if (alamat) (*env)->ReleaseStringUTFChars(env, j_alamat, alamat);
        return -1;
    }

    Pelanggan *p = &g_pelanggan[g_jumlah_pelanggan];
    p->id                 = next_id();
    p->total_belanja      = 0.0;
    p->jumlah_transaksi   = 0;
    copy_string(p->nama,    sizeof(p->nama),    nama);
    copy_string(p->telepon, sizeof(p->telepon), telepon);
    copy_string(p->alamat,  sizeof(p->alamat),  alamat);
    tanggal_sekarang(p->bergabung, sizeof(p->bergabung));

    g_jumlah_pelanggan++;

    (*env)->ReleaseStringUTFChars(env, j_nama,    nama);
    (*env)->ReleaseStringUTFChars(env, j_telepon, telepon);
    (*env)->ReleaseStringUTFChars(env, j_alamat,  alamat);

    LOGI("Pelanggan ditambahkan: %s (id=%d)", p->nama, p->id);
    return (jint)p->id;
}

JNIEXPORT jstring JNICALL
Java_com_umkmudah_NativeLib_getDaftarPelanggan(JNIEnv *env, jobject obj) {
    /* Return JSON array string */
    char *buf = (char *)malloc(MAX_PELANGGAN * 256);
    if (!buf) return (*env)->NewStringUTF(env, "[]");

    int pos = 0;
    pos += sprintf(buf + pos, "[");
    for (int i = 0; i < g_jumlah_pelanggan; i++) {
        Pelanggan *p = &g_pelanggan[i];
        if (i > 0) pos += sprintf(buf + pos, ",");
        pos += sprintf(buf + pos,
            "{\"id\":%d,\"nama\":\"%s\",\"telepon\":\"%s\","
            "\"alamat\":\"%s\",\"total_belanja\":%.0f,"
            "\"jumlah_transaksi\":%d,\"bergabung\":\"%s\"}",
            p->id, p->nama, p->telepon, p->alamat,
            p->total_belanja, p->jumlah_transaksi, p->bergabung);
    }
    pos += sprintf(buf + pos, "]");

    jstring result = (*env)->NewStringUTF(env, buf);
    free(buf);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_com_umkmudah_NativeLib_hapusPelanggan(JNIEnv *env, jobject obj, jint id) {
    for (int i = 0; i < g_jumlah_pelanggan; i++) {
        if (g_pelanggan[i].id == (int)id) {
            /* Geser array */
            for (int j = i; j < g_jumlah_pelanggan - 1; j++) {
                g_pelanggan[j] = g_pelanggan[j + 1];
            }
            g_jumlah_pelanggan--;
            LOGI("Pelanggan id=%d dihapus", (int)id);
            return JNI_TRUE;
        }
    }
    return JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_com_umkmudah_NativeLib_getJumlahPelanggan(JNIEnv *env, jobject obj) {
    return (jint)g_jumlah_pelanggan;
}

/* ============================================================
 * MODUL: PENCATATAN STOK BARANG
 * ============================================================ */

JNIEXPORT jint JNICALL
Java_com_umkmudah_NativeLib_tambahBarang(
        JNIEnv *env, jobject obj,
        jstring j_nama, jstring j_kategori,
        jdouble harga_beli, jdouble harga_jual,
        jint stok, jint stok_min, jstring j_satuan) {

    if (g_jumlah_barang >= MAX_BARANG) {
        LOGE("Stok barang penuh!");
        return -1;
    }

    if (j_nama == NULL || j_kategori == NULL || j_satuan == NULL || stok < 0 || stok_min < 0 || harga_beli < 0 || harga_jual < 0) {
        return -1;
    }

    const char *nama     = (*env)->GetStringUTFChars(env, j_nama,     NULL);
    const char *kategori = (*env)->GetStringUTFChars(env, j_kategori, NULL);
    const char *satuan   = (*env)->GetStringUTFChars(env, j_satuan,   NULL);
    if (nama == NULL || kategori == NULL || satuan == NULL) {
        if (nama) (*env)->ReleaseStringUTFChars(env, j_nama, nama);
        if (kategori) (*env)->ReleaseStringUTFChars(env, j_kategori, kategori);
        if (satuan) (*env)->ReleaseStringUTFChars(env, j_satuan, satuan);
        return -1;
    }

    Barang *b = &g_barang[g_jumlah_barang];
    b->id           = next_id();
    b->harga_beli   = (double)harga_beli;
    b->harga_jual   = (double)harga_jual;
    b->stok         = (int)stok;
    b->stok_minimum = (int)stok_min;
    copy_string(b->nama,     sizeof(b->nama),     nama);
    copy_string(b->kategori, sizeof(b->kategori), kategori);
    copy_string(b->satuan,   sizeof(b->satuan),   satuan);

    g_jumlah_barang++;

    (*env)->ReleaseStringUTFChars(env, j_nama,     nama);
    (*env)->ReleaseStringUTFChars(env, j_kategori, kategori);
    (*env)->ReleaseStringUTFChars(env, j_satuan,   satuan);

    LOGI("Barang ditambahkan: %s (id=%d, stok=%d)", b->nama, b->id, b->stok);
    return (jint)b->id;
}

JNIEXPORT jboolean JNICALL
Java_com_umkmudah_NativeLib_updateStok(
        JNIEnv *env, jobject obj, jint id, jint delta) {
    for (int i = 0; i < g_jumlah_barang; i++) {
        if (g_barang[i].id == (int)id) {
            g_barang[i].stok += (int)delta;
            if (g_barang[i].stok < 0) g_barang[i].stok = 0;
            LOGI("Stok barang id=%d diupdate menjadi %d", (int)id, g_barang[i].stok);
            return JNI_TRUE;
        }
    }
    return JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_umkmudah_NativeLib_getDaftarBarang(JNIEnv *env, jobject obj) {
    char *buf = (char *)malloc(MAX_BARANG * 256);
    if (!buf) return (*env)->NewStringUTF(env, "[]");

    int pos = 0;
    pos += sprintf(buf + pos, "[");
    for (int i = 0; i < g_jumlah_barang; i++) {
        Barang *b = &g_barang[i];
        int low_stock = (b->stok <= b->stok_minimum) ? 1 : 0;
        if (i > 0) pos += sprintf(buf + pos, ",");
        pos += sprintf(buf + pos,
            "{\"id\":%d,\"nama\":\"%s\",\"kategori\":\"%s\","
            "\"harga_beli\":%.0f,\"harga_jual\":%.0f,"
            "\"stok\":%d,\"stok_minimum\":%d,\"satuan\":\"%s\","
            "\"low_stock\":%d}",
            b->id, b->nama, b->kategori,
            b->harga_beli, b->harga_jual,
            b->stok, b->stok_minimum, b->satuan, low_stock);
    }
    pos += sprintf(buf + pos, "]");

    jstring result = (*env)->NewStringUTF(env, buf);
    free(buf);
    return result;
}

JNIEXPORT jint JNICALL
Java_com_umkmudah_NativeLib_getBarangStokRendah(JNIEnv *env, jobject obj) {
    int count = 0;
    for (int i = 0; i < g_jumlah_barang; i++) {
        if (g_barang[i].stok <= g_barang[i].stok_minimum) count++;
    }
    return (jint)count;
}

/* ============================================================
 * MODUL: LAPORAN KEUANGAN
 * ============================================================ */

JNIEXPORT jint JNICALL
Java_com_umkmudah_NativeLib_catatTransaksi(
        JNIEnv *env, jobject obj,
        jint pelanggan_id, jint barang_id,
        jint jumlah, jstring j_jenis) {

    if (g_jumlah_transaksi >= MAX_TRANSAKSI) {
        LOGE("Transaksi penuh!");
        return -1;
    }

    if (jumlah <= 0 || j_jenis == NULL) {
        return -1;
    }

    const char *jenis = (*env)->GetStringUTFChars(env, j_jenis, NULL);
    if (jenis == NULL) {
        return -1;
    }
    if (strcmp(jenis, "keluar") != 0 && strcmp(jenis, "masuk") != 0) {
        (*env)->ReleaseStringUTFChars(env, j_jenis, jenis);
        return -3;
    }

    /* Cari barang */
    Barang *b = NULL;
    for (int i = 0; i < g_jumlah_barang; i++) {
        if (g_barang[i].id == (int)barang_id) {
            b = &g_barang[i];
            break;
        }
    }
    if (!b) {
        (*env)->ReleaseStringUTFChars(env, j_jenis, jenis);
        return -2;
    }

    if (strcmp(jenis, "keluar") == 0 && b->stok < (int)jumlah) {
        (*env)->ReleaseStringUTFChars(env, j_jenis, jenis);
        return -4;
    }

    double subtotal = (strcmp(jenis, "keluar") == 0 ? b->harga_jual : b->harga_beli) * (double)jumlah;

    Transaksi *t = &g_transaksi[g_jumlah_transaksi];
    t->id            = next_id();
    t->pelanggan_id  = (int)pelanggan_id;
    t->barang_id     = (int)barang_id;
    t->jumlah        = (int)jumlah;
    t->subtotal      = subtotal;
    copy_string(t->jenis, sizeof(t->jenis), jenis);
    tanggal_sekarang(t->tanggal, sizeof(t->tanggal));

    /* Update stok otomatis jika "keluar" (penjualan) */
    if (strcmp(jenis, "keluar") == 0) {
        b->stok -= (int)jumlah;
        if (b->stok < 0) b->stok = 0;

        /* Update data pelanggan */
        for (int i = 0; i < g_jumlah_pelanggan; i++) {
            if (g_pelanggan[i].id == (int)pelanggan_id) {
                g_pelanggan[i].total_belanja    += subtotal;
                g_pelanggan[i].jumlah_transaksi += 1;
                break;
            }
        }
    } else {
        /* Stok masuk */
        b->stok += (int)jumlah;
    }

    g_jumlah_transaksi++;

    (*env)->ReleaseStringUTFChars(env, j_jenis, jenis);
    LOGI("Transaksi id=%d dicatat: %s x%d = Rp%.0f", t->id, b->nama, (int)jumlah, subtotal);
    return (jint)t->id;
}

JNIEXPORT jstring JNICALL
Java_com_umkmudah_NativeLib_getLaporanKeuangan(JNIEnv *env, jobject obj) {
    double total_pendapatan = 0.0;
    double total_pengeluaran = 0.0;
    int    total_transaksi_keluar = 0;
    int    total_transaksi_masuk  = 0;

    for (int i = 0; i < g_jumlah_transaksi; i++) {
        Transaksi *t = &g_transaksi[i];
        if (strcmp(t->jenis, "keluar") == 0) {
            total_pendapatan += t->subtotal;
            total_transaksi_keluar++;
        } else {
            /* Hitung pengeluaran dari harga beli */
            for (int j = 0; j < g_jumlah_barang; j++) {
                if (g_barang[j].id == t->barang_id) {
                    total_pengeluaran += g_barang[j].harga_beli * t->jumlah;
                    break;
                }
            }
            total_transaksi_masuk++;
        }
    }

    double laba_bersih = total_pendapatan - total_pengeluaran;

    /* Hitung nilai total stok */
    double nilai_stok = 0.0;
    for (int i = 0; i < g_jumlah_barang; i++) {
        nilai_stok += g_barang[i].harga_beli * g_barang[i].stok;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf),
        "{\"total_pendapatan\":%.0f,"
        "\"total_pengeluaran\":%.0f,"
        "\"laba_bersih\":%.0f,"
        "\"total_transaksi_penjualan\":%d,"
        "\"total_transaksi_pembelian\":%d,"
        "\"nilai_total_stok\":%.0f,"
        "\"jumlah_pelanggan\":%d,"
        "\"jumlah_produk\":%d}",
        total_pendapatan, total_pengeluaran, laba_bersih,
        total_transaksi_keluar, total_transaksi_masuk,
        nilai_stok, g_jumlah_pelanggan, g_jumlah_barang);

    return (*env)->NewStringUTF(env, buf);
}

JNIEXPORT jstring JNICALL
Java_com_umkmudah_NativeLib_getRiwayatTransaksi(JNIEnv *env, jobject obj) {
    char *buf = (char *)malloc(MAX_TRANSAKSI * 256);
    if (!buf) return (*env)->NewStringUTF(env, "[]");

    int pos = 0;
    pos += sprintf(buf + pos, "[");
    /* Tampilkan terbaru dulu (reverse) */
    for (int i = g_jumlah_transaksi - 1; i >= 0; i--) {
        Transaksi *t = &g_transaksi[i];
        char nama_barang[MAX_STR] = "?";
        char nama_pelanggan[MAX_STR] = "Umum";
        for (int j = 0; j < g_jumlah_barang; j++) {
            if (g_barang[j].id == t->barang_id) {
                copy_string(nama_barang, sizeof(nama_barang), g_barang[j].nama);
                break;
            }
        }
        for (int j = 0; j < g_jumlah_pelanggan; j++) {
            if (g_pelanggan[j].id == t->pelanggan_id) {
                copy_string(nama_pelanggan, sizeof(nama_pelanggan), g_pelanggan[j].nama);
                break;
            }
        }
        if (i < g_jumlah_transaksi - 1) pos += sprintf(buf + pos, ",");
        pos += sprintf(buf + pos,
            "{\"id\":%d,\"barang\":\"%s\",\"pelanggan\":\"%s\","
            "\"jumlah\":%d,\"subtotal\":%.0f,"
            "\"jenis\":\"%s\",\"tanggal\":\"%s\"}",
            t->id, nama_barang, nama_pelanggan,
            t->jumlah, t->subtotal, t->jenis, t->tanggal);
    }
    pos += sprintf(buf + pos, "]");

    jstring result = (*env)->NewStringUTF(env, buf);
    free(buf);
    return result;
}

/* ============================================================
 * INISIALISASI & INFO VERSI
 * ============================================================ */

JNIEXPORT jstring JNICALL
Java_com_umkmudah_NativeLib_getVersi(JNIEnv *env, jobject obj) {
    return (*env)->NewStringUTF(env, "UMKMudah v1.0 | C NDK Build | Visual Studio");
}

JNIEXPORT void JNICALL
Java_com_umkmudah_NativeLib_resetData(JNIEnv *env, jobject obj) {
    g_jumlah_pelanggan  = 0;
    g_jumlah_barang     = 0;
    g_jumlah_transaksi  = 0;
    g_next_id           = 1;
    LOGI("Data direset.");
}

/* Seed data demo */
JNIEXPORT void JNICALL
Java_com_umkmudah_NativeLib_seedDemoData(JNIEnv *env, jobject obj) {
    if (g_jumlah_pelanggan > 0 || g_jumlah_barang > 0 || g_jumlah_transaksi > 0) {
        return;
    }

    /* Pelanggan demo */
    jstring nama1 = (*env)->NewStringUTF(env, "Budi Santoso");
    jstring tlp1  = (*env)->NewStringUTF(env, "08123456789");
    jstring alm1  = (*env)->NewStringUTF(env, "Jl. Merdeka No.1, Jakarta");
    Java_com_umkmudah_NativeLib_tambahPelanggan(env, obj, nama1, tlp1, alm1);

    jstring nama2 = (*env)->NewStringUTF(env, "Siti Rahayu");
    jstring tlp2  = (*env)->NewStringUTF(env, "08234567890");
    jstring alm2  = (*env)->NewStringUTF(env, "Jl. Pahlawan No.5, Bandung");
    Java_com_umkmudah_NativeLib_tambahPelanggan(env, obj, nama2, tlp2, alm2);

    jstring nama3 = (*env)->NewStringUTF(env, "Ahmad Fauzi");
    jstring tlp3  = (*env)->NewStringUTF(env, "08345678901");
    jstring alm3  = (*env)->NewStringUTF(env, "Jl. Sudirman No.12, Surabaya");
    Java_com_umkmudah_NativeLib_tambahPelanggan(env, obj, nama3, tlp3, alm3);

    /* Barang demo */
    jstring nb1 = (*env)->NewStringUTF(env, "Kopi Arabika 250g");
    jstring kb1 = (*env)->NewStringUTF(env, "Minuman");
    jstring sb1 = (*env)->NewStringUTF(env, "pcs");
    Java_com_umkmudah_NativeLib_tambahBarang(env, obj, nb1, kb1, 35000, 55000, 50, 10, sb1);

    jstring nb2 = (*env)->NewStringUTF(env, "Teh Hijau Premium");
    jstring kb2 = (*env)->NewStringUTF(env, "Minuman");
    jstring sb2 = (*env)->NewStringUTF(env, "pcs");
    Java_com_umkmudah_NativeLib_tambahBarang(env, obj, nb2, kb2, 20000, 35000, 8, 10, sb2);

    jstring nb3 = (*env)->NewStringUTF(env, "Snack Keripik Singkong");
    jstring kb3 = (*env)->NewStringUTF(env, "Makanan");
    jstring sb3 = (*env)->NewStringUTF(env, "bungkus");
    Java_com_umkmudah_NativeLib_tambahBarang(env, obj, nb3, kb3, 8000, 15000, 100, 20, sb3);

    jstring nb4 = (*env)->NewStringUTF(env, "Sabun Cuci Tangan");
    jstring kb4 = (*env)->NewStringUTF(env, "Kebersihan");
    jstring sb4 = (*env)->NewStringUTF(env, "botol");
    Java_com_umkmudah_NativeLib_tambahBarang(env, obj, nb4, kb4, 12000, 22000, 5, 10, sb4);

    LOGI("Demo data seeded.");
}
