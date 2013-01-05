package org.retroarch.browser;

import org.retroarch.R;

import java.io.*;

import android.content.*;
import android.content.res.AssetManager;
import android.app.*;
import android.os.*;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.widget.*;
import android.util.Log;
import android.view.*;
import android.view.inputmethod.*;
import android.graphics.drawable.*;

class ModuleWrapper implements IconAdapterItem {
	public final File file;

	public ModuleWrapper(Context aContext, File aFile) throws IOException {
		file = aFile;
	}

	@Override
	public boolean isEnabled() {
		return true;
	}

	@Override
	public String getText() {
		return file.getName();
	}

	@Override
	public int getIconResourceId() {
		return 0;
	}

	@Override
	public Drawable getIconDrawable() {
		return null;
	}
}

public class RetroArch extends Activity implements
		AdapterView.OnItemClickListener, PopupMenu.OnMenuItemClickListener {
	private IconAdapter<ModuleWrapper> adapter;
	static private final int ACTIVITY_LOAD_ROM = 0;
	static private String libretro_path;
	static private final String TAG = "RetroArch-Phoenix";
	private ConfigFile config;

	public float getRefreshRate() {
		final WindowManager wm = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
		final Display display = wm.getDefaultDisplay();
		float rate = display.getRefreshRate();
		return rate;
	}
	
	private byte[] loadAsset(String asset) throws IOException {
		String path = asset;
		InputStream stream = getAssets().open(path);
		int len = stream.available();
		byte[] buf = new byte[len];
		stream.read(buf, 0, len);
		return buf;
	}
	
	private void extractAssets(AssetManager manager, String cacheDir, String relativePath, int level) throws IOException {
		final String[] paths = manager.list(relativePath);
		if (paths != null && paths.length > 0) { // Directory
			Log.d(TAG, "Extracting assets directory: " + relativePath);
			for (final String path : paths)
				extractAssets(manager, cacheDir, relativePath + (level > 0 ? File.separator : "") + path, level + 1);	
		} else { // File, extract.
			Log.d(TAG, "Extracting assets file: " + relativePath);
			
			String parentPath = new File(relativePath).getParent();
			if (parentPath != null) {
				File parentFile = new File(cacheDir, parentPath);
				Log.d(TAG, "Creating folder: " + parentFile.getAbsolutePath());
				parentFile.mkdirs(); // Doesn't throw.
			}
			
			byte[] asset = loadAsset(relativePath);
			BufferedOutputStream writer = new BufferedOutputStream(
					new FileOutputStream(new File(cacheDir, relativePath)));

			writer.write(asset, 0, asset.length);
			writer.flush();
			writer.close();
		}
	}
	
	private void extractAssets() {
		try {
			AssetManager assets = getAssets();
			String cacheDir = getCacheDir().getAbsolutePath();
			//extractAssets(assets, cacheDir, "", 0);
			//extractAssets(assets, cacheDir, "Shaders", 1);
			extractAssets(assets, cacheDir, "Overlays", 1);
		} catch (IOException e) {
			Log.e(TAG, "Failed to extract assets to cache.");			
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		try {
			config = new ConfigFile(new File(getDefaultConfigPath()));
		} catch (IOException e) {
			config = new ConfigFile();
		}
		
		// Extracting assets appears to take considerable amount of time, so
		// move extraction to a thread.
		Thread assetThread = new Thread(new Runnable() {
			public void run() {
				extractAssets();
			}
		});
		assetThread.start();
		
		setContentView(R.layout.line_list);

		// Setup the list
		adapter = new IconAdapter<ModuleWrapper>(this, R.layout.line_list_item);
		ListView list = (ListView) findViewById(R.id.list);
		list.setAdapter(adapter);
		list.setOnItemClickListener(this);

		setTitle("Select Libretro core");

		// Populate the list
		final String modulePath = getApplicationInfo().nativeLibraryDir;
		for (final File lib : new File(modulePath).listFiles()) {
			String libName = lib.getName();
			
			// Allow both libretro-core.so and libretro_core.so.
			if (libName.startsWith("libretro") && !libName.startsWith("libretroarch")) {
				try {
					adapter.add(new ModuleWrapper(this, lib));
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	@Override
	public void onItemClick(AdapterView<?> aListView, View aView,
			int aPosition, long aID) {
		final ModuleWrapper item = adapter.getItem(aPosition);
		libretro_path = item.file.getAbsolutePath();

		Intent myIntent;
		myIntent = new Intent(this, ROMActivity.class);
		startActivityForResult(myIntent, ACTIVITY_LOAD_ROM);
	}
	
	private String getDefaultConfigPath() {
		String internal = System.getenv("INTERNAL_STORAGE");
		String external = System.getenv("EXTERNAL_STORAGE");
		
		if (external != null) {
			String confPath = external + File.separator + "retroarch.cfg";
			if (new File(confPath).exists())
				return confPath;
		} else if (internal != null) {
			String confPath = internal + File.separator + "retroarch.cfg";
			if (new File(confPath).exists())
				return confPath;
		} else {
			String confPath = "/mnt/extsd/retroarch.cfg";
			if (new File(confPath).exists())
				return confPath;
		}
		
		if (internal != null && new File(internal + File.separator + "retroarch.cfg").canWrite())
			return internal + File.separator + "retroarch.cfg";
		else if (external != null && new File(internal + File.separator + "retroarch.cfg").canWrite())
			return external + File.separator + "retroarch.cfg";
		else
			return getCacheDir().getAbsolutePath() + File.separator + "retroarch.cfg";
	}
	
	private void updateConfigFile() {
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
		config.setBoolean("audio_rate_control", prefs.getBoolean("audio_rate_control", true));
		config.setBoolean("audio_enable", prefs.getBoolean("audio_enable", true));
		config.setBoolean("video_smooth", prefs.getBoolean("video_smooth", true));
		config.setBoolean("video_allow_rotate", prefs.getBoolean("video_allow_rotate", true));
		config.setBoolean("savestate_auto_save", prefs.getBoolean("savestate_auto_save", false));
		config.setBoolean("rewind_enable", prefs.getBoolean("rewind_enable", false));
		config.setBoolean("video_vsync", prefs.getBoolean("video_vsync", true));
		config.setBoolean("input_autodetect_enable", prefs.getBoolean("input_autodetect_enable", true));
		
		String aspect = prefs.getString("video_aspect_ratio", "auto");
		if (aspect.equals("full")) {
			config.setBoolean("video_force_aspect", false);
		} else if (aspect.equals("auto")) {
			config.setBoolean("video_force_aspect", true);
			config.setBoolean("video_force_aspect_auto", true);
			config.setDouble("video_aspect_ratio", -1.0);
		} else if (aspect.equals("square")) {
			config.setBoolean("video_force_aspect", true);
			config.setBoolean("video_force_aspect_auto", false);
			config.setDouble("video_aspect_ratio", -1.0);
		} else {
			double aspect_ratio = Double.parseDouble(aspect);
			config.setBoolean("video_force_aspect", true);
			config.setDouble("video_aspect_ratio", aspect_ratio);
		}
		
		String shaderPath = prefs.getString("video_bsnes_shader", "");
		if (prefs.getBoolean("video_shader_enable", false) && new File(shaderPath).exists()) {
			config.setString("video_shader_type", "bsnes");
			config.setString("video_bsnes_shader", shaderPath);
		} else {
			config.setString("video_shader_type", "none");
			config.setString("video_bsnes_shader", "");
		}
		
		config.setBoolean("video_render_to_texture", prefs.getBoolean("video_render_to_texture", false));
		config.setString("video_second_pass_shader",
				prefs.getBoolean("video_second_pass_shader_enable", false) ?
						prefs.getString("video_second_pass_shader", "") : "");
		
		config.setBoolean("video_second_pass_smooth", prefs.getBoolean("video_second_pass_smooth", true));
		
		config.setString("video_fbo_scale_x", prefs.getString("video_fbo_scale", "2.0"));
		config.setString("video_fbo_scale_y", prefs.getString("video_fbo_scale", "2.0"));
		
		boolean useOverlay = prefs.getBoolean("input_overlay_enable", true);
		if (useOverlay) {
			String overlayPath = prefs.getString("input_overlay", getCacheDir() + "/Overlays/basic_overlay.cfg");
			config.setString("input_overlay", overlayPath);
		} else {
			config.setString("input_overlay", "");
		}

		String confPath = getDefaultConfigPath();
		try {
			config.write(new File(confPath));
		} catch (IOException e) {
			Log.e(TAG, "Failed to save config file to: " + confPath);
		}
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		Intent myIntent;
		String current_ime = Settings.Secure.getString(getContentResolver(), Settings.Secure.DEFAULT_INPUT_METHOD);
		
		updateConfigFile();
		
		switch (requestCode) {
		case ACTIVITY_LOAD_ROM:
			if (data.getStringExtra("PATH") != null) {
				Toast.makeText(this,
						"Loading: [" + data.getStringExtra("PATH") + "]...",
						Toast.LENGTH_SHORT).show();
				myIntent = new Intent(this, NativeActivity.class);
				myIntent.putExtra("ROM", data.getStringExtra("PATH"));
				myIntent.putExtra("LIBRETRO", libretro_path);
				myIntent.putExtra("REFRESHRATE",
						Float.toString(getRefreshRate()));
				myIntent.putExtra("CONFIGFILE", getDefaultConfigPath());
				myIntent.putExtra("IME", current_ime);
				startActivity(myIntent);
			}
			break;
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu aMenu) {
		super.onCreateOptionsMenu(aMenu);
		getMenuInflater().inflate(R.menu.directory_list, aMenu);
		return true;
	}
	
	public void showPopup(View v) {
		PopupMenu menu = new PopupMenu(this, v);
		MenuInflater inflater = menu.getMenuInflater();
		inflater.inflate(R.menu.context_menu, menu.getMenu());
		menu.setOnMenuItemClickListener(this);
		menu.show();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem aItem) {
		switch (aItem.getItemId()) {
		case R.id.settings:
			showPopup(findViewById(R.id.settings));
			Log.i(TAG, "Got settings ...");
			return true;

		default:
			return super.onOptionsItemSelected(aItem);
		}
	}

	@Override
	public boolean onMenuItemClick(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.input_method_select:
			InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
			imm.showInputMethodPicker();
			return true;
			
		case R.id.rarch_settings:
			Log.i(TAG, "Rarch settings clicked!");			
			Intent rset = new Intent(this, SettingsActivity.class);
			startActivity(rset);
			return true;

		default:
			return false;
		}
	}
}
