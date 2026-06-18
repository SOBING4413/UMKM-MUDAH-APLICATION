package com.umkmudah;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import org.json.JSONObject;

public class DashboardFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_dashboard, container, false);

        NativeLib lib = ((MainActivity) requireActivity()).getNativeLib();

        try {
            /* Ambil laporan keuangan dari C */
            String json = lib.getLaporanKeuangan();
            JSONObject obj = new JSONObject(json);

            long pendapatan  = obj.getLong("total_pendapatan");
            long pengeluaran = obj.getLong("total_pengeluaran");
            long laba        = obj.getLong("laba_bersih");
            int  pelanggan   = obj.getInt("jumlah_pelanggan");
            int  produk      = obj.getInt("jumlah_produk");
            int  stokRendah  = lib.getBarangStokRendah();

            /* Update UI */
            setText(v, R.id.txt_pendapatan,  "Rp " + formatRupiah(pendapatan));
            setText(v, R.id.txt_pengeluaran, "Rp " + formatRupiah(pengeluaran));
            setText(v, R.id.txt_laba,        "Rp " + formatRupiah(laba));
            setText(v, R.id.txt_pelanggan,   String.valueOf(pelanggan));
            setText(v, R.id.txt_produk,      String.valueOf(produk));
            setText(v, R.id.txt_stok_rendah, String.valueOf(stokRendah));

        } catch (Exception e) {
            e.printStackTrace();
        }

        return v;
    }

    private void setText(View root, int id, String text) {
        TextView tv = root.findViewById(id);
        if (tv != null) tv.setText(text);
    }

    private String formatRupiah(long amount) {
        /* Format: 1.500.000 */
        String s = String.valueOf(amount);
        StringBuilder sb = new StringBuilder();
        int mod = s.length() % 3;
        if (mod > 0) {
            sb.append(s, 0, mod);
            if (s.length() > mod) sb.append(".");
        }
        for (int i = mod; i < s.length(); i += 3) {
            sb.append(s, i, Math.min(i + 3, s.length()));
            if (i + 3 < s.length()) sb.append(".");
        }
        return sb.toString();
    }
}
