package org.thestaticvoid.cmsc484blog;

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
