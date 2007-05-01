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

	public UserData(int uid, String username) {
		this.uid = uid;
		this.username = username;
	}
	
	public int getUid() {
		return uid;
	}

	public String getUsername() {
		return username;
	}
}
