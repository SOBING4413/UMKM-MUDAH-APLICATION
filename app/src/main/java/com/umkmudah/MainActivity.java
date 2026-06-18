package com.umkmudah;

import android.os.Bundle;
import android.view.MenuItem;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;

public class MainActivity extends AppCompatActivity {

    private NativeLib nativeLib;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /* Inisialisasi native library */
        nativeLib = new NativeLib();
        nativeLib.seedDemoData();   /* load data demo */

        /* Bottom navigation */
        BottomNavigationView nav = findViewById(R.id.bottom_nav);
        nav.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                Fragment fragment = null;
                int id = item.getItemId();

                if (id == R.id.nav_dashboard) {
                    fragment = new DashboardFragment();
                } else if (id == R.id.nav_pelanggan) {
                    fragment = new PelangganFragment();
                } else if (id == R.id.nav_stok) {
                    fragment = new StokFragment();
                } else if (id == R.id.nav_laporan) {
                    fragment = new LaporanFragment();
                }

                if (fragment != null) {
                    getSupportFragmentManager()
                        .beginTransaction()
                        .replace(R.id.fragment_container, fragment)
                        .commit();
                    return true;
                }
                return false;
            }
        });

        /* Default ke dashboard */
        nav.setSelectedItemId(R.id.nav_dashboard);
    }

    public NativeLib getNativeLib() {
        return nativeLib;
    }
}
