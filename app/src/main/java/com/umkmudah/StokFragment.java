package com.umkmudah;

import android.app.AlertDialog;
import android.os.Bundle;
import android.view.*;
import android.widget.*;
import androidx.annotation.*;
import androidx.fragment.app.Fragment;
import org.json.*;

public class StokFragment extends Fragment {

    private NativeLib lib;
    private LinearLayout listContainer;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_stok, container, false);
        lib = ((MainActivity) requireActivity()).getNativeLib();
        listContainer = v.findViewById(R.id.list_stok);

        v.findViewById(R.id.btn_tambah_barang).setOnClickListener(btn -> showTambahDialog());
        loadBarang();
        return v;
    }

    private void loadBarang() {
        listContainer.removeAllViews();
        try {
            JSONArray arr = new JSONArray(lib.getDaftarBarang());
            for (int i = 0; i < arr.length(); i++) {
                JSONObject b = arr.getJSONObject(i);
                addBarangCard(b);
            }
        } catch (JSONException e) { e.printStackTrace(); }
    }

    private void addBarangCard(JSONObject b) throws JSONException {
        View card = LayoutInflater.from(getContext())
                .inflate(R.layout.item_barang, listContainer, false);

        boolean lowStock = b.getInt("low_stock") == 1;

        ((TextView) card.findViewById(R.id.tv_nama_barang))
                .setText(b.getString("nama"));
        ((TextView) card.findViewById(R.id.tv_kategori))
                .setText(b.getString("kategori"));
        ((TextView) card.findViewById(R.id.tv_harga_jual))
                .setText("Jual: Rp " + b.getString("harga_jual"));

        TextView tvStok = card.findViewById(R.id.tv_stok);
        tvStok.setText("Stok: " + b.getInt("stok") + " " + b.getString("satuan"));
        if (lowStock) {
            tvStok.setTextColor(0xFFE53935); /* merah jika stok rendah */
        }

        int id = b.getInt("id");
        card.findViewById(R.id.btn_tambah_stok).setOnClickListener(btn -> {
            lib.updateStok(id, 10);
            loadBarang();
        });
        card.findViewById(R.id.btn_kurang_stok).setOnClickListener(btn -> {
            lib.updateStok(id, -1);
            loadBarang();
        });

        listContainer.addView(card);
    }

    private void showTambahDialog() {
        View dlgView = LayoutInflater.from(getContext())
                .inflate(R.layout.dialog_tambah_barang, null);

        EditText etNama      = dlgView.findViewById(R.id.et_nama_barang);
        EditText etKategori  = dlgView.findViewById(R.id.et_kategori);
        EditText etHargaBeli = dlgView.findViewById(R.id.et_harga_beli);
        EditText etHargaJual = dlgView.findViewById(R.id.et_harga_jual);
        EditText etStok      = dlgView.findViewById(R.id.et_stok);
        EditText etStokMin   = dlgView.findViewById(R.id.et_stok_min);
        EditText etSatuan    = dlgView.findViewById(R.id.et_satuan);

        new AlertDialog.Builder(getContext())
            .setTitle("Tambah Barang")
            .setView(dlgView)
            .setPositiveButton("Simpan", (d, w) -> {
                try {
                    lib.tambahBarang(
                        etNama.getText().toString(),
                        etKategori.getText().toString(),
                        Double.parseDouble(etHargaBeli.getText().toString()),
                        Double.parseDouble(etHargaJual.getText().toString()),
                        Integer.parseInt(etStok.getText().toString()),
                        Integer.parseInt(etStokMin.getText().toString()),
                        etSatuan.getText().toString()
                    );
                    loadBarang();
                } catch (NumberFormatException e) {
                    Toast.makeText(getContext(), "Isi semua field dengan benar!", Toast.LENGTH_SHORT).show();
                }
            })
            .setNegativeButton("Batal", null)
            .show();
    }
}
