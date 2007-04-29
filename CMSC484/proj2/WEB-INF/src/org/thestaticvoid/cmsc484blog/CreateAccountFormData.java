package org.thestaticvoid.cmsc484blog;

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
