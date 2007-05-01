package org.thestaticvoid.cmsc484blog;

/**
 * Used by servlets to pass an error string to a JSP page
 *
 * @author	James Lee
 * @version	20070501
 */
public class Error {
	private String error;

	public Error(String error) {
		this.error = error;
	}

	public String getError() {
		return error;
	}
}
