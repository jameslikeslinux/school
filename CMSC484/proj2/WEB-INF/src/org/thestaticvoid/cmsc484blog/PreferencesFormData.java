package org.thestaticvoid.cmsc484blog;

/**
 * Contains how the preferences form should initially be set
 *
 * @author	James Lee
 * @version	20070501
 */
public class PreferencesFormData {
	private boolean invert;

	public PreferencesFormData(boolean invert) {
		this.invert = invert;
	}

	public boolean getInvert() {
		return invert;
	}
}
