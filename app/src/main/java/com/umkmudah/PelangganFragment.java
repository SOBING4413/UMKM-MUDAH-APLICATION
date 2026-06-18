package com.umkmudah;

import android.app.AlertDialog;
import android.os.Bundle;
import android.view.*;
import android.widget.*;
import androidx.annotation.*;
import androidx.fragment.app.Fragment;
import org.json.*;

public class PelangganFragment extends Fragment {

    private NativeLib lib;
    private LinearLayout listContainer;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_pelanggan, container, false);
        lib = ((MainActivity) requireActivity()).getNativeLib();
        listContainer = v.findViewById(R.id.list_pelanggan);

        v.findViewById(R.id.btn_tambah_pelanggan).setOnClickListener(btn -> showTambahDialog());
        loadPelanggan();
        return v;
    }

    private void loadPelanggan() {
        listContainer.removeAllViews();
        try {
            JSONArray arr = new JSONArray(lib.getDaftarPelanggan());
            for (int i = 0; i < arr.length(); i++) {
                JSONObject p = arr.getJSONObject(i);
                addPelangganCard(p);
            }
        } catch (JSONException e) { e.printStackTrace(); }
    }

    private void addPelangganCard(JSONObject p) throws JSONException {
        View card = LayoutInflater.from(getContext())
                .inflate(R.layout.item_pelanggan, listContainer, false);

        ((TextView) card.findViewById(R.id.tv_nama_pelanggan))
                .setText(p.getString("nama"));
        ((TextView) card.findViewById(R.id.tv_telepon))
                .setText(p.getString("telepon"));
        ((TextView) card.findViewById(R.id.tv_total_belanja))
                .setText("Total: Rp " + p.getString("total_belanja"));
        ((TextView) card.findViewById(R.id.tv_jml_transaksi))
                .setText(p.getInt("jumlah_transaksi") + " transaksi");

        int id = p.getInt("id");
        card.findViewById(R.id.btn_hapus_pelanggan).setOnClickListener(btn -> {
            new AlertDialog.Builder(getContext())
                .setTitle("Hapus Pelanggan?")
                .setMessage("Data pelanggan akan dihapus permanen.")
                .setPositiveButton("Hapus", (d, w) -> {
                    lib.hapusPelanggan(id);
                    loadPelanggan();
                })
                .setNegativeButton("Batal", null)
                .show();
        });

        listContainer.addView(card);
    }

    private void showTambahDialog() {
        View dlgView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_tambah_pelanggan, null);
        EditText etNama    = dlgView.findViewById(R.id.et_nama);
        EditText etTelepon = dlgView.findViewById(R.id.et_telepon);
        EditText etAlamat  = dlgView.findViewById(R.id.et_alamat);

        new AlertDialog.Builder(getContext())
            .setTitle("Tambah Pelanggan")
            .setView(dlgView)
            .setPositiveButton("Simpan", (d, w) -> {
                String nama    = etNama.getText().toString().trim();
                String telepon = etTelepon.getText().toString().trim();
                String alamat  = etAlamat.getText().toString().trim();
                if (!nama.isEmpty()) {
                    lib.tambahPelanggan(nama, telepon, alamat);
                    loadPelanggan();
                }
            })
            .setNegativeButton("Batal", null)
            .show();
    }
}
