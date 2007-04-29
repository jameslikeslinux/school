package org.thestaticvoid.csmc484blog;

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
