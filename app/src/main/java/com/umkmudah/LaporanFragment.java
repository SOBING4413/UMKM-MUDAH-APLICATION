package com.umkmudah;

import android.os.Bundle;
import android.view.*;
import android.widget.*;
import androidx.annotation.*;
import androidx.fragment.app.Fragment;
import org.json.*;

public class LaporanFragment extends Fragment {

    private NativeLib lib;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_laporan, container, false);
        lib = ((MainActivity) requireActivity()).getNativeLib();
        loadLaporan(v);
        return v;
    }

    private void loadLaporan(View v) {
        try {
            /* Ringkasan keuangan */
            JSONObject obj = new JSONObject(lib.getLaporanKeuangan());
            setText(v, R.id.tv_total_pendapatan, "Rp " + fmt(obj.getLong("total_pendapatan")));
            setText(v, R.id.tv_total_pengeluaran,"Rp " + fmt(obj.getLong("total_pengeluaran")));
            setText(v, R.id.tv_laba_bersih,      "Rp " + fmt(obj.getLong("laba_bersih")));
            setText(v, R.id.tv_nilai_stok,        "Rp " + fmt(obj.getLong("nilai_total_stok")));
            setText(v, R.id.tv_jml_penjualan,
                    obj.getInt("total_transaksi_penjualan") + " transaksi");
            setText(v, R.id.tv_jml_pembelian,
                    obj.getInt("total_transaksi_pembelian") + " transaksi");

            /* Riwayat transaksi */
            LinearLayout listTx = v.findViewById(R.id.list_transaksi);
            listTx.removeAllViews();
            JSONArray arr = new JSONArray(lib.getRiwayatTransaksi());
            for (int i = 0; i < arr.length(); i++) {
                JSONObject t = arr.getJSONObject(i);
                addTransaksiRow(listTx, t);
            }

        } catch (JSONException e) { e.printStackTrace(); }
    }

    private void addTransaksiRow(LinearLayout parent, JSONObject t) throws JSONException {
        View row = LayoutInflater.from(getContext())
                .inflate(R.layout.item_transaksi, parent, false);

        boolean isKeluar = "keluar".equals(t.getString("jenis"));

        ((TextView) row.findViewById(R.id.tv_tx_barang))
                .setText(t.getString("barang") + " x" + t.getInt("jumlah"));
        ((TextView) row.findViewById(R.id.tv_tx_pelanggan))
                .setText(t.getString("pelanggan") + " · " + t.getString("tanggal"));

        TextView tvAmount = row.findViewById(R.id.tv_tx_amount);
        tvAmount.setText((isKeluar ? "+" : "-") + " Rp " + fmt(t.getLong("subtotal")));
        tvAmount.setTextColor(isKeluar ? 0xFF43A047 : 0xFFE53935);

        parent.addView(row);
    }

    private void setText(View root, int id, String text) {
        TextView tv = root.findViewById(id);
        if (tv != null) tv.setText(text);
    }

    private String fmt(long amount) {
        String s = String.valueOf(amount);
        StringBuilder sb = new StringBuilder();
        int mod = s.length() % 3;
        if (mod > 0) { sb.append(s, 0, mod); if (s.length() > mod) sb.append("."); }
        for (int i = mod; i < s.length(); i += 3) {
            sb.append(s, i, Math.min(i + 3, s.length()));
            if (i + 3 < s.length()) sb.append(".");
        }
        return sb.toString();
    }
}
