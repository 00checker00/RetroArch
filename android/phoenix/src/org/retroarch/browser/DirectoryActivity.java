package org.retroarch.browser;

import org.retroarch.R;

import java.util.*;
import java.io.*;

import android.content.*;
import android.app.*;
import android.os.*;
import android.preference.PreferenceManager;
import android.widget.*;
import android.view.*;
import android.graphics.drawable.*;

class FileWrapper implements IconAdapterItem {
	public final File file;
	public final boolean parentItem;
	protected final int typeIndex;
	protected final boolean enabled;

	public FileWrapper(File aFile, boolean aIsParentItem, boolean aIsEnabled) {
		file = aFile;
		typeIndex = aIsParentItem ? 0 : (file.isDirectory() ? 1 : 0)
				+ (file.isFile() ? 2 : 0);
		parentItem = aIsParentItem;
		enabled = aIsParentItem || aIsEnabled;
	}

	@Override
	public boolean isEnabled() {
		return enabled;
	}

	@Override
	public String getText() {
		return parentItem ? "[Parent Directory]" : file.getName();
	}

	@Override
	public int getIconResourceId() {
		if (!parentItem) {
			return file.isFile() ? R.drawable.ic_file : R.drawable.ic_dir;
		} else {
			return R.drawable.ic_dir;
		}
	}

	@Override
	public Drawable getIconDrawable() {
		return null;
	}

	public int compareTo(FileWrapper aOther) {
		if (null != aOther) {
			// Who says ternary is hard to follow
			if (isEnabled() == aOther.isEnabled()) {
				return (typeIndex == aOther.typeIndex) ? file
						.compareTo(aOther.file)
						: ((typeIndex < aOther.typeIndex) ? -1 : 1);
			} else {
				return isEnabled() ? -1 : 1;
			}
		}

		return -1;
	}
}

public class DirectoryActivity extends Activity implements
		AdapterView.OnItemClickListener {
	private IconAdapter<FileWrapper> adapter;
	private File listedDirectory;

	public static class BackStackItem implements Parcelable {
		public String path;
		public boolean parentIsBack;

		public BackStackItem(String aPath, boolean aParentIsBack) {
			path = aPath;
			parentIsBack = aParentIsBack;
		}

		private BackStackItem(Parcel aIn) {
			path = aIn.readString();
			parentIsBack = aIn.readInt() != 0;
		}

		public int describeContents() {
			return 0;
		}

		public void writeToParcel(Parcel out, int flags) {
			out.writeString(path);
			out.writeInt(parentIsBack ? 1 : 0);
		}

		public static final Parcelable.Creator<BackStackItem> CREATOR = new Parcelable.Creator<BackStackItem>() {
			public BackStackItem createFromParcel(Parcel in) {
				return new BackStackItem(in);
			}

			public BackStackItem[] newArray(int size) {
				return new BackStackItem[size];
			}
		};

	}

	private ArrayList<BackStackItem> backStack;
	
	protected String startDirectory;
	protected String pathSettingKey;
	
	protected void setStartDirectory(String path) {
		startDirectory = path;
	}
	
	protected void setPathSettingKey(String key) {
		pathSettingKey = key;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.line_list);

		// Setup the list
		adapter = new IconAdapter<FileWrapper>(this, R.layout.line_list_item);
		ListView list = (ListView) findViewById(R.id.list);
		list.setAdapter(adapter);
		list.setOnItemClickListener(this);

		// Load Directory
		if (savedInstanceState != null) {
			backStack = savedInstanceState.getParcelableArrayList("BACKSTACK");
		}

		if (backStack == null || backStack.size() == 0) {
			backStack = new ArrayList<BackStackItem>();
			String startPath = (startDirectory == null || startDirectory.isEmpty()) ? Environment
					.getExternalStorageDirectory().getPath() : startDirectory;
			backStack.add(new BackStackItem(startPath, false));
		}

		wrapFiles();
	}

	@Override
	protected void onSaveInstanceState(Bundle aState) {
		super.onSaveInstanceState(aState);
		aState.putParcelableArrayList("BACKSTACK", backStack);
	}

	@Override
	public void onItemClick(AdapterView<?> aListView, View aView,
			int aPosition, long aID) {
		final FileWrapper item = adapter.getItem(aPosition);

		if (item.parentItem && backStack.get(backStack.size() - 1).parentIsBack) {
			backStack.remove(backStack.size() - 1);
			wrapFiles();
			return;
		}

		final File selected = item.parentItem ? listedDirectory.getParentFile()
				: item.file;

		if (selected.isDirectory()) {
			backStack.add(new BackStackItem(selected.getAbsolutePath(),
					!item.parentItem));
			wrapFiles();
		} else {
			Intent intent = new Intent();
			String filePath = selected.getAbsolutePath();
			
			if (pathSettingKey != null && !pathSettingKey.isEmpty()) {
				SharedPreferences settings = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
				SharedPreferences.Editor editor = settings.edit();
				editor.putString(pathSettingKey, filePath);
				editor.commit();
			}
			
			intent.putExtra("PATH", filePath);
			setResult(RESULT_OK, intent);
			finish();
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (backStack.size() > 1) {
				backStack.remove(backStack.size() - 1);
				wrapFiles();
			} else {
				Intent intent = new Intent();
				setResult(RESULT_CANCELED, intent);
				finish();
			}
			return true;
		}

		return super.onKeyDown(keyCode, event);
	}
	
	private ArrayList<String> allowedExt;
	private ArrayList<String> disallowedExt;
	
	private boolean filterPath(String path) {
		if (disallowedExt != null) {
			for (String ext : disallowedExt)
				if (path.endsWith(ext))
					return false;
		}
		
		if (allowedExt != null) {
			for (String ext : allowedExt)
				if (path.endsWith(ext))
					return true;
			
			return false;
		}

		return true;
	}
	
	protected void addAllowedExt(String ext) {
		if (allowedExt == null)
			allowedExt = new ArrayList<String>();
		
		allowedExt.add(ext);
	}
	
	protected void addDisallowedExt(String ext) {
		if (disallowedExt == null)
			disallowedExt = new ArrayList<String>();
		
		disallowedExt.add(ext);
	}

	private void wrapFiles() {
		listedDirectory = new File(backStack.get(backStack.size() - 1).path);

		if (!listedDirectory.isDirectory()) {
			throw new IllegalArgumentException("Directory is not valid.");
		}

		adapter.clear();
		setTitle(listedDirectory.getAbsolutePath());

		if (listedDirectory.getParentFile() != null) {
			adapter.add(new FileWrapper(null, true, true));
		}

		// Copy new items
		final File[] files = listedDirectory.listFiles();
		if (files != null) {
			for (File file : files) {
				String path = file.getName();

				boolean allowFile = filterPath(path);

				// Don't list save files in ROM list.
				if (allowFile)
					adapter.add(new FileWrapper(file, false,
							file.isDirectory() || true));
			}
		}

		// Sort items
		adapter.sort(new Comparator<FileWrapper>() {
			@Override
			public int compare(FileWrapper aLeft, FileWrapper aRight) {
				return aLeft.compareTo(aRight);
			};
		});

		// Update
		adapter.notifyDataSetChanged();
	}
}
