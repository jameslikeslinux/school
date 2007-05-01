package org.thestaticvoid.cmsc484blog;

/**
 * Used to pass data back to a form when an error occured.
 *
 * @author	James Lee
 * @version	20070501
 */
public class CreateAccountFormData {
	private String username, name;

	public CreateAccountFormData(String username, String name) {
		this.username = username;
		this.name = name;
	}

	public String getUsername() {
		return username;
	}

	public String getName() {
		return name;
	}
}
