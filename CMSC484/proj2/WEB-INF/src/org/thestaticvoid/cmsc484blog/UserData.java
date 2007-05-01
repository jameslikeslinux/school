package org.thestaticvoid.cmsc484blog;

/**
 * Information about the logged-in user to be stored in the session.
 *
 * @author	James Lee
 * @version	20070501
 */
public class UserData {
	private int uid;
	private String username;

	public int getUid() {
		return uid;
	}

	public void setUid(int uid) {
		this.uid = uid;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}
}
