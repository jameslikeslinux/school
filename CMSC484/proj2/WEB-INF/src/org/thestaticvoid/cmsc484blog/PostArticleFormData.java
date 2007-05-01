package org.thestaticvoid.cmsc484blog;

/**
 * Used to pass data back to a form when an error occured.
 *
 * @author	James Lee
 * @version	20070501
 */
public class PostArticleFormData {
	private String title;
	private String content;

	public PostArticleFormData(String title, String content) {
		this.title = title;
		this.content = content;
	}

	public String getTitle() {
		return title;
	}

	public String getContent() {
		return content;
	}
}
