package org.thestaticvoid.cmsc484blog;

/**
 * Used to pass data back to a form when an error occured.
 *
 * @author	James Lee
 * @version	20070501
 */
public class LoginFormData {
	private String username;

	public LoginFormData(String username) {
		this.username = username;
	}

	public String getUsername() {
		return username;
	}
}
