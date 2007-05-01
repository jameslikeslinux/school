package org.thestaticvoid.cmsc484blog;

/**
 * Used to pass data back to a form when an error occured.
 *
 * @author	James Lee
 * @version	20070501
 */
public class CreateAccountFormData {
	private String username, name;

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}
}
