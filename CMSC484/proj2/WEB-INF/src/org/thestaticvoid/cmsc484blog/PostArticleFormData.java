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

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}
}
